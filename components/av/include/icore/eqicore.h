/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQICORE_H__
#define __EQICORE_H__

#include "aef/eq_cls.h"

__BEGIN_DECLS__

typedef struct eqicore eqicore_t;

/**
 * @brief  init the icore equalizer
 * @return 0/-1
 */
int eqicore_init();

/**
 * @brief  alloc a equalizer
 * @param  [in] rate
 * @param  [in] eq_segments : eq filter segment number max
 * @return NULL on error
 */
eqicore_t* eqicore_new(uint32_t rate, uint8_t eq_segments);

/**
 * @brief  control whole eq filter enable/disable
 * @param  [in] hdl
 * @param  [in] enable
 * @return 0/-1
 */
int eqicore_set_enable(eqicore_t *hdl, uint8_t enable);

/**
 * @brief  set config param to the filter segment
 * @param  [in] hdl
 * @param  [in] segid : id of the filter segment
 * @param  [in] param
 * @return
 */
int eqicore_set_param(eqicore_t *hdl, uint8_t segid, const eqfp_t *param);

/**
 * @brief  process samples
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int eqicore_process(eqicore_t *hdl, const int16_t *in, int16_t *out, size_t nb_samples);

/**
 * @brief  free the equalizer
 * @param  [in] hdl
 * @return 0/-1
 */
int eqicore_free(eqicore_t *hdl);

__END_DECLS__

#endif /* __EQICORE_H__ */

