/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <sys/stat.h>
#include <aos/crc16.h>
#include <yoc/adb_server.h>
#include <yoc/adb.h>
#include <aos/ringbuffer.h>
#include <vfs.h>
#include <dirent.h>

#define TAG "adb"

#define RINGBUFFER_WRITE_EVENT 0x000000F0
static char device[] = "4144426465766963656964";

typedef struct {
    uint8_t * buffer;
    char *    fpath;
    size_t    offset;
    size_t    size;
    size_t    size_total;
    int       pfile;
    aos_sem_t sem;
} recv_t;

typedef struct {
    uint8_t * buffer;
    char *    fpath;
    size_t    offset;
    size_t    size;
    int       pfile;
    aos_sem_t req_sem;
    aos_sem_t ack_sem;
} send_t;

typedef struct {
    size_t       size_total;
    size_t       offset;
    uint32_t     transmit_rb_full_count;
    uint32_t     transmit_rb_max;
} transmit_t;

typedef struct {
    aos_task_t   task;
    send_t *     send;
    recv_t *     recv;
    transmit_t * transmit;
    char *       device_id;
    aos_sem_t    connect_sem;
    adb_status_e status;
    uint8_t      exit;
    adb_event_cb_t adb_event_cb;
} adb_t;

static adb_t *adb = NULL;

#define ADB_PUSHREQ()       adbserver_send("\r\n+PUSHREQ\r\n")
#define ADB_PUSHRETRANS()   adbserver_send("\r\n+PUSHRET\r\n")
#define ADB_PULLRETRANS()   adbserver_send("\r\n+PULLRET\r\n")
#define ADB_ER()            adbserver_send("\r\nER\r\n")
#define ADB_CLICMD_RETURN() adbserver_send("\r\nSHELL_RETURN\r\n")
#define ADB_TRANSMITOVER()  adbserver_send("\r\nTRANSMITOVER\r\n")

#define PIECE_SIZE 1024

extern int strsplit(char **array, size_t count, char *data, const char *delim);
void adb_cmd_cli(char *cmd, int type, char *data)
{
#ifndef CLI_MAX_ARG_NUM
#define CLI_MAX_ARG_NUM 20
#endif
    if (type == WRITE_CMD) {
        char *items[CLI_MAX_ARG_NUM + 1];

        memset(&items, 0, sizeof(items));
        int count = strsplit(items, CLI_MAX_ARG_NUM, data, " ");

        int32_t i, n, commands_num;
        struct  cli_command *cmd = NULL;

        commands_num = aos_cli_get_commands_num();
        for (i = 0, n = 0; i < commands_num; i++) {
            cmd = aos_cli_get_command(i);
            if (!cmd) {
                continue;
            }
            if (cmd->name) {
                if (items[0] != NULL && strcmp(items[0], cmd->name) == 0) {
                    if (strcmp(cmd->name, "reboot") == 0) {
                        ADB_CLICMD_RETURN();
                    }
                    cmd->function(NULL, 0, count, items);
                    ADB_CLICMD_RETURN();
                    return;
                }

                n++;
            }
        }
    }
}
static struct {
    dev_ringbuf_t rb_hdl;
    uint8_t *     rb_buffer;
    aos_event_t   evt;
} transmit_rb_priv;

static uint32_t transmit_raw_data_count = 0;
int adb_cmd_transmit_write(uint8_t *buff, int size)
{
    if (transmit_rb_priv.rb_buffer == NULL) {
        return -1;
    }

    int write_space = ringbuffer_available_write_space(&transmit_rb_priv.rb_hdl);
    if (write_space < size) {
        LOGW(TAG, "pcm rb full\r\n");
        adb->transmit->transmit_rb_full_count++;
        return -1;
    }
    ringbuffer_write(&transmit_rb_priv.rb_hdl, buff, size);

    int rb_count = ringbuffer_available_read_space(&transmit_rb_priv.rb_hdl);
    if (rb_count > adb->transmit->transmit_rb_max) {
        adb->transmit->transmit_rb_max = rb_count;
    }

    aos_event_set(&transmit_rb_priv.evt, RINGBUFFER_WRITE_EVENT, AOS_EVENT_OR);

    return 0;
}
int adb_cmd_transmit_read(uint8_t *buf, int size)
{
    int          ret, remain_len = size;
    unsigned int actl_flags = 0;

    while (transmit_rb_priv.rb_buffer) {
        ret = ringbuffer_read(&transmit_rb_priv.rb_hdl, buf + size - remain_len, remain_len);
        if(ret > 0) {
            remain_len -= ret;
            if (remain_len <= 0) {
                break;
            }
        } else {
            if(transmit_raw_data_count < adb->transmit->size_total)
                aos_event_get(&transmit_rb_priv.evt, RINGBUFFER_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
            else
                break;
        }
    }

    return size - remain_len;
}

static int tran_channel = 3;
static int tran_sample_bits = 16;
static int tran_sample_rate = 16000;
#define transmit_payload_per_packet 768
#define transmit_data_per_packet (transmit_payload_per_packet + 8)

__attribute__((weak)) void adb_transmit_init_hook(void)
{

}
__attribute__((weak)) void adb_transmit_uninit_hook(void)
{
    
}
static uint32_t adb_normal_send_index = 0;
#define ADB_TRAN_RINGBUFF_SIZE (50 * 1024)
static int adb_compress_trabsmit_flag = 0;
int adb_get_transmit_channel(void)
{
    return tran_channel;
}
int adb_get_compress_flag(void)
{
    return adb_compress_trabsmit_flag;
}
int adb_transmit_raw_data_add(uint32_t size)
{
    transmit_raw_data_count += size;
    return (transmit_raw_data_count >= adb->transmit->size_total ? 1 : 0);
}
void adb_cmd_tramsmit_start(char *cmd, int type, char *data)
{
    static int adb_first_transmit_flag = 0;
    adb_normal_send_index              = 0;
    adb->transmit->offset              = 0;
    transmit_raw_data_count            = 0;
    char *items[AT_CMD_MAX_ARG_NUM];

    memset(&items, 0, sizeof(items));
    strsplit(items, CLI_MAX_ARG_NUM, data, ",");

    if(items[1] != NULL) 
        tran_channel = atoi(items[1]);
    else 
        tran_channel = 3;

    if(items[2] != NULL)
        adb_compress_trabsmit_flag = atoi(items[2]);
    else 
        adb_compress_trabsmit_flag = 0;

    if (items[0] == NULL)
        adb->transmit->size_total = -1;
    else
        adb->transmit->size_total = atoi(items[0]) * tran_sample_rate * tran_channel * (tran_sample_bits / 8); //s*sample_rate*channel*sample_accuracy

    adb_transmit_init_hook();

    if (!adb_first_transmit_flag) {
        adb_first_transmit_flag    = 1;
        transmit_rb_priv.rb_buffer = aos_malloc(ADB_TRAN_RINGBUFF_SIZE + 1);
        if (transmit_rb_priv.rb_buffer == NULL) {
            ADB_BACK_ERR();
            return;
        }
        ringbuffer_create(&transmit_rb_priv.rb_hdl, (char *)transmit_rb_priv.rb_buffer,
                          ADB_TRAN_RINGBUFF_SIZE + 1);
        aos_event_new(&transmit_rb_priv.evt, 0);
    }

    adb->status = ADB_TRANSMIT_DATA;
    aos_sem_signal(&adb->connect_sem);
}
void adb_cmd_tramsmit_stop(char *cmd, int type, char *data)
{
    adb->status = ADB_STOP;
    aos_event_set(&transmit_rb_priv.evt, RINGBUFFER_WRITE_EVENT, AOS_EVENT_OR);

    if (transmit_rb_priv.rb_buffer) {
        memset(transmit_rb_priv.rb_buffer, 0, ADB_TRAN_RINGBUFF_SIZE);
        transmit_rb_priv.rb_hdl.ridx = 0;
        transmit_rb_priv.rb_hdl.widx = 0;
    }
    ADB_BACK_OK();
    adb_transmit_uninit_hook();
}
void adb_cmd_pushinfo(char *cmd, int type, char *data)
{
    char buf[128];
    char *items[AT_CMD_MAX_ARG_NUM];

    LOGD(TAG, "adb_cmd_pushinfo");
    if (type == WRITE_CMD) {
        memset(&items, 0, sizeof(items));
        strsplit(items, CLI_MAX_ARG_NUM, data, ",");
        strncpy(adb->recv->fpath, items[0], ADB_FILE_PATH_SIZE_MAX);
        if(items[1])
            adb->recv->size_total = atoi(items[1]);
        else
            adb->recv->size_total = -1;

        snprintf(buf, 128, "\r\n%s\r\n%d\r\n%d\r\n%s\r\n", "PUSHINFO", 0 /*unspport offset*/, PIECE_SIZE, "OK");
        adbserver_send(buf);
    }
}

void adb_cmd_pullinfo(char *cmd, int type, char *data)
{
    char        buf[128];
    aos_stat_t s;

    LOGD(TAG, "adb_cmd_pullinfo");
    if (type == WRITE_CMD) {
        strncpy(adb->send->fpath, data, ADB_FILE_PATH_SIZE_MAX);
        if (aos_stat(adb->send->fpath, &s) < 0) {
            LOGE(TAG, "%s not existed\n", adb->send->fpath);
            ADB_BACK_ERRNO(FILE_NOT_EXIST);
            return;
        }

        uint32_t total_len       = s.st_size;
        adb->send->size = (size_t)total_len;
        snprintf(buf, 128, "\r\n%s\r\n%u\r\n%d\r\n%s\r\n", "PULLINFO", total_len, PIECE_SIZE, "OK");
        adbserver_send(buf);
    }
}

static int is_pushstart_cmd = 0;
void adb_cmd_pushstart(char *cmd, int type, char *data)
{
    is_pushstart_cmd = 1;
    LOGD(TAG, "adb_cmd_pushstart");
    adb->adb_event_cb(ADB_PUSH_START);

    if ((adb->recv->pfile = aos_open(adb->recv->fpath, (O_WRONLY | O_CREAT | O_TRUNC))) < 0) {
        LOGD(TAG, "the file %s can not be opened.\n", adb->recv->fpath);
        ADB_BACK_ERR();
    }
    ADB_BACK_OK();
    adb->recv->offset = 0;
    adb->recv->size   = 0;
    adb->status       = ADB_DOWNLOAD;
    aos_sem_signal(&adb->connect_sem);

}

void adb_cmd_pushfinish(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pushfinish");
    ADB_BACK_OK();
    adb->recv->offset = 0;
    adb->recv->size   = 0;
    adb->status       = ADB_STOP;
    aos_close(adb->recv->pfile);

    adb->adb_event_cb(ADB_PUSH_FINISH);
}

void adb_cmd_pullstart(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pullstart");
    adb->adb_event_cb(ADB_PULL_START);

    if ((adb->send->pfile = aos_open(adb->send->fpath, O_RDONLY)) < 0) {
        LOGD(TAG, "the file %s can not be opened.\n", adb->send->fpath);
        ADB_BACK_ERRNO(FILE_NOT_EXIST);
        return;
    }

    adb->status = ADB_UPLOAD;
    aos_sem_signal(&adb->connect_sem);
    ADB_BACK_OK();
}

void adb_cmd_pullfinish(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pullfinish");
    ADB_BACK_OK();
    adb->status = ADB_STOP;
    aos_close(adb->send->pfile);

    adb->adb_event_cb(ADB_PULL_FINISH);
}

static void pass_th_cb(void *data, int len)
{
    LOGD(TAG, "pass th get data, %d", len);
    memcpy(adb->recv->buffer, (uint8_t *)data, len);
    adb->recv->size = len;
    aos_sem_signal(&adb->recv->sem);
}

void adb_cmd_pushpost(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pushpost");
    if (type == WRITE_CMD) {
        int len;
        adbserver_scanf("%d", &len);
        LOGD(TAG, "pass len:%d", len);
        adbserver_pass_through(len, pass_th_cb);
        ADB_BACK_OK();
    }
}

void adb_cmd_pullreq(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pullreq");
    aos_sem_signal(&adb->send->req_sem);
}

void adb_cmd_adbok(char *cmd, int type, char *data)
{
    LOGD(TAG, "adb_cmd_pullreq");
    aos_sem_signal(&adb->send->ack_sem);
}

void adb_cmd_devices(char *cmd, int type, char *data)
{
    adbserver_send(adb->device_id);
}

void adb_cmd_start(char *cmd, int type, char *data)
{
    /* ADB模式下执行AT+ADBSTART，不做任何反馈 */
    ;
}

void adb_cmd_stop(char *cmd, int type, char *data)
{
    if (is_pushstart_cmd == 1) {
        /* 需要删除当 tydb 推送失败后的空文件*/
        is_pushstart_cmd = 0;
        aos_stat_t s;

        if (aos_stat(adb->recv->fpath, &s) >= 0) {
            uint32_t total_len = s.st_size;

            if (adb->recv->size_total != -1 && (size_t)total_len < adb->recv->size_total) {
                LOGD(TAG, "rm %s", adb->recv->fpath);
                aos_unlink(adb->recv->fpath);
            }
        }
    }
    adbserver_pass_through_exit();
}

void adb_cmd_exit(char *cmd, int type, char *data)
{
    adbserver_enabled(0);
}

static int adb_read(uint8_t *buffer, int length, int timeoutms)
{
    int         ret;
    uint16_t    r_crc16, calc_crc16;
    int         content_len = 0;
    aos_stat_t s;

    LOGD(TAG, "adb_read, timeoutms:%d", timeoutms);

    ADB_PUSHREQ();

    ret = aos_sem_wait(&adb->recv->sem, timeoutms);
    LOGD(TAG, "sem_wait ret:%d", ret);
    if (ret != 0) {
        LOGD(TAG, "recv timeout...");
        //AT_BACK_ERRNO(TIME_OUT);
        ret = -2;
        goto _err;
    }
    if (adb->recv->size > length) {
        LOGE(TAG, "size too long");
        //AT_BACK_ERRNO(SIZE_LONG_ERROR);
        ret = -3;
        goto _err;
    } else if (adb->recv->size <= 2) {
        LOGE(TAG, "size too short");
        //AT_BACK_ERRNO(SIZE_SHORT_ERROR);
        ret = -4;
        goto _err;
    }
    content_len = adb->recv->size;
    LOGD(TAG, "content_len:%d", content_len);

    // dump_data(buffer, content_len);
    // crc16 verify
    r_crc16 = adb->recv->buffer[adb->recv->size - 2];
    r_crc16 <<= 8;
    r_crc16 |= adb->recv->buffer[adb->recv->size - 1];
    LOGD(TAG, "read crc16:0x%x", r_crc16);

    calc_crc16 = crc16(0, adb->recv->buffer, adb->recv->size - 2);
    LOGD(TAG, "calc crc16:0x%x", calc_crc16);
    if (calc_crc16 != r_crc16) {
        LOGE(TAG, "crc verify error");
        ADB_BACK_ERRNO(CRC_ERROR);
        ret = -5;
        goto _err;
    }

    content_len -= 2; // remove crc16
    ADB_BACK_OK();
    return content_len;
_err:
    if (adb->status == ADB_DOWNLOAD) {
        //删除错误文件 发送重传指令
        if (aos_stat(adb->recv->fpath, &s) >= 0) {
            aos_close(adb->recv->pfile);
            aos_unlink(adb->recv->fpath);
        }
        ADB_ER();
        ADB_PUSHRETRANS();
    }
    adbserver_pass_through_exit();
    adb->recv->size = 0;
    return ret;
}

static int adb_write(uint8_t *buffer, int length, int timeoutms)
{
    int         ret;
    uint16_t    calc_crc16;
    int         content_len = 0;
    char        buf[128];
    aos_stat_t s;

    LOGD(TAG, "adb_write, timeoutms:%d", timeoutms);

    ret = aos_sem_wait(&adb->send->req_sem, timeoutms);
    LOGD(TAG, "req_sem ret:%d", ret);
    if (ret != 0) {
        LOGD(TAG, "recv timeout...");
        //AT_BACK_ERRNO(TIME_OUT);
        goto _err;
    }

    snprintf(buf, 128, "\r\nAT+PULLPOST=%4d\r\n", length);
    adbserver_send(buf);

    ret = aos_sem_wait(&adb->send->ack_sem, timeoutms);
    LOGD(TAG, "pull post ack_sem ret:%d", ret);
    if (ret != 0) {
        LOGD(TAG, "recv timeout...");
        //AT_BACK_ERRNO(TIME_OUT);
        goto _err;
    }

    content_len = length;
    LOGD(TAG, "content_len:%d", content_len);

    calc_crc16         = crc16(0, buffer, length - 2);
    buffer[length - 2] = (calc_crc16 >> 8) & 0xFF;
    buffer[length - 1] = calc_crc16 & 0xFF;
    LOGD(TAG, "calc crc16:0x%x", calc_crc16);

    adbserver_write(buffer, length);

    ret = aos_sem_wait(&adb->send->ack_sem, timeoutms);
    LOGD(TAG, "data upload ack_sem ret:%d", ret);
    if (ret != 0) {
        LOGD(TAG, "recv timeout...");
        //AT_BACK_ERRNO(TIME_OUT);
        goto _err;
    }

    content_len -= 2; // remove crc16
    return content_len;
_err:
    if (aos_stat(adb->send->fpath, &s) >= 0)
        aos_close(adb->send->pfile);
    ADB_ER();
    ADB_PULLRETRANS();
    return -1;
}
void adb_cmd_delete(char *cmd, int type, char *data)
{
    aos_stat_t s;
    char *      del_file = data;

    if (aos_stat(del_file, &s) >= 0) {
        aos_unlink(del_file);
        ADB_BACK_OK();
    } else {
        ADB_BACK_ERR();
    }
}

static int  adb_task_running_flag;
static void adb_task(void *arg)
{
    int ret;
    adb->exit             = 0;
    adb_task_running_flag = 1;
    uint16_t calc_crc16;
    uint8_t  normal_send_data_buff[transmit_data_per_packet];

    while (adb_task_running_flag) {
        if (adb->status == ADB_DOWNLOAD) { //往设备传文件
            int size = adb_read(adb->recv->buffer, PIECE_SIZE + 2, 1000);
            if (size <= 0) {
                adb->status = ADB_STOP;
            }
            LOGI(TAG, "read: %d", size);
            //aos_lseek(adb->recv->pfile, adb->recv->offset, SEEK_SET);
            size = aos_write(adb->recv->pfile, adb->recv->buffer, size);
            LOGI(TAG, "fwrite: %d", size);
            if (size > 0) {
                adb->recv->offset += size;
            } else {
                adb->status = ADB_STOP;
            }
            if (adb->recv->offset == adb->recv->size_total) {
                adb->status = ADB_STOP;
            }
        } else if (adb->status == ADB_UPLOAD) { //从设备拉文件
            if (!adb->send->fpath) {
                continue;
            }
            int pfile = adb->send->pfile;
            int   size;
            adb->send->offset = 0;
            while (1) {
                size = PIECE_SIZE;
                if (adb->send->offset == adb->send->size) {
                    adb->status = ADB_STOP;
                    break;
                }

                if (adb->send->offset + size > adb->send->size) {
                    size = adb->send->size - adb->send->offset;
                }
                aos_lseek(pfile, adb->send->offset, SEEK_SET);
                size = aos_read(pfile, adb->send->buffer, size);
                if (size <= 0) {
                    adb->status = ADB_STOP;
                    break;
                }
                LOGI(TAG, "fread: %d", size);
                adb->send->offset += size;

                size = adb_write(adb->send->buffer, size + 2, 3000);
                if (size > 0) {
                    LOGI(TAG, "upload: %d", size);
                } else {
                    adb->status = ADB_STOP;
                    break;
                }
            }

        } else if (adb->status == ADB_TRANSMIT_DATA) {
            normal_send_data_buff[0] = 0x12;
            normal_send_data_buff[1] = 0x34;
            normal_send_data_buff[2] = 0x56;
            normal_send_data_buff[3] = 0x78;
            while (adb->status == ADB_TRANSMIT_DATA) {
                normal_send_data_buff[4] = adb_normal_send_index / 256;
                normal_send_data_buff[5] = adb_normal_send_index % 256;

                ret = adb_cmd_transmit_read(normal_send_data_buff + 6,
                                      transmit_payload_per_packet);
                if(ret != transmit_payload_per_packet) {
                   memcpy(normal_send_data_buff + 6 + ret, "fLaC", 4);
                   memset(normal_send_data_buff + 6 + ret + 4, 0, transmit_payload_per_packet - 6 - ret - 4);
                }

                calc_crc16 =
                    crc16(0, normal_send_data_buff + 6, transmit_payload_per_packet);
                normal_send_data_buff[transmit_data_per_packet - 2] = (calc_crc16 >> 8) & 0xFF;
                normal_send_data_buff[transmit_data_per_packet - 1] = calc_crc16 & 0xFF;
                adbserver_write(normal_send_data_buff, transmit_data_per_packet);
                adb->transmit->offset += transmit_payload_per_packet;

                if ((adb->transmit->offset >= adb->transmit->size_total) ||  \
                    ((transmit_raw_data_count >= adb->transmit->size_total) && (ringbuffer_available_read_space(&transmit_rb_priv.rb_hdl) == 0))) {
                    ADB_TRANSMITOVER();
                    memset(transmit_rb_priv.rb_buffer, 0, ADB_TRAN_RINGBUFF_SIZE);
                    transmit_rb_priv.rb_hdl.ridx = 0;
                    transmit_rb_priv.rb_hdl.widx = 0;
                    adb->status = ADB_STOP;
                }

                adb_normal_send_index++;
                if (adb_normal_send_index > 65535)
                    adb_normal_send_index = 0;
            }

        } else { /*ADB_INIT, ADB_STOP,ADB_CONNECT*/
            aos_sem_free(&adb->send->ack_sem);
            aos_sem_free(&adb->send->req_sem);
            aos_sem_free(&adb->recv->sem);
            aos_sem_new(&adb->send->req_sem, 0);
            aos_sem_new(&adb->send->ack_sem, 0);
            aos_sem_new(&adb->recv->sem, 0);

            aos_sem_wait(&adb->connect_sem, -1);
        }
    }
    aos_task_exit(0);
}

static void buffer_init(void)
{
    adb->send->buffer = aos_malloc(PIECE_SIZE + 2);
    adb->recv->buffer = aos_malloc(PIECE_SIZE + 2);
    adb->send->fpath  = aos_malloc(ADB_FILE_PATH_SIZE_MAX);
    adb->recv->fpath  = aos_malloc(ADB_FILE_PATH_SIZE_MAX);
}

int adb_start(void)
{
    adb = aos_malloc(sizeof(adb_t));
    if (adb) {
        adb->send     = aos_malloc(sizeof(send_t));
        adb->recv     = aos_malloc(sizeof(recv_t));
        adb->transmit = aos_malloc(sizeof(transmit_t));
        buffer_init();
        adb->status                 = ADB_INIT;
        adb->transmit->transmit_rb_full_count = 0;
        adb->transmit->transmit_rb_max        = 0;
    }
    adb->device_id = device;
    aos_sem_new(&adb->connect_sem, 0);
    aos_sem_new(&adb->send->ack_sem, 0);
    aos_sem_new(&adb->send->req_sem, 0);
    aos_sem_new(&adb->recv->sem, 0);
    aos_task_new_ext(&adb->task, "adb_task", adb_task, NULL, 4096, AOS_DEFAULT_APP_PRI);

    return 0;
}

#if 0
int adb_stop(void)
{
    adb_task_running_flag = 0;
    aos_free(adb->send->buffer);
    aos_free(adb->send->fpath);
    aos_free(adb->recv->buffer);
    aos_free(adb->recv->fpath);
    aos_free(adb->send);
    aos_free(adb->recv);
    aos_free(adb->transmit);
    aos_free(adb);

    aos_sem_free(&adb->connect_sem);
    aos_sem_free(&adb->send->ack_sem);
    aos_sem_free(&adb->send->req_sem);
    aos_sem_free(&adb->recv->sem);

    return 0;
}
#endif

adb_status_e adb_get_status(void)
{
    if (adb == NULL) {
        return ADB_STOP;
    }

    return adb->status;
}

void adb_info_print(int clear)
{
    if (adb == NULL) {
        printf("run adb cmd first\r\n");
        return;
    }

    if (clear) {
        adb->transmit->transmit_rb_full_count = 0;
    }
    printf("trans rb full %d\r\n", adb->transmit->transmit_rb_full_count);
    printf("trans rb max %d\r\n", adb->transmit->transmit_rb_max);
}

int adb_event_register(adb_event_cb_t cb)
{
    if (adb) {
        adb->adb_event_cb = cb;
        return 0;
    }
    return -1;
}
