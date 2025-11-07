#pragma once
#include <cstdint>
#include <vector>

struct QParams {
    float scale;      // float -> int8 스케일
    int   zero_point; // 여기서는 대칭 quant라 0 고정
};

// 대칭 MinMax per-tensor quant (zero-point = 0)
QParams compute_qparams_symmetric(const std::vector<float>& x);

// float -> int8 (대칭, clamp to [-127,127])
void quantize_int8(const std::vector<float>& x,
                   std::vector<int8_t>& q,
                   const QParams& qp);

// int8 -> float (복구)
void dequantize_int8(const std::vector<int8_t>& q,
                     std::vector<float>& x,
                     const QParams& qp);
