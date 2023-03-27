/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <time.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <aos/ringbuffer.h>
#include <yoc/record.h>
#include <yoc/mic.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#include <aos/aos.h>
#include <sys/stat.h>
#include <vfs.h>
#include <dirent.h>


#include "record_process.h"

#define TAG "ALGREC"

#define HELP_USAGE                                                                                                     \
    "usage:\r\n"                                                                                                       \
    "\trecord start/stop ws://192.168.50.216:8090 micdata.pcm chncnt [buf_size]\r\n"                                              \
    "\trecord start/stop usb:// chncnt\r\n"                                                                            \
    "\t\t-chncnt record channel count support 3 or 5\r\n"                                                              \
    "\t\t-chncnt 3:(2mic+ref) 5:(2mic+ref)+(ssp1+ssp2)\r\n"

#define MAX_WSREC_GROUP_COUNT 3
#define EVENT_RECORD_DROP_CHECK  (EVENT_USER + 0x2000)
#define WS_RECORD_BUFFER_SIZE (150 * 1024)

typedef struct _rec_mgr {
    dev_ringbuf_t ringbuffer;
    char         *rbuffer;
    aos_event_t   wait_evt;
    // aos_sem_t     wait_sem;
    rec_hdl_t hdl;
    /* debug info */
    uint32_t  total_rec;
    uint32_t  rb_min_left;
    uint32_t  rb_cur_left;
    long long last_print_ms;
    uint32_t  total_drop;
} rec_mgr_t;

static rec_mgr_t *g_rec_mgrs[MAX_WSREC_GROUP_COUNT];

static void data_ready(void *arg)
{
    unsigned   flags   = 0;
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    // LOGD(TAG, "read wait...");
    aos_event_get(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
    // aos_sem_wait(&rec_mgr->wait_sem, 100);
}

static void data_release(void *arg)
{
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;
    aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);
    // aos_sem_signal(&rec_mgr->wait_sem);
}

void rec_copy_data(int index, uint8_t *data, uint32_t size)
{
    if (index < MAX_WSREC_GROUP_COUNT) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[index];
        if (rec_mgr && rec_mgr->hdl && size > 0) {

            uint32_t write_space = ringbuffer_available_write_space(&rec_mgr->ringbuffer);
            if (write_space > size) {
                ringbuffer_write(&rec_mgr->ringbuffer, data, size);

                /* Debug info */
                rec_mgr->total_rec += size;
                rec_mgr->rb_cur_left = write_space;
                if (write_space < rec_mgr->rb_min_left) {
                    rec_mgr->rb_min_left = write_space;
                } else if (rec_mgr->rb_min_left == 0) {
                    rec_mgr->rb_min_left = write_space;
                }

            } else {
                // LOGW(TAG, "buffer full, drop %d", size);
                rec_mgr->total_drop += size;
            }

            data_release(rec_mgr);

            /* Debug info print */
            if (aos_now_ms() - rec_mgr->last_print_ms > 10000) {
                LOGW(TAG,
                     "wrb info: wtotal=%u wsapce min=%u wsapce=%u drop=%u\r\n",
                     rec_mgr->total_rec,
                     rec_mgr->rb_min_left,
                     rec_mgr->rb_cur_left,
                     rec_mgr->total_drop);
                rec_mgr->last_print_ms = aos_now_ms();
            }
        }
    }
}

int rec_start_ws(const char *url, const char *save_name, int rb_size, int chncnt)
{
    char url_from[64];
    char url_to[128];

    if (chncnt != 3 && chncnt != 5 && chncnt != 6) {
        LOGE(TAG, "chncnt error %d\n", chncnt);
        return -1;
    }

    int grpcnt = 1;
    for (int i = 0; i < grpcnt; i++) {
        rec_mgr_t *rec_mgr = (rec_mgr_t *)aos_zalloc_check(sizeof(rec_mgr_t));

        rec_mgr->rbuffer = (char *)aos_malloc_check(rb_size + 1);
        ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, rb_size + 1);
        aos_event_new(&rec_mgr->wait_evt, 0);
        // aos_sem_new(&rec_mgr->wait_sem, rb_size / frame);
        snprintf(url_from, sizeof(url_from), "ringbuffer://handle=%llu", (long long unsigned)(&rec_mgr->ringbuffer));

        time_t tt = time(NULL);
        if (i == 0) {
            snprintf(url_to, sizeof(url_to), "%s/%06lld_%dch_%s", url, (long long)tt, chncnt, save_name);
        }

        rec_hdl_t hdl = record_register(url_from, url_to);
        record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
        record_set_data_release_cb(hdl, data_release, (void *)rec_mgr);
        record_set_chunk_size(hdl, rb_size);
        record_start(hdl);
        rec_mgr->hdl  = hdl;
        g_rec_mgrs[i] = rec_mgr;
        LOGD(TAG, "start mic rec [%d]\n", i);
    }

    /*通知其他节点，开始录制*/
    // record_set_grpcnt(grpcnt);
    record_set_chncnt(chncnt);
    record_msg_publish(1);

    return 0;
}

int rec_start_usb(const char *url, int rb_size, int chncnt)
{
    char url_from[64];
    char url_to[64];

    if (chncnt != 3 && chncnt != 5 && chncnt != 6) {
        LOGE(TAG, "chncnt error %d\n", chncnt);
        return -1;
    }

    rec_mgr_t *rec_mgr = (rec_mgr_t *)aos_zalloc_check(sizeof(rec_mgr_t));

    rec_mgr->rbuffer = (char *)aos_malloc_check(rb_size + 1);
    ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, rb_size + 1);
    aos_event_new(&rec_mgr->wait_evt, 0);
    // aos_sem_new(&rec_mgr->wait_sem, rb_size / frame);

    snprintf(url_from, sizeof(url_from), "ringbuffer://handle=%llu", (long long unsigned)(&rec_mgr->ringbuffer));
    snprintf(url_to, sizeof(url_to), "usb://chncnt=%d", chncnt);

    rec_hdl_t hdl = record_register(url_from, url_to);
    record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
    record_set_data_release_cb(hdl, data_release, (void *)rec_mgr);

    int chunk_size = (rb_size / 5 / 1024) * 1024;
    record_set_chunk_size(hdl, chunk_size);
    record_start(hdl);
    rec_mgr->hdl  = hdl;
    g_rec_mgrs[0] = rec_mgr;
    LOGD(TAG, "record from %s to %s rbsize = %d chksize=%d\n", url_from, url_to, rb_size, chunk_size);

    /*通知其他节点，开始录制*/
    record_set_chncnt(chncnt);
    record_msg_publish(1);

    return 0;
}

int rec_start_mem(const char *url, int rb_size, int chncnt, uint8_t *mem, int size)
{
    char url_from[64];
    char url_to[64];

    if (chncnt != 3 && chncnt != 5 && chncnt != 6) {
        LOGE(TAG, "chncnt error %d\n", chncnt);
        return -1;
    }

    rec_mgr_t *rec_mgr = (rec_mgr_t *)aos_zalloc_check(sizeof(rec_mgr_t));

    rec_mgr->rbuffer = (char *)aos_malloc_check(rb_size + 1);
    ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, rb_size + 1);
    aos_event_new(&rec_mgr->wait_evt, 0);
    // aos_sem_new(&rec_mgr->wait_sem, rb_size / frame);

    snprintf(url_from, sizeof(url_from), "ringbuffer://handle=%llu", (long long unsigned)(&rec_mgr->ringbuffer));
    snprintf(url_to, sizeof(url_to), "mem://address=%llu&size=%d", (long long unsigned)mem, size);

    rec_hdl_t hdl = record_register(url_from, url_to);
    record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
    record_set_data_release_cb(hdl, data_release, (void *)rec_mgr);

    int chunk_size = 1024;
    record_set_chunk_size(hdl, chunk_size);
    record_start(hdl);
    rec_mgr->hdl  = hdl;
    g_rec_mgrs[0] = rec_mgr;
    LOGD(TAG, "record from %s to %s rbsize = %d chksize=%d, memsize=%d\n", url_from, url_to, rb_size, chunk_size, size);

    /*通知其他节点，开始录制*/
    record_set_chncnt(chncnt);
    record_msg_publish(1);

    return 0;
}

void rec_stop(void)
{
    /*通知其他节点，停止录制*/
    record_msg_publish(0);

    for (int i = 0; i < MAX_WSREC_GROUP_COUNT; i++) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[i];
        if (rec_mgr == NULL) {
            continue;
        }

        record_stop(rec_mgr->hdl);
        aos_event_free(&rec_mgr->wait_evt);
        // aos_event_free(&rec_mgr->wait_sem);
        aos_free(rec_mgr->rbuffer);
        ringbuffer_destroy(&rec_mgr->ringbuffer);
        record_unregister(rec_mgr->hdl);
        rec_mgr->hdl = NULL;
        aos_free(rec_mgr);
        g_rec_mgrs[i] = NULL;
    }
}

static int  g_record_start = 0;
static int  g_record_auto_start = 0;
static uint8_t *g_alg_mem_record = NULL;
static int g_alg_mem_size = 0;
static char *g_cmd_argv[10] = { NULL };
static char g_cmd_argc = 0;
static void drop_data_check_event(uint32_t event_id, const void *data, void *context);
static void cmd_record_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        printf("%s\n", HELP_USAGE);
        return;
    }

    //backup cmd
    if (g_cmd_argv != argv) {
        for(int i = 0; i < g_cmd_argc; i++) {
            if (g_cmd_argv[i]) {
                free(g_cmd_argv[i]);
            }
        }

        g_cmd_argc = argc;
        for(int i = 0; i < argc; i++) {
            g_cmd_argv[i] = strdup(argv[i]);
        }
        g_cmd_argv[argc] = NULL;
    }

    //cmd parse
    if ((strcmp(argv[1], "start") == 0) || (strcmp(argv[1], "autostart")) == 0) {
        if (argc < 4) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        if (g_record_start == 1) {
            printf("record already start\r\n");
            return;
        }

        //aui_mic_control(MIC_CTRL_ENABLE_LINEAR_AEC_DATA, 0);

        if (strcmp(argv[1], "autostart") == 0) {
            printf("auto start, start check timer\r\n");
            if (g_record_auto_start == 0) {
                event_subscribe(EVENT_RECORD_DROP_CHECK, drop_data_check_event, NULL);
                event_publish_delay(EVENT_RECORD_DROP_CHECK, NULL, 10000);
            }
            g_record_auto_start = 1;
        }

        int ret = 0;
        if (strstr(argv[2], "ws:")) {
            int chncnt = 0;
            int buf_size = WS_RECORD_BUFFER_SIZE;
            if (argc == 6) {
                buf_size = atoi(argv[5]);
                chncnt = atoi(argv[4]);
            } else if (argc == 5) {
                chncnt = atoi(argv[4]);
            } else {
                chncnt = 3;
            }
            printf("buf_size:%d\r\n", buf_size);
            ret = rec_start_ws(argv[2], argv[3], buf_size, chncnt);
        } else if (strstr(argv[2], "usb:")) {
            int chncnt = atoi(argv[3]);

            ret = rec_start_usb(argv[2], 256 * 1024, chncnt);
        } else if (strstr(argv[2], "mem:")) {
            g_alg_mem_size = atoi(argv[3]) * 1024;
            g_alg_mem_record = aos_zalloc_check(g_alg_mem_size);
            ret = rec_start_mem(argv[2], 100 * 1024, 3, g_alg_mem_record, g_alg_mem_size);
        } else {
            return;
        }

        if (ret == 0) {
            g_record_start = 1;
            printf("record start.\n");
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        rec_stop();
        //aui_mic_control(MIC_CTRL_ENABLE_LINEAR_AEC_DATA, 1);
        g_record_start = 0;
        g_record_auto_start = 0;
        event_unsubscribe(EVENT_RECORD_DROP_CHECK, drop_data_check_event, NULL);
        if (g_alg_mem_record && g_alg_mem_size > 0) {
            printf("start write to flash size=%d\r\n", g_alg_mem_size);
            int fd = aos_open("/rec_3ch.bin", (O_WRONLY | O_CREAT | O_TRUNC));
            aos_write(fd, g_alg_mem_record, g_alg_mem_size);
            aos_close(fd);
            aos_free(g_alg_mem_record);
            g_alg_mem_record = NULL;
            g_alg_mem_size = 0;
        }
        printf("record stop\n");
        return;
    } else {
        printf("%s\n", HELP_USAGE);
    }
    return;
}

void cli_reg_cmd_record(void)
{
    static const struct cli_command cmd_info = { "record", "record pcm", cmd_record_func };

    aos_cli_register_command(&cmd_info);
}

static void drop_data_check_event(uint32_t event_id, const void *data, void *context)
{
    if ( g_record_auto_start == 0) {
        return;
    }

    if (g_rec_mgrs[0] && g_rec_mgrs[0]->total_drop > 0) {
        /* lost stop */
        rec_stop();
        g_record_start = 0;
        LOGD(TAG, "drop check: stop record");
    } else {
        if (g_record_start == 0) {
            LOGD(TAG, "drop check: start record");
            cmd_record_func(NULL, 0, g_cmd_argc, g_cmd_argv);
        }
    }

    event_publish_delay(EVENT_RECORD_DROP_CHECK, NULL, 10000);
}
