#include <iostream>
#include <vector>
#include "common/utils.h"
#include "quant.h"

int main() {
    std::cout << "[quant] demo\n";
    std::cout << "add(3,5)=" << add(3,5) << "\n";

    std::vector<float> x = {-1.0f,-0.5f,0.0f,0.5f,1.0f,2.0f,-3.0f};
    QParams qp = compute_qparams_symmetric(x);

    std::vector<int8_t> q;
    quantize_int8(x, q, qp);

    std::cout << "scale=" << qp.scale << " zp=" << qp.zero_point << "\n";
    std::vector<int> qi(q.begin(), q.end());
    std::cout << "q: " << joinInts(qi, ", ") << "\n";

    std::vector<float> xr;
    dequantize_int8(q, xr, qp);
    std::cout << "x_recon:";
    for (auto v : xr) std::cout << " " << v;
    std::cout << "\n";
    return 0;
}
