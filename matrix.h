#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"

#define matrix_for_each(j, i)                   \
    for (size_t j = 0; j < NJ; j++)                \
        for (size_t i = 0; i < NI; i++)

template <size_t NI = 4, size_t NJ = 4, typename T = float>
class Matrix
{
    T p[NJ][NI];
public:
    Matrix()
    {
        matrix_for_each(j, i)
            p[j][i] = 0;
    }

    Matrix(const Matrix &m)
    {
        *this = m;
    }

    void operator = (const Matrix &m)
    {
        matrix_for_each(j, i)
            p[j][i] = m(j, i);
    }

    T operator () (size_t j, size_t i) const
    {
        return p[j][i];
    }

    T& operator () (size_t j, size_t i)
    {
        return p[j][i];
    }

    void print()
    {
        for (size_t i = 0; i < NI; i++)
        {
            for (size_t j = 0; j < NJ; j++)
                printf("%f ", p[j][i]);
            printf("\n");
        }
    }


#define MATRIX_OP(op)                           \
    Matrix operator op (const Matrix &m) const  \
    {                                           \
        Matrix n(*this);                        \
        n op##= m;                              \
        return n;                               \
    }                                           \
    void operator op##= (const Matrix &m)       \
    {                                           \
        matrix_for_each(j, i)                   \
        p[j][i] op##= m(j, i);                  \
    }

#define SCALAR_OP(op)                           \
    Matrix operator op (T s) const              \
    {                                           \
        Matrix m(*this);                        \
        m op##= s;                              \
        return m;                               \
    }                                           \
    void operator op##= (T s)                   \
    {                                           \
        matrix_for_each(j, i)                   \
        p[j][i] op##= s;                        \
    }

    MATRIX_OP(+)
    MATRIX_OP(-)
    SCALAR_OP(*)
    SCALAR_OP(/)

    void load_identity()
    {
        assert(NJ == NI);
        for (size_t j = 0; j < NJ; j++)
            p[j][j] = 1;
    }
};

template <size_t LR, size_t LC, size_t RC, typename T>
Matrix<LR, RC, T> operator * (const Matrix<LR, LC, T> &ml,
                              const Matrix<LC, RC, T> &mr)
{
    Matrix<LR, RC, T> n;

    for (size_t j = 0; j < RC; j++)
        for (size_t i = 0; i < LR; i++)
            for (size_t s = 0; s < LR; s++)
                n(j, i) += ml(s, i)*mr(j, s);

    return n;
}

template <size_t LR, size_t LC, typename T>
vec<LC, T> operator * (const Matrix<LR, LC, T> &ml,
                       const vec<LC, T> &vr)
{
    vec<LC, T> n;

    for (size_t i = 0; i < LR; i++)
        for (size_t s = 0; s < LR; s++)
            n[i] += ml(s, i)*vr[s];

    return n;
}

#endif
