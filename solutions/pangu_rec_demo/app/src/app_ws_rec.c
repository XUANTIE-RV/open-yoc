#include "app_config.h"
#include <sys/time.h>
#include <unistd.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/ringbuffer.h>
#include "audio/test2.wav.c"

#include <yoc/record.h>

static const char *TAG = "appwsrec";

#define PIECE_SIZE (2560 * 1)
#define RINGBUFFER_SIZE (PIECE_SIZE * 50 + 1)

typedef struct _rec_mgr {
    dev_ringbuf_t ringbuffer;
    char *rbuffer;
    int piece_size;
    int src_data_r_pos;
    int quit;
    aos_event_t wait_evt;
    rec_hdl_t hdl;
    int over_print;
} rec_mgr_t;

#define REC_MGR_COUT 4
rec_mgr_t *g_rec_mgr[REC_MGR_COUT];
static int g_rec_start;

static void write_data_task(void *arg)
{
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    while(!rec_mgr->quit) {
        if (rec_mgr->src_data_r_pos + rec_mgr->piece_size > sizeof(local_audio_test2)) {
            int remain_len = sizeof(local_audio_test2) - rec_mgr->src_data_r_pos;
            if (remain_len > 0) {
                ringbuffer_write(&rec_mgr->ringbuffer, (uint8_t *)&local_audio_test2[rec_mgr->src_data_r_pos], remain_len);
                rec_mgr->src_data_r_pos += remain_len;
                LOGD(TAG, "last piece");               
            } else {
                if (rec_mgr->over_print == 0) {
                    LOGD(TAG, "write finish [%d]", rec_mgr->src_data_r_pos);
                    rec_mgr->over_print = 1; 
                }
            }
        } else {
            ringbuffer_write(&rec_mgr->ringbuffer, (uint8_t *)&local_audio_test2[rec_mgr->src_data_r_pos], rec_mgr->piece_size);
            rec_mgr->src_data_r_pos += rec_mgr->piece_size;
            // LOGD(TAG, "write 2 ringbuffer, %d, 0x%x", rec_mgr->piece_size, arg);
            
        }
        if (ringbuffer_available_read_space(&rec_mgr->ringbuffer) > 0)
            aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);
        aos_msleep(80);
    }
}

static void data_ready(void *arg)
{
    unsigned flags;
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    // LOGD(TAG, "read wait...");
    aos_event_get(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
}

void app_ws_rec_start(const char *url, const char *save_name)
{
    if (g_rec_start) {
        LOGW(TAG, "rec start yet, please stop before starting rec!");
        return;
    }
    g_rec_start = 1;
    for (int i = 0; i < REC_MGR_COUT; i++) {
        rec_mgr_t *rec_mgr = aos_zalloc_check(sizeof(rec_mgr_t));

        rec_mgr->src_data_r_pos = 0;
        rec_mgr->piece_size = PIECE_SIZE;
        rec_mgr->rbuffer = aos_malloc_check(RINGBUFFER_SIZE);
        ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, RINGBUFFER_SIZE);
        rec_mgr->quit = 0;
        aos_task_new("ws-rec-w", write_data_task, (void *)rec_mgr, 4096);
        aos_event_new(&rec_mgr->wait_evt, 0);
        char buf[64];
        snprintf(buf, sizeof(buf), "ringbuffer://handle=%u", &rec_mgr->ringbuffer);
        char buf2[128];
        time_t tt = time(NULL);
        if (i == 0) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "mic1", save_name);
        } else if (i == 1) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "mic2", save_name);
        } else if (i == 2) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "ref", save_name);
        } else if (i == 3) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s" ,url, tt, "proc", save_name);
        }
        rec_hdl_t hdl = record_register(buf, buf2);
        record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
        record_set_chunk_size(hdl, rec_mgr->piece_size);
        record_start(hdl);
        rec_mgr->hdl = hdl;
        g_rec_mgr[i] = rec_mgr; 
        LOGD(TAG, "start mic rec [%d]\n", i);  
    }
}

void app_ws_rec_stop()
{
    for (int i = 0; i < REC_MGR_COUT; i++) {
        rec_mgr_t *rec_mgr = g_rec_mgr[i];
        if (rec_mgr == NULL) {
            continue;
        }
        rec_mgr->quit = 1;
        aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);
        record_stop(rec_mgr->hdl);
        aos_event_free(&rec_mgr->wait_evt);
        aos_free(rec_mgr->rbuffer);
        ringbuffer_destroy(&rec_mgr->ringbuffer);    
        record_unregister(rec_mgr->hdl);  
        rec_mgr->hdl = NULL;
        aos_free(rec_mgr);
        g_rec_mgr[i] = NULL;
        LOGD(TAG, "mic rec stop over."); 
    }
    g_rec_start = 0;
}