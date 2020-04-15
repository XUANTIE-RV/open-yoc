/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ADICORE_H__
#define __ADICORE_H__

#include "avutil/avframe.h"
#include "avutil/avpacket.h"
#include "icore/adicore_typedef.h"

__BEGIN_DECLS__

typedef struct adicore adicore_t;

/**
 * @brief  init the icore audio decoder
 * @return 0/-1
 */
int adicore_init();

/**
 * @brief  open/create one audio remote-ipc decoder
 * @param  [in] ash
 * @return NULL on error
 */
adicore_t* adicore_open(const adih_t *ash);

/**
* @brief  get the audio sample format
* @param  [in] hdl
* @param  [out] sf
* @return 0/-1
*/
int adicore_get_sf(adicore_t *hdl, sf_t *sf);

/**
 * @brief  decode one audio frame
 * @param  [in] hdl
 * @param  [out] frame : frame of pcm
 * @param  [out] got_frame : whether decode one frame
 * @param  [in]  pkt : data of encoded audio
 * @return -1 when err happens, otherwise the number of bytes consumed from the
 *         input avpacket is returned
 */
int adicore_decode(adicore_t *hdl, avframe_t *frame, int *got_frame, const avpacket_t *pkt);

/**
 * @brief  reset the adicore
 * @param  [in] hdl
 * @return 0/-1
 */
int adicore_reset(adicore_t *hdl);

/**
 * @brief  close/destroy audio remote-ipc decoder
 * @param  [in] hdl
 * @return 0/-1
 */
int adicore_close(adicore_t *hdl);

__END_DECLS__

#endif /* __ADICORE_H__ */

