#include <stdlib.h>
#include <math.h>
#include "Complex.h"

inline float UniformRandom()
{
  float rand_value = (float)rand() / (float)RAND_MAX;
  return rand_value;
}

Complex NormalComplexRandom()
{
  float x1, x2, w;
  do {
    x1 = 2.0f * UniformRandom() - 1.0f;
    x2 = 2.0f * UniformRandom() - 1.0f;
    w = x1 * x1 + x2 * x2;
  } while (w >= 1.0f);
  w = sqrt((-2.0f * log(w)) / w);
  float rand1 = x1 * w;
  float rand2 = x2 * w;
  return Complex(rand1, rand2);
}