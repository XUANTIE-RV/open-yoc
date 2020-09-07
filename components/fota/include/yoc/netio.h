/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_NETIO_H
#define YOC_NETIO_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <aos/list.h>

#include <aos/kernel.h>
#include <aos/network.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct netio_cls netio_cls_t;

typedef struct {
    const netio_cls_t *cls;
    size_t offset;
    size_t size;
    size_t block_size;

    void *private;
} netio_t;

struct netio_cls {
    const char *name;
    int (*open)(netio_t *io, const char *path);
    int (*close)(netio_t *io);

    int (*read)(netio_t *io, uint8_t *buffer, int length, int timeoutms);
    int (*write)(netio_t *io, uint8_t *buffer, int length, int timeoutms);
    int (*remove)(netio_t *io);
    int (*seek)(netio_t *io, size_t offset, int whence);
    // int (*getinfo)(netio_t *io, fota_info_t *info);
};

/**
 * @brief  netio 注册
 * @param  [in] cls: netio 操作接口
 * @return 0 on success, -1 on failed
 */
int netio_register(const netio_cls_t *cls);

/**
 * @brief  将http操作功能注册到netio
 * @return 0 on success, -1 on failed
 */
int netio_register_http(void);

/**
 * @brief  将flash操作功能注册到netio
 * @return 0 on success, -1 on failed
 */
int netio_register_flash(void);

/**
 * @brief  将coap操作功能注册到netio
 * @return 0 on success, -1 on failed
 */
int netio_register_coap(void);

/**
 * @brief  netio 打开
 * @param  [in] path: 下载或者储存 url
 * @return netio_t句柄或者NULL
 */
netio_t *netio_open(const char *path);

/**
 * @brief  netio 关闭
 * @param  [in] io: netio句柄
 * @return 0 on success, -1 on failed
 */
int netio_close(netio_t *io);

/**
 * @brief  netio 读取
 * @param  [in] io: netio句柄
 * @param  [in] buffer: 用于存放读取数据的buffer
 * @param  [in] lenght: buffer大小
 * @param  [in] timeoutms: 超时时长
 * @return 0表示文件读完，-1超时失败，否则为读取的长度
 */
int netio_read(netio_t *io, uint8_t *buffer, size_t lenght, int timeoutms);

/**
 * @brief  netio 写
 * @param  [in] io: netio句柄
 * @param  [in] buffer: 用于存放读取数据的buffer
 * @param  [in] lenght: buffer大小
 * @param  [in] timeoutms: 超时时长
 * @return -1超时失败，否则为写入的长度
 */
int netio_write(netio_t *io, uint8_t *buffer, size_t lenght, int timeoutms);

/**
 * @brief  netio 寻址
 * @param  [in] io: netio句柄
 * @param  [in] offset：偏移
 * @param  [in] whence：偏移方向
 * @return 0 on success, -1 on failed
 */
int netio_seek(netio_t *io, size_t offset, int whence);
// int netio_getinfo(netio_t *io, fota_info_t *info);

#ifdef __cplusplus
}
#endif

#endif