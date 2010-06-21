#ifndef MATRIX_H
#define MATRIX_H

#define matrix_for_each(j, i)                   \
    for (int j = 0; j < NJ; j++)                \
        for (int i = 0; i < NI; i++)

template <int NI = 4, int NJ = 4, typename T = float>
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

    T operator () (int j, int i) const
    {
        return p[j][i];
    }

    T& operator () (int j, int i)
    {
        return p[j][i];
    }

    void print()
    {
        for (int i = 0; i < NI; i++)
        {
            for (int j = 0; j < NJ; j++)
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
        for (int j = 0; j < NJ; j++)
            p[j][j] = 1;
    }
};

template <int LR, int LC, int RC, typename T>
Matrix<LR, RC, T> operator * (const Matrix<LR, LC, T> &ml,
                              const Matrix<LC, RC, T> &mr)
{
    Matrix<LR, RC, T> n;

    for (int j = 0; j < RC; j++)
        for (int i = 0; i < LR; i++)
            for (int s = 0; s < LR; s++)
                n(j, i) += ml(s, i)*mr(j, s);

    return n;
}

template <int LR, int LC, int RC, typename T>
void operator *= (Matrix<LR, LC, T> &ml,
                  const Matrix<LC, RC, T> &mr)
{
    ml = ml*mr;
}

template <typename T>
class Transform : public Matrix<4, 4, T>
{
    typedef Matrix<4, 4, T> m_t;
public:
    Transform()
    {
        m_t::load_identity();
    }

    Transform& scale(T sx, T sy, T sz)
    {
        m_t m;
        m(0, 0) = sx;
        m(1, 1) = sy;
        m(2, 2) = sz;
        m(3, 3) = 1;

        *this *= m;
        return *this;
    }

    Transform& translate(T tx, T ty, T tz)
    {
        m_t m;
        m.load_identity();
        m(3, 0) = tx;
        m(3, 1) = ty;
        m(3, 2) = tz;

        *this *= m;
        return *this;
    }

    Transform& rotate(T a, T x, T y, T z)
    {
        return *this;
    }
};

void matrix_test()
{
    Matrix<4, 6> a;

    int cnt = 0;
    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 3; i++)
            a(j, i) = cnt++;
        a(j, 3) = 1;
    }

    Transform<float> b;
    b.scale(1, 2, 3).translate(10, 10, 0);

    Matrix<4, 6> c = b*a;

    c.print();
}

#endif
