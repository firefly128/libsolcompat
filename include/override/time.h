/*
 * override/time.h — Wrap system <time.h> with CLOCK_MONOTONIC support
 *
 * Solaris 7's <time.h> lacks CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID,
 * and other POSIX clock IDs that modern software expects.  This override
 * includes the system header, then pulls in solcompat/clock.h which
 * defines the missing constants and provides replacement clock_gettime()
 * that implements CLOCK_MONOTONIC via gethrtime().
 *
 * Part of libsolcompat — https://github.com/Sunstorm-Project/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_TIME_H
#define _SOLCOMPAT_OVERRIDE_TIME_H

/* Include the real system <time.h> */
#include_next <time.h>

/* Pull in CLOCK_MONOTONIC and related definitions */
#include <solcompat/clock.h>

#endif /* _SOLCOMPAT_OVERRIDE_TIME_H */
