/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"
#include "k_api.h"
// #include "net_cmds.h"
#include "ethernetif.h"
#include <aos/kernel.h>
#include <aos/debug.h>
#include <aos/cli.h>

#include "cvi_mac_phy.h"

#define GMAC_BUF_LEN (1500 + 20)

extern uint8_t g_mac_addr[6];

static uint8_t SendDataBuf[GMAC_BUF_LEN];
static uint8_t RecvDataBuf[GMAC_BUF_LEN];
extern uint8_t g_phy_online_stat;

#define PHY_OFFLINE 0
#define PHY_ONLINE 1

aos_sem_t eth_rx_sem;
extern struct netif xnetif;

extern void csi_eth_phy_update_link_task(void *arg);
extern void csi_send_packet(uint8_t *buf, uint32_t len, uint32_t flags);
extern int32_t csi_receive_packet(uint8_t *buf, uint32_t len);
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
int  ethernetif_input(struct netif *netif);

void eth_rx(void *arg)
{
  int ret;
    while(1) {
      aos_sem_wait(&eth_rx_sem, AOS_WAIT_FOREVER);
      while(1) {
        ret = ethernetif_input(&xnetif);
        if(ret == 0) {
          break;
        }
      }
    }
    aos_task_exit(0);
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
  //struct ethernetif *ethernetif = netif->state;
  int ret = 0;
  uint8_t mac[6] = {0xf2, 0x42, 0x9f, 0xa5, 0x0a, 0x72};
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* get MAC hardware address */
  csi_get_macaddr(mac);
  memcpy(netif->hwaddr, mac, ETHARP_HWADDR_LEN);

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IPV6 && LWIP_IPV6_MLD
  /*
   * For hardware/netifs that implement MAC filtering.
   * All-nodes link-local is handled by default, so we must let the hardware know
   * to allow multicast packets in.
   * Should set mld_mac_filter previously. */
  if (netif->mld_mac_filter != NULL) {
    ip6_addr_t ip6_allnodes_ll;
    ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
    netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

  /* Do whatever else is needed to initialize interface. */
    aos_task_t eth_rec_handle;
    aos_task_t eth_uplink_handle;

    aos_sem_new(&eth_rx_sem, 0);
    ret = aos_task_new_ext(&eth_rec_handle, "eth_recv_task", eth_rx, NULL, 8192, 36);
    if (RHINO_SUCCESS != ret) {
        printf("eth_recv_task create fail!\n");
    }
      ret = aos_task_new_ext(&eth_uplink_handle, "eth_uplink_task", csi_eth_phy_update_link_task, NULL, 4096, 50);
    if (RHINO_SUCCESS != ret) {
        printf("eth_uplink_task create fail!\n");
    }
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  //struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;
  uint32_t i = 0;
  // CPSR_ALLOC();
  //initiate transfer();

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

  for (q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    //send data from(q->payload, q->len);
    MEMCPY((uint8_t *)&SendDataBuf[i],(uint8_t *)q->payload,q->len);
    i = i + q->len;
    if((i > GMAC_BUF_LEN) || (i > p->tot_len)) {
      LWIP_DEBUGF(NETIF_DEBUG, ("low_level_output: error, len=%"U32_F",tot_len=%"U32_F"\n\t", i, p->tot_len));
      return ERR_BUF;
    }
  }
  if(i == p->tot_len) {
    
    // RHINO_CPU_INTRPT_DISABLE()
    csi_send_packet(SendDataBuf, i, 0);
    // RHINO_CPU_INTRPT_ENABLE() 
 
    //signal that packet should be sent();
    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
    if (((u8_t*)p->payload)[0] & 1) {
      /* broadcast or multicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    } else {
      /* unicast packet */
      MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
    /* increase ifoutdiscards or ifouterrors on error */

  #if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
  #endif

    LINK_STATS_INC(link.xmit);
    // RHINO_CPU_INTRPT_ENABLE() 

    return ERR_OK;
  } else {
    LWIP_PLATFORM_DIAG(("send_packet: length mismatch, slen=%"U32_F",tot_len=%"U32_F"\n\t", i, p->tot_len));
    return ERR_BUF;
  }
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  //struct ethernetif *ethernetif = netif->state;
  struct pbuf *p, *q;
  int32_t len;
  uint32_t i = 0;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  // CPSR_ALLOC();
  // RHINO_CPU_INTRPT_DISABLE()
  len = csi_receive_packet(RecvDataBuf, GMAC_BUF_LEN);
  // RHINO_CPU_INTRPT_ENABLE() 

  if((len <= 0) || (len > GMAC_BUF_LEN)) {
    //LWIP_DEBUGF(NETIF_DEBUG, ("low_level_input: len error. len: %d\n", len));
    return NULL;
  }
#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for (q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */
      memcpy((u8_t*)q->payload, (u8_t*)&RecvDataBuf[i], q->len);
      i = i + q->len;
     // read data into(q->payload, q->len);
    }

    if((i != p->tot_len) || (i > len)) { 
        return NULL;
    }
   // acknowledge that packet has been read();

    MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
    if (((u8_t*)p->payload)[0] & 1) {
      /* broadcast or multicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
    } else {
      /* unicast packet*/
      MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
    }
  #if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    //drop packet();
    LWIP_DEBUGF(NETIF_DEBUG, ("drop packet.\n"));
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(netif, ifindiscards);
  }

  return p;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "T-head";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    etharp_init();
    netif_set_flags(netif, NETIF_FLAG_IGMP);
    return ERR_OK;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

int ethernetif_input(struct netif *netif)
{
  //struct ethernetif *ethernetif;
  //struct eth_hdr *ethhdr;
  struct pbuf *p;

  //ethernetif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* if no packet could be read, silently ignore this */
  if (p != NULL) {
    /* pass all packets to ethernet_input, which decides what packets it supports */
    if (netif->input(p, netif) != ERR_OK) {
      LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
      pbuf_free(p);
      p = NULL;
    }
    return 1;
  }
  return 0;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}



#include <aos/cli.h>

#include "lwip/ip_addr.h"

#include "lwip/netif.h"
static void hwaddr_aton(char *buf, unsigned char *mac)
{
    int i = 0;
    char *ptr;
    char *temp = strtok(buf,":");
    while(temp && i < 6)
    {
        mac[i] = (char)(strtol(temp, &ptr, 16) & 0xff);
        temp = strtok(NULL,":");
        i++;
    }
}

static void cmd_ifconfig_func(int argc, char **argv)
{
    unsigned char mac[6] = {0};
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    // ip_addr_t dns_svr[2];

    // wifi_ap_record_t ap_info = {0};


    /** ifconfig */
    // hal_net_get_ipaddr(dev, &ipaddr, &netmask, &gw);
    // hal_net_get_mac_addr(dev, mac);
    csi_get_macaddr(mac);
    // ret = hal_net_get_dns_server(dev, dns_svr, 2);

    struct netif *netif = &xnetif;
    // aos_check_return_einval(netif && ipaddr && netmask && gw);

    if (argc > 1) {
      if (strcmp(argv[1], "ip") == 0) {
        ipaddr_aton(argv[2], &ipaddr);
        netif_set_ipaddr(&xnetif, &ipaddr);
      }else if (strcmp(argv[1], "GWaddr") == 0) {
        ipaddr_aton(argv[2], &gw);
        netif_set_gw(&xnetif, &gw);
      }else if (strcmp(argv[1], "Mask") == 0) {
        ipaddr_aton(argv[2], &netmask);
        netif_set_netmask(&xnetif, &netmask);
      }else if (strcmp(argv[1], "HWaddr") == 0) {
        hwaddr_aton(argv[2], mac);
        csi_set_macaddr(mac);
        memcpy(netif->hwaddr, mac, ETHARP_HWADDR_LEN);
        etharp_cleanup_netif(netif);
      }else if (strcmp(argv[1], "help") == 0) {
        printf("example:\n");
        printf("\tifconfig ip 192.168.1.3\n");
        printf("\tifconfig GWaddr 192.168.1.1\n");
        printf("\tifconfig Mask 255.255.255.0\n");
        printf("\tifconfig HWaddr f2:42:9f:a5:0a:72\n");
        return;
      }
    }

    ip_addr_copy(*(ip4_addr_t *)(&ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)(&gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(&netmask), *netif_ip_netmask4(netif));

    printf("\neth\tLink encap:eth  HWaddr ");
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("    \tinet addr:%s\n", ipaddr_ntoa(&ipaddr));
    printf("\tGWaddr:%s\n", ipaddr_ntoa(&gw));
    printf("\tMask:%s\n", ipaddr_ntoa(&netmask));
}

ALIOS_CLI_CMD_REGISTER(cmd_ifconfig_func, ifconfig, ifconfig info);


#endif /* 0 */
