#include <iostream>
#include <vector>
#include <numeric>
#include "utils.h"
#include "quant.h"

int main() {
    std::cout << "C++ Test Environment ✅\n";
    std::cout << "sum(3, 5) = " << add(3, 5) << "\n\n";

    // 실험 데이터
    std::vector<float> x = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 2.0f, -3.0f};

    // 1) qparams 계산
    QParams qp = compute_qparams_symmetric(x);
    std::cout << "[qparams] scale=" << qp.scale << " zero_point=" << qp.zero_point << "\n";

    // 2) quantize
    std::vector<int8_t> q;
    quantize_int8(x, q, qp);
    std::cout << "[q] ";
    for (auto v : q) std::cout << int(v) << " ";
    std::cout << "\n";

    // 3) dequantize
    std::vector<float> xr;
    dequantize_int8(q, xr, qp);
    std::cout << "[x_recon] ";
    for (auto v : xr) std::cout << v << " ";
    std::cout << "\n";

    // 간단 오차 지표 (L2)
    float l2 = 0.f;
    for (size_t i = 0; i < x.size(); ++i) {
        float d = x[i] - xr[i];
        l2 += d*d;
    }
    std::cout << "[recon_L2] " << l2 << "\n";

    return 0;
}
