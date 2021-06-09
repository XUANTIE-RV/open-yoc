/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _SPI_TEST_H_
#define _SPI_TEST_H_
#include <stdint.h>
#include <drv/spi.h>
#include <drv/dma.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include "test_common.h"
#define SPI_TRANSFER_LEN 1024

typedef struct{
    uint32_t idx;                    
    csi_spi_mode_t spi_mode;         
    csi_spi_frame_len_t frame_len;   
    csi_spi_cp_format_t cp_format;   
    uint32_t baud;                   
    uint32_t link_dma;               //0: not link dma, 1: link tx_dma, 2: link rx_dma
    uint64_t trans_size;
} test_spi_args_t;


extern int spi_test_config(csi_spi_t *spi_handler, test_spi_args_t *test_args, csi_dma_t *spi_dma_handler, csi_dma_ch_t *spi_dma_ch, csi_dma_ch_t *spi_dma_ch_1);
extern int test_spi_interface(char *args);
extern int test_spi_sync_send(char *args);
extern int test_spi_sync_receive(char *args);
extern int test_spi_sync_send_receive(char *args);
extern int test_spi_async_send(char *args);
extern int test_spi_async_receive(char *args);
extern int test_spi_async_send_receive(char *args);
extern int test_spi_dma_send(char *args);
extern int test_spi_dma_receive(char *args);
extern int test_spi_dma_send_receive(char *args);

#endif
