// #define PI 3.14159265358979323846
// #define ONE_RAD (3.14159 / 180.0);

float deg2rad(float num) { return num * (3.14159265358979323846 / 180.0); }

float sqr(float num) { return num * num; }

int clampi(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}