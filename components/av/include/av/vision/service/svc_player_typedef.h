/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __SVC_PLAYER_TYPEDEF_H__
#define __SVC_PLAYER_TYPEDEF_H__

#include "av/xplayer/xplayer_typedef.h"

__BEGIN_DECLS__

typedef enum {
    SVC_PLAYER_EVENT_UNKNOWN,
    SVC_PLAYER_EVENT_ERROR,
    SVC_PLAYER_EVENT_START,
    SVC_PLAYER_EVENT_PAUSED,
    SVC_PLAYER_EVENT_RESUME,
    SVC_PLAYER_EVENT_STOPD,
    SVC_PLAYER_EVENT_FINISH,
    SVC_PLAYER_EVENT_UNDER_RUN,    ///< for stream-cache status
    SVC_PLAYER_EVENT_OVER_RUN,     ///< for stream-cache status
} svc_player_event_t;

enum svc_player_status {
    SVC_PLAYER_STATUS_STOPED,
    SVC_PLAYER_STATUS_PLAYING,
    SVC_PLAYER_STATUS_PAUSED,
};

/**
* @brief  svc_player event callback for user
* @param  [in] user_data
* @param  [in] event : SVC_PLAYER_EVENT_XXX
* @param  [in] data  : reserved
* @param  [in] len   : reserved
*/
typedef void (*svc_player_eventcb_t)(void *user_data, uint8_t event, const void *data, uint32_t len);

typedef struct player_service_config {
    //TODO:
    xwindow_t                 viewport;

    svc_player_eventcb_t      event_cb;            ///< callback of the svc_player event
    void                      *user_data;          ///< user data for event_cb
} player_service_cnf_t;

typedef struct {
    //TODO:
    xwindow_t                 win;

    uint64_t                  start_time;
    uint8_t                   freezed;             ///< hold last pic when play stoped
} svc_player_cnf_t;

__END_DECLS__

#endif /* __SVC_PLAYER_TYPEDEF_H__ */

