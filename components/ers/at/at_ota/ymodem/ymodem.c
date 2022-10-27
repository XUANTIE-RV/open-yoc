/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef CONFIG_SUPPORT_YMODEM

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "ymodem.h"

#define TAG "YModem"

static unsigned short int ymodem_crc(unsigned char *data, unsigned short int len);

typedef struct _ymodem_ctx_t {
    uint8_t             init : 1;
    const ymodem_ops_t *ops;
    int                 program_count;
    int                 file_size;
    int                 recv_size_now;
    char                file_name[64];
    int                 crc_error;
    int                 packet_index_error;
    int                 ymodem_start;
    unsigned char       receive_buff[8192 + 5];
    int                 rcv_packet_index;
    int                 rcv_packet_index_last;
    unsigned char       IsFileEnd;
    int                 EOT_Flag;
    unsigned char       result;
    int                 first_package;
    void *              user_ctx;
} ymodem_ctx_t;

static ymodem_ctx_t ymodem_ctx = { 0 };

static inline int _ymodem_recv(ymodem_ctx_t *ctx, void *buf, uint32_t len)
{
    if (ctx->ops->recv) {
        return ctx->ops->recv(ctx, buf, len);
    }

    return -1;
}

static inline int _ymodem_send(ymodem_ctx_t *ctx, char ch)
{
    if (ctx->ops->send) {
        return ctx->ops->send(ctx, ch);
    }

    return -1;
}

static inline int _ymodem_init(ymodem_ctx_t *ctx)
{
    if (ctx->ops->init) {
        return ctx->ops->init(ctx);
    }

    return -1;
}

static inline int _ymodem_uninit(ymodem_ctx_t *ctx)
{
    if (ctx->ops->uninit) {
        return ctx->ops->uninit(ctx);
    }

	return -1;
}

static inline int _ymodem_start(ymodem_ctx_t *ctx)
{
    if (ctx->ops->start) {
        return ctx->ops->start(ctx);
    }

    return -1;
}

static inline int _ymodem_finish(ymodem_ctx_t *ctx, int result)
{
    if (ctx->ops->finish) {
        return ctx->ops->finish(ctx, result);
    }

	return -1;
}

static inline int _ymodem_write(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len)
{
    if (ctx->ops->write) {
        return ctx->ops->write(ctx, offset, buf, len);
    }

    return -1;
}

static inline int _ymodem_read(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len)
{
    if (ctx->ops->read) {
        return ctx->ops->read(ctx, offset, buf, len);
    }

    return -1;
}

static inline int _ymodem_erase(ymodem_ctx_t *ctx, uint32_t offset, uint32_t len)
{
    if (ctx->ops->erase) {
        return ctx->ops->erase(ctx, offset, len);
    }

    return -1;
}

static inline int _ymodem_file_check(ymodem_ctx_t *ctx, char *file_name, uint32_t file_size)
{
    if (ctx->ops->file_check) {
        return ctx->ops->file_check(ctx, file_name, file_size);
    }

    return -1;
}

#define YmodemSendACK(ch) _ymodem_send(ctx, ch)

//保存并应答
static void ymodem_save_and_ack(ymodem_ctx_t *ctx, unsigned char PacketReturn, unsigned char *IsFileEnd)
{
    switch (PacketReturn) {
        case IS_FIRST_PACKET: {
            _ymodem_erase(ctx, 0, ctx->file_size);
            YmodemSendACK(YMODEM_ACK);
            YmodemSendACK(YMODEM_C);
            break;
        }

        case IS_TRANSMIT_OVER: {
            YmodemSendACK(YMODEM_CAN);
            break;
        }

        case IS_NORMAL_FILE_128:
        case IS_NORMAL_PACKET: {
            //写flash
//#if defined(UART_PORTING_SYNC) &&  UART_PORTING_SYNC
            //YmodemSendACK(YMODEM_ACK);
//#endif

            if ((ctx->file_size - ctx->program_count) >= ctx->recv_size_now) {
                _ymodem_write(ctx, ctx->program_count, ctx->receive_buff + 3, ctx->recv_size_now);
                ctx->program_count += ctx->recv_size_now;
            } else {
                _ymodem_write(ctx, ctx->program_count, ctx->receive_buff + 3, (ctx->file_size - ctx->program_count));
                ctx->program_count += (ctx->file_size - ctx->program_count);
            }

//#if defined(UART_PORTING_SYNC) &&  UART_PORTING_SYNC
            YmodemSendACK(YMODEM_ACK);
//#endif
            break;
        }

        case IS_END_OF_TRANSMIT: {
            if (ctx->EOT_Flag == 1) {
                YmodemSendACK(YMODEM_NAK);
            }

            if (ctx->EOT_Flag == 2) { // 第二次收到EOT发ACK 和大写 C
                YmodemSendACK(YMODEM_ACK);
                YmodemSendACK(YMODEM_C);
            }

            if (ctx->EOT_Flag == 3) { //传输结束
                YmodemSendACK(YMODEM_ACK);
                ctx->EOT_Flag = 0;
                *IsFileEnd    = 1;
            }

            break;
        }

        case IS_ERROR_PACKET: {
            YmodemSendACK(YMODEM_NAK);
            break;
        }
    }
}
static int get_file_size(unsigned char *data)
{
    int  first = 0, second = 0;
    char first_find  = 0;
    char second_find = ' ';

    for (int i = 0; i < 50; i++) {
        if (data[i] == first_find) {
            first = i;
        }

        if (data[i] == second_find) {
            second = i;
            break;
        }
    }

    int tmp    = 0;
    int tmp_10 = 1;

    for (int i = (second - 1); i > first; i--) {
        tmp = tmp + ((data[i] - '0') * tmp_10);
        tmp_10 *= 10;
    }

    return tmp;
}

static int file_is_valid(ymodem_ctx_t *ctx, unsigned char *data)
{
    int ret;
    ctx->program_count = 0;
    ctx->file_size     = get_file_size(data);
    strncpy(ctx->file_name, (char *)(data), sizeof(ctx->file_name) - 1);

    ret = _ymodem_file_check(ctx, ctx->file_name, ctx->file_size);

    return ret == 0 ? 1 : 0;
}

static int ymodem_data_check(ymodem_ctx_t *ctx, unsigned char *rcvbuf, unsigned char *result)
{
    unsigned short int crc;
    unsigned char      debug;
    unsigned char      debug1;

    unsigned short int crc_result;

    switch (*(rcvbuf)) {
        case YMODEM_EOT: {
            ctx->EOT_Flag++;
            *result = IS_END_OF_TRANSMIT;
            break;
        }

        case YMODEM_128: {
            debug  = ((~(*(rcvbuf + 1))));
            debug1 = ((*(rcvbuf + 2)));

            if (debug != debug1) {
                *result = IS_ERROR_PACKET;
                return -1;
            }

            crc        = *(rcvbuf + 128 + 3) << 8 | *(rcvbuf + 128 + 3 + 1);
            crc_result = ymodem_crc(rcvbuf + 3, 128);

            if (crc_result != crc) {
                *result = IS_ERROR_PACKET;
                ctx->crc_error++;
                return -1;
            }

            ctx->rcv_packet_index = *(rcvbuf + 1);

            if (ctx->rcv_packet_index == 0) {
                if (*result == IS_END_OF_TRANSMIT) {
                    *result = IS_END_OF_TRANSMIT;
                    ctx->EOT_Flag++;
                    return 0;
                }

                if (ctx->first_package) {
                    if (file_is_valid(ctx, rcvbuf + 3)) {
                        ctx->first_package = 0;
                        *result            = IS_FIRST_PACKET;
                    } else {
                        *result = IS_TRANSMIT_OVER;
                        return -2;
                    }
                } else {
                    if ((ctx->rcv_packet_index_last) != 255) {
                        ctx->packet_index_error++;
                        *result = IS_ERROR_PACKET;
                    } else {
                        ctx->rcv_packet_index_last = ctx->rcv_packet_index;
                        *result                    = IS_NORMAL_FILE_128;
                    }
                }
            } else {
                if (ctx->rcv_packet_index != (ctx->rcv_packet_index_last + 1)) {
                    ctx->packet_index_error++;
                    *result = IS_ERROR_PACKET;
                } else {
                    ctx->rcv_packet_index_last = ctx->rcv_packet_index;
                    *result                    = IS_NORMAL_FILE_128;
                }
            }

            break;
        }

        case YMODEM_1024:
        case YMODEM_4096:
        case YMODEM_8192: {
            debug  = ((~(*(rcvbuf + 1))));
            debug1 = ((*(rcvbuf + 2)));

            if (debug != debug1) {
                *result = IS_ERROR_PACKET;
                return -1;
            }

            crc        = *(rcvbuf + ctx->recv_size_now + 3) << 8 | *(rcvbuf + ctx->recv_size_now + 3 + 1);
            crc_result = ymodem_crc(rcvbuf + 3, ctx->recv_size_now);

            if (crc_result != crc) {
                *result = IS_ERROR_PACKET;
                ctx->crc_error++;
                return -1;
            }

            ctx->rcv_packet_index = *(rcvbuf + 1);

            if (ctx->rcv_packet_index == 0) {
                if (ctx->first_package) {
                    if (file_is_valid(ctx, rcvbuf + 3)) {
                        ctx->first_package = 0;
                        *result            = IS_FIRST_PACKET;
                    } else {
                        *result = IS_TRANSMIT_OVER;
                        return -2;
                    }
                } else {
                    if ((ctx->rcv_packet_index_last) != 255) {
                        ctx->packet_index_error++;
                        *result = IS_ERROR_PACKET;
                    } else {
                        ctx->rcv_packet_index_last = ctx->rcv_packet_index;
                        *result                    = IS_NORMAL_PACKET;

                        if (ctx->EOT_Flag > 0) {
                            ctx->EOT_Flag = 0;
                            LOGW(TAG, "may recive EIO_FLAG in Transmit\r\n");
                        }
                    }
                }
            } else {
                if (ctx->rcv_packet_index != (ctx->rcv_packet_index_last + 1)) {
                    ctx->packet_index_error++;
                    *result = IS_ERROR_PACKET;
                } else {
                    ctx->rcv_packet_index_last = ctx->rcv_packet_index;
                    *result                    = IS_NORMAL_PACKET;
                }
            }

            break;
        }

        default:
            break;
    }

    return 0;
}
// CRC16_XMODEM  x16+x12+x5+1 初始值 0x0000
static unsigned short int ymodem_crc(unsigned char *data, unsigned short int len)
{
    int crc = 0;
    int i, j;

    for (i = 0; i < len; i++) {
        crc = crc ^ (data[i] << 8);

        for (j = 0; j < 8; j++) {
            if ((crc & ((int)0x8000)) != 0) {
                crc = ((crc << 1) ^ 0x1021);
            } else {
                crc = crc << 1;
            }
        }
    }

    return (crc & 0xFFFF);
}

int ymodem_init(ymodem_config_t config)
{
    int ret;

    if (ymodem_ctx.init) {
        LOGE(TAG, "Ymodel is init");
        return -1;
    }

    memset(&ymodem_ctx, 0, sizeof(ymodem_ctx));

    ymodem_ctx.first_package = 1;
    ymodem_ctx.ops           = config.ops;
    ymodem_ctx.init          = 1;
    ymodem_ctx.user_ctx      = config.user_ctx;

    ret = _ymodem_init(&ymodem_ctx);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

int ymodem_uninit(void)
{
    int ret;

    if (!ymodem_ctx.init) {
        return 0;
    }

    ret = _ymodem_uninit(&ymodem_ctx);

    if (ret < 0) {
        return -1;
    }

    memset(&ymodem_ctx, 0, sizeof(ymodem_ctx));

    return 0;
}

void *ymodem_get_user_ctx(ymodem_ctx_t *ctx)
{
    if (ctx) {
        return ctx->user_ctx;
    }

    return NULL;
}

static int get_rcv_size(unsigned char packet_head)
{
    int rcv_size;

    switch (packet_head) {
        case YMODEM_128:
            rcv_size = 128;
            break;

        case YMODEM_1024:
            rcv_size = 1024;
            break;

        case YMODEM_4096:
            rcv_size = 4096;
            break;

        case YMODEM_8192:
            rcv_size = 8192;
            break;

        case YMODEM_EOT:
            rcv_size = -4;
            break;

        default:
            rcv_size = 0;
            break;
    }

    return rcv_size;
}

int ymodem_upgrade(void)
{
    ymodem_ctx_t *ctx = &ymodem_ctx;

    static int rcv_count = 0;
    int        ret       = -1;
    uint32_t   t, t1 = 0;

    LOGD(TAG, "Upgrade");

    _ymodem_start(ctx);

    t = aos_now_ms();

    while (1) {
        if (ctx->ymodem_start == 0) {
            aos_msleep(100);
            YmodemSendACK(YMODEM_C);
        }

        ret = _ymodem_recv(ctx, ctx->receive_buff, 1);

        if ((ret > 0)
            && ((ctx->receive_buff[0] == YMODEM_128) || (ctx->receive_buff[0] == YMODEM_1024)
                || (ctx->receive_buff[0] == YMODEM_4096) || (ctx->receive_buff[0] == YMODEM_8192)
                || (ctx->receive_buff[0] == YMODEM_EOT)))
        {
            ctx->ymodem_start = 1;
            rcv_count += ret;
            break;
        }

        if (aos_now_ms() - t > 60000) {
            LOGE(TAG, "upgrade start timeout\n");
            goto fail;
        }
    }

    t = aos_now_ms();

    while (1) {
        ret = _ymodem_recv(ctx, ctx->receive_buff + rcv_count, sizeof(ctx->receive_buff) - rcv_count);
        rcv_count += ret;

        ctx->recv_size_now = get_rcv_size(ctx->receive_buff[0]);

        if (ctx->recv_size_now == 0) {
            rcv_count = 0;
        }

        if (rcv_count > 0) {
            t1 = aos_now_ms();

            while (1) {
                ret = _ymodem_recv(ctx, ctx->receive_buff + rcv_count, sizeof(ctx->receive_buff) - rcv_count);
                rcv_count += ret;

                if (rcv_count >= (ctx->recv_size_now + 5)) {
                    if (ymodem_data_check(ctx, ctx->receive_buff, &ctx->result) == -2) {
                        ymodem_save_and_ack(ctx, ctx->result, &ctx->IsFileEnd);
                        _ymodem_finish(ctx, ctx->IsFileEnd);
                        return 0;
                    }

                    ymodem_save_and_ack(ctx, ctx->result, &ctx->IsFileEnd);
                    rcv_count = 0;
                    break;
                }

                if (aos_now_ms() - t1 > 1000) {
                    /* clear all recevied data */
                    while (ret > 0) {
                        ret = _ymodem_recv(ctx, ctx->receive_buff, sizeof(ctx->receive_buff));
                    }

                    rcv_count = 0;
                    ymodem_save_and_ack(ctx, IS_ERROR_PACKET, &ctx->IsFileEnd);
                    t1 = aos_now_ms();
                    LOGW(TAG, "transmit timeout, retry\n");
                }

                if (aos_now_ms() - t > 30 * 1000) {
                    LOGE(TAG, "upgrade fail,  timeout\n");
                    goto fail;
                }
            }
        }

        if (aos_now_ms() - t > 5 * 60 * 1000) {
            LOGE(TAG, "upgrade fail,  timeout\n");
            goto fail;
        }

        if (ctx->IsFileEnd) {
            _ymodem_finish(ctx, 1);
#if 0

            for (int i = 0; i < 10; i++) {
                printf("********************************************\n");
                printf("...0x%x\n", program_start_addr);
                printf("%s\n", file_name);
                printf("error: %d %d\n", packet_index_error, crc_error);
                printf("%d %d\n", file_size, program_count);
            }

            for (int i = 0; i < 64; i++) {
                printf("%#x ", *(uint8_t *)(program_start_addr + i));
            }

#endif
            return 0;
        }
    }

fail:
    _ymodem_finish(ctx, 0);
    return -1;
}
#endif
