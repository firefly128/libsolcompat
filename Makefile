# libsolcompat — POSIX Compatibility Library for Solaris 7
#
# Builds with GCC (cross or native). No GNU make extensions required,
# but GNU make is recommended (gmake on Solaris).
#
# Cross-compile:  make CC=sparc-sun-solaris2.7-gcc AR=sparc-sun-solaris2.7-ar
# Native:         make
# Install:        make install DESTDIR=/path/to/staging

CC       ?= gcc
AR       ?= ar
CFLAGS   ?= -O2 -Wall -Wextra -Wno-unused-parameter
CPPFLAGS ?= -D_REENTRANT -I$(CURDIR)/include
LDFLAGS  ?=
PICFLAGS  = -fPIC

PREFIX   ?= /usr/tgcware
LIBDIR    = $(PREFIX)/lib
INCDIR    = $(PREFIX)/include/solcompat
DOCDIR    = $(PREFIX)/share/doc/solcompat

SONAME    = libsolcompat.so.1
SOVERSION = 1.0.0

# Source files
SRCS = src/snprintf.c \
       src/string.c \
       src/stdio.c \
       src/stdlib.c \
       src/c99_types.c \
       src/network.c \
       src/clock.c \
       src/math.c \
       src/memory.c \
       src/filesystem.c \
       src/at_funcs.c \
       src/pty.c \
       src/process.c \
       src/poll.c \
       src/random.c \
       src/stubs.c

OBJS     = $(SRCS:.c=.o)
PIC_OBJS = $(SRCS:.c=.lo)

# Targets
all: libsolcompat.a $(SONAME)

libsolcompat.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(SONAME): $(PIC_OBJS)
	$(CC) -shared -Wl,-h,$(SONAME) -o $@ $(PIC_OBJS) $(LDFLAGS) -lrt -lsocket -lnsl -lm

# Compile rules
.SUFFIXES: .c .o .lo

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.c.lo:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PICFLAGS) -c -o $@ $<

# Install
install: all
	mkdir -p $(DESTDIR)$(LIBDIR)
	mkdir -p $(DESTDIR)$(INCDIR)
	mkdir -p $(DESTDIR)$(DOCDIR)
	cp libsolcompat.a $(DESTDIR)$(LIBDIR)/
	cp $(SONAME) $(DESTDIR)$(LIBDIR)/libsolcompat.so.$(SOVERSION)
	cd $(DESTDIR)$(LIBDIR) && ln -sf libsolcompat.so.$(SOVERSION) $(SONAME)
	cd $(DESTDIR)$(LIBDIR) && ln -sf $(SONAME) libsolcompat.so
	cp include/solcompat/*.h $(DESTDIR)$(INCDIR)/
	cp README.md $(DESTDIR)$(DOCDIR)/ 2>/dev/null || true
	cp LICENSE $(DESTDIR)$(DOCDIR)/ 2>/dev/null || true

# Tests (run on Solaris target)
test: all
	cd tests && $(CC) $(CPPFLAGS) $(CFLAGS) -o test_all test_all.c \
		-L$(CURDIR) -lsolcompat -lrt -lsocket -lnsl -lm && ./test_all

clean:
	rm -f $(OBJS) $(PIC_OBJS) libsolcompat.a $(SONAME)
	rm -f tests/test_all

.PHONY: all install test clean
