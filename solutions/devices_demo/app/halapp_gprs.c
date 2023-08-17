/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if defined(CONFIG_BOARD_CV181XH)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include <aos/cli.h>

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/gprs.h>
#include <devices/netdrv.h>
#include <devices/devicelist.h>
#include <drv_usbh_class.h>

#include "usbh_core.h"
#include "usbh_rndis.h"
#include "usbh_serial.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char* help_gprs_str = "gprs test\n"
    "gprs ifconfig ap <ssid> <password>\n"
    "gprs ping www.baidu.com\n"
    ".......\n\n>";


static int fd_gprs_demo;

// ifconfig
static int demo_ifconfig(int argc, char **argv)
{
    // if (argc != 5) {
    //     printf("cmd err\n");
    //     return -1;
    // }

    // rvm_hal_wifi_config_t wifi_config;
    // wifi_config.mode = WIFI_MODE_STA;
    // strcpy(wifi_config.ssid, argv[3]);
    // strcpy(wifi_config.password, argv[4]);

    // int ret = ioctl(fd_gprs_demo, WIFI_IOC_START, &wifi_config);
    // if (ret < 0) {
    //     printf("WIFI_IOC_START fail !\n");
    //     close(fd_wifi_demo);
    //     return -1;
    // }

    return 0;
}    

// ping
static int demo_ping(int type, char *remote_ip)
{   
    rvm_netdrv_dev_msg_t msg;
    msg.type = type;
    msg.remote_ip = remote_ip;
    int ret = ioctl(fd_gprs_demo, NETDRV_IOC_PING, &msg);
    if (ret < 0) {
        printf("NETDRV_IOC_PING fail !\n");
        close(fd_gprs_demo);
        return -1;
    }

    return 0;
}

static int demo_ping_func(int argc, char **argv)
{
    int ping_type = 0;
    char *host_ip;

    if (argc < 3) {
        printf("cmd err\n");
        return -1;
    }

    if (strcmp(argv[2], "-6") == 0) {
        ping_type = 6;
        host_ip = argv[3];
    } else {
        ping_type = 4;
        host_ip = argv[2];
    }

    demo_ping(ping_type, host_ip);

    return 0;
}

static void gprs_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        goto help;
    }

    int ret = 0;

    if (strcmp(argv[1], "ping") == 0) {
        ret = demo_ping_func(argc, argv);
        if (ret == -1) {
            goto help;
        }
    } else if (strcmp(argv[1], "ifconfig") == 0) {
        ret = demo_ifconfig(argc, argv);
        if (ret == -1) {
            goto help;
        } 
    } else {
        goto help;
    }

    return;

help:   
    printf("Argument failed\n%s", help_gprs_str);
}

void gprs_test(void)
{
    static const struct cli_command gprs_cmd_info = {
        "gprs",
        "gprs test",
        gprs_cmd
    };
    aos_cli_register_command(&gprs_cmd_info);
}


int devfs_gprs_demo(void)
{
    printf("build time: %s, %s\r\n", __DATE__, __TIME__);
    USBH_REGISTER_RNDIS_CLASS();
    USBH_REGISTER_USB_SERIAL_CLASS();
    usbh_initialize();

    drv_ec200a_serial_register(0);

    drv_ec200a_rndis_register();

    int ret = -1;
    char *gprsdev = "/dev/gprs0";

    fd_gprs_demo = open(gprsdev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd_gprs_demo);
    if (fd_gprs_demo < 0) {
        printf("open %s failed. fd:%d\n", gprsdev, fd_gprs_demo);
        return -1;
    }

    ret = ioctl(fd_gprs_demo, GPRS_IOC_MODULE_INIT_CHECK);
    if (ret < 0) {
        printf("GPRS_IOC_MODULE_INIT_CHECK fail !\n");
        goto failure;
    }

    rvm_hal_gprs_mode_t mode = CONNECT_MODE_GPRS;
    ret = ioctl(fd_gprs_demo, GPRS_IOC_SET_MODE, &mode);
    if (ret < 0) {
        printf("GPRS_IOC_SET_MODE fail !\n");
        goto failure;
    }

    ret = ioctl(fd_gprs_demo, GPRS_IOC_CONNECT_TO_GPRS);
    if (ret < 0) {
        printf("GPRS_IOC_CONNECT_TO_GPRS fail !\n");
        goto failure;
    }
   
    aos_msleep(3000);

    // ip_addr_t ipaddr;
    // rvm_netdrv_dev_msg_t msg;
    // msg.ipaddr = &ipaddr;

    // ret = ioctl(fd_gprs_demo, NETDRV_IOC_GET_IPADDR, &msg);
    // if (ret < 0) {
    //     printf("NETDRV_IOC_GET_IPADDR fail !\n");
    //     goto failure;
    // }

    // printf("rvm_hal_net_get_ipaddr ipaddr:%s\n", ipaddr_ntoa(&ipaddr));

    gprs_test();

    return 0;
failure:
    close(fd_gprs_demo);
    return -1;
}

#endif
#endif/*CONFIG_BOARD_CV181XH*/
