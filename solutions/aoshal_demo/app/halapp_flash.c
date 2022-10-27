/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "aos/hal/flash.h"

#define FLASH_SECTOR 4096
#define FLASH_TEST_PAR 0

const hal_logic_partition_t hal_partitions[1] = {
#if (CONFIG_CHIP_CH2601)
    [FLASH_TEST_PAR] =
    {
        .partition_owner      = HAL_FLASH_SPI,
        .partition_description  = "aos_hal_test",
        .partition_start_addr  = 0x1810B000,
        .partition_length     = 0x2000,
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#elif (CONFIG_CHIP_BL606P_E907)
    [FLASH_TEST_PAR] =
    {
        .partition_owner      = HAL_FLASH_SPI,
        .partition_description  = "aos_hal_test",
        .partition_start_addr  = 0x39C000,
        .partition_length     = 0x2000,
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#elif (CONFIG_CHIP_D1)
    [FLASH_TEST_PAR] =
    {
        .partition_owner      = HAL_FLASH_SPI,
        .partition_description  = "aos_hal_test",
        .partition_start_addr  = 0xB42000,
        .partition_length     = 0x2000,
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
};

int hal_flash_demo(void)
{
    uint32_t off_read = 0, off_write = 0;
    char buf_write[32] = {0x1, 0x5, 0x1, 0x5};
    char buf_read[32] = {0};
    int ret = -1;

    printf("flash demo start\n");

    hal_logic_partition_t partition_info = {0};
    ret = hal_flash_info_get(FLASH_TEST_PAR, &partition_info);
    if (ret != 0) {
        printf("hal_flash_info_get fail !");
        return -1;
    }

    printf("the partition_start_addr: 0x%x\n", partition_info.partition_start_addr);

    ret = hal_flash_erase(FLASH_TEST_PAR, off_write, FLASH_SECTOR);
    if (ret != 0) {
        printf("hal_flash_erase fail !");
        return -1;
    }

    ret = hal_flash_write(FLASH_TEST_PAR, &off_write, buf_write, sizeof(buf_write));
    if (ret != 0) {
        printf("hal_flash_write fail !");
        return -1;
    }

    ret = hal_flash_read(FLASH_TEST_PAR, &off_read, buf_read,  sizeof(buf_write));
    if (ret != 0) {
        printf("hal_flash_read fail !");
        return -1;
    }

    if (0 == memcmp(buf_write, buf_read, sizeof(buf_write))) {
        printf("flash write and read successfully !\n");
    } else {
        printf("flash operate failed !\n");
        return -1;
    }

    printf("flash demo successfully !");
  
    return 0;
}
