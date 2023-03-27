/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/yloop.h>

#include <yoc/atparser.h>
#include <yoc/netmgr.h>
#include <devices/netdrv.h>
#include <drv/gpio.h>
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#include "devices/w800.h"
#include "w800_api.h"

static atparser_uservice_t *g_atparser_uservice_t;
static int                  g_net_status = 0;
static net_data_input_cb_t  g_net_data_input_cb;
static net_close_cb_t       g_net_close_cb;
static aos_mutex_t          g_cmd_mutex;
static csi_gpio_pin_t       g_reset_pin;
#define TAG "w800_api"

static int _connect_error_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int connect_errno = atoi(oob_data->buf);
    //LOGE(TAG, "+cwjap oob  %d", connect_errno);

    switch (connect_errno) {

        case 1 :
            LOGW(TAG, "connect ap timeout");
            //connect_errno = NET_DISCON_REASON_WIFI_TIMEOUT;
            break;

        case 2 :
            LOGW(TAG, "connect ap password err");
            //connect_errno = NET_DISCON_REASON_WIFI_PSK_ERR;
            break;

        case 3 :
            LOGW(TAG, "connect ap not exist");
            //connect_errno = NET_DISCON_REASON_WIFI_NOEXIST;
            break;

        default:
            //connect_errno = NET_DISCON_REASON_ERROR;
            LOGW(TAG, "connect ap fail %d", connect_errno);
            break;
    }

    oob_data->used_len = 1;
    g_net_status = NET_STATUS_LINKDOWN;

    event_publish(EVENT_WIFI_LINK_DOWN, NULL);

    return 0;
}

#define TIME_OUT 180 //ms

typedef struct ipd {
    int linkid;
    int rlen;
    char remote_ip[16];
    uint16_t remote_port;
    char *data;
    size_t data_size;
} ipd_t;

static int ipd_put(ipd_t *ipd, char *data, int len)
{
    if(ipd->data == NULL) {
        ipd->data = aos_malloc(ipd->rlen);
        if(ipd->data == NULL) {
            LOGW(TAG, "IPD mem err\n");
        }
    }
    int copy_len;
    if ((ipd->data_size + len) > ipd->rlen) {
        copy_len = ipd->rlen - ipd->data_size;
    } else {
        copy_len = len;
    }
    memcpy(&ipd->data[ipd->data_size], data, copy_len);
    ipd->data_size += copy_len;

    return copy_len;
}

static void ipd_clear(ipd_t *ipd)
{
    free(ipd->data);
    memset(ipd, 0, sizeof(ipd_t));
}

static int _recv_data_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    //+EVENT=RECV,0,512,106.15.213.199,5684:
    static ipd_t ipd;

    // oob timeout
    if (aos_now_ms() > (oob_data->time_stamp + TIME_OUT)) {
        ipd_clear(&ipd);
        return 0;
    }

    if (ipd.rlen == 0) { // first data
        char *str = strchr(oob_data->buf, ':');
        if (str  != NULL) {
            sscanf(oob_data->buf, "%d,%d,%[^,],%d:%*s", &ipd.linkid, &ipd.rlen, ipd.remote_ip, (int*)&ipd.remote_port);
            oob_data->used_len = ipd_put(&ipd, str+1, oob_data->len - (str - oob_data->buf+1));
            //LOGI(TAG,"%s(%d)",oob_data->buf,oob_data->used_len);
            oob_data->used_len += (str - oob_data->buf+1);
        }
    } else {
        oob_data->used_len = ipd_put(&ipd, oob_data->buf, oob_data->len);
    }

    if (ipd.data_size == ipd.rlen ) {
        if (g_net_data_input_cb) {
            g_net_data_input_cb(ipd.linkid, ipd.data, ipd.rlen, ipd.remote_ip, ipd.remote_port);
        }
        ipd_clear(&ipd);
        return 0;
    }

    /* recv data is not enough need recv more */
    return -1;

}

static int _closed0_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    LOGD(TAG, "socket[0] closed\n");

    int linkid = 0;

    if (g_net_close_cb) {
        g_net_close_cb(linkid);
    }

    return  0;
}

static int _closed1_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    LOGD(TAG, "socket[1] closed\n");

    int linkid = 1;

    if (g_net_close_cb) {
        g_net_close_cb(linkid);
    }

    return 0;
}

static int _closed2_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    LOGD(TAG, "socket[2] closed\n");

    int linkid = 2;

    if (g_net_close_cb) {
        g_net_close_cb(linkid);
    }

    return 0;
}

static int _apconnect_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    if (g_net_status >= NET_STATUS_READY) {
        g_net_status = NET_STATUS_LINKUP;
        event_publish(EVENT_WIFI_LINK_UP, NULL);
    }

    return 0;
}

static int _disconnect_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    LOGE(TAG, "dis handle: %d\n", g_net_status);

    if (g_net_status > NET_STATUS_LINKDOWN) {
        event_publish(EVENT_WIFI_LINK_DOWN, NULL);
    }

    g_net_status = NET_STATUS_LINKDOWN;

    return 0;
}

static int _gotip_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    if (g_net_status >= NET_STATUS_READY) {
        g_net_status = NET_STATUS_GOTIP;
        event_publish(EVENT_NET_GOT_IP, NULL);
    }

    return 0;
}

static int _config_param_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int store_order = (int)priv;
    static char wifi_ssid[64] = {0};
    static char wifi_psw[64] = {0};
    int len = 0;
    char *buf = oob_data->buf;

    LOGD(TAG, "WIFI param store len=%d", len);

    if (store_order == 0) {
        sscanf(buf, "%*[^\r\n]%n", &len);
        if(len == 0) {
            return -1;
        }
        if (len < sizeof(wifi_ssid)) {
            sscanf(buf, "%[^\r\n]", wifi_ssid);
        } else {
            LOGE(TAG, "fmt");
        }
    } else {
        sscanf(buf, "%*[^\r\n]%n", &len);
        if(len == 0) {
            return -1;
        }
        if (len < sizeof(wifi_psw)) {
            sscanf(buf, "%[^\r\n]", wifi_psw);
            netmgr_hdl_t hdl = netmgr_get_handle("wifi");
            LOGI(TAG, "apconfig ssid:%s, psw:%s\n", wifi_ssid, wifi_psw);
            netmgr_config_wifi(hdl, wifi_ssid, strlen(wifi_ssid), wifi_psw, strlen(wifi_psw));
        } else {
            LOGE(TAG, "fmt");
        }
    }
    oob_data->used_len = len;

    return 0;
}

/*********************************************************************

w800 api

**********************************************************************/
int w800_ap_info(char ssid[32], int bssid[6], int *channel, int *rssi)
{
    int ret = -1;

    if (g_net_status < NET_STATUS_LINKUP) {
        memset(ssid, 0, 32);
        memset(bssid, 0, 18);
        *rssi = -1;
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    atparser_set_timeout(g_atparser_uservice_t,8*1000);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPGETWIFIINFO") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPGETWIFIINFO:bssid:%x:%x:%x:%x:%x:%x",
            &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CIPGETWIFIINFO:ssid:%s", ssid) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CIPGETWIFIINFO:channel:%d", channel) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CIPGETWIFIINFO:signal:%d", rssi) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
            printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\r\n", 
                    bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_ap_connect(const char *ap, const char *passPhrase)
{
    int ret = -1;
    g_net_status = NET_STATUS_READY;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_set_timeout(g_atparser_uservice_t, 30 * 1000);
    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+WJAP=%s,%s", ap, passPhrase) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_set_timeout(g_atparser_uservice_t, 8 * 1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_at0(void)
{
    int ret = -1;
    int count = 3;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT") == 0) {
        while (count --) {
            if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
                ret = 0;
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_software_reset(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    atparser_set_timeout(g_atparser_uservice_t,100);

    if (atparser_send(g_atparser_uservice_t, "AT+RST") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_set_timeout(g_atparser_uservice_t, 8000);
    
    ret = -1;
    if (atparser_recv(g_atparser_uservice_t, "+EVENT=READY") == 0) {
        ret = 0;
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    if(ret == 0) {
        LOGD(TAG, "WIFI SOFT RESET OK\n");
    }
    return ret;
}

int w800_hard_reset(void)
{
    int ret = -1;

    // atparser_debug_control(g_atparser_uservice_t,0);

    // gpio_pin_handle_t reset_pin_hd;
    // reset_pin_hd = csi_gpio_pin_initialize(g_reset_pin, NULL);
    // csi_gpio_pin_config(reset_pin_hd, GPIO_MODE_OPEN_DRAIN, GPIO_DIRECTION_OUTPUT);
    // csi_gpio_pin_write(reset_pin_hd, 0);
    // aos_msleep(50);
    // csi_gpio_pin_write(reset_pin_hd, 1);
    // g_net_status = NET_STATUS_UNKNOW;

    // ret = w800_reset_hook("test");
    // if(ret == 0) {
    //     LOGD(TAG, "WIFI HARD RESET OK\n");
    // }
    return ret;
}

int w800_close_echo(void)
{
    int ret = -1;
    int count = 3;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "ATE0") == 0) {
        while (count --) {
            if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
                ret = 0;
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    atparser_debug_control(g_atparser_uservice_t,1);
    return ret;
}

int w800_get_remote_info(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPDINFO=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_ap_disconnect(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CWQAP") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_enable_muti_sockets(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPMUX=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_set_wifi_mode(int mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CWMODE_DEF=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int w800_get_mac(rvm_dev_t *dev, uint8_t *out_mac)
{
    int ret = -1;
    int mac[6];

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSTAMAC") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPSTAMAC:%x:%x:%x:%x:%x:%x\n", 
                &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 0)) {
            out_mac[0] = mac[0];
            out_mac[1] = mac[1];
            out_mac[2] = mac[2];
            out_mac[3] = mac[3];
            out_mac[4] = mac[4];
            out_mac[5] = mac[5];
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int w800_get_status(int *stat)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPGETSTATUS") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPGETSTATUS:%d\n", stat) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
            LOGD(TAG, "link status %d\r\n", *stat);
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_get_link_status(void)
{
    static int stat = -1;

    if (g_net_status == stat) {
        return g_net_status;
    }

    if (g_net_status == NET_STATUS_UNKNOW) {
        g_net_status = NET_STATUS_LINKDOWN;
        stat = g_net_status;
        return g_net_status;
    }

    if (w800_get_status(&stat) == 0) {
        switch (stat) {
            case 2:
                g_net_status = NET_STATUS_GOTIP;
                break;

            case 3:
                g_net_status = NET_STATUS_GOTIP;
                break;

            case 4:
                g_net_status = NET_STATUS_GOTIP;
                break;

            case 5:
                g_net_status = NET_STATUS_LINKDOWN;
                break;

            default:
                break;
        }

        stat = g_net_status;
        return g_net_status;
    }

    return -1;
}

int w800_domain_to_ip(char *domain, char ip[16])
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        memset(ip, 0, 16);
        LOGE(TAG, "domain status = %d\n", g_net_status);
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    atparser_set_timeout(g_atparser_uservice_t, 30*1000);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPDOMAIN=%s", domain) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPDOMAIN:%s\n", ip) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
            LOGE(TAG, "domain to ip: %s", ip);
        } else {
            LOGE(TAG, "domain resp fail\n");
        }
    }
    atparser_set_timeout(g_atparser_uservice_t, 8*1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int w800_link_info(char ip[16], char gw[16], char mask[16])
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        strcpy(ip, "0.0.0.0");
        strcpy(gw, "0.0.0.0");
        strcpy(mask, "0.0.0.0");
        LOGE(TAG, "link info status = %d\n", g_net_status);
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSTA") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPSTA:ip:%s", ip) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CIPSTA:gateway:%s", gw) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CIPSTA:netmask:%s", mask) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}

int w800_ping(const char *ip, uint8_t seq_num)
{
    int ret = -1;
    int ping_time;

    if (g_net_status < NET_STATUS_GOTIP) {
        LOGE(TAG, "net status error\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+PING=%s", ip) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+PING:%d", &ping_time) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
        } else {
            printf("\t from %s: Destination Host Unreachable\r\n", ip);
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    if (ret == 0) {
        printf("\t from %s: icmp_seq=%d time=%dms\r\n", ip, seq_num, ping_time);
    }

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port)
{
    int ret = -1;
    int ret_id;
    int local_port = 1838;

    if (g_net_status < NET_STATUS_GOTIP) {
        LOGE(TAG, "net status error\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    switch (type) {
        case NET_TYPE_TCP_SERVER:
            /* TCP Server can NOT ignore lport */
            break;

        case NET_TYPE_UDP_UNICAST:
            ret = atparser_send(g_atparser_uservice_t, "AT+CIPSTART=%d,%s,%s,%d,%d", id, "udp_unicast", srvname, port, local_port);
            break;
        case NET_TYPE_TCP_CLIENT:
            ret = atparser_send(g_atparser_uservice_t, "AT+CIPSTART=%d,%s,%s,%d", id, "tcp_client", srvname, port);
            break;

        default:
            LOGE(TAG, "type=%d err!", type);
            return -1;

    }

    if (ret == 0) {
        ret = -1;

        if ((atparser_recv(g_atparser_uservice_t, "OK\n") == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+EVENT=CONNECT,%d\n", &ret_id) == 0)) {
            if (ret_id == id) {
                ret = 0;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_choose_id(int id)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPID=%d", id) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        } else {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
int w800_send_data(const uint8_t *pdata, int len,int timeout)
{
    int ret = -2;

    if (g_net_status < NET_STATUS_GOTIP) {
        LOGE(TAG, "net status error\n");
        return -1;
    }

    LOGD(TAG, "w800 start send(%d)(time:%d)\n", len, timeout);
    
    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    atparser_set_timeout(g_atparser_uservice_t, timeout);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSEND=%d", len) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {

            atparser_write(g_atparser_uservice_t, pdata, len);
            if ( atparser_recv(g_atparser_uservice_t, "Recv %*d bytes") == 0 && \
                    atparser_recv(g_atparser_uservice_t, "+CIPSEND:%d", &ret) == 0){
                
                if (ret >= 0) {
                    ret = 0;
                } else {
                    ret = -1;
                }
            } else {
                ret = -3;
            }
        } else {
            LOGE(TAG, "send no resp\n");
        }
    } else {
        ret = -2;
    }

    LOGD(TAG, "w800 end send(ret:%d)\n", ret);
    atparser_set_timeout(g_atparser_uservice_t, 8*1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;;
}

int w800_packet_input_cb_register(net_data_input_cb_t cb)
{
    if (cb) {
        g_net_data_input_cb = cb;
    } else {
        LOGE(TAG, "packet input cb error\n");
    }

    return 0;
}

int w800_close_cb_register(net_close_cb_t cb)
{
    if (cb) {
        g_net_close_cb = cb;
    } else {
        LOGE(TAG, "packet input cb error\n");
    }

    return 0;
}

int w800_close(int id)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSTOP=%d", id) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        } else {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int w800_uart_config(uint32_t baud, uint8_t flow_control)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+UART_CUR=%d,8,1,0,%d", baud, flow_control) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK\n") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

void w800_set_timeout(int ms)
{
    atparser_set_timeout(g_atparser_uservice_t, ms);
}

static int w800_module_inited = 0;

extern at_channel_t spi_channel;
int w800_module_init(utask_t *task, w800_wifi_param_t *param)
{
    if (w800_module_inited) {
        return 0;
    }

    if (task == NULL) {
        task = utask_new("w800", 7 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);
    }

    if (task == NULL) {
        return -1;
    }

    if (param->reset_pin) {
        csi_gpio_pin_init(&g_reset_pin, param->reset_pin);
        csi_gpio_pin_mode(&g_reset_pin,GPIO_MODE_PULLUP);
        csi_gpio_pin_dir(&g_reset_pin,GPIO_DIRECTION_OUTPUT);
        csi_gpio_pin_write(&g_reset_pin, GPIO_PIN_LOW);
        aos_msleep(200);
        csi_gpio_pin_write(&g_reset_pin, GPIO_PIN_HIGH);
        aos_msleep(1000);
        LOGD(TAG, "hard reset");
    }

    g_atparser_uservice_t = atparser_channel_init(task, NULL, param, &spi_channel);
    aos_mutex_new(&g_cmd_mutex);

    atparser_debug_control(g_atparser_uservice_t, 1);
    atparser_oob_create(g_atparser_uservice_t, "2,CLOSED", _closed2_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "1,CLOSED", _closed1_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "0,CLOSED", _closed0_handler, NULL);

    atparser_oob_create(g_atparser_uservice_t, "+EVENT=NET,LINK_UP", _gotip_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+EVENT=NET,LINK_DOWN", _disconnect_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+EVENT=RECV,", _recv_data_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+EVENT=DISCONNECT,0", _closed0_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+EVENT=DISCONNECT,1", _closed1_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+EVENT=DISCONNECT,2", _closed2_handler, NULL);

    w800_module_inited = 1;

    return 0;
}

