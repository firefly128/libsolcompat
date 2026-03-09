/*
 * override/endian.h — Byte-order macros for Solaris
 *
 * glibc provides <endian.h> with __BYTE_ORDER, __BIG_ENDIAN, etc.
 * Solaris has no equivalent header; endianness information lives in
 * <sys/isa_defs.h> (which defines _BIG_ENDIAN or _LITTLE_ENDIAN).
 *
 * This override provides the glibc-style macros so portable software
 * (e.g. NetSurf's libnsfb) builds without modification.
 *
 * Part of libsolcompat — https://github.com/firefly128/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_ENDIAN_H
#define _SOLCOMPAT_OVERRIDE_ENDIAN_H

/* Standard byte-order constants (matching glibc values) */
#define __BIG_ENDIAN    4321
#define __LITTLE_ENDIAN 1234
#define __PDP_ENDIAN    3412

/* Aliases without underscores (BSD/POSIX style) */
#define BIG_ENDIAN      __BIG_ENDIAN
#define LITTLE_ENDIAN   __LITTLE_ENDIAN
#define PDP_ENDIAN      __PDP_ENDIAN

/* Determine byte order from Solaris or GCC built-in macros */
#if defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
    /* GCC 4.6+ provides these built-ins */
    #define __BYTE_ORDER    __BYTE_ORDER__
    #define BYTE_ORDER      __BYTE_ORDER__
#elif defined(_BIG_ENDIAN) || defined(__sparc) || defined(__sparc__)
    #define __BYTE_ORDER    __BIG_ENDIAN
    #define BYTE_ORDER      __BIG_ENDIAN
#elif defined(_LITTLE_ENDIAN) || defined(__i386) || defined(__amd64)
    #define __BYTE_ORDER    __LITTLE_ENDIAN
    #define BYTE_ORDER      __LITTLE_ENDIAN
#else
    #error "Cannot determine byte order for this platform"
#endif

#endif /* _SOLCOMPAT_OVERRIDE_ENDIAN_H */
