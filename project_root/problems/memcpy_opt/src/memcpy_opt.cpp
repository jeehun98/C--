#include "memcpy_opt.h"

// 단순 32B 청크 카피 (예시). 실제 정렬 보장은 별도 관리 필요.
void memcpy_aligned(float* dst, const float* src, std::size_t n) {
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        dst[i+0] = src[i+0]; dst[i+1] = src[i+1];
        dst[i+2] = src[i+2]; dst[i+3] = src[i+3];
        dst[i+4] = src[i+4]; dst[i+5] = src[i+5];
        dst[i+6] = src[i+6]; dst[i+7] = src[i+7];
    }
    for (; i < n; ++i) dst[i] = src[i];
}
