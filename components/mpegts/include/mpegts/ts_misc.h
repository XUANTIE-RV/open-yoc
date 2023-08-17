/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_MISC_H__
#define __TS_MISC_H__

#include <mpegts/ts_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief  crc32(x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1), CRC_32_IEEE
* @param  [in] data
* @param  [in] len
* @return
*/
uint32_t ts_crc32(const uint8_t *data, size_t len);

/**
* @brief  get stream ID by es type
* @param  [in] type
* @return 0 on error
*/
uint8_t ts_get_stream_id(ES_TYPE_ID type);

/**
 * @brief  whether audio by es type
 * @param  [in] type
 * @return 0/1
 */

uint64_t ts_read_pts(const uint8_t data[5]);

/**
* @brief  write pts or dts
* @param  [in] data[5]
* @param  [in] pts
* @param  [in] fill_flag
* @return
*/
void ts_write_pts(uint8_t data[5], uint64_t pts, uint32_t fill_flag);

/**
* @brief  write pcr adaption filed
* @param  [in] data[6]
* @param  [in] pcr
* @return
*/
void ts_write_pcr(uint8_t data[6], uint64_t pcr);

#ifdef __cplusplus
}
#endif

#endif /* __TS_MISC_H__ */