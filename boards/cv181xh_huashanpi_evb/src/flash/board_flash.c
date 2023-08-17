/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#include <stdlib.h>
#include <stdio.h>
#include <devices/blockdev.h>
#include <devices/devicelist.h>

void board_flash_init(void)
{
    rvm_spiflash_drv_register(0);
#if CONFIG_PARTITION_SUPPORT_EMMC
	rvm_hal_mmc_config_t mmc_config;
	memset(&mmc_config, 0, sizeof(rvm_hal_mmc_config_t));
	mmc_config.sdif = CONFIG_EMMC_SDIF;
	mmc_config.hostVoltageWindowVCC = kMMC_VoltageWindows270to360;
	mmc_config.default_busWidth = kMMC_DataBusWidth4bit;
	mmc_config.use_default_busWidth = 1;
	rvm_mmc_drv_register(0, &mmc_config);
#endif
    rvm_hal_sd_config_t sd_config;
    sd_config.sdif = CONFIG_SD_SDIF;
    rvm_sd_drv_register(0, &sd_config);
}
