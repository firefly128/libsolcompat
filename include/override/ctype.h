/*
 * libsolcompat -- override <ctype.h>
 *
 * Solaris 7's ctype.h lacks the C99 isblank() function.
 * This wrapper includes the real header then declares the function
 * (implemented in libsolcompat).
 *
 * Part of libsolcompat -- https://github.com/firefly128/libsolcompat
 */
#ifndef _SOLCOMPAT_OVERRIDE_CTYPE_H
#define _SOLCOMPAT_OVERRIDE_CTYPE_H

#include_next <ctype.h>

#ifndef isblank
#ifdef __cplusplus
extern "C" {
#endif
extern int isblank(int c);
#ifdef __cplusplus
}
#endif
#endif /* isblank */

#endif /* _SOLCOMPAT_OVERRIDE_CTYPE_H */
