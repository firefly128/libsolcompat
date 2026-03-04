/*
 * solcompat/snprintf.h — C99-conformant snprintf/vsnprintf
 *
 * Solaris 7's snprintf/vsnprintf return -1 on truncation instead of
 * the C99-required count of characters that would have been written.
 * This breaks the universal pattern: n = snprintf(NULL, 0, fmt, ...)
 *
 * These replacements provide correct C99 return-value semantics.
 */
#ifndef SOLCOMPAT_SNPRINTF_H
#define SOLCOMPAT_SNPRINTF_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int solcompat_snprintf(char *str, size_t size, const char *fmt, ...);
int solcompat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

/*
 * Override libc versions.  Link with -lsolcompat BEFORE -lc, or use
 * -include solcompat/solcompat.h to get these macros automatically.
 */
#define snprintf  solcompat_snprintf
#define vsnprintf solcompat_vsnprintf

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_SNPRINTF_H */
