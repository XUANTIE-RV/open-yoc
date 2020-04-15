/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_SIM800_API_H
#define DEVICE_SIM800_API_H

#include <devices/uart.h>
#include <devices/netdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIM800_POWERON             0
#define SIM800_POWEROFF            1

typedef enum {
    NET_TYPE_NULL = 0,
    NET_TYPE_TCP_SERVER,
    NET_TYPE_TCP_CLIENT,
    NET_TYPE_UDP_UNICAST,
    NET_TYPE_MAX
} net_conn_e;

typedef enum {
    NET_STATUS_UNKNOW = 0,
    NET_STATUS_READY,
    NET_STATUS_LINKDOWN,
    NET_STATUS_CONFIGING,
    NET_STATUS_LINKUP,
    NET_STATUS_GOTIP,
} net_status_t;

typedef void (*net_data_input_cb_t)(int linkid, void *data, size_t len, char remote_ip[16], uint16_t remote_ports);
typedef void (*net_close_cb_t)(int linkid);

/**
 * This function will test wifi module is online/uart can use
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int sim800_at0(void);

/**
 * This function will disconnect ap which is connect bofore
 * @return      Zero on success, -1 on failed
 */
int sim800_ap_disconnect(void);

/**
 * This function will reset wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int sim800_hard_reset(void);

/**
 * This function will init wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int sim800_close_echo(void);

/**
 * This function will set wifi mode
 * @param[in]   mode   station/softap/station_softap
 * @return      Zero on success, -1 on failed
 */
int sim800_set_gprs_mode(int mode);

/**
 * This function will set wifi mode
 * @param[in]   enable   enable muti socket mode
 * @return      Zero on success, -1 on failed
 */
int sim800_enable_muti_sockets(uint8_t enable);

/**
 * This function will get sim800 wifi status
 * @param[in]   stat    sim800 status
 * @return      Zero on success, -1 on failed
 */
int sim800_get_status(char *str);

int sim800_get_link_status(void);
/**
 * This function will parse domain to ip
 * @param[in]   servername
 * @param[out]  hostip_buf
 * @return      Zero on success, -1 on failed
 */
int sim800_domain_to_ip(char *domain, char ip[16]);

/**
 * This function will get local ip from ap
 * @param[out]   ip     ip_buf
 * @param[out]   gw     gw_buf
 * @param[out]   mask   mask_buf
 * @return      Zero on success, -1 on failed
 */
int sim800_link_info(char ip[16]);

/**
 * This function will ping host to confirm net well
 * @param[in]  hostip     ping retome ip
 * @param[in]  seq_num    ping times
 * @return     Zero on success, -1 on failed
 */
int sim800_ping(const char *ip, uint8_t seq_num);

/**
 * This function will connect a server.
 * @param[in]   id          link id
 * @param[in]   srvname     server name
 * @param[in]   port        socket port
 * @return      >=0 hanlde , -1 on failed
 */
int sim800_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port);

/**
 * This function will send bytes to device.
 * @param[in]   id          link id
 * @param[in]   pdata       data will be send
 * @param[in]   len         the byte length of data
 * @return      >=0 real sent len, -1 on failed
 */
int sim800_send_data(int id, const uint8_t *pdata, int len, int timeout);

/**
 * This function will send bytes to device.
 * @param[in]   net_data_input_cb_t  data_handle_function
 * @return      >=0 real sent len, -1 on failed
 */
int sim800_packet_input_cb_register(net_data_input_cb_t cb);

int sim800_close_cb_register(net_close_cb_t cb);

/**
 * This function will disconect ap connect
 * @param[in]   id    link id
 * @return      Zero on success, -1 on failed
 */
int sim800_close(int id);

/**
 * This function will config uart to commit to wifi module
 * @param[in]   baud
 * @param[in]   flow_control
 * @return      Zero on success, -1 on failed
 */

int sim800_get_local_ip(char ip[16]);

int sim800_check_simcard_is_insert(uint8_t isAsync);

int sim800_check_signal_quality(sig_qual_resp_t *respond, uint8_t isAsync);

int sim800_check_register_status(regs_stat_resp_t *respond, uint8_t isAsync);

int sim800_check_gprs_service_status(int *sstat, uint8_t isAsync);

int sim800_wireless_set_apn(uint8_t isAsync);

int sim800_connect_to_network(uint8_t isAsync);

int sim800_define_pdp_context(uint8_t mode, uint8_t isAsync);

int sim800_recv_show_head(uint8_t mode);

int sim800_recv_show_remote_ip(uint8_t mode);

int sim800_network_shut(uint8_t isAsync);

int sim800_get_state_pin(void);

int sim800_set_gprs_service_status(int stat, uint8_t isAsync);

int sim800_get_send_len(int id, int *len, int timeout);

int sim800_set_send_prompt(int mode);

int sim800_get_ccid(char ccid[21]);

void sim800_set_timeout(int ms);

#ifdef __cplusplus
}
#endif

#endif
