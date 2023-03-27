#include "aos/kernel.h"
#include "k_api.h"
#include "lwip/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"
#include "ulog/ulog.h"
#include "ethernetif.h"
#include "cvi_mac_phy.h"
#if (CONFIG_APP_RTSP_SUPPORT == 1)
#include "rtsp_exapi.h"
#endif
// #include "ntp.h"
// #include "cx_cloud.h"
#define TAG "ethernet"
// #include "link_visual_struct.h"
uint8_t g_eth_got_ip = 0;
/*Static IP ADDRESS*/
#ifndef IP_ADDR0
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   3
#endif
/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif
/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif

struct netif xnetif; /* network interface structure */

#if 0
void *cxcloud_hdl;

static void ntp_task(void *arg)
{
    while(1) {
        if(ntp_sync_time(NULL) == 0) {
            break;
        }
        aos_msleep(1000);
    }
}

/* 属性设置回调 */
static void iot_set_property_handler(const lv_device_auth_s *auth, const char *value) 
{
    
}

static int iot_connect_handle(void)
{
    return 0;
}

static int iot_disconnect_handle(void)
{
    return 0;
}

static void cx_cloud_task(void *arg)
{
    char product_key[PRODUCT_KEY_LEN + 1] = "a1LC3nIcMir";
    char device_name[DEVICE_NAME_LEN + 1] = "cv182x";
    char device_secret[DEVICE_SECRET_LEN + 1] = "4f2f0998cf4175b047e5cec4145ad06e";
    char product_secret[PRODUCT_SECRET_LEN + 1] = "wXrhzDimVtHhHI8X";
    int log_level = CX_CLOUD_LOG_WARN;//默认debug日志级别

    // if ((0 == HAL_GetProductKey(product_key)) || (0 == HAL_GetProductSecret(product_secret)) ||
    //     (0 == HAL_GetDeviceSecret(device_secret)) || (0 == HAL_GetDeviceName(device_name))) {
    //     printf("!!!!!! missing ProductKey/ProductSecret/DeviceName/DeviceSecret\n");
    //     return;
    // }
    LOGD(TAG, "device_name:%s", device_name);
    LOGD(TAG, "device_secret:%s", device_secret);
    LOGD(TAG, "product_key:%s", product_key);
    LOGD(TAG, "product_secret:%s", product_secret);

    cx_cloud_param_t param;
    param.dev_id = 0;
    param.device_name = device_name;
    param.device_secret = device_secret;
    param.product_key = product_key;
    param.product_secret = product_secret;
    param.log_level = log_level;

    iot_register_set_property_callback(iot_set_property_handler);
    iot_register_connect_callback(iot_connect_handle);
    iot_register_disconnect_callback(iot_disconnect_handle);
    cxcloud_hdl = cx_cloud_init(&param, NULL);
    cx_cloud_connect(cxcloud_hdl);
    aos_task_exit(0);
}
#endif

void net_status_callback(struct netif *netif)
{
#if 0
    aos_task_t ntp_handle;
    aos_task_t cx_cloud_handle;
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        // event_publish(EVENT_NET_GOT_IP, NULL);
		LOGD(TAG, "IP address: %s", ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(&netif->ip_addr)));
        LOGD(TAG, "Subnet mask: %s", ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(&netif->netmask)));
        LOGD(TAG, "Default gateway:  %s", ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(&netif->gw)));
        aos_task_new_ext(&ntp_handle, "ntp_task", ntp_task, NULL, 2048, AOS_DEFAULT_APP_PRI + 10);
        aos_task_new_ext(&cx_cloud_handle, "cx_cloud_task", cx_cloud_task, NULL, 8192, AOS_DEFAULT_APP_PRI + 10);
    }
#endif
#if (CONFIG_APP_RTSP_SUPPORT == 1)
    CVI_RtspUpdateIP_CallBack();
#endif
    g_eth_got_ip = 1;
}
static void eth_init_func(void *paras)
{
	ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    int8_t idx = 0;
    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);


	xnetif.name[0] = 'r';
	xnetif.name[1] = '0' + idx;
	netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, ethif_init, tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&xnetif);

    /*  When the netif is fully configured this function must be called.*/
	netif_set_link_up(&xnetif);
	netif_set_up(&xnetif);

	// netif_set_ipaddr(&xnetif, NULL);
    // netif_set_netmask(&xnetif, NULL);
    // netif_set_gw(&xnetif[0], NULL);
	netif_set_status_callback(&xnetif, net_status_callback);
    LOGD(TAG, "DHCP start");
	netifapi_dhcp_start(&xnetif);
}

void ethernet_init(void)
{
    int ret;
    aos_task_t eth_hdl;

    csi_eth_mac_phy_init();
    ret = aos_task_new_ext(&eth_hdl, "eth_init", eth_init_func, NULL, 8192, AOS_DEFAULT_APP_PRI+1);
    if (RHINO_SUCCESS != ret) {
        printf("gmac_init task create fail!\n");
    }
}
