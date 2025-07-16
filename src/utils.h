#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <random>

using namespace std;

// inline random_device rd;
// inline mt19937 gen(rd());
// inline uniform_int_distribution<uint32_t> gen_rand_uint32_t(0, UINT32_MAX);
// inline uniform_real_distribution<float> gen_rand_float(0, numeric_limits<float>::max());

namespace utils {
extern double deltaTime;
extern bool running;

template <typename T> T clamp(const T &n, const T &lower, const T &upper) { return std::max(lower, std::min(n, upper)); }

// uint32_t rand_uint32_t();
// float rand_float();
int64_t GetMicroTime();
void Sleep(int duration);
int CalculateAverageFps(int executionTime);
} // namespace utils

#endif