/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <yoc/fota.h>
#include <aos/kv.h>
#include <aos/cli.h>
#include <stdio.h>
#include <aos/aos.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <yoc/netmgr.h>
#include <devices/netdrv.h>
#include <lwip/netdb.h>
#include <arpa/inet.h>
#include <devices/netdrv.h>
#include <devices/wifi.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <wifi_prov.h>
#include <devices/display.h>
#include "wifi_config.h"
#include <gateway.h>
#include <math.h>
#include <smart_audio.h>
#include <av/avutil/vol_scale.h>
#include "app_player.h"
#include "bt/app_bt.h"
#include "event_mgr/app_event.h"
#include <ulog/ulog.h>
#include "app_net.h"
#include <devices/device.h>
#include <devices/display.h>
#include <devices/input.h>
#include <lvgl.h>
#include <lv_port_disp.h>
#include <lv_port_indev.h>

#include "ui_entry.h"

#ifndef CONFIG_UI_TASK_STACK_SIZE
#define CONFIG_UI_TASK_STACK_SIZE (65536)
#endif

#ifndef CONFIG_LVGL_IDLE_TIME
#define CONFIG_LVGL_IDLE_TIME 20
#endif

#define TAG "UI"

/*****Obj Src *****/
struct node_array {
    int  index;
    char pos[20];
} NODE[10]; // meshdev added infor
struct scan_array_type {
    gw_discovered_dev_t scan_array[10];
    int                 len;
} scandev_array; // meshdev scan infor

struct {
    int16_t index_array[10];
    int     len;
} index_def; // meshdev adding index

struct _updatestatus {
    gw_subdev_t subdev;
    uint8_t     onoff;
};                                                             // meshdev onoffstatus update
static gw_evt_discovered_info_t *scan_msg          = NULL;     // meshdev found event update
static gw_evt_subdev_add_t *     gw_evt_subdev_add = NULL;     // meshdev added indev update
extern rvm_hal_wifi_ap_record_t  g_app_wifi_user_scan_table[]; // wifi scanover ssid array
static lv_obj_t *                wifiscan_list;
static lv_obj_t *                scene_scr_array[4]; // scene control
static lv_obj_t *                mesh_scr;
static lv_obj_t *                mesh_scr_devctl_list;
static lv_obj_t *                music_scr;
static lv_obj_t *                scene_scr;
static lv_obj_t *                setting_scr;
static lv_obj_t *                layertop_scr_wifi;
static lv_obj_t *                mesh_scan_scr; // music scr
static lv_obj_t *                music_label_name;
static lv_obj_t *                music_label_author;
static lv_obj_t *                added_temp_obj = NULL; // revice before-laytop obj,use to del
static lv_obj_t *                del_temp_obj   = NULL;
static lv_obj_t **               mesh_dev_scan_obj;  // create scan obj
static lv_obj_t **               mesh_dev_added_obj; // create added obj
static lv_obj_t *                input_box;
static lv_obj_t *                setting_fota_scr;
static lv_obj_t *                pwd_ta = NULL;
static lv_obj_t *                ssid_label; // wifi ssid
static char                      user_ssid[USER_WIFI_SSID_LEN_MAX];
static char                      user_psk[USER_WIFI_PSK_LEN_MAX];
static uint32_t                  timeout = GW_DEV_DISCOVER_NO_TIMEOUT;
static lv_obj_t *                album_img_obj; // music scr
static lv_obj_t *                play_obj;
static lv_obj_t *                music_next;
static lv_obj_t *                music_prev;

static aos_mutex_t lvgl_mutex;

static lv_obj_t *meshadd_spinner;
static uint32_t  track_id;
static int       display_480p; // display 480p or 720p confirm
static lv_obj_t *wifilist_btn;
// static int disp_brightness = 200;                                //default display brightness
static lv_obj_t *wifilink_ssid;
static lv_obj_t *wifilink_statuslabel;
static lv_obj_t *wifiscan_btn;

static int       fotaupdate_progress = 0;
static lv_obj_t *fota_progresslabel;
static lv_obj_t *deviceonlinelabel;
static lv_obj_t *homewifistatus;
static lv_obj_t *home_scr;
/****Event***/
static void scene_scr_mode_event_cb(lv_event_t *e); // use for change home scr
static void
mesh_dev_added_downlist_typeselect_event_cb(lv_event_t *e); // mesh add scr downlist select type:开关、照明灯、窗帘
static void
mesh_scr_devctl_listbtn_event_cb(lv_event_t *e);   // device control list event,after select type to use control meshdev
static void home_scr_func_event_cb(lv_event_t *e); // change home scr:home/device/music/scene/setting
static void func_scr_img_close_event_cb(lv_event_t *e); // close scr
static void listbtn_imgbg_event_cb(lv_event_t *e);
static void listbtn_arc_event_cb(lv_event_t *e);
static void play_obj_event_click_cb(lv_event_t *e);           // music scr,start/stop music
static void setting_scr_wifibtn_event_cb(lv_event_t *e);      // setting wifibtn screen create
static void setting_scr_lightnessbtn_event_cb(lv_event_t *e); // setting display lightness
static void setting_scr_voice_event_cb(lv_event_t *e);
static void wifi_btn_clear_event_handler(lv_event_t *e);
static void setting_scr_meshbtn_event_cb(lv_event_t *e);
static void setting_scr_fotabtn_event_cb(lv_event_t *e);
static void setting_scr_information_btn_event_cb(lv_event_t *e);
static void ta_event_cb(lv_event_t *e); // texttrea event fot get text by keyboard
static void fota_ta_event_cb(lv_event_t *e);
static void msg_scr_btnok_event_cb(lv_event_t *e); // setting fotabtn,confirm/cancel btn
static void msg_scr_btncancel_event_cb(lv_event_t *e);
static void fotaen_switch_event_cb(lv_event_t *e);      // setting fotabtn, en/disable fota check
static void layertop_scr_close_event_cb(lv_event_t *e); // clean and hidden layertop
static void wifiscr_scr_close_event_cb(lv_event_t *e);
static void setting_mesh_scr_close_event_cb(lv_event_t *e); // close setting-meshbtn
static void mesh_scanbtn_event_cb(lv_event_t *e);           // setting meshbtn,scan meshdev
static void mesh_showbtn_event_cb(lv_event_t *e);           // setting meshbtn,show added meshdev information
static void mesh_dev_scan_obj_event_cb(lv_event_t *e);      // mesh start scan,close scr, add meshdev
static void mesh_dev_scan_obj_closebtn_event_cb(lv_event_t *e);
static void mesh_dev_scan_obj_addbtn_event_cb(lv_event_t *e);
static void mesh_dev_added_setting_obj_event_cb(lv_event_t *e); // mesh added scr create
static void mesh_dev_added_onoff_obj_event_cb(lv_event_t *e);   // mesh added scr,meshdev onoff
static void mesh_dev_added_close_event_cb(lv_event_t *e);       // mesh added scr close
static void mesh_dev_added_delbtn_event_cb(lv_event_t *e);      // mesh addded scr close
static void album_gesture_event_cb(lv_event_t *e);              // music song img create/change
static void next_click_event_cb(lv_event_t *e);                 // music icon change for prev/next song
static void prev_click_event_cb(lv_event_t *e);
// static void voicectrl_event_cb(lv_event_t * e);
// static void voiceslider_event_cb(lv_event_t * e);
static void ok_btn_event_cb(lv_event_t *e); // wifi scr add/cancel
static void exit_btn_event_cb(lv_event_t *e);
static void my_list_ssid_select_event_handler(lv_event_t *e);
static void my_btn_scan_event_handler(lv_event_t *e);
static void gw_main_event_handler(uint32_t event_id, const void *data, void *context); // subscribe to main event
static void setting_scr_lightnessslider_event_cb(lv_event_t *e);
static void setting_scr_voiceslider_event_cb(lv_event_t *e);

/*****timer src****/
static void meshdev_report_timer(lv_timer_t *timer); // scan meshdev,report/stop/create timer
static void meshdev_stop_timer(lv_timer_t *timer);
static void mesh_scan_dev_create_timer(lv_timer_t *timer);
static void meshadd_report_timer(lv_timer_t *timer); // added meshdev report
static void layersys_timer_cb(lv_timer_t *timer);    // layertop timer
static void clock_date_task_callback(lv_timer_t *timer);
/*****Func Src******/
static void      track_load(uint32_t id);             // music scr song change event
static void      lv_demo_music_album_next(bool next); // music scr song track_load id
static lv_obj_t *album_img_create(lv_obj_t *parent);
static void      set_img_angle(void *img, int32_t v); // music scr song img animation
// static void set_slider_value(void* slider, int32_t v);                                              //music scr song
// slider animation static void set_slidertime_value(void* label, int32_t v);
static void music_anim_status(bool status);
static int  check_wifi_ssid_name(void *name);

typedef struct _lv_clock {
    lv_obj_t *time_hour_label;    // 时
    lv_obj_t *time_minutes_label; //分
    lv_obj_t *date_label;         // 日
    lv_obj_t *weekday_label;      // 周
} lv_clock_t;

/******Main******/

static int meshonline_cnt()
{
    char kv_pos[32];
    char kv_index[10];
    int  cnt = 0;
    for (int i = 0; i < 11; i++) {
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            cnt++;
        }
    }
    return cnt;
}

static lv_clock_t lv_clock = { 0 };
static void clock_date_task_callback(lv_timer_t *timer)
{
    static time_t     unix_time;
    static struct tm *time_info;

    unix_time = time(NULL);
    time_info = localtime(&unix_time);

    int year    = time_info->tm_year + 1900;
    int month   = time_info->tm_mon + 1;
    int day     = time_info->tm_mday;
    int hour    = time_info->tm_hour;
    int minutes = time_info->tm_min;
    // int second = time_info->tm_sec;

    if (timer != NULL && timer->user_data != NULL) {
        lv_clock_t *clock = (lv_clock_t *)(timer->user_data);
        if (clock->time_hour_label != NULL) {
            lv_label_set_text_fmt(clock->time_hour_label, "#ffffff %02d", hour);
        }
        if (clock->time_minutes_label != NULL) {
            lv_label_set_text_fmt(clock->time_minutes_label, "#ffffff %02d", minutes);
        }
        if (clock->date_label != NULL) {
            lv_label_set_text_fmt(clock->date_label, "#ffffff %d/%02d/%02d#", year, month, day);
        }
    }
}

void gw_main(void)
{

    // //组
    // for(int i = 0; i < 10; i++)
    // {
    //     gateway_subgrp_del(i);
    // }

    // Display 480p or 720p confirm
    rvm_dev_t *disp_dev;
    disp_dev = rvm_hal_display_open("disp");
    rvm_hal_display_info_t disp_info;
    rvm_hal_display_get_info(disp_dev, &disp_info);
    if ((int)disp_info.x_res == 480 && (int)disp_info.y_res == 480) {
        display_480p = 1;
    } else {
        display_480p = 0;
    }
    rvm_hal_display_close(disp_dev);
    // Index init
    memset(index_def.index_array, 0, sizeof(u16_t) * 10);
    index_def.len = 0;

// loading anim
#if 0
//Loading Animation create
    lv_obj_t* obj_large = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_large, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(obj_large);
    lv_obj_clear_flag(obj_large, LV_OBJ_FLAG_SCROLLABLE);                                                   //clear scr flag:scrollable scrollbar bgcolor radius
    lv_obj_set_scrollbar_mode(obj_large, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(obj_large, lv_color_black(), 0);
    lv_obj_set_style_border_color(obj_large, lv_color_black(), 0);
    lv_obj_set_style_radius(obj_large, 0, 0);
    //Loading img create
    lv_obj_t* img = lv_img_create(obj_large);
    lv_img_set_src(img, &img_thead_1);
    lv_obj_center(img);
    //Loading animtion create
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, img);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_values(&anim, LV_IMG_ZOOM_NONE, 0);                                                         //set img zoom 0~256
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);                                                   //animation ease in out
    lv_anim_set_time(&anim, 1000);
    lv_anim_set_delay(&anim, 500);
    lv_anim_start(&anim);
    lv_obj_del_delayed(obj_large, 1700);
#endif

    // HOME SCREEN
    home_scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(home_scr, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(home_scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(home_scr, LV_OPA_100, 0);
    lv_obj_set_style_border_opa(home_scr, LV_OPA_0, 0);
    lv_obj_clear_flag(home_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(home_scr, 0, 0);
    if (display_480p) {
        lv_obj_set_style_pad_top(home_scr, LV_VER_RES / 20, 0);
        lv_obj_set_style_pad_left(home_scr, LV_HOR_RES / 20, 0);
    }
    // homescr wifistatus
    homewifistatus = lv_img_create(home_scr);
    lv_img_set_src(homewifistatus, &img_homewifistatus);
    lv_obj_align(homewifistatus, LV_ALIGN_TOP_RIGHT, 0, -10);
    lv_obj_add_flag(homewifistatus, LV_OBJ_FLAG_IGNORE_LAYOUT);
    // time date
    lv_clock.time_hour_label = lv_label_create(home_scr);
    lv_obj_set_style_text_font(lv_clock.time_hour_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(lv_clock.time_hour_label, true);
    lv_label_set_text(lv_clock.time_hour_label, "#ffffff 00");
    lv_obj_align_to(lv_clock.time_hour_label, home_scr, LV_ALIGN_TOP_LEFT, 40, 20);

    lv_obj_t *clock_colon = lv_img_create(home_scr);
    lv_img_set_src(clock_colon, &img_colon);
    lv_obj_align_to(clock_colon, lv_clock.time_hour_label, LV_ALIGN_LEFT_MID, 22, 0);

    lv_clock.time_minutes_label = lv_label_create(home_scr);
    lv_obj_set_style_text_font(lv_clock.time_minutes_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(lv_clock.time_minutes_label, true);
    lv_label_set_text(lv_clock.time_minutes_label, "#ffffff 00");
    lv_obj_align_to(lv_clock.time_minutes_label, lv_clock.time_hour_label, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

    lv_clock.date_label = lv_label_create(home_scr);
    lv_obj_set_style_text_font(lv_clock.date_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(lv_clock.date_label, true);
    lv_label_set_text(lv_clock.date_label, "#ffffff 2023/01/01");
    lv_obj_set_style_text_letter_space(lv_clock.date_label, -1, LV_PART_MAIN);
    lv_obj_align_to(lv_clock.date_label, clock_colon, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    // device online status
    lv_obj_t *deviceonline = lv_led_create(home_scr);
    lv_led_on(deviceonline);
    lv_obj_set_size(deviceonline, 10, 10);
    lv_led_set_color(deviceonline, lv_color_make(0x00, 0xff, 0x00));
    lv_obj_add_flag(deviceonline, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(deviceonline, LV_ALIGN_TOP_RIGHT, -170, 75);

    lv_timer_t *clock_timer = lv_timer_create(clock_date_task_callback, 10000, (void *)&lv_clock);
    lv_timer_set_repeat_count(clock_timer, -1);
    // device online label
    // deviceonlinelabel = onlinelabel_func(home_scr);
    // lv_obj_align_to(deviceonlinelabel, deviceonline, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    int deviceonline_cnt = meshonline_cnt();
    deviceonlinelabel    = lv_label_create(home_scr);
    lv_obj_set_style_text_font(deviceonlinelabel, &lv_d1s_font_25, 0);
    lv_label_set_recolor(deviceonlinelabel, true);
    lv_label_set_text_fmt(deviceonlinelabel, "#ffffff 设备在线  %d", deviceonline_cnt);
    lv_obj_align_to(deviceonlinelabel, deviceonline, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    // HOME_SCR layout GRID
    // lv_obj_set_grid_align(home_scr, LV_GRID_ALIGN_SPACE_BETWEEN, LV_GRID_ALIGN_END);
    // lv_coord_t home_scr_col_dsc[] = { 195, 200, LV_GRID_TEMPLATE_LAST};
    // lv_coord_t home_scr_row_dsc[] = { 150, 150, LV_GRID_TEMPLATE_LAST};
    // lv_obj_set_grid_dsc_array(home_scr, home_scr_col_dsc, home_scr_row_dsc);
    // home scr style
    static lv_style_t home_scr_style;
    lv_style_init(&home_scr_style);
    lv_style_set_shadow_color(&home_scr_style, lv_color_white());
    lv_style_set_shadow_width(&home_scr_style, 5);
    lv_style_set_bg_color(&home_scr_style, lv_color_black());
    lv_style_set_radius(&home_scr_style, 20);
    lv_style_set_bg_opa(&home_scr_style, LV_OPA_50);
    lv_style_set_border_opa(&home_scr_style, LV_OPA_0);

    // child scr style
    static lv_style_t child_scr_style;
    lv_style_init(&child_scr_style);
    lv_style_set_border_opa(&child_scr_style, LV_OPA_0);
    lv_style_set_bg_color(&child_scr_style, lv_color_black());
    lv_style_set_bg_opa(&child_scr_style, LV_OPA_100);

    /*HOME_SCR device contrl*/
    lv_obj_t *home_scr_mesh = lv_btn_create(home_scr);
    lv_obj_set_size(home_scr_mesh, 195, 150);
    lv_obj_align(home_scr_mesh, LV_ALIGN_LEFT_MID, 0, -25);
    lv_obj_clear_flag(home_scr_mesh, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(home_scr_mesh, &home_scr_style, 0);
    lv_obj_t *home_scr_mesh_label = lv_label_create(home_scr_mesh); // create scr label
    lv_obj_set_style_text_font(home_scr_mesh_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(home_scr_mesh_label, true);
    lv_label_set_text(home_scr_mesh_label, "#ffffff 设备");
    lv_obj_align(home_scr_mesh_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t *home_scr_mesh_img = lv_img_create(home_scr_mesh); // create scr img
    lv_img_set_src(home_scr_mesh_img, &img_device);
    lv_obj_align(home_scr_mesh_img, LV_ALIGN_BOTTOM_RIGHT, 35, 15);
    lv_obj_add_flag(home_scr_mesh_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        home_scr_mesh, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_mesh_label); // home scr event_cb
    lv_obj_add_event_cb(home_scr_mesh_img, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_mesh_label);

    // device contrl page
    mesh_scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mesh_scr, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(mesh_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(mesh_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(mesh_scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(mesh_scr, &child_scr_style, 0);
    // device contrl title
    lv_obj_t *mesh_scr_title = lv_obj_create(mesh_scr);
    lv_obj_set_size(mesh_scr_title, LV_HOR_RES, 90);
    lv_obj_clear_flag(mesh_scr_title, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(mesh_scr_title, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_opa(mesh_scr_title, LV_OPA_0, 0);
    lv_obj_set_style_radius(mesh_scr_title, 0, 0);
    lv_obj_align_to(mesh_scr_title, mesh_scr, LV_ALIGN_OUT_TOP_MID, 0, LV_VER_RES / 7);
    lv_obj_set_style_bg_opa(mesh_scr_title, LV_OPA_0, 0);
    // device contrl close img
    lv_obj_t *mesh_scr_img_close = lv_img_create(mesh_scr_title);
    lv_img_set_src(mesh_scr_img_close, &img_close);
    lv_obj_align(mesh_scr_img_close, LV_ALIGN_RIGHT_MID, -10, 10);
    lv_obj_add_flag(mesh_scr_img_close, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        mesh_scr_img_close, func_scr_img_close_event_cb, LV_EVENT_CLICKED, 0); // img close event,used to close meshscr
    // device contrl label
    lv_obj_t *mesh_scr_label = lv_label_create(mesh_scr_title);
    lv_obj_set_style_text_font(mesh_scr_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(mesh_scr_label, true);
    lv_label_set_text(mesh_scr_label, "#ffffff 设备列表");
    lv_obj_align(mesh_scr_label, LV_ALIGN_LEFT_MID, 0, 15);
    // device contrl main scr
    lv_obj_t *mesh_scr_main = lv_obj_create(mesh_scr);
    lv_obj_set_style_border_opa(mesh_scr_main, LV_OPA_0, 0);
    lv_obj_set_style_radius(mesh_scr_main, 0, 0);
    lv_obj_set_scroll_dir(mesh_scr_main, LV_DIR_VER);
    lv_obj_set_size(mesh_scr_main, LV_HOR_RES, 390);
    lv_obj_align_to(mesh_scr_main, mesh_scr_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(mesh_scr_main, LV_OPA_0, 0);
    lv_obj_clear_flag(mesh_scr_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(mesh_scr_main, LV_SCROLLBAR_MODE_OFF);
    // device contrl main scr list
    mesh_scr_devctl_list = lv_list_create(mesh_scr_main);
    lv_obj_set_size(mesh_scr_devctl_list, lv_pct(100), 405);
    lv_obj_set_scroll_dir(mesh_scr_devctl_list, LV_DIR_VER);
    lv_obj_align(mesh_scr_devctl_list, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(mesh_scr_devctl_list, &home_scr_style, 0);

    /*HOME_SCR_MUSIC*/
    lv_obj_t *home_scr_music = lv_btn_create(home_scr);
    lv_obj_set_size(home_scr_music, 200, 150);
    lv_obj_align(home_scr_music, LV_ALIGN_RIGHT_MID, -10, -25);
    lv_obj_clear_flag(home_scr_music, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(home_scr_music, &home_scr_style, 0);
    lv_obj_t *home_scr_music_label = lv_label_create(home_scr_music);
    lv_obj_set_style_text_font(home_scr_music_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(home_scr_music_label, true);
    lv_label_set_text(home_scr_music_label, "#ffffff 音乐");
    lv_obj_align(home_scr_music_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t *home_scr_music_img = lv_img_create(home_scr_music);
    lv_img_set_src(home_scr_music_img, &img_music);
    lv_obj_align(home_scr_music_img, LV_ALIGN_BOTTOM_RIGHT, 20, 5);
    lv_obj_add_flag(home_scr_music_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        home_scr_music_img, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_music_label); // home mussic scr
    lv_obj_add_event_cb(home_scr_music, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_music_label);
    // MUSIC main scr
    music_scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(music_scr, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(music_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(music_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(music_scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(music_scr, &child_scr_style, 0);
    // music title
    lv_obj_t *music_title = lv_label_create(music_scr);
    lv_obj_set_style_text_font(music_title, &lv_d1s_font_25, 0);
    lv_label_set_recolor(music_title, true);
    lv_label_set_text(music_title, "#ffffff 音乐播放器");
    lv_obj_align(music_title, LV_ALIGN_TOP_LEFT, 0, 10);
    // MUSIC_SCR img close
    lv_obj_t *music_scr_img_close = lv_img_create(music_scr);
    lv_img_set_src(music_scr_img_close, &img_close);
    lv_obj_align(music_scr_img_close, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_flag(music_scr_img_close, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(music_scr_img_close, func_scr_img_close_event_cb, LV_EVENT_CLICKED, 0); // close music scr

    // MUSIC_SCR song name&author
    music_label_name = lv_label_create(music_scr);
    // lv_obj_set_size(music_label_name, 240, 50);
    lv_obj_set_style_text_font(music_label_name, &lv_d1s_font_30, 0);
    lv_label_set_recolor(music_label_name, true);
    lv_obj_align(music_label_name, LV_ALIGN_TOP_MID, 0, 0);
    music_label_author = lv_label_create(music_scr);
    // lv_obj_set_size(music_label_author, 150, 50);
    lv_obj_set_style_text_font(music_label_author, &lv_d1s_font_25, 0);
    lv_label_set_recolor(music_label_author, true);
    lv_obj_align(music_label_author, LV_ALIGN_TOP_MID, 0, 40);

    // MUSIC_SCR control scr
    lv_obj_t *music_scr_control = lv_obj_create(music_scr);
    lv_obj_set_size(music_scr_control, LV_HOR_RES, 160);
    lv_obj_align(music_scr_control, LV_ALIGN_BOTTOM_MID, 0, 30);
    lv_obj_set_style_bg_opa(music_scr_control, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(music_scr_control, LV_OPA_0, 0);
    lv_obj_clear_flag(music_scr_control, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(music_scr_control, LV_SCROLLBAR_MODE_OFF);

    // MUSIC_SCR control play btn create
    play_obj = lv_img_create(music_scr_control);
    lv_img_set_src(play_obj, &img_musicplay);
    lv_obj_add_flag(play_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_grid_cell(play_obj, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_add_event_cb(play_obj, play_obj_event_click_cb, LV_EVENT_CLICKED, NULL); // start/stop music
    lv_obj_align(play_obj, LV_ALIGN_BOTTOM_MID, 0, -20);

    music_prev = lv_img_create(music_scr_control);
    lv_img_set_src(music_prev, &img_musicprev);
    // lv_obj_set_grid_cell(music_prev, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_add_event_cb(music_prev, prev_click_event_cb, LV_EVENT_CLICKED, NULL); // prev song
    lv_obj_add_flag(music_prev, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(music_prev, play_obj, LV_ALIGN_OUT_LEFT_MID, -50, 0);

    // MUSIC_SCR control next btn create
    music_next = lv_img_create(music_scr_control);
    lv_img_set_src(music_next, &img_musicnext);
    // lv_obj_set_grid_cell(music_next, LV_GRID_ALIGN_CENTER, 4, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_add_event_cb(music_next, next_click_event_cb, LV_EVENT_CLICKED, NULL); // next song
    lv_obj_add_flag(music_next, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(music_next, play_obj, LV_ALIGN_OUT_RIGHT_MID, 50, 0); // MUSIC_SCR control slider create

    // music_scr spectrum create
    album_img_obj = album_img_create(music_scr);
    lv_obj_align(album_img_obj, LV_ALIGN_CENTER, 0, 0);

    /*HOME_SCR CONTROL scene create*/
    lv_obj_t *home_scr_scene = lv_obj_create(home_scr);
    // lv_obj_set_grid_cell(home_scr_scene, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_size(home_scr_scene, 195, 150);
    lv_obj_align(home_scr_scene, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_clear_flag(home_scr_scene, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(home_scr_scene, &home_scr_style, 0);
    lv_obj_set_style_bg_opa(home_scr_scene, LV_OPA_30, 0);

    lv_obj_t *home_scr_scene_label = lv_label_create(home_scr_scene);
    lv_obj_set_style_text_font(home_scr_scene_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(home_scr_scene_label, true);
    lv_label_set_text(home_scr_scene_label, "#ffffff 场景");
    lv_obj_align(home_scr_scene_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t *home_scr_scene_img = lv_img_create(home_scr_scene);
    lv_img_set_src(home_scr_scene_img, &img_scene);
    lv_obj_align(home_scr_scene_img, LV_ALIGN_BOTTOM_RIGHT, 20, 5);
    lv_obj_add_flag(home_scr_scene_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        home_scr_scene_img, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_scene_label); // display scenectl screen
    lv_obj_add_event_cb(home_scr_scene, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_scene_label);
    // scene page
    scene_scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(scene_scr, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(scene_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(scene_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scene_scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_top(scene_scr, LV_VER_RES / 7, 0);
    lv_obj_add_style(scene_scr, &child_scr_style, 0);

    // scene page layout GRID
    lv_obj_set_layout(scene_scr, LV_LAYOUT_GRID);
    lv_coord_t scene_scr_col_dsc[] = { 200, 200, LV_GRID_TEMPLATE_LAST };
    lv_coord_t scene_scr_row_dsc[] = { 180, 180, LV_GRID_TEMPLATE_LAST };
    lv_obj_set_grid_dsc_array(scene_scr, scene_scr_col_dsc, scene_scr_row_dsc);
    lv_obj_set_grid_align(scene_scr, LV_GRID_ALIGN_SPACE_BETWEEN, LV_GRID_ALIGN_END);
    // scene1
    lv_obj_t *scene_scr_mode1 = lv_obj_create(scene_scr);
    scene_scr_array[0]        = scene_scr_mode1;
    lv_obj_set_grid_cell(scene_scr_mode1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_clear_flag(scene_scr_mode1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scene_scr_mode1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(scene_scr_mode1, &home_scr_style, 0);
    lv_obj_add_flag(scene_scr_mode1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(scene_scr_mode1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(scene_scr_mode1, scene_scr_mode_event_cb, LV_EVENT_ALL, 0); // scene1 control meshdev
    lv_obj_t *scene_scr_mode1_img = lv_img_create(scene_scr_mode1);
    lv_img_set_src(scene_scr_mode1_img, &img_stayhome);
    lv_img_set_zoom(scene_scr_mode1_img, LV_IMG_ZOOM_NONE * 2);
    if (display_480p)
        lv_img_set_zoom(scene_scr_mode1_img, LV_IMG_ZOOM_NONE);
    lv_obj_align(scene_scr_mode1_img, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t *scene_scr_mode1_label = lv_label_create(scene_scr_mode1);
    lv_obj_set_style_text_font(scene_scr_mode1_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(scene_scr_mode1_label, true);
    lv_label_set_text(scene_scr_mode1_label, "#ffffff 回家模式");
    lv_obj_align(scene_scr_mode1_label, LV_ALIGN_BOTTOM_MID, 0, 5);
    // scene2
    lv_obj_t *scene_scr_mode2 = lv_obj_create(scene_scr);
    scene_scr_array[1]        = scene_scr_mode2;
    lv_obj_set_grid_cell(scene_scr_mode2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_clear_flag(scene_scr_mode2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scene_scr_mode2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(scene_scr_mode2, &home_scr_style, 0);
    lv_obj_add_flag(scene_scr_mode2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(scene_scr_mode2, scene_scr_mode_event_cb, LV_EVENT_ALL, 0); // scene2 control meshdev
    lv_obj_t *scene_scr_mode2_img = lv_img_create(scene_scr_mode2);
    lv_img_set_src(scene_scr_mode2_img, &img_leavehome);
    lv_img_set_zoom(scene_scr_mode2_img, 512);
    if (display_480p)
        lv_img_set_zoom(scene_scr_mode2_img, LV_IMG_ZOOM_NONE);
    lv_obj_align(scene_scr_mode2_img, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t *scene_scr_mode2_label = lv_label_create(scene_scr_mode2);
    lv_obj_set_style_text_font(scene_scr_mode2_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(scene_scr_mode2_label, true);
    lv_label_set_text(scene_scr_mode2_label, "#ffffff 离家模式");
    lv_obj_align(scene_scr_mode2_label, LV_ALIGN_BOTTOM_MID, 0, 5);
    // scene3
    lv_obj_t *scene_scr_mode3 = lv_obj_create(scene_scr);
    scene_scr_array[2]        = scene_scr_mode3;
    lv_obj_set_grid_cell(scene_scr_mode3, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_clear_flag(scene_scr_mode3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scene_scr_mode3, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(scene_scr_mode3, &home_scr_style, 0);
    lv_obj_add_flag(scene_scr_mode3, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(scene_scr_mode3, scene_scr_mode_event_cb, LV_EVENT_ALL, 0); // scene3 control meshdev
    lv_obj_t *scene_scr_mode3_img = lv_img_create(scene_scr_mode3);
    lv_img_set_src(scene_scr_mode3_img, &img_film);
    lv_img_set_zoom(scene_scr_mode3_img, LV_IMG_ZOOM_NONE * 2);
    if (display_480p)
        lv_img_set_zoom(scene_scr_mode3_img, LV_IMG_ZOOM_NONE);
    lv_obj_align(scene_scr_mode3_img, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t *scene_scr_mode3_label = lv_label_create(scene_scr_mode3);
    lv_obj_set_style_text_font(scene_scr_mode3_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(scene_scr_mode3_label, true);
    lv_label_set_text(scene_scr_mode3_label, "#ffffff 会议模式");
    lv_obj_align(scene_scr_mode3_label, LV_ALIGN_BOTTOM_MID, 0, 5);
    // scene4
    lv_obj_t *scene_scr_mode4 = lv_obj_create(scene_scr);
    scene_scr_array[3]        = scene_scr_mode4;
    lv_obj_set_grid_cell(scene_scr_mode4, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_clear_flag(scene_scr_mode4, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(scene_scr_mode4, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(scene_scr_mode4, &home_scr_style, 0);
    lv_obj_add_flag(scene_scr_mode4, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(scene_scr_mode4, scene_scr_mode_event_cb, LV_EVENT_ALL, 0); // scene4 control meshdev
    lv_obj_t *scene_scr_mode4_img = lv_img_create(scene_scr_mode4);
    lv_img_set_src(scene_scr_mode4_img, &img_metting);
    lv_img_set_zoom(scene_scr_mode4_img, LV_IMG_ZOOM_NONE * 2);
    if (display_480p)
        lv_img_set_zoom(scene_scr_mode4_img, LV_IMG_ZOOM_NONE);
    lv_obj_align(scene_scr_mode4_img, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t *scene_scr_mode4_label = lv_label_create(scene_scr_mode4);
    lv_obj_set_style_text_font(scene_scr_mode4_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(scene_scr_mode4_label, true);
    lv_label_set_text(scene_scr_mode4_label, "#ffffff 电影模式");
    lv_obj_align(scene_scr_mode4_label, LV_ALIGN_BOTTOM_MID, 0, 5);
    // control scr title create
    lv_obj_t *scene_scr_label = lv_label_create(scene_scr);
    lv_obj_add_flag(scene_scr_label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_set_style_text_font(scene_scr_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(scene_scr_label, true);
    lv_label_set_text(scene_scr_label, "#ffffff 场景模式");
    lv_obj_align_to(scene_scr_label, scene_scr, LV_ALIGN_TOP_LEFT, 10, -40);

    // control scr close img
    lv_obj_t *scene_scr_img_close = lv_img_create(scene_scr);
    lv_obj_add_flag(scene_scr_img_close, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_img_set_src(scene_scr_img_close, &img_close);
    lv_obj_align_to(scene_scr_img_close, scene_scr, LV_ALIGN_TOP_RIGHT, 0, -40);
    lv_obj_add_flag(scene_scr_img_close, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scene_scr_img_close, func_scr_img_close_event_cb, LV_EVENT_CLICKED, 0); // close scene scr
    /*HOME_SCR_SETTING*/
    lv_obj_t *home_scr_setting = lv_obj_create(home_scr);
    lv_obj_set_size(home_scr_setting, 200, 150);
    lv_obj_align(home_scr_setting, LV_ALIGN_BOTTOM_RIGHT, -10, 0);
    lv_obj_clear_flag(home_scr_setting, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(home_scr_setting, &home_scr_style, 0);
    lv_obj_set_style_bg_opa(home_scr_setting, LV_OPA_30, 0);
    lv_obj_t *home_scr_setting_label = lv_label_create(home_scr_setting);
    lv_obj_set_style_text_font(home_scr_setting_label, &lv_d1s_font_35, 0);
    lv_label_set_recolor(home_scr_setting_label, true);
    lv_label_set_text(home_scr_setting_label, "#ffffff 设置");
    lv_obj_align(home_scr_setting_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_t *home_scr_setting_img = lv_img_create(home_scr_setting);
    lv_img_set_src(home_scr_setting_img, &img_setting);
    lv_obj_align(home_scr_setting_img, LV_ALIGN_BOTTOM_RIGHT, 20, 5);
    lv_obj_add_flag(home_scr_setting_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(home_scr_setting_img,
                        home_scr_func_event_cb,
                        LV_EVENT_CLICKED,
                        home_scr_setting_label); // display setting screen
    lv_obj_add_event_cb(home_scr_setting, home_scr_func_event_cb, LV_EVENT_CLICKED, home_scr_setting_label);
    // setting page create
    setting_scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(setting_scr, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(setting_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(setting_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(setting_scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(setting_scr, LV_DIR_VER);
    lv_obj_add_style(setting_scr, &child_scr_style, 0);
    // setting page title scr
    lv_obj_t *setting_scr_title = lv_obj_create(setting_scr);
    lv_obj_set_size(setting_scr_title, LV_HOR_RES, 90);
    lv_obj_clear_flag(setting_scr_title, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(setting_scr_title, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_opa(setting_scr_title, LV_OPA_0, 0);
    lv_obj_set_style_radius(setting_scr_title, 0, 0);
    lv_obj_align_to(setting_scr_title, setting_scr, LV_ALIGN_OUT_TOP_MID, 0, LV_VER_RES / 7);
    lv_obj_set_style_bg_opa(setting_scr_title, LV_OPA_10, 0);
    // setting page title scr close img cerate
    lv_obj_t *setting_scr_img_close = lv_img_create(setting_scr_title);
    lv_img_set_src(setting_scr_img_close, &img_close);
    lv_obj_align(setting_scr_img_close, LV_ALIGN_RIGHT_MID, 0, 10);
    lv_obj_add_flag(setting_scr_img_close, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(
        setting_scr_img_close, func_scr_img_close_event_cb, LV_EVENT_CLICKED, 0); // close setting_scr_title
    // setting page title scr title label cerate
    lv_obj_t *setting_scr_label = lv_label_create(setting_scr_title);
    lv_label_set_recolor(setting_scr_label, true);
    lv_obj_set_style_text_font(setting_scr_label, &lv_d1s_font_25, 0);
    lv_label_set_text(setting_scr_label, "#ffffff 系统设置");
    lv_obj_align(setting_scr_label, LV_ALIGN_LEFT_MID, 0, 15);

    // setting page main scr create
    lv_obj_t *setting_scr_main = lv_obj_create(setting_scr);
    lv_obj_set_style_border_opa(setting_scr_main, LV_OPA_0, 0);
    lv_obj_set_style_radius(setting_scr_main, 0, 0);
    lv_obj_set_scroll_dir(setting_scr_main, LV_DIR_VER);
    lv_obj_set_size(setting_scr_main, LV_HOR_RES, 390);
    lv_obj_align_to(setting_scr_main, setting_scr_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(setting_scr_main, LV_OPA_0, 0);

    // main page-function screen create
    lv_obj_t *setting_function_list = lv_list_create(setting_scr_main);
    lv_obj_set_size(setting_function_list, lv_pct(80), lv_pct(120));
    lv_obj_clear_flag(setting_function_list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(setting_function_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(setting_function_list, LV_DIR_VER);
    lv_obj_add_style(setting_function_list, &home_scr_style, 0);
    lv_obj_align_to(setting_function_list, setting_scr_main, LV_ALIGN_TOP_MID, 0, 10);

    // function screen wifibtn
    lv_obj_t *wifi_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(wifi_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(wifi_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *wifi_btn_imgfunc = lv_img_create(wifi_btn);
    lv_img_set_src(wifi_btn_imgfunc, &img_homewifistatus_on);
    lv_obj_align(wifi_btn_imgfunc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_t *wifi_btn_label = lv_label_create(wifi_btn);
    lv_obj_set_style_text_font(wifi_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(wifi_btn_label, true);
    lv_label_set_text(wifi_btn_label, "#ffffff 无线局域网");
    lv_obj_align(wifi_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *wifi_btn_imgnext = lv_img_create(wifi_btn);
    lv_img_set_src(wifi_btn_imgnext, &img_next);
    lv_obj_align(wifi_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(wifi_btn, LV_FLEX_FLOW_ROW, 0);

    // function voice_btn create
    lv_obj_t *voice_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(voice_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(voice_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *voice_btn_imgfunc = lv_img_create(voice_btn);
    lv_img_set_src(voice_btn_imgfunc, &img_voice);
    lv_obj_align(voice_btn_imgfunc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_t *voice_btn_label = lv_label_create(voice_btn);
    lv_obj_set_style_text_font(voice_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(voice_btn_label, true);
    lv_label_set_text(voice_btn_label, "#ffffff 音量调节");
    lv_obj_align(voice_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *voice_btn_imgnext = lv_img_create(voice_btn);
    lv_img_set_src(voice_btn_imgnext, &img_next);
    lv_obj_align(voice_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(voice_btn, LV_FLEX_FLOW_ROW, 0);
    lv_obj_add_event_cb(voice_btn, setting_scr_voice_event_cb, LV_EVENT_SHORT_CLICKED, NULL);

#if 1
    // create wifi scr
    layertop_scr_wifi = lv_obj_create(lv_layer_top());
    lv_obj_set_size(layertop_scr_wifi, lv_pct(95), lv_pct(85));
    lv_obj_center(layertop_scr_wifi);
    lv_obj_add_style(layertop_scr_wifi, &home_scr_style, 0);
    lv_obj_set_style_bg_opa(layertop_scr_wifi, LV_OPA_100, 0);
    lv_obj_set_scrollbar_mode(layertop_scr_wifi, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(layertop_scr_wifi, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(layertop_scr_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(layertop_scr_wifi, LV_OBJ_FLAG_HIDDEN);
    // create wifi scr label
    lv_obj_t *layertop_scr_label = lv_label_create(layertop_scr_wifi);
    lv_obj_set_style_text_font(layertop_scr_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(layertop_scr_label, true);
    lv_label_set_text(layertop_scr_label, "#ffffff 无线网配置");
    lv_obj_align(layertop_scr_label, LV_ALIGN_TOP_LEFT, 0, 0);
    // create wifi scr img close
    lv_obj_t *layertop_scr_close = lv_img_create(layertop_scr_wifi);
    lv_img_set_src(layertop_scr_close, &img_close);
    lv_obj_align(layertop_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_flag(layertop_scr_close, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(layertop_scr_close, wifiscr_scr_close_event_cb, LV_EVENT_CLICKED, 0); // for close wifi scr
    // create wifiscan list,wait event refresh to update wifissid
    wifiscan_list = lv_list_create(layertop_scr_wifi);
    lv_obj_set_size(wifiscan_list, 300, 250);
    lv_obj_align(wifiscan_list, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    // create curent wifi and wifi connect status
    lv_obj_t *wifilink_img = lv_img_create(layertop_scr_wifi);
    lv_img_set_src(wifilink_img, &img_homewifistatus_on);
    lv_obj_align_to(wifilink_img, wifiscan_list, LV_ALIGN_OUT_TOP_LEFT, 10, -45);

    wifilink_ssid = lv_label_create(layertop_scr_wifi);
    lv_obj_set_style_text_font(wifilink_ssid, &lv_d1s_font_22, 0);
    lv_obj_set_size(wifilink_ssid, 350, 30);
    lv_label_set_long_mode(wifilink_ssid, LV_LABEL_LONG_DOT);
    // lv_obj_set_style_text_align(wifilink_ssid, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_recolor(wifilink_ssid, true);
    if (app_wifi_config_get_used_ssid() != NULL) {
        lv_label_set_text_fmt(wifilink_ssid, "%s", app_wifi_config_get_used_ssid());
    } else {
        lv_label_set_text(wifilink_ssid, "#ff0000 未获取");
    }
    lv_label_set_long_mode(wifilink_ssid, LV_LABEL_LONG_WRAP);
    lv_obj_align_to(wifilink_ssid, wifilink_img, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *wifilink_status = lv_label_create(layertop_scr_wifi);
    lv_obj_set_style_text_font(wifilink_status, &lv_d1s_font_22, 0);
    lv_label_set_recolor(wifilink_status, true);
    lv_label_set_text(wifilink_status, "#ffffff 状态:");
    lv_obj_align_to(wifilink_status, wifilink_img, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    wifilink_statuslabel = lv_label_create(layertop_scr_wifi);
    lv_obj_set_style_text_font(wifilink_statuslabel, &lv_d1s_font_22, 0);
    lv_label_set_recolor(wifilink_statuslabel, true);
    if (app_network_internet_is_connected()) {
        lv_label_set_text(wifilink_statuslabel, "#00ff00 已连接");
    } else {
        lv_label_set_text(wifilink_statuslabel, "#ff0000 未连接");
    }
    lv_obj_align_to(wifilink_statuslabel, wifilink_status, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // create scan btn
    wifiscan_btn = lv_btn_create(layertop_scr_wifi);
    lv_obj_set_style_radius(wifiscan_btn, 5, 0);
    lv_obj_set_size(wifiscan_btn, lv_pct(22), lv_pct(13));
    lv_obj_add_event_cb(wifiscan_btn, my_btn_scan_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(wifiscan_btn, wifiscan_list, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_t *wifiscan_label = lv_label_create(wifiscan_btn);
    lv_obj_set_style_text_font(wifiscan_label, &lv_d1s_font_25, 0);
    lv_label_set_text(wifiscan_label, "扫 描");
    lv_obj_center(wifiscan_label);
    // create reset btn
    lv_obj_t *wificlear_btn = lv_btn_create(layertop_scr_wifi);
    lv_obj_set_style_radius(wificlear_btn, 5, 0);
    lv_obj_set_size(wificlear_btn, lv_pct(22), lv_pct(13));
    lv_obj_add_event_cb(wificlear_btn, wifi_btn_clear_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(wificlear_btn, wifiscan_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_t *wificlear_label = lv_label_create(wificlear_btn);
    lv_obj_set_style_text_font(wificlear_label, &lv_d1s_font_25, 0);
    lv_label_set_text(wificlear_label, "重 置");
    lv_obj_center(wificlear_label);
    lv_obj_add_event_cb(wifi_btn, setting_scr_wifibtn_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // display wifi scr
#endif
    // function screen lightnessbtn
    lv_obj_t *lightness_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(lightness_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(lightness_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *lightness_btn_imgfunc = lv_img_create(lightness_btn);
    lv_img_set_src(lightness_btn_imgfunc, &img_lightbrightness);
    lv_obj_align(lightness_btn_imgfunc, LV_ALIGN_LEFT_MID, 17, 0);
    lv_obj_t *lightness_btn_label = lv_label_create(lightness_btn);
    lv_obj_set_style_text_font(lightness_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(lightness_btn_label, true);
    lv_label_set_text(lightness_btn_label, "#ffffff 屏幕亮度");
    lv_obj_align(lightness_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *lightness_btn_imgnext = lv_img_create(lightness_btn);
    lv_img_set_src(lightness_btn_imgnext, &img_next);
    lv_obj_align(lightness_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(lightness_btn, LV_FLEX_FLOW_ROW, 0);
    lv_obj_add_event_cb(
        lightness_btn, setting_scr_lightnessbtn_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // display lightness scr

    // otherscr screen meshbtn
    lv_obj_t *mesh_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(mesh_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(mesh_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *mesh_btn_imgfunc = lv_img_create(mesh_btn);
    lv_img_set_src(mesh_btn_imgfunc, &img_ble);
    lv_obj_align(mesh_btn_imgfunc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_t *mesh_btn_label = lv_label_create(mesh_btn);
    lv_obj_set_style_text_font(mesh_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(mesh_btn_label, true);
    lv_label_set_text(mesh_btn_label, "#ffffff 蓝牙设备");
    lv_obj_align(mesh_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *mesh_btn_imgnext = lv_img_create(mesh_btn);
    lv_img_set_src(mesh_btn_imgnext, &img_next);
    lv_obj_align(mesh_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(mesh_btn, LV_FLEX_FLOW_ROW, 0);
    lv_obj_add_event_cb(mesh_btn, setting_scr_meshbtn_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // display mesh scr

    // otherscreen fotabtn create
    lv_obj_t *fota_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(fota_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(fota_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *fota_btn_imgfunc = lv_img_create(fota_btn);
    lv_img_set_src(fota_btn_imgfunc, &img_fotaupate);
    lv_obj_align(fota_btn_imgfunc, LV_ALIGN_LEFT_MID, 17, 0);
    lv_obj_t *fota_btn_label = lv_label_create(fota_btn);
    lv_obj_set_style_text_font(fota_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(fota_btn_label, true);
    lv_label_set_text(fota_btn_label, "#ffffff 固件升级");
    lv_obj_align(fota_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *fota_btn_imgnext = lv_img_create(fota_btn);
    lv_img_set_src(fota_btn_imgnext, &img_next);
    lv_obj_align(fota_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(fota_btn, LV_FLEX_FLOW_ROW, 0);
    lv_obj_add_event_cb(fota_btn, setting_scr_fotabtn_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // display fota scr

    // otherscreen informationbtn
    lv_obj_t *information_btn = lv_list_add_btn(setting_function_list, NULL, NULL);
    lv_obj_set_style_bg_opa(information_btn, LV_OPA_0, 0);
    lv_obj_set_style_bg_opa(information_btn, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_t *information_btn_imgfunc = lv_img_create(information_btn);
    lv_img_set_src(information_btn_imgfunc, &img_information);
    lv_obj_align(information_btn_imgfunc, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_t *information_btn_label = lv_label_create(information_btn);
    lv_obj_set_style_text_font(information_btn_label, &lv_d1s_font_25, 0);
    lv_label_set_recolor(information_btn_label, true);
    lv_label_set_text(information_btn_label, "#ffffff 设备信息");
    lv_obj_align(information_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *information_btn_imgnext = lv_img_create(information_btn);
    lv_img_set_src(information_btn_imgnext, &img_next);
    lv_obj_align(information_btn_imgnext, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_layout(information_btn, LV_FLEX_FLOW_ROW, 0);
    lv_obj_add_event_cb(information_btn, setting_scr_information_btn_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
}

static void scene_scr_mode_event_cb(lv_event_t *e)
{
    lv_event_code_t code  = lv_event_get_code(e);
    lv_obj_t *      scene = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        // switch 4 scene
        for (int i = 0; i < 4; i++) {
            if (scene == scene_scr_array[i]) {
                if (lv_obj_has_state(scene_scr_array[i], LV_STATE_CHECKED)) {
                    lv_obj_set_style_bg_color(scene_scr_array[i], lv_color_grey(), 0);
                    lv_obj_set_style_bg_opa(scene_scr_array[i], LV_OPA_100, 0);
                    switch (i) {
                        case 0: {
                            gateway_subgrp_set_onoff(0, 1);
                        } break;
                        case 1: {
                            gateway_subgrp_set_onoff(0, 0);
                        } break;
                        case 2: {
                            gateway_subgrp_set_onoff(1, 1);
                            gateway_subgrp_set_onoff(2, 0);
                        } break;
                        case 3: {
                            gateway_subgrp_set_onoff(1, 0);
                            gateway_subgrp_set_onoff(2, 1);
                        } break;
                        default:
                            LOGD(TAG, "NOTHING clicked\r\n");
                            break;
                    }
                } else {
                    lv_obj_set_style_bg_color(scene_scr_array[i], lv_color_black(), 0);
                    lv_obj_set_style_bg_opa(scene_scr_array[i], LV_OPA_50, 0);
                    gateway_subgrp_set_onoff(0, 0);
                }
            } else {
                // gateway_subgrp_set_onoff(0, 0);
                // LOGD(TAG, "[[[[[%d]]]]] not clicked\r\n", i);
                // clear btn status
                lv_obj_clear_state(scene_scr_array[i], LV_STATE_CHECKED);
                lv_obj_set_style_bg_color(scene_scr_array[i], lv_color_black(), 0);
                lv_obj_set_style_bg_opa(scene_scr_array[i], LV_OPA_50, 0);
            }
        }
    }
}

static void home_scr_func_event_cb(lv_event_t *e)
{
    lv_event_code_t code  = lv_event_get_code(e);
    lv_obj_t *      label = lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        if (!strcmp(lv_label_get_text(label), "#ffffff 设备")) // home control screen
        {
            lv_obj_clear_flag(mesh_scr, LV_OBJ_FLAG_HIDDEN);
            if (lv_obj_get_child_cnt(mesh_scr_devctl_list) > 0) // clean list screen
            {
                lv_obj_clean(mesh_scr_devctl_list);
            }
            char kv_pos[32];
            char kv_index[10];

            for (int i = 1; i < 11; i++) {

                memset(kv_index, 0, sizeof(kv_index));
                memset(kv_pos, 0, sizeof(kv_pos));
                lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                if (ret >= 0) {
                    // switch pos
                    if (strcmp(kv_pos, "卧室灯") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);

                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);
                        // lv_obj_add_event_cb(listbtn, mesh_scr_devctl_listbtn_event_cb, LV_EVENT_SHORT_CLICKED, i);
                        // //add event for control meshdev

                        lv_obj_clear_flag(listbtn, LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_set_style_pad_top(listbtn, 30, 0);
                        lv_obj_t *listbtn_arc = lv_arc_create(listbtn);
                        lv_obj_set_size(listbtn_arc, 200, 200);
                        lv_arc_set_rotation(listbtn_arc, 135);
                        lv_arc_set_bg_angles(listbtn_arc, 0, 270);
                        lv_obj_set_style_arc_width(listbtn_arc, 15, LV_PART_INDICATOR);
                        lv_obj_set_style_arc_width(listbtn_arc, 15, LV_PART_MAIN);
                        lv_obj_set_style_arc_color(
                            listbtn_arc, lv_palette_lighten(LV_PALETTE_YELLOW, 2), LV_PART_INDICATOR);
                        lv_obj_set_style_bg_color(listbtn_arc, lv_palette_lighten(LV_PALETTE_YELLOW, 3), LV_PART_KNOB);
                        lv_arc_set_range(listbtn_arc, 0, 100);
                        lv_arc_set_value(listbtn_arc, 0);
                        lv_obj_align(listbtn_arc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);

                        lv_obj_t *listbtn_imgbg = lv_btn_create(listbtn);
                        lv_obj_set_size(listbtn_imgbg, 100, 100);
                        lv_obj_set_style_radius(listbtn_imgbg, LV_RADIUS_CIRCLE, 0);
                        lv_obj_set_style_bg_color(listbtn_imgbg, lv_color_black(), LV_STATE_CHECKED);
                        lv_obj_set_style_bg_opa(listbtn_imgbg, LV_OPA_30, 0);
                        lv_obj_set_style_bg_opa(listbtn_imgbg, LV_OPA_30, LV_STATE_CHECKED);
                        lv_obj_set_style_border_opa(listbtn_imgbg, 0, 0);
                        lv_obj_set_style_bg_color(listbtn_imgbg, lv_color_black(), 0);
                        lv_obj_add_flag(listbtn_imgbg, LV_OBJ_FLAG_CHECKABLE);
                        lv_obj_align(listbtn_imgbg, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);

                        lv_obj_add_event_cb(listbtn_arc, listbtn_arc_event_cb, LV_EVENT_ALL, (void *)(size_t)i);
                        lv_obj_add_event_cb(listbtn_imgbg, listbtn_imgbg_event_cb, LV_EVENT_CLICKED, (void *)(size_t)i);

                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_label_set_text(listbtn_label, "#ffffff 卧室灯");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);

                        lv_obj_t *listbtn_img = lv_img_create(listbtn_imgbg);
                        lv_img_set_src(listbtn_img, &img_bedroom_0);
                        lv_obj_center(listbtn_img);
                    }
                }
            }
            for (int i = 1; i < 11; i++) {

                memset(kv_index, 0, sizeof(kv_index));
                memset(kv_pos, 0, sizeof(kv_pos));
                lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                if (ret >= 0) {
                    // switch pos
                    if (strcmp(kv_pos, "厨房灯") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);

                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);

                        lv_obj_add_event_cb(listbtn,
                                            mesh_scr_devctl_listbtn_event_cb,
                                            LV_EVENT_SHORT_CLICKED,
                                            (void *)(size_t)i); // add event for control meshdev
                        // create img add label in listbtn
                        lv_obj_t *listbtn_imgfunc = lv_img_create(listbtn);
                        lv_img_set_src(listbtn_imgfunc, &img_foodlight_off);
                        lv_obj_align(listbtn_imgfunc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);
                        lv_obj_set_style_pad_top(listbtn_imgfunc, 20, 0);
                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_label_set_text(listbtn_label, "#ffffff 厨房灯");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                    } else if (strcmp(kv_pos, "客厅灯") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);

                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);
                        lv_obj_add_event_cb(listbtn,
                                            mesh_scr_devctl_listbtn_event_cb,
                                            LV_EVENT_SHORT_CLICKED,
                                            (void *)(size_t)i); // add event for control meshdev
                        // create img add label in listbtn
                        lv_obj_t *listbtn_imgfunc = lv_img_create(listbtn);
                        lv_img_set_src(listbtn_imgfunc, &img_livingled_off);
                        lv_obj_align(listbtn_imgfunc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);
                        lv_obj_set_style_pad_top(listbtn_imgfunc, 20, 0);
                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_label_set_text(listbtn_label, "#ffffff 客厅灯");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                    }
                }
            }
            for (int i = 1; i < 11; i++) {
                // switch pos
                memset(kv_index, 0, sizeof(kv_index));
                memset(kv_pos, 0, sizeof(kv_pos));
                lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                if (ret >= 0) {
                    if (strcmp(kv_pos, "窗帘") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);
                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);
                        lv_obj_add_event_cb(listbtn,
                                            mesh_scr_devctl_listbtn_event_cb,
                                            LV_EVENT_SHORT_CLICKED,
                                            (void *)(size_t)i); // add event for control meshdev
                        // create img add label in listbtn
                        lv_obj_t *listbtn_imgfunc = lv_img_create(listbtn);
                        lv_img_set_src(listbtn_imgfunc, &img_curtain_off);
                        lv_obj_align(listbtn_imgfunc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);
                        lv_obj_set_style_pad_top(listbtn_imgfunc, 30, 0);
                        lv_obj_set_style_pad_bottom(listbtn_imgfunc, 10, 0);
                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_text(listbtn_label, "#ffffff 窗 帘");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                    } else if (strcmp(kv_pos, "无线开关1") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);

                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);

                        lv_obj_add_event_cb(listbtn,
                                            mesh_scr_devctl_listbtn_event_cb,
                                            LV_EVENT_SHORT_CLICKED,
                                            (void *)(size_t)i); // add event for control meshdev
                        // create img add label in listbtn
                        lv_obj_t *listbtn_imgfunc = lv_img_create(listbtn);
                        lv_img_set_src(listbtn_imgfunc, &img_wifisw_off);
                        lv_obj_align(listbtn_imgfunc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);
                        lv_obj_set_style_pad_top(listbtn_imgfunc, 20, 0);
                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_label_set_text(listbtn_label, "#ffffff 无线开关1");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                    } else if (strcmp(kv_pos, "无线开关2") == 0) {
                        // add btn in mesh_scr_devctl_list
                        lv_obj_t *listbtn = lv_list_add_btn(mesh_scr_devctl_list, NULL, NULL);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, 0);
                        lv_obj_set_style_bg_opa(listbtn, LV_OPA_0, LV_STATE_PRESSED);
                        lv_obj_set_style_layout(listbtn, LV_FLEX_FLOW_ROW, 0);
                        lv_obj_add_flag(listbtn, LV_OBJ_FLAG_CHECKABLE);

                        lv_obj_add_event_cb(listbtn,
                                            mesh_scr_devctl_listbtn_event_cb,
                                            LV_EVENT_SHORT_CLICKED,
                                            (void *)(size_t)i); // add event for control meshdev
                        // create img add label in listbtn
                        lv_obj_t *listbtn_imgfunc = lv_img_create(listbtn);
                        lv_img_set_src(listbtn_imgfunc, &img_wifisw_off);
                        lv_obj_align(listbtn_imgfunc, LV_ALIGN_CENTER, -(LV_HOR_RES / 7), 0);
                        lv_obj_set_style_pad_top(listbtn_imgfunc, 20, 0);
                        lv_obj_t *listbtn_label = lv_label_create(listbtn);
                        lv_obj_set_style_text_font(listbtn_label, &lv_d1s_font_25, 0);
                        lv_label_set_recolor(listbtn_label, true);
                        lv_label_set_text(listbtn_label, "#ffffff 无线开关2");
                        lv_obj_align(listbtn_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                    }
                }
            }
        } else if (!strcmp(lv_label_get_text(label), "#ffffff 音乐")) {
            // add hidden flag
            lv_obj_clear_flag(music_scr, LV_OBJ_FLAG_HIDDEN);
        } else if (!strcmp(lv_label_get_text(label), "#ffffff 场景")) {
            // add hidden flag
            lv_obj_clear_flag(scene_scr, LV_OBJ_FLAG_HIDDEN);
        } else if (!strcmp(lv_label_get_text(label), "#ffffff 设置")) {
            // add hidden flag
            lv_obj_clear_flag(setting_scr, LV_OBJ_FLAG_HIDDEN);
            // for(int i = 0; i < 10; i++)
            // {
            //     gateway_subgrp_del(i);
            // }
        }
    }
}
static void listbtn_arc_event_cb(lv_event_t *e)
{
    lv_obj_t *      arc   = lv_event_get_target(e);
    lv_event_code_t code  = lv_event_get_code(e);
    int16_t         index = (size_t)lv_event_get_user_data(e);
    if (code == LV_EVENT_RELEASED) {
        int16_t brightness = lv_arc_get_value(arc);
        if (brightness == 0) {
            // lv_obj_set_style_shadow_color(lv_obj_get_child(lv_obj_get_parent(arc), 1),
            // lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
            lv_obj_clear_state(lv_obj_get_child(lv_obj_get_parent(arc), 1), LV_STATE_CHECKED);
        } else {
            // lv_obj_set_style_shadow_color(lv_obj_get_child(lv_obj_get_parent(arc), 1),
            // lv_palette_lighten(LV_PALETTE_YELLOW, 2), 0);
            lv_obj_add_state(lv_obj_get_child(lv_obj_get_parent(arc), 1), LV_STATE_CHECKED);
        }
        brightness *= 2.55;
        LOGD(TAG, "brightness:%d\n", brightness);
        gateway_subdev_set_brightness(index, brightness);
        if (brightness == 0) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_0);
            lv_obj_clear_state(lv_obj_get_child(lv_obj_get_parent(arc), 1), LV_STATE_CHECKED);
        } else if (brightness > 2 && brightness <= 64) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_20);
        } else if (brightness > 64 && brightness <= 128) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_40);
        } else if (brightness > 128 && brightness <= 192) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_60);
        } else if (brightness > 192 && brightness < 254) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_80);
        } else if (brightness == 254) {
            lv_img_set_src(lv_obj_get_child(lv_obj_get_child(lv_obj_get_parent(arc), 1), 0), &img_bedroom_100);
            lv_obj_add_state(lv_obj_get_child(lv_obj_get_parent(arc), 1), LV_STATE_CHECKED);
        }
    }
}
static void listbtn_imgbg_event_cb(lv_event_t *e)
{
    lv_obj_t *      obj   = lv_event_get_target(e);
    int16_t         index = (size_t)lv_event_get_user_data(e);
    lv_event_code_t code  = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_arc_set_value(lv_obj_get_child(lv_obj_get_parent(obj), 0), 100);
            lv_event_send(lv_obj_get_child(lv_obj_get_parent(obj), 0), LV_EVENT_VALUE_CHANGED, 0);
            // lv_obj_set_style_shadow_color(obj, lv_palette_lighten(LV_PALETTE_YELLOW, 1), 0);
            lv_img_set_src(lv_obj_get_child(obj, 0), &img_bedroom_100);
            gateway_subdev_set_brightness(index, 255);
        } else {
            lv_arc_set_value(lv_obj_get_child(lv_obj_get_parent(obj), 0), 1);
            lv_event_send(lv_obj_get_child(lv_obj_get_parent(obj), 0), LV_EVENT_VALUE_CHANGED, 0);
            // lv_obj_set_style_shadow_color(obj, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
            lv_img_set_src(lv_obj_get_child(obj, 0), &img_bedroom_0);
            gateway_subdev_set_brightness(index, 1);
        }
    }
}
static void func_scr_img_close_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // hidden all child screen
        lv_obj_add_flag(mesh_scr, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(music_scr, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(scene_scr, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(setting_scr, LV_OBJ_FLAG_HIDDEN);
    }
}

static void play_obj_event_click_cb(lv_event_t *e)
{
    lv_obj_t *      obj  = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            switch (track_id) {
                case 2: // swtich 2th song
                    smtaudio_start(MEDIA_MUSIC, "file:///mnt/alibaba3.mp3", 0, 0);
                    lv_label_set_text(music_label_name, "#ffffff Music 3");
                    lv_label_set_text(music_label_author, "#ffffff Author 3");
                    break;
                case 1: // switch 1th song
                    smtaudio_start(MEDIA_MUSIC, "file:///mnt/alibaba2.mp3", 0, 0);
                    lv_label_set_text(music_label_name, "#ffffff Music 2");
                    lv_label_set_text(music_label_author, "#ffffff Author 2");
                    break;
                case 0: // switch 3th song
                    smtaudio_start(MEDIA_MUSIC, "file:///mnt/alibaba1.mp3", 0, 0);
                    lv_label_set_text(music_label_name, "#ffffff Music 1");
                    lv_label_set_text(music_label_author, "#ffffff Author 1");
                    break;
                default:
                    break;
            };
            // music animation start
            music_anim_status(true);
            lv_img_set_src(obj, &img_musicpause);
        } else {
            lv_img_set_src(obj, &img_musicplay);
            // close all music
            smtaudio_stop(SMTAUDIO_TYPE_ALL);
            // music animation stop
            music_anim_status(false);
        }
    }
}

void music_anim_status(bool status)
{
    // create img angle animation
    lv_anim_t imgangle_anim;
    lv_anim_init(&imgangle_anim);
    lv_anim_set_var(&imgangle_anim, album_img_obj);
    lv_anim_set_exec_cb(&imgangle_anim, set_img_angle);
    lv_anim_set_values(&imgangle_anim, 0, 3600);
    lv_anim_set_time(&imgangle_anim, 3000);
    lv_anim_set_repeat_count(&imgangle_anim, LV_ANIM_REPEAT_INFINITE);
    // switch animation status
    if (status == true) {
        // animation start
        lv_anim_start(&imgangle_anim);
        // lv_anim_start(&slider_anim);
        // lv_anim_start(&slidertime_anim);
    } else if (status == false) {
        // animation stop
        lv_anim_del(album_img_obj, set_img_angle);
        // lv_anim_del(slider_obj, set_slider_value);
        // lv_anim_del(time_obj, set_slidertime_value);
    }
}

static void setting_scr_voiceslider_event_cb(lv_event_t *e)
{
    lv_event_code_t code         = lv_event_get_code(e);
    lv_obj_t *      slider       = lv_event_get_target(e);
    lv_obj_t *      slider_label = lv_event_get_user_data(e);
    char            buf[8];

    if (code == LV_EVENT_CLICKED) {
        LOGD(TAG, "EVENT_CLICKED%d \r\n", (int)lv_slider_get_value(slider));
        int voice = lv_slider_get_value(slider);
        smtaudio_vol_set(voice);
    }
    if (code == LV_EVENT_VALUE_CHANGED) {
        LOGD(TAG, "EVENT_VALUE_CHANGED%d \r\n", (int)lv_slider_get_value(slider));
        memset(buf, 0, sizeof(buf));
        lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
        lv_label_set_recolor(slider_label, true);
        lv_label_set_text_fmt(slider_label, "#ffffff %s", buf);
        lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

        if ((int)lv_slider_get_value(slider) % 5 == 0) {
            LOGD(TAG, "EVENT_VALUE_set----->%d \r\n");
            int voice = lv_slider_get_value(slider);
            smtaudio_vol_set(voice);
        }
        if (code == LV_EVENT_RELEASED) {
            LOGD(TAG, "EVENT_RELEASED%d \r\n", (int)lv_slider_get_value(slider));
            int voice = lv_slider_get_value(slider);
            smtaudio_vol_set(voice);
        }
    }
}

static void setting_scr_lightnessslider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider       = lv_event_get_target(e);
    lv_obj_t *slider_label = lv_event_get_user_data(e);
    char      buf[8];
    // get slider value assignment label
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_recolor(slider_label, true);
    lv_label_set_text_fmt(slider_label, "#ffffff %s", buf);

    int disp_brightness = (int)lv_slider_get_value(slider) * 2.55 + 0.5;
    LOGD(TAG, "DISP_BRIGHTNESS: %d", disp_brightness);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    rvm_dev_t *disp_dev = rvm_hal_display_open("disp");
    rvm_hal_display_set_brightness(disp_dev, disp_brightness);
    rvm_hal_display_close(disp_dev);
}

static void setting_scr_voice_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {

        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 10);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);

        // enable layertop clicked,change background color
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        // create lightness scr
        lv_obj_t *layertop_scr_voice = lv_obj_create(lv_layer_top());
        lv_obj_add_style(layertop_scr_voice, &home_scr_style, 0);
        lv_obj_set_size(layertop_scr_voice, lv_pct(90), lv_pct(40));
        lv_obj_align(layertop_scr_voice, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_border_opa(layertop_scr_voice, LV_OPA_0, 0);
        lv_obj_set_scrollbar_mode(layertop_scr_voice, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(layertop_scr_voice, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(layertop_scr_voice, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *layertop_scr_label = lv_label_create(layertop_scr_voice);
        lv_obj_set_style_text_font(layertop_scr_label, &lv_d1s_font_22, 0);
        lv_label_set_recolor(layertop_scr_label, true);
        lv_label_set_text(layertop_scr_label, "#ffffff 音量调节");
        lv_obj_align(layertop_scr_label, LV_ALIGN_TOP_LEFT, 0, 0);
        // create lightness scr close img
        lv_obj_t *layertop_scr_close = lv_img_create(layertop_scr_voice);
        lv_img_set_src(layertop_scr_close, &img_close);
        // if(display_480p) lv_img_set_zoom(layertop_scr_close, 180);
        lv_obj_align(layertop_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_add_flag(layertop_scr_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(layertop_scr_close, layertop_scr_close_event_cb, LV_EVENT_CLICKED, 0);
        // create lightness img
        lv_obj_t *lightness_img = lv_img_create(layertop_scr_voice);
        lv_img_set_src(lightness_img, &img_voice);
        // if(display_480p) lv_img_set_zoom(lightness_img, LV_IMG_ZOOM_NONE/2);
        lv_obj_align(lightness_img, LV_ALIGN_LEFT_MID, 10, 15);

        lv_obj_t *voice_slider = lv_slider_create(layertop_scr_voice);
        lv_obj_set_size(voice_slider, lv_pct(60), lv_pct(17));
        int voice = smtaudio_vol_get();
        lv_slider_set_value(voice_slider, voice, LV_ANIM_OFF);
        lv_obj_align_to(voice_slider, lightness_img, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 25, 0);
        lv_obj_t *slider_label = lv_label_create(layertop_scr_voice);
        lv_label_set_recolor(slider_label, true);
        lv_obj_set_style_text_font(slider_label, &lv_d1s_font_25, 0);
        lv_label_set_text_fmt(slider_label, "#ffffff %d%%", voice);
        lv_obj_align_to(slider_label, voice_slider, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 25, 0);
        // lv_obj_add_event_cb(voice_slider, setting_scr_voiceslider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);
        lv_obj_add_event_cb(voice_slider, setting_scr_voiceslider_event_cb, LV_EVENT_ALL, slider_label);
    }
}

static void setting_scr_lightnessbtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {

        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 10);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);

        // enable layertop clicked,change background color
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        // create lightness scr
        lv_obj_t *layertop_scr_lightness = lv_obj_create(lv_layer_top());
        lv_obj_add_style(layertop_scr_lightness, &home_scr_style, 0);
        lv_obj_set_size(layertop_scr_lightness, lv_pct(90), lv_pct(40));
        lv_obj_align(layertop_scr_lightness, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_border_opa(layertop_scr_lightness, LV_OPA_0, 0);
        lv_obj_set_scrollbar_mode(layertop_scr_lightness, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(layertop_scr_lightness, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(layertop_scr_lightness, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *layertop_scr_label = lv_label_create(layertop_scr_lightness);
        lv_obj_set_style_text_font(layertop_scr_label, &lv_d1s_font_22, 0);
        lv_label_set_recolor(layertop_scr_label, true);
        lv_label_set_text(layertop_scr_label, "#ffffff 屏幕亮度");
        lv_obj_align(layertop_scr_label, LV_ALIGN_TOP_LEFT, 0, 0);
        // create lightness scr close img
        lv_obj_t *layertop_scr_close = lv_img_create(layertop_scr_lightness);
        lv_img_set_src(layertop_scr_close, &img_close);
        // if(display_480p) lv_img_set_zoom(layertop_scr_close, 180);
        lv_obj_align(layertop_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_add_flag(layertop_scr_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(layertop_scr_close, layertop_scr_close_event_cb, LV_EVENT_CLICKED, 0);
        // create lightness img
        lv_obj_t *lightness_img = lv_img_create(layertop_scr_lightness);
        lv_img_set_src(lightness_img, &img_lightbrightness);
        // if(display_480p) lv_img_set_zoom(lightness_img, LV_IMG_ZOOM_NONE/2);
        lv_obj_align(lightness_img, LV_ALIGN_LEFT_MID, 10, 15);
        // create lightness slider
        rvm_dev_t *disp_dev = rvm_hal_display_open("disp");
        uint8_t    brightness;
        rvm_hal_display_get_brightness(disp_dev, (uint8_t *)&brightness);
        brightness = brightness / 2.55 + 0.5;
        LOGD(TAG, "BRIGHTNESS: %d", brightness);
        rvm_hal_display_close(disp_dev);

        lv_obj_t *lightness_slider = lv_slider_create(layertop_scr_lightness);
        lv_obj_set_size(lightness_slider, lv_pct(60), lv_pct(17));
        lv_slider_set_value(lightness_slider, brightness, LV_ANIM_OFF);
        lv_obj_align_to(lightness_slider, lightness_img, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 25, 0);
        lv_obj_t *slider_label = lv_label_create(layertop_scr_lightness);
        lv_label_set_recolor(slider_label, true);
        lv_obj_set_style_text_font(slider_label, &lv_d1s_font_25, 0);
        lv_label_set_text_fmt(slider_label, "#ffffff %d%%", brightness);
        lv_obj_align_to(slider_label, lightness_slider, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 25, 0);
        lv_obj_add_event_cb(
            lightness_slider, setting_scr_lightnessslider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);
    }
}

static void wifi_btn_clear_event_handler(lv_event_t *e)
{
    char ssid_buf[20];
    char psk_buf[20];
    aos_kv_del("wifi_ssid");
    aos_kv_del("wifi_psk");
    for (int i = 0; i < 5; i++) {
        // clear kv_list wifi conf
        lv_snprintf(ssid_buf, sizeof(ssid_buf), "wifi_ssid%d", i);
        aos_kv_del(ssid_buf);
        lv_snprintf(psk_buf, sizeof(psk_buf), "wifi_psk%d", i);
        aos_kv_del(psk_buf);
    }
}

static void setting_scr_wifibtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {
        // enable layertop clicked,add background color/opa
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        lv_obj_clear_flag(layertop_scr_wifi, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      ta   = lv_event_get_target(e);
    lv_obj_t *      kb   = lv_event_get_user_data(e);
    /*Focus on the clicked text area*/
    if (code == LV_EVENT_FOCUSED) {
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_SIZE_CONTENT / 2, 0);
            lv_obj_update_layout(input_box);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_align_to(input_box, kb, LV_ALIGN_OUT_TOP_MID, 0, -10);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    // keyboard defocused
    else if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_center(input_box);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    // keyboard ENTER-key
    else if (code == LV_EVENT_READY)
    {
        lv_obj_center(input_box);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(e->target, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, e->target);
        LOGD(TAG, "textarea:%s", lv_textarea_get_text(ta));
    }
    // keyboard CANCEL-key
    else if (code == LV_EVENT_CANCEL)
    {
        lv_obj_center(input_box);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(e->target, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, e->target);
    }
}

static void wifiscr_scr_close_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // clear layertop clicked,bg color,opa,del all child
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_0, 0);
        lv_obj_add_flag(layertop_scr_wifi, LV_OBJ_FLAG_HIDDEN);
    }
}

static void layertop_scr_close_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        // clear layertop clicked,bg color,opa,del all child
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_top(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_0, 0);
        lv_obj_del_async(obj->parent);
    }
}

static void setting_scr_meshbtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {
        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 10);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);
        // add layertop clicked,bg color,opa
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_80, 0);
        // create mesh scr on layertop
        lv_obj_t *setting_mesh_scr = lv_obj_create(lv_layer_top());
        lv_obj_set_size(setting_mesh_scr, lv_pct(95), lv_pct(70));
        lv_obj_add_style(setting_mesh_scr, &home_scr_style, 0);
        lv_obj_center(setting_mesh_scr);
        lv_obj_set_scrollbar_mode(setting_mesh_scr, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(setting_mesh_scr, LV_OBJ_FLAG_SCROLLABLE);
        // create mesh scr  title
        lv_obj_t *setting_mesh_scr_title = lv_label_create(setting_mesh_scr);
        lv_obj_set_style_text_font(setting_mesh_scr_title, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_mesh_scr_title, &lv_d1s_font_22, 0);
        lv_label_set_recolor(setting_mesh_scr_title, true);
        lv_label_set_text(setting_mesh_scr_title, "#ffffff Mesh设置");
        lv_obj_align(setting_mesh_scr_title, LV_ALIGN_TOP_LEFT, 0, 0);
        // create mesh scr img close
        lv_obj_t *setting_mesh_scr_close = lv_img_create(setting_mesh_scr);
        lv_img_set_src(setting_mesh_scr_close, &img_close);
        // if(display_480p) lv_img_set_zoom(setting_mesh_scr_close, 180);
        lv_obj_align(setting_mesh_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_add_flag(setting_mesh_scr_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(setting_mesh_scr_close,
                            setting_mesh_scr_close_event_cb,
                            LV_EVENT_CLICKED,
                            setting_mesh_scr); // close mesh scr
        // create mesh scan scr
        mesh_scan_scr = lv_obj_create(setting_mesh_scr);
        lv_obj_set_size(mesh_scan_scr, lv_pct(75), lv_pct(65));
        lv_obj_align(mesh_scan_scr, LV_ALIGN_TOP_LEFT, 0, LV_VER_RES / 6);
        lv_obj_set_style_bg_color(mesh_scan_scr, lv_color_make(235, 235, 235), 0);
        // lv_obj_set_scrollbar_mode(mesh_scan_scr, LV_SCROLLBAR_MODE_OFF);
        // lv_obj_clear_flag(mesh_scan_scr, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_flex_flow(mesh_scan_scr, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(mesh_scan_scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        // set screen layout
        lv_obj_set_style_pad_top(mesh_scan_scr, 20, 0);
        lv_obj_set_style_pad_bottom(mesh_scan_scr, LV_VER_RES / 3, 0);
        lv_obj_set_style_pad_row(mesh_scan_scr, 10, 0);
        lv_obj_set_style_pad_column(mesh_scan_scr, 10, 0);
        // create mesh scr btn style
        static lv_style_t style_btn;
        lv_style_init(&style_btn);
        lv_style_set_width(&style_btn, lv_pct(23));
        lv_style_set_height(&style_btn, lv_pct(15));
        lv_style_set_radius(&style_btn, 5);
        // create scan added btn
        lv_obj_t *btn_label;
        lv_obj_t *setting_mesh_scanbtn = lv_btn_create(setting_mesh_scr);
        lv_obj_add_style(setting_mesh_scanbtn, &style_btn, 0);
        lv_obj_align_to(setting_mesh_scanbtn, mesh_scan_scr, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
        lv_obj_add_event_cb(setting_mesh_scanbtn, mesh_scanbtn_event_cb, LV_EVENT_CLICKED, mesh_scan_scr);
        btn_label = lv_label_create(setting_mesh_scanbtn);
        lv_obj_set_style_text_font(btn_label, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(btn_label, &lv_d1s_font_25, 0);
        lv_label_set_text(btn_label, "扫  描");
        lv_obj_center(btn_label);
        lv_obj_t *setting_mesh_showbtn = lv_btn_create(setting_mesh_scr);
        lv_obj_add_style(setting_mesh_showbtn, &style_btn, 0);
        lv_obj_align_to(setting_mesh_showbtn, setting_mesh_scanbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
        lv_obj_add_event_cb(setting_mesh_showbtn, mesh_showbtn_event_cb, LV_EVENT_CLICKED, 0);
        btn_label = lv_label_create(setting_mesh_showbtn);
        lv_obj_set_style_text_font(btn_label, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(btn_label, &lv_d1s_font_25, 0);
        lv_label_set_text(btn_label, "已入网");
        lv_obj_center(btn_label);
    }
}

static void setting_scr_fotabtn_event_cb(lv_event_t *e)
{
    char            getvalue[128];
    int             ret = 0;
    int             intvalue;
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {
        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 10);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);

        // add layertop clicked,bgcolor,opa
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        // create keyboard on layertop
        lv_obj_t *kb = lv_keyboard_create(lv_layer_top());
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_font(kb, &lv_font_montserrat_34, 0);
        if (display_480p)
            lv_obj_set_style_text_font(kb, &lv_font_montserrat_24, 0);
        // create fota scr
        setting_fota_scr = lv_obj_create(lv_layer_top());
        lv_obj_set_size(setting_fota_scr, lv_pct(90), lv_pct(70));
        lv_obj_center(setting_fota_scr);
        lv_obj_set_scrollbar_mode(setting_fota_scr, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(setting_fota_scr, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_style(setting_fota_scr, &home_scr_style, 0);
        // create fota scr title
        lv_obj_t *setting_fota_scr_title_label = lv_label_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_scr_title_label, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_scr_title_label, &lv_d1s_font_22, 0);
        lv_label_set_recolor(setting_fota_scr_title_label, true);
        lv_label_set_text(setting_fota_scr_title_label, "#ffffff OCC控制台");
        lv_obj_align(setting_fota_scr_title_label, LV_ALIGN_TOP_LEFT, 0, 0);
        // create fota scr img close
        lv_obj_t *setting_fota_scr_close = lv_img_create(setting_fota_scr);
        lv_img_set_src(setting_fota_scr_close, &img_close);
        // if(display_480p) lv_img_set_zoom(setting_fota_scr_close, 180);
        lv_obj_align(setting_fota_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_add_flag(setting_fota_scr_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(
            setting_fota_scr_close, setting_mesh_scr_close_event_cb, LV_EVENT_CLICKED, setting_fota_scr);
        // get kv list "deviceid" "model" "cop_version" information
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("device_id", getvalue, sizeof(getvalue));
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("model", getvalue, sizeof(getvalue));
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("cop_version", getvalue, sizeof(getvalue));
        ret = aos_kv_getint("fota_en", &intvalue);
        if (ret >= 0) {
            LOGD(TAG, "kv getint:%d\n", intvalue);
        } else {
            LOGD(TAG, "kv getint fail\n");
        }
        // create fota scr version label
        lv_obj_t *setting_fota_version_label = lv_label_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_version_label, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_version_label, &lv_d1s_font_22, 0);
        lv_label_set_recolor(setting_fota_version_label, true);
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("cop_version", getvalue, sizeof(getvalue));
        // set cop_version
        ret < 0 ? lv_label_set_text(setting_fota_version_label, "#ffffff 固件版本:# #ff0000 获取失败#")
                : lv_label_set_text_fmt(setting_fota_version_label, "#ffffff 固件版本:%s#", getvalue);
        lv_obj_set_pos(setting_fota_version_label, 0, lv_pct(25));
        // create fota scr device_id
        lv_obj_t *setting_fota_deviceid_label = lv_label_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_deviceid_label, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_deviceid_label, &lv_d1s_font_25, 0);
        lv_label_set_recolor(setting_fota_deviceid_label, true);
        lv_label_set_text(setting_fota_deviceid_label, "#ffffff 设备编号:");
        lv_obj_align_to(
            setting_fota_deviceid_label, setting_fota_version_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_VER_RES / 15);
        lv_obj_t *setting_fota_deviceid_textarea = lv_textarea_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_deviceid_textarea, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_deviceid_textarea, &lv_d1s_font_25, 0);
        lv_textarea_set_one_line(setting_fota_deviceid_textarea, true);
        lv_obj_set_width(setting_fota_deviceid_textarea, lv_pct(65));
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("device_id", getvalue, sizeof(getvalue));
        // set device_id
        ret < 0 ? lv_textarea_set_placeholder_text(setting_fota_deviceid_textarea, "未设置设备编号")
                : lv_textarea_set_text(setting_fota_deviceid_textarea, getvalue);
        lv_obj_align_to(
            setting_fota_deviceid_textarea, setting_fota_deviceid_label, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 18, 0);
        lv_obj_add_event_cb(
            setting_fota_deviceid_textarea, fota_ta_event_cb, LV_EVENT_ALL, kb); // use keyboard enter device_id
        // create fota scr model label
        lv_obj_t *setting_fota_devicemodel_label = lv_label_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_devicemodel_label, &lv_d1s_font_35, 0);
        lv_label_set_recolor(setting_fota_devicemodel_label, true);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_devicemodel_label, &lv_d1s_font_25, 0);
        lv_label_set_text(setting_fota_devicemodel_label, "#ffffff 产品型号:");
        lv_obj_align_to(
            setting_fota_devicemodel_label, setting_fota_deviceid_label, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_VER_RES / 15);
        lv_obj_t *setting_fota_devicemodel_textarea = lv_textarea_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_devicemodel_textarea, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_devicemodel_textarea, &lv_d1s_font_25, 0);
        lv_textarea_set_one_line(setting_fota_devicemodel_textarea, true);
        lv_obj_set_width(setting_fota_devicemodel_textarea, lv_pct(65));
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring("model", getvalue, sizeof(getvalue));
        // set model
        ret < 0 ? lv_textarea_set_placeholder_text(setting_fota_devicemodel_textarea, "未设置产品型号")
                : lv_textarea_set_text(setting_fota_devicemodel_textarea, getvalue);
        lv_obj_align_to(setting_fota_devicemodel_textarea,
                        setting_fota_devicemodel_label,
                        LV_ALIGN_OUT_RIGHT_MID,
                        LV_HOR_RES / 18,
                        0);
        lv_obj_add_event_cb(
            setting_fota_devicemodel_textarea, fota_ta_event_cb, LV_EVENT_ALL, kb); // use keyboard enter model
        // create fota scr check switch and label
        lv_obj_t *setting_fota_encheck_label = lv_label_create(setting_fota_scr);
        lv_obj_set_style_text_font(setting_fota_encheck_label, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(setting_fota_encheck_label, &lv_d1s_font_25, 0);
        lv_label_set_recolor(setting_fota_encheck_label, true);
        lv_label_set_text(setting_fota_encheck_label, "#ffffff FOTA检测:");
        lv_obj_align_to(
            setting_fota_encheck_label, setting_fota_devicemodel_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_VER_RES / 15);
        lv_obj_t *setting_fotaen_switch = lv_switch_create(setting_fota_scr);
        lv_obj_set_size(setting_fotaen_switch, lv_pct(20), lv_pct(15));
        lv_obj_add_event_cb(setting_fotaen_switch, fotaen_switch_event_cb, LV_EVENT_ALL, NULL);
        // get fotaen_status to set switch
        intvalue ? lv_obj_add_state(setting_fotaen_switch, LV_STATE_CHECKED)
                 : lv_obj_clear_state(setting_fotaen_switch, LV_STATE_CHECKED);
        lv_obj_align_to(setting_fotaen_switch, setting_fota_encheck_label, LV_ALIGN_OUT_RIGHT_MID, LV_HOR_RES / 15, 0);
    }
}

static void setting_scr_information_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) {

        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 10);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);
        // add layertop clicked,bgcolor,opa
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
        // create fota scr
        lv_obj_t *setting_information_scr = lv_obj_create(lv_layer_top());
        lv_obj_set_size(setting_information_scr, lv_pct(95), lv_pct(70));
        lv_obj_add_style(setting_information_scr, &home_scr_style, 0);
        lv_obj_center(setting_information_scr);
        lv_obj_set_scrollbar_mode(setting_information_scr, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(setting_information_scr, LV_OBJ_FLAG_SCROLLABLE);
        // create fota scr img close
        lv_obj_t *setting_information_scr_close = lv_img_create(setting_information_scr);
        lv_img_set_src(setting_information_scr_close, &img_close);
        lv_obj_align(setting_information_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_obj_add_flag(setting_information_scr_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(
            setting_information_scr_close, setting_mesh_scr_close_event_cb, LV_EVENT_CLICKED, setting_information_scr);
        lv_obj_t *inforlabel1 = lv_label_create(setting_information_scr);
        lv_obj_set_style_text_font(inforlabel1, &lv_d1s_font_35, 0);
        lv_label_set_recolor(inforlabel1, true);
        lv_label_set_text(inforlabel1, "#ffffff Smart panel LVGL\n#ffffff         RE-D1SAL\n#ffffff            V7.7.0");
        lv_obj_align(inforlabel1, LV_ALIGN_CENTER, 0, 0);
    }
}

static void fotaen_switch_event_cb(lv_event_t *e)
{
    char            getidvalue[128];
    char            getmodelvalue[128];
    int             ret;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        // get fota device_id model
        if (lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 4))
            && strlen(lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 4)))
            && lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 6))
            && strlen(lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 6))))
        {
            LOGD(TAG,
                 "device_id:%s, model:%s\n",
                 lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 4)),
                 lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 6)));
            // get fota device_id
            memset(getidvalue, 0, sizeof(getidvalue));
            ret = aos_kv_getstring("device_id", getidvalue, sizeof(getidvalue));
            LOGD(TAG, "getidvalue: %s\n", ret < 0 ? "failed" : getidvalue);
            // create fota model
            memset(getmodelvalue, 0, sizeof(getmodelvalue));
            ret = aos_kv_getstring("model", getmodelvalue, sizeof(getmodelvalue));
            LOGD(TAG, "getmodelvalue: %s\n", ret < 0 ? "failed" : getmodelvalue);
            // getdevice_id model compare old setting
            if (strcmp(getidvalue, lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 4))) != 0
                || strcmp(getmodelvalue, lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 6))) != 0)
            {
                LOGD(TAG, "FOTA set new device_id&model\n");
                aos_kv_setstring("device_id", lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 4)));
                aos_kv_setstring("model", lv_textarea_get_text(lv_obj_get_child(lv_obj_get_parent(obj), 6)));
            }
            // switch status
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                aos_kv_setint("fota_en", 1);
                LOGD(TAG, "FOTA check opening...\n");
            } else {
                aos_kv_setint("fota_en", 0);
                LOGD(TAG, "FOTA check closeing...\n");
            }
            // add layertop clicked, bg color,opa
            lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
            lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_50, 0);
            // create msgscreen on layertop
            lv_obj_t *msg_scr = lv_obj_create(lv_layer_sys());
            lv_obj_set_size(msg_scr, lv_pct(70), lv_pct(50));
            lv_obj_center(msg_scr);
            lv_obj_set_scrollbar_mode(msg_scr, LV_SCROLLBAR_MODE_OFF);
            lv_obj_clear_flag(msg_scr, LV_OBJ_FLAG_SCROLLABLE);
            // create msg screen title
            lv_obj_t *msg_scr_title = lv_label_create(msg_scr);
            lv_obj_set_style_text_font(msg_scr_title, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_title, &lv_d1s_font_25, 0);
            lv_label_set_recolor(msg_scr_title, true);
            lv_label_set_text(msg_scr_title, "#ff0000 注意!!!");
            lv_obj_align(msg_scr_title, LV_ALIGN_TOP_LEFT, 0, 0);
            // create msg scr text
            lv_obj_t *msg_scr_text = lv_label_create(msg_scr);
            lv_obj_set_style_text_font(msg_scr_text, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_text, &lv_d1s_font_25, 0);
            lv_label_set_text(msg_scr_text, "开启/关闭FOTA检测后\n需重新启动设备生效");
            lv_obj_center(msg_scr_text);
            // create msg scr ok btn
            lv_obj_t *msg_scr_btnok = lv_btn_create(msg_scr);
            lv_obj_set_size(msg_scr_btnok, lv_pct(35), lv_pct(15));
            lv_obj_set_style_bg_color(msg_scr_btnok, lv_color_red(), 0);
            lv_obj_add_event_cb(msg_scr_btnok, msg_scr_btnok_event_cb, LV_EVENT_CLICKED, obj);
            lv_obj_align(msg_scr_btnok, LV_ALIGN_BOTTOM_MID, -(LV_HOR_RES / 6), 0);
            lv_obj_t *msg_scr_btnok_label = lv_label_create(msg_scr_btnok);
            lv_obj_set_style_text_font(msg_scr_btnok_label, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_btnok_label, &lv_d1s_font_25, 0);
            lv_label_set_text(msg_scr_btnok_label, "立刻重启");
            lv_obj_center(msg_scr_btnok_label);
            // create msg scr cancel btn
            lv_obj_t *msg_scr_btncancel = lv_btn_create(msg_scr);
            lv_obj_set_size(msg_scr_btncancel, lv_pct(35), lv_pct(15));
            lv_obj_add_event_cb(msg_scr_btncancel, msg_scr_btncancel_event_cb, LV_EVENT_CLICKED, 0);
            lv_obj_align(msg_scr_btncancel, LV_ALIGN_BOTTOM_MID, LV_HOR_RES / 6, 0);
            lv_obj_t *msg_scr_btncancel_label = lv_label_create(msg_scr_btncancel);
            lv_obj_set_style_text_font(msg_scr_btncancel_label, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_btncancel_label, &lv_d1s_font_25, 0);
            lv_label_set_text(msg_scr_btncancel_label, "稍后重启");
            lv_obj_center(msg_scr_btncancel_label);
        } else {
            LOGD(TAG, " device_id or model is NULL\n");

            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                lv_obj_clear_state(obj, LV_STATE_CHECKED);
            }
            // add layertop clicked,bg color,opa
            lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
            lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_50, 0);
            // create msg scr on layertop
            lv_obj_t *msg_scr = lv_obj_create(lv_layer_sys());
            lv_obj_set_size(msg_scr, lv_pct(70), lv_pct(50));
            lv_obj_center(msg_scr);
            lv_obj_set_scrollbar_mode(msg_scr, LV_SCROLLBAR_MODE_OFF);
            lv_obj_clear_flag(msg_scr, LV_OBJ_FLAG_SCROLLABLE);
            // create msg_scr title
            lv_obj_t *msg_scr_title = lv_label_create(msg_scr);
            lv_obj_set_style_text_font(msg_scr_title, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_title, &lv_d1s_font_25, 0);
            lv_label_set_recolor(msg_scr_title, true);
            lv_label_set_text(msg_scr_title, "#ff0000 注意!!!");
            lv_obj_align(msg_scr_title, LV_ALIGN_TOP_LEFT, 0, 0);
            // create msg_scr text
            lv_obj_t *msg_scr_text = lv_label_create(msg_scr);
            lv_obj_set_style_text_font(msg_scr_text, &lv_d1s_font_35, 0);
            lv_label_set_text(msg_scr_text, "请输入有效的设备编号及类型");
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_text, &lv_d1s_font_25, 0);
            lv_obj_center(msg_scr_text);
            // create msg_scr ok btn
            lv_obj_t *msg_scr_btnok = lv_btn_create(msg_scr);
            lv_obj_set_size(msg_scr_btnok, lv_pct(35), lv_pct(15));
            lv_obj_add_event_cb(msg_scr_btnok, msg_scr_btncancel_event_cb, LV_EVENT_CLICKED, obj);
            lv_obj_align(msg_scr_btnok, LV_ALIGN_BOTTOM_MID, 0, 0);
            lv_obj_t *msg_scr_btnok_label = lv_label_create(msg_scr_btnok);
            lv_obj_set_style_text_font(msg_scr_btnok_label, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(msg_scr_btnok_label, &lv_d1s_font_25, 0);
            lv_label_set_text(msg_scr_btnok_label, "好 的");
            lv_obj_center(msg_scr_btnok_label);
        }
    }
}

static void msg_scr_btnok_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LOGD(TAG, "fotaen reboot \n");
        // reboot system to change fota check
        aos_reboot();
    }
}

static void msg_scr_btncancel_event_cb(lv_event_t *e)
{
    lv_obj_t *      obj  = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // clear layertop clicked and opa
        lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
        lv_obj_del(lv_obj_get_parent(obj));
    }
}

static void fota_ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      ta   = lv_event_get_target(e);
    lv_obj_t *      kb   = lv_event_get_user_data(e);
    // keyboard inputing
    if (code == LV_EVENT_FOCUSED) {
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_SIZE_CONTENT / 2, 0);
            lv_obj_update_layout(setting_fota_scr);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_align_to(setting_fota_scr, kb, LV_ALIGN_OUT_TOP_MID, 0, -10);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    // keyboard clicked without
    else if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_center(setting_fota_scr);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    // keyboard input enter
    else if (code == LV_EVENT_READY)
    {
        lv_obj_center(setting_fota_scr);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(e->target, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, e->target);
        LOGD(TAG, "textarea:%s", lv_textarea_get_text(ta));
    }
    // keyboard input cancel
    else if (code == LV_EVENT_CANCEL)
    {
        lv_obj_center(setting_fota_scr);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(e->target, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, e->target);
    }
}

static void mesh_scanbtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code       = lv_event_get_code(e);
    lv_obj_t *      scr        = lv_event_get_user_data(e);
    lv_obj_t *      scr_father = lv_obj_get_parent(scr);
    if (code == LV_EVENT_CLICKED) {
        if (lv_obj_get_child_cnt(mesh_scan_scr) > 0) {
            lv_obj_clean(mesh_scan_scr);
        }
        // reset mesh scan obj
        free(mesh_dev_scan_obj);
        // init scandev_array
        memset(scandev_array.scan_array, 0, sizeof(gw_discovered_dev_t) * 10);
        scandev_array.len = 0;
        // start meshdev scan
        gateway_discover_start(timeout);
        // meshdev scan start
        lv_obj_add_flag(scr_father, LV_OBJ_FLAG_HIDDEN);
        lv_timer_t *mesh_scan_dev_reporttimer = lv_timer_create(meshdev_report_timer, 1000, 0); // meshdev report timer
        lv_timer_set_repeat_count(mesh_scan_dev_reporttimer, 6);
        lv_timer_t *mesh_scan_dev_stoptimer = lv_timer_create(meshdev_stop_timer, 6000, scr); // meshdev stop timer
        lv_timer_set_repeat_count(mesh_scan_dev_stoptimer, 1);
        lv_timer_t *mesh_scan_dev_createtimes
            = lv_timer_create(mesh_scan_dev_create_timer, 6000, 0); // meshdev create timer
        lv_timer_set_repeat_count(mesh_scan_dev_createtimes, 1);
        // create spinner to wait
        lv_obj_t *spinner = lv_spinner_create(lv_layer_top(), 1000, 100);
        lv_obj_set_size(spinner, LV_HOR_RES / 3, LV_VER_RES / 3);
        lv_obj_center(spinner);
        lv_obj_del_delayed(spinner, 5500);
    }
}

int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len) // tranlates addr to str
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

int bt_uuid_val_to_str(const uint8_t uuid[16], char *str, size_t len) // tranlates uuid to str
{
    return snprintf(str,
                    len,
                    "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                    uuid[0],
                    uuid[1],
                    uuid[2],
                    uuid[3],
                    uuid[4],
                    uuid[5],
                    uuid[6],
                    uuid[7],
                    uuid[8],
                    uuid[9],
                    uuid[10],
                    uuid[11],
                    uuid[12],
                    uuid[13],
                    uuid[14],
                    uuid[15]);
}

void mesh_scan_dev_create_timer(lv_timer_t *timer)
{
    // malloc space to create mesh_dev_scan_obj
    mesh_dev_scan_obj = (lv_obj_t **)malloc(sizeof(lv_obj_t *) * scandev_array.len);
    for (int i = 0; i < scandev_array.len; i++) {
        // create mesh_dev_scan_obj
        mesh_dev_scan_obj[i] = lv_img_create(mesh_scan_scr);
        lv_img_set_src(mesh_dev_scan_obj[i], &img_scan_off);
        lv_img_set_zoom(mesh_dev_scan_obj[i], 448);
        if (display_480p)
            lv_img_set_zoom(mesh_dev_scan_obj[i], LV_IMG_ZOOM_NONE);
        lv_obj_add_flag(mesh_dev_scan_obj[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(mesh_dev_scan_obj[i],
                            mesh_dev_scan_obj_event_cb,
                            LV_EVENT_CLICKED,
                            &scandev_array.scan_array[i]); // add event to create meshdev scr
    }
}

void meshdev_report_timer(lv_timer_t *timer)
{
    int     flag1 = 0;
    uint8_t dev_addr_str[20];
    uint8_t uuid_str[40];
    uint8_t scan_mac_str[20];
    if (scan_msg) // event subscribe update
    {
        bt_addr_val_to_str(scan_msg->protocol_info.ble_mesh_info.dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));
        for (int i = 0; i < scandev_array.len; i++) {
            bt_addr_val_to_str(scandev_array.scan_array[i].protocol_info.ble_mesh_info.dev_addr,
                               (char *)scan_mac_str,
                               sizeof(scan_mac_str));
            // compare scanmsg and scandev
            if (strcmp((char *)scan_mac_str, (char *)dev_addr_str) == 0) {
                // meshdev exist!
                flag1 = 1;
                break;
            } else {
                // meshdev unexist!
                flag1 = 0;
            }
        }

        if (!flag1) // add new meshdev
        {
            LOGD(TAG, "scan_mac_str:%s\n", dev_addr_str);
            // dev_addr_str NONE
            if (strcmp((char *)dev_addr_str, "00:00:00:00:00:00")) {
                // add new meshdev
                memcpy(&scandev_array.scan_array[scandev_array.len++], scan_msg, sizeof(gw_evt_discovered_info_t));
            }
        }
        for (int i = 0; i < scandev_array.len; i++) {
            bt_addr_val_to_str(scandev_array.scan_array[i].protocol_info.ble_mesh_info.dev_addr,
                               (char *)scan_mac_str,
                               sizeof(scan_mac_str));
            // set protocol_info.ble_mesh_info.bearer 1 for add
            scandev_array.scan_array[i].protocol_info.ble_mesh_info.bearer = 1;
            bt_uuid_val_to_str(
                scandev_array.scan_array[i].protocol_info.ble_mesh_info.uuid, (char *)uuid_str, sizeof(uuid_str));
            LOGI(TAG,
                 "[%0d]Save dev mac:%s type:%02x uuid:%s berar:%x\n",
                 i + 1,
                 scan_mac_str,
                 scandev_array.scan_array[i].protocol_info.ble_mesh_info.addr_type,
                 uuid_str,
                 scandev_array.scan_array[i].protocol_info.ble_mesh_info.bearer);
        }
    }
    LOGD(TAG, "[[scan over]]\n");
}

void meshdev_stop_timer(lv_timer_t *timer)
{
    lv_obj_t *scr        = timer->user_data;
    lv_obj_t *scr_father = lv_obj_get_parent(scr);
    lv_obj_clear_flag(scr_father, LV_OBJ_FLAG_HIDDEN);
    // stop mesh scan
    gateway_discover_stop();
    memset(&scan_msg, 0, sizeof(scan_msg));
}

static void mesh_dev_scan_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    // use to del img
    added_temp_obj = obj;
    uint8_t              addr_str[20];
    uint8_t              uuid_str[40];
    gw_discovered_dev_t *dev = lv_event_get_user_data(e);
    bt_addr_val_to_str(dev->protocol_info.ble_mesh_info.dev_addr, (char *)addr_str, sizeof(addr_str));
    bt_uuid_val_to_str(dev->protocol_info.ble_mesh_info.uuid, (char *)uuid_str, sizeof(uuid_str));

    if (code == LV_EVENT_CLICKED) {
        int indexmax = 0;
        for (int i = 0; i < 10; i++) {
            if (index_def.index_array[i])
                indexmax++;
        }
        if (indexmax < 10) {
            // home scr style
            static lv_style_t home_scr_style;
            lv_style_init(&home_scr_style);
            lv_style_set_shadow_color(&home_scr_style, lv_color_white());
            lv_style_set_shadow_width(&home_scr_style, 5);
            lv_style_set_bg_color(&home_scr_style, lv_color_black());
            lv_style_set_radius(&home_scr_style, 20);
            lv_style_set_border_opa(&home_scr_style, LV_OPA_0);
            // create mesh_dev_scan_obj_scr
            lv_obj_t *mesh_dev_scan_obj_scr = lv_obj_create(lv_layer_top());
            lv_obj_set_size(mesh_dev_scan_obj_scr, lv_pct(95), lv_pct(70));
            lv_obj_center(mesh_dev_scan_obj_scr);
            lv_obj_clear_flag(mesh_dev_scan_obj_scr, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(mesh_dev_scan_obj_scr, LV_SCROLLBAR_MODE_OFF);
            lv_obj_add_style(mesh_dev_scan_obj_scr, &home_scr_style, 0);
            // create mesh_dev_scan_obj_scr label
            lv_obj_t *mesh_dev_scan_obj_scr_maclabel = lv_label_create(mesh_dev_scan_obj_scr);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_maclabel, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_maclabel, &lv_d1s_font_25, 0);
            lv_label_set_recolor(mesh_dev_scan_obj_scr_maclabel, true);
            lv_label_set_text_fmt(mesh_dev_scan_obj_scr_maclabel, "#ffffff MAC:  %s", addr_str);
            lv_obj_align_to(
                mesh_dev_scan_obj_scr_maclabel, mesh_dev_scan_obj_scr, LV_ALIGN_TOP_LEFT, 0, LV_VER_RES / 12);
            // create mesh_dev_scan_obj_scr uuidlabel
            lv_obj_t *mesh_dev_scan_obj_scr_uuidlabel = lv_label_create(mesh_dev_scan_obj_scr);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_uuidlabel, &lv_d1s_font_30, 0);
            if (display_480p)
                lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_uuidlabel, &lv_d1s_font_22, 0);
            lv_label_set_recolor(mesh_dev_scan_obj_scr_uuidlabel, true);
            lv_label_set_text_fmt(mesh_dev_scan_obj_scr_uuidlabel, "#ffffff UUID:  %s", uuid_str);
            lv_obj_align_to(mesh_dev_scan_obj_scr_uuidlabel,
                            mesh_dev_scan_obj_scr_maclabel,
                            LV_ALIGN_OUT_BOTTOM_LEFT,
                            0,
                            LV_VER_RES / 15);
            // create mesh_dev_scan_obj_scr statuslabel
            lv_obj_t *mesh_dev_scan_obj_scr_statuslabel = lv_label_create(mesh_dev_scan_obj_scr);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_statuslabel, &lv_d1s_font_30, 0);
            if (display_480p)
                lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_statuslabel, &lv_d1s_font_25, 0);
            lv_label_set_recolor(mesh_dev_scan_obj_scr_statuslabel, true);
            lv_label_set_text_fmt(mesh_dev_scan_obj_scr_statuslabel, "#ffffff %s#  #ff0000 %s#", "状  态: ", "未入网");
            lv_obj_align_to(mesh_dev_scan_obj_scr_statuslabel,
                            mesh_dev_scan_obj_scr_uuidlabel,
                            LV_ALIGN_OUT_BOTTOM_LEFT,
                            0,
                            LV_VER_RES / 15);
            // crate mesh_dev_scan_obj_scr addbtn
            lv_obj_t *mesh_dev_scan_obj_addbtn = lv_btn_create(mesh_dev_scan_obj_scr);
            lv_obj_set_size(mesh_dev_scan_obj_addbtn, lv_pct(25), lv_pct(15));
            lv_obj_set_style_radius(mesh_dev_scan_obj_addbtn, 5, 0);
            lv_obj_align_to(mesh_dev_scan_obj_addbtn,
                            mesh_dev_scan_obj_scr,
                            LV_ALIGN_BOTTOM_LEFT,
                            LV_HOR_RES / 12,
                            -LV_VER_RES / 15);
            lv_obj_add_event_cb(mesh_dev_scan_obj_addbtn, mesh_dev_scan_obj_addbtn_event_cb, LV_EVENT_CLICKED, dev);
            lv_obj_t *mesh_dev_scan_obj_addbtn_label = lv_label_create(mesh_dev_scan_obj_addbtn);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_addbtn_label, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(mesh_dev_scan_obj_addbtn_label, &lv_d1s_font_25, 0);
            lv_label_set_text(mesh_dev_scan_obj_addbtn_label, "加入");
            lv_obj_center(mesh_dev_scan_obj_addbtn_label);
            // crate mesh_dev_scan_obj_scr closebtn
            lv_obj_t *mesh_dev_scan_obj_closebtn = lv_btn_create(mesh_dev_scan_obj_scr);
            lv_obj_set_size(mesh_dev_scan_obj_closebtn, lv_pct(25), lv_pct(15));
            lv_obj_set_style_radius(mesh_dev_scan_obj_closebtn, 5, 0);
            lv_obj_align_to(mesh_dev_scan_obj_closebtn,
                            mesh_dev_scan_obj_scr,
                            LV_ALIGN_BOTTOM_RIGHT,
                            -LV_HOR_RES / 12,
                            -LV_VER_RES / 15);
            lv_obj_add_event_cb(mesh_dev_scan_obj_closebtn,
                                mesh_dev_scan_obj_closebtn_event_cb,
                                LV_EVENT_CLICKED,
                                mesh_dev_scan_obj_scr);
            lv_obj_t *mesh_dev_scan_obj_closebtn_label = lv_label_create(mesh_dev_scan_obj_closebtn);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_closebtn_label, &lv_d1s_font_35, 0);
            if (display_480p)
                lv_obj_set_style_text_font(mesh_dev_scan_obj_closebtn_label, &lv_d1s_font_25, 0);
            lv_label_set_text(mesh_dev_scan_obj_closebtn_label, "取 消");
            lv_obj_center(mesh_dev_scan_obj_closebtn_label);
        } else {
            // home scr style
            static lv_style_t home_scr_style;
            lv_style_init(&home_scr_style);
            lv_style_set_shadow_color(&home_scr_style, lv_color_white());
            lv_style_set_shadow_width(&home_scr_style, 5);
            lv_style_set_bg_color(&home_scr_style, lv_color_black());
            lv_style_set_radius(&home_scr_style, 20);
            lv_style_set_border_opa(&home_scr_style, LV_OPA_0);

            // create mesh_dev_scan_obj_scr
            lv_obj_t *mesh_dev_scan_obj_scr = lv_obj_create(lv_layer_top());
            lv_obj_set_size(mesh_dev_scan_obj_scr, lv_pct(90), lv_pct(70));
            lv_obj_add_style(mesh_dev_scan_obj_scr, &home_scr_style, 0);
            lv_obj_center(mesh_dev_scan_obj_scr);
            lv_obj_clear_flag(mesh_dev_scan_obj_scr, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(mesh_dev_scan_obj_scr, LV_SCROLLBAR_MODE_OFF);
            // create mesh_dev_scan_obj_scr_label
            lv_obj_t *mesh_dev_scan_obj_scr_label1 = lv_label_create(mesh_dev_scan_obj_scr);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_label1, &lv_d1s_font_22, 0);
            lv_label_set_recolor(mesh_dev_scan_obj_scr_label1, true);
            lv_label_set_text(mesh_dev_scan_obj_scr_label1, "#ffffff 注意!!!");
            lv_obj_align(mesh_dev_scan_obj_scr_label1, LV_ALIGN_TOP_LEFT, 0, 0);
            // create mesh_dev_scan_obj_scr_label2
            lv_obj_t *mesh_dev_scan_obj_scr_label2 = lv_label_create(mesh_dev_scan_obj_scr);
            lv_obj_set_style_text_font(mesh_dev_scan_obj_scr_label2, &lv_d1s_font_22, 0);
            lv_label_set_recolor(mesh_dev_scan_obj_scr_label2, true);
            lv_label_set_text(mesh_dev_scan_obj_scr_label2, "#ffffff 入网子设备已到达上限!!!");
            lv_obj_center(mesh_dev_scan_obj_scr_label2);
            // create mesh_dev_scan_obj_scr_close
            lv_obj_t *mesh_dev_scan_obj_scr_close = lv_img_create(mesh_dev_scan_obj_scr);
            lv_img_set_src(mesh_dev_scan_obj_scr_close, &img_close);
            lv_obj_add_flag(mesh_dev_scan_obj_scr_close, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(mesh_dev_scan_obj_scr_close, mesh_dev_added_close_event_cb, LV_EVENT_CLICKED, 0);
            lv_obj_align(mesh_dev_scan_obj_scr_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        }
    }
}

static void mesh_dev_scan_obj_addbtn_event_cb(lv_event_t *e)
{
    lv_obj_t *      obj  = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    uint8_t         addr_str[20];
    uint8_t         uuid_str[40];
    uint8_t         dev_addr_str[20];
    // get meshdev info
    gw_discovered_dev_t *dev = lv_event_get_user_data(e);
    bt_addr_val_to_str(dev->protocol_info.ble_mesh_info.dev_addr, (char *)addr_str, sizeof(addr_str));
    bt_uuid_val_to_str(dev->protocol_info.ble_mesh_info.uuid, (char *)uuid_str, sizeof(uuid_str));

    LOGD(TAG, "addr_straddr:%s\n", addr_str);

    if (code == LV_EVENT_CLICKED) {
        for (int i = 0; i < scandev_array.len; i++) {
            bt_addr_val_to_str(scandev_array.scan_array[i].protocol_info.ble_mesh_info.dev_addr,
                               (char *)dev_addr_str,
                               sizeof(dev_addr_str));
            // compare meshdev index
            if (strcmp((char *)dev_addr_str, (char *)addr_str) == 0) {
                int ret = gateway_subdev_add(scandev_array.scan_array[i]);
                if (ret == 0) {
                    // disable btn clicked,change status
                    lv_obj_add_state(obj, LV_STATE_DISABLED);
                    lv_label_set_text_fmt(lv_obj_get_child(lv_obj_get_parent(obj), 2),
                                          "#ffffff %s#  #ff0000 %s# ",
                                          "状  态:",
                                          "Adding...");
                    lv_obj_add_state(lv_obj_get_child(lv_obj_get_parent(obj), 4), LV_STATE_DISABLED);
                    // add layertop clicked,bg color bg opa
                    lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
                    lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_50, 0);
                    // create spinner wait animation
                    meshadd_spinner = lv_spinner_create(lv_layer_sys(), 700, 60);
                    lv_obj_set_size(meshadd_spinner, LV_HOR_RES / 3, LV_VER_RES / 3);
                    lv_obj_center(meshadd_spinner);
                    // create report
                    lv_timer_t *mesh_add_dev_reporttimer = lv_timer_create(meshadd_report_timer, 20000, obj);
                    lv_timer_set_repeat_count(mesh_add_dev_reporttimer, 1);
                }
            }
        }
    }
}

static void mesh_dev_scan_obj_closebtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code       = lv_event_get_code(e);
    lv_obj_t *      obj_father = lv_event_get_user_data(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_del_async(obj_father);
    }
}

static void setting_mesh_scr_close_event_cb(lv_event_t *e)
{
    // clear layertop clicked
    lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_0, 0);
    lv_obj_t *obj = lv_event_get_user_data(e);
    // del event obj
    lv_obj_del_async(obj);
}

void meshadd_report_timer(lv_timer_t *timer)
{
    int       flag = 0;
    lv_obj_t *obj  = timer->user_data;
    int16_t   index;
    // get event index
    if (gw_evt_subdev_add) {
        index = gw_evt_subdev_add->subdev;
    } else
        index = 0;

    LOGD(TAG, "[///////////index:%d///////]\n", index);
    // meshdev add failed
    if (index == 0) {
        // change meshdev status label
        lv_label_set_text_fmt(
            lv_obj_get_child(lv_obj_get_parent(obj), 2), "#ffffff %s# #ff0000 %s#", "状  态: ", "Mesh Add Failed!!!");
        lv_obj_clear_state(obj, LV_STATE_DISABLED);
        lv_obj_clear_state(lv_obj_get_child(lv_obj_get_parent(obj), 4), LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(obj, lv_color_red(), 0);
        LOGD(TAG, "[///////////FAILED///////]\n");
        // del spinner animation
        lv_obj_del_async(meshadd_spinner);
        lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
    } else {
        for (int i = 0; i < 10; i++) {
            if (index == index_def.index_array[i]) {
                LOGD(TAG, "[///////////Again&Failed///////]\n");
                flag = 1;
                break;
            }
        }
        // meshdev event report
        if (!flag) {
            for (int i = 0; i < 10; i++) {
                if (index_def.index_array[i] == 0) {
                    // set inndexdef array
                    index_def.index_array[i] = index;
                    break;
                }
            }
            // change obj status
            lv_label_set_text_fmt(lv_obj_get_child(lv_obj_get_parent(obj), 2),
                                  "#ffffff %s# #00ff00 %s#",
                                  "状  态: ",
                                  "Mesh Add Success!!!");
            lv_obj_set_style_bg_color(obj, lv_color_make(0x0, 0xff, 0x0), 0);
            lv_obj_clear_state(lv_obj_get_child(lv_obj_get_parent(obj), 4), LV_STATE_DISABLED);
            // change origin obj img
            if (added_temp_obj) {
                lv_img_set_src(added_temp_obj, &img_scan_on);
                lv_obj_clear_flag(added_temp_obj, LV_OBJ_FLAG_CLICKABLE);
                added_temp_obj = NULL;
            } else {
                added_temp_obj = NULL;
            }
            // // //更新设备在线数
            // int deviceonline_cnt = meshonline_cnt();
            // lv_label_set_text_fmt(deviceonlinelabel, "#ffffff 设备在线   %d", deviceonline_cnt);
        } else {
            // change obj status
            lv_label_set_text_fmt(
                lv_obj_get_child(lv_obj_get_parent(obj), 2), "#fffff %s# #ff0000 %s#", "状  态:", "Mesh Add again!!!");
            lv_obj_clear_state(obj, LV_STATE_DISABLED);
            lv_obj_clear_state(lv_obj_get_child(lv_obj_get_parent(obj), 4), LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(obj, lv_color_red(), 0);
        }
        lv_obj_del_async(meshadd_spinner);
        lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
    }
    memset(&gw_evt_subdev_add, 0, sizeof(gw_evt_subdev_add));
}

static void mesh_dev_added_close_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_del_async(lv_obj_get_parent(obj));
    }
}

static void mesh_dev_added_delbtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      btn  = lv_event_get_target(e);
    // get event_cb obj index
    void *  p     = lv_event_get_user_data(e);
    int16_t index = *(int16_t *)p;
    LOGD(TAG, "index=====%d\n", index);
    gw_subdev_t subdev = index;

    if (code == LV_EVENT_CLICKED) {
        // del kv pos index
        char kv_index[10];
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", index);
        aos_kv_del(kv_index);
        // //更新设备在线数
        int deviceonline_cnt = meshonline_cnt();
        lv_label_set_text_fmt(deviceonlinelabel, "#ffffff 设备在线   %d", deviceonline_cnt);
        // del meshdev by index
        gateway_subdev_del(subdev);
        lv_obj_add_state(btn, LV_STATE_DISABLED);
        for (int i = 0; i < 10; i++) {
            if (index_def.index_array[i] == index) {
                index_def.index_array[i] = 0;
                break;
            }
        }
        // change origin obj status
        if (del_temp_obj) {
            // change img
            lv_img_set_src(del_temp_obj, &img_scan_off);
            lv_obj_clear_flag(del_temp_obj, LV_OBJ_FLAG_CLICKABLE);
            del_temp_obj = NULL;
            LOGD(TAG, "deldeldeldeldeldedledledledledl\n");
        } else {
            del_temp_obj = NULL;
        }
        lv_obj_del_delayed(lv_obj_get_parent(btn), 100);
        // set NODE index 0
        for (int i = 0; i < 10; i++) {
            if (NODE[i].index == index) {
                NODE[i].index = 0;
                LOGD(TAG, "deldeldeldeldeldedledledledledl\n");
                memset(NODE[i].pos, 0, sizeof(NODE[i].pos));
                break;
            }
        }
    }
}

static void mesh_showbtn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (lv_obj_get_child_cnt(mesh_scan_scr) > 0) {
        lv_obj_clean(mesh_scan_scr);
    }
    if (code == LV_EVENT_CLICKED) {
        //创建mesh群组
        char kv_group[20];
        char kv_groupname[32];
        lv_snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 0);
        int ret = aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
        LOGD(TAG, "ret %d %s", ret, kv_group);
        // if(ret >= 0)
        // {
        if (strcmp(kv_groupname, "所有设备组") != 0) {
            aos_kv_del(kv_group);
            gateway_subgrp_create("所有设备组");
        }
        // }
        memset(kv_group, 0, sizeof(kv_group));
        memset(kv_groupname, 0, sizeof(kv_groupname));
        lv_snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 1);
        ret = aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
        LOGD(TAG, "ret %d", ret);
        // if(ret >= 0)
        // {
        if (strcmp(kv_groupname, "灯光控制组") != 0) {
            aos_kv_del(kv_group);
            gateway_subgrp_create("灯光控制组");
        }
        // }
        memset(kv_group, 0, sizeof(kv_group));
        memset(kv_groupname, 0, sizeof(kv_groupname));
        lv_snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 2);
        ret = aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
        LOGD(TAG, "ret %d", ret);
        // if(ret >= 0)
        // {
        if (strcmp(kv_groupname, "窗帘控制组") != 0) {
            aos_kv_del(kv_group);
            gateway_subgrp_create("窗帘控制组");
        }
        // }
        // get add dev index
        int16_t       index;
        subdev_info_t subdev_info = { 0 };
        uint8_t       mac_str[20];
        for (int i = 1; i < 11; i++) {
            index   = i;
            int ret = gateway_subdev_get_info(index, &subdev_info);
            if (ret == 0) {
                bt_addr_val_to_str(subdev_info.dev_addr, (char *)mac_str, sizeof(mac_str));
                LOGD(TAG, "index%d get mac:%s active_status:%d\n", index, mac_str, (int)subdev_info.active_status);
                index_def.index_array[i - 1] = index;
            }
        }
        // get mesh_dev_added_obj
        if (mesh_dev_added_obj) {
            free(mesh_dev_added_obj);
            mesh_dev_added_obj = NULL;
        }
        // switch index 0
        int num = 0;
        for (int i = 0; i < 10; i++) {
            if (index_def.index_array[i] != 0) {
                num++;
            }
        }
        if (!mesh_dev_added_obj && num != 0) {
            mesh_dev_added_obj = (lv_obj_t **)malloc(sizeof(lv_obj_t *) * num);
            for (int i = 0; i < 10; i++) { ////judge NULL
                if (index_def.index_array[i] != 0) {
                    lv_obj_t *img = lv_img_create(mesh_scan_scr);
                    lv_img_set_src(img, &img_scan_on);
                    lv_img_set_zoom(img, LV_IMG_ZOOM_NONE * 2);
                    if (display_480p)
                        lv_img_set_zoom(img, LV_IMG_ZOOM_NONE);
                    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_add_event_cb(
                        img, mesh_dev_added_setting_obj_event_cb, LV_EVENT_CLICKED, (void *)&index_def.index_array[i]);
                }
            }
        }
    }
}

static void mesh_dev_added_onoff_obj_event_cb(lv_event_t *e)
{
    uint8_t on  = atoi("1");
    uint8_t off = atoi("0");
    // get user event index data
    void *          p     = lv_event_get_user_data(e);
    int16_t         index = *(int16_t *)p;
    lv_obj_t *      img   = lv_event_get_target(e);
    lv_event_code_t code  = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        if (lv_obj_has_state(img, LV_STATE_CHECKED)) {
            gateway_subdev_set_onoff(index, off);
            lv_img_set_src(img, &img_scan_off);
        } else {
            gateway_subdev_set_onoff(index, on);
            lv_img_set_src(img, &img_scan_on);
        }
    }
}

static void mesh_dev_added_setting_obj_event_cb(lv_event_t *e)
{
    lv_event_code_t code      = lv_event_get_code(e);
    lv_obj_t *      obj       = lv_event_get_target(e);
    del_temp_obj              = obj;
    void *        p           = lv_event_get_user_data(e);
    int16_t       index       = *(int16_t *)p;
    gw_subdev_t   subdev      = index;
    subdev_info_t subdev_info = { 0 };
    uint8_t       mac_str[20];
    int           ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        bt_addr_val_to_str(subdev_info.dev_addr, (char *)mac_str, sizeof(mac_str));
        LOGD(TAG, "index%d get mac:%s\n", index, mac_str);
    }

    if (code == LV_EVENT_CLICKED) {
        // home scr style
        static lv_style_t home_scr_style;
        lv_style_init(&home_scr_style);
        lv_style_set_shadow_color(&home_scr_style, lv_color_white());
        lv_style_set_shadow_width(&home_scr_style, 5);
        lv_style_set_bg_color(&home_scr_style, lv_color_black());
        lv_style_set_radius(&home_scr_style, 20);
        lv_style_set_border_opa(&home_scr_style, LV_OPA_0);
        // create mesh_dev_added_scr
        lv_obj_t *mesh_dev_added_scr = lv_obj_create(lv_layer_top());
        lv_obj_add_style(mesh_dev_added_scr, &home_scr_style, 0);
        lv_obj_set_size(mesh_dev_added_scr, lv_pct(95), lv_pct(70));
        lv_obj_center(mesh_dev_added_scr);
        lv_obj_clear_flag(mesh_dev_added_scr, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(mesh_dev_added_scr, LV_SCROLLBAR_MODE_OFF);
        // create mesh_dev_added_scr devimg
        lv_obj_t *mesh_dev_added_devimg = lv_img_create(mesh_dev_added_scr);
        lv_img_set_src(mesh_dev_added_devimg, &img_scan_on);
        lv_img_set_zoom(mesh_dev_added_devimg, LV_IMG_ZOOM_NONE * 2);
        if (display_480p)
            lv_img_set_zoom(mesh_dev_added_devimg, LV_IMG_ZOOM_NONE * 1.5);
        lv_obj_add_flag(mesh_dev_added_devimg, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(mesh_dev_added_devimg, mesh_dev_added_onoff_obj_event_cb, LV_EVENT_CLICKED, p);
        lv_obj_align(mesh_dev_added_devimg, LV_ALIGN_TOP_LEFT, 10, 40);
        // create mesh_dev_added_scr closeimg
        lv_obj_t *mesh_dev_added_close = lv_img_create(mesh_dev_added_scr);
        lv_img_set_src(mesh_dev_added_close, &img_close);
        // if(display_480p) lv_img_set_zoom(mesh_dev_added_close, 180);
        lv_obj_add_flag(mesh_dev_added_close, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(mesh_dev_added_close, mesh_dev_added_close_event_cb, LV_EVENT_CLICKED, 0);
        lv_obj_align(mesh_dev_added_close, LV_ALIGN_TOP_RIGHT, 0, 0);
        // create mesh_dev_added_scr typeselect_label
        lv_obj_t *mesh_dev_added_downlist_typeselect_label = lv_label_create(mesh_dev_added_scr);
        lv_obj_set_style_text_font(mesh_dev_added_downlist_typeselect_label, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(mesh_dev_added_downlist_typeselect_label, &lv_d1s_font_25, 0);
        lv_label_set_recolor(mesh_dev_added_downlist_typeselect_label, true);
        lv_label_set_text(mesh_dev_added_downlist_typeselect_label, "#ffffff 类型:");
        lv_obj_align_to(
            mesh_dev_added_downlist_typeselect_label, mesh_dev_added_devimg, LV_ALIGN_OUT_RIGHT_BOTTOM, 20, 0);
        // create mesh_dev_added_scr downlist_typeselect
        lv_obj_t *mesh_dev_added_downlist_typeselect = lv_dropdown_create(mesh_dev_added_scr);
        lv_obj_set_size(mesh_dev_added_downlist_typeselect, 140, 50);
        lv_obj_set_style_text_font(mesh_dev_added_downlist_typeselect, &lv_d1s_font_25, LV_PART_MAIN);
        lv_dropdown_set_symbol(mesh_dev_added_downlist_typeselect, &img_next);
        lv_obj_align_to(
            mesh_dev_added_downlist_typeselect, mesh_dev_added_downlist_typeselect_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_obj_set_style_text_font(mesh_dev_added_downlist_typeselect, &lv_d1s_font_30, 0);
        if (display_480p)
            lv_obj_set_style_text_font(mesh_dev_added_downlist_typeselect, &lv_d1s_font_22, 0);
        // create dropdownstyle
        lv_obj_t *dropdownstyle = lv_dropdown_get_list(mesh_dev_added_downlist_typeselect);
        lv_obj_set_style_align(dropdownstyle, LV_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(dropdownstyle, &lv_d1s_font_35, LV_PART_MAIN);
        if (display_480p)
            lv_obj_set_style_text_font(dropdownstyle, &lv_d1s_font_25, LV_PART_MAIN);
        lv_obj_set_style_max_height(dropdownstyle, LV_HOR_RES / 3, 0);
        //再次点击后，判断index是否已经选过位置，是则显示已选位置
        lv_dropdown_set_options(mesh_dev_added_downlist_typeselect,
                                "\n"
                                "窗帘\n"
                                "客厅灯\n"
                                "卧室灯\n"
                                "厨房灯\n"
                                "无线开关1\n"
                                "无线开关2\n"
                                "无线贴\n");
        lv_obj_set_style_max_height(mesh_dev_added_downlist_typeselect, LV_HOR_RES / 3, 0);
        lv_obj_add_event_cb(
            mesh_dev_added_downlist_typeselect, mesh_dev_added_downlist_typeselect_event_cb, LV_EVENT_VALUE_CHANGED, p);

        char kv_pos[32];
        char kv_index[10];
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", index);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret < 0) {
            lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 0);
        } else {
            if (strcmp(kv_pos, "窗帘") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 1);
            }
            if (strcmp(kv_pos, "客厅灯") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 2);
            }
            if (strcmp(kv_pos, "卧室灯") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 3);
            }
            if (strcmp(kv_pos, "厨房灯") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 4);
            }
            if (strcmp(kv_pos, "无线开关1") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 5);
            }
            if (strcmp(kv_pos, "无线开关2") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 6);
            }
            if (strcmp(kv_pos, "无线贴") == 0) {
                lv_dropdown_set_selected(mesh_dev_added_downlist_typeselect, 7);
            }
        }

        // create maclabel
        lv_obj_t *mesh_dev_added_maclabel = lv_label_create(mesh_dev_added_scr);
        lv_obj_set_style_text_font(mesh_dev_added_maclabel, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(mesh_dev_added_maclabel, &lv_d1s_font_25, 0);
        lv_label_set_recolor(mesh_dev_added_maclabel, true);
        lv_label_set_text_fmt(mesh_dev_added_maclabel, "#ffffff MAC地址:  %s", mac_str);
        lv_obj_align(mesh_dev_added_maclabel, LV_ALIGN_LEFT_MID, LV_HOR_RES / 20, LV_VER_RES / 15);
        // create statuslabel
        lv_obj_t *mesh_dev_added_statuslabel = lv_label_create(mesh_dev_added_scr);
        lv_obj_set_style_text_font(mesh_dev_added_statuslabel, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(mesh_dev_added_statuslabel, &lv_d1s_font_25, 0);
        lv_label_set_recolor(mesh_dev_added_statuslabel, true);
        lv_label_set_recolor(mesh_dev_added_statuslabel, true);
        lv_label_set_text_fmt(mesh_dev_added_statuslabel, "#ffffff %s# #00ff00 %s#", "状   态:", "Join Success!");
        lv_obj_align_to(
            mesh_dev_added_statuslabel, mesh_dev_added_maclabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_VER_RES / 15);
        // create del button
        lv_obj_t *mesh_dev_added_delbtn = lv_btn_create(mesh_dev_added_scr);
        lv_obj_set_size(mesh_dev_added_delbtn, lv_pct(25), lv_pct(15));
        lv_obj_set_style_radius(mesh_dev_added_delbtn, 5, 0);
        lv_obj_align(mesh_dev_added_delbtn, LV_ALIGN_BOTTOM_RIGHT, -10, -25);
        lv_obj_add_event_cb(mesh_dev_added_delbtn, mesh_dev_added_delbtn_event_cb, LV_EVENT_CLICKED, p);
        lv_obj_t *mesh_dev_added_delbtn_label = lv_label_create(mesh_dev_added_delbtn);
        lv_obj_set_style_text_font(mesh_dev_added_delbtn_label, &lv_d1s_font_35, 0);
        if (display_480p)
            lv_obj_set_style_text_font(mesh_dev_added_delbtn_label, &lv_d1s_font_25, 0);
        lv_label_set_text(mesh_dev_added_delbtn_label, "删 除");
        lv_obj_center(mesh_dev_added_delbtn_label);
    }
}

static void mesh_dev_added_downlist_typeselect_event_cb(lv_event_t *e)
{
    lv_event_code_t code     = lv_event_get_code(e);
    lv_obj_t *      downlist = lv_event_get_target(e);
    // get event user data
    void *  p     = lv_event_get_user_data(e);
    int16_t index = *(int16_t *)p;
    char    selectbuf[20];
    memset(selectbuf, 0, sizeof(selectbuf));
    // set meshdev index
    gw_subdev_t   subdev      = index;
    subdev_info_t subdev_info = { 0 };
    uint8_t       mac_str[20];
    char          kv_index[10];
    int           ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        bt_addr_val_to_str(subdev_info.dev_addr, (char *)mac_str, sizeof(mac_str));
        LOGD(TAG, "index%d get mac:%s\n", index, mac_str);
    }

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_dropdown_get_selected_str(downlist, selectbuf, sizeof(selectbuf));
        LOGD(TAG, "[select [%s] %d]\n", selectbuf, strlen(selectbuf));
        //添加组
        //
        if (strcmp(selectbuf, "") != 0) {
            if (strcmp(selectbuf, "无线开关1") != 0 && strcmp(selectbuf, "无线开关2") != 0
                && strcmp(selectbuf, "无线贴") != 0) {
                LOGD(TAG, "[[[[[[[[[[[[[[[[[[[[[[[[[加入了所有设备组[]]]]]]]]]]]]]]]]]]]]]]]]");
                gateway_subgrp_add_subdev(0, index);
                if (strcmp(selectbuf, "卧室灯") == 0 || strcmp(selectbuf, "厨房灯") == 0
                    || strcmp(selectbuf, "客厅灯") == 0) {
                    gateway_subgrp_add_subdev(1, index);
                    LOGD(TAG, "[[[[[[[[[[[[[[[[[[[[[[[[[加入了照明灯组[]]]]]]]]]]]]]]]]]]]]]]]]");
                    // break;
                } else if (strcmp(selectbuf, "窗帘") == 0) {
                    LOGD(TAG, "[[[[[[[[[[[[[[[[[[[[[[[[[加入了窗帘组组[]]]]]]]]]]]]]]]]]]]]]]]]");
                    gateway_subgrp_add_subdev(2, index);
                    // break;
                }
            }
        }
        //删除
        // gateway_subgrp_del_subdev(0, index);
        // selectbuf 为null 表示选了个空位置 要把他这个index pos删除
        if (strlen(selectbuf) == 0) {
            //删除kv中该节点的信息
            memset(kv_index, 0, sizeof(kv_index));
            lv_snprintf(kv_index, sizeof(kv_index), "index%d", index);
            aos_kv_del(kv_index);
            // //更新设备在线数
            int deviceonline_cnt = meshonline_cnt();
            lv_label_set_text_fmt(deviceonlinelabel, "#ffffff 设备在线   %d", deviceonline_cnt);
            //更新NODE
            for (int i = 0; i < 10; i++) {
                if (NODE[i].index == index) {
                    NODE[i].index = 0;
                    memset(NODE[i].pos, 0, sizeof(NODE[i].pos));
                }
            }
        } else {
            for (int i = 0; i < 10; i++) {
                //更新kv位置列表
                memset(kv_index, 0, sizeof(kv_index));
                lv_snprintf(kv_index, sizeof(kv_index), "index%d", index);
                aos_kv_setstring(kv_index, selectbuf);
                // //更新设备在线数
                int deviceonline_cnt = meshonline_cnt();
                lv_label_set_text_fmt(deviceonlinelabel, "#ffffff 设备在线   %d", deviceonline_cnt);
                //如果selectbuf不为空，并且NODE之前有存过index信息 直接更新pos
                if (NODE[i].index == index) {
                    strcpy(NODE[i].pos, selectbuf);
                    break;
                }
                //如果selectbuf不为空，NODE没存过index信息，找个0的位置存index和pos
                else if (NODE[i].index == 0)
                {
                    NODE[i].index = index;
                    strcpy(NODE[i].pos, selectbuf);
                    break;
                }
            }
        }
    }
}

static void mesh_scr_devctl_listbtn_event_cb(lv_event_t *e)
{
    uint8_t on  = atoi("1");
    uint8_t off = atoi("0");
    // int16_t *indextmp = (int16_t*)lv_event_get_user_data(e);
    // int16_t index = *indextmp;
    int16_t index = (size_t)lv_event_get_user_data(e);
    LOGD(TAG, "index = %d", index);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    char            kv_index[10];
    char            kv_pos[32];
    if (code == LV_EVENT_SHORT_CLICKED) {
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", index);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "卧室灯") == 0) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_bedroom_100);
                    gateway_subdev_set_onoff(index, on);
                } else {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_80, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_bedroom_0);
                    gateway_subdev_set_onoff(index, off);
                }
            }
            if (strcmp(kv_pos, "窗帘") == 0) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_curtain_on);
                    gateway_subdev_set_onoff(index, on);
                } else {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_80, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_curtain_off);
                    gateway_subdev_set_onoff(index, off);
                }
            } else if (strcmp(kv_pos, "厨房灯") == 0) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_foodlight_on);
                    gateway_subdev_set_onoff(index, on);
                } else {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_80, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_foodlight_off);
                    gateway_subdev_set_onoff(index, off);
                }
            } else if (strcmp(kv_pos, "客厅灯") == 0) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_livingled_on);
                    gateway_subdev_set_onoff(index, on);
                } else {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_80, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_livingled_off);
                    gateway_subdev_set_onoff(index, off);
                }
            } else if (strcmp(kv_pos, "无线开关1") == 0 || strcmp(kv_pos, "无线开关2") == 0) {
                if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_wifisw_on);
                    gateway_subdev_set_onoff(index, on);
                } else {
                    // lv_obj_set_style_bg_opa(obj, LV_OPA_80, 0);
                    lv_img_set_src(lv_obj_get_child(obj, 0), &img_wifisw_off);
                    gateway_subdev_set_onoff(index, off);
                }
            }
        }
    }
}

static void album_gesture_event_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_LEFT)
        lv_demo_music_album_next(true);
    if (dir == LV_DIR_RIGHT)
        lv_demo_music_album_next(false);
}
void lv_demo_music_album_next(bool next)
{
    uint32_t id = track_id;
    if (next) {
        id++;
        if (id >= ACTIVE_TRACK_CNT)
            id = 0;
    } else {
        if (id == 0) {
            id = ACTIVE_TRACK_CNT - 1;
        } else {
            id--;
        }
    }
    track_load(id);
}
static lv_obj_t *album_img_create(lv_obj_t *parent)
{
    lv_obj_t *img;
    img = lv_img_create(parent);
    switch (track_id) {
        case 2: // switch song 2
            lv_img_set_src(img, &img_record);
            // lv_obj_add_event_cb(play_obj, play_obj_event_click_cb, LV_EVENT_CLICKED, NULL);
            lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            lv_label_set_text(music_label_name, "#ffffff Music 3");
            lv_label_set_text(music_label_author, "#ffffff Author 3");
            break;
        case 1: // switch song 1
            lv_img_set_src(img, &img_record);
            // lv_obj_add_event_cb(play_obj, play_obj_event_click_cb, LV_EVENT_CLICKED, NULL);
            lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            lv_label_set_text(music_label_name, "#ffffff Music 2");
            lv_label_set_text(music_label_author, "#ffffff Author 2");
            break;
        case 0: // switch song 0
            lv_img_set_src(img, &img_record);
            // lv_obj_add_event_cb(play_obj, play_obj_event_click_cb, LV_EVENT_CLICKED, NULL);
            lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            lv_label_set_text(music_label_name, "#ffffff Music 1");
            lv_label_set_text(music_label_author, "#ffffff Author 1");
            break;
    }
    lv_img_set_antialias(img, false);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(img, album_gesture_event_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    return img;
}
static void album_fade_anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_img_opa(var, v, 0);
}
static void _obj_set_x_anim_cb(void *obj, int32_t x)
{
    lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)x);
}
static void _img_set_zoom_anim_cb(void *obj, int32_t zoom)
{
    lv_img_set_zoom((lv_obj_t *)obj, (uint16_t)zoom);
}
static void track_load(uint32_t id)
{
    if (id == track_id)
        return;
    bool next = false;
    if ((track_id + 1) % ACTIVE_TRACK_CNT == id)
        next = true;
    track_id = id;
    // create fade animation
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_values(&a, lv_obj_get_style_img_opa(album_img_obj, 0), LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, album_fade_anim_cb);
    lv_anim_set_time(&a, 500);
    lv_anim_start(&a);
    // create path animation
    lv_anim_init(&a);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    // get next troad id
    if (next) {
        lv_anim_set_values(&a, 0, -LV_HOR_RES / 2);
    } else {
        lv_anim_set_values(&a, 0, LV_HOR_RES / 2);
    }
    // set animation exec_cb
    lv_anim_set_exec_cb(&a, _obj_set_x_anim_cb);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE / 2);
    if (display_480p)
        lv_anim_set_values(&a, 220, LV_IMG_ZOOM_NONE / 2);
    lv_anim_set_exec_cb(&a, _img_set_zoom_anim_cb);
    lv_anim_set_ready_cb(&a, NULL);
    lv_anim_start(&a);
    // create music scr img
    album_img_obj = album_img_create(music_scr);
    lv_obj_align(album_img_obj, LV_ALIGN_CENTER, 0, 0);
    // set animation exec_cb
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_delay(&a, 100);
    lv_anim_set_values(&a, LV_IMG_ZOOM_NONE / 2, LV_IMG_ZOOM_NONE);
    if (display_480p)
        lv_anim_set_values(&a, 220, LV_IMG_ZOOM_NONE);
    lv_anim_set_exec_cb(&a, _img_set_zoom_anim_cb);
    lv_anim_set_ready_cb(&a, NULL);
    lv_anim_start(&a);
    // create fade animation
    lv_anim_init(&a);
    lv_anim_set_var(&a, album_img_obj);
    lv_anim_set_values(&a, 0, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, album_fade_anim_cb);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_delay(&a, 100);
    lv_anim_start(&a);
}

static void prev_click_event_cb(lv_event_t *e)

{
    LV_UNUSED(e);
    // lv_event_code_t code = lv_event_get_code(e);
    // if(code == LV_EVENT_CLICKED) {
    lv_demo_music_album_next(false);
    //     if(lv_obj_has_state(play_obj, LV_STATE_CHECKED))
    //     {
    //         music_anim_status(true);
    //     }
    // }
}
static void next_click_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    // lv_event_code_t code = lv_event_get_code(e);
    // if(code == LV_EVENT_CLICKED) {
    lv_demo_music_album_next(true);
    // if(lv_obj_has_state(play_obj, LV_STATE_CHECKED))
    // {
    //     music_anim_status(true);
    // }
    //}
}

static void set_img_angle(void *img, int32_t v)
{
    lv_img_set_angle(img, v);
}

void layersys_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *spinner = timer->user_data;
    lv_obj_del_async(spinner);
    lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
    lv_obj_clear_state(wifiscan_btn, LV_STATE_DISABLED);
}

static void my_btn_scan_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        /* 清空 list的子对象 */
        if (lv_obj_get_child_cnt(wifiscan_list) > 0) {
            lv_obj_clean(wifiscan_list);
        }
        // update wifi scan start event
        app_event_update(EVENT_GUI_USER_WIFI_SCAN_START);
        lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_50, 0);
        // create spinner wait animation
        lv_obj_t *spinner = lv_spinner_create(lv_layer_sys(), 800, 100);
        lv_obj_set_size(spinner, lv_pct(30), lv_pct(30));
        lv_obj_center(spinner);
        // create layersys timer
        lv_obj_add_state(wifiscan_btn, LV_STATE_DISABLED);
        lv_timer_t *layersys_timer = lv_timer_create(layersys_timer_cb, 8000, spinner);
        lv_timer_set_repeat_count(layersys_timer, 1);
    }
}

static void ok_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        // clear wifi old setting
        memset(user_psk, 0, sizeof(user_psk));
        memset(user_ssid, 0, sizeof(user_ssid));
        strncpy(user_psk, lv_textarea_get_text(pwd_ta), strlen(lv_textarea_get_text(pwd_ta)));
        strncpy(user_ssid, lv_label_get_text(ssid_label), strlen(lv_label_get_text(ssid_label)));
        lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
        // update wifilink ssid
        lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", lv_label_get_text(ssid_label));
        lv_label_set_text(wifilink_statuslabel, "#ffffff 连接中...");
        // uppdate wifi scan
        app_event_update(EVENT_GUI_USER_WIFI_PROV_START);
        for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
            lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_make(220, 220, 200), 0);
            // lv_obj_set_style_bg_opa(obj, LV_OPA_30, 0);
        }
        lv_obj_del_async(obj->parent);
    }
}

static void exit_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *      obj  = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_clear_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_0, 0);
        lv_obj_del_async(obj->parent);
    }
}

static void my_list_ssid_select_event_handler(lv_event_t *e)
{
    lv_obj_t *label;
    lv_obj_t *obj = lv_event_get_target(e);
    // add layer_sys clicked
    lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
    lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_50, 0);
    // create inputbox on layersys
    input_box = lv_obj_create(lv_layer_sys());
    lv_obj_set_size(input_box, LV_HOR_RES * 3 / 4, LV_VER_RES / 2);
    lv_obj_center(input_box);
    lv_obj_clear_flag(input_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(input_box, LV_SCROLLBAR_MODE_OFF);
    // create keyboard
    lv_obj_t *kb = lv_keyboard_create(lv_layer_sys());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(kb, &lv_font_montserrat_34, 0);
    if (display_480p)
        lv_obj_set_style_text_font(kb, &lv_font_montserrat_24, 0);
    // create inputbox img
    lv_obj_t *input_box_img = lv_img_create(input_box);
    lv_img_set_src(input_box_img, &img_wifi_L);
    if (display_480p)
        lv_img_set_zoom(input_box_img, 180);
    lv_obj_align(input_box_img, LV_ALIGN_LEFT_MID, LV_HOR_RES / 12, -(LV_VER_RES / 14));
    if (display_480p)
        lv_obj_align(input_box_img, LV_ALIGN_LEFT_MID, LV_HOR_RES / 18, -(LV_VER_RES / 14));
    /*Create the password box*/
    pwd_ta = lv_textarea_create(input_box);
    lv_textarea_set_text(pwd_ta, "");
    lv_textarea_set_password_mode(pwd_ta, true);
    lv_textarea_set_one_line(pwd_ta, true);
    lv_obj_set_width(pwd_ta, lv_pct(50));
    lv_textarea_set_placeholder_text(pwd_ta, "Password");
    lv_obj_align_to(pwd_ta, input_box_img, LV_ALIGN_OUT_RIGHT_BOTTOM, LV_HOR_RES / 16, 0);
    if (display_480p)
        lv_obj_align_to(pwd_ta, input_box_img, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    lv_obj_add_event_cb(pwd_ta, ta_event_cb, LV_EVENT_ALL, kb);
    /*Create a label and position it above the text box*/
    ssid_label = lv_label_create(input_box);
    lv_obj_set_style_text_font(ssid_label, &lv_d1s_font_30, 0);
    if (display_480p)
        lv_obj_set_style_text_font(ssid_label, &lv_d1s_font_22, 0);
    lv_obj_align_to(ssid_label, pwd_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    lv_label_set_long_mode(ssid_label, LV_LABEL_LONG_WRAP);
    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
        lv_obj_t *child = lv_obj_get_child(wifiscan_list, i);
        /*Do something with child*/
        if (child == obj) {
            LOGI(TAG, "Selected item %d :ssid is %s", i, lv_list_get_btn_text(wifiscan_list, obj));
            lv_label_set_text(ssid_label, lv_list_get_btn_text(wifiscan_list, obj));
            lv_label_set_long_mode(ssid_label, LV_LABEL_LONG_WRAP);
            break;
        }
    }
    // create button ok clicked
    lv_obj_t *btn_ok = lv_btn_create(input_box);
    lv_obj_set_size(btn_ok, lv_pct(30), lv_pct(15));
    lv_obj_align(btn_ok, LV_ALIGN_BOTTOM_MID, -(LV_HOR_RES) / 8, -(LV_VER_RES / 12));
    lv_obj_add_event_cb(btn_ok, ok_btn_event_cb, LV_EVENT_ALL, NULL);
    // create button ok label
    label = lv_label_create(btn_ok);
    lv_obj_set_style_text_font(label, &lv_d1s_font_30, 0);
    if (display_480p)
        lv_obj_set_style_text_font(label, &lv_d1s_font_22, 0);
    lv_label_set_text(label, "加入");
    lv_obj_center(label);
    // create button exit
    lv_obj_t *btn_exit = lv_btn_create(input_box);
    lv_obj_set_size(btn_exit, lv_pct(30), lv_pct(15));
    lv_obj_align(btn_exit, LV_ALIGN_BOTTOM_MID, (LV_HOR_RES) / 8, -(LV_VER_RES / 12));
    lv_obj_add_event_cb(btn_exit, exit_btn_event_cb, LV_EVENT_ALL, NULL);
    // create button exit label
    label = lv_label_create(btn_exit);
    lv_obj_set_style_text_font(label, &lv_d1s_font_30, 0);
    if (display_480p)
        lv_obj_set_style_text_font(label, &lv_d1s_font_22, 0);
    lv_label_set_text(label, "取消");
    lv_obj_center(label);
}

static void gw_main_event_handler(uint32_t event_id, const void *data, void *context)
{
    // LOGD(TAG, "GUI Event ID = %x", event_id);
    uint8_t on  = atoi("1");
    uint8_t off = atoi("0");
    switch (event_id) {
        case EVENT_GUI_USER_MESH_SCAN_INFO_UPDATE: {
            scan_msg = (gw_evt_discovered_info_t *)data;
            // debug
            // uint8_t dev_addr_str[20];
            // uint8_t uuid_str[40];
            // bt_addr_val_to_str(scan_msg->protocol_info.ble_mesh_info.dev_addr, (char *)dev_addr_str,
            // sizeof(dev_addr_str)); bt_uuid_val_to_str(scan_msg->protocol_info.ble_mesh_info.uuid, (char *)uuid_str,
            // sizeof(uuid_str)); LOGD(TAG, "[----->EVENT_GUI_USER_MESH_SCAN_INFO_UPDATE:%p,[%s][%s]<-----]\r\n", data,
            // dev_addr_str, uuid_str);
            break;
        }
        case EVENT_GUI_USER_MESH_ADD_INFO_UPDATE: {
            gw_evt_subdev_add = (gw_evt_subdev_add_t *)data;
            int16_t index     = gw_evt_subdev_add->subdev;
            // debug
            LOGD(TAG, "[[[[[[[index:%d]]]]]]]\r\n", index);
            break;
        }
        case EVENT_GUI_USER_MESH_STATUS_ONOFF_UPDATE: {
            char                 kv_index[10];
            char                 kv_pos[32];
            struct _updatestatus onoffstatus = *(struct _updatestatus *)data;
            LOGD(TAG, "[subdev_index:%d. onoff:%d]\n", onoffstatus.subdev, onoffstatus.onoff);

            lv_snprintf(kv_index, sizeof(kv_index), "index%d", (int)onoffstatus.subdev);
            int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
            if (ret >= 0) {
                if (strcmp(kv_pos, "无线开关1") == 0) {
                    LOGD(TAG, "SWITCH1 ui start\n");
                    if (onoffstatus.onoff) {
                        gateway_subgrp_set_onoff(1, onoffstatus.onoff);
                    } else {
                        gateway_subgrp_set_onoff(1, onoffstatus.onoff);
                    }
                } else if (strcmp(kv_pos, "无线开关2") == 0) {
                    LOGD(TAG, "SWITCH2 ui start\n");
                    gateway_subgrp_set_onoff(2, onoffstatus.onoff);
                    if (onoffstatus.onoff) {
                        //     LOGD(TAG,"OPEN ALL DEVICE without switch 1 2");
                        //     for(int i = 1; i < 11; i++)
                        //     {
                        //         lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                        //         ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                        //         if(ret >= 0)
                        //         {
                        //             if(strcmp(kv_pos, "窗帘") == 0)
                        //             {
                        //                 gw_subdev_t index = (gw_subdev_t)i;
                        //                 gateway_subdev_set_onoff(index, on);
                        //             }
                        //         }
                        //     }
                    } else {
                        LOGD(TAG, "CLOSE ALL DEVICE without switch1&2");
                        gateway_subgrp_set_onoff(2, onoffstatus.onoff);
                        // for(int i = 1; i < 11; i++)
                        // {
                        //     lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                        //     ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                        //     if(ret >= 0)
                        //     {
                        //         if(strcmp(kv_pos, "窗帘") == 0)
                        //         {
                        //             gw_subdev_t index = (gw_subdev_t)i;
                        //             gateway_subdev_set_onoff(index, off);
                        //         }
                        //     }
                        // }
                    }
                }
            }
            break;
        }
        case EVENT_STATUS_WIFI_CONN_SUCCESS:
        case EVENT_STATUS_NTP_SUCCESS: {
            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            lv_img_set_src(homewifistatus, &img_homewifistatus_on);

            // LOGD(TAG, "EVENT_STATUS_NTP_SUCCESS");
            if (wifilink_statuslabel && wifilink_ssid) {
                lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", app_wifi_config_get_used_ssid());
                lv_label_set_text(wifilink_statuslabel, "#00ff00 连接成功");
            }
            for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
                lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
                lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
            }
            aos_mutex_unlock(&lvgl_mutex);
            break;
        }
        case EVENT_GUI_USER_WIFI_PSK_ERR: {
            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            lv_img_set_src(homewifistatus, &img_homewifistatus);
            LOGD(TAG, "[[[[[password error]]]]]");
            if (wifilink_statuslabel && wifilink_ssid) {
                lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", app_wifi_config_get_used_ssid());
                lv_label_set_text(wifilink_statuslabel, "#ff0000 密码错误");
            }
            for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
                lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
                lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
            }
            aos_mutex_unlock(&lvgl_mutex);
            break;
        }
        case EVENT_GUI_USER_WIFI_NOEXIST: {
            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            lv_img_set_src(homewifistatus, &img_homewifistatus);
            LOGD(TAG, "[[[[[AP not found]]]]]");
            if (wifilink_statuslabel && wifilink_ssid) {
                lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", app_wifi_config_get_used_ssid());
                lv_label_set_text(wifilink_statuslabel, "#ff0000 配网超时");
            }
            for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
                lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
                lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
            }
            aos_mutex_unlock(&lvgl_mutex);
            break;
        }
        case EVENT_STATUS_WIFI_CONN_FAILED: {
            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            lv_img_set_src(homewifistatus, &img_homewifistatus);
            LOGD(TAG, "[[[[[Net down]]]]]");
            if (wifilink_statuslabel && wifilink_ssid) {
                if (app_wifi_config_get_used_ssid()) {
                    lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", app_wifi_config_get_used_ssid());
                    lv_label_set_text(wifilink_statuslabel, "#ff0000 配网失败");
                }
            }
            for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
                lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
                lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
            }
            aos_mutex_unlock(&lvgl_mutex);
            break;
        }
        case EVENT_STATUS_WIFI_PROV_FAILED: {
            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            lv_img_set_src(homewifistatus, &img_homewifistatus);
            LOGD(TAG, "[[[[[Net check error]]]]]");
            if (wifilink_statuslabel && wifilink_ssid) {
                if (app_wifi_config_get_used_ssid()) {
                    lv_label_set_text_fmt(wifilink_ssid, "#ffffff %s", app_wifi_config_get_used_ssid());
                    lv_label_set_text(wifilink_statuslabel, "#ff0000 连接失败");
                }
            }
            for (int i = 0; i < lv_obj_get_child_cnt(wifiscan_list); i++) {
                lv_obj_t *obj = lv_obj_get_child(wifiscan_list, i);
                lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
            }
            aos_mutex_unlock(&lvgl_mutex);

            break;
        }
        case EVENT_GUI_USER_WIFI_SCAN_FINISH: {
            int i = 0;

            aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
            while (g_app_wifi_user_scan_table[i].rssi != 0) {
                if (check_wifi_ssid_name(g_app_wifi_user_scan_table[i].ssid) != 1) {
                    // wifilist_btn = lv_list_add_btn(wifiscan_list, LV_SYMBOL_WIFI,
                    // (char*)g_app_wifi_user_scan_table[i].ssid);
                    wifilist_btn   = lv_list_add_btn(wifiscan_list, LV_SYMBOL_WIFI, NULL);
                    lv_obj_t *text = lv_label_create(wifilist_btn);
                    lv_obj_set_width(text, 200);
                    lv_label_set_text(text, (char *)g_app_wifi_user_scan_table[i].ssid);
                    lv_label_set_long_mode(text, LV_LABEL_LONG_WRAP);
                    lv_obj_add_event_cb(wifilist_btn, my_list_ssid_select_event_handler, LV_EVENT_CLICKED, NULL);
                }
                i++;
            }
            aos_mutex_unlock(&lvgl_mutex);
            break;
        }
        case EVENT_GUI_USER_WIFI_PROV_START: {
            LOGD(TAG, "EVENT_GUI_USER_WIFI_PROV_START event");
            extern netmgr_hdl_t app_netmgr_hdl;
            app_wifi_config_add(user_ssid, user_psk);
            netmgr_config_wifi(app_netmgr_hdl, user_ssid, strlen(user_ssid), user_psk, strlen(user_psk));
            rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
            rvm_hal_net_set_hostname(dev, "T-head");
            netmgr_start(app_netmgr_hdl);
            break;
        }
        case EVENT_GUI_USER_VOICEASRRESULT_UPDATE: {
            char kv_index[10];
            char kv_pos[10];
            LOGD(TAG, "speaker:%s,addr:%p\n", (char *)data, data);
            if (strcmp(data, "打开卧室灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "卧室灯") == 0) {
                        gateway_subdev_set_onoff(i, on);
                    }
                }
            } else if (strcmp(data, "关闭卧室灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "卧室灯") == 0) {
                        gateway_subdev_set_onoff(i, off);
                    }
                }
            }
            if (strcmp(data, "打开厨房灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "厨房灯") == 0) {
                        gateway_subdev_set_onoff(i, on);
                    }
                }
            } else if (strcmp(data, "关闭厨房灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "厨房灯") == 0) {
                        gateway_subdev_set_onoff(i, off);
                    }
                }
            }
            if (strcmp(data, "打开客厅灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "客厅灯") == 0) {
                        gateway_subdev_set_onoff(i, on);
                    }
                }
            } else if (strcmp(data, "关闭客厅灯") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "客厅灯") == 0) {
                        gateway_subdev_set_onoff(i, off);
                    }
                }
            }
            if (strcmp(data, "打开窗帘") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "窗帘") == 0) {
                        gateway_subdev_set_onoff(i, on);
                    }
                }
            } else if (strcmp(data, "关闭窗帘") == 0) {
                for (int i = 1; i < 11; i++) {
                    lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
                    aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                    if (strcmp(kv_pos, "窗帘") == 0) {
                        gateway_subdev_set_onoff(i, off);
                    }
                }
            } else if (strcmp(data, "暂停") == 0 || strcmp(data, "停止") == 0) {
                LOGD(TAG, "暂停&停止");
                if (lv_obj_has_state(play_obj, LV_STATE_CHECKED) == true) {
                    lv_img_set_src(play_obj, &img_musicplay);
                    // close all music
                    // smtaudio_stop(SMTAUDIO_TYPE_ALL);
                    // music animation stop
                    lv_obj_clear_state(play_obj, LV_STATE_CHECKED);
                    music_anim_status(false);
                }
            } else if (strcmp(data, "回家模式") == 0) {
                lv_obj_add_state(scene_scr_array[0], LV_STATE_CHECKED);
                lv_event_send(scene_scr_array[0], LV_EVENT_CLICKED, 0);
            } else if (strcmp(data, "离家模式") == 0) {
                lv_obj_add_state(scene_scr_array[1], LV_STATE_CHECKED);
                lv_event_send(scene_scr_array[1], LV_EVENT_CLICKED, 0);
            } else if (strcmp(data, "会议模式") == 0) {
                lv_obj_add_state(scene_scr_array[2], LV_STATE_CHECKED);
                lv_event_send(scene_scr_array[2], LV_EVENT_CLICKED, 0);
            } else if (strcmp(data, "电影模式") == 0) {
                lv_obj_add_state(scene_scr_array[3], LV_STATE_CHECKED);
                lv_event_send(scene_scr_array[3], LV_EVENT_CLICKED, 0);
            } else if (strcmp(data, "开所有灯") == 0) {
                gateway_subgrp_set_onoff(1, 1);
            } else if (strcmp(data, "关所有灯") == 0) {
                gateway_subgrp_set_onoff(1, 0);
            }
            break;
        }
        case FOTA_EVENT_START:
        case EVENT_GUI_USER_FOTA_START_UPDATE: {
            // lv_obj_clean(lv_layer_sys());
            LOGD(TAG, "/***********GUI get FOTA_EVENT_START check version%s\n***********/\n", (char *)data);
            // crate layersys flag
            lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(lv_layer_sys(), lv_color_grey(), 0);
            lv_obj_set_style_bg_opa(lv_layer_sys(), LV_OPA_70, 0);
            // create fotaimg
            lv_obj_t *fota_img = lv_img_create(lv_layer_sys());
            lv_img_set_src(fota_img, &img_occ);
            lv_obj_align(fota_img, LV_ALIGN_CENTER, 0, -(LV_VER_RES / 10));
            // create fota label
            lv_obj_t *fota_label = lv_label_create(lv_layer_sys());
            lv_obj_set_style_text_font(fota_label, &lv_d1s_font_25, 0);
            if (display_480p)
                lv_obj_set_style_text_font(fota_label, &lv_d1s_font_30, 0);
            lv_label_set_text(fota_label, "检测到新固件......");
            lv_obj_align(fota_label, LV_ALIGN_CENTER, 0, LV_VER_RES / 7);
            // create fota label1
            lv_obj_t *fota_label1 = lv_label_create(lv_layer_sys());
            lv_obj_set_style_text_font(fota_label1, &lv_d1s_font_25, 0);
            if (display_480p)
                lv_obj_set_style_text_font(fota_label1, &lv_d1s_font_22, 0);
            lv_label_set_text_fmt(fota_label1, "正在升级:%s", (char *)data);
            lv_label_set_long_mode(fota_label1, LV_LABEL_LONG_SCROLL_CIRCULAR);
            lv_obj_set_width(fota_label1, lv_pct(45));
            lv_obj_align_to(fota_label1, fota_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

            fota_progresslabel = lv_label_create(lv_layer_sys());
            lv_obj_set_style_text_font(fota_progresslabel, &lv_d1s_font_25, 0);
            lv_obj_align_to(fota_progresslabel, fota_label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            lv_label_set_text(fota_progresslabel, "0%");
            // create spinner wait animation
            lv_obj_t *fota_spinner = lv_spinner_create(lv_layer_sys(), 600, 60);
            lv_obj_set_size(fota_spinner, lv_pct(75), lv_pct(75));
            lv_obj_center(fota_spinner);
            break;
        }
        case FOTA_EVENT_VERSION: {
            LOGD(TAG, "/***********GUI get FOTA_EVENT_VERSION***********/\n");
            break;
        }
        // case FOTA_EVENT_PROGRESS:
        case EVENT_GUI_USER_FOTA_PROGRESS: {
            fotaupdate_progress = *(int *)data;

            if (fotaupdate_progress)
                lv_label_set_text_fmt(fota_progresslabel, "%d%%", fotaupdate_progress);

            break;
        }
        case FOTA_EVENT_FINISH:
        case FOTA_EVENT_RESTART: {
            LOGD(TAG, "/***********GUI get FOTA_EVENT_FINISH or restart***********/\n");
            break;
        }
        case EVENT_GUI_USER_MUSIC_PREV: {
            lv_event_send(music_prev, LV_EVENT_CLICKED, NULL);
            if (lv_obj_has_state(play_obj, LV_STATE_CHECKED) == false) {
                LOGD(TAG, "/***********EVENT_GUI_USER_MUSIC_PREV***********/\n");
                // lv_obj_add_state(play_obj, LV_STATE_CHECKED);
                lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            }
            break;
        }
        case EVENT_GUI_USER_MUSIC_NEXT: {
            lv_event_send(music_next, LV_EVENT_CLICKED, NULL);
            if (lv_obj_has_state(play_obj, LV_STATE_CHECKED) == false) {
                // lv_obj_add_state(play_obj, LV_STATE_CHECKED);
                lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            }
            break;
        }
        case EVENT_GUI_USER_MUSIC_PLAY: {
            lv_obj_add_state(play_obj, LV_STATE_CHECKED);
            lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);
            break;
        }
        case EVENT_GUI_USER_PLAY_OVER: {
            if (lv_obj_has_state(play_obj, LV_STATE_CHECKED) == true) {
                lv_img_set_src(play_obj, &img_musicplay);
                lv_obj_clear_state(play_obj, LV_STATE_CHECKED);
                music_anim_status(false);
            }
            break;
        }
        default:
            break;
    }
}

static int check_wifi_ssid_name(void *name)
{
    uint8_t *ch = name;

    while (*ch != 0) {
        if ((*ch > 0x20) && (*ch < 0x7f)) /* 有可显示字符 */
            return 0;
        else if (*ch == 0x20) /* 空格-继续检查是否全部为空格 */
            continue;
        else
            return -1; /* 非法字符 */
        ch++;
    }

    return 1; /* 全部为空格 */
}

void app_ui_event_init()
{
    event_subscribe(EVENT_GUI_USER_MUSIC_PREV, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MUSIC_NEXT, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MUSIC_PAUSE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MUSIC_PLAY, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_FOTA_PROGRESS, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_WIFI_CONN_SUCCESS, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_WIFI_PSK_ERR, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_WIFI_NOEXIST, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_NTP_SUCCESS, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_WIFI_CONN_FAILED, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_WIFI_PROV_FAILED, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_WIFI_SCAN_INFO_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_WIFI_SCAN_FINISH, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_WIFI_PROV_START, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_SESSION_START, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_SESSION_STOP, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_NLP_NOTHING, gw_main_event_handler, NULL);
    event_subscribe(EVENT_STATUS_NLP_UNKNOWN, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_MEDIA_SYSTEM_FINISH, gw_main_event_handler, NULL);
    event_subscribe(EVENT_MEDIA_SYSTEM_START, gw_main_event_handler, NULL);
    event_subscribe(FOTA_EVENT_START, gw_main_event_handler, NULL);
    event_subscribe(FOTA_EVENT_VERSION, gw_main_event_handler, NULL);
    event_subscribe(FOTA_EVENT_PROGRESS, gw_main_event_handler, NULL);
    event_subscribe(FOTA_EVENT_FINISH, gw_main_event_handler, NULL);
    event_subscribe(FOTA_EVENT_RESTART, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_FOTA_START_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MESH_SCAN_INFO_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MESH_ADD_INFO_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_MESH_STATUS_ONOFF_UPDATE, gw_main_event_handler, NULL);
    event_subscribe(EVENT_GUI_USER_PLAY_OVER, gw_main_event_handler, NULL);
}

/* LVGL UI Demo */
static void lvgl_ui_task(void *arg)
{
    while (1) {
        aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
        lv_task_handler();
        aos_mutex_unlock(&lvgl_mutex);
        aos_msleep(CONFIG_LVGL_IDLE_TIME);
    }
}

void app_ui_init(void)
{
    aos_task_t lvgl_demo_task;
    // aos_task_t timer_task;

    aos_mutex_new(&lvgl_mutex);

    LOGD(TAG, "lvgl demo build time: %s, %s\r\n", __DATE__, __TIME__);

    /* Init lvgl*/
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    gw_main();
    aos_task_new_ext(
        &lvgl_demo_task, "lvgl-ui-task", lvgl_ui_task, NULL, CONFIG_UI_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
}