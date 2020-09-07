/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_PALYER_H_
#define  _APP_PALYER_H_

void app_player_init(void);
void app_volume_inc(int notify);
void app_volume_dec(int notify);
void app_volume_set(int vol, int notify);
void app_volume_mute(void);
void app_player_pause(void);
void app_player_stop(void);
void app_player_resume(void);
void app_player_reverse(void);
int app_player_get_mute_state(void);
int app_player_get_status(void);
int app_player_is_busy(void);
#endif
