/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/flash.h>
#include <devices/devicelist.h>

#define FLASH_SECTOR 4096
#define FLASH_TEST_PAR 0

#if defined(CONFIG_CHIP_CH2601)
#define FLASH_PROGRAM_ADDR 0x1810B000
#elif defined(CONFIG_CHIP_D1)
#define FLASH_PROGRAM_ADDR 0xB42000
#else
#define FLASH_PROGRAM_ADDR 0xffffffff
#endif

int hal_flash_demo(void)
{
    char buf_write[32] = {0x1, 0x5, 0x1, 0x5};
    char buf_read[32] = {0};
    int ret = -1;

    printf("hal flash demo start\n");

    rvm_spiflash_drv_register(0);

    rvm_dev_t *flash_dev = NULL;
    char *filename = "flash0";

    flash_dev = rvm_hal_flash_open(filename);

    rvm_hal_flash_dev_info_t partition_info = {0};

    ret = rvm_hal_flash_get_info(flash_dev, &partition_info);
    if (ret != 0) {
        printf("rvm_hal_flash_get_info fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_erase(flash_dev, FLASH_PROGRAM_ADDR, 1);
    if (ret != 0) {
        printf("rvm_hal_flash_erase fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_program(flash_dev, FLASH_PROGRAM_ADDR, buf_write, sizeof(buf_write));
    if (ret != 0) {
        printf("rvm_hal_flash_program fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_read(flash_dev, FLASH_PROGRAM_ADDR, buf_read, sizeof(buf_write));
    if (ret != 0) {
        printf("rvm_hal_flash_read fail !\n");
        return -1;
    }

    if (0 == memcmp(buf_write, buf_read, sizeof(buf_write))) {
        printf("flash write and read successfully !\n");
    } else {
        printf("flash operate failed !\n");
        return -1;
    }

    printf("flash demo successfully !\n");
  
    return 0;
}
