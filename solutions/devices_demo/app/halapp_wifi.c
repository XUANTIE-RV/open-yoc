/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <aos/kv.h>
#include <yoc/netmgr_service.h>

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include "app_main.h"
#include "soc.h"
#include "board.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/wifi.h>
#include <devices/netdrv.h>
#include <devices/devicelist.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef CONFIG_WIFI_XR829
#include <devices/xr829.h>
#elif defined(CONFIG_WIFI_DRIVER_RTL8723)
#include <devices/rtl8723ds.h>
#elif defined(CONFIG_WIFI_DRIVER_BL606P)
#include <devices/bl606p_wifi.h>
#elif defined(CONFIG_WIFI_DRIVER_W800)
#include <devices/w800.h>
#else
#error "No WiFi driver found."
#endif

static int fd_wifi;
static aos_sem_t g_wifi_sem;

static void event_callback(uint32_t event_id, const void *data, void *context)
{
    printf("event_callback event_id: 0x%x\n", event_id);
    switch (event_id)
    {
    case EVENT_NET_GOT_IP:
        printf("EVENT_NET_GOT_IP...\n");
        if (aos_sem_is_valid(&g_wifi_sem))
            aos_sem_signal(&g_wifi_sem);
        break;
    case EVENT_WIFI_LINK_DOWN:
        break;
    case EVENT_WIFI_LINK_UP:
        printf("EVENT_WIFI_LINK_UP...\n");
        if (aos_sem_is_valid(&g_wifi_sem))
            aos_sem_signal(&g_wifi_sem);
        break;

    default:
        break;
    }
}

static int g_wifi_demo_run;
int devfs_wifi_demo(void)
{
    if (g_wifi_demo_run) {
        printf("Please input reboot if you want to run wifi demo again.\r\n");
        return 0;
    }
    g_wifi_demo_run = 1;
    /* init wifi driver and network */
#ifdef CONFIG_WIFI_XR829
    wifi_xr829_register(NULL);
#elif defined(CONFIG_WIFI_DRIVER_RTL8723)
    board_wifi_init();
#elif defined(CONFIG_WIFI_DRIVER_BL606P)
    board_wifi_init();
#elif defined(CONFIG_WIFI_DRIVER_W800)
    w800_wifi_param_t w800_param;
    /* init wifi driver and network */
    w800_param.reset_pin      = PA21;
    w800_param.baud           = 1*1000000;
    w800_param.cs_pin         = PA15;
    w800_param.wakeup_pin     = PA25;
    w800_param.int_pin        = PA22;
    w800_param.channel_id     = 0;
    w800_param.buffer_size    = 4*1024;
    wifi_w800_register(NULL, &w800_param);
#else
#error "No WiFi driver found."
#endif

    int ret = -1;
    char *wifidev = "/dev/wifi0";

    fd_wifi = open(wifidev, O_RDWR);
    if (fd_wifi < 0) {
        printf("open %s failed. fd:%d\n", wifidev, fd_wifi);
        return -1;
    }
    ret = aos_sem_new(&g_wifi_sem, 0);
    if (ret < 0) {
        printf("aos_sem_new fail !\n");
        goto failure;
    }

    rvm_netdrv_dev_msg_t msg;
    msg.event = EVENT_NET_GOT_IP;
    msg.cb = event_callback;
    msg.param = NULL;
    ret = ioctl(fd_wifi, NETDRV_IOC_SUBSCRIBE, &msg);
    if (ret < 0) {
        printf("NETDRV_IOC_SUBSCRIBE fail !\n");
        goto failure;
    }
    msg.event = EVENT_WIFI_LINK_UP;
    ret = ioctl(fd_wifi, NETDRV_IOC_SUBSCRIBE, &msg);
    if (ret < 0) {
        printf("NETDRV_IOC_SUBSCRIBE fail !\n");
        goto failure;
    }

    ret = ioctl(fd_wifi, WIFI_IOC_INIT);
    if (ret < 0) {
        printf("WIFI_IOC_INIT fail !\n");
        goto failure;
    }

    ret = ioctl(fd_wifi, WIFI_IOC_RESET);
    if (ret < 0) {
        printf("WIFI_IOC_RESET fail !\n");
        goto failure;
    }

    char ssid[64];
    char passwd[64];
    ret = aos_kv_getstring(KV_WIFI_SSID, ssid, sizeof(ssid));
    if (ret < 0) {
        printf("get wifi_ssid fail !\n");
        goto failure;
    }
    ret = aos_kv_getstring(KV_WIFI_PSK, passwd, sizeof(passwd));
    if (ret < 0) {
        printf("get wifi_psk fail !\n");
        goto failure;
    }
    printf("wifi_ssid: %s, wifi_psk: %s\n", ssid, passwd);
    rvm_hal_wifi_config_t wifi_config;
    wifi_config.mode = WIFI_MODE_STA;
    strcpy(wifi_config.ssid, ssid);
    strcpy(wifi_config.password, passwd);

    ret = ioctl(fd_wifi, WIFI_IOC_START, &wifi_config);
    if (ret < 0) {
        printf("WIFI_IOC_START fail !\n");
        goto failure;
    }
#if !defined(CONFIG_WIFI_DRIVER_W800)
    printf("waiting 20000ms for wifi linkup..\n");
    ret = aos_sem_wait(&g_wifi_sem, 20000);
    if (ret < 0) {
        printf("waiting for wifi linkup timeout!!! please try again.\n");
        goto failure;
    }
    ret = ioctl(fd_wifi, NETDRV_IOC_START_DHCP);
    if (ret < 0) {
        printf("NETDRV_IOC_START_DHCP fail !\n");
        goto failure;
    }
#endif
    printf("waiting 30000ms for wifi got ip..\n");
    ret = aos_sem_wait(&g_wifi_sem, 30000);
    if (ret < 0) {
        printf("waiting for wifi got ip timeout!!! please try again.\n");
        goto failure;
    }

    ret = ioctl(fd_wifi, NETDRV_IOC_GET_IPADDR, &msg);
    if (ret < 0) {
        printf("NETDRV_IOC_GET_IPADDR fail !\n");
        goto failure;
    }

    printf("NETDRV_IOC_GET_IPADDR ipaddr:%s\n", ipaddr_ntoa(&msg.ipaddr));

    aos_sem_free(&g_wifi_sem);

    extern void cli_reg_cmd_ping(void);
    cli_reg_cmd_ping();

    return 0;
failure:
    ret = ioctl(fd_wifi, WIFI_IOC_DEINIT, NULL);
    if (ret < 0) {
        printf("WIFI_IOC_DEINIT fail !\n");
    }
    aos_sem_free(&g_wifi_sem);
    close(fd_wifi);
    printf("I am going to reboot!!!\n");
    aos_msleep(3000);
    aos_reboot();
    return -1;
}

#endif