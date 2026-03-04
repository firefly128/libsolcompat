/*
 * solcompat/math_ext.h — Missing C99 math functions and macros
 */
#ifndef SOLCOMPAT_MATH_EXT_H
#define SOLCOMPAT_MATH_EXT_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INFINITY
#define INFINITY (__builtin_inff())
#endif

#ifndef NAN
#define NAN (__builtin_nanf(""))
#endif

#ifndef HUGE_VALF
#define HUGE_VALF INFINITY
#endif

#ifndef HAVE_ROUND
double round(double x);
#endif
#ifndef HAVE_ROUNDF
float roundf(float x);
#endif

#ifndef HAVE_TRUNC
double trunc(double x);
#endif
#ifndef HAVE_TRUNCF
float truncf(float x);
#endif

#ifndef HAVE_LOG2
double log2(double x);
#endif
#ifndef HAVE_LOG2F
float log2f(float x);
#endif

#ifndef HAVE_EXP2
double exp2(double x);
#endif
#ifndef HAVE_EXP2F
float exp2f(float x);
#endif

#ifndef HAVE_FDIM
double fdim(double x, double y);
#endif

#ifndef HAVE_FMIN
double fmin(double x, double y);
#endif

#ifndef HAVE_FMAX
double fmax(double x, double y);
#endif

#ifndef HAVE_FMINF
float fminf(float x, float y);
#endif

#ifndef HAVE_FMAXF
float fmaxf(float x, float y);
#endif

/* fpclassify — Solaris 7 has isnan/isinf but not fpclassify */
#ifndef FP_NAN
#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4
#endif

#ifndef fpclassify
int solcompat_fpclassify_d(double x);
int solcompat_fpclassify_f(float x);
#define fpclassify(x) \
    (sizeof(x) == sizeof(float) ? solcompat_fpclassify_f(x) : solcompat_fpclassify_d(x))
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_MATH_EXT_H */
