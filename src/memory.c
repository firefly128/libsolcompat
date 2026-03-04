/*
 * memory.c — Missing memory allocation functions for Solaris 7
 *
 * posix_memalign (wrapping memalign), aligned_alloc, reallocarray,
 * MAP_ANONYMOUS wrapper via /dev/zero
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

/* Solaris 7 has memalign() in <stdlib.h> */
extern void *memalign(size_t alignment, size_t size);

int
posix_memalign(void **memptr, size_t alignment, size_t size)
{
    void *ptr;

    /* POSIX requires alignment to be power of 2 and multiple of sizeof(void*) */
    if (alignment < sizeof(void *))
        return EINVAL;
    if ((alignment & (alignment - 1)) != 0)
        return EINVAL;

    ptr = memalign(alignment, size);
    if (!ptr)
        return ENOMEM;

    *memptr = ptr;
    return 0;
}

void *
aligned_alloc(size_t alignment, size_t size)
{
    /* C11: size must be multiple of alignment */
    if (alignment == 0 || (size % alignment) != 0) {
        errno = EINVAL;
        return NULL;
    }
    return memalign(alignment, size);
}

void *
reallocarray(void *ptr, size_t nmemb, size_t size)
{
    /* Overflow check */
    if (nmemb != 0 && size != 0) {
        if (nmemb > (size_t)-1 / size) {
            errno = ENOMEM;
            return NULL;
        }
    }
    return realloc(ptr, nmemb * size);
}

/*
 * MAP_ANONYMOUS helper.
 * Solaris 7 doesn't support MAP_ANONYMOUS directly — you must
 * mmap /dev/zero.  This wrapper transparently opens /dev/zero.
 */
void *
solcompat_mmap_anon(void *addr, size_t length, int prot, int flags)
{
    int fd;
    void *result;

    fd = open("/dev/zero", O_RDWR);
    if (fd < 0)
        return MAP_FAILED;

    /* Strip our synthetic MAP_ANONYMOUS flag, use the fd */
    flags &= ~0x100;

    result = mmap(addr, length, prot, flags, fd, 0);
    close(fd);

    return result;
}
