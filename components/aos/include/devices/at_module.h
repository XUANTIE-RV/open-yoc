/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_AT_PARSER_H
#define DEVICE_AT_PARSER_H

#include <aos/kernel.h>
#include <aos/list.h>
#include <aos/ringbuffer.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "drv/usart.h"


#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define FRONT 1
#define BACK  0
#define NO_WAIT 0
#define WAIT_FOREVER  -1
#define LINE_LEN 128

aos_queue_t g_netm_queue;
aos_queue_t g_netm2app_queue;
aos_mutex_t g_cmd_mutex;

typedef enum {
    AT_CMD_NOECHO = 1,
    AT_CMD_RST,
    AT_CMD_CPIN,
    AT_CMD_CSQ,
    AT_CMD_CREG,
    AT_CMD_CGATT,
    AT_CMD_CSTT,
    AT_CMD_CIICR,
    AT_CMD_GETIP,
    AT_CMD_CONN,
    AT_CMD_OPEN,
    AT_CMD_DISC,
    AT_CMD_SEND,
    AT_CMD_MUX_SEND,
    AT_CMD_CIPHEAD,
    AT_CMD_STAT,
    AT_CMD_MODE,
    AT_CMD_RESTORE,
    AT_CMD_PING,
    AT_CMD_INFO_LINK,
    AT_CMD_INFO_MAC,
    AT_CMD_INFO,
    AT_CMD_DOMAIN,
    AT_CMD_SHUT,
    AT_CMD_PDPC,
    AT_CMD_GPRS_MODE,
    AT_CMD_SET_AP,
    AT_CMD_AP_DISC,
    AT_CMD_MUX_MODE,
    AT_CMD_SMART_CFG,
    AT_CMD_UART_CONFIG,
    AT_CMD_NET_ACK,
    AT_CMD_RAW,
} at_cmd_t;

typedef struct netm_dev_s {
    usart_handle_t usart_handle;

    aos_sem_t              ksem_write;
    aos_sem_t              ksem_read;

    /* I/O buffers */
    dev_ringbuf_t *rb;

    usart_event_e          usart_txevent;
    usart_event_e          usart_rxevent;
    uint8_t                stat_txmit;
    uint8_t                stat_rxmit;
    uint8_t                block;       /*1: block 0: nonblock */
    uint8_t                flowctrl;
    uint32_t               read_num;
} netm_dev_t;

typedef enum {
    NETM_STATUS_UNKNOW = 0,
    NETM_STATUS_READY,
    NETM_STATUS_LINKDOWN,
    NETM_STATUS_CONFIGING,
    NETM_STATUS_LINKUP,
    NETM_STATUS_GOTIP,
    NETM_STATUS_SOCKET_DISCONNECT,
    NETM_STATUS_SOCKET_CONNECTED,
} at_status_t;

typedef struct {
    int msg_src;
    int cmd;
    char param[64];
    int param_len;
    int result;
    uint32_t timeout;
    uint8_t isAsync;
} at_msg_t;

typedef enum {
    SRC_APP = 1,
    SRC_DRIVER,
} msg_src_t;

typedef struct {
    int id;
    void *data;
    uint32_t len;
} mux_send_param_t;

/*
* --> | slist | --> | slist | --> NULL
*     ---------     ---------
*     | smhr  |     | smpr  |
*     ---------     ---------
*     | rsp   |     | rsp   |
*     ---------     ---------
*/
typedef struct at_task_s {
    slist_t next;
    aos_sem_t smpr;
    char *command;
    char *rsp;
    char *rsp_prefix;
    char *rsp_success_postfix;
    char *rsp_fail_postfix;
    uint32_t rsp_prefix_len;
    uint32_t rsp_success_postfix_len;
    uint32_t rsp_fail_postfix_len;
    uint32_t rsp_offset;
    uint32_t rsp_len;
} at_task_t;

typedef enum {
    NORMAL = 0,
    ASYN
} at_mode_t;

typedef enum {
    AT_SEND_RAW = 0,
    AT_SEND_PBUF
} at_send_t;


typedef struct {
    int type;
    char *cmd_name;
    char *resp_name;
} at_resp_cmd_t;

typedef struct {
    int      port;
    int      flow_control;
    uint32_t baud_rate;
    uint32_t rbsize;
} at_uart_param_t;

typedef void (*rdata_cb)(int fd, void *data, size_t len);
typedef void (*linkstat_cb)(int linkid, int status);

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

/**
* Parser structure for parsing AT commands
*/
typedef struct {
    /// used only internally
    usart_handle_t _pstuart;
    int _timeout;
    aos_mutex_t at_mutex;
    aos_mutex_t at_uart_send_mutex;
    aos_mutex_t task_mutex;
    at_mode_t   _mode;
    at_status_t _status;

    // can be used externally
    slist_t task_l;
    /**
    * initialization
    *
    * @param u uart port used for AT communication.
    * @param send_delimiter string of characters to use as line delimiters for sending
    * @param recv_delimiter string of characters to use as line delimiters for receiving
    * @param timeout timeout of the connection
    */
    int (*init)(at_uart_param_t *param, int timeout);

    int (*deinit)(void);

    void (*set_mode)(at_mode_t m);

    void (*set_timeout)(int timeout);

    /*
    * This is a blocking API. It hanbles raw command sending, then is blocked
    * to wait for response.
    *
    * This API sends raw command (prepared by caller) and wait for raw response.
    * The caller is responsible for preparing the ready-to-send command
    * as well as parsing the response result. The caller is also responsible
    * for allocating/freeing rsp buffer.
    */
    int (*send_raw)(const char *command, char *rsp, uint32_t rsplen);

    /*
    * This is a blocking API. It hanbles data sending, it inside follows
    * below steps:
    *    1. Send first line (with send_delimeter);
    *    2. Waiting for prompt symbol, usually '>' character;
    *    3. Send data with 'len' length, and without send_delimeter.
    *
    * This API sends prefix command and data, and wait for raw response.
    * The caller is responsible for preparing the ready-to-send first line
    * as well as parsing the response result. The caller is also responsible
    * for allocating/freeing rsp buffer.
    */
    int (*send_data_2stage)(const char *fst, const char *data,
                            uint32_t len, char *rsp, uint32_t rsplen);

    /**
    * Write an array of bytes to the underlying stream.
    */
    int (*write)(const char *data, int size);

    int (*read_cb)(int idx, rdata_cb cb, linkstat_cb lcb);
} at_parser_t;

extern at_parser_t at;
extern at_parser_t at_sim800c;

#ifdef __cplusplus
}
#endif

#endif
