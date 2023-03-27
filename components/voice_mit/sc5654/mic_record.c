#include <time.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <aos/ringbuffer.h>
#include <yoc/record.h>

#define MIC_REC_COUNT 1

static const char *TAG = "micrec";

#define CHUNK_SIZE 5 * 3200
#define RINGBUFFER_SIZE (CHUNK_SIZE * 20 + 1)

typedef struct _rec_mgr {
    dev_ringbuf_t ringbuffer;
    char *        rbuffer;
    aos_event_t   wait_evt;
    rec_hdl_t     hdl;
} rec_mgr_t;

static rec_mgr_t *g_rec_mgrs[MIC_REC_COUNT];
static int print_period = 0;

void audio_record_copy_data(int index, uint8_t *data, uint32_t size)
{
    if (index >= MIC_REC_COUNT) {
        return;
    }

    rec_mgr_t *rec_mgr = g_rec_mgrs[index];
    if (rec_mgr && rec_mgr->hdl) {
        if (index == 0) {
            if (print_period % (320 * 1000) == 0) {
                printf(".");
            }
            print_period += size;
        }

        int write_space = ringbuffer_available_write_space(&rec_mgr->ringbuffer);
        if (write_space > CHUNK_SIZE) {
            int ret = ringbuffer_write(&rec_mgr->ringbuffer, data, size);
            if (size != ret) {
                LOGW(TAG, "buffer full, write=%d ret=%d", size, ret);
            }
        } else {
            LOGW(TAG, "buffer full, drop %d", CHUNK_SIZE);
        }

        int read_size = ringbuffer_available_read_space(&rec_mgr->ringbuffer);
        if (read_size > 0) {
            aos_event_set(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR);
        }
    }
}

static void data_ready(void *arg)
{
    unsigned   flags;
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    // LOGD(TAG, "read wait...");
    aos_event_get(&rec_mgr->wait_evt, 0x01, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
}

void audio_record_start(const char *url, const char *save_name)
{
    for (int i = 0; i < MIC_REC_COUNT; i++) {
        rec_mgr_t *rec_mgr = aos_zalloc_check(sizeof(rec_mgr_t));

        rec_mgr->rbuffer = aos_malloc_check(RINGBUFFER_SIZE);
        ringbuffer_create(&rec_mgr->ringbuffer, rec_mgr->rbuffer, RINGBUFFER_SIZE);
        aos_event_new(&rec_mgr->wait_evt, 0);
        char buf[64];
        snprintf(buf, sizeof(buf), "ringbuffer://handle=%u", (uint32_t)(&rec_mgr->ringbuffer));
        char   buf2[128];
        time_t tt = time(NULL);
        if (i == 0) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, tt, "mic", save_name);
        } else if (i == 1) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, tt, "ref", save_name);
        } else if (i == 2) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, tt, "proc", save_name);
        }

        rec_hdl_t hdl = record_register(buf, buf2);
        record_set_data_ready_cb(hdl, data_ready, (void *)rec_mgr);
        record_set_chunk_size(hdl, CHUNK_SIZE);
        record_start(hdl);
        rec_mgr->hdl  = hdl;
        g_rec_mgrs[i] = rec_mgr;
        LOGD(TAG, "start mic rec [%d]\n", i);
    }
    print_period = 0;
}

void audio_record_stop(void)
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