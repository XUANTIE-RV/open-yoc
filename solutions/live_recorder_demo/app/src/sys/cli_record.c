/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <string.h>
#include <errno.h>
#include <cx/record.h>
#include "link_visual_enum.h"
#include "linkvisual_client.h"
#include <cx/cloud_lv_service.h>
#include <time.h>
#include "media_config.h"
#include "app_cx_record.h"
#include <cx/init.h>
#include "pthread.h"

#define TAG "record"

//static
extern pthread_mutex_t sensor_toggle_lock;
cx_dvr_hdl_t record_handle = NULL;
cxlv_hdl_t g_cloud_lv_handle = NULL;

#define SNAPSHOT_PIC_MAX_SIZE (500*1024)
void dvr_record_callback(cx_dvr_event_t event, void *data)
{
    cx_dvr_file_info_t *info = (cx_dvr_file_info_t *)data;
    printf("record callback:%d %s %d\n", event, info->file_path, info->type);
    if(event == CXDVR_EVENT_SNAP_GENERATED) {
        int ret;
        FILE *fp = fopen(info->file_path, "r");
        if (!fp) {
            printf("Open file failed: %s\n", info->file_path);
            return;
        }
        fseek(fp, 0, SEEK_END);
        unsigned int file_size = ftell(fp);
        if(file_size > SNAPSHOT_PIC_MAX_SIZE) {
            printf("snap_shot pic size too large:%d\n", file_size);
            return;
        }
        char *snap_shot_buff = (char *)malloc(SNAPSHOT_PIC_MAX_SIZE);
        fseek(fp, 0, SEEK_SET);
        ret = fread(snap_shot_buff, 1, file_size, fp);
        if(ret != file_size) {
            printf("File read error: %s\n", info->file_path);
            fclose(fp);
            free(snap_shot_buff);
            return;
        }
        fclose(fp);
        cx_lv_alarm_param_t param;
        param.event_type = CX_LV_MOTION_DETECTED_EVENT;
        param.media_format = CX_MEIDA_JPEG;
        param.media.p = snap_shot_buff;
        param.media.len = file_size;
        cx_lv_post_alarm_event(g_cloud_lv_handle, &param);
        free(snap_shot_buff);
    }
}

static void Usage(char *programName)
{
    printf("Usage: %s\n", programName);
    printf("record video start <time, ms>\n");
    printf("record video stop\n");
    printf("record video search <start_time> <stop_time>\n");
    printf("record snap shot <quality, 1~99> [num] [interval, ms]\n");
    printf("record snap search <start_time> <stop_time>\n");
}

void cmd_record_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2) {
        goto err_param;
    }

    if(strcmp(argv[1], "snap") == 0) {
        if(argc < 3) {
            goto err_param;
        }
        if(strcmp(argv[2], "shot") == 0) {
            if(argc < 4) {
                goto err_param;
            }
            SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
            MediaVideoRGBInit();
#endif
            if(argv[4] && argv[5]) {
                cx_dvr_snapshort_config_t config;
                config.type = CXDVR_SNAP_BURST;
                config.quality = atoi(argv[3]);
                config.busrt_num = atoi(argv[4]);
                config.burst_interval = atoi(argv[5]);
                cx_dvr_trigger_snapshot(record_handle, &config);
            } else {
                cx_dvr_snapshort_config_t config;
                config.type = CXDVR_SNAP_SINGLE;
                config.quality = atoi(argv[3]);
                cx_dvr_trigger_snapshot(record_handle, &config);
            }
            SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
        } else if(strcmp(argv[2], "search") == 0) {
            if(argc < 5) {
                goto err_param;
            }
            uint32_t start_time = atoi(argv[3]);
            uint32_t stop_time = atoi(argv[4]);
            cx_dvr_file_info_t *infos = (cx_dvr_file_info_t *)malloc(sizeof(cx_dvr_file_info_t) * 100);
            int ret = cx_dvr_search(record_handle, start_time, stop_time, CXDVR_FILE_TYPE_SNAPSHORT, infos, 100);
            for(int i=0; i<ret; i++) {
                printf("snapshot:%d,%s size:%d type:%d time:%d-%d\n", i, infos[i].file_path, infos[i].size,\
                        infos[i].type, infos[i].begin_time, infos[i].end_time);
            }
            free(infos);
        }
    } else if(strcmp(argv[1], "video") == 0) {
        if(argc < 3) {
            goto err_param;
        }
        if(strcmp(argv[2], "start") == 0) {
            if(argc < 4) {
                goto err_param;
            }
            SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
            MediaVideoRGBInit();
#endif
            cx_dvr_record_start(record_handle, atoi(argv[3]));
            SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
        } else if(strcmp(argv[2], "stop") == 0) {
            cx_dvr_record_stop(record_handle);
        }
        else if(strcmp(argv[2], "search") == 0) {
            if(argc < 5) {
                goto err_param;
            }
            uint32_t start_time = atoi(argv[3]);
            uint32_t stop_time = atoi(argv[4]);
            cx_dvr_file_info_t *infos = (cx_dvr_file_info_t *)malloc(sizeof(cx_dvr_file_info_t) * 100);
            int ret = cx_dvr_search(record_handle, start_time, stop_time, CXDVR_FILE_TYPE_VIDEO, infos, 100);
            for(int i=0; i<ret; i++) {
                printf("record:%d,%s size:%d type:%d time:%d-%d\n", i, infos[i].file_path, infos[i].size,\
                        infos[i].type, infos[i].begin_time, infos[i].end_time);
            }
            free(infos);
        }
    } else if(argc == 2) {
        if(strcmp(argv[1], "get_time") == 0) {
            time_t utime;
            struct tm *tm = NULL;
            long long start_time, stop_time;

            time(&utime);
            tm = localtime(&utime);
            start_time = utime - (tm->tm_hour*3600+tm->tm_min*60+tm->tm_sec);
            stop_time = start_time + 24*60*60 - 1; 
            printf("current day start time:%lld, stop time:%lld\n", start_time, stop_time);
        } else if(strcmp(argv[1], "start") == 0) {
            record_handle = cx_dvr_start("recorder", dvr_record_callback);
        } else if(strcmp(argv[1], "stop") == 0) {
            cx_dvr_stop(record_handle);
        }
    }
    return;

err_param:
    printf("%s %d err param\n", __func__, __LINE__);
    Usage(argv[0]);
    return;
}

static void cmd_vidinfo(char *wbuf, int wbuf_len, int argc, char **argv)
{
    extern void VidChannelInfoDump(void);
    VidChannelInfoDump();
}
extern int test_ai_vid_source(int shot_count);
static void cmd_sensor_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(strcmp(argv[1], "ir") == 0) {
        if(strcmp(argv[2], "test") == 0) {
            test_ai_vid_source(2);
        } else if(atoi(argv[2]) == 0) {
            SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
            MediaVideoIRDeInit();
#endif
            SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
        } else {
            SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
            MediaVideoIRInit();
#endif
            SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
        }
    } else if(strcmp(argv[1], "rgb") == 0) {
        if(argc == 3) {
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
            SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
            if(atoi(argv[2])  == 0) {
                MediaVideoRGBDeInit();
            } else {
                MediaVideoRGBInit();
            }
            SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
#endif
        } else if(argc == 4) {
            if(strcmp(argv[2], "mono") == 0) {
                if(atoi(argv[3]) == 1) {
                    const char *hardware_config = R"({	
                    "sensors": [	
                        {	
                        "tag": "rgb0",	
                        "monoMode": 1
                        }
                    ]	
                    })";
                    cx_hardware_config(hardware_config);
                } else {
                    const char *hardware_config = R"({	
                    "sensors": [	
                        {	
                        "tag": "rgb0",	
                        "monoMode": 0
                        }
                    ]	
                    })";
                    cx_hardware_config(hardware_config);
                }

            }
        }

    }
}


void cli_reg_cmd_lv(void)
{
    static const struct cli_command cli_info_record_test = {"record", "record test", cmd_record_test};
    aos_cli_register_command(&cli_info_record_test);

    static const struct cli_command cli_info_vid_test = {"vid_info", "vid test", cmd_vidinfo};
    aos_cli_register_command(&cli_info_vid_test);

    static const struct cli_command cli_sensor_test = {"sensor", "sensor test", cmd_sensor_test};
    aos_cli_register_command(&cli_sensor_test);
}
/*
record video start
record video stop
reocrd video search xx xx

record snap shot quality count interval 
record snap search

record get_time

*/

int app_record_init(void)
{
    if (record_handle != NULL) {
        LOGW(TAG, "dvr already start");
        return -EBUSY;
    }
    SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoRGBInit();
#endif
    record_handle = cx_dvr_start("recorder", dvr_record_callback);
    SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    if (record_handle == NULL) {
        LOGE(TAG, "dvr start failed");
        return -EPERM;
    }
    return 0;
}

void cloud_lv_callback_func(cx_lv_event_e event, void *data)
{
    printf("lv event:%d\n", event);
    if(event == CXLV_EVENT_LIVE_START) {
        SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
        MediaVideoRGBInit();
#endif
        SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    } else if(event == CXLV_EVENT_LIVE_STOP) {

    }
}

int app_linkvisual_init(void)
{
    if (g_cloud_lv_handle != NULL) {
        LOGE(TAG, "linkvisual init already");
        return -EBUSY;
    }

    g_cloud_lv_handle = cx_lv_start("linkvisual", cloud_lv_callback_func);
    if (g_cloud_lv_handle == NULL) {
        return -EPERM;
    }
    cx_lv_connect(g_cloud_lv_handle);
    return 0;
}
