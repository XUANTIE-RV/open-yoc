#ifndef FIXED_CSKY_H
#define FIXED_CSKY_H

/** 16x32 multiplication, followed by a 16-bit shift right. Results fits in 32 bits */
#undef MULT16_32_Q16
static OPUS_INLINE opus_val32 MULT16_32_Q16_csky(opus_val16 a, opus_val32 b)
{
  int res;
  __asm__(
      "#MULT16_32_Q16\n\t"
      "mulxl.s32 %0, %1, %2\n\t"
      : "=r"(res)
      : "r"(b),"r"(a)
  );
  return res;
}
#define MULT16_32_Q16(a, b) (MULT16_32_Q16_csky(a, b))


/** 16x32 multiplication, followed by a 15-bit shift right. Results fits in 32 bits */
#undef MULT16_32_Q15
static OPUS_INLINE opus_val32 MULT16_32_Q15_csky(opus_val32 a, opus_val32 b)
{
  int res;
  __asm__(
      "mul.s32 t0, %1, %2\n\t"
      "dexti %0, t0, t1, 15\n\t"
      : "=r"(res)
      : "r"(b), "r"(a)
      : "t0", "t1");
  return res;
}
#define MULT16_32_Q15(a, b) (MULT16_32_Q15_csky(a, b))

#undef MULT16_32_Q14
static OPUS_INLINE opus_val32 MULT16_32_Q14_csky(opus_val32 a, opus_val32 b)
{
  int res;
  __asm__(
      "mul.s32 t0, %1, %2\n\t"
      "dexti %0, t0, t1, 14\n\t"
      : "=r"(res)
      : "r"(b), "r"(a)
      : "t0", "t1");
  return res;
}
#define MULT16_32_Q14(a, b) (MULT16_32_Q14_csky(a, b))

/** 16x32 multiply, followed by a 15-bit shift right and 32-bit add.
    b must fit in 31 bits.
    Result fits in 32 bits. */
#undef MAC16_32_Q15
static OPUS_INLINE opus_val32 MAC16_32_Q15_csky(opus_val32 c, opus_val16 a,
 opus_val32 b)
{
  __asm__(
      "#MAC16_32_Q15\n\t"
      "mulaxl.s32.s %0, %1, %2;\n"
      : "+r"(c)
      : "r"(SHL32(b,1)), "r"(a)
  );
  return c;
}
#define MAC16_32_Q15(c, a, b) (MAC16_32_Q15_csky(c, a, b))

/** 16x32 multiply, followed by a 16-bit shift right and 32-bit add.
    Result fits in 32 bits. */
#undef MAC16_32_Q16
static OPUS_INLINE opus_val32 MAC16_32_Q16_csky(opus_val32 c, opus_val16 a,
 opus_val32 b)
{
  __asm__(
      "#MAC16_32_Q16\n\t"
      "mulaxl.s32.s %0, %1, %2;\n"
      : "+r"(c)
      : "r"(b), "r"(a)
  );
  return c;
}
#define MAC16_32_Q16(c, a, b) (MAC16_32_Q16_csky(c, a, b))

/** 16x16 multiply-add where the result fits in 32 bits */
#undef MAC16_16
static OPUS_INLINE opus_val32 MAC16_16_csky(opus_val32 c, opus_val16 a,
 opus_val16 b)
{
  __asm__(
      "#MAC16_16\n\t"
      "mulall.s16.s %0, %1, %2;\n"
      : "+r"(c)
      : "r"(a), "r"(b)
  );
  return c;
}
#define MAC16_16(c, a, b) (MAC16_16_csky(c, a, b))

/** 16x16 multiplication where the result fits in 32 bits */
#undef MULT16_16
static OPUS_INLINE opus_val32 MULT16_16_csky(opus_val16 a, opus_val16 b)
{
  int res;
  __asm__(
      "#MULT16_16\n\t"
      "mulll.s16 %0, %1, %2;\n"
      : "=r"(res)
      : "r"(a), "r"(b)
  );
  return res;
}
#define MULT16_16(a, b) (MULT16_16_csky(a, b))

#undef SIG2WORD16
static OPUS_INLINE opus_val16 SIG2WORD16_armv6(opus_val32 x)
{
   celt_sig res;
   __asm__(
       "#SIG2WORD16\n\t"
       "asri %0, %1, 12\n\t"
       "clipi.s32 %0, %0, 16\n\t"
       : "=&r"(res)
       : "r"(x+2048)
   );
   return EXTRACT16(res);
}
#define SIG2WORD16(x) (SIG2WORD16_armv6(x))


/* ==================LIKE MIPS BELOW================ */
#undef MULT16_32_P16
static inline int MULT16_32_P16(int a, int b)
{
    int c;

  __asm__(
      "mul.s32 t0, %1, %2\n\t"
      "dexti %0, t0, t1, 16\n\t"
      : "=r"(c)
      : "r"(b), "r"(a)
      : "t0", "t1");

    return c;
}

#undef MULT32_32_Q31
static inline int MULT32_32_Q31(int a, int b)
{
    int r;

  __asm__(
      "mul.s32 t0, %1, %2\n\t"
      "dexti %0, t0, t1, 31\n\t"
      : "=r"(r)
      : "r"(b), "r"(a)
      : "t0", "t1");

    return r;
}

#undef PSHR32
#define PSHR32(a,shift) (SHR32((a),(shift)))

#undef MULT16_16_P15
static inline int MULT16_16_P15(int a, int b)
{
    int r;

  __asm__(
      "mul.s32 t0, %1, %2\n\t"
      "dexti %0, t0, t1, 15\n\t"
      : "=r"(r)
      : "r"(b), "r"(a)
      : "t0", "t1");

    return r;
}

/* ==================LIKE MIPS END  ================ */


#endif
