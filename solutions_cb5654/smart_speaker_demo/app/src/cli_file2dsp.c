#include <app_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <csi_core.h>
#include <aos/log.h>
#include <vfs.h>
#include <yv_ap.h>
#include "app_main.h"
#include "audio/audio_res.h"
#include "silan_voice_adc.h"

#define TAG "F2C"

#define SD_PATH "/fatfs0/%s"

#define CHUNK_SIZE  (5*32*40)     // 40 ms, 5 channels
#define WRITE_SIZE  (2*32*20)     // 20 ms, 2 channels
#define WRITE_CHUNK  (2*32)     // 1 ms, 2 channels
#define MIN(a, b)   ((a) < (b) ? (a) : (b))

static int tsk_running = 0;

static void rbuf_write2(codec_adrb_t *rbuf, char *data, int data_len)
{
    int tail = rbuf->tail;
    int last_tail = tail;

    for (int i = 0; i < data_len; ++i) {
        rbuf->buffer[tail++] = data[i];
        if (tail == rbuf->length) {
            csi_dcache_clean_invalid_range((uint32_t *)&rbuf->buffer[last_tail], rbuf->length - last_tail);
            tail = 0;
            last_tail = tail;
        }
    }

    if (tail != last_tail)
        csi_dcache_clean_invalid_range((uint32_t *)&rbuf->buffer[last_tail], tail - last_tail);

    rbuf->tail = tail;
    csi_dcache_clean_invalid_range((uint32_t *)&rbuf->tail, 4);
}

static int send_to_dsp(char *data, int data_len)
{
    codec_adrb_t *micbuf = voice_get_mic_rb_sram();
    codec_adrb_t *refbuf = voice_get_ref_rb();
    int chansize;
    char data_mic[WRITE_SIZE], data_ref[WRITE_SIZE];

    if (data_len % 10 != 0) {
        return -1;
    }

    while (data_len > 0) {
        chansize = MIN(WRITE_SIZE / 2, data_len / 5) / 2;   // short type

        for (int i = 0; i < chansize; ++i) {
            memcpy(&data_mic[i * 4], &data[i * 10], 4);
            memcpy(&data_ref[i * 4], &data[i * 10 + 4], 4);
        }

        /* to avoid ref 20 ms delay, write 1 ms alternatively */
        for (int i = 0; i < WRITE_SIZE / WRITE_CHUNK; ++i) {
            rbuf_write2(micbuf, data_mic + WRITE_CHUNK * i, WRITE_CHUNK);   // 2 channel * 2 bytes
            rbuf_write2(refbuf, data_ref + WRITE_CHUNK * i, WRITE_CHUNK);
        }

        aos_msleep(20);

        data_len -= chansize * 2 * 5;
        data += chansize * 2 * 5;
    }

    return 0;
}

static void file2dsp(void *sd_filename)
{
    char filepath[256];
    char data[CHUNK_SIZE];       
    int fd = -1;

    sprintf(filepath, SD_PATH, (char *)sd_filename);

    fd = aos_open(filepath, O_RDONLY);
    if (fd < 0) {
        LOGE(TAG, "open file %s failed", filepath);
        goto END;
    }

	if (strstr(filepath, ".wav")) {
	  LOGE(TAG, "this is wav header of 44 bytes !!");
	  aos_read(fd, data, 44);
	}

    while (1) {
        int rbytes = aos_read(fd, data, CHUNK_SIZE);
        if (rbytes < 0 || rbytes % 10 != 0) {       // 5 channel * 16 /2 bytes
            LOGE(TAG, "file read failed %d", rbytes);
            goto END;
        } else if (rbytes == 0) {
            LOGD(TAG, "reach the end of file");
            break;
        }

        int ret = send_to_dsp(data, rbytes);
        if (ret < 0) {
            LOGW(TAG, "send to dsp error");
        }
        // printf("send %d B", rbytes);
    }

END:
    if (fd >= 0) {
        aos_close(fd);
    }
    tsk_running = 0;
}

/**
 * use this function to source pcm file in sd card to dsp instead of mic samples,
 * this is used to do automatic dialog test, mic input is no longer used after calling this funciton
 * 
 * */
int pcm_file_to_dsp(char *filename)
{
    if (tsk_running) {
        printf("last file still streamming");
        return -1;
    }

    tsk_running = 1;
    
    sram_mic_stop();
    voice_ref_stop();

    aos_msleep(50);
    aos_task_new("f2dsp", file2dsp, filename, 16 * 1024);
    return 0;
}

