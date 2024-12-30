#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <vector>
#include <stdexcept>
#include <limits>

// Function declarations
bool nearlyEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon());
template<typename T>
T logBase(T x, T base);
template<typename T>
T csch(T x);
template<typename T>
T coth(T x);
template<typename T>
T sech(T x);
template<typename T>
T arcsin(T x);
template<typename T>
T arccos(T x);
template<typename T>
T arctan(T x);
template<typename T>
std::vector<T> bezierQuadratic(T t, const std::vector<T>& P0, const std::vector<T>& P1, const std::vector<T>& P2);
template<typename T>
T sec(T x);
template<typename T>
T csc(T x);
template<typename T>
T cot(T x);
template<typename T>
T arcsinh(T x);
template<typename T>
T arccosh(T x);
template<typename T>
T arctanh(T x);
float clamp(float value, float min, float max);
float lerpF(float a, float b, float f);

#endif // MATH_H