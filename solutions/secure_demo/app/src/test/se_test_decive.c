/*
 * Copyright (C) 2019-2023 Alibaba Group Holding Limited
 */
#include <string.h>
#include "se_device.h"
#include "yunit.h"
#include "HSCK2_APP_and_sign.h"
#include <devices/spi.h>

static void se_test_device_demo(void)
{
    int ret = 0;
    uint8_t data[2] = {0x00}; 
    uint32_t size = sizeof(rawData);

    printf("test se_update_firmware interface\n");
	/* Start SE update firmware */
	ret = se_start_update_firmware();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_start_update_firmware is %d", ret);

	/* Update update firmware */
    ret = se_update_firmware((const void *)rawData, size, 0x00);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_firmware_upgrade_demo is %d", ret);

	/* Stop SE update firmware */
    printf("test se_get_firmware_version interface\n");
    ret = se_stop_update_firmware();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_firmware_upgrade_demo is %d", ret);
    ret = se_get_firmware_version(NULL, (uint8_t *)data);
    printf("AppVersion:0x%x0%x\n", data[0], data[1]);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_get_firmware_version is %d", ret);
}
int se_drv_init(void)
{
    se_dev_t *dev = NULL;
    rvm_hal_spi_config_t spi_config;

    int ret;

    rvm_hal_spi_default_config_get(&spi_config);
	spi_config.mode = RVM_HAL_SPI_MODE_MASTER;
	spi_config.freq = 1 * 1000 * 1000; //10 * 1000000; // 10M
	spi_config.format = RVM_HAL_SPI_FORMAT_CPOL0_CPHA1;

	ret = se_init(dev, 0, SE_IF_TYPE_SPI, &spi_config);
    CHECK_RET_WITH_RET(ret == 0, ret);

    printf("se_drv_init finish\n");

    return 0;
}


void se_test_device_by_cmd(void)
{
    /***************DEVICE START****************/
    
    se_test_device_demo();

    /***************DEVICE END****************/
}

void se_drv_init_by_cmd(char *outbuf, int len, int argc, char **argv)
{
    se_drv_init();
}

