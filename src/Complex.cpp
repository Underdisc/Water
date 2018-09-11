
#include "Complex.h"

Complex::Complex() : m_Real(0), m_Imaginary(0)
{}

Complex::Complex(float real, float imaginary) :
  m_Real(real), m_Imaginary(imaginary)
{}

Complex::Complex(const Complex & other) :
  m_Real(other.m_Real), m_Imaginary(other.m_Imaginary)
{}

float Complex::Real()
{
  return m_Real;
}


float Complex::Imaginary()
{
  return m_Imaginary;
}

Complex Complex::Conjugate()
{
  return Complex(m_Real, -m_Imaginary);
}

Complex Complex::operator+(const Complex & rhs)
{
  float new_real = m_Real + rhs.m_Real;
  float new_im = m_Imaginary + rhs.m_Imaginary;
  return Complex(new_real, new_im);
}

Complex Complex::operator-(const Complex & rhs)
{
  float new_real = m_Real - rhs.m_Real;
  float new_im = m_Imaginary - rhs.m_Imaginary;
  return Complex(new_real, new_im);
}

Complex Complex::operator*(const Complex & rhs) const
{
  float new_real = m_Real * rhs.m_Real - m_Imaginary * rhs.m_Imaginary;
  float new_im = m_Real * rhs.m_Imaginary + m_Imaginary * rhs.m_Real;
  return Complex(new_real, new_im);
}

Complex Complex::operator*(float rhs) const
{
  float new_real = m_Real * rhs;
  float new_im = m_Imaginary * rhs;
  return Complex(new_real, new_im);
}

Complex & Complex::operator+=(const Complex & rhs)
{
  m_Real += rhs.m_Real;
  m_Imaginary += rhs.m_Imaginary;
  return *this;
}

Complex & Complex::operator-=(const Complex & rhs)
{
  m_Real -= rhs.m_Real;
  m_Imaginary -= rhs.m_Imaginary;
  return *this;
}

Complex & Complex::operator*=(const Complex & rhs)
{
  float new_real = m_Real * rhs.m_Real - m_Imaginary * rhs.m_Imaginary;
  m_Imaginary = m_Real * rhs.m_Imaginary + m_Imaginary * rhs.m_Real;
  m_Real = new_real;
  return *this;
}

Complex & Complex::operator*=(float rhs)
{
  m_Real *= rhs;
  m_Imaginary *= rhs;
  return *this;
}