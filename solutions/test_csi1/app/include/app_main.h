/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

//#include <cJSON.h>
#include <uservice/eventid.h>
//#include <yoc/aui_cloud.h>
//#include <app_aui_cloud.h>

extern long long    g_lpm_asr_tag;

/* app player */
void app_player_init(void);
void app_volume_inc(int notify);
void app_volume_dec(int notify);
void app_volume_set(int vol, int notify);
void app_volume_mute(void);
int app_player_play(int type, const char *url, int resume);
void app_player_pause(void);
void app_player_stop(void);
void app_player_resume(void);
void app_player_reverse(void);
int  app_player_get_mute_state(void);
int app_player_is_busy(void);
int app_player_lpm(int state);

int app_mic_is_busy(void);

/* button*/
void app_button_init(void);

/* led */
void led_test_task(void);

/* wifi & net */
typedef enum {
    MODE_WIFI_TEST = -2,
    MODE_WIFI_CLOSE = -1,
    MODE_WIFI_NORMAL = 0,
    MODE_WIFI_PAIRING = 1
}wifi_mode_e;
wifi_mode_e app_network_init(void);
int wifi_connecting();
void wifi_lpm_enable(int lpm_en);
void wifi_set_lpm(int lpm_on);
void wifi_pair_start(void);
int wifi_internet_is_connected();
int wifi_is_pairing();
int wifi_getmac(uint8_t mac[6]);

/* rtc */
void app_rtc_init(void);
void app_rtc_from_system(void);
void app_rtc_to_system(void);

/* fota */
void app_fota_start(void);
void app_fota_init(void);
int app_fota_is_running(void);
void app_fota_do_check(void);
void app_fota_set_auto_check(int enable);
int app_fota_is_downloading(void);

void cpu1_init(void);
void cpu2_init(void);
void speaker_init(void);
void speaker_uninit(void);

void app_lpm_init(void);
int lpm_check(void);
void lpm_update(void);

int app_aui_init(void);

void app_rec_start(const char *url, const char *save_name);
void app_rec_stop(void);
void app_rec_copy_data(int index, uint8_t *data, uint32_t size);

/* event id define */
#define EVENT_NTP_RETRY_TIMER       (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER       (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS       (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT     (EVENT_USER + 4)

#define EVENT_LPM_CHECK             (EVENT_USER + 11)

#endif