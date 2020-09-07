/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/misc.h"
#include "swresample/pcm_convert.h"

#if 0
static void _swap_s16(void *buf, size_t size)
{
    int i;
    int16_t *b = buf;

    for (i = 0; i < size; i++) {
        b[i] = bswap16(b[i]);
    }
}
#endif

/**
 * @brief  s8_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t sample;
    int16_t *d      = dst;
    const int8_t *s = src;
    size_t size     = nb_samples;

    for (i = 0; i < size; i++) {
        sample = s[i] << 8;
        d[j++] = sample;
        d[j++] = sample;
    }
}

/**
 * @brief  s8_ch1_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch1_to_s16_ch1(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t sample;
    int16_t *d      = dst;
    const int8_t *s = src;
    size_t size     = nb_samples;

    for (i = 0; i < size; i++) {
        sample = s[i] << 8;
        d[j++] = sample;
    }
}

/**
 * @brief  u8_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void u8_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t sample;
    int16_t *d       = dst;
    const uint8_t *s = src;
    size_t size      = nb_samples;

    for (i = 0; i < size; i++) {
        sample  = s[i] << 8;
        sample -= 32768;
        d[j++]  = sample;
        d[j++]  = sample;
    }
}

/**
 * @brief  s8_ch2_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch2_to_s16_ch2(void *dst, const void *src, size_t nb_samples)
{
    int i;
    int16_t sample;
    int16_t *d      = dst;
    const int8_t *s = src;
    size_t size     = 2 * nb_samples;

    for (i = 0; i < size; i++) {
        sample = s[i] << 8;
        d[i]   = sample;
    }
}

/**
 * @brief  s8_ch2_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch2_to_s16_ch1(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t sample;
    int16_t *d      = dst;
    const int8_t *s = src;
    size_t size     = 2 * nb_samples;

    for (i = 0; i < size;) {
        sample = ((s[i] << 8) + (s[i + 1] << 8)) / 2;
        d[j++] = sample;
        i += 2;
    }
}

/**
 * @brief  u8_ch2_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void u8_ch2_to_s16_ch2(void *dst, const void *src, size_t nb_samples)
{
    int i;
    int16_t sample;
    int16_t *d      = dst;
    const int8_t *s = src;
    size_t size     = 2 * nb_samples;

    for (i = 0; i < size; i++) {
        sample  = s[i] << 8;
        sample -= 32768;
        d[i]    = sample;
    }
}

/**
 * @brief  s16_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s16_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t *d       = dst;
    const int16_t *s = src;
    size_t size      = nb_samples;

    for (i = 0; i < size; i++) {
        d[j++] = s[i];
        d[j++] = s[i];
    }
}

/**
 * @brief  s16_ch2_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s16_ch2_to_s16_ch1(void *dst, const void *src, size_t nb_samples)
{
    int i, j = 0;
    int16_t *d       = dst;
    const int16_t *s = src;
    size_t size      = 2 * nb_samples;

    for (i = 0; i < size;) {
        d[j++] = (s[i] + s[i + 1]) / 2;
        i += 2;
    }
}

/**
 * @brief  u16_le_to_s16_le
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] size : space(src) / sizeof(int16_t)
 * @return
 */
void u16_le_to_s16_le(void *dst, const void *src, size_t size)
{
    int i, j = 0;
    int sample;
    int16_t *d        = dst;
    const uint16_t *s = src;

    for (i = 0; i < size; i++) {
        sample  = s[i];
        sample -= 32768;
        d[j++]  = sample;
    }
}

/**
 * @brief  u16_be_to_s16_le
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] size : space(src) / sizeof(int16_t)
 * @return
 */
void u16_be_to_s16_le(void *dst, const void *src, size_t size)
{
    int i, j = 0;
    int sample;
    int16_t *d        = dst;
    const uint16_t *s = src;

    for (i = 0; i < size; i++) {
        sample  = bswap16(s[i]);
        sample -= 32768;
        d[j++]  = sample;
    }
}



