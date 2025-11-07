#include "common/utils.h"
#include <sstream>

int add(int a, int b) { return a + b; }

std::string joinInts(const std::vector<int>& v, const std::string& sep) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << sep;
        oss << v[i];
    }
    return oss.str();
}
