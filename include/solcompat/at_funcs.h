/*
 * solcompat/at_funcs.h — *at() directory-relative functions
 *
 * Emulated via mutex-protected fchdir dance on Solaris 7.
 * These are safe for single-threaded code and provide
 * serialized access for multi-threaded code.
 */
#ifndef SOLCOMPAT_AT_FUNCS_H
#define SOLCOMPAT_AT_FUNCS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AT_FDCWD
#define AT_FDCWD             -100
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0x100
#endif
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR         0x200
#endif
#ifndef AT_EACCESS
#define AT_EACCESS           0x200
#endif
#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW    0x400
#endif

#ifndef HAVE_OPENAT
int openat(int dirfd, const char *pathname, int flags, ...);
#endif

#ifndef HAVE_MKDIRAT
int mkdirat(int dirfd, const char *pathname, mode_t mode);
#endif

#ifndef HAVE_RENAMEAT
int renameat(int olddirfd, const char *oldpath,
             int newdirfd, const char *newpath);
#endif

#ifndef HAVE_UNLINKAT
int unlinkat(int dirfd, const char *pathname, int flags);
#endif

#ifndef HAVE_FSTATAT
int fstatat(int dirfd, const char *pathname,
            struct stat *buf, int flags);
#endif

#ifndef HAVE_FCHOWNAT
int fchownat(int dirfd, const char *pathname,
             uid_t owner, gid_t group, int flags);
#endif

#ifndef HAVE_FCHMODAT
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
#endif

#ifndef HAVE_READLINKAT
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
#endif

#ifndef HAVE_SYMLINKAT
int symlinkat(const char *target, int newdirfd, const char *linkpath);
#endif

#ifndef HAVE_LINKAT
int linkat(int olddirfd, const char *oldpath,
           int newdirfd, const char *newpath, int flags);
#endif

#ifndef HAVE_FACCESSAT
int faccessat(int dirfd, const char *pathname, int mode, int flags);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_AT_FUNCS_H */
