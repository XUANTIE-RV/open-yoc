/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/clk.h>
#include <devices/devicelist.h>

int hal_clk_demo(void)
{
    int ret = -1;

    printf("hal clk demo start\n");

    rvm_clk_drv_register();

    rvm_dev_t *clk_dev = NULL;
    char *filename = "clk";

    clk_dev = rvm_hal_clk_open(filename);

    uint32_t freq_set = 1000000, freq_get[1] = {0};

    ret = rvm_hal_clk_set_freq(clk_dev, RVM_HAL_SYS_CLK, 0, freq_set);
    if (ret != 0) {
        printf("rvm_hal_clk_set_freq error!\n");
        return -1;
    }
  
    ret = rvm_hal_clk_get_freq(clk_dev, RVM_HAL_SYS_CLK, 0, freq_get);
    if (ret != 0) {
        printf("rvm_hal_clk_get_freq error!\n");
        return -1;
    }

    printf("freq of the SYS: %d\n", freq_get[0]);

    printf("hal clk demo successfully!\n");
  
    return 0;
}
