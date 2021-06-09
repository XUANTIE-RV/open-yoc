/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spiflash_test.h"


int test_spiflash_qspi_programread(char *args)
{
    csi_error_t ret;
    csi_spiflash_t qspiflash_handle;
    test_spiflash_args_t spiflash_args;
    csi_spiflash_info_t info;
    uint8_t *tx_data = NULL;
    uint8_t *rx_data = NULL;

    sscanf(args, "%d,%d,%d", &spiflash_args.idx, &spiflash_args.offset, &spiflash_args.size);
    TEST_CASE_TIPS("idx: %d, offset: %x, size %d", spiflash_args.idx, spiflash_args.offset, spiflash_args.size);

    ret = csi_spiflash_qspi_init(&qspiflash_handle, spiflash_args.idx);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash spi init error");

    ret = csi_spiflash_get_flash_info(&qspiflash_handle, &info);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash spi get flash info error");
    TEST_CASE_TIPS("flash name %s", info.flash_name);
    TEST_CASE_TIPS("flash id 0x%x", info.flash_id);
    TEST_CASE_TIPS("flash size 0x%x", info.flash_size);
    TEST_CASE_TIPS("flash pagesize 0x%x", info.page_size);
    TEST_CASE_TIPS("flash sectorsize 0x%x", info.sector_size);
    TEST_CASE_TIPS("flash xipaddr 0x%x", info.xip_addr);

    ret = csi_spiflash_erase(&qspiflash_handle, spiflash_args.offset, spiflash_args.size);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash spi erase error");

    tx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
    TEST_CASE_ASSERT(tx_data != NULL, "tx_data malloc error");
    memset(tx_data, 0xa, sizeof(uint8_t) * spiflash_args.size);
    ret = csi_spiflash_program(&qspiflash_handle, spiflash_args.offset, tx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash spi program error");

    rx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
    memset(rx_data, 0, sizeof(uint8_t) * spiflash_args.size);
    ret = csi_spiflash_read(&qspiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret >= 0, "spiflash read error");
    ret = memcmp(tx_data, rx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret == 0, "the results of spiflash reading and programing are not equal");

    free(tx_data);
    free(rx_data);

    return 0;
}
