/*
 * atomic_compat.c -- GCC libatomic stubs for SPARC v7
 *
 * SPARC v7 lacks compare-and-swap instructions, so GCC emits calls
 * to libatomic helpers (__atomic_compare_exchange_N, etc.).  Since
 * GCC's libatomic was not built with our cross-toolchain, provide
 * minimal non-atomic implementations suitable for single-CPU systems.
 *
 * These are NOT truly atomic on SMP, but Solaris 7 on sun4m is
 * single-processor, so sequential consistency is guaranteed by the
 * kernel's non-preemptive scheduling of user threads.
 */

#include <string.h>

/* __atomic_is_lock_free -- always return true for sizes <= 4 on
 * single-CPU SPARC v7 (no actual lock needed). */
_Bool __atomic_is_lock_free(unsigned int size, const volatile void *ptr)
{
    (void)ptr;
    return (size <= 4) ? 1 : 0;
}

/* 4-byte (int/uint32_t) compare-and-swap */
int __atomic_compare_exchange_4(volatile void *ptr, void *expected,
                                unsigned int desired,
                                int success_memorder,
                                int failure_memorder)
{
    volatile unsigned int *p = (volatile unsigned int *)ptr;
    unsigned int *e = (unsigned int *)expected;
    if (*p == *e) {
        *p = desired;
        return 1;
    } else {
        *e = *p;
        return 0;
    }
}

/* 4-byte atomic exchange */
unsigned int __atomic_exchange_4(volatile void *ptr, unsigned int val,
                                 int memorder)
{
    volatile unsigned int *p = (volatile unsigned int *)ptr;
    unsigned int old = *p;
    *p = val;
    return old;
}

/* 4-byte atomic load */
unsigned int __atomic_load_4(const volatile void *ptr, int memorder)
{
    return *(const volatile unsigned int *)ptr;
}

/* 4-byte atomic store */
void __atomic_store_4(volatile void *ptr, unsigned int val, int memorder)
{
    *(volatile unsigned int *)ptr = val;
}

/* 4-byte atomic fetch-and-add */
unsigned int __atomic_fetch_add_4(volatile void *ptr, unsigned int val,
                                  int memorder)
{
    volatile unsigned int *p = (volatile unsigned int *)ptr;
    unsigned int old = *p;
    *p = old + val;
    return old;
}

/* 4-byte atomic fetch-and-sub */
unsigned int __atomic_fetch_sub_4(volatile void *ptr, unsigned int val,
                                  int memorder)
{
    volatile unsigned int *p = (volatile unsigned int *)ptr;
    unsigned int old = *p;
    *p = old - val;
    return old;
}

/* 1-byte compare-and-swap (for char-sized atomics) */
int __atomic_compare_exchange_1(volatile void *ptr, void *expected,
                                unsigned char desired,
                                int success_memorder,
                                int failure_memorder)
{
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    unsigned char *e = (unsigned char *)expected;
    if (*p == *e) {
        *p = desired;
        return 1;
    } else {
        *e = *p;
        return 0;
    }
}

/* 1-byte atomic exchange */
unsigned char __atomic_exchange_1(volatile void *ptr, unsigned char val,
                                  int memorder)
{
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    unsigned char old = *p;
    *p = val;
    return old;
}
