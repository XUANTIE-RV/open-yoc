/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_ESP8266_API_H
#define DEVICE_ESP8266_API_H

#include <devices/uart.h>
#include <devices/netdrv.h>

#ifdef __cplusplus
extern "C" {
#endif



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

#define ESP8266_MAX_SEND_LEN (2048)

typedef void (*net_data_input_cb_t)(int linkid, void *data, size_t len, char remote_ip[16], uint16_t remote_ports);
typedef void (*net_close_cb_t)(int linkid);

/**
 * This function will test wifi module is online/uart can use
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int esp8266_at0(void);

/**
 * This function will connect to useable ap
 * @param[in]   ap           ap name
 * @param[in]   passPhrase   ap password
 * @return      Zero on success, -1 on failed
 */
int esp8266_ap_connect(const char *ap, const char *passPhrase);

/**
 * This function will disconnect ap which is connect bofore
 * @return      Zero on success, -1 on failed
 */
int esp8266_ap_disconnect(void);

/**
 * This function will get ap info eg.channel rssi
 * @param[out]   ssid     link name
 * @param[out]   bssid    link mac
 * @param[out]   channel  link channel
 * @param[out]   rssi     link signal_quality
 * @return      Zero on success, -1 on failed
 */
int esp8266_ap_info(char ssid[32], char bssid[18], int *channel, int *rssi);

/**
 * This function will reset wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int esp8266_software_reset(void);

int esp8266_hard_reset(void);

/**
 * This function will init wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int esp8266_close_echo(void);

/**
 * This function will set wifi mode
 * @param[in]   mode   station/softap/station_softap
 * @return      Zero on success, -1 on failed
 */
int esp8266_set_wifi_mode(int mode);

/**
 * This function will set wifi mode
 * @param[in]   enable   enable muti socket mode
 * @return      Zero on success, -1 on failed
 */
int esp8266_enable_muti_sockets(uint8_t enable);

/**
 * This function will get wifi mac
 * @param[out]   mac    mac address of esp8266
 * @return      Zero on success, -1 on failed
 */
int esp8266_get_mac(aos_dev_t *dev, uint8_t *mac);

/**
 * This function will get esp8266 wifi status
 * @param[in]   stat    esp8266 status
 * @return      Zero on success, -1 on failed
 */
int esp8266_get_status(int *stat);

int esp8266_get_link_status(void);
/**
 * This function will parse domain to ip
 * @param[in]   servername
 * @param[out]  hostip_buf
 * @return      Zero on success, -1 on failed
 */
int esp8266_domain_to_ip(char *domain, char ip[16]);

/**
 * This function will get local ip from ap
 * @param[out]   ip     ip_buf
 * @param[out]   gw     gw_buf
 * @param[out]   mask   mask_buf
 * @return      Zero on success, -1 on failed
 */
int esp8266_link_info(char ip[16], char gw[16], char mask[16]);

/**
 * This function will get remote info as ip port
 * @param[in]   enable  enable get remote_info
 * @return      Zero on success, -1 on failed
 */
int esp8266_get_remote_info(uint8_t enable);

/**
 * This function will ping host to confirm net well
 * @param[in]  hostip     ping retome ip
 * @param[in]  seq_num    ping times
 * @return     Zero on success, -1 on failed
 */
int esp8266_ping(const char *ip, uint8_t seq_num);

/**
 * This function will connect a server.
 * @param[in]   id          link id
 * @param[in]   srvname     server name
 * @param[in]   port        socket port
 * @return      >=0 hanlde , -1 on failed
 */
int esp8266_connect_remote(int id, net_conn_e type, char *srvname, uint16_t port);

/**
 * This function will send bytes to device.
 * @param[in]   id          link id
 * @param[in]   pdata       data will be send
 * @param[in]   len         the byte length of data
 * @return      >=0 real sent len, -1 on failed
 */
int esp8266_send_data(int id, const uint8_t *pdata, int len, int timeout);

/**
 * This function will send bytes to device.
 * @param[in]   net_data_input_cb_t  data_handle_function
 * @return      >=0 real sent len, -1 on failed
 */
int esp8266_packet_input_cb_register(net_data_input_cb_t cb);

int esp8266_close_cb_register(net_close_cb_t cb);

/**
 * This function will disconect ap connect
 * @param[in]   id    link id
 * @return      Zero on success, -1 on failed
 */
int esp8266_close(int id);

/**
 * This function will open smartconfig mode for phone app to config apconnetc
 * @param[in]   enable         smartconfig enable/disenable
 * @return      Zero on success, -1 on failed
 */
int esp8266_smartconfig_enable(uint8_t enable);

/**
 * This function will config uart to commit to wifi module
 * @param[in]   baud
 * @param[in]   flow_control
 * @return      Zero on success, -1 on failed
 */
int esp8266_uart_config(uint32_t baud, uint8_t flow_control);

#ifdef __cplusplus
}
#endif

#endif
