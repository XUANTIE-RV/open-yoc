#include "aos/kernel.h"
#include <devices/netdrv.h>
#include <devices/ethernet.h>
#include <devices/impl/net_impl.h>
#include <devices/impl/ethernet_impl.h>
#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>
#include <yoc/netmgr_service.h>
#include <yoc/netmgr.h>
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
uint8_t g_eth_got_ip = 0;
netmgr_hdl_t g_netmgr;
#define TAG "ethernet"
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

#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
struct netif eth_xnetif; /* network interface structure */

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

void eth_net_status_callback(struct netif *netif)
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
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        event_publish(EVENT_NET_GOT_IP, NULL);
    }
#if (CONFIG_APP_RTSP_SUPPORT == 1)
    CVI_RtspUpdateIP_CallBack();
#endif
    g_eth_got_ip = 1;
}

typedef struct {
    rvm_dev_t device;

    void *priv;
} eth_dev_t;


static rvm_dev_t *eth_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(eth_dev_t), id);

    return dev;
}

#define eth_dev_uninit rvm_hal_device_free

static int eth_dev_open(rvm_dev_t *dev)
{

    return 0;
}

static int eth_dev_close(rvm_dev_t *dev)
{

    return 0;
}

/*****************************************
 * common netif driver interface
 ******************************************/
static int eth_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    csi_set_macaddr((uint8_t *)mac);
    return 0;
}

static int eth_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    csi_get_macaddr(mac);
    return 0;
}

static int eth_start_dhcp(rvm_dev_t *dev)
{
#if 0
    struct netif *netif = &xnetif;
    aos_check_return_einval(netif);

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    netif_set_status_callback(netif, net_status_callback);
    return netifapi_dhcp_start(netif);
#endif
    return 0;
}

static int eth_stop_dhcp(rvm_dev_t *dev)
{
#if 0
    struct netif *netif = &xnetif;
    aos_check_return_einval(netif);

    netifapi_dhcp_stop(netif);
#endif
    return 0;
}

static int eth_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return -1;
}

static int eth_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    struct netif *netif = &eth_xnetif;
    aos_check_return_einval(netif && ipaddr && netmask_addr && gw_addr);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw_addr), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask_addr), *netif_ip_netmask4(netif));

    return 0;
}

static int eth_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

int eth_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i++) {
        dns_setserver(i, &ipaddr[i]);
    }

    return n;
}

static int eth_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i++) {
        if (!ip_addr_isany(dns_getserver(i))) {
            memcpy(&ipaddr[i], dns_getserver(i), sizeof(ip_addr_t));
        } else {
            return i;
        }
    }

    return n;
}

static int eth_set_hostname(rvm_dev_t *dev, const char *name)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &eth_xnetif;
    netif_set_hostname(netif, name);
    return 0;
#else
    return -1;
#endif
}

static const char *eth_get_hostname(rvm_dev_t *dev)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &eth_xnetif;
    return netif_get_hostname(netif);
#else
    return NULL;
#endif
}

/*****************************************
 * eth driver interface
 ******************************************/

static int eth_mac_control(rvm_dev_t *dev, eth_config_t *config)
{

    return 0;
}

static int eth_set_packet_filter(rvm_dev_t *dev, int type)
{

    return 0;
}

static int eth_start(rvm_dev_t *dev)
{

    return 0;
}

static int eth_stop(rvm_dev_t *dev)
{

    return 0;
}

static int eth_reset(rvm_dev_t *dev)
{

    return 0;
}

static net_ops_t eth_net_driver = {
    .get_mac_addr   = eth_get_mac_addr,
    .set_mac_addr   = eth_set_mac_addr,
    .set_dns_server = eth_set_dns_server,
    .get_dns_server = eth_get_dns_server,
    .set_hostname   = eth_set_hostname,
    .get_hostname   = eth_get_hostname,
    .start_dhcp     = eth_start_dhcp,
    .stop_dhcp      = eth_stop_dhcp,
    .set_ipaddr     = eth_set_ipaddr,
    .get_ipaddr     = eth_get_ipaddr,
    .subscribe      = eth_subscribe,
    .ping           = NULL,
};

static eth_driver_t eth_driver = {
    .mac_control       = eth_mac_control,
    .set_packet_filter = eth_set_packet_filter,
    .start             = eth_start,
    .stop              = eth_stop,
    .reset             = eth_reset,
};

static netdev_driver_t neteth_driver = {
    .drv = {
        .name   = "eth",
        .init   = eth_dev_init,
        .uninit = eth_dev_uninit,
        .open   = eth_dev_open,
        .close  = eth_dev_close,
    },
    .link_type = NETDEV_TYPE_ETH,
    .net_ops =  &eth_net_driver,
    .link_ops = &eth_driver,
};

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


	eth_xnetif.name[0] = 'r';
	eth_xnetif.name[1] = '0' + idx;
	netif_add(&eth_xnetif, &ipaddr, &netmask, &gw, NULL, ethif_init, tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&eth_xnetif);

    /*  When the netif is fully configured this function must be called.*/
	netif_set_link_up(&eth_xnetif);
	netif_set_up(&eth_xnetif);

	// netif_set_ipaddr(&xnetif, NULL);
    // netif_set_netmask(&xnetif, NULL);
    // netif_set_gw(&xnetif[0], NULL);
	netif_set_status_callback(&eth_xnetif, eth_net_status_callback);
    if(rvm_driver_register(&neteth_driver.drv, NULL, 0) < 0) {
        printf("ether device register error \r\n");
    } else {
        printf("ether device register success \r\n");
    }
    g_netmgr = netmgr_dev_eth_init();
    event_service_init(NULL);
    netmgr_service_init(NULL);
    LOGD(TAG, "DHCP start");
	netifapi_dhcp_start(&eth_xnetif);
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
