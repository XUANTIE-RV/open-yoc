/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <spi_test.h>

uint8_t spi_transfer_data[SPI_TRANSFER_LEN + 1];

int spi_test_config(csi_spi_t *spi_handler, test_spi_args_t *test_args, csi_dma_t *spi_dma_handler, csi_dma_ch_t *spi_dma_ch, csi_dma_ch_t *spi_dma_ch_1)
{
    csi_error_t ret;
    ret = csi_spi_mode(spi_handler, test_args->spi_mode);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "spi config mode error, spi mode: %d", test_args->spi_mode);

    ret = csi_spi_cp_format(spi_handler, test_args->cp_format);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "spi config cp_format error, cp_format: %d", test_args->cp_format);

    ret = csi_spi_frame_len(spi_handler, test_args->frame_len);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "spi config frame length error, frame_len: %d", test_args->frame_len);
    uint32_t baud_set = 0;

    if (test_args->spi_mode == SPI_MASTER) {
        baud_set = csi_spi_baud(spi_handler, test_args->baud);
        TEST_CASE_ASSERT_QUIT(baud_set > 0, "spi config baud error, baud: %d", test_args->baud);
    }

    csi_spi_select_slave(spi_handler, 0);

    if (test_args->link_dma == 1) {
        ret = csi_spi_link_dma(spi_handler, spi_dma_ch, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "link DMA error");
    }

    if (test_args->link_dma == 2) {
        ret = csi_spi_link_dma(spi_handler, NULL, spi_dma_ch);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "link DMA error");
    }

    if (test_args->link_dma == 3) {
        ret = csi_spi_link_dma(spi_handler, spi_dma_ch, spi_dma_ch_1);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "link DMA error");
    }

    return 0;
}

test_func_map_t spi_test_funcs_map[] = {
    {"SPI_INTERFACE", test_spi_interface},
    {"SPI_SYNC_SEND", test_spi_sync_send},
    {"SPI_SYNC_RECEIVE", test_spi_sync_receive},
    {"SPI_ASYNC_SEND", test_spi_async_send},
    {"SPI_ASYNC_RECEIVE", test_spi_async_receive},
    {"SPI_DMA_SEND", test_spi_dma_send},
    {"SPI_DMA_RECEIVE", test_spi_dma_receive},
    {"SPI_SYNC_SENDRECEIVE", test_spi_sync_send_receive},
    {"SPI_ASYNC_SENDRECEIVE", test_spi_async_send_receive},
    {"SPI_DMA_SENDRECEIVE", test_spi_dma_send_receive},
};

int test_spi_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(spi_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, spi_test_funcs_map[i].test_func_name)) {
            (*(spi_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("SPI module don't support this command.");
    return -1;
}

