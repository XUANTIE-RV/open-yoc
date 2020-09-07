/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/yloop.h>

#include <yoc/atparser.h>
#include <drv/gpio.h>
#include <devices/gprs.h>
#include <devices/netdrv.h>
#include <devices/hal/gnss_impl.h>
#include <devices/m6313.h>

#include "m6313_api.h"

static atparser_uservice_t *g_atparser_uservice_t;
static int g_net_status = 0;
static net_data_input_cb_t g_net_data_input_cb;
static net_close_cb_t g_net_close_cb;
static aos_mutex_t g_cmd_mutex;
static int g_powerkey_pin = -1;
static int g_power_on_pin = -1;
static aos_event_t g_event;
static recv_sms_cb g_recv_sms_cb = NULL;

#define TAG "m6313_api"

#define TIME_OUT 180 //ms

#define IPD_STAT_GOTNONE  0
#define IPD_STAT_GOTLEN   1
#define IPD_STAT_GOTIP    2
#define IPD_STAT_GOTDATA  3

#define EVENT_SMS_RECV 0x20000001

#define EVENT_CREG_1 (0x1 << 1)
#define EVENT_CREG_2 (0x1 << 2)
#define EVENT_CREG_3 (0x1 << 3)
#define EVENT_CREG_4 (0x1 << 4)
#define EVENT_CREG_5 (0x1 << 5)

typedef struct ipd {
    int linkid;
    int rlen;
    char remote_ip[16];
    uint16_t remote_port;
    char *data;
    size_t data_size;
    int stat;
} ipd_t;

static int ipd_put(ipd_t *ipd, char *data, int len)
{
    if (ipd->data == NULL) {
        ipd->data = aos_malloc(ipd->rlen);

        if (ipd->data == NULL) {
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
    // +RECEIVE: 0, 1280
    // RECV FROM:106.15.100.2:1883
    // IPD1280:{data}
    static ipd_t ipd;
    char *ptr = oob_data->buf;
    int used_len;
    char *str;

    // oob timeout
    if (aos_now_ms() > (oob_data->time_stamp + TIME_OUT)) {
        ipd_clear(&ipd);
        return 0;
    }

    while (1) {
        switch (ipd.stat) {
            case IPD_STAT_GOTNONE :

                str = strchr(ptr, '\n');

                if (str == NULL) {
                    return -1;
                }

                sscanf(ptr, "%d,%d\r\n%n", &ipd.linkid, &ipd.rlen, &used_len);

                ptr += used_len;
                oob_data->used_len += used_len;
                ipd.stat = IPD_STAT_GOTLEN;
                break;

            case IPD_STAT_GOTLEN :
                str = strchr(ptr, '\n');

                if (str == NULL) {
                    return -1;
                }

                sscanf(ptr, "RECV FROM:%[^:]:%d\r\n%n",
                       ipd.remote_ip, &ipd.remote_port, &used_len);

                ptr += used_len;
                oob_data->used_len += used_len;
                ipd.stat = IPD_STAT_GOTIP;
                break;

            case IPD_STAT_GOTIP :
                str = strchr(ptr, ':');

                if (str == NULL) {
                    return -1;
                }

                sscanf(ptr, "IPD%*d:%n", &used_len);

                ptr += used_len;
                oob_data->used_len += used_len;
                ipd.stat = IPD_STAT_GOTDATA;
                break;

            case IPD_STAT_GOTDATA :
                oob_data->used_len += ipd_put(&ipd, ptr, oob_data->len - oob_data->used_len);

                if (ipd.data_size == ipd.rlen) {
                    if (g_net_data_input_cb) {

                        g_net_data_input_cb(ipd.linkid, ipd.data, ipd.rlen, ipd.remote_ip, ipd.remote_port);
                    }

                    ipd_clear(&ipd);
                    return 0;
                }

                return -1;

                break;

            default:
                ipd_clear(&ipd);
                return 0;
        }
    }

    /* recv data is not enough need recv more */
    return -1;

}

static int _closed_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int linkid = (int)(priv);

    LOGI(TAG, "socket[%d] closed", linkid);

    if (g_net_close_cb) {
        g_net_close_cb(linkid);
    }

    return 0;
}

static int _disconnect_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    if (g_net_status > NET_STATUS_LINKDOWN) {
        event_publish(EVENT_GPRS_LINK_DOWN, NULL);
    }

    g_net_status = NET_STATUS_LINKDOWN;

    return 0;
}

static int _creg_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int netlinkid = (int)(priv);

    LOGI(TAG, "creg[%d]", netlinkid);

    aos_event_set(&g_event, 0x1 << netlinkid, AOS_EVENT_OR);

    return 0;
}

static int _void_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    return 0;
}

static int _clean_line(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    char *ptr = oob_data->buf;
    char *str;

    str = strchr(ptr, '\n');

    if (str == NULL) {
        return -1;
    }

    oob_data->used_len = (str - ptr) + 1;

    return 0;
}

static int _recv_sms_int(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    char *ptr = oob_data->buf;
    char *str, *num;
    int index;

    str = strchr(ptr, '\n');

    if (str == NULL) {
        return -1;
    }

    num = strchr(ptr, ',');

    if (num) {
        index = atoi(num + 1);

        LOGD(TAG, "RECV SMS %d", index);

        event_publish(EVENT_SMS_RECV, (void*)index);
    }

    oob_data->used_len = (str - ptr) + 1;

    return 0;
}

static void _recv_sms(uint32_t event_id, const void *data, void *context)
{
    static char oa[20];

    if (event_id == EVENT_SMS_RECV) {
        char *content = aos_malloc_check(1024);
        int ret = m6313_sms_cmgr((int)data, oa, content, 1024);
        // printf("%s", content);
        if (ret == 0 && g_recv_sms_cb) {
            g_recv_sms_cb(oa, content, strlen(content));
        }
        free(content);
    }
}

/*********************************************************************

m6313 api

**********************************************************************/
int m6313_at0(void)
{
    int ret = -1;
    int count = 3;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT") == 0) {
        while (count --) {
            if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
                ret = 0;
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_power_on(void)
{
    if (g_power_on_pin < 0) {
        return -1;
    }

    gpio_pin_handle_t pin_hd;

    pin_hd = csi_gpio_pin_initialize(g_power_on_pin, NULL);
    csi_gpio_pin_config(pin_hd, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pin_hd, 0);
    aos_msleep(200);
    csi_gpio_pin_write(pin_hd, 1);

    return 0;
}

int m6313_powerkey_en(void)
{
    // int ret = -1;

    if (g_powerkey_pin < 0) {
        return -1;
    }

    /* clear the flag */
    aos_event_set(&g_event, 0x0, AOS_EVENT_AND);

    gpio_pin_handle_t reset_pin_hd;
    reset_pin_hd = csi_gpio_pin_initialize(g_powerkey_pin, NULL);
    csi_gpio_pin_config(reset_pin_hd, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(reset_pin_hd, m6313_POWEROFF);
    aos_msleep(2000);
    csi_gpio_pin_write(reset_pin_hd, m6313_POWERON);
    // sleep(8);
    g_net_status = NET_STATUS_UNKNOW;

    unsigned int actl_flags = 0;
    int ret = aos_event_get(&g_event, EVENT_CREG_1 | EVENT_CREG_3 | EVENT_CREG_4 | EVENT_CREG_5,
    AOS_EVENT_OR_CLEAR, &actl_flags, 25000);

    if (ret != 0) {
        LOGE(TAG, "aos_event_get timeout");
        return -1;
    }

    // LOGD("");

    switch (actl_flags) {
        case EVENT_CREG_1:
        case EVENT_CREG_5:
            return 0;
        case EVENT_CREG_3:
        case EVENT_CREG_4:
            return -1;
    };

    return 0;
}


int m6313_close_echo(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "ATE0") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "ATE0") == 0 &&
            atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_enable_muti_sockets(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QIMUX=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_set_gprs_mode(int mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QICSGP=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_get_status(char *str) //more data do handle
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QISTAT") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0) \
            && (atparser_recv(g_atparser_uservice_t, "STATE: %[^\r\n]", str) == 0)) {
            ret = 0;
        } else {
            ret = -2;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

typedef struct {
    char str[15];
    int state;
} m6313_STATUS_T;
int m6313_get_link_status(void)
{
    static int stat = -1;

    if (g_net_status == stat) {
        return g_net_status;
    }

    char str[15];
    int ret = m6313_get_status(str);

    if (ret == 0) {
        LOGD(TAG, "gprs state:%s", str);
        const m6313_STATUS_T m6313_state_table[] = {
            {"IP INITIAL", NET_STATUS_LINKDOWN},
            {"IP START", NET_STATUS_GOTIP},
            {"IP CONFIG", NET_STATUS_LINKUP},
            {"IP GPRSACT", NET_STATUS_GOTIP},
            {"IP STATUS", NET_STATUS_LINKUP},
            {"IP PROCESSING", NET_STATUS_GOTIP},
            {"PDP DEACT", NET_STATUS_LINKDOWN},
        };

        for (int i = 0; i < sizeof(m6313_state_table) / sizeof(m6313_STATUS_T); i++) {
            if (memcmp(str, m6313_state_table[i].str, strlen(m6313_state_table[i].str)) == 0) {
                g_net_status = m6313_state_table[i].state;
                break;
            }
        }

        stat = g_net_status;
        return g_net_status;
    } else {
        LOGD(TAG, "get status(%d)", ret);
    }

    return -1;
}

int m6313_get_local_ip(char ip[16])
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        strcpy(ip, "0.0.0.0");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QILOCIP") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "%s", ip) == 0) {
            ret = 0;
            g_net_status = NET_STATUS_GOTIP;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_domain_to_ip(char *domain, char ip[16])
{
    int ret = -1;
    char ip_temp[24];

    if (g_net_status < NET_STATUS_GOTIP) {
        memset(ip_temp, 0, sizeof(ip_temp));
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QIDNSGIP=\"%s\"", domain) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0) \
            //&& (atparser_recv(g_atparser_uservice_t, "\"%[^\"]\"", ip) == 0)) {
            && (atparser_recv(g_atparser_uservice_t, "%s", ip_temp) == 0)) {
            if (ip_temp[0] == '"') {
                memcpy(ip, ip_temp + 1, strlen(ip_temp) - 2);
            } else {
                memcpy(ip, ip_temp, strlen(ip_temp));
            }

            ret = 0;
        }
    } //+CDNSGIP:0,<dns error code>

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int m6313_link_info(char ip[16])
{
    int ret = -1;

    memset(ip, 0, 16);

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QILOCIP") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", ip) == 0)) {
            ret = 0;
            g_net_status = NET_STATUS_GOTIP;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}

int m6313_ping(const char *ip, uint8_t seq_num)
{
    int ret = -1;
    int ping_time;
    char ipaddr[16];

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QPING=\"%s\",5,1", ip) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+QPING: %*d,%[^,],%*d,%d,%*d",
                           ipaddr, &ping_time) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "+QPING: %*s") == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    if (ret == 0) {
        printf("\t from %s: icmp_seq=%d time=%dms\r\n", ipaddr, seq_num, ping_time);
    } else {
        printf("\t from %s: Destination Host Unreachable\r\n", ip);
    }

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port)
{
    int ret = -1;
    int ret_id;

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    switch (type) {
        case NET_TYPE_TCP_SERVER:
            /* TCP Server can NOT ignore lport */
            break;

        case NET_TYPE_TCP_CLIENT:
            ret = atparser_send(g_atparser_uservice_t, "AT+QIOPEN=%d,\"TCP\",\"%s\",%d", id, srvname, port);

            break;

        case NET_TYPE_UDP_UNICAST:
            ret = atparser_send(g_atparser_uservice_t, "AT+QIOPEN=%d,\"UDP\",\"%s\",%d", id, srvname, port);

            break;

        default:
            LOGE(TAG, "type=%d err!", type);
            return -1;

    }

    if (ret == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0) \
            && (atparser_recv(g_atparser_uservice_t, "%d, CONNECT OK", &ret_id) == 0)) {
            if (ret_id == id) {
                ret = 0;
            }
        }
    } // <n>,CONNECT FAIL

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

#if 0
int m6313_get_send_len(int id, int *len, int timeout)
{
    int ret = -1;
    int i ;
    int n;
    int send_len;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    m6313_set_timeout(timeout);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSEND?") == 0) {
        for (i = 0; i < 6; i++) {
            if (atparser_recv(g_atparser_uservice_t, "+CIPSEND:%d,%d", &n, &send_len) == 0) {
                if (n == id) {
                    *len = send_len;
                    ret = 0;
                }
            } else {
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    m6313_set_timeout(8 * 1000);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
#endif

int m6313_send_data(int id, const uint8_t *pdata, int len, int timeout)
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    m6313_set_timeout(timeout);
    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QISEND=%d,%d", id, len) == 0) {
        if (atparser_recv_str(g_atparser_uservice_t, "> ") == 0) {
            atparser_write(g_atparser_uservice_t, pdata, len);

            if (atparser_recv(g_atparser_uservice_t, "SEND OK") == 0) {
                ret = 0;
            }
        }
    }

    m6313_set_timeout(8 * 1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_packet_input_cb_register(net_data_input_cb_t cb)
{
    if (cb) {
        g_net_data_input_cb = cb;
    } else {
        LOGI(TAG, "packet input cb error\n");
    }

    return 0;
}

int m6313_close_cb_register(net_close_cb_t cb)
{
    if (cb) {
        g_net_close_cb = cb;
    } else {
        LOGE(TAG, "packet input cb error\n");
    }

    return 0;
}

int m6313_close(int id)
{
    int ret = -1;
    int rid = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QICLOSE=%d", id) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "%d,CLOSE OK", &rid) == 0) {
            if (rid == id) {
                ret = 0;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

/********************************************************
    GPRS API

********************************************************/

int m6313_check_simcard_is_insert(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CPIN?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CPIN: READY") == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_check_signal_quality(sig_qual_resp_t *respond, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CSQ") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CSQ:%d,%d", &respond->rssi, &respond->ber) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gprs_get_imsi(char *imsi)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIMI") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", imsi) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gprs_get_imei(char *imei)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGSN") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", imei) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
/*
int m6313_check_register_status(regs_stat_resp_t *respond, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CREG?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CREG:%d,%d", &respond->n, &respond->stat) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
*/

int m6313_check_gprs_service_status(int *sstat, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGATT?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CGATT:%d", sstat) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}


int m6313_set_gprs_service_status(int state, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGATT=%d", state) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}

#if 0
int m6313_wireless_set_apn(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CSTT=\"CMNET\"") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}
#endif

int m6313_connect_to_network()
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGACT=1,1") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int m6313_define_pdp_context(uint8_t mode, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGDCONT=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}

int m6313_reg_app(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    m6313_set_timeout(40 * 1000);

    if (atparser_send(g_atparser_uservice_t, "AT+QIREGAPP") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    if (ret) {
        return ret;
    }

    ret = -1;

    atparser_clr_buf(g_atparser_uservice_t);
    m6313_set_timeout(40 * 1000);

    if (atparser_send(g_atparser_uservice_t, "AT+QIACT") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_recv_show_head(uint8_t mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QIHEAD=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_set_ats(int time)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "ATS0=%d", time) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int m6313_recv_show_remote_ip(uint8_t mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QISHOWRA=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_network_shut(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    m6313_set_timeout(40 * 1000);

    if (atparser_send(g_atparser_uservice_t, "AT+QIDEACT") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "DEACT OK") == 0) {
            ret = 0;
        }
    }

    ret = 0;

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


/* SMS API */
int m6313_sms_cmcf(uint8_t mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CMGF=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_sms_csca(char *sca)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CSCA=\"%s\"", sca) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_sms_cmgs(char *da, char *content, uint32_t timeout)
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    char end = 0x1A;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    m6313_set_timeout(timeout);
    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CMGS=\"%s\"", da) == 0) {
        if (atparser_recv_str(g_atparser_uservice_t, "> ") == 0) {
            atparser_write(g_atparser_uservice_t, content, strlen(content));
            atparser_write(g_atparser_uservice_t, &end, 1);

            if ((atparser_recv(g_atparser_uservice_t, "+CMGS:%d", &ret) == 0) &&
                (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
                ret = 0;
            }
        }
    }

    m6313_set_timeout(8 * 1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_sms_cmgr(uint32_t index, char *da, char *content, uint32_t len)
{
    int ret = -1;
    char *ok;

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    memset(content, 0, len);

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CMGR=%d", index) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CMGR: \"REC UNREAD\",\"%[^\"]\"%*s", da) == 0)) {
            while(atparser_recv(g_atparser_uservice_t, "%s", content + strlen(content)) == 0) {
                    if ((ok = strstr(content, "OK")) != NULL) {
                        ret = 0;
                        *(ok - 2) = 0;
                        break;
                    }
                    content[strlen(content)] = '\r';
                    content[strlen(content)] = '\n';

                    // LOGI(TAG, "%s", data);
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

void m6313_sms_set_cb(recv_sms_cb cb)
{
    g_recv_sms_cb = cb;
}

/* GNSS API */
int m6313_gnss_switch(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QGNSSC=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_epc_switch(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QGNSSEPO=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_is_time_sync()
{
    int ret = 0;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QGNSSTS?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+QGNSSTS: %d", &ret) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_set_ref_location(double ref_latitude, double ref_longitude)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QGREFLOC=\"%llf\",\"%llf\"", ref_latitude, ref_longitude) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_set_lbskey(char *str)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+LBSKEY=\"%s\"", str) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_get_cell_location(double *latitude, double *longitude)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QCELLLOC=1") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+QCELLLOC: %llf,%llf", longitude, latitude) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_gnss_get_gnss_info(gnss_info_t *info)
{
    int ret = -1;

    double longitude, latitude, altitude, speed, orientation;
    char longitude_indicator, laitude_indicator;
    uint32_t position_indicator;
    uint32_t star_num, data;
    char hdop_str[8] = {0};
    char utc_str[16] = {0};
    char position_status;
    uint32_t gsv_num;


    /*
        +QGNSSRD: $GNGGA,014809.000,3017.4845,N,12006.7366,E,1,10,1.13,0.2,M,7.0,M,,*48
        $GPGSA,A,3,16,27,09,31,08,23,193,,,,,,1.42,1.13,0.86*33
        $BDGSA,A,3,28,16,08,,,,,,,,,,1.42,1.13,0.86*1C
        $GPGSV,4,1,14,27,88,063,12,195,72,101,,194,69,073,,08,53,220,09*72
        $GPGSV,4,2,14,16,50,027,14,23,45,253,17,26,32,059,,09,31,296,16*72
        $GPGSV,4,3,14,31,17,124,13,21,14,052,,07,14,315,26,11,09,198,*73
        $GPGSV,4,4,14,18,08,179,,193,08,168,21*4D
        $BDGSV,5,1,18,23,68,036,,07,64,358,,28,54,184,25,03,53,198,*60
        $BDGSV,5,2,18,10,50,308,,01,50,144,,09,46,227,,11,36,045,*64
        $BDGSV,5,3,18,02,36,234,,04,35,121,,16,33,200,29,25,31,104,*6B
        $BDGSV,5,4,18,06,30,193,,08,23,166,26,05,15,253,,27,07,207,*60
        $BDGSV,5,5,18,13,06,187,,20,05,286,*60
        $GNRMC,014809.000,A,3017.4845,N,12006.7366,E,1.002,251.48,300519,,,A*4A
        $GNVTG,251.48,T,,M,1.002,N,1.857,K,A*21
        $GNACC,22.2*78

        OK
    */

   if (info == NULL) {
       return -1;
   }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QGNSSRD?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+QGNSSRD: $GNGGA,%[^,],%lf,%c,%lf,%c,%d,%d,%[0-9.],%lf,%*s",
                           utc_str, &latitude, &laitude_indicator, &longitude, &longitude_indicator,
                           &position_indicator, &star_num, hdop_str, &altitude) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "$GPGSA%*s") == 0) &&
            (atparser_recv(g_atparser_uservice_t, "$BDGSA%*s") == 0)) {


            if ((atparser_recv(g_atparser_uservice_t, "$GPGSV,%d,%*s", &gsv_num) != 0)) {
                goto failed;
            }

            while (--gsv_num) {
                if ((atparser_recv(g_atparser_uservice_t, "$GPGSV,%*s") != 0)) {
                    goto failed;
                }
            }

            if ((atparser_recv(g_atparser_uservice_t, "$BDGSV,%d,%*s", &gsv_num) != 0)) {
                goto failed;
            }

            while (--gsv_num) {
                if ((atparser_recv(g_atparser_uservice_t, "$BDGSV,%*s") != 0)) {
                    goto failed;
                }
            }

            if ((atparser_recv(g_atparser_uservice_t, "$GNRMC,%*[^,],%c,%*lf,%*c,%*lf,%*c,%lf,%lf,%d,%*s",
                               &position_status, &speed, &orientation, &data) == 0) &&
                (atparser_recv(g_atparser_uservice_t, "$GNVTG%*s") == 0) &&
                (atparser_recv(g_atparser_uservice_t, "$GNACC%*s") == 0) &&
                (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {

                if (position_indicator == 0 || position_status == 'V') {
                    LOGW(TAG, "m6313_gnss_get_gnss_info failed %d, %c", position_indicator, position_status);
                }

                ret = 0;

                info->altitude            = altitude;
                /* dddmm.mmm -> ddd.dddddd */
                info->latitude            = floor(latitude / 100) + fmod(latitude, 100) / 60;
                /* dddmm.mmm -> ddd.dddddd */
                info->longitude           = floor(longitude / 100) + fmod(longitude, 100) / 60;
                info->orientation         = orientation;
                info->latitude_indocator  = laitude_indicator;
                info->longitude_indicator = longitude_indicator;
                info->hdop                = atof(hdop_str);
                /* Knot/h -> km/h */
                info->speed               = speed * 1.852;
                info->star_num            = star_num;
                info->time_utc            = atof(utc_str);
                info->data                = data;
                info->positon_indicator   = position_status;
            }
        }
    }

failed:
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);


    return ret;
}


int m6313_get_ccid(char ccid[21])
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CCID") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CCID: \"%[^\"]\"", ccid) == 0) &&
            (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int m6313_passthrough(char *atcmd)
{
    int ret = -1;
    char *data = malloc(1024);

    if (data == NULL) {
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);
    m6313_set_timeout(100);

    if (atparser_send(g_atparser_uservice_t, "%s", atcmd) == 0) {
        while(atparser_recv(g_atparser_uservice_t, "%s", data) == 0) {
                LOGI(TAG, "%s", data);
        }
    }

    m6313_set_timeout(8000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    free(data);

    return ret;
}

void m6313_set_timeout(int ms)
{
    atparser_set_timeout(g_atparser_uservice_t, ms);
}

int m6313_module_init(utask_t *task, m6313_param_t *param)
{
    uart_config_t config;

    uart_config_default(&config);
    config.baud_rate = param->baud;
    config.flow_control = param->enable_flowctl;

    if (task == NULL) {
        task = utask_new("m6313", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);
    }

    if (task == NULL) {
        return -1;
    }

    uart_config_default(&config);

    g_atparser_uservice_t = atparser_init(task, param->device_name, &config);

    if (g_atparser_uservice_t == NULL) {
        LOGI(TAG, "atparser_init FAIL");
        return -1;
    }

    aos_mutex_new(&g_cmd_mutex);

    if (g_cmd_mutex.hdl == NULL) {
        LOGI(TAG, "atparser_init mutex FAIL");
        return -1;
    }

    aos_event_new(&g_event, 0);

    aos_check(g_event.hdl, EIO);

    if (param->reset_pin) {
        g_powerkey_pin = param->reset_pin;
    }

    if (param->state_pin) {
        g_power_on_pin = param->state_pin;
    }

    atparser_oob_create(g_atparser_uservice_t, "0,CLOSED", _closed_handler, (void *)0);
    atparser_oob_create(g_atparser_uservice_t, "1,CLOSED", _closed_handler, (void *)1);
    atparser_oob_create(g_atparser_uservice_t, "2,CLOSED", _closed_handler, (void *)2);
    atparser_oob_create(g_atparser_uservice_t, "3,CLOSED", _closed_handler, (void *)3);
    atparser_oob_create(g_atparser_uservice_t, "4,CLOSED", _closed_handler, (void *)4);
    atparser_oob_create(g_atparser_uservice_t, "5,CLOSED", _closed_handler, (void *)5);
    atparser_oob_create(g_atparser_uservice_t, "+PDP DEACT", _disconnect_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+CPIN: NOT READY", _disconnect_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+RECEIVE:", _recv_data_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+CREG: 1", _creg_handler, (void *)1);
    atparser_oob_create(g_atparser_uservice_t, "+CREG: 2", _creg_handler, (void *)2);
    atparser_oob_create(g_atparser_uservice_t, "+CREG: 3", _creg_handler, (void *)3);
    atparser_oob_create(g_atparser_uservice_t, "+CREG: 4", _creg_handler, (void *)4);
    atparser_oob_create(g_atparser_uservice_t, "+CREG: 5", _creg_handler, (void *)5);
    atparser_oob_create(g_atparser_uservice_t, "SMS Ready", _void_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "Call Ready", _void_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+QNITZ:", _clean_line, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+CMTI:", _recv_sms_int, NULL);

    event_subscribe(EVENT_SMS_RECV, _recv_sms, NULL);

    atparser_debug_control(g_atparser_uservice_t, 1);

    return 0;
}
