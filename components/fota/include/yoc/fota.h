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

/**
 * @brief  开始下载镜像
 * @param  [in] fota: fota 句柄
 * @return 0 on success, -1 on failed
 */
int fota_upgrade(fota_t *fota);

/**
 * @brief  强制检测版本
 * @param  [in] fota: fota 句柄
 */
void fota_do_check(fota_t *fota);

/**
 * @brief  创建FOTA服务
 * @param  [in] fota: fota 句柄
 * @return 0 on success, -1 on failed
 */
int fota_start(fota_t *fota);

/**
 * @brief  停止FOTA功能，退出FOTA服务
 * @param  [in] fota: fota 句柄
 * @return 0 on success, -1 on failed
 */
int fota_stop(fota_t *fota);

/**
 * @brief  FOTA完成，调用用户实现的finish接口
 * @param  [in] fota: fota 句柄
 */
void fota_finish(fota_t *fota);

/**
 * @brief  FOTA失败，调用用户实现的fail接口，并释放FOTA资源,但不释放FOTA句柄
 * @param  [in] fota: fota 句柄
 */
void fota_fail(fota_t *fota);

/**
 * @brief  配置FOTA参数
 * @param  [in] fota: fota 句柄
 * @param  [in] config: 配置数据指针，具体见 `fota_config_t`
 */
void fota_config(fota_t *fota, fota_config_t *config);

/**
 * @brief  设置是否自动不断检测服务器固件版本并升级的功能
 * @param  [in] fota: fota 句柄
 * @param  [in] enable: 0表示不自动检测，1表示自动检测
 */
void fota_set_auto_check(fota_t *fota, int enable);

/**
 * @brief  获取是否自动检测判断
 * @param  [in] fota: fota 句柄
 * @return 0表示不自动检测，1表示自动检测
 */
int fota_get_auto_check(fota_t *fota);

/**
 * @brief  获取升级状态
 * @param  [in] fota: fota 句柄
 * @return fota_status_e
 */
fota_status_e fota_get_status(fota_t *fota);

/**
 * @brief  fota初始化
 * @param  [in] fota_name: FOTA平台名字，比如"cop",
 * @param  [in] dst: 差分包存储url
 * @param  [in] event_cb: 用户事件回调
 * @return fota句柄或者NULL
 */
fota_t *fota_open(const char *fota_name, const char *dst, fota_event_cb_t event_cb);

/**
 * @brief  关闭FOTA功能，释放所有资源
 * @param  [in] fota: fota 句柄
 * @return 0 on success, -1 on failed
 */
int fota_close(fota_t *fota);

/**
 * @brief  注册为cop平台，即从cop平台下载固件
 * @return 0 on success, -1 on failed
 */
int fota_register_cop(void);

/**
 * @brief  注册为coap平台，即从coap平台下载固件
 * @return 0 on success, -1 on failed
 */
int fota_register_coap(void);

/**
 * @brief  注册平台接口
 * @param  [in] cls: 不同平台实现的接口集合，具体实现接口见`fota_cls_t`
 * @return 0 on success, -1 on failed
 */
int fota_register(const fota_cls_t *cls);

#ifdef __cplusplus
}
#endif
#endif
