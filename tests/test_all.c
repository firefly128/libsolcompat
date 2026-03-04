/*
 * test_all.c — Test suite for libsolcompat
 *
 * Compile: cc -I../include -o test_all test_all.c -L.. -lsolcompat -lrt -lsocket -lnsl -lm
 * Run:     ./test_all
 *
 * Each subsystem has a test_*() function.  On failure, prints the
 * test name and exits with code 1.  On success, prints "PASS".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

/* Include the master header — this pulls in everything */
#include <solcompat/solcompat.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        printf("  %-40s ", #name); \
        fflush(stdout); \
    } while (0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASS\n"); \
    } while (0)

#define FAIL(msg) \
    do { \
        printf("FAIL: %s\n", msg); \
    } while (0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { FAIL(msg); return; } \
    } while (0)

/* ===== snprintf tests ===== */
static void test_snprintf_basic(void)
{
    char buf[32];
    int n;

    TEST(snprintf_basic);
    n = snprintf(buf, sizeof(buf), "hello %s", "world");
    ASSERT(n == 11, "expected 11");
    ASSERT(strcmp(buf, "hello world") == 0, "wrong output");
    PASS();
}

static void test_snprintf_truncation(void)
{
    char buf[8];
    int n;

    TEST(snprintf_truncation);
    n = snprintf(buf, sizeof(buf), "hello world, this is long");
    ASSERT(n == 25, "should return 25 (full length)");
    ASSERT(buf[7] == '\0', "should be NUL terminated");
    ASSERT(strncmp(buf, "hello w", 7) == 0, "wrong truncated output");
    PASS();
}

static void test_snprintf_null_measure(void)
{
    int n;

    TEST(snprintf_null_measure);
    n = snprintf(NULL, 0, "test %d %s", 42, "string");
    ASSERT(n > 0, "should return positive count");
    ASSERT(n == 14, "expected 14");
    PASS();
}

/* ===== string tests ===== */
static void test_strndup(void)
{
    char *s;

    TEST(strndup);
    s = strndup("hello world", 5);
    ASSERT(s != NULL, "returned NULL");
    ASSERT(strcmp(s, "hello") == 0, "wrong result");
    free(s);
    PASS();
}

static void test_strnlen(void)
{
    TEST(strnlen);
    ASSERT(strnlen("hello", 10) == 5, "wrong for short string");
    ASSERT(strnlen("hello", 3) == 3, "wrong for limited maxlen");
    PASS();
}

static void test_strlcpy(void)
{
    char buf[8];

    TEST(strlcpy);
    ASSERT(strlcpy(buf, "hello", sizeof(buf)) == 5, "wrong return");
    ASSERT(strcmp(buf, "hello") == 0, "wrong copy");
    ASSERT(strlcpy(buf, "very long string", sizeof(buf)) == 16, "should return src len");
    ASSERT(buf[7] == '\0', "not NUL terminated");
    PASS();
}

static void test_strlcat(void)
{
    char buf[16];

    TEST(strlcat);
    strlcpy(buf, "hello", sizeof(buf));
    ASSERT(strlcat(buf, " world", sizeof(buf)) == 11, "wrong return");
    ASSERT(strcmp(buf, "hello world") == 0, "wrong concat");
    PASS();
}

static void test_strcasestr(void)
{
    TEST(strcasestr);
    ASSERT(strcasestr("Hello World", "WORLD") != NULL, "should find");
    ASSERT(strcasestr("Hello World", "xyz") == NULL, "should not find");
    PASS();
}

static void test_memmem(void)
{
    TEST(memmem);
    ASSERT(memmem("hello world", 11, "world", 5) != NULL, "should find");
    ASSERT(memmem("hello world", 11, "xyz", 3) == NULL, "should not find");
    PASS();
}

static void test_strsep(void)
{
    char buf[] = "one:two:three";
    char *p = buf;
    char *tok;

    TEST(strsep);
    tok = strsep(&p, ":");
    ASSERT(strcmp(tok, "one") == 0, "first token wrong");
    tok = strsep(&p, ":");
    ASSERT(strcmp(tok, "two") == 0, "second token wrong");
    tok = strsep(&p, ":");
    ASSERT(strcmp(tok, "three") == 0, "third token wrong");
    ASSERT(p == NULL, "should be NULL after last token");
    PASS();
}

static void test_memrchr(void)
{
    TEST(memrchr);
    ASSERT(memrchr("hello", 'l', 5) == (void *)("hello" + 3), "should find last l");
    ASSERT(memrchr("hello", 'z', 5) == NULL, "should not find z");
    PASS();
}

/* ===== stdio tests ===== */
static void test_vasprintf_wrapper(void)
{
    char *s;
    int n;

    TEST(asprintf);
    n = asprintf(&s, "number %d and %s", 42, "str");
    ASSERT(n > 0, "should return positive");
    ASSERT(s != NULL, "should allocate");
    ASSERT(strcmp(s, "number 42 and str") == 0, "wrong output");
    free(s);
    PASS();
}

static void test_getline(void)
{
    FILE *f;
    char *line = NULL;
    size_t n = 0;
    ssize_t len;

    TEST(getline);
    f = tmpfile();
    ASSERT(f != NULL, "tmpfile failed");

    fputs("line one\nline two\n", f);
    rewind(f);

    len = getline(&line, &n, f);
    ASSERT(len > 0, "getline returned <= 0");
    ASSERT(strcmp(line, "line one\n") == 0, "wrong first line");

    len = getline(&line, &n, f);
    ASSERT(len > 0, "getline returned <= 0 for second line");
    ASSERT(strcmp(line, "line two\n") == 0, "wrong second line");

    free(line);
    fclose(f);
    PASS();
}

/* ===== stdlib tests ===== */
static void test_setenv_unsetenv(void)
{
    TEST(setenv_unsetenv);
    ASSERT(setenv("SOLCOMPAT_TEST_VAR", "hello", 1) == 0, "setenv failed");
    ASSERT(strcmp(getenv("SOLCOMPAT_TEST_VAR"), "hello") == 0, "wrong value");
    ASSERT(setenv("SOLCOMPAT_TEST_VAR", "world", 0) == 0, "setenv no-overwrite failed");
    ASSERT(strcmp(getenv("SOLCOMPAT_TEST_VAR"), "hello") == 0, "should not overwrite");
    ASSERT(unsetenv("SOLCOMPAT_TEST_VAR") == 0, "unsetenv failed");
    ASSERT(getenv("SOLCOMPAT_TEST_VAR") == NULL, "should be gone");
    PASS();
}

static void test_mkdtemp(void)
{
    char tmpl[] = "/tmp/solcompat_test_XXXXXX";

    TEST(mkdtemp);
    ASSERT(mkdtemp(tmpl) != NULL, "mkdtemp returned NULL");
    /* Verify directory exists */
    {
        struct stat st;
        ASSERT(stat(tmpl, &st) == 0, "dir doesn't exist");
        ASSERT(S_ISDIR(st.st_mode), "not a directory");
    }
    rmdir(tmpl);
    PASS();
}

/* ===== clock tests ===== */
static void test_clock_monotonic(void)
{
    struct timespec ts;

    TEST(clock_monotonic);
    ASSERT(clock_gettime(CLOCK_MONOTONIC, &ts) == 0, "clock_gettime failed");
    ASSERT(ts.tv_sec > 0 || ts.tv_nsec > 0, "time should be non-zero");
    PASS();
}

static void test_clock_realtime(void)
{
    struct timespec ts;

    TEST(clock_realtime);
    ASSERT(clock_gettime(CLOCK_REALTIME, &ts) == 0, "clock_gettime failed");
    ASSERT(ts.tv_sec > 946684800, "time should be after Y2K");
    PASS();
}

static void test_clock_getres(void)
{
    struct timespec res;

    TEST(clock_getres);
    ASSERT(clock_getres(CLOCK_MONOTONIC, &res) == 0, "clock_getres failed");
    ASSERT(res.tv_sec == 0 && res.tv_nsec > 0, "resolution should be sub-second");
    PASS();
}

/* ===== math tests ===== */
static void test_round(void)
{
    TEST(round);
    ASSERT(round(1.4) == 1.0, "round(1.4) != 1.0");
    ASSERT(round(1.5) == 2.0, "round(1.5) != 2.0");
    ASSERT(round(-1.5) == -2.0, "round(-1.5) != -2.0");
    PASS();
}

static void test_trunc(void)
{
    TEST(trunc);
    ASSERT(trunc(1.9) == 1.0, "trunc(1.9) != 1.0");
    ASSERT(trunc(-1.9) == -1.0, "trunc(-1.9) != -1.0");
    PASS();
}

static void test_log2_exp2(void)
{
    TEST(log2_exp2);
    ASSERT(fabs(log2(8.0) - 3.0) < 0.0001, "log2(8) != 3");
    ASSERT(fabs(exp2(3.0) - 8.0) < 0.0001, "exp2(3) != 8");
    PASS();
}

static void test_fmin_fmax(void)
{
    TEST(fmin_fmax);
    ASSERT(fmin(1.0, 2.0) == 1.0, "fmin wrong");
    ASSERT(fmax(1.0, 2.0) == 2.0, "fmax wrong");
    PASS();
}

/* ===== memory tests ===== */
static void test_posix_memalign(void)
{
    void *ptr = NULL;

    TEST(posix_memalign);
    ASSERT(posix_memalign(&ptr, 64, 256) == 0, "posix_memalign failed");
    ASSERT(ptr != NULL, "returned NULL");
    ASSERT(((unsigned long)ptr & 63) == 0, "not aligned to 64");
    free(ptr);
    PASS();
}

static void test_reallocarray(void)
{
    void *ptr;

    TEST(reallocarray);
    ptr = reallocarray(NULL, 100, sizeof(int));
    ASSERT(ptr != NULL, "returned NULL");
    free(ptr);

    /* Overflow test */
    ptr = reallocarray(NULL, (size_t)-1, (size_t)-1);
    ASSERT(ptr == NULL, "should fail on overflow");
    ASSERT(errno == ENOMEM, "should set ENOMEM");
    PASS();
}

/* ===== filesystem tests ===== */
static void test_flock(void)
{
    int fd;

    TEST(flock);
    fd = open("/tmp/solcompat_flock_test", O_CREAT | O_WRONLY, 0644);
    ASSERT(fd >= 0, "open failed");
    ASSERT(flock(fd, LOCK_EX | LOCK_NB) == 0, "exclusive lock failed");
    ASSERT(flock(fd, LOCK_UN) == 0, "unlock failed");
    close(fd);
    unlink("/tmp/solcompat_flock_test");
    PASS();
}

static void test_scandir(void)
{
    struct dirent **namelist;
    int n;

    TEST(scandir);
    n = scandir("/tmp", &namelist, NULL, alphasort);
    ASSERT(n >= 0, "scandir failed");
    /* Free results */
    {
        int i;
        for (i = 0; i < n; i++)
            free(namelist[i]);
        free(namelist);
    }
    PASS();
}

/* ===== at_funcs tests ===== */
static void test_openat(void)
{
    int fd;
    char path[] = "/tmp/solcompat_openat_test";

    TEST(openat);
    /* AT_FDCWD should behave like normal open */
    fd = openat(AT_FDCWD, path, O_CREAT | O_WRONLY, 0644);
    ASSERT(fd >= 0, "openat(AT_FDCWD) failed");
    close(fd);
    unlink(path);
    PASS();
}

static void test_fstatat(void)
{
    struct stat st;

    TEST(fstatat);
    ASSERT(fstatat(AT_FDCWD, "/tmp", &st, 0) == 0, "fstatat /tmp failed");
    ASSERT(S_ISDIR(st.st_mode), "/tmp should be a directory");
    PASS();
}

/* ===== pipe2/dup3 tests ===== */
static void test_pipe2(void)
{
    int pipefd[2];

    TEST(pipe2);
    ASSERT(pipe2(pipefd, 0) == 0, "pipe2 failed");
    close(pipefd[0]);
    close(pipefd[1]);
    PASS();
}

static void test_dup3(void)
{
    int fd, newfd;

    TEST(dup3);
    fd = open("/dev/null", O_RDONLY);
    ASSERT(fd >= 0, "open /dev/null failed");
    newfd = dup3(fd, fd + 10, O_CLOEXEC);
    ASSERT(newfd == fd + 10, "dup3 wrong fd");
    close(fd);
    close(newfd);
    PASS();
}

/* ===== random tests ===== */
static void test_getentropy(void)
{
    unsigned char buf[32];

    TEST(getentropy);
    memset(buf, 0, sizeof(buf));
    ASSERT(getentropy(buf, sizeof(buf)) == 0, "getentropy failed");
    /* Extremely unlikely to be all zeros if working */
    {
        int all_zero = 1;
        int i;
        for (i = 0; i < 32; i++) {
            if (buf[i] != 0) { all_zero = 0; break; }
        }
        ASSERT(!all_zero, "all zeros — probably broken");
    }
    PASS();
}

static void test_arc4random(void)
{
    uint32_t a, b;

    TEST(arc4random);
    a = arc4random();
    b = arc4random();
    /* They could theoretically be equal but astronomically unlikely */
    ASSERT(a != b || a != arc4random(), "all same values — broken");
    PASS();
}

static void test_arc4random_uniform(void)
{
    int i;
    int bad = 0;

    TEST(arc4random_uniform);
    for (i = 0; i < 1000; i++) {
        uint32_t v = arc4random_uniform(100);
        if (v >= 100) { bad = 1; break; }
    }
    ASSERT(!bad, "value out of range");
    PASS();
}

/* ===== explicit_bzero test ===== */
static void test_explicit_bzero(void)
{
    char buf[16];

    TEST(explicit_bzero);
    memset(buf, 0xAA, sizeof(buf));
    explicit_bzero(buf, sizeof(buf));
    {
        int i, all_zero = 1;
        for (i = 0; i < 16; i++) {
            if (buf[i] != 0) { all_zero = 0; break; }
        }
        ASSERT(all_zero, "not zeroed");
    }
    PASS();
}

/* ===== ppoll test ===== */
static void test_ppoll(void)
{
    struct pollfd pfd;
    struct timespec ts;
    int ret;

    TEST(ppoll);
    pfd.fd = 0; /* stdin */
    pfd.events = POLLIN;
    pfd.revents = 0;

    ts.tv_sec = 0;
    ts.tv_nsec = 1000000; /* 1ms timeout */

    ret = ppoll(&pfd, 1, &ts, NULL);
    /* Should timeout (return 0) or succeed — not error */
    ASSERT(ret >= 0 || errno == EINTR, "ppoll failed unexpectedly");
    PASS();
}

/* ===== locale stub tests ===== */
static void test_locale_stubs(void)
{
    locale_t loc;

    TEST(locale_stubs);
    loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT(loc != NULL, "newlocale returned NULL");

    /* uselocale should return previous */
    uselocale(loc);
    freelocale(loc);
    PASS();
}

/* ===== Main ===== */
int
main(void)
{
    printf("libsolcompat test suite v%s\n", SOLCOMPAT_VERSION_STRING);
    printf("========================================\n\n");

    printf("[snprintf]\n");
    test_snprintf_basic();
    test_snprintf_truncation();
    test_snprintf_null_measure();

    printf("\n[string]\n");
    test_strndup();
    test_strnlen();
    test_strlcpy();
    test_strlcat();
    test_strcasestr();
    test_memmem();
    test_strsep();
    test_memrchr();

    printf("\n[stdio]\n");
    test_vasprintf_wrapper();
    test_getline();

    printf("\n[stdlib]\n");
    test_setenv_unsetenv();
    test_mkdtemp();

    printf("\n[clock]\n");
    test_clock_monotonic();
    test_clock_realtime();
    test_clock_getres();

    printf("\n[math]\n");
    test_round();
    test_trunc();
    test_log2_exp2();
    test_fmin_fmax();

    printf("\n[memory]\n");
    test_posix_memalign();
    test_reallocarray();

    printf("\n[filesystem]\n");
    test_flock();
    test_scandir();

    printf("\n[at_funcs]\n");
    test_openat();
    test_fstatat();

    printf("\n[process]\n");
    test_pipe2();
    test_dup3();

    printf("\n[random]\n");
    test_getentropy();
    test_arc4random();
    test_arc4random_uniform();
    test_explicit_bzero();

    printf("\n[poll]\n");
    test_ppoll();

    printf("\n[stubs]\n");
    test_locale_stubs();

    printf("\n========================================\n");
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
