/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _UIENTRY_MAIN_H_
#define _UIENTRY_MAIN_H_

#include <lvgl.h>

#define USER_WIFI_SSID_LEN_MAX 128
#define USER_WIFI_PSK_LEN_MAX 128
#define ACTIVE_TRACK_CNT 3
/*****include*****/

// #include <devices/hal/display_impl.h>
/*****IMG SRC*****/
LV_IMG_DECLARE(img_bg);
LV_IMG_DECLARE(img_music);
LV_IMG_DECLARE(img_device);
LV_IMG_DECLARE(img_scene);
LV_IMG_DECLARE(img_setting);
LV_IMG_DECLARE(img_homethead);
LV_IMG_DECLARE(img_homewifistatus);
LV_IMG_DECLARE(img_homewifistatus_on);
LV_IMG_DECLARE(img_settingthead);
LV_IMG_DECLARE(img_close);
LV_IMG_DECLARE(img_close_black);
LV_IMG_DECLARE(img_voice);
LV_IMG_DECLARE(img_record);
LV_IMG_DECLARE(img_music_rnd);
LV_IMG_DECLARE(img_lv_demo_music_btn_play);
LV_IMG_DECLARE(img_music_next);
LV_IMG_DECLARE(img_lv_demo_music_btn_pause);
LV_IMG_DECLARE(img_music_prev);
LV_IMG_DECLARE(img_foodlight_off);
LV_IMG_DECLARE(img_foodlight_on);
LV_IMG_DECLARE(img_wifisw_off);
LV_IMG_DECLARE(img_wifisw_on);
LV_IMG_DECLARE(img_livingled_off);
LV_IMG_DECLARE(img_livingled_on);
LV_IMG_DECLARE(img_tmall);
LV_IMG_DECLARE(img_musicprev);
LV_IMG_DECLARE(img_musicnext);
LV_IMG_DECLARE(img_musicplay);
LV_IMG_DECLARE(img_musicpause);
LV_IMG_DECLARE(img_bedroom_0);
LV_IMG_DECLARE(img_bedroom_20);
LV_IMG_DECLARE(img_bedroom_40);
LV_IMG_DECLARE(img_bedroom_60);
LV_IMG_DECLARE(img_bedroom_80);
LV_IMG_DECLARE(img_bedroom_100);
LV_IMG_DECLARE(img_colon);

LV_IMG_DECLARE(img_lv_demo_music_wave_top);
LV_IMG_DECLARE(img_lv_demo_music_wave_bottom);
LV_IMG_DECLARE(img_wifi_on);
LV_IMG_DECLARE(img_lightbrightness);
LV_IMG_DECLARE(img_wifi_off);
LV_IMG_DECLARE(img_ble);
LV_IMG_DECLARE(img_fotaupate);
LV_IMG_DECLARE(img_aliyun);
LV_IMG_DECLARE(img_information);
LV_IMG_DECLARE(img_switch_on);
LV_IMG_DECLARE(img_switch_off);
LV_IMG_DECLARE(img_curtain_on);
LV_IMG_DECLARE(img_curtain_off);
LV_IMG_DECLARE(img_light_0);
LV_IMG_DECLARE(img_light_20);
LV_IMG_DECLARE(img_light_40);
LV_IMG_DECLARE(img_light_60);
LV_IMG_DECLARE(img_light_80);
LV_IMG_DECLARE(img_light_100);
LV_IMG_DECLARE(img_windows_on);
LV_IMG_DECLARE(img_windows_off);
LV_IMG_DECLARE(img_enjoy);
LV_IMG_DECLARE(img_next);
LV_IMG_DECLARE(img_date);
LV_IMG_DECLARE(img_lv_demo_music_cover_1);
LV_IMG_DECLARE(img_lv_demo_music_cover_2);
LV_IMG_DECLARE(img_lv_demo_music_cover_3);
LV_IMG_DECLARE(img_thead_3);
LV_IMG_DECLARE(img_thead_4);
LV_IMG_DECLARE(img_thead_1);
LV_IMG_DECLARE(img_thead_2);
LV_IMG_DECLARE(img_thead_setting);
LV_IMG_DECLARE(img_occ);
LV_IMG_DECLARE(img_scan_on);
LV_IMG_DECLARE(img_scan_off);
LV_IMG_DECLARE(img_wifi_L);
LV_IMG_DECLARE(img_stayhome);
LV_IMG_DECLARE(img_leavehome);
LV_IMG_DECLARE(img_metting);
LV_IMG_DECLARE(img_film);
LV_IMG_DECLARE(img_music1);
LV_IMG_DECLARE(img_music2);
LV_IMG_DECLARE(img_music3);
LV_IMG_DECLARE(img_lightadd);
LV_IMG_DECLARE(img_lightreduce);
LV_IMG_DECLARE(img_lv_demo_music_icon_1);
LV_IMG_DECLARE(img_lv_demo_music_icon_2);
LV_IMG_DECLARE(img_lv_demo_music_icon_3);
LV_IMG_DECLARE(img_lv_demo_music_icon_4);
LV_IMG_DECLARE(img_lv_demo_music_slider_knob);
/*****Font Src******/
LV_FONT_DECLARE(lv_d1s_font_22);
LV_FONT_DECLARE(lv_d1s_font_25);
LV_FONT_DECLARE(lv_d1s_font_30);
LV_FONT_DECLARE(lv_d1s_font_35);

/*****Function*****/
static inline lv_color_t lv_color_grey(void)
{
    return lv_color_make(0x69, 0x69, 0x69);
}

static inline lv_color_t lv_color_red(void)
{
    return lv_color_make(0xff, 0x00, 0x00);
}

void app_ui_event_init();
void app_ui_init();
#endif