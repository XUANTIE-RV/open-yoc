/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _CLOUD_MIT_H_
#define _CLOUD_MIT_H_

#include <stdint.h>
#include <yoc/aui_cloud.h>

typedef struct {
    char *device_uuid;
    char *asr_app_key;
    char *asr_token;
    char *asr_url;
    char *tts_app_key;
    char *tts_token;
    char *tts_url;
    char *tts_key_id;
    char *tts_key_secret;
}  mit_account_info_t;

int mit_asr_init(aui_t *aui);
int mit_tts_init(aui_t *aui);
uint8_t* mit_get_mac_addr(void);

extern mit_account_info_t g_mit_account_info;

#endif
