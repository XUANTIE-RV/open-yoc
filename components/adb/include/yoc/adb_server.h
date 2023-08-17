/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_AT_SERVER_H
#define YOC_AT_SERVER_H

#include <stdarg.h>

#include <devices/uart.h>
#include <uservice/uservice.h>
#include <yoc/adb_port.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_MIN_SIZE (128)
#define BUFFER_MAX_SIZE (1024 + 16)
#define BUFFER_STEP_SIZE (32)

typedef enum {
    TEST_CMD,
    READ_CMD,
    WRITE_CMD,
    EXECUTE_CMD,

    INVAILD_CMD
} ADB_CMD_TYPE;

#define CRLF          "\r\n"
#define OK_CRLF       "OK\r\n"
#define CRLF_OK_CRLF  "\r\nOK\r\n"

#define ERR_CRLF      "ERROR\r\n"
#define CRLF_ERR_CRLF "\r\nERROR\r\n"

#define ADB_BACK_CMD(cmd)                      adbserver_send("%s\r\n", cmd + 2)
#define ADB_BACK_OK()                          adbserver_send("\r\nOK\r\n")
#define ADB_BACK_ERR()                         adbserver_send("\r\nERROR\r\n")
#define ADB_BACK_ERRNO(errno)                  adbserver_send("\r\nERROR: %d\r\n", errno)
#define ADB_BACK_RET_OK(cmd, par1)             adbserver_send("\r\n%s:%s\r\nOK\r\n", cmd + 2, par1)
#define ADB_BACK_ICA_RET_OK(cmd, par1)         adbserver_send("\r\n%s=%s\r\nOK\r\n", cmd + 2, par1)

#define ADB_BACK_OK_INT(val)                   adbserver_send("\r\n%d\r\nOK\r\n", val)
#define ADB_BACK_OK_INT2(val1, val2)           adbserver_send("\r\n%d,%d\r\nOK\r\n", val1, val2)
#define ADB_BACK_RET_OK_INT(cmd, val)          adbserver_send("\r\n%s:%d\r\nOK\r\n", cmd + 2, val)
#define ADB_BACK_RET_OK_INT2(cmd, val1, val2)  adbserver_send("\r\n%s:%d,%d\r\nOK\r\n", cmd + 2, val1, val2)
#define ADB_BACK_RET_OK2(cmd, par1, par2)      adbserver_send("\r\n%s:%s,%s\r\nOK\r\n", cmd + 2, par1, par2)
#define ADB_BACK_RET_OK_HEAD(cmd, ret)         adbserver_send("\r\n%s:%s", cmd + 2, ret)
#define ADB_BACK_RET_OK_CMD_HEAD(cmd)          adbserver_send("\r\n%s:", cmd + 2)
#define ADB_BACK_RET_OK_END()                  adbserver_send("\r\nOK\r\n")
#define ADB_BACK_RET_ERR(cmd, errno)           adbserver_send("\r\n%s:%d\r\nERROR\r\n", cmd + 2, errno)
#define ADB_BACK_CME_ERR(errno)                adbserver_send("\r\n+CME ERROR: %d\r\n", errno)
#define ADB_BACK_CIS_ERR(errno)                adbserver_send("\r\n+CIS ERROR: %d\r\n", errno)
#define ADB_BACK_STR(str)                      adbserver_send("%s", str)

typedef void (*cmd_callback)(char *cmd, int type, char *data);
typedef void (*pass_through_cb)(void *data, int len);

typedef struct adbserver_cmd {
    const char        *cmd;
    const cmd_callback cb;
} adbserver_cmd_t;

int adbserver_init(utask_t *task, uint8_t port, rvm_hal_uart_config_t *config);
int  adbserver_channel_init(utask_t *task, uint8_t port, void *config, adb_channel_t *channel);
int  adbserver_send(const char *format, ...);
int  adbserver_sendv(const char *command, va_list args);
int  adbserver_write(const void *data, int size);
int  adbserver_add_command(const adbserver_cmd_t adb_cmd[]);
void adbserver_set_output_terminator(const char *terminator);
void adbserver_set_para_delimiter(char delimiter);
void adbserver_set_timeout(int timeout);
int  adbserver_scanf(const char *format, ...);
int adbserver_pass_through(int len, pass_through_cb cb);
int adbserver_pass_through_exit(void);
void adbserver_enabled(int flag);
void adbserver_uart_config(rvm_hal_uart_config_t *config);

#ifdef __cplusplus
}
#endif


#endif
