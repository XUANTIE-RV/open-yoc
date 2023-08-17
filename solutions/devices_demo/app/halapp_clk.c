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
    char *filename = "clk0";

    clk_dev = rvm_hal_clk_open(filename);

    uint32_t freq_set = 1000000, freq_get[1] = {0};

    ret = rvm_hal_clk_get_freq(clk_dev, RVM_HAL_SYS_CLK, 0, freq_get);
    if (ret != 0) {
        printf("rvm_hal_clk_get_freq error!\n");
        return -1;
    }
    printf("get freq of the SYS: %d\n", freq_get[0]);

    freq_set = freq_get[0];
    ret = rvm_hal_clk_set_freq(clk_dev, RVM_HAL_SYS_CLK, 0, freq_set);
    if (ret != 0) {
        printf("rvm_hal_clk_set_freq error!\n");
        return -1;
    }
    printf("set freq of the SYS: %d\n", freq_set);

    printf("hal clk demo successfully!\n");
  
    return 0;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_clk_demo(void)
{
    printf("devfs_clk_demo start\n");

    int ret = -1;

    rvm_clk_drv_register();

    char *clkdev = "/dev/clk0";

    int fd = open(clkdev, O_RDWR);
    printf("open clkdev fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", clkdev, fd);
        return -1;
    }

    rvm_clk_dev_msg_t msg_clk;
    msg_clk.clk_id = RVM_HAL_SYS_CLK;
    msg_clk.idx = 0;

    ret = ioctl(fd, CLK_IOC_GET_FREQ, &msg_clk);
    if (ret < 0) {
        printf("CLK_IOC_GET_FREQ fail !\n");
        goto failure;
    }
    printf("get freq of the SYS: %d\n", msg_clk.freq);

    uint32_t get_freq = msg_clk.freq;
    msg_clk.freq = get_freq;
    ret = ioctl(fd, CLK_IOC_SET_FREQ, &msg_clk);
    if (ret < 0) {
        printf("CLK_IOC_SET_FREQ fail !\n");
        goto failure;
    }
    printf("set freq of the SYS: %d\n", msg_clk.freq);

    char *dev_uart = "uart0";
    ret = ioctl(fd, CLK_IOC_DISABLE, dev_uart);
    if (ret < 0) {
        printf("CLK_IOC_DISABLE fail !\n");
        goto failure;
    }

    /*
        d1的clk_map定义为假，该功能实际未执行
    */
    // printf("note: this log should not appear\n");

    ret = ioctl(fd, CLK_IOC_ENABLE, dev_uart);
    if (ret < 0) {
        printf("CLK_IOC_ENABLE fail !\n");
        goto failure;
    }

    printf("devfs clk demo successfully!\n");

    close(fd);
    return 0;
failure:
    close(fd);
    return -1;
}

#endif