#include <iostream>
#include <vector>
#include "conv.h"

int main() {
    std::vector<std::vector<float>> X = {
        {1,2,3},
        {4,5,6},
        {7,8,9}
    };
    std::vector<std::vector<float>> K = {
        {1,0},
        {0,-1}
    };
    std::vector<std::vector<float>> Y(2, std::vector<float>(2, 0.f));
    conv2d(X, K, Y);

    std::cout << "[conv2d] out:\n";
    for (auto& r : Y) {
        for (auto v : r) std::cout << v << " ";
        std::cout << "\n";
    }
    return 0;
}
