/*
 * override/atomic.h — Wrapper for Solaris atomic operations
 *
 * Solaris 10+ provides <atomic.h> which simply includes <sys/atomic.h>.
 * Solaris 7/8/9 only have <sys/atomic.h> directly.  This override
 * provides the expected <atomic.h> include path so that software
 * written for Solaris 10+ (e.g. OpenSSL 3.x) can find it.
 *
 * Part of libsolcompat — https://github.com/firefly128/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_ATOMIC_H
#define _SOLCOMPAT_OVERRIDE_ATOMIC_H

#include <sys/atomic.h>

#endif /* _SOLCOMPAT_OVERRIDE_ATOMIC_H */
