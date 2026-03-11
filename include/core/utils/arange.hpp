#pragma once

#include <vector>

namespace utils {
inline std::vector<double> arange(double start, double stop, double step) {
    std::vector<double> result;

    for (double v = start; v <= stop; v += step)
        result.push_back(v);

    return result;
}
} // namespace utils