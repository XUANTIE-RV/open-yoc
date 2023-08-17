/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __ADB_H__
#define __ADB_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ADB_FILE_PATH_SIZE_MAX 256
#define ADB_TRANS_SIZE_PER_PACKET_VALID_DATA (6 * 128)
#define ADB_TRANS_SIZE_PER_PACKET (6 * 128 + 8)
#define AT_CMD_MAX_ARG_NUM 10

typedef enum _adb_status {
    ADB_INIT     = 1,
    ADB_CONNECT  = 2,
    ADB_DOWNLOAD = 3,
    ADB_UPLOAD   = 4,
    ADB_STOP     = 5,
    ADB_TRANSMIT_DATA = 6,
} adb_status_e;

typedef enum adb_error {
    TIME_OUT         = 0,
    CRC_ERROR        = 1,
    SIZE_LONG_ERROR  = 2,
    SIZE_SHORT_ERROR = 3,
    FILE_EXIST       = 4,
    FILE_NOT_EXIST   = 5,
} adb_error_e;

typedef enum adb_event {
    ADB_PUSH_START,
    ADB_PUSH_FINISH,
    ADB_PULL_START,
    ADB_PULL_FINISH,
} adb_event_t;

typedef void (*adb_event_cb_t)(adb_event_t event);

/* push */
#define AT_PUSHINFO     {"AT+PUSHINFO",   adb_cmd_pushinfo}
#define AT_PUSHSTART    {"AT+PUSHSTART",  adb_cmd_pushstart}
#define AT_PUSHFINISH   {"AT+PUSHFINISH", adb_cmd_pushfinish}
#define AT_PUSHPOST     {"AT+PUSHPOST",   adb_cmd_pushpost}

/* pull */
#define AT_PULLINFO     {"AT+PULLINFO",   adb_cmd_pullinfo}
#define AT_PULLSTART    {"AT+PULLSTART",  adb_cmd_pullstart}
#define AT_PULLFINISH   {"AT+PULLFINISH", adb_cmd_pullfinish}
#define AT_PULLREQ      {"AT+PULLREQ",    adb_cmd_pullreq}

#define AT_DELETE        {"AT+ADBDELETE", adb_cmd_delete}

#define AT_DEVICES      {"AT+ADBDEVICES", adb_cmd_devices}
#define AT_ADBOK        {"AT+ADBOK",      adb_cmd_adbok}
#define AT_ADBCLI       {"AT+ADBCLI",     adb_cmd_cli}
#define AT_ADBSTART     {"AT+ADBSTART",    adb_cmd_start}
#define AT_ADBSTOP      {"AT+ADBSTOP",    adb_cmd_stop}
#define AT_ADBEXIT      {"AT+ADBEXIT",    adb_cmd_exit}
#define AT_RECORDSTART  {"AT+RECORDSTART", adb_cmd_tramsmit_start}
#define AT_RECORDSTOP   {"AT+RECORDSTOP", adb_cmd_tramsmit_stop}
#define AT_NULL         { NULL,      NULL}                       //最后必须为NULL

void adb_cmd_pushinfo(char *cmd, int type, char *data);
void adb_cmd_pullinfo(char *cmd, int type, char *data);
void adb_cmd_pushstart(char *cmd, int type, char *data);
void adb_cmd_pushfinish(char *cmd, int type, char *data);
void adb_cmd_pullstart(char *cmd, int type, char *data);
void adb_cmd_pullfinish(char *cmd, int type, char *data);
void adb_cmd_pushpost(char *cmd, int type, char *data);
void adb_cmd_pullreq(char *cmd, int type, char *data);
void adb_cmd_delete(char *cmd, int type, char *data);
void adb_cmd_adbok(char *cmd, int type, char *data);
void adb_cmd_devices(char *cmd, int type, char *data);
void adb_cmd_cli(char *cmd, int type, char *data);
void adb_cmd_start(char *cmd, int type, char *data);
void adb_cmd_stop(char *cmd, int type, char *data);
void adb_cmd_exit(char *cmd, int type, char *data);
adb_status_e  adb_get_status(void);
void adb_cmd_tramsmit_start(char *cmd, int type, char *data);
void adb_cmd_tramsmit_stop(char *cmd, int type, char *data);
int  adb_cmd_transmit_write(uint8_t *buff, int size);
int adb_get_transmit_channel(void);
int adb_get_compress_flag(void);
int adb_transmit_raw_data_add(uint32_t size);
int adb_event_register(adb_event_cb_t cb);
int adb_start(void);
#endif
