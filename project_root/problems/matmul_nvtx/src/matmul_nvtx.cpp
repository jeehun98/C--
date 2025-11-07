/*
 matmul_nvtx.cpp — NVTX 범위가 포함된 행렬연산 데모 (CPU)

 ▶ 목적
   - C++ 단일 파일로 행렬 곱(Y = A * B)과 간단한 검증/프로파일링을 수행
   - NVTX로 주요 구간을 표시하여 Nsight Systems에서 타임라인 시각화
   - NVTX가 없는 환경을 위해 더미 매크로 제공

 ▶ 빌드 (Windows / MSVC)
   - NVTX 포함(권장):
     cl /O2 /std:c++17 matmul_nvtx.cpp /Fe:matmul_nvtx.exe ^
       /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.6\include" ^
       /DUSE_NVTX
   - NVTX 미포함(더미 매크로 사용):
     cl /O2 /std:c++17 matmul_nvtx.cpp /Fe:matmul_nvtx.exe

 ▶ 빌드 (Linux / GCC/Clang)
   - NVTX 포함(권장):
     g++ -O2 -std=c++17 matmul_nvtx.cpp -o matmul_nvtx \
         -I /usr/local/cuda/include -DUSE_NVTX
   - NVTX 미포함(더미 매크로 사용):
     g++ -O2 -std=c++17 matmul_nvtx.cpp -o matmul_nvtx

 ▶ Nsight Systems로 프로파일
   - nsys profile --trace=cpu,os ./matmul_nvtx 2048 2048 2048
   - GUI에서 NVTX Range("init", "matmul-baseline", "run-blocked", "verify") 확인
*/

// Windows의 min/max 매크로 충돌 방지 (std::min/std::max 사용 보장)
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// ===== NVTX helpers =====
#ifdef USE_NVTX
  #include <nvtx3/nvToolsExt.h>

  // 두 단계 결합 매크로 (__LINE__ 확장 순서 이슈 회피)
  #define NVTX_CONCAT_IMPL(x,y) x##y
  #define NVTX_CONCAT(x,y) NVTX_CONCAT_IMPL(x,y)

  struct NvtxScope {
    explicit NvtxScope(const char* name) { nvtxRangePushA(name); }
    ~NvtxScope() { nvtxRangePop(); }
  };

  // 같은 줄에서 중복 선언될 가능성까지 차단하기 위해 __COUNTER__ 우선 사용
  #ifdef __COUNTER__
    #define NVTX_SCOPE(name) ::NvtxScope NVTX_CONCAT(_nvtx_scope_, __COUNTER__){name}
  #else
    #define NVTX_SCOPE(name) ::NvtxScope NVTX_CONCAT(_nvtx_scope_, __LINE__){name}
  #endif

  #define NVTX_MARK(name)  nvtxMarkA(name)
#else
  // NVTX가 없을 때도 동일한 소스가 컴파일되도록 더미 정의
  #define NVTX_CONCAT_IMPL(x,y) x##y
  #define NVTX_CONCAT(x,y) NVTX_CONCAT_IMPL(x,y)
  struct NvtxScope { explicit NvtxScope(const char*){} ~NvtxScope(){} };
  #define NVTX_SCOPE(name) ::NvtxScope NVTX_CONCAT(_nvtx_scope_, __LINE__){name}
  #define NVTX_MARK(name)  ((void)0)
#endif

// Row-major: A[MxK], B[KxN], C[MxN]
static void matmul_baseline(const float* A, const float* B, float* C,
                            int M, int K, int N)
{
    NVTX_SCOPE("matmul-baseline");
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            float acc = 0.0f;
            // inner product of row i of A and column j of B
            for (int k = 0; k < K; ++k) {
                acc += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = acc;
        }
    }
}

// Cache-friendly blocked (tiled) matmul: optional faster version
static void matmul_blocked(const float* A, const float* B, float* C,
                           int M, int K, int N, int BS)
{
    NVTX_SCOPE("matmul-blocked");
    std::fill(C, C + (size_t)M * N, 0.0f);

    for (int i0 = 0; i0 < M; i0 += BS) {
        int iMax = (std::min)(i0 + BS, M);
        for (int k0 = 0; k0 < K; k0 += BS) {
            int kMax = (std::min)(k0 + BS, K);
            for (int j0 = 0; j0 < N; j0 += BS) {
                int jMax = (std::min)(j0 + BS, N);
                // micro-kernel: (i0..iMax) x (k0..kMax) x (j0..jMax)
                for (int i = i0; i < iMax; ++i) {
                    for (int k = k0; k < kMax; ++k) {
                        float aik = A[i * K + k];
                        const float* __restrict bRow = &B[k * N + j0];
                        float* __restrict cRow = &C[i * N + j0];
                        for (int j = j0; j < jMax; ++j) {
                            cRow[j - j0] += aik * bRow[j - j0];
                        }
                    }
                }
            }
        }
    }
}

static double wall_ms_since(const std::chrono::high_resolution_clock::time_point& t0)
{
    using clock = std::chrono::high_resolution_clock;
    auto t1 = clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main(int argc, char** argv)
{
    // Parse dimensions (default 1024x1024 * 1024)
    int M = 1024, K = 1024, N = 1024;
    bool use_blocked = true; // toggle to compare
    int BS = 64;             // block size for blocked matmul

    if (argc >= 4) {
        M = std::stoi(argv[1]);
        K = std::stoi(argv[2]);
        N = std::stoi(argv[3]);
    }
    if (argc >= 5) {
        std::string mode = argv[4];
        if (mode == "baseline") use_blocked = false;
        if (mode == "blocked")  use_blocked = true;
    }
    if (argc >= 6) {
        BS = std::stoi(argv[5]);
    }

    std::cout << "[conf] M=" << M << " K=" << K << " N=" << N
              << " mode=" << (use_blocked ? "blocked" : "baseline")
              << (use_blocked ? (", BS=" + std::to_string(BS)) : "")
              << "\n";

    NVTX_SCOPE("main");

    // Allocate
    NVTX_SCOPE("alloc");
    std::vector<float> A((size_t)M * K), B((size_t)K * N), C((size_t)M * N), Cref((size_t)M * N);

    // Initialize deterministic random values
    {
        NVTX_SCOPE("init");
        std::mt19937 rng(123);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (auto& x : A) x = dist(rng);
        for (auto& x : B) x = dist(rng);
        std::fill(C.begin(), C.end(), 0.0f);
        std::fill(Cref.begin(), Cref.end(), 0.0f);
    }

    // Baseline (for reference / correctness)
    double t_ms_ref = 0.0;
    {
        NVTX_SCOPE("run-baseline(ref)");
        auto t0 = std::chrono::high_resolution_clock::now();
        matmul_baseline(A.data(), B.data(), Cref.data(), M, K, N);
        t_ms_ref = wall_ms_since(t0);
    }

    // Tested kernel
    double t_ms = 0.0;
    {
        NVTX_SCOPE(use_blocked ? "run-blocked" : "run-baseline");
        auto t0 = std::chrono::high_resolution_clock::now();
        if (use_blocked) {
            matmul_blocked(A.data(), B.data(), C.data(), M, K, N, BS);
        } else {
            matmul_baseline(A.data(), B.data(), C.data(), M, K, N);
        }
        t_ms = wall_ms_since(t0);
    }

    // Verify (RMSE)
    double rmse = 0.0;
    {
        NVTX_SCOPE("verify");
        double err2 = 0.0, ref2 = 0.0;
        for (size_t i = 0; i < C.size(); ++i) {
            double d = (double)C[i] - (double)Cref[i];
            err2 += d * d;
            ref2 += (double)Cref[i] * (double)Cref[i];
        }
        rmse = std::sqrt(err2 / (double)std::max<size_t>(1, C.size()));
        std::cout << "[verify] RMSE=" << rmse
                  << " rel=" << (std::sqrt(err2) / (std::sqrt(ref2) + 1e-12)) << "\n";
    }

    std::cout << "[time] baseline(ref) = " << t_ms_ref << " ms\n";
    std::cout << "[time] test(" << (use_blocked ? "blocked" : "baseline") << ") = " << t_ms << " ms\n";
    if (t_ms > 0) {
        std::cout << "[speedup] " << (t_ms_ref / t_ms) << "x\n";
    }

    // Single point NVTX marker for quick jump in timeline
    NVTX_MARK("done");
    return 0;
}
