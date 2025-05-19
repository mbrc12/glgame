#include "common.hpp"

std::ostream &operator<<(std::ostream &os, const std::vector<float> &v) {
    os << "[";
    for (auto &i : v) {
        os << i << " ";
    }
    os << "]";
    return os;
}
                         
