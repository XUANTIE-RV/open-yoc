/*
 * math.h
 * 
 * Copyright(C):2012 Hangzhou C-SKY Microsystem Co.,LTD.
 * Author:Jin Yue  (yue_jin@c-sky.com)
 * Date:2012-04-26
 */

#ifndef  _MATH_H_
#define  _MATH_H_

#include <features.h>
#include "ansidef.h"
#include "serf/math_private.h"

_BEGIN_STD_C

/* Natural log of 2 */
#define _M_LN2        0.693147180559945309417

#if defined(__GNUC__) && \
  ( (__GNUC__ >= 4) || \
    ( (__GNUC__ >= 3) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 3) ) )

 /* gcc >= 3.3 implicitly defines builtins for HUGE_VALx values.  */

# ifndef HUGE_VAL
#  define HUGE_VAL (__builtin_huge_val())
# endif

# ifndef HUGE_VALF
#  define HUGE_VALF (__builtin_huge_valf())
# endif

# ifndef HUGE_VALL
#  define HUGE_VALL (__builtin_huge_vall())
# endif

# ifndef INFINITY
#  define INFINITY (__builtin_inff())
# endif

# ifndef NAN
#  define NAN (__builtin_nanf(""))
# endif

#else /* !gcc >= 3.3  */

 /*      No builtins.  Use fixed defines instead.  (All 3 HUGE plus the INFINITY
  * and NAN macros are required to be constant expressions.  Using a variable--
  * even a static const--does not meet this requirement, as it cannot be
  * evaluated at translation time.)
  *      The infinities are done using numbers that are far in excess of
  * something that would be expected to be encountered in a floating-point
  * implementation.  (A more certain way uses values from float.h, but that is
  * avoided because system includes are not supposed to include each other.)
  *      This method might produce warnings from some compilers.  (It does in
  * newer GCCs, but not for ones that would hit this #else.)  If this happens,
  * please report details to the Minilibc mailing list.  */

 #ifndef HUGE_VAL
  #define HUGE_VAL (1.0e999999999)
 #endif

 #ifndef HUGE_VALF
  #define HUGE_VALF (1.0e999999999F)
 #endif

 #if !defined(HUGE_VALL)  &&  defined(_HAVE_LONG_DOUBLE)
  #define HUGE_VALL (1.0e999999999L)
 #endif

 #if !defined(INFINITY)
  #define INFINITY (HUGE_VALF)
 #endif

 #if !defined(NAN)
  #if defined(__GNUC__)  &&  defined(__cplusplus)
    /* Exception:  older g++ versions warn about the divide by 0 used in the
     * normal case (even though older gccs do not).  This trick suppresses the
     * warning, but causes errors for plain gcc, so is only used in the one
     * special case.  */
    static const union { unsigned long __i[1]; float __d; } __Nanf = {0x7FC00000};
    #define NAN (__Nanf.__d)
  #else
    #define NAN (0.0F/0.0F)
  #endif
 #endif

#endif /* !gcc >= 3.3  */

#if defined(__cplusplus) || __STDC_VERSION__ >= 199901L

/* ISO C99 types and macros. */

#ifndef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD 0
typedef float float_t;
typedef double double_t;
#endif /* FLT_EVAL_METHOD */

#define FP_NAN         0
#define FP_INFINITE    1
#define FP_ZERO        2
#define FP_SUBNORMAL   3
#define FP_NORMAL      4

#ifndef FP_ILOGB0
# define FP_ILOGB0 (-INT_MAX)
#endif
#ifndef FP_ILOGBNAN
# define FP_ILOGBNAN INT_MAX
#endif

#ifndef MATH_ERRNO
# define MATH_ERRNO 1
#endif
#ifndef MATH_ERREXCEPT
# define MATH_ERREXCEPT 2
#endif
#ifndef math_errhandling
# define math_errhandling MATH_ERRNO
#endif


#define fpclassify(__x) \
	((sizeof(__x) == sizeof(float))  ? __fpclassifyf(__x) : \
	__fpclassifyd(__x))

#define isfinite(__y) \
          (__extension__ ({int __cy = fpclassify(__y); \
                           __cy != FP_INFINITE && __cy != FP_NAN;}))

/* Note: isinf and isnan were once functions in minilibc that took double
 *       arguments.  C99 specifies that these names are reserved for macros
 *       supporting multiple floating point types.  Thus, they are
 *       now defined as macros.  Implementations of the old functions
 *       taking double arguments still exist for compatibility purposes
 *       (prototypes for them are in <ieeefp.h>).  */
#ifndef isinf
  #define isinf(y) (fpclassify(y) == FP_INFINITE)
#endif

#ifndef isnan
  #define isnan(y) (fpclassify(y) == FP_NAN)
#endif

#define isnormal(y) (fpclassify(y) == FP_NORMAL)
#define signbit(__x) \
	((sizeof(__x) == sizeof(float))  ?  __signbitf(__x) : \
		__signbitd(__x))

#define isgreater(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x > __y);}))
#define isgreaterequal(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x >= __y);}))
#define isless(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x < __y);}))
#define islessequal(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x <= __y);}))
#define islessgreater(x,y) \
          (__extension__ ({__typeof__(x) __x = (x); __typeof__(y) __y = (y); \
                           !isunordered(__x,__y) && (__x < __y || __x > __y);}))

#define isunordered(a,b) \
          (__extension__ ({__typeof__(a) __a = (a); __typeof__(b) __b = (b); \
                           fpclassify(__a) == FP_NAN || fpclassify(__b) == FP_NAN;}))

/* Non ANSI double precision functions.  */

#if !defined(__cplusplus)
#define log2(x) (log (x) / _M_LN2)
#endif

/*  single precision functions.  */

#if !defined(__cplusplus)
#define log2f(x) (logf (x) / (float_t) _M_LN2)
#endif
#endif /* defined(__cplusplus) || __STDC_VERSION__ >= 199901L */

/* The exception structure passed to the matherr routine.  */
/* We have a problem when using C++ since `exception' is a reserved
   name in C++.  */
#ifdef __cplusplus
struct __exception
#else
struct exception
#endif
{
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
	int err;
};

/* Values for the type field of struct exception.  */

#define DOMAIN 1
#define SING 2
#define OVERFLOW 3
#define UNDERFLOW 4
#define TLOSS 5
#define PLOSS 6

/* Useful constants.  */

#define MAXFLOAT		3.40282347e+38F

#define M_E				2.7182818284590452354		/* e */ 
#define M_LOG2E			1.4426950408889634074		/* log_2 e */
#define M_LOG10E		0.43429448190325182765		/* log_2 10 */
#define M_LN2			_M_LN2						/* log_e 2 */
#define M_LN10			2.30258509299404568402		/* log_e 10 */
#define M_PI			3.14159265358979323846		/* pi */
#define M_TWOPI			(M_PI * 2.0)				/* 2pi */
#define M_PI_2			1.57079632679489661923		/* pi/2 */
#define M_PI_4			0.78539816339744830962		/* pi/4 */
#define M_3PI_4			2.3561944901923448370E0		/* 3pi/4 */
#define M_SQRTPI		1.77245385090551602792981	/* sqrt(pi) */
#define M_1_PI			0.31830988618379067154		/* 1/pi */
#define M_2_PI			0.63661977236758134308		/* 2/pi */
#define M_2_SQRTPI		1.12837916709551257390		/* 2/sqrt(pi) */
#define M_SQRT2			1.41421356237309504880		/* sqrt(2) */
#define M_SQRT1_2		0.70710678118654752440		/* 1/sqrt(2) */
#define M_LN2LO			1.9082149292705877000E-10	
#define M_LN2HI			6.9314718036912381649E-1
#define M_SQRT3			1.73205080756887719000		/* sqrt(3) */
#define M_IVLN10		0.43429448190325182765		/* 1/log(10) */
#define M_LOG2_E        _M_LN2
#define M_INVLN2        1.4426950408889633870E0		/* 1/log(2) */

extern double pow(double x,double y);
extern double log(double x);
extern double log10(double x);
extern double copysign(double x, double y);
extern double scalbn(double x, int n);
extern double fact(int x);
extern double sqrt(double x);
extern double fabs(double x);
extern double asin(double x);
extern double sin(double x);
extern double cos(double x);
extern double acos(double x);
extern double tan(double x);
extern double cosh(double x);
extern double sinh(double x);
extern double tanh(double x);
extern double atan(double x);
extern double ceil(double x);
extern double floor(double x);
extern double fmod(double x,double y);
extern double frexp(double f, int *p);
extern double ldexp(double x,int p);
extern double modf(double x, double *i);

extern double atan2(double y,double x);
extern double exp(double x);
extern double nan (const char *);
extern double expm1(double x);

/* Single precision versions of ANSI functions.  */

extern float atanf _PARAMS((float));
extern float cosf _PARAMS((float));
extern float sinf _PARAMS((float));
extern float tanf _PARAMS((float));
extern float tanhf _PARAMS((float));
extern float frexpf _PARAMS((float, int *));
extern float modff _PARAMS((float, float *));
extern float ceilf _PARAMS((float));
extern float fabsf _PARAMS((float));
extern float floorf _PARAMS((float));

#ifndef _REENT_ONLY
extern float acosf _PARAMS((float));
extern float asinf _PARAMS((float));
extern float atan2f _PARAMS((float, float));
extern float coshf _PARAMS((float));
extern float sinhf _PARAMS((float));
extern float expf _PARAMS((float));
extern float ldexpf _PARAMS((float, int));
extern float logf _PARAMS((float));
extern float log10f _PARAMS((float));
extern float powf _PARAMS((float, float));
extern float sqrtf _PARAMS((float));
extern float fmodf _PARAMS((float, float));
#endif /* ! defined (_REENT_ONLY) */

/* Other single precision functions.  */

extern float nanf (const char *);
extern float copysignf _PARAMS((float, float));
extern float scalbnf _PARAMS((float, int));
extern float expm1f _PARAMS((float));

#ifndef _REENT_ONLY
extern float hypotf _PARAMS((float, float));
#endif /* ! defined (_REENT_ONLY) */




/* self minilibc define below */
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef NAN
#define NAN (__builtin_nanf (""))
#endif

#ifndef isnan
#define isnan(x) __builtin_isnan (x)
#endif

#ifndef isfinite
#define isfinite(x) __builtin_isfinite (x)
#endif

_END_STD_C

#endif /* _MATH_H_ */
