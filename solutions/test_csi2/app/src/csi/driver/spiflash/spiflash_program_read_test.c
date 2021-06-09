/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spiflash_test.h"

static csi_gpio_t g_gpio;
uint32_t tst_gpio_pin_mask;

static void spi_cs_callback(csi_gpio_pin_state_t value)
{
    csi_gpio_write(&g_gpio, 1 << tst_gpio_pin_mask, value);
}


int test_spiflash_program_read(char *args)
{
    csi_error_t ret;
    csi_spiflash_t spiflash_handle;
    test_spiflash_args_t spiflash_args;
    csi_spiflash_info_t info;
    uint8_t *tx_data = NULL;
    uint8_t *rx_data = NULL;

    uint32_t gpio_idx;

    sscanf(args, "%d,%d,%d,%d,%d", &gpio_idx, &tst_gpio_pin_mask, &spiflash_args.idx, &spiflash_args.offset, &spiflash_args.size);
    TEST_CASE_TIPS("gpio_idx: %d, gpio_pin_mask: %d", gpio_idx, tst_gpio_pin_mask);
    TEST_CASE_TIPS("idx: %d, offset: %x, size %d", spiflash_args.idx, spiflash_args.offset, spiflash_args.size);

    csi_gpio_init(&g_gpio, gpio_idx);
    csi_gpio_dir(&g_gpio, 1 << tst_gpio_pin_mask, GPIO_DIRECTION_OUTPUT);

    ret = csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, spi_cs_callback);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash init error, actual return value is %d", ret);

    ret = csi_spiflash_get_flash_info(&spiflash_handle, &info);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash spi get flash info error");
    TEST_CASE_TIPS("flash info:flash name %s", info.flash_name);
    TEST_CASE_TIPS("flash info:flash id 0x%x", info.flash_id);
    TEST_CASE_TIPS("flash info:flash size 0x%x", info.flash_size);
    TEST_CASE_TIPS("flash info:flash pagesize 0x%x", info.page_size);
    TEST_CASE_TIPS("flash info:flash sectorsize 0x%x", info.sector_size);
    TEST_CASE_TIPS("flash info:flash xipaddr 0x%x", info.xip_addr);

    ret = csi_spiflash_erase(&spiflash_handle, spiflash_args.offset, info.sector_size);
    TEST_CASE_ASSERT(ret == CSI_OK, "csi spiflash spi erase error");

    tx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
    TEST_CASE_ASSERT(tx_data != NULL, "tx_data malloc error");
    memset(tx_data, 0xa, sizeof(uint8_t) * spiflash_args.size);
    ret = csi_spiflash_program(&spiflash_handle, spiflash_args.offset, tx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash spi program error");

    rx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
    TEST_CASE_ASSERT(rx_data != NULL, "rx_data malloc error");
    memset(rx_data, 0, sizeof(uint8_t) * spiflash_args.size);
    ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret >= 0, "csi spiflash spi read error");
    ret = memcmp(tx_data, rx_data, spiflash_args.size);
    TEST_CASE_ASSERT(ret == 0, "the results of spiflash reading and programing are not equal");


    free(tx_data);
    free(rx_data);

    csi_spiflash_spi_uninit(&spiflash_handle);

    return 0;
}
