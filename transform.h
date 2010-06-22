#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"

template <typename T>
class Transform : public Matrix<4, 4, T>
{
    typedef Matrix<4, 4, T> m_t;
public:
    Transform()
    {
        m_t::load_identity();
    }

    void transform(const m_t &m)
    {
        *(m_t*)this = m*(*(m_t*)this);
    }

    Transform& scale(T sx, T sy, T sz)
    {
        m_t m;
        m(0, 0) = sx;
        m(1, 1) = sy;
        m(2, 2) = sz;
        m(3, 3) = 1;

        transform(m);
        return *this;
    }

    Transform& translate(T tx, T ty, T tz)
    {
        m_t m;
        m.load_identity();
        m(3, 0) = tx;
        m(3, 1) = ty;
        m(3, 2) = tz;

        transform(m);
        return *this;
    }

    // Fix it
    Transform& rotate(T a, T x, T y, T z)
    {
        m_t m;
        m.load_identity();

        m(0, 0) = cos(a);
        m(0, 1) = sin(a);
        m(1, 0) = -sin(a);
        m(1, 1) = cos(a);

        transform(m);
        return *this;
    }
};

#endif
