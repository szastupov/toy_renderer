#ifndef VEC_H
#define VEC_H

#include <cstdlib>
#include <cmath>
#include <cassert>

#define vec_for_each(v) for (size_t v = 0; v < N; v++)

template <size_t N = 2, typename T = float>
class vec
{
	T p[N];
public:
    vec(T v = 0)
    {
        assign(v);
    }

    void assign(T v)
    {
        vec_for_each(i) p[i] = 0;
    }

    vec(const vec& src)
	{
        vec_for_each(i) p[i] = src[i];
	}

    template <size_t SN, typename ST>
    vec(const vec<SN, ST>& src)
    {
        int n = SN < N ? SN : N;
        for (int i = 0; i < n; i++)
            p[i] = (T)src[i];
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
	bool operator op (const vec &b) const       \
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

template <typename T>
static vec<3, T> cross(const vec<3, T> &a, const vec<3, T> &b)
{
    return vec3(a[1]*b[2]-a[2]*b[1],
                a[2]*b[0]-a[0]*b[2],
                a[0]*b[1]-a[1]*b[0]);
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

typedef vec<2, float> vec2f;
typedef vec<3, float> vec3f;
typedef vec<4, float> vec4f;
typedef vec<2, int> vec2i;

template <typename T>
vec<2, T> vec2(T a, T b)
{
    vec<2, T> r;
    r[0] = a;
    r[1] = b;
    return r;
}

template <typename T>
vec<3, T> vec3(T a, T b, T c)
{
    vec<3, T> r;
    r[0] = a;
    r[1] = b;
    r[2] = c;
    return r;
}

template <typename T>
vec<4, T> vec4(T a, T b, T c, T d)
{
    vec<4, T> r;
    r[0] = a;
    r[1] = b;
    r[2] = c;
    r[3] = d;
    return r;
}

#ifdef VEC_TEST
static void vec_test()
{
#define ASSERT(e) printf("%-40s = %s\n", #e, (e) ? "ok" : "fail");
    vec2f a = vec2(1, 2);
    vec2f b = vec2(2, 4);
    vec2f c = vec2(1, 2);
    vec2f d = vec2(-1, -2);
    vec3f n1 = vec3(1.f, 0.f, 0.f);
    vec3f n2 = vec3(0.f, 1.f, 0.f);
    vec3f n3 = vec3(0.f, 0.f, 1.f);

    ASSERT(a != b);
    ASSERT(a == c);
    ASSERT(a+c == b);
    ASSERT(d == -a);
    ASSERT(a*2 == b);
    ASSERT(b/2 == a);
    ASSERT(a[0] == 1 && a[1] == 2);
    ASSERT(a[0] == a.x() && a[1] == a.y());
    ASSERT(b.length() != 0);
    ASSERT(b.normalized() < vec2(1.f, 1.f));
    ASSERT(dot(a, b) == 10);
    ASSERT(cross(n1, n2) == n3);

#undef ASSERT
}
#endif

#ifdef ENABLE_IOSTREAM
#include <iostream>
template <size_t N, typename T>
std::ostream &operator<<(std::ostream &out, const vec<N, T> &v)
{
    out << "(";
    vec_for_each(i) {
        out << v[i];
        if (i+1 != N)
            out << ", ";
    }
    out << ")";

    return out;
}
#endif

#undef vec_for_each

#endif
