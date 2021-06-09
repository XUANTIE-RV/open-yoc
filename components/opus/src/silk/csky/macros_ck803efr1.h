
#ifndef SILK_MACROS_CK803EFR1_H
#define SILK_MACROS_CK803EFR1_H

#undef silk_SMULWB
static OPUS_INLINE opus_int32 silk_SMULWB_ck803efr1(opus_int32 a, opus_int16 b)
{
  int res;
  __asm__(
      "#silk_SMULWB\n\t"
      "mulxl.s32 %0, %1, %2\n\t"
      : "=r"(res)
      : "r"(a), "r"(b)
  );
  return res;
}
#define silk_SMULWB(a, b) (silk_SMULWB_ck803efr1(a, b))

/* a32 + (b32 * (opus_int32)((opus_int16)(c32))) >> 16 output have to be 32bit int */
#undef silk_SMLAWB
static OPUS_INLINE opus_int32 silk_SMLAWB_ck803efr1(opus_int32 a, opus_int32 b,
 opus_int16 c)
{
  __asm__(
      "#silk_SMLAWB\n\t"
      "mulaxl.s32.s %0, %1, %2\n\t"
      : "+r"(a)
      : "r"(b), "r"(c)
  );
  return a;
}
#define silk_SMLAWB(a, b, c) (silk_SMLAWB_ck803efr1(a, b, c))

/* (a32 * (b32 >> 16)) >> 16 */
#undef silk_SMULWT
static OPUS_INLINE opus_int32 silk_SMULWT_ck803efr1(opus_int32 a, opus_int32 b)
{
  int res;
  __asm__(
      "#silk_SMULWT\n\t"
      "mulxh.s32 %0, %1, %2\n\t"
      : "=r"(res)
      : "r"(a), "r"(b)
  );
  return res;
}
#define silk_SMULWT(a, b) (silk_SMULWT_ck803efr1(a, b))

/* a32 + (b32 * (c32 >> 16)) >> 16 */
#undef silk_SMLAWT
static OPUS_INLINE opus_int32 silk_SMLAWT_ck803efr1(opus_int32 a, opus_int32 b,
 opus_int32 c)
{
  __asm__(
      "#silk_SMLAWT\n\t"
      "mulaxh.s32.s %0, %1, %2\n\t"
      : "+r"(a)
      : "r"(b), "r"(c)
  );
  return a;
}
#define silk_SMLAWT(a, b, c) (silk_SMLAWT_ck803efr1(a, b, c))

/* a32 + (opus_int32)((opus_int16)(b32)) * (opus_int32)((opus_int16)(c32)) output have to be 32bit int */
#undef silk_SMLABB
static OPUS_INLINE opus_int32 silk_SMLABB_ck803efr1(opus_int32 a, opus_int32 b,
 opus_int32 c)
{
  __asm__(
      "#silk_SMLABB\n\t"
      "mulall.s16.s %0, %1, %2\n\t"
      : "+r"(a)
      : "%r"(b), "r"(c)
  );
  return a;
}
#define silk_SMLABB(a, b, c) (silk_SMLABB_ck803efr1(a, b, c))

/* madd */
static OPUS_INLINE opus_int64 silk_MADD_ck803efr1(opus_int64 a, opus_int32 b, opus_int32 c)
{
  __asm__(
      "#silk_MADD\n\t"
      "mula.s32 %0, %1, %2\n\t"
      : "+r"(a)
      : "%r"(b), "r"(c)
  );
  return a;
}
static OPUS_INLINE opus_int64 csky_shilo(opus_int64 a, opus_int32 b)
{
  return (b >= 0) ? (a >> b) : (a << -b);
}

#endif
