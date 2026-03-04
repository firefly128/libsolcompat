/*
 * solcompat/random.h — Cryptographic and general-purpose randomness
 *
 * explicit_bzero, getentropy, arc4random family
 */
#ifndef SOLCOMPAT_RANDOM_H
#define SOLCOMPAT_RANDOM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_EXPLICIT_BZERO
void explicit_bzero(void *s, size_t n);
#endif

#ifndef HAVE_GETENTROPY
int getentropy(void *buffer, size_t length);
#endif

#ifndef HAVE_ARC4RANDOM
uint32_t arc4random(void);
void arc4random_buf(void *buf, size_t nbytes);
uint32_t arc4random_uniform(uint32_t upper_bound);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SOLCOMPAT_RANDOM_H */
