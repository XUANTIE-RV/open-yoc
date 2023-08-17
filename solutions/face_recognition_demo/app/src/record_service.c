/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <cx/record.h>
#include "link_visual_enum.h"
#include "linkvisual_client.h"
#include <cx/cloud_lv_service.h>

extern cxlv_hdl_t cloud_lv_handle;

#define SNAPSHOT_PIC_MAX_SIZE (500*1024)
void dvr_record_callback(cx_dvr_event_t event, void *data)
{
    cx_dvr_file_info_t *info = (cx_dvr_file_info_t *)data;
    printf("record callback:%d %s %d\n", event, info->file_path, info->type);
    if(event == CXDVR_EVENT_SNAP_GENERATED) {
        int ret;
        // char name[128];
        // ret = snprintf(name, sizeof(name), "/mnt/sd/record/recorder0/picture/%s/%s", info->folder_name, info->file_name);     //fix me
        // if(ret < 0) {
        //     printf("%s %d snprintf fail\n", __func__, __LINE__);
        // }
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
        cx_lv_post_alarm_event(cloud_lv_handle, &param);
        free(snap_shot_buff);
    }
}