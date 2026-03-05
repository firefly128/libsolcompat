/*
 * c99_types.c — C99 integer type support functions
 *
 * imaxabs/imaxdiv for <inttypes.h>, strtoimax/strtoumax.
 * PRI*/SCN* format macros are in the header only.
 */

#include "solcompat/c99_types.h"
#include <stdlib.h>  /* strtol */
#include <errno.h>

/*
 * imaxabs — absolute value of intmax_t (C99 7.8.2.1)
 */
long long
imaxabs(long long j)
{
    return (j < 0) ? -j : j;
}

/*
 * imaxdiv — quotient and remainder of intmax_t division (C99 7.8.2.2)
 */
imaxdiv_t
imaxdiv(long long numer, long long denom)
{
    imaxdiv_t result;
    result.quot = numer / denom;
    result.rem  = numer % denom;
    return result;
}
