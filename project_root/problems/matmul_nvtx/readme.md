# 📘 matmul_nvtx — NVTX Instrumented Matrix Multiplication

## 🧭 Overview

**matmul_nvtx** 는 CPU 기반 행렬 곱셈 연산에 **NVTX(NVIDIA Tools Extension)** 범위를 삽입하여 **Nsight Systems** 상에서 연산 구간을 시각적으로 추적할 수 있도록 만든 실험용 모듈입니다.

이 코드는 “AI 추론 최적화” 또는 “Edge 디바이스용 연산 엔진”의 성능 병목을 **정확히 어디서 발생하는지 계측하는 최소 단위 예제**로 설계되었습니다.

---

## ⚙️ Build & Run

### 🏗️ Requirements

* **CMake ≥ 3.24**
* **MSVC 2022 (v143)** or **GCC / Clang**
* **CUDA Toolkit ≥ 12.6** (NVTX 헤더만 사용)
* **Ninja or NMake Makefiles**

### 🧩 Build (Ninja + Release)

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DUSE_NVTX=ON \
  -DNVTX_INCLUDE_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include" ..
cmake --build .
```

> NVTX 헤더가 없으면 자동으로 더미 매크로로 대체되어 빌드 통과.

### ▶️ Run

```bash
./matmul_nvtx [M] [K] [N] [mode] [block_size]
```

| 인자           | 기본값     | 설명                        |
| ------------ | ------- | ------------------------- |
| `M,K,N`      | 1024    | 행렬 차원                     |
| `mode`       | blocked | `baseline` / `blocked` 선택 |
| `block_size` | 64      | 타일 크기 (blocked 모드만 사용)    |

예시:

```bash
./matmul_nvtx 1024 1024 1024 blocked 64
```

---

## 🔍 NVTX Instrumentation

### 📊 시각화 도구

```bash
nsys profile --trace=cpu,os ./matmul_nvtx 1024 1024 1024
```

→ Nsight Systems GUI 에서 다음 NVTX Range들이 색상 블록으로 표시됩니다.

```
main
 ├─ alloc
 ├─ init
 ├─ run-baseline(ref)
 ├─ run-blocked
 ├─ verify
 └─ done
```

### 🧩 코드 삽입 위치

| 구간      | NVTX Range            | 설명             |
| ------- | --------------------- | -------------- |
| 프로그램 전체 | `"main"`              | 상위 스코프         |
| 메모리 할당  | `"alloc"`             | 벡터 A, B, C 초기화 |
| 초기화     | `"init"`              | 난수 생성 / 초기화    |
| 기준 실행   | `"run-baseline(ref)"` | 기준 알고리즘 수행     |
| 테스트 실행  | `"run-blocked"`       | 타일링 버전 수행      |
| 검증      | `"verify"`            | RMSE 계산        |
| 종료      | `"done"`              | 타임라인 마킹 포인트    |

---

## 🧠 Core Structure

### 1️⃣ NVTX Scope 매크로

```cpp
#define NVTX_SCOPE(name) ::NvtxScope NVTX_CONCAT(_nvtx_scope_, __COUNTER__){name}
```

* RAII 방식으로 `push/pop` 자동 관리
* `__COUNTER__` 로 중복 선언 방지
* 없는 환경에서도 `USE_NVTX` 매크로로 더미 처리

### 2️⃣ Blocked Matrix Multiply

```cpp
for (int i0 = 0; i0 < M; i0 += BS) {
  int iMax = (std::min)(i0 + BS, M);
  ...
  for (int j = j0; j < jMax; ++j)
    cRow[j - j0] += aik * bRow[j - j0];
}
```

* 캐시 친화적인 **타일 단위 연산**
* 대형 행렬에서도 CPU 캐시 히트율 향상
* NVTX로 각 구간 타이밍을 명확히 비교 가능

### 3️⃣ RMSE 검증

```cpp
rmse = sqrt(err2 / (double)std::max<size_t>(1, C.size()));
```

→ 블록 버전과 기준 버전의 수치 차이를 검증 (정상 작동시 RMSE ≈ 0)

---

## 📈 Example Output

```
[conf] M=1024 K=1024 N=1024 mode=blocked, BS=64
[verify] RMSE=2.74857e-06 rel=1.2e-07
[time] baseline(ref) = 11842.3 ms
[time] test(blocked) = 6510.5 ms
[speedup] 1.82x
```

---

## 🧩 Key Learnings

| 항목         | 내용                                               |
| ---------- | ------------------------------------------------ |
| NVTX 적용    | 단일 CPU 코드에서도 구간별 시간 시각화 가능                       |
| NVTX 범위 관리 | RAII + `__COUNTER__` 매크로로 안정적 구현                 |
| 성능 최적화     | 블록 단위 타일링으로 캐시 효율 향상                             |
| 문법 주의      | Windows `min/max` 매크로 충돌 시 `#define NOMINMAX` 필수 |
| 확장 가능성     | OpenMP / AVX / CUDA 버전으로 확장하여 비교 분석 가능           |

---

## 🚀 Next Step

* 🔹 `OpenMP` 멀티스레드 버전 추가 (`#pragma omp parallel for collapse(2)`)
* 🔹 `AVX/FMA` 벡터화 커널 실험
* 🔹 `CUDA kernel` 버전으로 확장, GPU vs CPU 비교
* 🔹 `nsys profile` 로그 자동 분석 스크립트 작성
