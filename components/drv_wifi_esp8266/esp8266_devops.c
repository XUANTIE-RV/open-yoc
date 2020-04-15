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
#include <devices/hal/wifi_impl.h>

#include "esp8266.h"
#include "esp8266_api.h"

#include "pin.h"
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
    aos_dev_t   device;
    uint8_t mode;

    struct wifi_connect_req_params conn_params;

    void (*write_event)(aos_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;

#define TAG "8266_dev"

char net_ipaddr[16], net_mask[16], net_gw[16];

typedef struct link_s {
    int       fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

aos_mutex_t    g_link_mutex;
#define AT_SOCKET_NUM 5

static int                     g_link_status = 0;
static link_t                  g_link[AT_SOCKET_NUM];
static netconn_data_input_cb_t g_netconn_data_input_cb;
static netconn_close_cb_t      g_netconn_close_cb;
static sal_op_t                esp8266_sal_driver;
static esp_wifi_param_t        esp8266_param;

static gpio_pin_handle_t *smart_pin = NULL;

extern void sal_init(void);

static void smartcfg_int_handle(int32_t idx)
{
    event_publish(EVENT_WIFI_SMARTCFG, NULL);
}

int esp8266_pin_init(int pin_name)
{
    drv_pinmux_config(pin_name, PIN_FUNC_GPIO);

    smart_pin = csi_gpio_pin_initialize(pin_name, smartcfg_int_handle);
    csi_gpio_pin_config_mode(smart_pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(smart_pin, GPIO_DIRECTION_INPUT);

    csi_gpio_pin_set_irq(smart_pin, GPIO_IRQ_MODE_RISING_EDGE, 1);

    return 0;
}

/*****************************************
* common driver interface
******************************************/
static aos_dev_t *esp8266_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

#define esp8266_dev_uninit device_free

static int esp8266_dev_open(aos_dev_t *dev)
{
    // power on device
    sal_module_register(&esp8266_sal_driver);

    sal_init();

    return 0;
}

static int esp8266_dev_close(aos_dev_t *dev)
{
    //power off device
    return 0;
}

/*****************************************
* common netif driver interface
******************************************/
static int esp8266_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    return 0;
}

#if 0
static int esp8266_start_dhcp(aos_dev_t *dev)
{
    return 0;
}

static int esp8266_stop_dhcp(aos_dev_t *dev)
{
    return 0;
}
#endif

static int esp8266_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask,
                              const ip_addr_t *gw)
{
    return 0;
}

static int esp8266_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    int  ret;
    char ip[16], gw[16], mask[16];

    ret = esp8266_link_info(ip, gw, mask);

    ipaddr_aton(ip, (ip4_addr_t *)ipaddr);
    ipaddr_aton(mask, (ip4_addr_t *)netmask_addr);
    ipaddr_aton(gw, (ip4_addr_t *)gw_addr);

    return ret;
}

static int esp8266_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    return esp8266_get_mac(dev, mac);
}


int esp8266_set_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    return -1;
}

int esp8266_get_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    return -1;
}

static int esp8266_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

/*****************************************
* wifi driver interface
******************************************/
int esp8266_set_mode(aos_dev_t *dev, wifi_mode_t mode)
{
    if (esp8266_set_wifi_mode(mode)) {
        LOGD(TAG, "set mode");
        return -1;
    }

    if (esp8266_enable_muti_sockets(1)) {
        LOGD(TAG, "muti sock");
        return -1;
    }

    if (esp8266_get_remote_info(1)) {
        LOGD(TAG, "remote info");
        return -1;
    }

    return 0;
}

int esp8266_get_mode(aos_dev_t *dev, wifi_mode_t *mode)
{
    if (mode == NULL) {
        return -EINVAL;
    }

    *mode = WIFI_MODE_STA;
    return 0;
}

int esp8266_init(aos_dev_t *dev)
{
    return 0;
}

int esp8266_deinit(aos_dev_t *dev)
{
    return 0;
}

int esp8266_start(aos_dev_t *dev, wifi_config_t * config)
{
    esp8266_set_mode(dev, config->mode);
    return esp8266_ap_connect((const char *)config->ssid, (const char *)config->password);

}

int esp8266_stop(aos_dev_t *dev)
{
    return 0;
}

int esp8266_reset(aos_dev_t *dev)
{
    int ret;
    int i;

    for (i = 0; i < 3; i++) {
        ret = esp8266_hard_reset();
        if (ret < 0) {
            ret = esp8266_software_reset();
            if (ret >= 0)
                break;
        } else {
            break;
        }
    }

    if (ret < 0) {
        LOGE(TAG, "reset failed");
        return -1;
    }

    ret = esp8266_at0();

    if (ret < 0) {
        printf("\r\n");
        LOGE(TAG, "not online");
        return -1;
    }

    ret = esp8266_close_echo();

    if (ret < 0) {
        LOGE(TAG, "close echo err");
        return -1;
    }

    if(esp8266_param.enable_flowctl == 0) {
        ret = esp8266_uart_config(esp8266_param.baud, 0);
    } else {
        ret = esp8266_uart_config(esp8266_param.baud, 3);
    }

    if(ret < 0) {
        LOGE(TAG, "uart config err");
        return -1;
    }

    return 0;
}

int esp8266_disconnect_ap(aos_dev_t *dev)
{
    return esp8266_ap_disconnect();
}

int esp8266_ping_remote(aos_dev_t *dev, int type, char *remote_ip)
{
    int retry = 0;
    int i;

    for (i = 0; i < 10; i++) {
        if (esp8266_ping(remote_ip, i + 1) < 0) {
            if ((retry++) >= 3) {
                break;
            }
        }
    }

    return 0;
}

int esp8266_get_ap_info(wifi_ap_record_t *ap_info)
{
    char ssid[32];
    char bssid[18];
    int bssid_tmp[6];
    int channel; 
    int rssi;
    int ret = 0;

    ret = esp8266_ap_info(ssid, bssid, &channel, &rssi);

    sscanf(bssid, "%02x:%02x:%02x:%02x:%02x:%02x" ,
        &bssid_tmp[0], &bssid_tmp[1], &bssid_tmp[2], 
        &bssid_tmp[3], &bssid_tmp[4], &bssid_tmp[5]);

    ap_info->bssid[0] = bssid_tmp[0];
    ap_info->bssid[1] = bssid_tmp[1];
    ap_info->bssid[2] = bssid_tmp[2];
    ap_info->bssid[3] = bssid_tmp[3];
    ap_info->bssid[4] = bssid_tmp[4];
    ap_info->bssid[5] = bssid_tmp[5];

    memcpy(ap_info->ssid, ssid, sizeof(ssid));
    ap_info->channel = channel;
    ap_info->rssi = rssi;

    return ret;
}


int esp8266_drv_get_link_status(aos_dev_t *dev, wifi_ap_record_t *ap_info)
{
    int status;

    if (ap_info == NULL || dev == NULL) {
        return -EINVAL;
    }

    status = esp8266_get_link_status();

    switch (status) {
        case NET_STATUS_GOTIP:
            ap_info->link_status = WIFI_STATUS_GOTIP;
            esp8266_get_ap_info(ap_info);
            break;
        case NET_STATUS_LINKUP:
            ap_info->link_status = WIFI_STATUS_LINK_UP;
            esp8266_get_ap_info(ap_info);
            break;
        case NET_STATUS_LINKDOWN:
            ap_info->link_status = WIFI_STATUS_LINK_DOWN;
            break;
        default:
            ap_info->link_status = WIFI_STATUS_LINK_DOWN;
    }


    return 0;
}

int esp8266_get_link_info(aos_dev_t *dev, char ip[16], char gw[16], char mask[16])
{
    return esp8266_link_info(ip, gw, mask);
}

int esp8266_smartconfig(aos_dev_t *dev, int enable)
{
    if (smart_pin == NULL) {
        LOGW(TAG, "smartconfig button disabled.");
        return -1;
    }

    return esp8266_smartconfig_enable(enable);
}

int esp8266_if_config(aos_dev_t *dev, uint32_t baud, uint8_t flow_control)
{
    return esp8266_uart_config(baud, flow_control);
}
/****************************************
*
* esp8266_wifi adaptation interface
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

static void esp8266_wifi_socket_data_handle(int linkid, void *data, size_t len, char remote_ip[16],
                                            uint16_t remote_ports)
{
    /*
    +IPD,0,44,106.15.213.199,5684:
    */
    if (g_netconn_data_input_cb && ((g_link[linkid].fd >= 0))) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, data, len, remote_ip, remote_ports)) {
            LOGW(TAG, "socket %d get data %d fail, drop", g_link[linkid].fd, len);
        }
    }

    return;
}

static void esp8266_wifi_close_handle(int linkid)
{
    /*
    0,CLOSED:
    */
    if (g_netconn_close_cb && ((g_link[linkid].fd >= 0))) {
        if (g_netconn_close_cb(g_link[linkid].fd)) {
            LOGD(TAG, "socket %d close fail");
        }
        g_link[linkid].fd = -1;
    }

    return;
}

int esp8266_link_status(int *socket_stat)
{
    static int now_stat = -1;
    int        i;

    if (now_stat != g_link_status) {
        esp8266_get_status(&g_link_status);
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

int aos_esp8266_get_link_status(void)
{
    return esp8266_link_status(NULL);
}

/*****************************************
* esp8266 SAL interface
******************************************/
static int esp8266_wifi_module_init(void)
{
    for (int i = 0; i < AT_SOCKET_NUM; i++) {
        g_link[i].fd = -1;
    }

    aos_mutex_new(&g_link_mutex);
    esp8266_packet_input_cb_register(&esp8266_wifi_socket_data_handle);
    esp8266_close_cb_register(&esp8266_wifi_close_handle);

    return 0;
}

static int esp8266_wifi_module_deinit()
{
    aos_mutex_free(&g_link_mutex);
    return 0;
}

static int esp8266_wifi_module_domain_to_ip(char *domain, char ip[16])
{
    /* param "domain&&ip" check at sal.c */

    if (esp8266_domain_to_ip(domain, ip) < 0) {
        //LOGE(TAG, "domain to ip fail");
        return -1;
    }

    return 0;
}

static int esp8266_wifi_module_conn_start(sal_conn_t *conn)
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

    ret = esp8266_connect_remote(linkid, net_type, conn->addr, conn->r_port);

    if (ret < 0) {
        LOGE(TAG, "conn_start fail(%d)(%d)", linkid, g_link[linkid].fd);
        return -1;
    }
    return 0;
}

static int esp8266_wifi_module_conn_close(int fd, int32_t remote_port)
{
    int linkid = 0;
    int ret    = 0;

    linkid = fd_to_linkid(fd);

    if (linkid >= AT_SOCKET_NUM || linkid < 0) {
        LOGD(TAG, "unknown fd(%d)", fd);
        return -1;
    }

    ret = esp8266_close(linkid);

    g_link[linkid].fd = -1;

    return ret;
}

static int esp8266_wifi_module_send(int fd, uint8_t *data, uint32_t len, char remote_ip[16],
                                    int32_t remote_port, int32_t timeout)
{
    #define ESP8266_MIN_TIMOUT (3000)
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
    long long time_th = timeout > ESP8266_MIN_TIMOUT?aos_now_ms() + timeout:aos_now_ms() + ESP8266_MIN_TIMOUT;
    int remain_time;

    while (1) {
        send_len = (len - offset) <= ESP8266_MAX_SEND_LEN ? len - offset : ESP8266_MAX_SEND_LEN;
        remain_time = (int)(time_th - aos_now_ms());
        if(remain_time <= 0) {
            LOGE(TAG, "esp8266 send timout(%d) remain:%d", timeout, remain_time);
            return -1;
        }
        ret = esp8266_send_data(linkid, data + offset, send_len, remain_time);

        if (ret < 0) {
            LOGE(TAG, "esp8266 send failed --> time:%d remain:%d ret:%d", timeout, (int)(time_th - aos_now_ms()), ret);
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

static int esp8266_wifi_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    /* param "cb" check at sal.c */

    g_netconn_data_input_cb = cb;

    return 0;
}

static int esp8266_wifi_close_cb_register(netconn_close_cb_t cb)
{
    /* param "cb" check at sal.c */

    g_netconn_close_cb = cb;

    return 0;
}

static net_ops_t esp8266_net_driver = {
    .set_mac_addr = esp8266_set_mac_addr,
    .start_dhcp   = NULL,/*esp8266_start_dhcp*/
    .stop_dhcp   = NULL,/*esp8266_stop_dhcp*/
    .set_ipaddr   = esp8266_set_ipaddr,
    .get_ipaddr   = esp8266_get_ipaddr,
    .get_mac_addr = esp8266_get_mac_addr,
    .set_dns_server = esp8266_set_dns_server,
    .get_dns_server = esp8266_get_dns_server,
    .ping         = esp8266_ping_remote,
    .subscribe    = esp8266_subscribe,
};

static wifi_driver_t esp8266_wifi_driver = {
    .init               = esp8266_init,
    .deinit             = esp8266_deinit,
    .start              = esp8266_start,
    .stop               = esp8266_stop,
    .reset              = esp8266_reset,
    .set_mode           = esp8266_set_mode,
    .get_mode           = esp8266_get_mode,
    .sta_get_link_status    = esp8266_drv_get_link_status,
    .set_smartcfg       = esp8266_smartconfig,
};

static sal_op_t esp8266_sal_driver = {
    .version                        = "1.0.0",
    .init                           = esp8266_wifi_module_init,
    .start                          = esp8266_wifi_module_conn_start,
    .send                           = esp8266_wifi_module_send,
    .domain_to_ip                   = esp8266_wifi_module_domain_to_ip,
    .close                          = esp8266_wifi_module_conn_close,
    .deinit                         = esp8266_wifi_module_deinit,
    .register_netconn_data_input_cb = esp8266_wifi_packet_input_cb_register,
    .register_netconn_close_cb      = esp8266_wifi_close_cb_register,
};

static netdev_driver_t esp8266_driver = {
    .drv =
        {
            .name   = "wifi",
            .init   = esp8266_dev_init,
            .uninit = esp8266_dev_uninit,
            .open   = esp8266_dev_open,
            .close  = esp8266_dev_close,
        },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops  = &esp8266_net_driver,
    .link_ops  = &esp8266_wifi_driver,
};

void wifi_esp8266_register(utask_t *task, esp_wifi_param_t *param)
{
    int ret = esp8266_module_init(task, param);

    if (ret < 0) {
        LOGE(TAG, "driver init error");
        return;
    }

    if (param->smartcfg_pin) {
        esp8266_pin_init(param->smartcfg_pin);
    }

    //run esp8266_dev_init to create wifi_dev_t and bind this driver
    ret = driver_register(&esp8266_driver.drv, NULL, 0);

    if (ret < 0) {
        LOGE(TAG, "device register error");
    }

    memcpy(&esp8266_param, param, sizeof(esp_wifi_param_t));
}
