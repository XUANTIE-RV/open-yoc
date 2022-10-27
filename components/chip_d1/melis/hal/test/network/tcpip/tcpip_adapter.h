/*
 * Filename: os_tcpip.h
 * description: tcp ip stack init
 * Created: 2019.07.22
 * Author:laumy
 */
#ifndef __TCPIP_ADAPTER_H__
#define __TCPIP_ADAPTER_H__

#include <lwip/netif.h>

typedef int net_err_t;

#define NET_OK 0
#define NET_ERR -1

typedef enum {
	MODE_STA = 0 , /*wlan station mode*/
	MODE_AP ,       /*wlan ap mode*/
	IF_MAX,
}if_type_t;

typedef struct {
	ip4_addr_t ip;
	ip4_addr_t netmask;
	ip4_addr_t gw;
}if_info_t;

void set_netif(if_type_t mode, struct netif *netif);
struct netif* get_netif(if_type_t mode);
#endif
