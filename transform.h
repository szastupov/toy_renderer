#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"

typedef Matrix<4, 4, float> Matrix4f;
typedef Matrix<4, 4, double> Matrix4d;

template <typename T>
Matrix<4, 4, T> scale(T sx, T sy, T sz);

template <typename T>
Matrix<4, 4, T> translate(T tx, T ty, T tz);

template <typename T>
Matrix<4, 4, T> rotate(T a, T x, T y, T z);

#endif
