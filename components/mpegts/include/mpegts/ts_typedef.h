/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_TYPEDEF_H__
#define __TS_TYPEDEF_H__

#include <string.h>
#include <inttypes.h>
#include <mpegts/ts_common.h>

#define SYNC_BYTE 0x47 // Sync Byte
#define STUFFING_BYTE 0xFF // Stuffing Byte
#define SPLIT_PROGRAM 0

typedef enum
{
    STREAM_ID_AUDIO_STREAM = 0xc0,
    STREAM_ID_VIDEO_STREAM = 0xe0,
} STREAM_ID_MEDIA_STREAM;

#define AUDIO_PACKET_ID(id)  ((id)==0xBD || ((id) >= 0xC0 && (id) <= 0xDF))
#define VIDEO_PACKET_ID(id)  ((id) >= 0xE0 && (id) <= 0xEF)

typedef enum
{
    PAT_PID = 0x0000,
    CAT_PID = 0x0001,
    TSDT_PID = 0x0002,
    IPMP_PID = 0x0003,
    //0x0004 - 0x000F   reversed
    //0x0010 -   0x1FFE may be assigned as network pid, program_map_pid, elementary_pid, or for other purposes
    //NIT, PMT, Media Channel PID
    NULL_PID = 0x1FFF,
} PACKET_ID;

typedef enum
{
    VIDEO_MPEG1 = 0x01,
    VIDEO_MPEG2 = 0x02,
    VIDEO_MPEG4 = 0x10,
    VIDEO_H264  = 0x1B,
    VIDEO_H265  = 0x24,

    AUDIO_MPEG1 = 0x03,
    AUDIO_MPEG2 = 0x04,
    AUDIO_ACC = 0x0F,
    AUDIO_AC3 = 0x81,
    AUDIO_G711 = 0x90,
    UNKNOWN = 0x00,
} ES_TYPE_ID;

#define AUDIO_ES_TYPE_ID(st) ((st)==ES_TYPE_ID::AUDIO_MPEG1 || \
                                      (st)==ES_TYPE_ID::AUDIO_MPEG2 || \
                                      (st)==ES_TYPE_ID::AUDIO_ACC   || \
                                      (st)==ES_TYPE_ID::AUDIO_AC3 || \
                                      (st)==ES_TYPE_ID::AUDIO_G711)
#define VIDEO_ES_TYPE_ID(st) ((st)==ES_TYPE_ID::VIDEO_MPEG1 || \
                                      (st)==ES_TYPE_ID::VIDEO_MPEG2 || \
                                      (st)==ES_TYPE_ID::VIDEO_MPEG4 || \
                                      (st)==ES_TYPE_ID::VIDEO_H264)

#define TS_PACKET_SIZE             (188)
#define TS_HEADER_SIZE             (4)
#define PAT_HEADER_SIZE            (8)
#define PMT_ELEMENT_HEADER_SIZE    (5)
#define PMT_HEADER_SIZE            (12)
#define PES_HEADER_SIZE            (9)
#define ADAPT_HEADER_SIZE          (2)

/* table id */
typedef enum
{
    /// Table Identifiers
    PAS_TABLE = 0x00, /// Program Association section contains a directory listing of all program map tables.  //PAT
    CAS_TABLE = 0x01, /// Conditional Access section contains a directory listing of all EMM streams.
    PMS_TABLE = 0x02, /// Program Map section contains a directory listing of all elementary streams. (PES)  //PMT
    TDS_TABLE = 0x03, /// Transport Stream Description section.
    MDS_TABLE = 0x06, /// Metadata section.
    NIL_TABLE = 0xFF, /// End of section.
} TABLE_ID;

typedef enum
{
    FIELD_TYPE_INVALID  = 0x0,
    FIELD_TYPE_PAYLOAD  = 0x01,
    FIELD_TYPE_ADAPTION = 0x02,
    FIELD_TYPE_BOTH     = 0x03
} ADAPTATION_FIELD_TYPE;

typedef enum
{
    TEST_PMT_PID = 0x1000,
    TEST_VIDEO_PID = 0x100,
    TEST_AUDIO_PID = 0x101,
} TEST_PACKET_PID;


#endif
