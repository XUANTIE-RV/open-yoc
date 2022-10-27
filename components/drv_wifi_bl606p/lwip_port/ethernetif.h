#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#ifdef __cplusplus
extern "C" {
#endif

void set_if(struct netif *netif, char* ip_addr, char* gw_addr, char* nm_addr);

#ifdef __cplusplus
}
#endif

#endif
