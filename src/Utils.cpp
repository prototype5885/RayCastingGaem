#include <cstdint>
#include <thread>

long GetMicroTime() {
  auto const now = std::chrono::steady_clock::now();

  auto const duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

void Sleep(int duration) { std::this_thread::sleep_for(std::chrono::microseconds(duration)); }

int CalculateAverageFps(int executionTime) {
  const int FPS_HISTORY_SIZE = 8;

  static int fpsHistory[8];

  for (int i = FPS_HISTORY_SIZE; i >= 0; i--) {
    int nexti = i + 1;
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