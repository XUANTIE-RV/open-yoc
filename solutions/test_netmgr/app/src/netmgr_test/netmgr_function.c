/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>
#include "../yunit.h"

#define TAG "TEST_NETMGR_FUNC"

#define NET_RESET_DELAY_TIME            15 /*second*/

netmgr_hdl_t netmgr_hdl;
char wifi_ssid[30];
char wifi_psk[30];
int wifi_dhcp_en = 1;
char *wifi_ipaddr;
char *wifi_netmask;
char *wifi_gw;

/**
 * 判断网络是否已经初始化
 */
static int test_wifi_net_inited()
{
    return (netmgr_hdl != NULL);
}

/**
 * 网络初始化
 */
static void wifi_net_init()
{
    LOGI(TAG, "Start wifi init");
    LOGI(TAG, "add utask netmgr");
    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    netmgr_hdl_t hdl = netmgr_dev_wifi_init();
    if (hdl) {
        LOGI(TAG, "netmgr wifi dev init pass");
    } else {
        LOGE(TAG, "netmgr wifi dev init fail");
    }
    LOGI(TAG, "netmgr service init");
    netmgr_service_init(task);
    netmgr_hdl = hdl;
}

/**
 * 网络驱动初始化
 */
static void wifi_dev_init()
{
    int wifi_en;
    int ret;
    ret = aos_kv_getint("wifi_en", &wifi_en);
    if (ret < 0) {
        ///<若没有设置KV,默认使能
        wifi_en = 1;
        ret = aos_kv_setint("wifi_en", 1);
        LOGI(TAG, "did not get kv wifi_en. set wifi_en to 1, ret=%d", ret);
    }

    if (wifi_en == 0) {
       ret = aos_kv_setint("wifi_en", 1);
       wifi_en = 1;
       LOGI(TAG, "wifi_en==0. set wifi_en to 1, ret=%d", ret);
    }

    test_wifi_register();
    wifi_net_init();
}

/**
 * ssid以及psk配置
 * @param ssid ap名称
 * @param psk ap密码
 */
static void test_netmgr_wifi_config(char *ssid, char *psk)
{
    int ret;
    memset(wifi_ssid,0, 30);
    memset(wifi_psk,0, 30);
    strncpy(wifi_ssid,ssid,strlen(ssid));
    strncpy(wifi_psk,psk,strlen(psk));
    LOGI(TAG, "config wifi, ssid=%s, passwd=%s", ssid, psk);
    ret = netmgr_config_wifi(netmgr_hdl, ssid, strlen(ssid), psk, strlen(psk));
    LOGI(TAG, "netmgr wifi config, result=%d", ret);
}

/**
 * 配置ipconfig
 * @param dhcp_en dhcp enable 
 * @param ipaddr  ip地址
 * @param netmask 子网掩码地址
 * @param gw  网关地址
 */
static void test_netmgr_ip_config(int dhcp_en, char *ipaddr, char *netmask, char *gw)
{
    int ret;
    wifi_dhcp_en = dhcp_en;
    wifi_ipaddr = ipaddr;
    wifi_netmask = netmask;
    wifi_gw = gw;

    if (wifi_dhcp_en == 1) {
        LOGI(TAG, "config ip interface mode: dhcp client");
    } else {
        LOGI(TAG, "config ip interface mode: static ip, ipaddr=%s, netmask=%s, gateway=%s", wifi_ipaddr, wifi_netmask, wifi_gw);
    }

    ret = netmgr_ipconfig(netmgr_hdl, wifi_dhcp_en, wifi_ipaddr, wifi_netmask, wifi_gw);
    LOGI(TAG, "netmgr ip config, result=%d", ret);
}

/**
 * 网络连接
 */
static void test_wifi_connect()
{
    int ret;
    LOGI(TAG, "connect to wifi, ssid=%s, passwd=%s", wifi_ssid, wifi_psk);
    if (wifi_dhcp_en == 1) {
        LOGI(TAG, "ip interface: dhcp client");
    } else {
        LOGI(TAG, "ip interface: static ip, ipaddr=%s, netmask=%s, gateway=%s", wifi_ipaddr, wifi_netmask, wifi_gw);
    }
    ret = netmgr_start(netmgr_hdl);
    LOGI(TAG, "netmgr start, result=%d", ret);

    sleep(2);
    ret = netmgr_is_gotip(netmgr_hdl);
    LOGI(TAG, "netmgr is gotip? %d", ret);

}


/**
 * 网络断开连接
 */
static void test_wifi_disconnect()
{
    int ret;
    LOGI(TAG, "Disconnect wifi network");
    ret = netmgr_stop(netmgr_hdl);
    LOGI(TAG, "netmgr stop, result=%d", ret);
}

/**
 * 网络reset
 */
static void test_wifi_reset()
{
    LOGI(TAG, "reset wifi network");
    int ret;
    netmgr_hdl_t hdl = netmgr_get_handle("wifi");
    if (hdl) {
        LOGI(TAG, "get netmgr hdl pass, hdl=%d", hdl);
    } else {
        LOGE(TAG, "get netmgr hdl fail");
        return;
    }

    ret = netmgr_reset(hdl, NET_RESET_DELAY_TIME);
    LOGI(TAG, "netmgr wifi reset, result=%d", ret);

    if (wifi_dhcp_en == 1) {
        LOGI(TAG, "wifi ssid=%s, psk=%s, ip interface: dhcp client", wifi_ssid, wifi_psk);
    } else {
        LOGI(TAG, "wifi ssid=%s, psk=%s, ip interface: static ip, ipaddr=%s, netmask=%s, gateway=%s", wifi_ssid, wifi_psk, wifi_ipaddr, wifi_netmask, wifi_gw);
    }
    sleep(3);

    ret = netmgr_is_gotip(netmgr_hdl);
    LOGI(TAG, "netmgr is gotip? %d", ret);
}

/**
* 网络去初始化
*/
static void test_wifi_deinit()
{
    LOGI(TAG, "Deinit wifi network");

    netmgr_stop(netmgr_hdl);
    netmgr_dev_wifi_deinit(netmgr_hdl);
    netmgr_service_deinit();

    netmgr_hdl = NULL;
}

/**
* cli 入口
*/
static void test_wifi_connnection(char *wbuf, int wbuf_len, int argc, char **argv)
{
    char *this_ssid;
    char *this_psk;
    int dhcp_en;
    char *this_ipaddr = NULL;
    char *this_netmask = NULL;
    char *this_gw = NULL;
    if (argc == 1) {
	    printf("usage: wifi [init|config|ipconfig|connect|disconnect|reset|deinit|info] [ssid] [psk] [dhcp|ip mask gw]\n");
	    return;
    }
    if (strcmp(argv[1], "init") == 0) {
        ///<判断是否已经初始化
        if (test_wifi_net_inited()!=0) {
            printf("wifi network already init.\n");
	        return;
        }
        ///<若没有初始化则运行
        wifi_dev_init();
    } else if(strcmp(argv[1], "config") == 0) {
        ///<配置ssid以及psk
        if (argc < 4) {
            printf("usage: wifi config <ssid> <psk>\n");
            return;
        }
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        this_ssid = argv[2];
        this_psk = argv[3];
        test_netmgr_wifi_config(this_ssid, this_psk);
    } else if(strcmp(argv[1], "ipconfig") == 0) {
        if (argc < 3) {
            printf("usage: wifi ipconfig [<dhcp>| <ip> <netmask> <gw>\n");
            return;
        }
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        ///<判断参数是否为dhcp
        if(strcmp(argv[2], "dhcp") == 0) {
            dhcp_en = 1;
        } else {
            ///<若不是dhcp，配置ip、netmask、gw
            if (argc < 5) {
                printf("usage: wifi ipconfig <ip> <netmask> <gw>\n");
                return;
            }
            dhcp_en = 0;
            this_ipaddr = argv[2];
            this_netmask = argv[3];
            this_gw = argv[4];
        }
        LOGI(TAG, "ipconfig, dhcp_en=%d, ipaddr=%s, netmask=%s, gw=%s", dhcp_en, this_ipaddr, this_netmask, this_gw);
        test_netmgr_ip_config(dhcp_en, this_ipaddr, this_netmask, this_gw);
    } else if(strcmp(argv[1], "connect") == 0) {
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        test_wifi_connect();
    } else if(strcmp(argv[1], "disconnect") == 0) {
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        test_wifi_disconnect();
    } else if(strcmp(argv[1], "reset") == 0) {
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        test_wifi_reset();
    } else if(strcmp(argv[1], "deinit") == 0) {
        test_wifi_deinit();
    } else if(strcmp(argv[1], "info") == 0) {
        if (test_wifi_net_inited()==0) {
            printf("wifi network did not init yet, please use <wifi init> to init it first.\n");
	        return;
        }
        netmgr_get_info(netmgr_hdl);
    } else {
        printf("usage: wifi [init|config|ipconfig|connect|disconnect|reset|deinit|info] [ssid] [psk] [dhcp|ip mask gw]\n");
	    return;
    }
}

/**
 * 命令注册函数
 */
void cli_reg_cmd_wifi(void)
{
    static struct cli_command cmd_info = {
        "wifi",
        "wifi [init|config|ipconfig|connect|disconnect|reset|deinit|info] [ssid] [psk] [dhcp|ip mask gw]",
        test_wifi_connnection
    };

    aos_cli_register_command(&cmd_info);
}
