#include "utils.h"

#include <thread>

using namespace std;

namespace utils {
double deltaTime = 1.0;
bool running = true;

// float rand_float() {
//   return gen_rand_float(gen);
// }

// uint32_t rand_uint32_t() { return gen_rand_uint32_t(gen); }

int64_t GetMicroTime() {
  auto const now = chrono::steady_clock::now();

  auto const duration = now.time_since_epoch();
  return chrono::duration_cast<chrono::microseconds>(duration).count();
}

void Sleep(const int duration) { this_thread::sleep_for(chrono::microseconds(duration)); }

int CalculateAverageFps(const int executionTime) {
  constexpr int FPS_HISTORY_SIZE = 8;

  static int fpsHistory[8];

  for (int i = FPS_HISTORY_SIZE; i >= 0; i--) {
    const int nexti = i + 1;
    if (nexti <= FPS_HISTORY_SIZE - 1) {
      fpsHistory[nexti] = fpsHistory[i];
    }
  }
  fpsHistory[0] = 1000000 / executionTime;

  int32_t sumFps = 0;
  for (int i = 0; i < FPS_HISTORY_SIZE; i++) {
    sumFps += fpsHistory[i];
  }
  const int avgFps = sumFps / FPS_HISTORY_SIZE;
  return avgFps;
}
} // namespace utils
