/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CLOUD_LV_SERVICE_INTERNAL_H
#define CLOUD_LV_SERVICE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LV_INTERCOM_DIR_TO_CLOUD = 0,
    LV_INTERCOM_DIR_FROM_CLOUD,
} lv_intercom_dir;

/* linkvisual internal interface*/
void linkvisual_set_live_clarity(int clarity);
int  linkvisual_get_intercom_dir(void);
void linkvisual_audio_play(const lv_on_push_streaming_data_param_s *param);
int  linkvisual_start_video_encode(int start);
int  linkvisual_start_audio_encode(int start);
int  linkvisual_get_encoded_video(void *data, uint32_t max_size, int *is_iframe, long long *timestamp);
int  linkvisual_get_encoded_audio(void *data, uint32_t max_size, long long *timestamp);
int  linkvisual_get_video_attributes(lv_video_param_s *param);
int  linkvisual_get_audio_attributes(lv_audio_param_s *param);

#ifdef __cplusplus
}
#endif

#endif