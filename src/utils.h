#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

// inline random_device rd;
// inline mt19937 gen(rd());
// inline uniform_int_distribution<uint32_t> gen_rand_uint32_t(0, UINT32_MAX);
// inline uniform_real_distribution<float> gen_rand_float(0, numeric_limits<float>::max());

namespace utils {
extern double deltaTime;
extern bool running;

// uint32_t rand_uint32_t();
// float rand_float();
int64_t GetMicroTime();
void Sleep(int duration);
int CalculateAverageFps(int executionTime);
} // namespace utils

#endif