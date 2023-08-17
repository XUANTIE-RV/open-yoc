/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <ulog/ulog.h>
#include <devices/uart.h>
#include "at_ota.h"
#include "at_ota_internal.h"
#include "ymodem.h"

#define TAG "AT_OTA"

static inline int _ymodem_porting_recv(ymodem_ctx_t *ctx, void *buf, uint32_t len)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);
    rvm_dev_t *uart_dev = rvm_hal_device_find("uart", at_ota_ctx->ymodem_port);
    uint32_t recv_len = 0;
    recv_len = rvm_hal_uart_recv(uart_dev, buf, len, 100);
    return recv_len;
}

static inline int _ymodem_porting_send(ymodem_ctx_t *ctx, char ch)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);
    rvm_dev_t *uart_dev = rvm_hal_device_find("uart", at_ota_ctx->ymodem_port);
    return rvm_hal_uart_send(uart_dev, (void *)&ch, 1, AOS_WAIT_FOREVER);
}

static inline int _ymodem_porting_init(ymodem_ctx_t *ctx)
{
    LOGD(TAG, "Init\n");
    return 0;
}

static inline int _ymodem_porting_uninit(ymodem_ctx_t *ctx)
{
    LOGD(TAG, "Unit\n");
    return 0;
}

static inline int _ymodem_porting_start(ymodem_ctx_t *ctx)
{
    LOGD(TAG, "Start\n");
    return 0;
}

static inline int _ymodem_porting_finish(ymodem_ctx_t *ctx, int result)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);

    LOGD(TAG, "Finish\n");

    if (result && at_ota_ctx && at_ota_ctx->cur_file)
    {
        at_ota_ctx->cur_file->complete = 1;
    }

    return 0;
}

static inline int _ymodem_porting_write(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);

    LOGD(TAG, "Write offset %d, len %d\n", offset, len);

    if (offset + len > at_ota_ctx->cur_file->file_size) {
        LOGE(TAG, "Write Overflow offset %d, len %d\n", offset, len);
        return -1;
    }

    memcpy(at_ota_ctx->cur_file->file + offset, buf, len);

    return 0;
}

static inline int _ymodem_porting_read(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);

    LOGD(TAG, "Read offset %d, len %d\n", offset, len);

    if (offset + len > at_ota_ctx->cur_file->file_size) {
        LOGE(TAG, "Read Overflow offset %d, len %d\n", offset, len);
        return -1;
    }

    memcpy(buf, at_ota_ctx->cur_file->file + offset, len);

    return 0;
}

static inline int _ymodem_porting_erase(ymodem_ctx_t *ctx, uint32_t offset, uint32_t len)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);

    LOGD(TAG, "Erase offset %d, len %d\n", offset, len);

    if (offset + len > at_ota_ctx->cur_file->file_size) {
        LOGE(TAG, "Erase Overflow offset %d, len %d", offset, len);
        return -1;
    }

    memset(at_ota_ctx->cur_file->file + offset, 0, len);

    return 0;
}

static inline int _ymodem_porting_file_check(ymodem_ctx_t *ctx, const char *file_name, uint32_t file_size)
{
    at_ota_ctx_t *at_ota_ctx = ymodem_get_user_ctx(ctx);

    LOGD(TAG, "Check file name %s, file size %d", file_name, file_size);

    if (at_ota_ctx->cur_file && at_ota_ctx->cur_file->file_size >= file_size) {
        memset(at_ota_ctx->cur_file->file, 0, at_ota_ctx->cur_file->file_size);
    } else {
        at_ota_ctx->cur_file->file = aos_zalloc(file_size);

        if (!at_ota_ctx->cur_file->file) {
            return -1;
        }
    }

    at_ota_ctx->cur_file->file_size = file_size;
    strncpy(at_ota_ctx->cur_file->file_name, file_name, sizeof(at_ota_ctx->cur_file->file_name) - 1);

    return 0;
}

static const ymodem_ops_t _ymodem_ops = {
    .recv = _ymodem_porting_recv,
    .send = _ymodem_porting_send,

    .init       = _ymodem_porting_init,
    .uninit     = _ymodem_porting_uninit,
    .start      = _ymodem_porting_start,
    .finish     = _ymodem_porting_finish,
    .write      = _ymodem_porting_write,
    .read       = _ymodem_porting_read,
    .erase      = _ymodem_porting_erase,
    .file_check = _ymodem_porting_file_check,
};

static void _at_ota_ymodem_task(void *arg)
{
    int ret;

    at_ota_ctx_t *at_ota_ctx = arg;

    LOGD(TAG, "===> ymodem_task\r\n");

    at_ota_prepare(at_ota_ctx);

    aos_msleep(1000);

    ymodem_config_t config = {
        .ops      = &_ymodem_ops,
        .user_ctx = at_ota_ctx,
    };

    ret = ymodem_init(config);

    if (ret) {
        goto done;
    }

    ret = ymodem_upgrade();

    if (ret) {
        ymodem_uninit();
        goto done;
    }

    ret = ymodem_uninit();

    if (ret) {
        goto done;
    }

done:
    at_ota_finish(at_ota_ctx, ret);

    if (ret) {
        LOGE(TAG, "Ymode err %d\n", ret);
        at_ota_del_file(at_ota_ctx->cur_file);
        at_ota_ctx->cur_file = NULL;
    }

    at_ota_ctx->cur_file = NULL;

    LOGD(TAG, "<=== ymodem_task\r\n");

    aos_task_exit(0);
}

int at_ota_ymodel_init(at_ota_ctx_t *at_ota_ctx)
{
    return aos_task_new_ext(&at_ota_ctx->task, "at_ota_ctx_task", _at_ota_ymodem_task, at_ota_ctx,
                     CONFIG_AT_OTA_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI + 1);
}