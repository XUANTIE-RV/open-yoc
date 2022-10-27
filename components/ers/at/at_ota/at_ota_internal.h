/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _AT_OTA_INTERNAL_H_
#define _AT_OTA_INTERNAL_H_

#ifndef CONFIG_AT_OTA_TASK_STACK_SIZE
#define CONFIG_AT_OTA_TASK_STACK_SIZE (4 * 1024)
#endif

#ifndef CONFIG_AT_OTA_MAX_FILE_NUM
#define CONFIG_AT_OTA_MAX_FILE_NUM (10)
#endif

#ifndef CONFIG_AT_OTA_FILE_NAME_LEN
#define CONFIG_AT_OTA_FILE_NAME_LEN (64)
#endif

typedef struct _at_ota_file_t {
    int      used     : 1;
    int      complete : 1;
    int      reserve  : 14;
    int      ota_idx;
    int      ota_target;
    int      ota_channel;
    int      ota_protocol;
    char     file_name[CONFIG_AT_OTA_FILE_NAME_LEN];
    void *   file;
    uint32_t file_size;
} at_ota_file_t;
typedef struct _at_ota_ctx_t {
    uint8_t        init : 1;
    at_ota_ops_t * ops;
    int            ymodem_port;
    at_ota_file_t *cur_file;
    at_ota_file_t  file[CONFIG_AT_OTA_MAX_FILE_NUM];
    aos_task_t     task;
} at_ota_ctx_t;

int at_ota_prepare(at_ota_ctx_t *ctx);
int at_ota_finish(at_ota_ctx_t *ctx, int result);

at_ota_file_t *at_ota_new_file(int ota_target, int ota_protocol, int ota_channel);
void           at_ota_del_file(at_ota_file_t *file);
at_ota_file_t *at_ota_file_find_by_idx(int ota_idx);
at_ota_file_t *at_ota_file_find_by_file_name(const char *file_name);

int at_ota_ymodel_init(at_ota_ctx_t *ctx);

int at_ota_start_upgrade(at_ota_file_t *file);

#endif
