#include "conv.h"

void conv2d(const std::vector<std::vector<float>>& in,
            const std::vector<std::vector<float>>& k,
            std::vector<std::vector<float>>& out) {
    int H  = static_cast<int>(in.size());
    int W  = static_cast<int>(in[0].size());
    int KH = static_cast<int>(k.size());
    int KW = static_cast<int>(k[0].size());

    for (int i = 0; i <= H - KH; ++i) {
        for (int j = 0; j <= W - KW; ++j) {
            float s = 0.f;
            for (int ki = 0; ki < KH; ++ki)
                for (int kj = 0; kj < KW; ++kj)
                    s += in[i + ki][j + kj] * k[ki][kj];
            out[i][j] = s;
        }
    }
}
