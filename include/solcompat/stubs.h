/*
 * solcompat/stubs.h — Stubbed locale and threading helpers
 *
 * pthread_setname_np, newlocale/uselocale/freelocale
 */
#ifndef SOLCOMPAT_STUBS_H
#define SOLCOMPAT_STUBS_H

#include <pthread.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- pthread_setname_np --- */
#ifndef HAVE_PTHREAD_SETNAME_NP
int pthread_setname_np(pthread_t thread, const char *name);
#endif

/* --- Per-thread locale (POSIX 2008) --- */
#ifndef HAVE_USELOCALE
typedef void *locale_t;

#define LC_GLOBAL_LOCALE ((locale_t)-1)

#define LC_COLLATE_MASK  (1 << LC_COLLATE)
#define LC_CTYPE_MASK    (1 << LC_CTYPE)
#define LC_MESSAGES_MASK (1 << LC_MESSAGES)
#define LC_MONETARY_MASK (1 << LC_MONETARY)
#define LC_NUMERIC_MASK  (1 << LC_NUMERIC)
#define LC_TIME_MASK     (1 << LC_TIME)
#define LC_ALL_MASK      (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MESSAGES_MASK | \
                          LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK)

locale_t newlocale(int category_mask, const char *locale, locale_t base);
locale_t uselocale(locale_t newloc);
void     freelocale(locale_t locobj);
locale_t duplocale(locale_t locobj);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_STUBS_H */
