#pragma once
#include <vector>

void conv2d(const std::vector<std::vector<float>>& in,
            const std::vector<std::vector<float>>& k,
            std::vector<std::vector<float>>& out);
