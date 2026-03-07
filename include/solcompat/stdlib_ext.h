/*
 * solcompat/stdlib_ext.h — Missing stdlib functions
 *
 * setenv, unsetenv, mkdtemp
 */
#ifndef SOLCOMPAT_STDLIB_EXT_H
#define SOLCOMPAT_STDLIB_EXT_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_SETENV
int setenv(const char *name, const char *value, int overwrite);
#endif

#ifndef HAVE_UNSETENV
int unsetenv(const char *name);
#endif

#ifndef HAVE_MKDTEMP
char *mkdtemp(char *tmpl);
#endif

/* C99 _Exit — immediate termination without atexit handlers */
#ifndef HAVE__EXIT_C99
void _Exit(int status);
#endif

/* C99/POSIX integer conversion */
#ifndef HAVE_STRTOIMAX
#include <sys/types.h>
long long strtoimax(const char *nptr, char **endptr, int base);
#endif

#ifndef HAVE_STRTOUMAX
unsigned long long strtoumax(const char *nptr, char **endptr, int base);
#endif

/*
 * Solaris 7 stdlib.h hides strtoll/strtoull/atoll behind __STDC__==0,
 * but GCC always defines __STDC__ as 1.  The functions exist in libc;
 * they just need declarations.
 */
long long strtoll(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *nptr, char **endptr, int base);
long long atoll(const char *nptr);

/* C99 strtof — not in Solaris 7 libc at all; implemented in stdlib.c */
float strtof(const char *nptr, char **endptr);

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_STDLIB_EXT_H */
