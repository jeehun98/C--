#include <iostream>
#include <chrono>
#include <vector>
#include "memcpy_opt.h"

int main() {
    const std::size_t N = 1 << 20;
    std::vector<float> a(N), b(N);
    for (std::size_t i = 0; i < N; ++i) a[i] = float(i);

    auto t0 = std::chrono::high_resolution_clock::now();
    memcpy_aligned(b.data(), a.data(), N);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "[memcpy_opt] time(ms): " << ms << "\n";
    std::cout << "b[0]=" << b[0] << ", b[N-1]=" << b[N-1] << "\n";
    return 0;
}
