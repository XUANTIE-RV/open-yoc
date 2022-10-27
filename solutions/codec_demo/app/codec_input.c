/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     example_codec.c
 * @brief    example for codec
 * @version  V1.0
 * @date     24. Mar 2020
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <drv/codec.h>

#define input_check(ret)                              \
    do {                                              \
        if (ret < 0 && ret != -4) {                   \
            printf("ret:%d, line:%d", ret, __LINE__); \
            while(1);                                 \
        }                                             \
    } while (0)

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

static csi_codec_t              g_codec;
static csi_codec_input_t        g_input_hdl;
static csi_dma_ch_t             dma_ch_input_handle;
static csi_codec_input_config_t g_input_config;
static uint32_t                 g_input_size;
static aos_sem_t                g_input_sem;
static csi_ringbuf_t            input_ring_buffer;
static uint8_t                  start_run;
static uint8_t                  g_input_buf[INPUT_BUFFER_SIZE];
static uint8_t                  g_read_buffer[READ_BUFFER_SIZE];

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

static void input_task(void *priv)
{
    int ret;
    start_run = 1;
    aos_sem_new(&g_input_sem, 0);
    ret = csi_codec_init(&g_codec, 0);

    if (ret != CSI_OK) {
        LOG("csi_codec_init error\n");
        return;
    }

    g_input_hdl.ring_buf          = &input_ring_buffer;
    g_input_hdl.sound_channel_num = INPUT_CHANNELS;
    ret = csi_codec_input_open(&g_codec, &g_input_hdl, 0);
    input_check(ret);
    ret = csi_codec_input_attach_callback(&g_input_hdl, codec_input_event_cb_fun, NULL);
    input_check(ret);

    /* input ch config */
    g_input_config.bit_width         = INPUT_SAMPLE_BITS;
    g_input_config.sample_rate       = INPUT_SAMPLE_RATE;
    g_input_config.buffer            = g_input_buf;
    g_input_config.buffer_size       = INPUT_BUFFER_SIZE;
    g_input_config.period            = INPUT_PERIOD_SIZE;
    g_input_config.mode              = CODEC_INPUT_DIFFERENCE;
    g_input_config.sound_channel_num = INPUT_CHANNELS;
    ret = csi_codec_input_config(&g_input_hdl, &g_input_config);
    input_check(ret);
    ret = csi_codec_input_analog_gain(&g_input_hdl, 0xcf);
    input_check(ret);
    ret = csi_codec_input_digital_gain(&g_input_hdl, 25);
    input_check(ret);
    ret = csi_codec_input_link_dma(&g_input_hdl, &dma_ch_input_handle);
    input_check(ret);
    ret = csi_codec_input_start(&g_input_hdl);
    input_check(ret);

    uint32_t size   = 0;
    uint32_t r_size = 0;
    g_input_size     = 0;
    // printf("input start(%lld)\n", aos_now_ms());

    while (1) {
        input_wait();
        r_size = (g_input_size + INPUT_PERIOD_SIZE) < READ_BUFFER_SIZE ? INPUT_PERIOD_SIZE : (READ_BUFFER_SIZE-g_input_size);
        size = csi_codec_input_read_async(&g_input_hdl, g_read_buffer + g_input_size, r_size);
        if (size != INPUT_PERIOD_SIZE) {
            // printf("input stop, get (%d)ms data (%lld)\n", READ_TIME, aos_now_ms());
            printf("read size err(%u)(%u)\n", size, r_size);
            break;
        }
        g_input_size += r_size;
    }

    aos_sem_free(&g_input_sem);
    csi_codec_input_stop(&g_input_hdl);
    csi_codec_input_link_dma(&g_input_hdl, NULL);
    csi_codec_input_detach_callback(&g_input_hdl);
    csi_codec_uninit(&g_codec);
    start_run = 0;
}

static void input_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (start_run) {
        printf("input task is running\n");
    	return;
    }
    aos_task_new( "input_task", input_task, NULL, 4096);
}

void codec_input_init(void)
{
    static const struct cli_command cmd_info = {
        "input",
        "codec input test",
        input_cmd
    };
    aos_cli_register_command(&cmd_info);
}