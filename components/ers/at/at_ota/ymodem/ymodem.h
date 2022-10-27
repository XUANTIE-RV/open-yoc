/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _YMODEM_H_
#define _YMODEM_H_

#include <stdint.h>

/*YModem standard CMD*/
#define YMODEM_128  (0x01)
#define YMODEM_1024 (0x02)
#define YMODEM_4096 (0x07)
#define YMODEM_8192 (0x08)
#define YMODEM_EOT  (0x04)
#define YMODEM_ACK  (0x06)
#define YMODEM_NAK  (0x15)
#define YMODEM_CAN  (0x18)
#define YMODEM_C    (0x43)
/*Rcv_CMD define*/
#define IS_NOT_FIRST_PACKET (0x00)
#define IS_FIRST_PACKET     (0x01)
#define IS_NORMAL_FILE_128  (0X02)
#define IS_NORMAL_PACKET    (0X03)
#define IS_END_OF_TRANSMIT  (0X04)
#define IS_ERROR_PACKET     (0x05)
#define IS_TRANSMIT_OVER    (0x06)

typedef struct _ymodem_ctx_t ymodem_ctx_t;

typedef int (*ymodem_recv_func_t)(ymodem_ctx_t *ctx, void *buf, uint32_t len);
typedef int (*ymodem_send_func_t)(ymodem_ctx_t *ctx, char ch);

typedef int (*ymodem_init_func_t)(ymodem_ctx_t *ctx);
typedef int (*ymodem_uninit_func_t)(ymodem_ctx_t *ctx);
typedef int (*ymodem_start_func_t)(ymodem_ctx_t *ctx);
typedef int (*ymodem_finish_func_t)(ymodem_ctx_t *ctx, int result);
typedef int (*ymodem_write_func_t)(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len);
typedef int (*ymodem_read_func_t)(ymodem_ctx_t *ctx, uint32_t offset, void *buf, uint32_t len);
typedef int (*ymodem_erase_func_t)(ymodem_ctx_t *ctx, uint32_t offset, uint32_t len);
typedef int (*ymodem_file_check_func_t)(ymodem_ctx_t *ctx, const char *file_name, uint32_t file_size);
typedef struct _ymodem_ops_t {
    ymodem_recv_func_t recv;
    ymodem_send_func_t send;

    ymodem_init_func_t       init;
    ymodem_uninit_func_t     uninit;
    ymodem_start_func_t      start;
    ymodem_finish_func_t     finish;
    ymodem_write_func_t      write;
    ymodem_read_func_t       read;
    ymodem_erase_func_t      erase;
    ymodem_file_check_func_t file_check;
} ymodem_ops_t;

typedef struct _ymodem_config_t {
    const ymodem_ops_t *ops;
    void *              user_ctx;
} ymodem_config_t;

/**
 * ymodem init
 */
int ymodem_init(ymodem_config_t config);

/**
 * enter the ymodem serial port upgrade
 * @return -1: client no ack at start
 */
int ymodem_upgrade(void);

/**
 * enter the ymodem serial port upgrade
 * @return -1: client no ack at start
 */
void *ymodem_get_user_ctx(ymodem_ctx_t *ctx);

/**
 * ymodem uninit
 */
int ymodem_uninit(void);

#endif