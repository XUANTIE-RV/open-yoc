/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <time.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/ringbuffer.h>
#include <yoc/record.h>
#include <yoc/mic.h>
#include "mic_internal.h"

#define TAG "mic_record"

#define CHUNK_SIZE 2560
#define RINGBUFFER_SIZE (CHUNK_SIZE * 30 + 1)

typedef struct _rec_mgr {
    dev_ringbuf_t ringbuffer;
    char *rbuffer;
    aos_event_t wait_evt;
    rec_hdl_t hdl;
} rec_mgr_t;

rec_mgr_t *g_rec_mgrs[MIC_REC_COUNT];
static int print_period = 0;

// #define MIC_REC_DEBUG

#ifdef MIC_REC_DEBUG
#include "test2.wav.c"
#include "cpu1.c"
#include "cpu2.c"

int g_mic_rec_test_rpos[MIC_REC_COUNT];

static void mic_rec_test_init(void)
{
    for (int i = 0; i < MIC_REC_COUNT; i++) {
        g_mic_rec_test_rpos[i] = 0;
    }
}

static int mic_rec_test_set_data(int index, uint8_t *data, uint32_t size)
{
    int realsize = size;

    if (index == 0) {
        if (g_mic_rec_test_rpos[index] + size > sizeof(local_audio_test2)) {
            realsize = sizeof(local_audio_test2) - g_mic_rec_test_rpos[index];
            if (realsize < 0) {
                return 0;
            }
        }
        
        memcpy(data, &local_audio_test2[g_mic_rec_test_rpos[index]], realsize);
        g_mic_rec_test_rpos[index] += realsize;        
    } else if (index == 1) {
        if (g_mic_rec_test_rpos[index] + size > sizeof(local_audio_cpu1)) {
            realsize = sizeof(local_audio_cpu1) - g_mic_rec_test_rpos[index];
            if (realsize < 0) {
                return 0;
            }
        }
        memcpy(data, &local_audio_cpu1[g_mic_rec_test_rpos[index]], realsize);
        g_mic_rec_test_rpos[index] += realsize;    
    } else if (index == 2) {
        if (g_mic_rec_test_rpos[index] + size > sizeof(local_audio_cpu2)) {
            realsize = sizeof(local_audio_cpu2) - g_mic_rec_test_rpos[index];
            if (realsize < 0) {
                return 0;
            }
        }
        memcpy(data, &local_audio_cpu2[g_mic_rec_test_rpos[index]], realsize);
        g_mic_rec_test_rpos[index] += realsize;    
    }

    return realsize;
}
#endif

void mic_rec_copy_data(int index, uint8_t *data, uint32_t size)
{
    if (index < MIC_REC_COUNT) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[index];
        if (rec_mgr && rec_mgr->hdl && size > 0) {
#ifdef MIC_REC_DEBUG
            if ((size = mic_rec_test_set_data(index, data, size)) <= 0) {
                return;
            }
#endif
            if (index == 0) {
                if (print_period % (320 * 1024) == 0) {
                    printf(".");
                }
                print_period += size;
            }

            ringbuffer_write(&rec_mgr->ringbuffer, data, size);
            if (ringbuffer_available_read_space(&rec_mgr->ringbuffer) > 0) {
                aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);      
            }     
        }
    }
}

static void data_ready(void *arg)
{
    unsigned flags;
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    // LOGD(TAG, "read wait...");
    aos_event_get(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
}

void mic_rec_start(const char *url, const char *save_name)
{
#ifdef MIC_REC_DEBUG
    mic_rec_test_init();
#endif
    for (int i = 0; i < MIC_REC_COUNT; i++) {
        rec_mgr_t *rec_mgr = aos_zalloc_check(sizeof(rec_mgr_t));

        rec_mgr->rbuffer = aos_malloc_check(RINGBUFFER_SIZE);
        ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, RINGBUFFER_SIZE);
        aos_event_new(&rec_mgr->wait_evt, 0);
        char buf[64];
        snprintf(buf, sizeof(buf), "ringbuffer://handle=%u", (uint32_t)(&rec_mgr->ringbuffer));
        char buf2[128];
        time_t tt = time(NULL);
        if (i == 0) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "mic", save_name);
        } else if (i == 1) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "ref", save_name);
        } else if (i == 2) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "proc", save_name);
        }
        
        rec_hdl_t hdl = record_register(buf, buf2);
        record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
        record_set_chunk_size(hdl, CHUNK_SIZE);
        record_start(hdl);
        rec_mgr->hdl = hdl;
        g_rec_mgrs[i] = rec_mgr;  
        LOGD(TAG, "start mic rec [%d]\n", i);
    }
    print_period = 0;
}

void mic_rec_stop(void)
{
    for (int i = 0; i < MIC_REC_COUNT; i++) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[i];
        if (rec_mgr == NULL) {
            continue;
        }
        aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);
        record_stop(rec_mgr->hdl);
        aos_event_free(&rec_mgr->wait_evt);
        aos_free(rec_mgr->rbuffer);
        ringbuffer_destroy(&rec_mgr->ringbuffer);    
        record_unregister(rec_mgr->hdl);  
        rec_mgr->hdl = NULL;
        aos_free(rec_mgr);
        g_rec_mgrs[i] = NULL;
        LOGD(TAG, "mic rec stop over.");
    }
}
