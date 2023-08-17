/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_AUI_CLOUD_H_
#define _APP_AUI_CLOUD_H_

#ifdef __cplusplus
extern "C" {
#endif

int app_aui_cloud_init(void);
int app_aui_cloud_start(int do_wwv);
int app_aui_cloud_push_audio(void *data, size_t size);
int app_aui_push_wwv_data(void *data, size_t len);
int app_aui_cloud_stop(int force_stop);
int app_aui_cloud_start_tts();
int app_aui_cloud_tts_run(const char *text, int wait_last);
int app_aui_cloud_stop_tts();
int app_aui_cloud_push_text(char *text);

int app_aui_cloud_wwv(int confirmed);

//离线命令词和云端命令词结合使用时
//app_voice.c调用此接口解析离线命令词的json
//nlp_handle调用app_aui_shortcutCMD解析在线命令词的json
void app_aui_shortcutCMD_offline(const char *resultCMD);

#ifdef __cplusplus
}
#endif

#endif
