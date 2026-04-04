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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef isblank
extern int isblank(int c);
#endif

/* POSIX.1-2008 locale-aware character classification */
typedef void *locale_t;  /* forward — real definition in stubs.h */
extern int isalnum_l(int, locale_t);
extern int isalpha_l(int, locale_t);
extern int isblank_l(int, locale_t);
extern int iscntrl_l(int, locale_t);
extern int isdigit_l(int, locale_t);
extern int isgraph_l(int, locale_t);
extern int islower_l(int, locale_t);
extern int isprint_l(int, locale_t);
extern int ispunct_l(int, locale_t);
extern int isspace_l(int, locale_t);
extern int isupper_l(int, locale_t);
extern int isxdigit_l(int, locale_t);
extern int tolower_l(int, locale_t);
extern int toupper_l(int, locale_t);

#ifdef __cplusplus
}
#endif

#endif /* _SOLCOMPAT_OVERRIDE_CTYPE_H */
