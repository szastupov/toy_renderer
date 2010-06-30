#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"
#include <cstdio>

#define matrix_for_each(j)                      \
    for (size_t j = 0; j < NJ; j++)

template <size_t NI = 4, size_t NJ = 4, typename T = float>
class Matrix
{
    typedef vec<NI, T> vec_t;
    vec_t p[NI];
public:
    Matrix()
    {
        loadIdentity();
    }

    Matrix(const Matrix &m)
    {
        *this = m;
    }

    template <size_t SI, size_t SJ>
    explicit
    Matrix(const Matrix<SI, SJ, T> &s)
    {
        assert(NI >= SI);
        assert(NJ >= SJ);

        loadIdentity();
        for (size_t j = 0; j < SJ; j++)
            for (size_t i = 0; i < SI; i++)
                p[j][i] = s[j][i];
    }

    Matrix(const vec_t src[NI])
    {
        matrix_for_each(j)
            p[j] = src[j];
    }

    vec_t& operator [] (size_t j)
    {
        return p[j];
    }

    const vec_t& operator [] (size_t j) const
    {
        return p[j];
    }

    void print()
    {
        for (size_t i = 0; i < NI; i++) {
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
        matrix_for_each(j)                      \
        p[j] op##= m[j];                        \
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
        matrix_for_each(j)                      \
        p[j] op##= s;                           \
    }

    MATRIX_OP(+)
    MATRIX_OP(-)
    SCALAR_OP(*)
    SCALAR_OP(/)

    void loadIdentity()
    {
        assert(NJ == NI);
        matrix_for_each(j) {
            p[j].assign(0);
            p[j][j] = 1;
        }
    }
};

template <size_t LR, size_t LC, size_t RC, typename T>
Matrix<LR, RC, T> operator * (const Matrix<LR, LC, T> &ml,
                              const Matrix<LC, RC, T> &mr)
{
    Matrix<LR, RC, T> n;

    for (size_t j = 0; j < RC; j++)
        for (size_t i = 0; i < LR; i++) {
            n[j][i] = 0;
            for (size_t s = 0; s < LR; s++)
                n[j][i] += ml[s][i]*mr[j][s];
        }

    return n;
}

template <size_t LR, size_t LC, typename T>
vec<LC, T> operator * (const Matrix<LR, LC, T> &ml,
                       const vec<LC, T> &vr)
{
    vec<LC, T> n;

    for (size_t i = 0; i < LR; i++)
        for (size_t s = 0; s < LR; s++)
            n[i] += ml[s][i]*vr[s];

    return n;
}

template <size_t N, typename T>
Matrix<N, N, T> outer(const vec<N, T> &u,
                      const vec<N, T> &v)
{
    Matrix<N, N, T> n;

    for (size_t i = 0; i < N; i++)
        n[i] = u*v[i];

    return n;
}

#endif
