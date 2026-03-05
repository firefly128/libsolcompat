/*
 * c99_types.c -- C99 integer type support functions
 *
 * imaxabs, imaxdiv for inttypes.h.
 * PRIxxx and SCNxxx format macros are in the override header only.
 */

#include "solcompat/c99_types.h"
#include <stdlib.h>  /* strtol */
#include <errno.h>

/* imaxdiv_t -- define here since solcompat/c99_types.h may not have it */
#ifndef _SOLCOMPAT_IMAXDIV_DEFINED
#define _SOLCOMPAT_IMAXDIV_DEFINED
typedef struct {
    long long quot;
    long long rem;
} imaxdiv_t;
#endif

/*
 * imaxabs -- absolute value of intmax_t (C99 7.8.2.1)
 */
long long
imaxabs(long long j)
{
    return (j < 0) ? -j : j;
}

/*
 * imaxdiv -- quotient and remainder of intmax_t division (C99 7.8.2.2)
 */
imaxdiv_t
imaxdiv(long long numer, long long denom)
{
    imaxdiv_t result;
    result.quot = numer / denom;
    result.rem  = numer % denom;
    return result;
}
