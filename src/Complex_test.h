#pragma once

#include <iostream>
#include "Complex.h"

void test_complex();
void test_add();
void test_sub();
void test_mul();

void test_complex()
{
  test_add();
  test_sub();
  test_mul();
}

void test_add()
{
  Complex a( 4.0f,  2.0f);
  Complex b( 3.0f, -2.0f);
  Complex c( 0.0f,  5.0f);
  Complex d(-3.0f,  0.0f);

  Complex res = a + b;
  c += res;
  res += d + c;
  // res: 11 + 5i
  std::cout << res.Real() << " + i * " << res.Imaginary() << std::endl;
}

void test_sub()
{
  Complex a(4.0f, 2.0f);
  Complex b(3.0f, -2.0f);
  Complex c(0.0f, 5.0f);
  Complex d(-3.0f, 0.0f);

  Complex res = a - b;
  c -= res;
  res -= d - c;
  // res: 3 + 5i
  std::cout << res.Real() << " + i * " << res.Imaginary() << std::endl;
}

void test_mul()
{
  Complex a(4.0f, 2.0f);
  Complex b(3.0f, -2.0f);
  Complex c(0.0f, 5.0f);
  Complex d(-3.0f, 0.0f);

  Complex res = a * b;
  res *= c * d;
  // res: -30 - 240i
  std::cout << res.Real() << " + i * " << res.Imaginary() << std::endl;
}