/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __FOTA_H__
#define __FOTA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <aos/list.h>
#include <yoc/netio.h>
#define FOTA_POS    "fota_pos"
#define BUFFER_SIZE 2048
#define FW_URL_KEY  "FW_URL_KEY"

typedef enum fota_event {
    FOTA_EVENT_VERSION = 0,
    FOTA_EVENT_START = 1,
    FOTA_EVENT_FAIL = 2,
    FOTA_EVENT_FINISH = 3,
} fota_event_e;

typedef enum fota_status {
    FOTA_INIT = 1,
    FOTA_DOWNLOAD = 2,
    FOTA_STOP = 3,
    FOTA_FINISH = 4,
} fota_status_e ;

typedef struct fota_data {
    char *url;
    void *data;
}fota_data_t;

typedef struct {
    char *fota_url;
} fota_info_t;

typedef struct fota_cls {
    const char *name;
    int (*init)(void);
    int (*version_check)(fota_info_t *info);
    int (*finish)(void);
    int (*fail)(void);
} fota_cls_t;

typedef int (*fota_event_cb_t)(void *fota, fota_event_e event);   ///< fota Event call back.

typedef struct fota {
    const fota_cls_t *cls;

    netio_t *from;
    netio_t *to;
    fota_status_e status;
    char *from_path;
    char *to_path;
    uint8_t *buffer;
    int offset;
    int quit;
    aos_task_t task;
    aos_sem_t sem;
    aos_event_t do_check_event;
    fota_event_cb_t event_cb;
    int auto_check_en;
    int running;
    int timeoutms;
    int retry_count;
    int sleep_time;
    void *private;
} fota_t;

typedef struct {
    int timeoutms;
    int retry_count;
    int sleep_time;
    int auto_check_en;
} fota_config_t;

int fota_upgrade(fota_t *fota);

void fota_do_check(fota_t *fota);

int fota_start(fota_t *fota);

int fota_stop(fota_t *fota);

void fota_finish(fota_t *fota);

void fota_fail(fota_t *fota);

void fota_config(fota_t *fota, fota_config_t *config);

void fota_set_auto_check(fota_t *fota, int enable);

int fota_get_auto_check(fota_t *fota);

fota_status_e fota_get_status(fota_t *fota);

fota_t *fota_open(const char *fota_name, const char *dst, fota_event_cb_t event_cb);

int fota_close(fota_t *fota);

int fota_register_cop(void);
int fota_register_coap(void);

int fota_register(const fota_cls_t *cls);

#ifdef __cplusplus
}
#endif
#endif
