/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>
#include "../yunit.h"

#define TAG "TEST_NETMGR"

netmgr_hdl_t netmgr_hdl;

typedef struct {
    netmgr_hdl_t hdl;
    ip_setting_t config;
} param_ip_setting_t;

/**
 * 回调函数，获取ip后回调
 */
static int test_netmgr_evt_ip_got(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    netmgr_dev_t *node = netmgr_find_dev(&netmgr->dev_list, NULL);
    aos_dev_t *dev = node->dev;

    hal_net_get_ipaddr(dev, &node->ipaddr, &node->netmask, &node->gw);
    LOGI(TAG, "IP: %s", ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(&node->ipaddr)));

    netmgr_set_gotip(NULL, 1);
    event_publish(EVENT_NETMGR_GOT_IP, &node->ipaddr);

    return 0;
}

/**
 * 回调函数，dhcp回调
 */
static int test_netmgr_check_dhcp(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    netmgr_dev_t *node = netmgr_find_dev(&netmgr->dev_list, NULL);
    aos_dev_t *dev = node->dev;

    if (!netmgr_is_gotip(NULL)) {
        LOGE(TAG, "dhcp failed");
        hal_net_stop_dhcp(dev);
        event_publish(EVENT_NETMGR_NET_DISCON, (void *)NET_DISCON_REASON_DHCP_ERROR);
    }
    return 0;
}

/**
 * 回调函数，ipconfig回调
 */
static int test_netmgr_srv_ipconfig(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    param_ip_setting_t *param = rpc_get_point(rpc);
    netmgr_dev_t *node = (netmgr_dev_t *)param->hdl;
    int ret = -EBADFD;

    if (node != NULL) {
        ip_setting_t *ip = &param->config;
        node->dhcp_en = ip->dhcp_en;
#ifdef CONFIG_KV_SMART
        if ((ip->ipaddr != NULL) && (ip->netmask != NULL) && (ip->gw != NULL)) {
            aos_kv_setint(KV_DHCP_EN, node->dhcp_en);
            aos_kv_set(KV_IP_ADDR, ip->ipaddr, strlen(ip->ipaddr), 1);
            aos_kv_set(KV_IP_ADDR, ip->netmask, strlen(ip->netmask), 1);
            aos_kv_set(KV_IP_ADDR, ip->gw, strlen(ip->gw), 1);

            ret = 0;
        } else {
            ret = -EBADFD;
        }

#endif
    }

    rpc_put_buffer(rpc, &ret, 4);
    return ret;
}

/**
 * 回调函数，reset回调
 */
static int test_netmgr_srv_reset(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    int ret = -1;
    netmgr_dev_t *node = (netmgr_dev_t *)rpc_get_point(rpc);

    if (node == NULL) {
        return -EBADFD;
    }

    if (node->reset) {
        if (!node->is_gotip) {
            ret = node->reset(node);
        }
    }

    return ret;
}

/**
 * 回调函数，netmgr start回调
 */
static int test_netmgr_srv_start(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    int ret = -1;
    netmgr_dev_t *node = (netmgr_dev_t *)rpc_get_point(rpc);

    if (node == NULL) {
        return -EBADFD;
    }

    LOGI(TAG, "start %s", node->name);

    netmgr_subscribe(EVENT_NETWORK_RESTART);
    netmgr_subscribe(EVENT_NET_GOT_IP);
    netmgr_subscribe(API_NET_DHCP_CHECK);

    if (node->provision) {
        ret = node->provision(node);
    }

    return ret;
}

/**
 * 回调函数，netmgr stop回调
 */
static int test_netmgr_srv_stop(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    netmgr_dev_t *node = *(netmgr_dev_t **)rpc_get_point(rpc);
    int ret = -1;

    if (node && node->unprovision) {
        ret = node->unprovision(node);
    }

    return ret;
}

static int test_netmgr_srv_info(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    netmgr_dev_t *node = *(netmgr_dev_t **)rpc_get_point(rpc);
    int ret = 0;

    if (node == NULL) {
        return -EBADFD;
    }
    if (node->info) {
        node->info(node);
    }       

    return ret;
}

static int test_netmgr_service(void *context, rpc_t *rpc)
{
    int ret = 0;
    struct netmgr_uservice *netmgr = (struct netmgr_uservice *)context;

    switch (rpc->cmd_id) {

        case API_NET_GET_HDL: {
            char *name = rpc_get_point(rpc);
            netmgr_dev_t *node;

            node = netmgr_find_dev(&netmgr->dev_list, name);

            if (node == NULL) {
                node = (netmgr_dev_t *)aos_zalloc(sizeof(netmgr_dev_t));

                if (node) {
                    node->dev = device_open(name);

                    if (node->dev) {
                        strcpy(node->name, name);
                        slist_add_tail((slist_t *)node, &netmgr->dev_list);
                    } else {
                        aos_free(node);
                        node = NULL;
                    }
                }
            }

            rpc_put_reset(rpc);
            rpc_put_point(rpc, node);
            rpc_reply(rpc);
            return 0;
        }

        default: {
            netmgr_srv_t *node;
            slist_for_each_entry(&netmgr->srv_list, node, netmgr_srv_t, next) {
                if (node->cmd_id == rpc->cmd_id) {
                    ret = node->func(netmgr, rpc);
                    break;
                }
            }
        }

        break;
    }

    rpc_put_reset(rpc);

    /*
     * When an asynchronous API call,
     * rpc->data is empty and cannot have a return value.
     */
    if (rpc->data) {
        rpc_put_int(rpc, ret);
    }

    rpc_reply(rpc);

    return 0;
}

static void test_wifi_dev_init()
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
}

/*
 *服务注册函数，遍历所有类型服务
 */
static void test_netmgr_service_init(utask_t *task)
{
    int ret;
    netmgr_subscribe(API_NET_IPCONFIG);
    netmgr_subscribe(API_NET_START);
    netmgr_subscribe(API_NET_STOP);
    netmgr_subscribe(API_NET_INFO);
    netmgr_subscribe(API_NET_RESET);
    netmgr_subscribe(EVENT_NET_GOT_IP);
    netmgr_subscribe(API_NET_DHCP_CHECK);

    ret = netmgr_reg_srv_func(API_NET_IPCONFIG, test_netmgr_srv_ipconfig);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_IPCONFIG, func=netmgr_srv_ipconfig, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_001");

    ret = netmgr_reg_srv_func(API_NET_START, test_netmgr_srv_start);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_START, func=netmgr_srv_start, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_002");

    ret = netmgr_reg_srv_func(API_NET_STOP, test_netmgr_srv_stop);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_STOP, func=netmgr_srv_stop, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_003");

    ret = netmgr_reg_srv_func(API_NET_INFO, test_netmgr_srv_info);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_INFO, func=netmgr_srv_info, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_004");

    ret = netmgr_reg_srv_func(API_NET_RESET, test_netmgr_srv_reset);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_RESET, func=netmgr_srv_reset, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_005");

    ret = netmgr_reg_srv_func(EVENT_NET_GOT_IP, test_netmgr_evt_ip_got);
    LOGI(TAG, "netmgr reg src func, cmd=EVENT_NET_GOT_IP, func=netmgr_evt_ip_got, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_006");

    ret = netmgr_reg_srv_func(API_NET_DHCP_CHECK, test_netmgr_check_dhcp);
    LOGI(TAG, "netmgr reg src func, cmd=API_NET_DHCP_CHECK, func=netmgr_check_dhcp, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Reg_007");

    if (task == NULL) {
        task = utask_new("netmgr", 32 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    }

    if (task) {
        netmgr_svc.task = task;
        netmgr_svc.srv = uservice_new("netmgr_svc", test_netmgr_service, &netmgr_svc);
        utask_add(task, netmgr_svc.srv);
    }
}

void test_netmgr_service_deinit()
{
    if (netmgr_svc.srv == NULL) {
        printf("Netmgr already stopped\n");
        return;
    }

    int ret;

    ret = netmgr_unreg_srv_func(API_NET_IPCONFIG, test_netmgr_srv_ipconfig);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_IPCONFIG, func=netmgr_srv_ipconfig, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_001");

    ret = netmgr_unreg_srv_func(API_NET_START, test_netmgr_srv_start);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_START, func=netmgr_srv_start, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_002");
    
    ret = netmgr_unreg_srv_func(API_NET_STOP, test_netmgr_srv_stop);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_STOP, func=netmgr_srv_stop, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_003");
 
    ret = netmgr_unreg_srv_func(API_NET_INFO, test_netmgr_srv_info);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_INFO, func=netmgr_srv_info, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_004");

    ret = netmgr_unreg_srv_func(API_NET_RESET, test_netmgr_srv_reset);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_RESET, func=netmgr_srv_reset, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_005");

    ret = netmgr_unreg_srv_func(EVENT_NET_GOT_IP, test_netmgr_evt_ip_got);
    LOGI(TAG, "netmgr unreg src func, cmd=EVENT_NET_GOT_IP, func=netmgr_evt_ip_got, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_006");

    ret = netmgr_unreg_srv_func(API_NET_DHCP_CHECK, test_netmgr_check_dhcp);
    LOGI(TAG, "netmgr unreg src func, cmd=API_NET_DHCP_CHECK, func=netmgr_check_dhcp, ret=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Unreg_007");

    netmgr_unsubscribe(API_NET_IPCONFIG);
    netmgr_unsubscribe(API_NET_START);
    netmgr_unsubscribe(API_NET_STOP);
    netmgr_unsubscribe(API_NET_INFO);
    netmgr_unsubscribe(API_NET_RESET);
    netmgr_unsubscribe(EVENT_NETWORK_RESTART);
    netmgr_unsubscribe(EVENT_NET_GOT_IP);
    netmgr_unsubscribe(API_NET_DHCP_CHECK);

    utask_destroy(netmgr_svc.task);
    uservice_destroy(netmgr_svc.srv);

    netmgr_svc.task = NULL;
    netmgr_svc.srv = NULL;
}

/*
 * netmgr_dev_wifi_init 接口测试
 */
static void test_wifi_net_init()
{
    int ret;
    LOGD(TAG, "wifi dev init");
    test_wifi_dev_init();
    LOGD(TAG, "add utask netmgr");
    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    LOGI(TAG, "netmgr dev wifi init api test.");
    netmgr_hdl_t hdl = netmgr_dev_wifi_init();
    if (hdl) {
        ret = 0;
        LOGI(TAG, "netmgr wifi dev init pass");
    } else {
        ret = -1;
        LOGE(TAG, "netmgr wifi dev init fail");
    }

    YUNIT_ASSERT_MSG_QA(ret==0, "hdl=%d", hdl, "YOC_NetMgr_WiFiInit_001");
    LOGI(TAG, "netmgr service init");

    test_netmgr_service_init(task);
    netmgr_hdl = hdl;
}

/*
 * netmgr_WiFiConf 接口测试
 */
static void test_netmgr_WiFiConf()
{
    int ret;

    ///<配置ssid以及psk（正常配置）
    char *wifi_ssid_test = "normal_ssid";
    char *wifi_psk_test = "normal_psk";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: normal ssid and psk, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_001");

    ///<配置ssid长度
    wifi_ssid_test = "max_len_ssid_0123456789012345678";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: max len ssid, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_002");

    ///<配置psk长度
    wifi_psk_test = "max_len_psk_0123456789012345678901234567890123456789012345678901";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: max len psk, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_003");

    ///<配置ssid以及psk为特殊字符
    wifi_ssid_test = "ssid special_!@#$\%^&*()<>,.?";
    wifi_psk_test = "psk special !@#$\%^&*()_+<>?";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: ssid and psk with special char, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_004");

    ///<配置ssid长度为最长+1
    wifi_ssid_test = "max+1_len_ssid_012345678901234567";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: max+1 len ssid, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_007");

    
    ///<配置psk长度为最长+1
    wifi_psk_test = "max+1_len_psk_012345678901234567890123456789012345678901234567890";
    ret = netmgr_config_wifi(netmgr_hdl, wifi_ssid_test, strlen(wifi_ssid_test), wifi_psk_test, strlen(wifi_psk_test));
    LOGI(TAG, "config wifi: max+1 len psk, ssid=%s, psk=%s, ret=%d", wifi_ssid_test, wifi_psk_test, ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_WiFiConf_008");
}

/*
 * netmgr_ip_config 接口测试
 */
static void test_netmgr_ip_config()
{
    int ret;

    ///<异常传参，DHCP为1
    ret = netmgr_ipconfig(netmgr_hdl, 1, NULL, NULL, NULL);
    LOGI(TAG, "netmgr ipconfig, dhcp_en=1, ip=NULL, mask=NULL, gw=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==-77, "ret=%d", ret, "YOC_NetMgr_IPConfig_001");

    ///<正常配置，DHCP为1
    ret = netmgr_ipconfig(netmgr_hdl, 1, "172.16.1.100", "255.255.255.0", "172.16.1.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=1, ip=172.16.1.100, mask=255.255.255.0, gw=172.16.1.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_IPConfig_002");

    ///<异常传参，DHCP为0
    ret = netmgr_ipconfig(netmgr_hdl, 0, NULL, NULL, NULL);
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=NULL, mask=NULL, gw=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_007");

    ///<正常配置，DHCP为0
    ret = netmgr_ipconfig(netmgr_hdl, 1, "172.16.1.100", "255.255.255.0", "172.16.1.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=172.16.1.100, mask=255.255.255.0, gw=172.16.1.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_IPConfig_002");

    ///<异常ip配置测试
    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.0", "255.255.255.0", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.0, mask=255.255.255.0, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_011");

    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.255", "255.255.255.0", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.255, mask=255.255.255.0, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_012");

    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.256", "255.255.255.0", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.255, mask=255.255.255.0, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_013");

    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.255.0", "192.168.100.0");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.255.0, gw=192.168.100.0, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_014");

    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.255.0", "192.168.100.255");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.255.0, gw=192.168.100.255, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_015");

    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.255.0", "192.168.100.256");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.255.0, gw=192.168.100.256, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_016");

    
    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.0.255", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.0.255, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_017");

    ///<mask地址异常
    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.255.255", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.255.255, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_018");

    ///<mask地址异常
    ret = netmgr_ipconfig(netmgr_hdl, 0, "192.168.100.1", "255.255.255.256", "192.168.100.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=0, ip=192.168.100.100.1, mask=255.255.255.256, gw=192.168.100.254, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_IPConfig_019");
}

/*
 * netmgr_wifi_connect 接口测试
 */
static void test_netmgr_wifi_connect()
{
    int ret;
   
    ///<异常参数测试
    ret = netmgr_start(NULL);
    LOGI(TAG, "netmgr start, hdl=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "hdl=NULL, ret=%d", ret, "YOC_NetMgr_Start_001");

    ///<正常传参
    ret = netmgr_start(netmgr_hdl);
    LOGI(TAG, "netmgr start, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Start_002");

    ///<配置got ip flag为1
    ret = netmgr_set_gotip("wifi", 1);
    LOGI(TAG, "netmgr set gotip 1, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==1, "set 1, ret=%d", ret, "YOC_NetMgr_setgotip_001");
    
    ///<获取got ip flag为1
    ret = netmgr_is_gotip(netmgr_hdl);
    LOGI(TAG, "netmgr is gotip, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==1, "ret=%d", ret, "YOC_NetMgr_isgotip_001");

    ///<配置got ip flag为0
    ret = netmgr_set_gotip("wifi", 0);
    LOGI(TAG, "netmgr set gotip 0, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_setgotip_002");

    ///<获取got ip flag为0
    ret = netmgr_is_gotip(netmgr_hdl);
    LOGI(TAG, "netmgr is gotip, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_isgotip_002");

    ///<网络reset，正常配置
    ret = netmgr_reset(netmgr_hdl, 0);
    LOGI(TAG, "netmgr wifi reset, delay=0, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret,  "YOC_NetMgr_Reset_001");

    ///<异常参数测试
    ret = netmgr_reset(NULL, 0);
    LOGI(TAG, "netmgr wifi reset, hdl=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "hdl=NULL, ret=%d", ret,  "YOC_NetMgr_Reset_004");

    sleep(5);
}

/*
 * netmgr_getdev 接口测试
 */
static void test_netmgr_getdev()
{
    int ret;
    uint8_t mac[6] = {0};
    netmgr_dev_t *node;

    aos_dev_t *aos_dev;

    ///<异常参数测试
    aos_dev = netmgr_get_dev(NULL);
    ret = hal_wifi_get_mac_addr(aos_dev, mac);
    LOGI(TAG, "netmgr get dev mac hdl=NULL ret=%d, mac=%X:%X:%X:%X:%X:%X", ret, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    YUNIT_ASSERT_MSG_QA(ret!=0, "aos_dev=NULL",  "YOC_NetMgr_Getdev_001");
    
    ///<正常获取dev信息，判断信息为非空
    aos_dev = netmgr_get_dev(netmgr_hdl);
    YUNIT_ASSERT_MSG_QA(aos_dev!=NULL, "aos_dev=NULL",  "YOC_NetMgr_Getdev_002");

    ///<异常参数测试
    ret = netmgr_get_info(NULL);
    LOGI(TAG, "netmgr get info, hdl=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret,  "YOC_NetMgr_Getinfo_001");

    ///<正常参数，判断返回值为0
    ret = netmgr_get_info(netmgr_hdl);
    LOGI(TAG, "netmgr get info, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret,  "YOC_NetMgr_Getinfo_002");

    ///<异常参数测
    node = netmgr_find_dev(&netmgr_svc.dev_list, NULL);
    LOGI(TAG, "netmgr find dev node with null, result=%s", node->name);
    YUNIT_ASSERT_MSG_QA(node->name!=NULL, "node_name=%s", node->name, "YOC_NetMgr_Finddev_001");

    ///<正常参数，获取dev信息无误
    node = netmgr_find_dev(&netmgr_svc.dev_list, "wifi");
    LOGI(TAG, "netmgr find dev node with wifi, result=%s", node->name);
    YUNIT_ASSERT_MSG_QA(strcmp(node->name, "wifi") == 0, "node_name=%s", node->name, "YOC_NetMgr_Finddev_002");

    ///<配置dhcp_en为0
    ret = netmgr_ipconfig(netmgr_hdl, 0, "172.16.1.100", "255.255.255.0", "172.16.1.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=1, ip=172.16.1.100, mask=255.255.255.0, gw=172.16.1.254, result=%d", ret);

    ///<kv配置dhcp_en为0,启动dhcp失败
    ret = netmgr_start_dhcp(&netmgr_svc, "wifi");
    LOGI(TAG, "netmgr start dhcp, name=wifi, dhcp_disable, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret,  "YOC_NetMgr_Startdhcp_001");

    ///<配置dhcp_en为0
    ret = netmgr_ipconfig(netmgr_hdl, 1, "172.16.1.100", "255.255.255.0", "172.16.1.254");
    LOGI(TAG, "netmgr ipconfig, dhcp_en=1, ip=172.16.1.100, mask=255.255.255.0, gw=172.16.1.254, result=%d", ret);

    ///<kv配置dhcp_en为1，启动dhcp成功
    ret = netmgr_start_dhcp(&netmgr_svc, "wifi");
    LOGI(TAG, "netmgr start dhcp, name=wifi, dhcp_disable, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret,  "YOC_NetMgr_Startdhcp_001");
}

/*
 * netmgr_kv接口测试
 */
static void test_netmgr_kvget()
{
    char *wifi_ssid_test;
    char *wifi_psk_test;
    
    ///<获取wifi_ssid,成功
    aos_kv_setstring("wifi_ssid", "normal_ssid");
    wifi_ssid_test = netmgr_kv_get("wifi_ssid");
    LOGI(TAG, "netmgr kv get wifi_ssid, ret=%s, expect=normal_ssid", wifi_ssid_test);
    YUNIT_ASSERT_MSG_QA(strcmp(wifi_ssid_test, "normal_ssid") == 0, "ret=%s", wifi_ssid_test , "YOC_NetMgr_Kvget_001");

    ///<获取wifi_psk,成功
    wifi_psk_test = netmgr_kv_get("wifi_psk");
    LOGI(TAG, "netmgr kv get wifi_psk, ret=%s, expect=normal_psk", wifi_psk_test);
    YUNIT_ASSERT_MSG_QA(strcmp(wifi_psk_test, "normal_psk") == 0, "ret=%s", wifi_psk_test , "YOC_NetMgr_Kvget_003");

    int dhcp_en;
    ///<获取dhcp_en=0,成功
    aos_kv_setint("dhcp_en", 0);
    dhcp_en = netmgr_kv_getint("dhcp_en");
    LOGI(TAG, "netmgr kv get int dhcp_en, ret=%d, expect=0", dhcp_en);
    YUNIT_ASSERT_MSG_QA(dhcp_en==0, "ret=%d", dhcp_en , "YOC_NetMgr_Kvgetint_003");

    ///<获取dhcp_en=1,成功
    aos_kv_setint("dhcp_en", 1);
    dhcp_en = netmgr_kv_getint("dhcp_en");
    LOGI(TAG, "netmgr kv get int dhcp_en, ret=%d, expect=1", dhcp_en);
    YUNIT_ASSERT_MSG_QA(dhcp_en==1, "ret=%d", dhcp_en , "YOC_NetMgr_Kvgetint_004");
}

/*
 * 网络去初始化
 */
static void test_wifi_net_deinit()
{
    int ret;
    ///<异常参数测试
    ret = netmgr_stop(NULL);
    LOGI(TAG, "netmgr stop, hdl=NULL, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret!=0, "ret=%d", ret, "YOC_NetMgr_Stop_001");
 
    ///<正常参数测试
    ret = netmgr_stop(netmgr_hdl);
    LOGI(TAG, "netmgr stop, result=%d", ret);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_NetMgr_Stop_002");
    
    ///<去初始化，测试结束
    test_netmgr_service_deinit();
    netmgr_dev_wifi_deinit(netmgr_hdl);

    netmgr_hdl = NULL;
}

static void test_netmgr()
{
    ///>初始化函数测试
	test_wifi_net_init();
    ///>网络kv值配置测试
	test_netmgr_kvget();
    ///>网络参数获取测试
	test_netmgr_getdev();
    ///>网络配置测试
	test_netmgr_WiFiConf();
	test_netmgr_ip_config();
    ///>网络连接函数测试
	test_netmgr_wifi_connect();
    ///>去初始化函数测试
	test_wifi_net_deinit();
}

/*api测试入口*/
void api_netmgr_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "netmgrapi", test_netmgr);
}
