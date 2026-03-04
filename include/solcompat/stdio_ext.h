/*
 * solcompat/stdio_ext.h — Missing stdio functions
 *
 * vasprintf, asprintf, dprintf, getline, getdelim
 */
#ifndef SOLCOMPAT_STDIO_EXT_H
#define SOLCOMPAT_STDIO_EXT_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_VASPRINTF
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **strp, const char *fmt, ...);
#endif

#ifndef HAVE_DPRINTF
int dprintf(int fd, const char *fmt, ...);
#endif

#ifndef HAVE_GETLINE
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

#ifndef HAVE_GETDELIM
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_STDIO_EXT_H */
