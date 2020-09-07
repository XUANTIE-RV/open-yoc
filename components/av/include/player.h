/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <aos/aos.h>
#include "stream/stream.h"
#include "stream/stream_all.h"
#include "avformat/avformat_all.h"
#include "avcodec/avcodec_all.h"
#include "output/output_all.h"
#include "aef/eq_all.h"
#include "aef/aef_all.h"
#include "swresample/resample_all.h"
#include "player_ioctl.h"

__BEGIN_DECLS__

enum {
    PLAYER_EVENT_UNKNOWN,
    PLAYER_EVENT_ERROR,
    PLAYER_EVENT_START,
    PLAYER_EVENT_FINISH,
};

typedef struct player_cb player_t;

/**
* @brief  player event callback for user
* @param  [in] player
* @param  [in] event : PLAYER_EVENT_XXX
* @param  [in] data  : reserved
* @param  [in] len   : reserved
*/
typedef void (*player_event_t)(player_t *player, uint8_t event, const void *data, uint32_t len);

typedef struct {
    uint64_t                  duration;      ///< ms, maybe a dynamic time
    uint64_t                  curtime;       ///< ms, current time
} play_time_t;

typedef struct {
    track_info_t              *tracks;       ///< for fulture use, don't need free for user
    size_t                    size;          ///< media file size
    uint64_t                  bps;
    uint64_t                  duration;      ///< ms, media duration
} media_info_t;

typedef struct player_conf {
    char                      *ao_name;      ///< ao name
    uint8_t                   vol_en;        ///< soft vol scale enable
    uint8_t                   vol_index;     ///< soft vol scale index (0~255)
    uint8_t                   eq_segments;   ///< equalizer segments number. 0 means don't need eq. TODO: not support yet!
    uint8_t                   *aef_conf;     ///< config data for aef
    size_t                    aef_conf_size; ///< size of the config data for aef
    uint32_t                  resample_rate; ///< none zereo means need to resample
    uint32_t                  rcv_timeout;   ///< timeout for recv stream. used inner default timeout when 0
    uint32_t                  cache_size;    ///< size of the web cache. 0 use default
    uint32_t                  cache_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint32_t                  period_ms;     ///< period cache size(ms) for audio out. 0 means use default
    uint32_t                  period_num;    ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default
    get_decrypt_cb_t          get_dec_cb;    ///< used for get decrypt info
    player_event_t            event_cb;      ///< callback of the player event
} ply_conf_t;

/**
 * @brief  init player module
 * @return 0/-1
 */
int player_init();

/**
 * @brief  init the player config param
 * @param  [in] ply_cnf
 * @return 0/-1
 */
int player_conf_init(ply_conf_t *ply_cnf);

/**
 * @brief  new a player obj
 * @param  [in] ply_cnf
 * @return NULL on error
 */
player_t* player_new(const ply_conf_t *ply_cnf);

/**
 * @brief  control/config the player by command
 * @param  [in] player
 * @param  [in] cmd    : PLAYER_CMD_XXX
 * @param  [in] ...
 * @return
 */
int player_ioctl(player_t *player, int cmd, ...);

/**
 * @brief  player play interface
 * @param  [in] player
 * @param  [in] url        : example: http://ip:port/xx.mp3
 * @param  [in] start_time : begin play time, ms
 * @return 0/-1
 */
int player_play(player_t *player, const char *url, uint64_t start_time);

/**
 * @brief  pause the player
 * @param  [in] player
 * @return 0/-1
 */
int player_pause(player_t *player);

/**
 * @brief  resume the player
 * @param  [in] player
 * @return 0/-1
 */
int player_resume(player_t *player);

/**
 * @brief  stop the player
 * @param  [in] player
 * @return 0/-1
 */
int player_stop(player_t *player);

/**
 * @brief  free/destroy the player obj
 * @param  [in] player
 * @return 0/-1
 */
int player_free(player_t *player);

/**
 * @brief  seek to the time
 * @param  [in] player
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int player_seek(player_t *player, uint64_t timestamp);

/**
 * @brief  get current play time
 * @param  [in] player
 * @param  [in/out] ptime
 * @return 0/-1
 */
int player_get_cur_ptime(player_t *player, play_time_t *ptime);

/**
 * @brief  get media info
 * @param  [in] player
 * @param  [in/out] minfo
 * @return 0/-1
 */
int player_get_media_info(player_t *player, media_info_t *minfo);

/**
 * @brief  get soft vol index of the player
 * @param  [in] player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int player_get_vol(player_t *player, uint8_t *vol);

/**
 * @brief  set soft vol index of the player
 * @param  [in] player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int player_set_vol(player_t *player, uint8_t vol);

__END_DECLS__

#endif /* __PLAYER_H__ */

