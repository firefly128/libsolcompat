/*
 * math.c — Missing C99 math functions for Solaris 7
 *
 * round, trunc, log2, exp2, fdim, fmin, fmax,
 * fpclassify helpers
 */

#include <math.h>
#include <float.h>
#include <ieeefp.h>   /* Solaris: finite(), isnand(), fpclass() */

double
round(double x)
{
    if (x >= 0.0)
        return floor(x + 0.5);
    else
        return ceil(x - 0.5);
}

float
roundf(float x)
{
    return (float)round((double)x);
}

double
trunc(double x)
{
    if (x >= 0.0)
        return floor(x);
    else
        return ceil(x);
}

float
truncf(float x)
{
    return (float)trunc((double)x);
}

double
log2(double x)
{
    /* log2(x) = log(x) / log(2) */
    return log(x) / 0.693147180559945309417;  /* ln(2) */
}

float
log2f(float x)
{
    return (float)log2((double)x);
}

double
exp2(double x)
{
    return pow(2.0, x);
}

float
exp2f(float x)
{
    return (float)exp2((double)x);
}

double
fdim(double x, double y)
{
    if (x > y)
        return x - y;
    return 0.0;
}

double
fmin(double x, double y)
{
    if (isnand(x)) return y;
    if (isnand(y)) return x;
    return (x < y) ? x : y;
}

double
fmax(double x, double y)
{
    if (isnand(x)) return y;
    if (isnand(y)) return x;
    return (x > y) ? x : y;
}

float
fminf(float x, float y)
{
    return (float)fmin((double)x, (double)y);
}

float
fmaxf(float x, float y)
{
    return (float)fmax((double)x, (double)y);
}

/*
 * fpclassify helpers
 * Solaris 7 has fpclass() in <ieeefp.h> returning fp_class_type
 */
int
solcompat_fpclassify_d(double x)
{
    fpclass_t c = fpclass(x);
    switch (c) {
    case FP_SNAN:
    case FP_QNAN:
        return 0; /* FP_NAN */
    case FP_NINF:
    case FP_PINF:
        return 1; /* FP_INFINITE */
    case FP_NZERO:
    case FP_PZERO:
        return 2; /* FP_ZERO */
    case FP_NDENORM:
    case FP_PDENORM:
        return 3; /* FP_SUBNORMAL */
    case FP_NNORM:
    case FP_PNORM:
    default:
        return 4; /* FP_NORMAL */
    }
}

int
solcompat_fpclassify_f(float x)
{
    return solcompat_fpclassify_d((double)x);
}
