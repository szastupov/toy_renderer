/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <redchrom@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Stepan Zastupov
 * ----------------------------------------------------------------------------
 */

#include <cstdlib>
#include <cmath>
#include <cassert>

#define vec_for_each(v) for (size_t v = 0; v < N; v++)

template <size_t N = 2, typename T = float>
class vec
{
	T p[N];
public:
    vec()
    {
        vec_for_each(i) p[i] = 0;
    }

	vec(T x, T y)
	{
		assert(N >= 2);
		p[0] = x;
		p[1] = y;
	}

	vec(T x, T y, T z)
	{
		assert(N >= 3);
		p[0] = x;
		p[1] = y;
		p[2] = z;
	}

    vec(T x, T y, T z, T w)
	{
		assert(N >= 3);
		p[0] = x;
		p[1] = y;
		p[2] = z;
        p[3] = w;
	}

	vec(const vec& src)
	{
        vec_for_each(i) p[i] = src[i];
	}

	T x() const { return p[0]; }
	T y() const { return p[1]; }
	T z() const { return p[2]; }
    T w() const { return p[3]; }

    T operator [](size_t i) const
    {
        return p[i];
    }

    T& operator [](size_t i)
    {
        return p[i];
    }

	T length() const
	{
		T sum = 0;
        vec_for_each(i) sum += p[i]*p[i];
		return sqrt(sum);
	}

	vec normalized() const
	{
		return *this / length();
	}

	vec& normalize()
	{
		*this /= length();
		return *this;
	}

	vec operator - () const
	{
		vec n(*this);
		vec_for_each(i) n[i] = -n[i];
		return n;
	}

#define VEC_OP(op)								\
	vec operator op (const vec &b) const		\
	{											\
		vec n(*this);							\
		n op##= b;								\
		return n;								\
	}											\
	void operator op##= (const vec &b)          \
	{											\
		vec_for_each(i) p[i] op##= b[i];        \
	}

#define SCALAR_OP(op)							\
	vec operator op (T s) const					\
	{											\
		vec n(*this);							\
		n op##= s;								\
		return n;								\
	}											\
	void operator op##= (T s)					\
	{											\
		vec_for_each(i) p[i] op##= s;           \
	}

	VEC_OP(+)
	VEC_OP(-)
	SCALAR_OP(*)
	SCALAR_OP(/)

#define CMP(op)									\
	bool operator op (const vec &b)				\
	{											\
        vec_for_each(i) {                       \
            if (!(p[i] op b[i]))                \
                return false;                   \
        }                                       \
		return true;							\
	}

	CMP(==)
	CMP(!=)
	CMP(>)
	CMP(>=)
	CMP(<)
	CMP(<=)
};

#undef CMP
#undef SCALAR_OP
#undef VEC_OP

template <size_t N, typename T>
static T dot(const vec<N, T> &a, const vec<N, T> &b)
{
	T sum = 0;
	vec_for_each(i) sum += a[i]*b[i];
	return sum;
}

template <size_t N, typename T>
static T distance(const vec<N, T> &a, const vec<N, T> &b)
{
	vec<N, T> c = a-b;
	return c.length();
}

template <typename T>
static T det(const vec<2, T> &a, const vec<2, T> &b)
{
	return a.x()*b.y()-b.x()*a.y();
}

#undef vec_for_each


typedef vec<2, float> vec2f;
typedef vec<3, float> vec3f;
typedef vec<4, float> vec4f;

#ifdef VEC_TEST
static void vec_test()
{
#define ASSERT(e) printf("%-40s = %s\n", #e, (e) ? "ok" : "fail");
    vec2f a(1, 2);
    vec2f b(2, 4);
    vec2f c(1, 2);
    vec2f d(-1, -2);

    ASSERT(a != b);
    ASSERT(a == c);
    ASSERT(a+c == b);
    ASSERT(d == -a);
    ASSERT(a*2 == b);
    ASSERT(b/2 == a);
    ASSERT(a[0] == 1 && a[1] == 2);
    ASSERT(a[0] == a.x() && a[1] == a.y());
    ASSERT(b.length() != 0);
    ASSERT(b.normalized() < vec2f(1, 1));
    ASSERT(dot(a, b) == 10);

#undef ASSERT
}
#endif
