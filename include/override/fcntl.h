/*
 * libsolcompat -- override <fcntl.h>
 *
 * Includes the real Solaris 7 <fcntl.h> then adds POSIX constants.
 */
#ifndef _SOLCOMPAT_OVERRIDE_FCNTL_H
#define _SOLCOMPAT_OVERRIDE_FCNTL_H

#include_next <fcntl.h>

#include <solcompat/filesystem.h>   /* POSIX_FADV_*, posix_fadvise */

/* O_CLOEXEC doesn't exist on Solaris 7 -- define a placeholder
 * value so bit-test code compiles; pipe2/dup3/mkostemp implement
 * the semantics via fcntl(F_SETFD, FD_CLOEXEC). */
#ifndef O_CLOEXEC
#define O_CLOEXEC 0x800000
#endif

#endif /* _SOLCOMPAT_OVERRIDE_FCNTL_H */
