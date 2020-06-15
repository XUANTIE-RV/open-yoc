/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MISC_H__
#define __MISC_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define bswap16(x) \
	((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define bswap32(x) \
	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |	\
	 (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

static inline int is_file_url(const char *name)
{
    return strncmp(name, "file://", 7) == 0;
}

static inline int is_http_url(const char *name)
{
    return strncmp(name, "http://", 7) == 0;
}

static inline int is_mem_url(const char *name)
{
    return strncmp(name, "mem://", 6) == 0;
}

static inline int is_named_chunkfifo_url(const char *name)
{
    return strncmp(name, "chunkfifo://", 12) == 0;
}

static inline int is_url(const char *name)
{
    return is_file_url(name) || is_http_url(name) || is_mem_url(name) || is_named_chunkfifo_url(name);
}

/**
 * @brief  get codec id by tag val
 * @param  [in] tags
 * @param  [in] tag
 * @return
 */
avcodec_id_t get_codec_id(const avcodec_tag_t *tags, uint32_t tag);

/**
 * @brief  get codec name
 * @param  [in] codec id
 * @return "unknown" on error
 */
const char* get_codec_name(avcodec_id_t id);

/**
 * @brief  convert hex string to bytes
 * @param  [in] strhex : hex string
 * @param  [in] bytes
 * @param  [in] ibytes : length of the bytes
 * @return 0/-1
 */
int bytes_from_hex(const char *strhex, uint8_t *bytes, size_t ibytes);

/**
* @brief  clip a signed integer value into the -32768,32767(short)
* @param  [in] v
* @return
*/
int16_t clip_int16(int v);

/**
* @brief  extend the val to sign int
* @param  [in] val
* @param  [in] bits: bits of the val
* @return
*/
int sign_extend(int val, unsigned bits);

/**
 * @brief  four byte to str
 * @param  [in] val
 * @return not null
 */
char* four_2_str(uint32_t val);

/**
* @brief  crc8(x8+x2+x+1)
* @param  [in] data
* @param  [in] len
* @return 0 if ok
*/
uint8_t av_crc8(uint8_t *data, size_t len);

__END_DECLS__

#endif /* __MISC_H__ */

