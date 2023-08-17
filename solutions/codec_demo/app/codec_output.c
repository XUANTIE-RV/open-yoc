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
#include "drv/codec.h"

#include  "pcm_data.h"

#define output_check(ret)                             \
    do {                                              \
        if (ret < 0 && ret != -4) {                   \
            printf("ret:%d, line:%d", ret, __LINE__); \
            while(1);                                 \
        }                                             \
    } while (0)

// output
#if (CONFIG_CHIP_CH2601)
#define HW_OUTBUF_SIZE      (200)
#else
#define HW_OUTBUF_SIZE      (17600)
#endif

#define OUTPUT_SAMPLE_RATE  (8000)
#define OUTPUT_SAMPLE_BITS  (16)
#define OUTPUT_CHANNELS     (1)

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

static int codec_output_test(uint32_t timeout)
{
    // output
    uint32_t write_len;

    g_snd_size = 0;
    g_audio_outbuf  = (uint8_t*)pcm_8K_16bits_1ch;
    g_audio_outsize = (uint32_t)pcm_8K_16bits_1ch_len;

    aos_event_new(&g_output_evt, 0);
    int ret = csi_codec_init(&g_codec, 0);
    output_check(ret);

    g_output_hdl.ring_buf          = &g_output_ring_buffer;
    ret = csi_codec_output_open(&g_codec, &g_output_hdl, 0);
    output_check(ret);
    ret = csi_codec_output_attach_callback(&g_output_hdl, codec_output_event_cb, NULL);
    output_check(ret);

    g_output_config.bit_width         = OUTPUT_SAMPLE_BITS;
    g_output_config.sample_rate       = OUTPUT_SAMPLE_RATE;
    g_output_config.buffer            = g_hw_outbuf;
    g_output_config.buffer_size       = HW_OUTBUF_SIZE;
    g_output_config.period            = 100;
    g_output_config.mode              = CODEC_OUTPUT_SINGLE_ENDED;
    g_output_config.sound_channel_num = OUTPUT_CHANNELS;

    ret = csi_codec_output_config(&g_output_hdl, &g_output_config);
    output_check(ret);
    ret = csi_codec_output_analog_gain(&g_output_hdl, 0xaf);   ///< 设置声音大小0xaf
    output_check(ret);
    ret = csi_codec_output_digital_gain(&g_output_hdl, 0x4);
    output_check(ret);
    ret = csi_codec_output_link_dma(&g_output_hdl, &g_dma_hdl);
    output_check(ret);
    ret = csi_codec_output_start(&g_output_hdl);
    output_check(ret);

    // output
    write_len = 4096;
    unsigned int flag = 0;
    printf("codec output start\r\n");
    while (1) {
        ret = csi_codec_output_write_async(&g_output_hdl, (void *)(g_audio_outbuf + g_snd_size), write_len);
        if (ret != write_len) {
            g_snd_size += ret;
            aos_event_get(&g_output_evt, OUTPUT_EMPTY_EVENT|OUTPUT_PERIOD_EVENT , AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        } else {
            g_snd_size += write_len;
        }
        
        if (g_snd_size >= g_audio_outsize) {
            aos_event_get(&g_output_evt, OUTPUT_EMPTY_EVENT, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        }
        
        if (flag & OUTPUT_EMPTY_EVENT) {
            printf("outbuf is empty\n");
            flag = 0;
            break;
        }
    }

    printf("audio tx stop\n");
    csi_codec_output_stop(&g_output_hdl);
    aos_event_free(&g_output_evt);
    csi_codec_output_link_dma(&g_output_hdl, NULL);
    csi_codec_output_detach_callback(&g_output_hdl);
    csi_codec_output_close(&g_output_hdl);
    g_output_hdl.codec = NULL;

    return 0;
}

static void codec_output_task(void *arg)
{
    codec_output_test(10000);
}

static void help(void)
{
    printf("output play \n"
           "output pause/resume \n"
           "output mute/unmute \n"
           "output gain [db] \n");
}

static void output_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
       help();  
    }

    int ret = 0;
    if (strcmp(argv[1], "play") == 0) {
        aos_task_new("output", codec_output_task, NULL, 4096);
    } else if (strcmp(argv[1], "pause") == 0) {
        if (!g_output_hdl.codec) {
            printf("Please input \"output play\" first!\n");
            return;
        }
        ret = csi_codec_output_pause(&g_output_hdl);    
        printf("ret: %d\n", ret);
    } else if (strcmp(argv[1], "resume") == 0) {
        if (!g_output_hdl.codec) {
            printf("Please input \"output play\" first!\n");
            return;
        }
        ret = csi_codec_output_resume(&g_output_hdl);    
        printf("ret: %d\n", ret);
    } else if (strcmp(argv[1], "mute") == 0) {
        if (!g_output_hdl.codec) {
            printf("Please input \"output play\" first!\n");
            return;
        }
        ret = csi_codec_output_mute(&g_output_hdl, 1); 
        printf("ret: %d\n", ret);
    } else if (strcmp(argv[1], "unmute") == 0) {
        if (!g_output_hdl.codec) {
            printf("Please input \"output play\" first!\n");
            return;
        }
        ret = csi_codec_output_mute(&g_output_hdl, 0); 
        printf("ret: %d\n", ret);
    } else if (strcmp(argv[1], "gain") == 0) {
        if (!g_output_hdl.codec) {
            printf("Please input \"output play\" first!\n");
            return;
        }
        printf("set gain:%d\n", atoi(argv[2]));

        ret = csi_codec_output_digital_gain(&g_output_hdl, atoi(argv[2]));
        ret = csi_codec_output_analog_gain(&g_output_hdl, atoi(argv[2]));
        printf("ret: %d\n", ret);
    }
}

void codec_output_init(void)
{
    static const struct cli_command cmd_info = {
        "output",
        "codec output test",
        output_cmd
    };
    aos_cli_register_command(&cmd_info);
}

