# libsolcompat

**POSIX/C99/C11 Compatibility Library for Solaris 7 SPARC**

libsolcompat bridges the gap between Solaris 7's SUSv2/POSIX.1-1997 APIs and
what modern (2026-era) open source software expects. It provides ~100+
functions and types that are missing from Solaris 7 but required by current
versions of GCC, GNU coreutils, Python, OpenSSL, git, and other foundational
tools.

## What It Provides

| Module | Functions |
|--------|-----------|
| **snprintf** | C99-conformant `snprintf`/`vsnprintf` (Solaris 7 returns -1 on truncation) |
| **string** | `strndup`, `strnlen`, `strlcpy`, `strlcat`, `strcasestr`, `memmem`, `strsep`, `stpcpy`, `stpncpy`, `strchrnul`, `memrchr`, `strtoimax`, `strtoumax` |
| **stdio** | `getline`, `getdelim`, `vasprintf`, `asprintf`, `dprintf` |
| **stdlib** | `setenv`, `unsetenv`, `mkdtemp` |
| **c99_types** | `SCN*` scan format macros, `INTMAX_C`/`UINTMAX_C` |
| **network** | `getaddrinfo`/`freeaddrinfo`/`gai_strerror`/`getnameinfo`, `inet_ntop`/`inet_pton`, `struct sockaddr_storage`, `getifaddrs`/`freeifaddrs` |
| **clock** | `CLOCK_MONOTONIC` (via `gethrtime()`), `CLOCK_PROCESS_CPUTIME_ID`, `clock_nanosleep` |
| **math** | `round`, `trunc`, `log2`, `exp2`, `fdim`, `fmin`, `fmax`, `fpclassify`, `NAN`, `INFINITY` |
| **memory** | `posix_memalign`, `aligned_alloc`, `reallocarray`, `MAP_ANONYMOUS` wrapper |
| **filesystem** | `utimes`, `futimens`, `utimensat`, `flock`, `scandir`/`alphasort`, `fdopendir`, `posix_fadvise` |
| **at_funcs** | `openat`, `mkdirat`, `renameat`, `unlinkat`, `fstatat`, `fchownat`, `fchmodat`, `readlinkat`, `symlinkat`, `linkat`, `faccessat` (mutex-protected fchdir emulation) |
| **pty** | `posix_openpt`, `openpty`, `forkpty`, `login_tty`, `cfmakeraw` |
| **process** | `daemon`, `err`/`warn`/`errx`/`warnx`, `posix_spawn`/`posix_spawnp`, `pipe2`, `dup3`, `mkostemp` |
| **poll** | `ppoll`, `pselect` |
| **random** | `explicit_bzero`, `getentropy`, `arc4random`/`arc4random_buf`/`arc4random_uniform` |
| **stubs** | `pthread_setname_np`, `newlocale`/`uselocale`/`freelocale`/`duplocale` |

## Building

### Cross-compile (from x86_64 Linux build host)

```sh
make CC=sparc-sun-solaris2.7-gcc AR=sparc-sun-solaris2.7-ar
```

### Native (on Solaris 7 SPARC)

```sh
gmake
```

### Install

```sh
gmake install DESTDIR=/path/to/staging PREFIX=/usr/tgcware
```

This installs:
- `lib/libsolcompat.a` — static library
- `lib/libsolcompat.so.1` — shared library
- `include/solcompat/*.h` — headers

## Usage

### Automatic (recommended)

Add to your build flags:
```sh
CPPFLAGS="-I/usr/tgcware/include -include solcompat/solcompat.h"
LDFLAGS="-L/usr/tgcware/lib -lsolcompat"
```

The `-include` flag automatically overrides broken libc functions (like
snprintf) and provides all missing prototypes.

### Selective

Include only the headers you need:
```c
#include <solcompat/snprintf.h>
#include <solcompat/string_ext.h>
```

## Testing

```sh
gmake test
```

Runs the test suite covering all subsystems. Requires running on Solaris 7
SPARC (or QEMU emulating one).

## Design Notes

- **Thread safety**: The `*at()` functions use a global mutex + `fchdir()`
  dance. Single-threaded code is unaffected; multi-threaded code gets
  serialized through these specific calls.
- **IPv4 only**: The networking module uses `gethostbyname` under the hood.
  Solaris 7 has experimental IPv6 but we don't rely on it.
- **No ELF TLS**: Solaris 7's linker doesn't support `__thread`. Use
  `--disable-tls` when building GCC (which activates emutls in libgcc).
- **Random quality**: If SUNWski is installed, `/dev/random` provides real
  entropy. Otherwise falls back to a PRNG seeded from `gethrtime()`.

## Target Environment

- Solaris 7 (SunOS 5.7) SPARC
- GCC 4.4.7+ (TGCware) or GCC 4.9.4 cross-compiled
- Installed to `/usr/tgcware` alongside the TGCware toolchain

## License

MIT — see [LICENSE](LICENSE)
