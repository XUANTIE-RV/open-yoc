
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include "drv/codec.h"

#include  "pcm_data.h"

#define loop_check(ret)                               \
    do {                                              \
        if (ret < 0 && ret != -4) {                   \
            printf("ret:%d, line:%d", ret, __LINE__); \
            while(1);                                 \
        }                                             \
    } while (0)

// output code
#if (CONFIG_CHIP_CH2601)
#define HW_OUTBUF_SIZE      (200)
#else
#define HW_OUTBUF_SIZE      (17600)
#endif

#define OUTPUT_SAMPLE_RATE  (16000)
#define OUTPUT_SAMPLE_BITS  (16)
#define OUTPUT_CHANNELS     (2)

#define OUTPUT_PERIOD_EVENT (0x01)
#define OUTPUT_EMPTY_EVENT  (0x02)

static csi_codec_t               g_codec;
static csi_codec_output_t        g_output_hdl;
static csi_codec_output_config_t g_output_config;
static csi_dma_ch_t              g_dma_hdl;
static csi_ringbuf_t             g_output_ring_buffer;
static uint8_t                   g_hw_outbuf[HW_OUTBUF_SIZE];
static aos_event_t               g_output_evt;
static uint8_t                  *g_audio_outbuf;
static uint32_t                  g_audio_outsize; 
static uint32_t                  g_snd_size;

static void codec_output_event_cb(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (!aos_event_is_valid(&g_output_evt)) {
        printf("maybe csi_codec_output_stop err\r\n");
        while(1);
    }
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        aos_event_set(&g_output_evt, OUTPUT_PERIOD_EVENT, AOS_EVENT_OR);
    }

    if (event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        aos_event_set(&g_output_evt, OUTPUT_EMPTY_EVENT, AOS_EVENT_OR);
    }
}
static void codec_output_init(void)
{
    g_audio_outbuf  = (uint8_t*)pcm_16K_16bit_mono;
    g_audio_outsize = (uint32_t)pcm_16K_16bit_mono_len;

    aos_event_new(&g_output_evt, 0);
    int ret = csi_codec_init(&g_codec, 0);
    loop_check(ret);

    g_output_hdl.ring_buf          = &g_output_ring_buffer;
    ret = csi_codec_output_open(&g_codec, &g_output_hdl, 0);
    loop_check(ret);
    ret = csi_codec_output_attach_callback(&g_output_hdl, codec_output_event_cb, NULL);
    loop_check(ret);

    g_output_config.bit_width         = OUTPUT_SAMPLE_BITS;
    g_output_config.sample_rate       = OUTPUT_SAMPLE_RATE;
    g_output_config.buffer            = g_hw_outbuf;
    g_output_config.buffer_size       = HW_OUTBUF_SIZE;
    g_output_config.period            = 100;
    g_output_config.mode              = CODEC_OUTPUT_SINGLE_ENDED;
    g_output_config.sound_channel_num = OUTPUT_CHANNELS;

    ret = csi_codec_output_config(&g_output_hdl, &g_output_config);
    loop_check(ret);
    ret = csi_codec_output_analog_gain(&g_output_hdl, 0xaf);   ///< 设置声音大小0xaf
    loop_check(ret);
    ret = csi_codec_output_digital_gain(&g_output_hdl, 0x4);
    loop_check(ret);
    ret = csi_codec_output_link_dma(&g_output_hdl, &g_dma_hdl);
    loop_check(ret);
    ret = csi_codec_output_start(&g_output_hdl);
    loop_check(ret);
}

static void codec_output_deinit(void)
{
    printf("audio tx stop\n");
    csi_codec_output_stop(&g_output_hdl);
    aos_event_free(&g_output_evt);
    csi_codec_output_link_dma(&g_output_hdl, NULL);
    csi_codec_output_detach_callback(&g_output_hdl);
    csi_codec_output_close(&g_output_hdl);
}
// input code
#define INPUT_SAMPLE_RATE  (8000)
#define INPUT_SAMPLE_BITS  (16)
#define INPUT_CHANNELS     (2)

// ch2601的资源小，录音数据不支持太大
#if (CONFIG_CHIP_CH2601)
#define INPUT_BUFFER_SIZE 1024
#define INPUT_PERIOD_SIZE 512
#define READ_BUFFER_SIZE 61440
#else
#define READ_TIME (2000)  //MS
#define INPUT_TIME (1200) //MS
#define PERIOD_TIME (20)  //MS

#define FRAME_SIZE (INPUT_SAMPLE_RATE * INPUT_SAMPLE_BITS / 8 / 1000 * INPUT_CHANNELS)
#define INPUT_BUFFER_SIZE FRAME_SIZE * INPUT_TIME
#define INPUT_PERIOD_SIZE FRAME_SIZE * PERIOD_TIME
#define READ_BUFFER_SIZE FRAME_SIZE * READ_TIME
#endif

typedef struct read_buffer_t {
    uint8_t   *buffer;
    uint32_t  buffer_size;
} read_buffer_t;

static csi_codec_input_t        g_input_hdl;
static csi_dma_ch_t             dma_ch_input_handle;
static csi_codec_input_config_t g_input_config;
static uint32_t                 g_recv_size;
static aos_sem_t                g_input_sem;
static csi_ringbuf_t            input_ring_buffer;
static uint8_t                  g_input_buf[INPUT_BUFFER_SIZE];
static read_buffer_t            g_read_buf;

static void codec_input_event_cb_fun(csi_codec_input_t *i2s, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        aos_sem_signal(&g_input_sem);
    }
}

static void input_wait(void) {
#if (CONFIG_CHIP_BL606P_E907)
    int ret = csi_codec_input_buffer_avail(&g_input_hdl);
#else
    int ret = csi_codec_input_buffer_remain(&g_input_hdl);
#endif

    if (ret < INPUT_PERIOD_SIZE) {
        aos_sem_wait(&g_input_sem, AOS_WAIT_FOREVER);
    }
}

static void codec_input_init(void)
{
    aos_sem_new(&g_input_sem, 0);

    g_input_hdl.ring_buf          = &input_ring_buffer;
    g_input_hdl.sound_channel_num = INPUT_CHANNELS;
    int ret = csi_codec_input_open(&g_codec, &g_input_hdl, 0);
    loop_check(ret);
    ret = csi_codec_input_attach_callback(&g_input_hdl, codec_input_event_cb_fun, NULL);
    loop_check(ret);

    /* input ch config */
    g_input_config.bit_width         = INPUT_SAMPLE_BITS;
    g_input_config.sample_rate       = INPUT_SAMPLE_RATE;
    g_input_config.buffer            = g_input_buf;
    g_input_config.buffer_size       = INPUT_BUFFER_SIZE;
    g_input_config.period            = INPUT_PERIOD_SIZE;
    g_input_config.mode              = CODEC_INPUT_DIFFERENCE;
    g_input_config.sound_channel_num = INPUT_CHANNELS;
    ret = csi_codec_input_config(&g_input_hdl, &g_input_config);
    loop_check(ret);
    ret = csi_codec_input_analog_gain(&g_input_hdl, 0xbf);
    loop_check(ret);
    ret = csi_codec_input_digital_gain(&g_input_hdl, 25);
    loop_check(ret);
    ret = csi_codec_input_link_dma(&g_input_hdl, &dma_ch_input_handle);
    loop_check(ret);
    ret = csi_codec_input_start(&g_input_hdl);
    loop_check(ret);
}

static void codec_input_deinit(void)
{
    // printf("input stop, get (%d)ms data (%lld)\n", READ_TIME, aos_now_ms());

    aos_sem_free(&g_input_sem);
    csi_codec_input_stop(&g_input_hdl);
    csi_codec_input_link_dma(&g_input_hdl, NULL);
    csi_codec_input_detach_callback(&g_input_hdl);
    csi_codec_uninit(&g_codec);
}

static void codec_output_task(void *priv)
{
    int ret;
    int write_len = 4096;
    unsigned int flag;
    long long t = aos_now_ms() + 5000;
    codec_output_init();
    while (1) {
        ret = csi_codec_output_write_async(&g_output_hdl, (void *)(g_audio_outbuf + g_snd_size), write_len);
        if (ret != write_len) {
            g_snd_size += ret;
            // printf("!!!(%d)\r\n", g_snd_size);
            aos_event_get(&g_output_evt, OUTPUT_EMPTY_EVENT|OUTPUT_PERIOD_EVENT , AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        } else {
            g_snd_size += write_len;
        }
        
        if (g_snd_size + write_len >= g_audio_outsize) {
            printf("------------(%d)(%d)\r\n", g_snd_size, g_audio_outsize);
            g_snd_size = 0;
            // break;
        }

        if (aos_now_ms() > t) {
            break;
        }
    }
    codec_output_deinit();
}

static void codec_input_task(void *priv)
{
    int r_size, size;
    codec_input_init();

    while (1) {
        input_wait();
        r_size = (g_recv_size + INPUT_PERIOD_SIZE) < READ_BUFFER_SIZE ? INPUT_PERIOD_SIZE : (READ_BUFFER_SIZE-g_recv_size);
        size = csi_codec_input_read_async(&g_input_hdl, g_read_buf.buffer + g_recv_size, r_size);
        if (size != INPUT_PERIOD_SIZE || g_recv_size >= g_read_buf.buffer_size) {
            if (g_recv_size >= g_read_buf.buffer_size) {
                size = 0;
            }
            printf("read size err(%d)(%d)(%d)\n", size, r_size, g_recv_size);
            break;
        }
        g_recv_size += r_size;
        if (g_recv_size >= g_read_buf.buffer_size) {
            printf("read buffer is enough \n");
        }
    }
    
    codec_input_deinit();
}

static void loop_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    aos_task_new("output", codec_output_task, NULL, 4096);
    aos_task_new("input", codec_input_task, NULL, 4096);
}

void codec_loop_init(void)
{
    g_read_buf.buffer_size = READ_BUFFER_SIZE;
    g_read_buf.buffer = aos_malloc(g_read_buf.buffer_size);
    if (g_read_buf.buffer  == NULL) {
        LOG("codec_loop malloc read buffer failed!");
        return;
    }
    static const struct cli_command cmd_info = {
        "loop",
        "codec loop test",
        loop_cmd
    };
    aos_cli_register_command(&cmd_info);
}
