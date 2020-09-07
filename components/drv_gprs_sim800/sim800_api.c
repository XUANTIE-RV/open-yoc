/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/yloop.h>

#include <yoc/atparser.h>
#include <drv/gpio.h>
#include <devices/netdrv.h>
#include <devices/hal/gprs_impl.h>

#include "sim800.h"
#include "sim800_api.h"

static atparser_uservice_t *g_atparser_uservice_t;
static int g_net_status = 0;
static net_data_input_cb_t g_net_data_input_cb;
static net_close_cb_t g_net_close_cb;
static aos_mutex_t g_cmd_mutex;
static int g_reset_pin = -1;
static int g_state_pin = -1;

#define TAG "sim800_api"

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
    //+RECEIVE,0,44,106.15.213.199:5684\r\n
    static ipd_t ipd;

    // oob timeout
    if (aos_now_ms() > (oob_data->time_stamp + TIME_OUT)) {
        ipd_clear(&ipd);
        return 0;
    }

    if (ipd.rlen == 0) { // first data
        char *str = strchr(oob_data->buf, '\n');

        if (str != NULL) {
            sscanf(oob_data->buf, "%d,%d,%[^:]:%d%*s", &ipd.linkid, &ipd.rlen, ipd.remote_ip, &ipd.remote_port);
            oob_data->used_len = ipd_put(&ipd, str + 1, oob_data->len - (str - oob_data->buf + 1));
            LOGI(TAG, "%.*s(%d)",(str - oob_data->buf + 1), oob_data->buf, oob_data->used_len);
            oob_data->used_len += (str - oob_data->buf + 1);
        }
    } else {
        oob_data->used_len = ipd_put(&ipd, oob_data->buf, oob_data->len);

        if (ipd.data_size == ipd.rlen) {
            if (g_net_data_input_cb) {
                g_net_data_input_cb(ipd.linkid, ipd.data, ipd.rlen, ipd.remote_ip, ipd.remote_port);
            }

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
/*********************************************************************

sim800 api

**********************************************************************/
int sim800_at0(void)
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

int sim800_get_state_pin(void)
{
    if (g_state_pin < 0) {
        return -1;
    }

    gpio_pin_handle_t pin_hd;
    bool val;

    pin_hd = csi_gpio_pin_initialize(g_state_pin, NULL);
    csi_gpio_pin_config(pin_hd, GPIO_MODE_PULLNONE, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_read(pin_hd, &val);

    return (int)val;
}

int sim800_hard_reset(void)
{
    if (g_reset_pin < 0) {
        return -1;
    }

    gpio_pin_handle_t reset_pin_hd;
    reset_pin_hd = csi_gpio_pin_initialize(g_reset_pin, NULL);
    csi_gpio_pin_config(reset_pin_hd, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(reset_pin_hd, SIM800_POWEROFF);
    sleep(2);
    csi_gpio_pin_write(reset_pin_hd, SIM800_POWERON);
    sleep(4);
    g_net_status = NET_STATUS_UNKNOW;

    return 0;
}


int sim800_close_echo(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "ATE0&W") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_enable_muti_sockets(uint8_t enable)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPMUX=%d", enable) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_set_gprs_mode(int mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPCSGP=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_get_status(char *str) //more data do handle
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSTATUS") == 0) {
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
} SIM800_STATUS_T;
int sim800_get_link_status(void)
{
    static int stat = -1;

    if (g_net_status == stat) {
        return g_net_status;
    }

    char str[15];
    int ret = sim800_get_status(str);

    if (ret == 0) {
        LOGD(TAG, "gprs state:%s", str);
        const SIM800_STATUS_T sim800_state_table[] = {
            {"IP INITIAL", NET_STATUS_LINKDOWN},
            {"IP START", NET_STATUS_GOTIP},
            {"IP CONFIG", NET_STATUS_LINKUP},
            {"IP GPRSACT", NET_STATUS_GOTIP},
            {"IP STATUS", NET_STATUS_LINKUP},
            {"IP PROCESSING", NET_STATUS_GOTIP},
            {"PDP DEACT", NET_STATUS_LINKDOWN},
        };

        for (int i = 0; i < sizeof(sim800_state_table) / sizeof(SIM800_STATUS_T); i++) {
            if (memcmp(str, sim800_state_table[i].str, strlen(sim800_state_table[i].str)) == 0) {
                g_net_status = sim800_state_table[i].state;
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

int sim800_get_local_ip(char ip[16])
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        strcpy(ip, "0.0.0.0");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIFSR") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "%s", ip) == 0) {
            ret = 0;
            g_net_status = NET_STATUS_GOTIP;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_domain_to_ip(char *domain, char ip[16])
{
    int ret = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        memset(ip, 0, 16);
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CDNSGIP=\"%s\"", domain) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0) \
            && (atparser_recv(g_atparser_uservice_t, "+CDNSGIP: 1,\"%*[^\"]\",\"%[^\"]%*s", ip) == 0)) {
            ret = 0;
        }
    } //+CDNSGIP:0,<dns error code>

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}


int sim800_link_info(char ip[16])
{
    int ret = -1;

    memset(ip, 0, 16);

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIFSR") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", ip) == 0)) {
            ret = 0;
            g_net_status = NET_STATUS_GOTIP;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;

}

int sim800_ping(const char *ip, uint8_t seq_num)
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

    if (atparser_send(g_atparser_uservice_t, "AT+CIPPING=\"%s\",1", ip) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CIPPING: %*d,\"%[^\"]\",%*d,%d", ipaddr, &ping_time) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK\n") == 0)) {
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

int sim800_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port)
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
            ret = atparser_send(g_atparser_uservice_t, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", id, srvname, port);

            break;

        case NET_TYPE_UDP_UNICAST:
            ret = atparser_send(g_atparser_uservice_t, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d", id, srvname, port);

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

int sim800_get_send_len(int id, int *len, int timeout)
{
    int ret = -1;
    int i ;
    int n;
    int send_len;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    sim800_set_timeout(timeout);

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

    sim800_set_timeout(8*1000);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_send_data(int id, const uint8_t *pdata, int len, int timeout)
{
    int ret = -1;
    int ret_id = -1;

    if (g_net_status < NET_STATUS_GOTIP) {
        printf("\t net status error\r\n");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    sim800_set_timeout(timeout);
    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSEND=%d,%d", id, len) == 0) {
        if (atparser_recv_str(g_atparser_uservice_t, "> ") == 0) {
            atparser_write(g_atparser_uservice_t, pdata, len);

            if (atparser_recv(g_atparser_uservice_t, "%d, SEND OK", &ret_id) == 0) {
                    if (ret_id == id) {
                        ret = 0;
                }
            }
        }
    }

    sim800_set_timeout(8 * 1000);
    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_packet_input_cb_register(net_data_input_cb_t cb)
{
    if (cb) {
        g_net_data_input_cb = cb;
    } else {
        LOGI(TAG, "packet input cb error\n");
    }

    return 0;
}

int sim800_close_cb_register(net_close_cb_t cb)
{
    if (cb) {
        g_net_close_cb = cb;
    } else {
        LOGE(TAG, "packet input cb error\n");
    }

    return 0;
}

int sim800_close(int id)
{
    int ret = -1;
    int rid = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPCLOSE=%d", id) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "%d, CLOSE OK", &rid) == 0) {
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

int sim800_check_simcard_is_insert(uint8_t isAsync)
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

int sim800_check_signal_quality(sig_qual_resp_t *respond, uint8_t isAsync)
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

int sim800_check_register_status(regs_stat_resp_t *respond, uint8_t isAsync)
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

int sim800_check_gprs_service_status(int *sstat, uint8_t isAsync)
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

int sim800_set_gprs_service_status(int state, uint8_t isAsync)
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

int sim800_wireless_set_apn(uint8_t isAsync)
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

int sim800_connect_to_network(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIICR") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_define_pdp_context(uint8_t mode, uint8_t isAsync)
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

int sim800_recv_show_head(uint8_t mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPHEAD=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_recv_show_remote_ip(uint8_t mode)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSRIP=%d", mode) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_network_shut(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIPSHUT") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "SHUT OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int sim800_get_ccid(char ccid[21])
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CCID") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", ccid) == 0 )\
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0 )) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

void sim800_set_timeout(int ms)
{
    atparser_set_timeout(g_atparser_uservice_t, ms);
}

int sim800_module_init(utask_t *task, sim_gprs_param_t *param)
{
    uart_config_t config;

    uart_config_default(&config);
    config.baud_rate = param->baud;
    config.flow_control = param->enable_flowctl;

    if (task == NULL) {
        task = utask_new("sim800", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);
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

    if (param->reset_pin) {
        g_reset_pin = param->reset_pin;
    }

    if (param->state_pin) {
        g_state_pin = param->state_pin;
    }

    atparser_oob_create(g_atparser_uservice_t, "0, CLOSED", _closed_handler, (void *)0);
    atparser_oob_create(g_atparser_uservice_t, "1, CLOSED", _closed_handler, (void *)1);
    atparser_oob_create(g_atparser_uservice_t, "2, CLOSED", _closed_handler, (void *)2);
    atparser_oob_create(g_atparser_uservice_t, "3, CLOSED", _closed_handler, (void *)3);
    atparser_oob_create(g_atparser_uservice_t, "4, CLOSED", _closed_handler, (void *)4);
    atparser_oob_create(g_atparser_uservice_t, "5, CLOSED", _closed_handler, (void *)5);
    atparser_oob_create(g_atparser_uservice_t, "+PDP: DEACT", _disconnect_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+CPIN: NOT READY", _disconnect_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+RECEIVE,", _recv_data_handler, NULL);

    atparser_debug_control(g_atparser_uservice_t, 1);

    return 0;
}

