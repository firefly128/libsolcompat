/*
 * fenv.c — C99 floating-point environment for Solaris 7 SPARC
 *
 * Solaris 7 has <ieeefp.h> (fpgetround/fpsetround, fpgetsticky/fpsetsticky)
 * but not the C99 <fenv.h> interface. This maps the C99 functions onto
 * the Solaris/SPARC floating-point status register (FSR).
 *
 * SPARC FSR layout (relevant bits):
 *   [4:0]   cexc — current exception flags (sticky after trap)
 *   [9:5]   aexc — accrued exception flags (what we read as "raised")
 *   [31:30] rd   — rounding direction
 *
 * Part of libsolcompat
 */

#include <ieeefp.h>

/* C99 exception flag constants (bitmask in aexc field) */
#define FE_INVALID    0x10  /* bit 4: invalid operation */
#define FE_OVERFLOW   0x08  /* bit 3: overflow */
#define FE_UNDERFLOW  0x04  /* bit 2: underflow */
#define FE_DIVBYZERO  0x02  /* bit 1: division by zero */
#define FE_INEXACT    0x01  /* bit 0: inexact result */
#define FE_ALL_EXCEPT (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

/* C99 rounding direction constants (match SPARC FSR rd field values) */
#define FE_TONEAREST  0
#define FE_TOWARDZERO 1
#define FE_UPWARD     2
#define FE_DOWNWARD   3

/* fenv_t — full FSR state */
typedef unsigned int fenv_t;

/* fexcept_t — exception flags */
typedef unsigned int fexcept_t;

/* Default environment */
static const fenv_t __fe_dfl_env = 0;
const fenv_t *_fe_dfl_env_p = &__fe_dfl_env;

/*
 * Read/write the SPARC FSR directly via inline assembly.
 * On SPARC v7/v8, the FSR is accessed with st %fsr / ld %fsr.
 */
static unsigned int
read_fsr(void)
{
    unsigned int fsr_value;
    __asm__ __volatile__("st %%fsr, %0" : "=m"(fsr_value));
    return fsr_value;
}

static void
write_fsr(unsigned int fsr_value)
{
    __asm__ __volatile__("ld %0, %%fsr" : : "m"(fsr_value));
}

/* Extract accrued exception bits (FSR[9:5]) */
#define FSR_AEXC(fsr_value) (((fsr_value) >> 5) & 0x1F)

/* Clear specific accrued exception bits */
#define FSR_CLEAR_AEXC(fsr_value, exception_mask) ((fsr_value) & ~(((exception_mask) & 0x1F) << 5))

/* Set specific accrued exception bits */
#define FSR_SET_AEXC(fsr_value, exception_mask) ((fsr_value) | (((exception_mask) & 0x1F) << 5))

/* Extract rounding direction (FSR[31:30]) */
#define FSR_RD(fsr_value) (((fsr_value) >> 30) & 0x3)

/* Set rounding direction */
#define FSR_SET_RD(fsr_value, rounding_direction) (((fsr_value) & ~(0x3U << 30)) | (((rounding_direction) & 0x3) << 30))


int
feclearexcept(int exception_flags)
{
    if (exception_flags & ~FE_ALL_EXCEPT)
        return -1;
    unsigned int fsr_value = read_fsr();
    fsr_value = FSR_CLEAR_AEXC(fsr_value, exception_flags);
    /* Also clear current exceptions */
    fsr_value &= ~(exception_flags & 0x1F);
    write_fsr(fsr_value);
    return 0;
}

int
feraiseexcept(int exception_flags)
{
    if (exception_flags & ~FE_ALL_EXCEPT)
        return -1;
    unsigned int fsr_value = read_fsr();
    fsr_value = FSR_SET_AEXC(fsr_value, exception_flags);
    write_fsr(fsr_value);
    return 0;
}

int
fetestexcept(int exception_flags)
{
    unsigned int fsr_value = read_fsr();
    return FSR_AEXC(fsr_value) & exception_flags;
}

int
fegetexceptflag(fexcept_t *flag_ptr, int exception_flags)
{
    unsigned int fsr_value = read_fsr();
    *flag_ptr = FSR_AEXC(fsr_value) & exception_flags;
    return 0;
}

int
fesetexceptflag(const fexcept_t *flag_ptr, int exception_flags)
{
    unsigned int fsr_value = read_fsr();
    fsr_value = FSR_CLEAR_AEXC(fsr_value, exception_flags);
    fsr_value = FSR_SET_AEXC(fsr_value, *flag_ptr & exception_flags);
    write_fsr(fsr_value);
    return 0;
}

int
fegetround(void)
{
    unsigned int fsr_value = read_fsr();
    return FSR_RD(fsr_value);
}

int
fesetround(int rounding_direction)
{
    if (rounding_direction < 0 || rounding_direction > 3)
        return -1;
    unsigned int fsr_value = read_fsr();
    fsr_value = FSR_SET_RD(fsr_value, rounding_direction);
    write_fsr(fsr_value);
    return 0;
}

int
fegetenv(fenv_t *env_ptr)
{
    *env_ptr = read_fsr();
    return 0;
}

int
fesetenv(const fenv_t *env_ptr)
{
    if (env_ptr == (const fenv_t *)(-1) /* FE_DFL_ENV */) {
        write_fsr(__fe_dfl_env);
    } else {
        write_fsr(*env_ptr);
    }
    return 0;
}

int
feholdexcept(fenv_t *env_ptr)
{
    *env_ptr = read_fsr();
    /* Clear all exception flags and disable traps */
    unsigned int clean_fsr = *env_ptr;
    clean_fsr = FSR_CLEAR_AEXC(clean_fsr, FE_ALL_EXCEPT);
    clean_fsr &= ~0x1F;           /* clear current exceptions */
    clean_fsr &= ~(0x1F << 23);   /* disable all trap enables */
    write_fsr(clean_fsr);
    return 0;
}

int
feupdateenv(const fenv_t *env_ptr)
{
    /* Save current exceptions, restore env, re-raise saved exceptions */
    unsigned int current_fsr = read_fsr();
    int raised_exceptions = FSR_AEXC(current_fsr);

    fesetenv(env_ptr);

    if (raised_exceptions)
        feraiseexcept(raised_exceptions);

    return 0;
}
