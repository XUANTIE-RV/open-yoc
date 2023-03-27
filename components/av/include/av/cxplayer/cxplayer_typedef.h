/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __CXPLAYER_TYPEDEF_H__
#define __CXPLAYER_TYPEDEF_H__

#include "av/avutil/common.h"
#include "av/avutil/av_typedef.h"
#include "av/avutil/eq_typedef.h"

__BEGIN_DECLS__

typedef enum {
    CXPLAYER_EVENT_UNKNOWN,
    CXPLAYER_EVENT_ERROR,
    CXPLAYER_EVENT_START,
    CXPLAYER_EVENT_FINISH,
    CXPLAYER_EVENT_UNDER_RUN,    ///< for stream-cache status
    CXPLAYER_EVENT_OVER_RUN,     ///< for stream-cache status
} cxplayer_event_t;

typedef enum {
    CXROTATE_DEGREE_0       = 0,
    CXROTATE_DEGREE_90      = 90,
    CXROTATE_DEGREE_180     = 180,
    CXROTATE_DEGREE_270     = 270,
} cxrotate_type_t;

enum cxplayer_status {
    CXPLAYER_STATUS_STOPED,
    CXPLAYER_STATUS_PLAYING,
    CXPLAYER_STATUS_PAUSED,
};

typedef enum {
    CXDISPLAY_LETTER_BOX,     /* use letterbox format */
    CXDISPLAY_PAN_SCAN,       /* use pan and scan format */
    CXDISPLAY_AUTO_FILLED,    /* use filled format */
} cxdisplay_format_t;

/**
* @brief  cxplayer event callback for user
* @param  [in] user_data
* @param  [in] event : CXPLAYER_EVENT_XXX
* @param  [in] data  : reserved
* @param  [in] len   : reserved
*/
typedef void (*cxplayer_eventcb_t)(void *user_data, uint8_t event, const void *data, uint32_t len);

typedef media_info_t cxmedia_info_t;
typedef play_time_t  cxplay_time_t;

typedef struct {
    uint32_t                  x;
    uint32_t                  y;
    uint32_t                  width;
    uint32_t                  height;
} cxwindow_t;

typedef struct {
    uint32_t                  web_cache_size;      ///< size of the web cache. 0 use default
    uint32_t                  web_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default

    uint8_t                   freezed;             ///< hold last pic when play stoped
} cxplayer_cnf_t;

__END_DECLS__

#endif /* __CXPLAYER_TYPEDEF_H__ */

