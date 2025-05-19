#pragma once
#include <iostream>
#include <vector>

#define DBG(x) std::cerr << __FILE__ << ":" << __LINE__ << " " << x << std::endl;

std::ostream &operator<<(std::ostream &os, const std::vector<float> &v);
