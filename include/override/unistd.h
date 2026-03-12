/*
 * override/unistd.h — POSIX extensions for Solaris 7
 *
 * Solaris 7's <unistd.h> is missing execvpe() and a handful of other
 * functions added in later POSIX revisions.  The missing functions are
 * declared in solcompat/process.h and implemented in process.c.
 *
 * Part of libsolcompat — https://github.com/firefly128/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_UNISTD_H
#define _SOLCOMPAT_OVERRIDE_UNISTD_H

/* Pull in the real Solaris 7 /usr/include/unistd.h */
#include_next <unistd.h>

/* Add execvpe() and related process helpers */
#include <solcompat/process.h>

#endif /* _SOLCOMPAT_OVERRIDE_UNISTD_H */
