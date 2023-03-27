/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/blockdev.h>
#include <devices/devicelist.h>

#ifdef CONFIG_COMP_SDMMC
static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0 && i > 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}

static uint8_t buffer[8192];
int hal_mmc_demo(void)
{
    printf("hal_mmc_demo start\n");
    
    rvm_dev_t *emmcdev;
    rvm_hal_blockdev_info_t mmc_info;
    //////////////////////////////////////////////////////////
    rvm_hal_mmc_config_t config;
    config.sdif = CONFIG_EMMC_SDIF;
    config.hostVoltageWindowVCC = kMMC_VoltageWindows270to360;
    config.default_busWidth = kMMC_DataBusWidth4bit;
    config.use_default_busWidth = 1;
    rvm_mmc_drv_register(0, &config);
    emmcdev = rvm_hal_blockdev_open("mmc");
    int ret;
    ret = rvm_hal_blockdev_read_blks(emmcdev, buffer, 0, 10);
    if (ret) {
        printf("read blks error, ret:%d\n", ret);
        return -1;
    }
    printf("read ok\n");
    dump_data(buffer, 32);
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = 0xCC;
    }
    ret = rvm_hal_blockdev_write_blks(emmcdev, buffer, 0, 10);
    if (ret) {
        printf("write blks error\n");
        return -1;
    }
    ret = rvm_hal_blockdev_read_blks(emmcdev, buffer, 0, 10);
    if (ret) {
        printf("read blks error\n");
        return -1;
    }
    printf("read ok\n");
    dump_data(buffer, 32);
    ret = rvm_hal_blockdev_erase_blks(emmcdev, 1024, 1024);
    if (ret) {
        printf("erase blks error\n");
        return -1;
    }
    printf("erase ok\n");
    ret = rvm_hal_blockdev_read_blks(emmcdev, buffer, 0, 10);
    if (ret) {
        printf("read blks error\n");
        return -1;
    }
    printf("read ok\n");
    dump_data(buffer, 32);

    ret = rvm_hal_blockdev_erase_blks(emmcdev, 0, 1024);
    if (ret) {
        printf("erase blks error\n");
        return -1;
    }
    printf("erase ok\n");
    ret = rvm_hal_blockdev_read_blks(emmcdev, buffer, 0, 10);
    if (ret) {
        printf("read blks error\n");
        return -1;
    }
    printf("read ok\n");
    dump_data(buffer, 32);

    ret = rvm_hal_blockdev_get_info(emmcdev, &mmc_info);
    if (ret) {
        printf("get info error\n");
        return -1;
    }
    printf("mmc_info.block_size:%d\n", mmc_info.block_size);
    printf("mmc_info.erase_blks:%d\n", mmc_info.erase_blks);
    printf("mmc_info.boot_area_blks:%d\n", mmc_info.boot_area_blks);
    printf("mmc_info.user_area_blks:%d\n", mmc_info.user_area_blks);

    /* close emmcdev */
    rvm_hal_blockdev_close(emmcdev);

    printf("mmc demo successfully !\n");

    return 0;
}
#else
int hal_mmc_demo(void)
{
    printf("Not support.\n");
    return 0;
}
#endif