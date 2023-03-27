/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <sal.h>
#include <devices/wifi.h>
#include <devices/impl/wifi_impl.h>

#include "w800.h"
#include "w800_api.h"
#include "board.h"
#include "drv/gpio.h"

#define WIFI_SSID_MAX_LEN 32
#define WIFI_PSK_MAX_LEN 64

#define WIFI_CHANNEL_MAX 14
#define WIFI_CHANNEL_ANY 255

enum wifi_security_type {
    WIFI_SECURITY_TYPE_NONE = 0,
    WIFI_SECURITY_TYPE_PSK,
};

struct wifi_connect_req_params {
    uint8_t ssid[WIFI_SSID_MAX_LEN];
    uint8_t ssid_length; /* Max 32 */

    uint8_t psk[WIFI_PSK_MAX_LEN];
    uint8_t psk_length; /* Min 8 - Max 64 */

    uint8_t                 channel;
    enum wifi_security_type security;
};

typedef struct {
    rvm_dev_t   device;
    uint8_t mode;

    struct wifi_connect_req_params conn_params;

    void (*write_event)(rvm_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;

#define TAG "w800_dev"

char net_ipaddr[16], net_mask[16], net_gw[16];

enum {
    CONN_STATUS_UNKNOWN = 0,
    CONN_STATUS_INITED,
    CONN_STATUS_CLOSING,
    CONN_STATUS_CLOSED
};

typedef struct link_s {
    int       fd;
    int       conn_status;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

aos_mutex_t    g_link_mutex;
#define AT_SOCKET_NUM 5

static int                     g_link_status = 0;
static link_t                  g_link[AT_SOCKET_NUM];
static netconn_data_input_cb_t g_netconn_data_input_cb;
static netconn_close_cb_t      g_netconn_close_cb;
static sal_op_t                w800_sal_driver;
static w800_wifi_param_t       w800_param;



/*****************************************
* common driver interface
******************************************/
static rvm_dev_t *w800_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

#define w800_dev_uninit rvm_hal_device_free

static int w800_dev_open(rvm_dev_t *dev)
{
    // power on device
    sal_module_register(&w800_sal_driver);

    sal_init();

    return 0;
}

static int w800_dev_close(rvm_dev_t *dev)
{
    //power off device
    sal_deinit();

    sal_module_unregister();
    return 0;
}

/*****************************************
* common netif driver interface
******************************************/
static int w800_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    return 0;
}

#if 0
static int w800_start_dhcp(rvm_dev_t *dev)
{
    return 0;
}

static int w800_stop_dhcp(rvm_dev_t *dev)
{
    return 0;
}
#endif

static int w800_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask,
                           const ip_addr_t *gw)
{
    return 0;
}

static int w800_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    int  ret;
    char ip[16], gw[16], mask[16];

    ret = w800_link_info(ip, gw, mask);

    ipaddr_aton(ip, (ip4_addr_t *)ipaddr);
    ipaddr_aton(mask, (ip4_addr_t *)netmask_addr);
    ipaddr_aton(gw, (ip4_addr_t *)gw_addr);

    return ret;
}

static int w800_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    return w800_get_mac(dev, mac);
}


int w800_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    return -1;
}

int w800_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    return -1;
}

static int w800_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

/*****************************************
* wifi driver interface
******************************************/
int w800_set_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t mode)
{
    if (w800_set_wifi_mode(mode)) {
        LOGD(TAG, "set mode");
        return -1;
    }

    if (w800_enable_muti_sockets(1)) {
        LOGD(TAG, "muti sock");
        return -1;
    }

    if (w800_get_remote_info(1)) {
        LOGD(TAG, "remote info");
        return -1;
    }

    return 0;
}

int w800_get_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t *mode)
{
    if (mode == NULL) {
        return -EINVAL;
    }

    *mode = WIFI_MODE_STA;
    return 0;
}

int w800_init(rvm_dev_t *dev)
{
    return 0;
}

int w800_deinit(rvm_dev_t *dev)
{
    return 0;
}

int w800_start(rvm_dev_t *dev, rvm_hal_wifi_config_t * config)
{
    // w800_set_mode(dev, config->mode);
    return w800_ap_connect((const char *)config->ssid, (const char *)config->password);

}

int w800_stop(rvm_dev_t *dev)
{
    return 0;
}

int w800_reset(rvm_dev_t *dev)
{
    int ret;

    for (int i = 0; i < 3; i++) {
        ret = w800_software_reset();
        if (ret >= 0)
            break;
    }

    return 0;
}

int w800_disconnect_ap(rvm_dev_t *dev)
{
    return w800_ap_disconnect();
}

int w800_ping_remote(rvm_dev_t *dev, int type, char *remote_ip)
{
    int retry = 0;
    int i;

    for (i = 0; i < 10; i++) {
        if (w800_ping(remote_ip, i + 1) < 0) {
            if ((retry++) >= 3) {
                break;
            }
        }
    }

    return 0;
}

int w800_get_ap_info(rvm_hal_wifi_ap_record_t *ap_info)
{
    char ssid[32];
    int bssid[6];
    int channel;
    int rssi;
    int ret = 0;

    ret = w800_ap_info(ssid, bssid, &channel, &rssi);

    if (ret >= 0) {
        ap_info->bssid[0] = bssid[0];
        ap_info->bssid[1] = bssid[1];
        ap_info->bssid[2] = bssid[2];
        ap_info->bssid[3] = bssid[3];
        ap_info->bssid[4] = bssid[4];
        ap_info->bssid[5] = bssid[5];

        memcpy(ap_info->ssid, ssid, sizeof(ssid));
        ap_info->channel = channel;
        ap_info->rssi = rssi;
    }

    return ret;
}


int w800_drv_get_link_status(rvm_dev_t *dev, rvm_hal_wifi_ap_record_t *ap_info)
{
    int status;

    if (ap_info == NULL || dev == NULL) {
        return -EINVAL;
    }

    status = w800_get_link_status();

    switch (status) {
    case NET_STATUS_GOTIP:
        ap_info->link_status = WIFI_STATUS_GOTIP;
        w800_get_ap_info(ap_info);
        break;
    case NET_STATUS_LINKUP:
        ap_info->link_status = WIFI_STATUS_LINK_UP;
        w800_get_ap_info(ap_info);
        break;
    case NET_STATUS_LINKDOWN:
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
        break;
    default:
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
    }


    return 0;
}

int w800_get_link_info(rvm_dev_t *dev, char ip[16], char gw[16], char mask[16])
{
    return w800_link_info(ip, gw, mask);
}

int w800_smartconfig(rvm_dev_t *dev, int enable)
{

    return -1;
}

int w800_if_config(rvm_dev_t *dev, uint32_t baud, uint8_t flow_control)
{
    return w800_uart_config(baud, flow_control);
}
/****************************************
*
* w800_wifi adaptation interface
*
*****************************************/
static int fd_to_linkid(int fd)
{
    int link_id;

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);

    for (link_id = 0; link_id < AT_SOCKET_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static void w800_wifi_socket_data_handle(int linkid, void *data, size_t len, char remote_ip[16],
        uint16_t remote_ports)
{
    /*
    +IPD,0,44,106.15.213.199,5684:
    */
    if (g_netconn_data_input_cb && ((g_link[linkid].fd >= 0))) {
        while (g_netconn_data_input_cb(g_link[linkid].fd, data, len, remote_ip, remote_ports) < 0) {
            if (g_link[linkid].conn_status == CONN_STATUS_INITED) {
                aos_msleep(100);
            } else {
                /* conn may be closing or closed */
                break;
            }
            // LOGW(TAG, "socket %d get data %d fail, drop", g_link[linkid].fd, len);
        }
    }

    return;
}

static void w800_wifi_close_handle(int linkid)
{
    /*
    0,CLOSED:
    */
    if (g_netconn_close_cb && ((g_link[linkid].fd >= 0))) {
        if (g_netconn_close_cb(g_link[linkid].fd)) {
            LOGD(TAG, "socket %d close fail");
        }
        g_link[linkid].fd          = -1;
        g_link[linkid].conn_status = CONN_STATUS_CLOSED;
    }

    return;
}

int w800_link_status(int *socket_stat)
{
    static int now_stat = -1;
    int        i;

    if (now_stat != g_link_status) {
        w800_get_status(&g_link_status);
        now_stat = g_link_status;
    }

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);

    for (i = 0; i < AT_SOCKET_NUM; i++) {
        if (g_link[i].fd >= 0) {
            *(socket_stat + i) = 1;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return g_link_status;
}

int aos_w800_get_link_status(void)
{
    return w800_link_status(NULL);
}

/*****************************************
* w800 SAL interface
******************************************/
static int w800_wifi_module_init(void)
{
    for (int i = 0; i < AT_SOCKET_NUM; i++) {
        g_link[i].fd = -1;
    }

    aos_mutex_new(&g_link_mutex);
    w800_packet_input_cb_register(&w800_wifi_socket_data_handle);
    w800_close_cb_register(&w800_wifi_close_handle);

    return 0;
}

static int w800_wifi_module_deinit()
{
    aos_mutex_free(&g_link_mutex);
    return 0;
}

static int w800_wifi_module_domain_to_ip(char *domain, char ip[16])
{
    /* param "domain&&ip" check at sal.c */

    if (w800_domain_to_ip(domain, ip) < 0) {
        //LOGE(TAG, "domain to ip fail");
        return -1;
    }

    return 0;
}

static int w800_wifi_module_conn_start(sal_conn_t *conn)
{
    int        linkid   = 0;
    int        ret      = -1;
    net_conn_e net_type = 0;

    /* param "conn && conn->addr" check at sal.c */

    aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER);

    for (linkid = 0; linkid < AT_SOCKET_NUM; linkid++) {
        if (g_link[linkid].fd >= 0) {
            continue;
        }

        g_link[linkid].fd = conn->fd;
        g_link[linkid].conn_status = CONN_STATUS_INITED;
        break;
    }

    aos_mutex_unlock(&g_link_mutex);

    if (linkid >= AT_SOCKET_NUM) {
        LOGE(TAG, "linkid");
        return -1;
    }

    switch (conn->type) {
    case TCP_SERVER:
        net_type = NET_TYPE_TCP_SERVER;
        break;

    case TCP_CLIENT:
        net_type = NET_TYPE_TCP_CLIENT;
        break;

    case UDP_UNICAST:
        net_type = NET_TYPE_UDP_UNICAST;
        break;

    case SSL_CLIENT:
    case UDP_BROADCAST:
    default:
        break;
    }

    ret = w800_connect_remote(linkid, net_type, conn->addr, conn->r_port);

    if (ret < 0) {
        LOGE(TAG, "conn_start fail(%d)(%d)", linkid, g_link[linkid].fd);
        return -1;
    }
    return 0;
}

static int w800_wifi_module_conn_close(int fd, int32_t remote_port)
{
    int linkid = 0;
    int ret    = 0;

    linkid = fd_to_linkid(fd);

    if (linkid >= AT_SOCKET_NUM || linkid < 0) {
        LOGD(TAG, "unknown fd(%d)", fd);
        return -1;
    }

    g_link[linkid].conn_status = CONN_STATUS_CLOSING;
    ret = w800_close(linkid);

    g_link[linkid].fd = -1;
    g_link[linkid].conn_status = CONN_STATUS_CLOSED;

    return ret;
}

static int w800_wifi_module_send(int fd, uint8_t *data, uint32_t len, char remote_ip[16],
                                 int32_t remote_port, int32_t timeout)
{
#define W800_MIN_TIMOUT (10000)
    int linkid;
    int ret = -1;

    /* param "data" check at sal.c */

    linkid = fd_to_linkid(fd);

    if (linkid >= AT_SOCKET_NUM) {
        LOGD(TAG, "unknown fd(%d)", fd);
        return -1;
    }

    int offset   = 0;
    int send_len = 0;
    long long time_th = timeout > W800_MIN_TIMOUT?aos_now_ms() + timeout:aos_now_ms() + W800_MIN_TIMOUT;
    int remain_time;

    w800_choose_id(linkid);

    while (1) {
        send_len = (len - offset) <= W800_MAX_SEND_LEN ? len - offset : W800_MAX_SEND_LEN;
        remain_time = (int)(time_th - aos_now_ms());
        if(remain_time <= 0) {
            LOGE(TAG, "w800 send timout(%d) remain:%d", timeout, remain_time);
            return -1;
        }
        ret = w800_send_data(data + offset, send_len, remain_time);

        if (ret < 0) {
            LOGE(TAG, "w800 send failed --> time:%d remain:%d ret:%d", timeout, (int)(time_th - aos_now_ms()), ret);
            return -1;
        } else {
            offset += send_len;
        }

        if(offset == len) {
            return 0;
        }
    }

    return 0;
}

static int w800_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    /* param "cb" check at sal.c */

    g_netconn_data_input_cb = cb;

    return 0;
}

static int w800_wifi_close_cb_register(netconn_close_cb_t cb)
{
    /* param "cb" check at sal.c */

    g_netconn_close_cb = cb;

    return 0;
}

static net_ops_t w800_net_driver = {
    .set_mac_addr   = w800_set_mac_addr,
    .start_dhcp     = NULL,/*w800_start_dhcp*/
    .stop_dhcp      = NULL,/*w800_stop_dhcp*/
    .set_ipaddr     = w800_set_ipaddr,
    .get_ipaddr     = w800_get_ipaddr,
    .get_mac_addr   = w800_get_mac_addr,
    .set_dns_server = w800_set_dns_server,
    .get_dns_server = w800_get_dns_server,
    .ping           = w800_ping_remote,
    .subscribe      = w800_subscribe,
};

static wifi_driver_t w800_wifi_driver = {
    .init                = w800_init,
    .deinit              = w800_deinit,
    .start               = w800_start,
    .stop                = w800_stop,
    .reset               = w800_reset,
    .set_mode            = w800_set_mode,
    .get_mode            = w800_get_mode,
    .sta_get_link_status = w800_drv_get_link_status,
    .set_smartcfg        = w800_smartconfig,
};

static sal_op_t w800_sal_driver = {
    .version                        = "1.0.0",
    .init                           = w800_wifi_module_init,
    .start                          = w800_wifi_module_conn_start,
    .send                           = w800_wifi_module_send,
    .domain_to_ip                   = w800_wifi_module_domain_to_ip,
    .close                          = w800_wifi_module_conn_close,
    .deinit                         = w800_wifi_module_deinit,
    .register_netconn_data_input_cb = w800_wifi_packet_input_cb_register,
    .register_netconn_close_cb      = w800_wifi_close_cb_register,
};

static netdev_driver_t w800_driver = {
    .drv =
    {
        .name   = "wifi",
        .init   = w800_dev_init,
        .uninit = w800_dev_uninit,
        .open   = w800_dev_open,
        .close  = w800_dev_close,
    },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops  = &w800_net_driver,
    .link_ops  = &w800_wifi_driver,
};

void wifi_w800_register(utask_t *task, w800_wifi_param_t *param)
{
    int ret = w800_module_init(task, param);

    if (ret < 0) {
        LOGE(TAG, "driver init error");
        return;
    }

    //run w800_dev_init to create wifi_dev_t and bind this driver
    ret = rvm_driver_register(&w800_driver.drv, NULL, 0);

    if (ret < 0) {
        LOGE(TAG, "device register error");
    }

    memcpy(&w800_param, param, sizeof(w800_wifi_param_t));
}
