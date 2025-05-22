#pragma once
#include <iostream>
#include <vector>
#define PI 3.14159265358979323846f
#define DBG(x) std::cerr << __FILE__ << ":" << __LINE__ << " " << x << std::endl;

std::ostream &operator<<(std::ostream &os, const std::vector<float> &v);
