/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <cJSON.h>
#include <yoc/eventid.h>
#include <yoc/aui_cloud.h>

/* ai cmd */
extern aui_cmd_set_t g_aui_cmd_process;

/* ai engine */
extern aui_t        g_aui_handler;

cJSON *xunfei_nlp_find(cJSON *array, const char *key, const char *value);
cJSON *xunfei_nlp_find_service(cJSON *service_array, const char *service_name);
cJSON *xunfei_nlp_get_first_intent(cJSON *js);

int aui_nlp_process_pre_check(cJSON *js, const char *json_text);
int aui_nlp_process_music_nlp(cJSON *js, const char *json_text);
int aui_nlp_process_music_url(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_tts_nlp(cJSON *js, const char *json_text);
int aui_nlp_process_cmd(cJSON *js, const char *json_text);
int aui_nlp_process_baidu_asr(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_textcmd(cJSON *js, const char *json_text);
int aui_nlp_process_xf_rtasr(cJSON *js, const char *json_text);
int aui_nlp_process_proxy_ws_asr(cJSON *js, const char *json_text);
int aui_nlp_process_aliyun_rasr(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_story_nlp(cJSON *js, const char *json_text);

void app_text_cmd_init(void);
/* app player */
void app_player_init(void);
void app_volume_inc(int notify);
void app_volume_dec(int notify);
void app_volume_set(int vol, int notify);
void app_volume_mute(void);
void app_player_pause(void);
void app_player_stop(void);
void app_player_resume(void);
void app_player_reverse(void);
int  app_player_get_mute_state(void);

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


/* event id define */
#define EVENT_NTP_RETRY_TIMER       (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER       (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS       (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT     (EVENT_USER + 4)

#endif