/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>

#if defined(CONFIG_CHIP_D1)
#if defined(CONFIG_COMP_DRV_BT_RTL8723DS)
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include "app_main.h"
#include <board.h>
#include <devices/device.h>
#include <devices/driver.h>
#include <devices/hci.h>
#include <devices/devicelist.h>
#include <devices/rtl8723ds_bt.h>
#include <drv/pin.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int fd_hci_demo;
static uint8_t data_buffer[4096] = {0};
static void devfs_hci_demo_event(rvm_hal_hci_event_t event, uint32_t size, void *priv)
{ 
    rvm_hci_dev_msg_t msg_hci;
    msg_hci.data = data_buffer;
    msg_hci.size = sizeof(data_buffer);
    int ret = ioctl(fd_hci_demo, HCI_IOC_RECV, &msg_hci);
    if (ret < 0) {
        printf("HCI_IOC_RECV fail !\n");
        close(fd_hci_demo);
        return;
    }
    if (data_buffer[0] != 192) {
        printf("recv err\n");
    }
    return;
}

int devfs_hci_demo(void)
{
    printf("devfs_hci_demo start\n");
    
    int ret = -1;

    board_bt_init();

    char *hcidev = "/dev/hci0";

    fd_hci_demo = open(hcidev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd_hci_demo);
    if (fd_hci_demo < 0) {
        printf("open %s failed. fd:%d\n", hcidev, fd_hci_demo);
        return -1;
    }

    rvm_hci_dev_msg_t msg_hci;
    msg_hci.event = devfs_hci_demo_event;
    msg_hci.priv = NULL;
    ret = ioctl(fd_hci_demo, HCI_IOC_SET_EVENT, &msg_hci);
    if (ret < 0) {
        printf("HCI_IOC_SET_EVENT fail !\n");
        goto failure;
    }

    uint8_t data[8] = {192, 0, 47, 0, 208, 1, 126, 192};
    msg_hci.data = data;
    msg_hci.size = sizeof(data);
    ret = ioctl(fd_hci_demo, HCI_IOC_SEND, &msg_hci);
    if (ret < 0) {
        printf("HCI_IOC_SEND fail !\n");
        goto failure;
    }

    aos_msleep(3000);

    printf("devfs hci demo successfully !\n");

    close(fd_hci_demo);

    return 0;
failure:
    close(fd_hci_demo);
    return -1;
}

#endif

#else
int devfs_hci_demo(void)
{
    printf("Not support!\n");
    return 0;
}
#endif /*CONFIG_COMP_DRV_BT_RTL8723DS*/

#endif /*CONFIG_CHIP_D1*/