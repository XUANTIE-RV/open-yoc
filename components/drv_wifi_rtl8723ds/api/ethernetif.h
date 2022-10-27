#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

void ethernetif_recv(struct netif *netif, int total_len);
err_t ethernetif_init_rtl(struct netif *netif);
void lwip_PRE_SLEEP_PROCESSING(void);
void lwip_POST_SLEEP_PROCESSING(void);

#ifdef __cplusplus
}
#endif

#endif
