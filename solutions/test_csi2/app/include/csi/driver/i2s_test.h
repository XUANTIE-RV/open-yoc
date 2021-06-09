/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _I2S_TEST_H_
#define _I2S_TEST_H_
#include <stdint.h>
#include <drv/i2s.h>
#include <drv/dma.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include "test_common.h"

#define I2S_BUF_SIZE      1024    //I2S data buffer
#define I2S_TRANSFER_SIZE 1024   //transfer size for data transfer testing
#define I2S_TRANSFER_PAT  0x5A   //data pattern for transfer test
#define I2S_TX     0
#define I2S_RX     1
#define I2S_DUPLEX 2

typedef struct{
    uint32_t idx;
    csi_i2s_sclk_freq_t sclk_freq;
    csi_i2s_mclk_freq_t mclk_freq;
    csi_i2s_sample_rate_t rate;
    uint32_t period;
    csi_i2s_sample_width_t width;
    csi_i2s_ws_left_polarity_t polarity;
    csi_i2s_protocol_t protocol;
    csi_i2s_mode_t mode;
    uint32_t sound_ch;
    csi_i2s_mclk_freq_t mclk_nfs;     
} test_i2s_args_t;

extern int test_i2s_interface(char *args);
extern int test_i2s_async_send(char *args);
extern int test_i2s_async_receive(char *args);
extern int test_i2s_sync_send(char *args);
extern int test_i2s_sync_receive(char *args);
extern int test_i2s_dma_send(char *args);
extern int test_i2s_dma_receive(char *args);
extern int test_i2s_sync_status(char *args);
#endif