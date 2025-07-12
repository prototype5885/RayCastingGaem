#include "stdint.h"
#include <sys/time.h>
#include <unistd.h>

long GetMicroTime() {
  struct timeval tv;
  gettimeofday(&tv, ((void *)0));

  return (long)(tv.tv_sec * 1000000L + tv.tv_usec);
}

void Wait(unsigned int duration) {
  sleep(duration);
  // this_thread::sleep_for(chrono::microseconds(duration));
}

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