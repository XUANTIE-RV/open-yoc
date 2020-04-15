/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AOICORE_H__
#define __AOICORE_H__

#include "icore/aoicore_typedef.h"

__BEGIN_DECLS__

typedef struct aoicore aoicore_t;

/**
 * @brief  init the icore audio output
 * @return 0/-1
 */
int aoicore_init();

/**
 * @brief  open/create one audio remote-ipc output
 * @param  [in] ash
 * @return NULL on error
 */
aoicore_t* aoicore_open(const aoih_t *ash);

/**
* @brief  start the audio output
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_start(aoicore_t *hdl);

/**
* @brief  stop the audio output
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_stop(aoicore_t *hdl);

/**
* @brief  drain out the tail pcm
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_drain(aoicore_t *hdl);

/**
 * @brief  write data to audio out
 * @param  [in] hdl
 * @param  [in] buf
 * @param  [in] count
 * @return -1 when err
 */
int aoicore_write(aoicore_t *hdl, const uint8_t *buf, size_t count);

/**
 * @brief  close/destroy audio remote-ipc output
 * @param  [in] hdl
 * @return 0/-1
 */
int aoicore_close(aoicore_t *hdl);

/**
 * @brief  set the volume(0-100)
 * @param  [in] vol
 * @return 0/-1
 */
int volicore_set(size_t vol);

__END_DECLS__

#endif /* __AOICORE_H__ */

