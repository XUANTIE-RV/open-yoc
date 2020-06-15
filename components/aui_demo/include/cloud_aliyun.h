/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __CLOUD_ALIYUN_H__
#define __CLOUD_ALIYUN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* aliyun proxy */
#define ALIYUN_ASR_IP "ai.c-sky.com"
#define ALIYUN_ASR_PORT 8090

/* aliyun */
#define ALIYUN_RASR_HOST    "nls-gateway.cn-shanghai.aliyuncs.com"
#define ALIYUN_RASR_URL     "wss://nls-gateway.cn-shanghai.aliyuncs.com/ws/v1"

#warning "Need to replace `aaa` with your aliyun App Key, replace `bbb` with your aliyun AccessKey ID, replace `ccc` with your aliyun AccessKey Secret!"
#define ALIYUN_RASR_APPKEY      "aaa"
#define ALIYUN_RASR_ACCESS_ID   "bbb"
#define ALIYUN_RASR_ACCESS_SEC  "ccc"

/* aliyun rasr */
const char *aliyun_rasr_get_token(void);
void aliyun_gen_uuid(char uuid[33]);

#ifdef __cplusplus
}
#endif

#endif