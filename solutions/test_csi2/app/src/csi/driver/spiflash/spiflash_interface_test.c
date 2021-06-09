/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spiflash_test.h"

int test_spiflash_interface(char *args)
{
    csi_error_t ret;

    ret = csi_spiflash_spi_init(NULL, 0, NULL);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_spi_init interface test error");

    ret = csi_spiflash_qspi_init(NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_qspi_init interface test error");

    ret = csi_spiflash_get_flash_info(NULL, NULL);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_get_flash_info interface test error");

    ret = csi_spiflash_read(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_read interface test error");

    ret = csi_spiflash_program(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_program interface test error");

    ret = csi_spiflash_erase(NULL, 0, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_erase interface test error");

    ret = csi_spiflash_read_reg(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_read_reg interface test error");

    ret = csi_spiflash_write_reg(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_spiflash_write_reg interface test error");

    csi_spiflash_qspi_uninit(NULL);
    csi_spiflash_spi_uninit(NULL);

    return 0;
}
