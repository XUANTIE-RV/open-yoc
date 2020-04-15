/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <yoc/uservice.h>

#include <sal.h>

#include <devices/netdrv.h>
#include <devices/hal/gprs_impl.h>


#include "sim800.h"
#include "sim800_api.h"

#include "pin.h"
#include "drv/gpio.h"

typedef struct {
    aos_dev_t device;
    uint8_t mode;

    void *priv;
} gprs_dev_t;

#define TAG "sim800_dev"

char net_ipaddr[16];

typedef struct link_s {
    int fd;
    aos_sem_t sem_start;
    aos_sem_t sem_close;
} link_t;

aos_mutex_t g_link_mutex;
#define AT_SOCKET_NUM  6

static int g_link_status = 0;
static link_t g_link[AT_SOCKET_NUM];
static netconn_data_input_cb_t g_netconn_data_input_cb;
static netconn_close_cb_t g_netconn_close_cb;
static sal_op_t sim800_sal_driver;

extern void sal_init(void);

/*****************************************
* common driver interface
******************************************/
static aos_dev_t *sim800_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(gprs_dev_t), id);

    return dev;
}

#define sim800_dev_uninit device_free

static int sim800_dev_open(aos_dev_t *dev)
{
    // power on device
    sal_module_register(&sim800_sal_driver);

    sal_init();

    return 0;
}

static int sim800_dev_close(aos_dev_t *dev)
{
    //power off device
    return 0;
}

/*****************************************
* common netif driver interface
******************************************/
static int sim800_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    return 0;
}

static int sim800_start_dhcp(aos_dev_t *dev)
{
    return 0;
}

static int sim800_stop_dhcp(aos_dev_t *dev)
{
    return 0;
}

static int sim800_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return 0;
}

static int sim800_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    int ret ;
    char ip[16];

    ret = sim800_get_local_ip(ip);
    // ret = sim800_link_info(ip);

    if (ret < 0) {
        LOGE(TAG, "gprs get_local_ip fail");
        return -1;
    }

    ipaddr_aton(ip, (ip4_addr_t *)ipaddr);

    return ret;
}

static int sim800_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

/*****************************************
* gprs driver interface
******************************************/
int sim800_set_mode(aos_dev_t *dev, gprs_mode_t mode)
{
    int ret = sim800_set_gprs_mode(mode);

    if (ret) {
        LOGE(TAG, "sim800 set mode failed");
        return -1;
    }

    ret = sim800_enable_muti_sockets(1);

    if (ret) {
        LOGE(TAG, "sim800 muti sock failed");
        return -1;
    }

    return ret;
}

int sim800_reset(aos_dev_t *dev)
{
    int ret;

    ret = sim800_get_state_pin();
    LOGD(TAG, "sim800 state pin:%d",ret);

    if(ret == 0) {
        ret = sim800_hard_reset();

        if (ret < 0) {
            LOGE(TAG, "sim800 reset failed");
            return -1;
        }

        ret = sim800_get_state_pin();
        LOGD(TAG, "sim800 state2 pin:%d",ret);
        if(ret < 0) {
            return -1;
        }
    } else if (ret == 1) {
        ret = sim800_hard_reset();

        if (ret < 0) {
            LOGE(TAG, "sim800 reset failed");
            return -1;
        }

        ret = sim800_get_state_pin();
        LOGD(TAG, "sim800 state3 pin:%d",ret);
        if(ret == 1) {
            return -1;
        }

        ret = sim800_hard_reset();

        if (ret < 0) {
            LOGE(TAG, "sim800 reset failed");
            return -1;
        }

        ret = sim800_get_state_pin();
        LOGD(TAG, "sim800 state4 pin:%d",ret);
        if(ret < 0) {
            return -1;
        }
    } else {
        return -1;
    }

    LOGE(TAG, "sim800 reset ok");
    return 0;
}

int sim800_init_check(aos_dev_t *dev)
{
    sig_qual_resp_t  signal_quality;
    regs_stat_resp_t reg_stat;
    int     server_stat = -1;
    int retry = 0;
    int ret;

    ret = sim800_get_state_pin();
    if(ret <= 0) {
        sim800_hard_reset();
    }

    retry = 3;
    do {
        ret = sim800_close_echo();
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "sim800 close echo failed");
        goto check_err;
    }

    retry = 3;
    ret = sim800_check_simcard_is_insert(0);

    if (ret < 0) {
        LOGE(TAG, "simcard is not inside");
        goto check_err;
    }

    retry = 10;
    do {
        ret = sim800_check_signal_quality(&signal_quality, 0);

        if (ret < 0 || signal_quality.rssi <= 0 || signal_quality.rssi == 99) {
            sleep(1);
        }
    } while (signal_quality.rssi <= 0 && retry--);

    if(retry == 0) {
        LOGI(TAG, "gprs sig_qua err(%d)", signal_quality.rssi);
        goto check_err;
    }

    retry = 10;
    do {
        ret = sim800_network_shut(0);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "sim800 shut failed");
        goto check_err;
    }

    retry = 10;
    do {
        ret = sim800_check_register_status(&reg_stat, 0);

        if (reg_stat.stat != 1) {
            sleep(1);
        }
    } while (reg_stat.stat != 1 && retry--);

    if(retry == 0) {
        LOGI(TAG, "gprs reg status err");
        goto check_err;
    }

    retry = 10;
    do {
        ret = sim800_check_gprs_service_status(&server_stat, 0);

        if (server_stat != 1) {
            sleep(1);
        }
    } while (server_stat != 1 && retry--);

    if(retry == 0) {
        LOGI(TAG, "gprs server state err");
        goto check_err;
    }

    return 0;
check_err:
    event_publish(EVENT_GPRS_LINK_DOWN, NULL);
    return -1;
}

int sim800_connect_to_gprs(aos_dev_t *dev)
{
    int ret;
    int retry;

    retry = 10;
    do {
        ret = sim800_wireless_set_apn(0);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs wireless_set_apn fail");
        goto connect_err;
    }

    retry = 10;
    do {
        ret = sim800_connect_to_network(0);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs connect_to_network fail");
        goto connect_err;
    } else {
        event_publish(EVENT_GPRS_LINK_UP, NULL);
    }

    retry = 10;
    do {
        ret = sim800_define_pdp_context(1, 0);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs sim800_define_pdp_context fail");
        goto connect_err;
    }

    retry = 10;
    do {
        ret = sim800_recv_show_head(1);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs set show_head fail");
        goto connect_err;
    }

    retry = 10;
    do {
        ret = sim800_recv_show_remote_ip(1);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs sim800_recv_show_remote_ip fail");
        goto connect_err;
    }

    retry = 5;
    do {
        char ip[16];

        ret = sim800_link_info(ip);
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "gprs sim800_link_info fail");
        goto connect_err;
    }

    event_publish(EVENT_NET_GOT_IP, NULL);
    // event_publish_delay(EVENT_NET_GOT_IP, NULL, 2000);

    return ret;
connect_err:
    event_publish(EVENT_GPRS_LINK_DOWN, NULL);
    return -1;
}

int sim800_disconnect_gprs(aos_dev_t *dev)
{
    int ret = -1;

    ret = sim800_network_shut(0);

    if (ret == 0) {
        event_publish(EVENT_GPRS_LINK_DOWN, NULL);
    }

    return ret;
}

int sim800_ping_remote(aos_dev_t *dev, int type, char *remote_ip)
{
    int retry = 0;
    int i;

    for (i = 0; i < 10; i++) {
        if (sim800_ping(remote_ip, i + 1) < 0) {
            if ((retry++) >= 3) {
                break;
            }
        }
    }

    return 0;
}

int sim800_drv_get_link_status(aos_dev_t *dev, gprs_status_link_t *link_status)
{
    return sim800_get_link_status();
}

int sim800_get_link_info(aos_dev_t *dev, char ip[16], char gw[16], char mask[16])
{
    return sim800_link_info(ip);
}

int sim800_get_csq(aos_dev_t *dev, int *csq)
{
    int ret;
    sig_qual_resp_t signal_quality;

    ret = sim800_check_signal_quality(&signal_quality, 0);

    if(ret == 0) {
        *csq = signal_quality.rssi;
    }

    return ret;
}

int sim800_get_simcard_info(aos_dev_t *dev, char ccid[21], int *insert)
{
    int ret = sim800_check_simcard_is_insert(0);

    if(ret == 0) {
        *insert = 0;
        ret = sim800_get_ccid(ccid);
    } else {
        *insert = -1;
    }
    return ret;
}
/****************************************
*
* sim800_gprs adaptation interface
*
*****************************************/
static int fd_to_linkid(int fd)
{
    int link_id;

    if (aos_mutex_lock(&g_link_mutex, AOS_WAIT_FOREVER) != 0) {
        LOGE(TAG, "Failed to lock mutex.");
        return -1;
    }

    for (link_id = 0; link_id < AT_SOCKET_NUM; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    aos_mutex_unlock(&g_link_mutex);

    return link_id;
}

static void sim800_gprs_socket_data_handle(int linkid, void *data, size_t len, char remote_ip[16], uint16_t remote_ports)
{
    /*
    +RECEIVE,0,44,106.15.213.199:5684:
    */
    if (g_netconn_data_input_cb && ((g_link[linkid].fd >= 0))) {
        if (g_netconn_data_input_cb(g_link[linkid].fd, data, len, remote_ip, remote_ports)) {
            LOGE(TAG, "socket %d get data len %d fail, drop it",
                 g_link[linkid].fd, len);
        }
    }

    return;
}

static void sim800_gprs_close_handle(int linkid)
{
    /*
    0,CLOSED:
    */
    if (g_netconn_close_cb && ((g_link[linkid].fd >= 0))) {
        if (g_netconn_close_cb(g_link[linkid].fd)) {
            LOGE(TAG, "socket %d close fail");
        }
        g_link[linkid].fd = -1;
    }

    return;
}

int sim800_link_status(int *socket_stat)
{
    static int now_stat = -1;
    int i;

    if (now_stat != g_link_status) {
        g_link_status = sim800_get_link_status();
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

int aos_sim800_get_link_status(void)
{
    return sim800_link_status(NULL);
}

int sim800_gprs_got_ip(const char *ssid, const char *psw)
{
    sim800_link_info(net_ipaddr);

    LOGD(TAG, "sim800 got ip %s", net_ipaddr);
    /*delay 5 seconds to post got ip event*/
    //aos_post_delayed_action(100, sim800_get_ip_delayed_action, NULL);
    // aos_post_event(EV_NET, CODE_NET_ON_CONNECTED, VALUE_NET_WIFI);
    return 0;
}

/*****************************************
* sim800 SAL interface
******************************************/
static int sim800_gprs_module_init(void)
{
    int i;

    if (aos_mutex_is_valid(&g_link_mutex)) {
        LOGI(TAG, "sim800 have already inited");
        return 0;
    }

    for (i = 0; i < AT_SOCKET_NUM; i++) {
        g_link[i].fd = -1;
    }

    if (0 != aos_mutex_new(&g_link_mutex)) {
        LOGE(TAG, "Creating link mutex failed.");
        goto err;
    }

    sim800_packet_input_cb_register(&sim800_gprs_socket_data_handle);

    sim800_close_cb_register(&sim800_gprs_close_handle);

    return 0;
err:

    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }

    return -1;
}


static int sim800_gprs_module_deinit()
{
    if (aos_mutex_is_valid(&g_link_mutex)) {
        aos_mutex_free(&g_link_mutex);
    }
    return 0;
}

static int sim800_gprs_module_domain_to_ip(char *domain, char ip[16])
{
    if (NULL == domain || NULL == ip) {
        return -1;
    }

    char *c = domain;
    int num = 0;
    while (*c) {
         if ( !isalnum(*c) && *c != '.') {
             break;
         }
         if (*c == '.')
            num++;
         c++;

    }


    if (*c == 0 && num == 3) {
        strcpy(ip, domain);
    } else {
        if (sim800_domain_to_ip(domain, ip) < 0) {
            return -1;
        }
    }

    return 0;
}

static int sim800_gprs_module_conn_start(sal_conn_t *conn)
{
    int  linkid = 0;
    int ret = -1;
    net_conn_e net_type = 0;

    if (!conn || !conn->addr) {
        LOGE(TAG, "connect invalid input");
        return -1;
    }

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
        LOGE(TAG, "No link available for now.");
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

    ret = sim800_connect_remote(linkid, net_type, conn->addr, conn->r_port);

    if (ret < 0) {
        LOGE(TAG, "conn_start fail");
        return -1;
    }

    return 0;
}

static int sim800_gprs_module_conn_close(int fd, int32_t remote_port)
{
    int  linkid = 0;
    int  ret = 0;

    linkid = fd_to_linkid(fd);

    if (linkid >= AT_SOCKET_NUM || linkid < 0) {
        LOGE(TAG, "No connection found for fd (%d)", fd);
        return -1;
    }

    ret = sim800_close(linkid);

    g_link[linkid].fd = -1;

    return ret;
}

static int sim800_gprs_module_send(int fd, uint8_t *data, uint32_t len,
                                   char remote_ip[16], int32_t remote_port, int32_t timeout)
{
    #define SIM800_MIN_TIMOUT (3000)

    int  linkid;
    int  ret = -1;

    linkid = fd_to_linkid(fd);

    if (linkid >= AT_SOCKET_NUM) {
        LOGE(TAG, "No connection found for fd (%d)", fd);
        return -1;
    }

    int send_len     = 0;
    int offset       = 0;
    int sendable_len = 0;
    long long time_th = timeout > SIM800_MIN_TIMOUT?aos_now_ms() + timeout:aos_now_ms() + SIM800_MIN_TIMOUT;
    int remain_time;

    while (1) {
        remain_time = (int)(time_th - aos_now_ms());
        if(remain_time <= 0) {
            LOGE(TAG, "sim800 send timout1(%d),remain:%d", timeout, remain_time);
            return -1;
        }

        ret = sim800_get_send_len(linkid, &sendable_len, remain_time);

        send_len = (sendable_len + offset) > len? (len - offset) : sendable_len;
        if(ret >= 0 && sendable_len > 0) {
            remain_time = (int)(time_th - aos_now_ms());
            if(remain_time <= 0) {
                LOGE(TAG, "sim800 send timout2(%d) remain:%d", timeout, remain_time);
                return -1;
            }

            ret = sim800_send_data(linkid, data + offset, send_len, remain_time);

            if (ret < 0) {
                LOGE(TAG, "sim800 send failed --> time:%d remain:%d", timeout, (int)(time_th - aos_now_ms()));
                return -1;
            } else {
                offset += send_len;
            }

            if(offset == len) {
                ret = 0;
                break;
            }
        } else {
            sleep(1);
            remain_time = (int)(time_th - aos_now_ms());
            if(remain_time <= 0) {
                LOGE(TAG, "sim800 send timout3(%d) remain:%d", timeout, remain_time);
                return -1;
            }
        }
    }

    return ret;
}

static int sim800_gprs_packet_input_cb_register(netconn_data_input_cb_t cb)
{
    if (cb) {
        g_netconn_data_input_cb = cb;
    } else {
        LOGE(TAG, "sim800 input cb register err");
    }

    return 0;
}

static int sim800_gprs_close_cb_register(netconn_close_cb_t cb)
{
    if (cb) {
        g_netconn_close_cb = cb;
    } else {
        LOGE(TAG, "esp8266 close cb register err");
    }

    return 0;
}

static net_ops_t sim800_net_driver = {
    .set_mac_addr = sim800_set_mac_addr,
    .start_dhcp = sim800_start_dhcp,
    .stop_dhcp = sim800_stop_dhcp,
    .set_ipaddr = sim800_set_ipaddr,
    .get_ipaddr = sim800_get_ipaddr,
    .subscribe  = sim800_subscribe,
    .ping       = sim800_ping_remote,
};

static gprs_driver_t sim800_gprs_driver = {
    .reset = sim800_reset,
    .set_mode = sim800_set_mode,
    .module_init_check = sim800_init_check,
    .connect_to_gprs = sim800_connect_to_gprs,
    .disconnect_from_gprs = sim800_disconnect_gprs,
    .get_link_status = sim800_drv_get_link_status,
    .get_csq        = sim800_get_csq,
    .get_simcard_info = sim800_get_simcard_info,
};

static sal_op_t sim800_sal_driver = {
    .version = "1.0.0",
    .init = sim800_gprs_module_init,
    .start = sim800_gprs_module_conn_start,
    .send = sim800_gprs_module_send,
    .domain_to_ip = sim800_gprs_module_domain_to_ip,
    .close = sim800_gprs_module_conn_close,
    .deinit = sim800_gprs_module_deinit,
    .register_netconn_data_input_cb = sim800_gprs_packet_input_cb_register,
    .register_netconn_close_cb = sim800_gprs_close_cb_register,
};

static netdev_driver_t sim800_driver = {
    .drv = {
        .name   = "gprs",
        .init   = sim800_dev_init,
        .uninit = sim800_dev_uninit,
        .open   = sim800_dev_open,
        .close  = sim800_dev_close,
    },
    .link_type = NETDEV_TYPE_GPRS,
    .net_ops =  &sim800_net_driver,
    .link_ops = &sim800_gprs_driver,
};

void gprs_sim800_register(utask_t *task, sim_gprs_param_t *param)
{
    int ret = sim800_module_init(task, param);

    if (ret < 0) {
        LOGI(TAG, "sim800 driver init error");
        return;
    }

    //run sim800_dev_init to create gprs_dev_t and bind this driver
    ret = driver_register(&sim800_driver.drv, param, 0);

    if (ret < 0) {
        LOGI(TAG, "sim800 device register error");
    }
}

