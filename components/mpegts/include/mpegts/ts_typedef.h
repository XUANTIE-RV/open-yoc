/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_TYPEDEF_H__
#define __TS_TYPEDEF_H__

#include <mpegts/ts_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PID_PAT                    (0x0000)
#define PID_CAT                    (0x0001)
#define PID_NULL                   (0x1FFF)

/* table id */
#define TID_PAT                    (0x00)
#define TID_PMT                    (0x02)

#define TS_PACKET_SIZE             (188)

#define TS_HEADER_SIZE             (4)
#define PAT_HEADER_SIZE            (8)
#define PMT_ELEMENT_HEADER_SIZE    (5)
#define PMT_HEADER_SIZE            (12)
#define PES_HEADER_SIZE            (9)
#define ADAPT_HEADER_SIZE          (2)

#define STREAM_ID_AUDIO_STREAM     (0xc0)
#define STREAM_ID_VIDEO_STREAM     (0xe0)

#define TS_STREAM_UNKNOWN          (0x0)
#define TS_STREAM_AUDIO            (0x1)
#define TS_STREAM_VIDEO            (0x2)

typedef enum {
    ES_TYPE_UNKNOWN                = 0x0,
    ES_TYPE_AUDIO_AAC              = 0x0f,
    ES_TYPE_VIDEO_H264             = 0x1b,
    ES_TYPE_VIDEO_H265             = 0x24
} es_type_t;

typedef enum {
    TS_FILTER_TYPE_UNKOWN,
    TS_FILTER_TYPE_PES,
    TS_FILTER_TYPE_SECTION,
} tsf_type_t;

typedef enum {
    ADAPTATION_FIELD_TYPE_INVALID  = 0x0,
    ADAPTATION_FIELD_TYPE_PAYLOAD  = 0x01,
    ADAPTATION_FIELD_TYPE_ADAPTION = 0x02,
    ADAPTATION_FIELD_TYPE_BOTH     = 0x03
} adaptation_field_type_t;

#ifdef __cplusplus
}
#endif

#endif /* __TS_TYPEDEF_H__ */

