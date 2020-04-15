/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFTICORE_H__
#define __FFTICORE_H__

#include <aos/types.h>
#include <aos/aos.h>

__BEGIN_DECLS__

typedef struct ffticore ffticore_t;

/**
 * @brief  init the icore inter-core fft
 * @return 0/-1
 */
int ffticore_init();

/**
 * @brief  alloc a inter-core fft
 * @param  [in] size : N of the fft
 * @return NULL on error
 */
ffticore_t* ffticore_new(size_t size);

/**
 * @brief  fft forward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int ffticore_forward(ffticore_t *hdl, const int16_t *in, int16_t *out);

/**
 * @brief  fft backward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int ffticore_backward(ffticore_t *hdl, const int16_t *in, int16_t *out);

/**
 * @brief  free the inter-core fft
 * @param  [in] hdl
 * @return 0/-1
 */
int ffticore_free(ffticore_t *hdl);

__END_DECLS__

#endif /* __FFTICORE_H__ */

