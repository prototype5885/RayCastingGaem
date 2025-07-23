#include "utils.h"

#include <algorithm>
#include <numeric>
#include <thread>
#include <vector>

namespace utils {
double deltaTime = 1.0;
bool running = true;

// float rand_float() {
//   return gen_rand_float(gen);
// }

// uint32_t rand_uint32_t() { return gen_rand_uint32_t(gen); }

int64_t GetMicroTime() {
  auto const now = std::chrono::steady_clock::now();

  auto const duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

void Sleep(const int duration) { std::this_thread::sleep_for(std::chrono::microseconds(duration)); }

int CalculateAverageFps(const int executionTime) {
  using namespace std;
  static vector<int> fpsHistory(8);
  copy_backward(fpsHistory.begin(), fpsHistory.end() - 1, fpsHistory.end());
  fpsHistory.front() = 1'000'000 / clamp(executionTime, 1, INT_MAX);
  const int sumFps = accumulate(fpsHistory.begin(), fpsHistory.end(), 0);
  return sumFps / static_cast<int>(fpsHistory.size());
}
} // namespace utils
