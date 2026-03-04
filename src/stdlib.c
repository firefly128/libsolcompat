/*
 * stdlib.c — Missing stdlib functions for Solaris 7
 *
 * setenv, unsetenv, mkdtemp
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int solcompat_snprintf(char *, size_t, const char *, ...);

int
setenv(const char *name, const char *value, int overwrite)
{
    char *envstr;
    size_t nlen, vlen;

    if (!name || !name[0] || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    if (!overwrite && getenv(name))
        return 0;

    nlen = strlen(name);
    vlen = strlen(value);
    envstr = (char *)malloc(nlen + vlen + 2);
    if (!envstr) {
        errno = ENOMEM;
        return -1;
    }

    memcpy(envstr, name, nlen);
    envstr[nlen] = '=';
    memcpy(envstr + nlen + 1, value, vlen + 1);

    if (putenv(envstr) != 0) {
        free(envstr);
        return -1;
    }
    /* Note: envstr is now owned by the environment — do NOT free it */
    return 0;
}

int
unsetenv(const char *name)
{
    extern char **environ;
    char **ep, **dp;
    size_t nlen;

    if (!name || !name[0] || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    nlen = strlen(name);
    for (ep = dp = environ; *ep; ep++) {
        if (strncmp(*ep, name, nlen) == 0 && (*ep)[nlen] == '=')
            continue;   /* skip this entry */
        *dp++ = *ep;
    }
    *dp = NULL;
    return 0;
}

char *
mkdtemp(char *tmpl)
{
    size_t len;
    int i;
    char *suffix;

    if (!tmpl) {
        errno = EINVAL;
        return NULL;
    }

    len = strlen(tmpl);
    if (len < 6) {
        errno = EINVAL;
        return NULL;
    }

    suffix = tmpl + len - 6;
    if (strcmp(suffix, "XXXXXX") != 0) {
        errno = EINVAL;
        return NULL;
    }

    for (i = 0; i < 1000; i++) {
        /* Use mktemp to generate a unique name, then try mkdir */
        char saved[7];
        memcpy(saved, suffix, 6);
        saved[6] = '\0';

        /* Reset the template */
        memcpy(suffix, "XXXXXX", 6);

        if (mktemp(tmpl) == NULL || tmpl[0] == '\0') {
            /* mktemp failed — restore and try again */
            memcpy(suffix, "XXXXXX", 6);
            continue;
        }

        if (mkdir(tmpl, 0700) == 0)
            return tmpl;

        if (errno != EEXIST) {
            /* Real error */
            return NULL;
        }

        /* EEXIST — try again with fresh XXXXXX */
        memcpy(suffix, "XXXXXX", 6);
    }

    errno = EEXIST;
    return NULL;
}
