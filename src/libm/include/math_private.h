/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 * $NetBSD: math_private.h,v 1.11 2001/02/21 18:09:26 bjh21 Exp $
 */

#ifndef _MATH_PRIVATE_H_
#define _MATH_PRIVATE_H_

typedef unsigned int uint32_t;
typedef unsigned int u_int32_t;
typedef signed   int int32_t;
typedef unsigned int u_int;

#ifdef __i386__
typedef long long          quad_t;
typedef unsigned long long u_quad_t;
#else
typedef long               quad_t;
typedef unsigned long      u_quad_t;
#endif

#define CHAR_BIT 8
#define UINT_MAX        __INT_MAX__
#define ERANGE   34
#define LITTLE_ENDIAN 1234
#define BYTE_ORDER LITTLE_ENDIAN
#define _IEEE_LIBM

extern int errno;

#define SNG_EXPBITS         8
#define SNG_FRACBITS        23
#define SNG_EXP_BIAS        127
#define SNG_EXP_INFNAN      255

#define DBL_EXPBITS         11
#define DBL_FRACHBITS       20
#define DBL_FRACLBITS       32
#define DBL_FRACBITS        (DBL_FRACHBITS + DBL_FRACLBITS)
#define DBL_EXP_BIAS        1023
#define DBL_EXP_INFNAN      2047

struct ieee_single {
#if _BYTE_ORDER == _BIG_ENDIAN
  u_int   sng_sign:1;
  u_int   sng_exp:SNG_EXPBITS;
  u_int   sng_frac:SNG_FRACBITS;
#else
  u_int   sng_frac:SNG_FRACBITS;
  u_int   sng_exp:SNG_EXPBITS;
  u_int   sng_sign:1;
#endif
};

struct ieee_double {
#if _BYTE_ORDER == _BIG_ENDIAN
  u_int   dbl_sign:1;
  u_int   dbl_exp:DBL_EXPBITS;
  u_int   dbl_frach:DBL_FRACHBITS;
  u_int   dbl_fracl:DBL_FRACLBITS;
#else
  u_int   dbl_fracl:DBL_FRACLBITS;
  u_int   dbl_frach:DBL_FRACHBITS;
  u_int   dbl_exp:DBL_EXPBITS;
  u_int   dbl_sign:1;
#endif
};

union ieee_single_u {
  float           sngu_f;
  struct ieee_single  sngu_sng;
};

union ieee_double_u {
  double          dblu_d;
  struct ieee_double  dblu_dbl;
};


/* The original fdlibm code used statements like:
	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
	ix0 = *(n0+(int*)&x);			* high word of x *
	ix1 = *((1-n0)+(int*)&x);		* low word of x *
   to dig two 32 bit words out of the 64 bit IEEE floating point
   value.  That is non-ANSI, and, moreover, the gcc instruction
   scheduler gets it wrong.  We instead use the following macros.
   Unlike the original code, we determine the endianness at compile
   time, not at run time; I don't see much benefit to selecting
   endianness at run time.  */

/* A union which permits us to convert between a double and two 32 bit
   ints.  */

/*
 * The ARM ports are little endian except for the FPA word order which is
 * big endian.
 */

#if (BYTE_ORDER == BIG_ENDIAN) || (defined(__arm__) && !defined(__VFP_FP__))
typedef union
{
  double value;
  struct
  {
    u_int32_t msw;
    u_int32_t lsw;
  } parts;
} ieee_double_shape_type;
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN) && !(defined(__arm__) && !defined(__VFP_FP__))
typedef union
{
  double value;
  struct
  {
    u_int32_t lsw;
    u_int32_t msw;
  } parts;
} ieee_double_shape_type;
#endif

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union
{
  float value;
  u_int32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

/* ieee style elementary functions */
extern double __ieee754_sqrt(double);
extern double __ieee754_acos(double);
extern double __ieee754_acosh(double);
extern double __ieee754_log(double);
extern double __ieee754_log2(double);
extern double __ieee754_atanh(double);
extern double __ieee754_asin(double);
extern double __ieee754_atan2(double,double);
extern double __ieee754_exp(double);
extern double __ieee754_cosh(double);
extern double __ieee754_fmod(double,double);
extern double __ieee754_pow(double,double);
extern double __ieee754_lgamma_r(double,int *);
extern double __ieee754_gamma_r(double,int *);
extern double __ieee754_lgamma(double);
extern double __ieee754_gamma(double);
extern double __ieee754_log10(double);
extern double __ieee754_sinh(double);
extern double __ieee754_hypot(double,double);
extern double __ieee754_j0(double);
extern double __ieee754_j1(double);
extern double __ieee754_y0(double);
extern double __ieee754_y1(double);
extern double __ieee754_jn(int,double);
extern double __ieee754_yn(int,double);
extern double __ieee754_remainder(double,double);
extern int    __ieee754_rem_pio2(double,double*);
extern double __ieee754_scalb(double,double);

/* fdlibm kernel function */
extern double __kernel_standard(double,double,int);
extern double __kernel_sin(double,double,int);
extern double __kernel_cos(double,double);
extern double __kernel_tan(double,double,int);
extern int    __kernel_rem_pio2(double*,double*,int,int,int,const int*);


/* ieee style elementary float functions */
extern float __ieee754_sqrtf(float);
extern float __ieee754_acosf(float);
extern float __ieee754_acoshf(float);
extern float __ieee754_logf(float);
extern float __ieee754_log2f(float);
extern float __ieee754_atanhf(float);
extern float __ieee754_asinf(float);
extern float __ieee754_atan2f(float,float);
extern float __ieee754_expf(float);
extern float __ieee754_coshf(float);
extern float __ieee754_fmodf(float,float);
extern float __ieee754_powf(float,float);
extern float __ieee754_lgammaf_r(float,int *);
extern float __ieee754_gammaf_r(float,int *);
extern float __ieee754_lgammaf(float);
extern float __ieee754_gammaf(float);
extern float __ieee754_log10f(float);
extern float __ieee754_sinhf(float);
extern float __ieee754_hypotf(float,float);
extern float __ieee754_j0f(float);
extern float __ieee754_j1f(float);
extern float __ieee754_y0f(float);
extern float __ieee754_y1f(float);
extern float __ieee754_jnf(int,float);
extern float __ieee754_ynf(int,float);
extern float __ieee754_remainderf(float,float);
extern int   __ieee754_rem_pio2f(float,float*);
extern float __ieee754_scalbf(float,float);

/* float versions of fdlibm kernel functions */
extern float __kernel_sinf(float,float,int);
extern float __kernel_cosf(float,float);
extern float __kernel_tanf(float,float,int);
extern int   __kernel_rem_pio2f(float*,float*,int,int,int,const int*);

extern int signgam;

#endif /* _MATH_PRIVATE_H_ */
