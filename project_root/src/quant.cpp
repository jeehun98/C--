#include "quant.h"
#include <algorithm>
#include <cmath>

static inline int8_t clamp_i8(int v) {
    if (v > 127)  return 127;
    if (v < -127) return -127; // -128 대신 -127 사용(대칭)
    return static_cast<int8_t>(v);
}

QParams compute_qparams_symmetric(const std::vector<float>& x) {
    if (x.empty()) return {1.0f, 0};
    auto [mn_it, mx_it] = std::minmax_element(x.begin(), x.end());
    float amax = std::max(std::fabs(*mn_it), std::fabs(*mx_it));
    float scale = (amax > 0.f) ? (127.f / amax) : 1.0f; // s = 127/amax
    return {scale, 0};
}

void quantize_int8(const std::vector<float>& x,
                   std::vector<int8_t>& q,
                   const QParams& qp) {
    q.resize(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        float v  = x[i] * qp.scale;           // s * x
        int   iv = static_cast<int>(std::nearbyint(v));
        q[i] = clamp_i8(iv);
    }
}

void dequantize_int8(const std::vector<int8_t>& q,
                     std::vector<float>& x,
                     const QParams& qp) {
    x.resize(q.size());
    const float invs = (qp.scale != 0.f) ? (1.0f / qp.scale) : 1.0f;
    for (size_t i = 0; i < q.size(); ++i) {
        x[i] = static_cast<float>(q[i]) * invs; // x ≈ q / s
    }
}
