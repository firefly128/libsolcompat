/*
 * poll.c — ppoll and pselect for Solaris 7
 *
 * Solaris 7 has poll() and select() but not ppoll() or pselect().
 * We emulate them by temporarily setting the signal mask and
 * using the non-atomic base functions.
 *
 * Note: This is NOT fully atomic w.r.t. signals (the window between
 * sigprocmask and poll/select is theoretically vulnerable to races).
 * A correct implementation would need kernel support.  This is
 * sufficient for most practical uses.
 */

#include <poll.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

int
ppoll(struct pollfd *fds, nfds_t nfds,
      const struct timespec *tmo_p, const sigset_t *sigmask)
{
    sigset_t origmask;
    int timeout_ms;
    int ret;

    /* Convert timespec to milliseconds for poll() */
    if (tmo_p == NULL) {
        timeout_ms = -1; /* infinite */
    } else {
        timeout_ms = (int)(tmo_p->tv_sec * 1000 + tmo_p->tv_nsec / 1000000);
        if (timeout_ms < 0)
            timeout_ms = 0;
    }

    if (sigmask)
        sigprocmask(SIG_SETMASK, sigmask, &origmask);

    ret = poll(fds, nfds, timeout_ms);

    if (sigmask)
        sigprocmask(SIG_SETMASK, &origmask, NULL);

    return ret;
}

int
pselect(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, const struct timespec *timeout,
        const sigset_t *sigmask)
{
    sigset_t origmask;
    struct timeval tv, *tvp;
    int ret;

    if (timeout) {
        tv.tv_sec = timeout->tv_sec;
        tv.tv_usec = timeout->tv_nsec / 1000;
        tvp = &tv;
    } else {
        tvp = NULL;
    }

    if (sigmask)
        sigprocmask(SIG_SETMASK, sigmask, &origmask);

    ret = select(nfds, readfds, writefds, exceptfds, tvp);

    if (sigmask)
        sigprocmask(SIG_SETMASK, &origmask, NULL);

    return ret;
}
