#define PI 3.14159265358979323846
#define ONE_RAD (3.14159 / 180.0);

float deg2rad(float num) { return num * (PI / 180.0f); }

float sqr(float num) { return num * num; }

#define MINDIFF 2.25e-308
float sqroot(float square) {
  float root = square / 3, last, diff = 1;
  if (square <= 0)
    return 0;
  do {
    last = root;
    root = (root + square / root) / 2;
    diff = root - last;
  } while (diff > MINDIFF || diff < -MINDIFF);
  return root;
}

float sinus(float x) {
  float t = x;
  float sine = t;
  for (int a = 1; a < 10; ++a) {
    float mult = -x * x / ((2 * a + 1) * (2 * a));
    t *= mult;
    sine += t;
  }
  return sine;
}

int roundToInt(float num) {
  int integer_part = (int)num;
  float decimal_part = num - integer_part;
  return integer_part + (decimal_part >= 0.5);
}

int clampi(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}