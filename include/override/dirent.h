/*
 * override/dirent.h — POSIX directory extensions for Solaris 7
 *
 * Solaris 7's <dirent.h> is missing dirfd() and fdopendir().
 * Both are declared in solcompat/filesystem.h and implemented in
 * filesystem.c.
 *
 * Part of libsolcompat — https://github.com/firefly128/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_DIRENT_H
#define _SOLCOMPAT_OVERRIDE_DIRENT_H

/* Pull in the real Solaris 7 /usr/include/dirent.h */
#include_next <dirent.h>

/* Add dirfd() and fdopendir() */
#include <solcompat/filesystem.h>

#endif /* _SOLCOMPAT_OVERRIDE_DIRENT_H */
