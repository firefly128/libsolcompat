/*
 * solcompat/memory.h — Missing memory allocation functions
 *
 * posix_memalign, aligned_alloc, reallocarray, MAP_ANONYMOUS
 */
#ifndef SOLCOMPAT_MEMORY_H
#define SOLCOMPAT_MEMORY_H

#include <stdlib.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_POSIX_MEMALIGN
int posix_memalign(void **memptr, size_t alignment, size_t size);
#endif

#ifndef HAVE_ALIGNED_ALLOC
void *aligned_alloc(size_t alignment, size_t size);
#endif

#ifndef HAVE_REALLOCARRAY
void *reallocarray(void *ptr, size_t nmemb, size_t size);
#endif

/* MAP_ANONYMOUS — Solaris 7 uses /dev/zero for anonymous mappings */
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS  0x100
#define SOLCOMPAT_MAP_ANON_NEEDS_DEVZERO 1
#endif
#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif

/*
 * Wrapper for mmap that handles MAP_ANONYMOUS on Solaris 7
 * by opening /dev/zero internally when needed.
 */
void *solcompat_mmap_anon(void *addr, size_t length, int prot, int flags);

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_MEMORY_H */
