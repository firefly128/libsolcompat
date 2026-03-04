/*
 * solcompat/pty.h — PTY functions
 *
 * posix_openpt, openpty, forkpty, login_tty, cfmakeraw
 */
#ifndef SOLCOMPAT_PTY_H
#define SOLCOMPAT_PTY_H

#include <termios.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_POSIX_OPENPT
int posix_openpt(int flags);
#endif

#ifndef HAVE_OPENPTY
int openpty(int *amaster, int *aslave, char *name,
            const struct termios *termp,
            const struct winsize *winp);
#endif

#ifndef HAVE_FORKPTY
pid_t forkpty(int *amaster, char *name,
              const struct termios *termp,
              const struct winsize *winp);
#endif

#ifndef HAVE_LOGIN_TTY
int login_tty(int fd);
#endif

#ifndef HAVE_CFMAKERAW
void cfmakeraw(struct termios *termios_p);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_PTY_H */
