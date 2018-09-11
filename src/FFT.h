// THIS IS ONLY A TEMPORARY FFT IMPLEMENTATION
// THIS WILL BE REPLACED BY KISS OR A CUSTOM IMPLEMENTATION


#ifndef FFT_H
#define FFT_H

#include <math.h>
#include "Complex.h"

class FFT {
private:
  unsigned int N, which;
  unsigned int log_2_N;
  float pi2;
  unsigned int *reversed;
  Complex **W;
  Complex *c[2];
protected:
public:
  FFT(unsigned int N);
  ~FFT();
  unsigned int reverse(unsigned int i);
  Complex w(unsigned int x, unsigned int N);
  void fft(Complex* input, Complex* output, int stride, int offset);
};

#endif