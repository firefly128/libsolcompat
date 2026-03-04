/*
 * solcompat/poll_ext.h — ppoll and pselect
 */
#ifndef SOLCOMPAT_POLL_EXT_H
#define SOLCOMPAT_POLL_EXT_H

#include <poll.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_PPOLL
int ppoll(struct pollfd *fds, nfds_t nfds,
          const struct timespec *tmo_p, const sigset_t *sigmask);
#endif

#ifndef HAVE_PSELECT
int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_POLL_EXT_H */
