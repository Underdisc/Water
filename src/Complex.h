//////////////////////////////////////////////////////////////////////////////
/// @file Complex.h
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-09-21
///
/// @brief Interface and implementation for complex number arithmetic.
///////////////////////////////////////////////////////////////////////////////
#ifndef COMPLEX_H
#define COMPLEX_H

#include <FFTW\fftw3.h>

class Complex
{
public:
  Complex();
  Complex(float real, float imaginary);
  Complex(const Complex & other);
  float Real();
  float Imaginary();
  Complex Conjugate();
  Complex operator+(const Complex & rhs);
  Complex operator-(const Complex & rhs);
  Complex operator*(const Complex & rhs) const;
  Complex operator*(float rhs) const;
  Complex & operator+=(const Complex & rhs);
  Complex & operator-=(const Complex & rhs);
  Complex & operator*=(const Complex & rhs);
  Complex & operator*=(float rhs);
private:
  union
  {
    struct
    {
      float m_Real, m_Imaginary;
    };
    float m_Values[2];
    fftwf_complex m_FFTWComplex;
  };
};

#endif // !COMPLEX_H