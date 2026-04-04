/*
 * override/ifaddrs.h — getifaddrs/freeifaddrs for Solaris 7
 *
 * Solaris 7 does not ship <ifaddrs.h> at all (added in Solaris 11).
 * Modern software (curl, Python, etc.) includes this header directly.
 * This provides the struct and function declarations via solcompat/network.h,
 * which has the full getifaddrs/freeifaddrs/struct ifaddrs implementation.
 *
 * Note: no #include_next since the real file doesn't exist.
 *
 * Part of libsolcompat — https://github.com/Sunstorm-Project/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_IFADDRS_H
#define _SOLCOMPAT_OVERRIDE_IFADDRS_H

#include <solcompat/network.h>

#endif /* _SOLCOMPAT_OVERRIDE_IFADDRS_H */
