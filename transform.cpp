#include "transform.h"

float tsin(float a)
{
    return sinf(a);
}

double tsin(double a)
{
    return sin(a);
}

float tcos(float a)
{
    return cosf(a);
}

double tcos(double a)
{
    return cos(a);
}

template <typename T>
T tpow(T v)
{
    return v*v;
}

template <typename T>
Matrix<4, 4, T> scale(T sx, T sy, T sz)
{
    Matrix<4, 4, T> m;
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
    m[3][3] = 1;

    return m;
}

template <typename T>
Matrix<4, 4, T> translate(T tx, T ty, T tz)
{
    Matrix<4, 4, T> m;
    m[3][0] = tx;
    m[3][1] = ty;
    m[3][2] = tz;

    return m;
}

template <typename T>
Matrix<3, 3, T> skew(const vec<3, T> &v)
{
    Matrix<3, 3, T> m;

    m[0][0] = 0;      m[1][0] = -v.z(); m[2][0] = v.y();
    m[0][1] = v.z();  m[1][1] = 0;      m[2][1] = -v.x();
    m[0][2] = -v.y(); m[1][2] = v.x();  m[2][2] = 0;

    return m;
}

template <typename T>
Matrix<4, 4, T> rotate(T a, T x, T y, T z)
{
    vec<3, T> u = vec3(x, y, z);    // axis
    u.normalize();

    Matrix<3, 3, T> P = outer(u, u);
    Matrix<3, 3, T> Q = skew(u);
    Matrix<3, 3, T> I;

    Matrix<3, 3, T> R = P+(I-P)*tcos(a)+Q*sin(a);

    return Matrix<4, 4, T>(R);
}

// Provide implementation
template Matrix4f scale(float, float, float);
template Matrix4f translate(float, float, float);
template Matrix4f rotate(float, float, float, float);
