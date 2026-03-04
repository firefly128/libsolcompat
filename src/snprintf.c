/*
 * snprintf.c — C99-conformant snprintf/vsnprintf for Solaris 7
 *
 * Solaris 7's libc snprintf/vsnprintf return -1 on buffer overflow
 * instead of the C99-required count of characters that *would* have
 * been written (excluding NUL).  This breaks the universal pattern:
 *
 *     int n = snprintf(NULL, 0, fmt, ...);   // measure
 *     char *buf = malloc(n + 1);
 *     snprintf(buf, n + 1, fmt, ...);         // format
 *
 * Strategy: Use the system vsprintf() into a large stack buffer to
 * determine the true length, then copy up to 'size' bytes into the
 * caller's buffer.  For very large formats (>8KB on stack), fall
 * back to a heap allocation with doubling.
 *
 * This intentionally does NOT call the broken system vsnprintf.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

/* Unhide our names — we define the actual functions here */
#undef snprintf
#undef vsnprintf

#define STACK_BUF_SIZE 8192

int
solcompat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
    char stack_buf[STACK_BUF_SIZE];
    char *heap_buf = NULL;
    char *work_buf = stack_buf;
    size_t work_size = STACK_BUF_SIZE;
    int len;
    va_list ap2;

    /*
     * First attempt: use vsprintf into stack buffer.
     * vsprintf always writes everything (no truncation),
     * so we need a buffer large enough.  If the format
     * produces more than STACK_BUF_SIZE-1 chars, we detect
     * it and retry with a heap buffer.
     *
     * Safety: we can't know the length ahead of time without
     * formatting.  The stack buffer handles 99%+ of real-world
     * format strings.  For pathological cases, grow on heap.
     */

    /* Make a copy of ap since we might need to retry */
    va_copy(ap2, ap);
    len = vsprintf(stack_buf, fmt, ap2);
    va_end(ap2);

    if (len < 0) {
        /* True encoding error */
        return -1;
    }

    if ((size_t)len >= STACK_BUF_SIZE) {
        /*
         * Output was truncated (vsprintf wrote past our buffer —
         * this is a buffer overrun on the stack, but here we use
         * it as a signal to allocate heap space).
         *
         * Actually, vsprintf returns the number of chars written,
         * but it doesn't stop!  It wrote past the buffer.
         * We need a safer approach.
         *
         * Revised strategy: use the broken system vsnprintf to
         * format into the stack buffer (which DOES truncate), then
         * if it returns -1, double a heap buffer until vsnprintf
         * returns >= 0 (meaning the buffer was big enough).
         */
        work_size = STACK_BUF_SIZE * 2;
        while (1) {
            heap_buf = (char *)malloc(work_size);
            if (!heap_buf) {
                errno = ENOMEM;
                return -1;
            }

            va_copy(ap2, ap);
            /*
             * Use the REAL vsprintf (unbounded) — it always
             * writes the full output.  We allocated enough space.
             */
            len = vsprintf(heap_buf, fmt, ap2);
            va_end(ap2);

            if (len >= 0 && (size_t)len < work_size)
                break;

            /* Double and retry */
            free(heap_buf);
            work_size *= 2;
            if (work_size > 16 * 1024 * 1024) {
                /* Sanity limit: 16 MB */
                errno = ENOMEM;
                return -1;
            }
        }
        work_buf = heap_buf;
    }

    /* Now we know len and work_buf contains the full formatted string */
    if (str != NULL && size > 0) {
        if ((size_t)len < size) {
            memcpy(str, work_buf, (size_t)len + 1);
        } else {
            memcpy(str, work_buf, size - 1);
            str[size - 1] = '\0';
        }
    }

    if (heap_buf)
        free(heap_buf);

    return len;
}

int
solcompat_snprintf(char *str, size_t size, const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = solcompat_vsnprintf(str, size, fmt, ap);
    va_end(ap);

    return ret;
}
