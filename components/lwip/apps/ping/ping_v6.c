/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <yoc_config.h>
#if defined(CONFIG_CLI) && defined(CONFIG_TCPIP)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "lwipopts.h"

#if LWIP_IPV6 && LWIP_RAW /* don't build if not configured for use in lwipopts.h */
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/icmp6.h"
#include "lwip/prot/prot_icmp6.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/netdb.h"

#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET

#include "lwip/sockets.h"
#include "arpa/inet.h"
#endif

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping target - should be an "ip4_addr_t" */
#ifndef PING_TARGET
#define PING_TARGET   (netif_default ? *netif_ip4_gw(netif_default) : (*IP4_ADDR_ANY))
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 4000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

#define PING_REPEAT_CNT     10

/* ping variables */
static u16_t ping_seq_num;
static uint64_t ping_time;
#if !PING_USE_SOCKETS
static struct raw_pcb *ping_pcb;
#endif /* PING_USE_SOCKETS */

extern char *host_ip;

/* Prepare a echo ICMP request */
static void ping6_prepare_echo(struct icmp6_echo_hdr *iecho, u16_t len, ip6_addr_t *saddr , ip6_addr_t *daddr)
{
    size_t i;
    struct pbuf r;
    size_t data_len = len - sizeof(struct icmp6_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP6_TYPE_EREQ);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp6_echo_hdr) + i] = (char)i;
    }

    memset((void *)&r, 0, sizeof(r));
    r.payload = iecho;
    r.len = len;
    r.tot_len = len;

    iecho->chksum = ip6_chksum_pseudo(&r, IP6_NEXTH_ICMP6, r.tot_len, saddr, daddr);
}

/* Ping using the socket ip */
static err_t ping_send(int s, ip_addr_t *addr)
{
    int err;
    struct sockaddr_storage to;
    struct netif *pnetif;
    struct icmp6_echo_hdr *iecho;
    ip6_addr_t * saddr;
    size_t ping_size = sizeof(struct icmp6_echo_hdr) + PING_DATA_SIZE;

    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    pnetif = netif_find("en0");

    if (pnetif == NULL) {
        printf("\t net work has not inited \r\n");
        return -1;
    }

    iecho = (struct icmp6_echo_hdr *)mem_malloc((mem_size_t)ping_size);

    if (!iecho) {
        return ERR_MEM;
    }

    if (ip6_addr_islinklocal(ip_2_ip6(addr))) {
        saddr = (ip6_addr_t *)netif_ip6_addr(pnetif, 0);
    } else {
        saddr = (ip6_addr_t *)netif_ip6_addr(pnetif, 1);
    }

    ping6_prepare_echo(iecho, (u16_t)ping_size, saddr, ip_2_ip6(addr));

    if (IP_IS_V6(addr)) {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&to;
        to6->sin6_len    = sizeof(to6);
        to6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(addr));
    }

    err = sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));

    if (err < 0) {
        printf(" sendto failed, %d \r\n", err);
        mem_free(iecho);
        return err;
    }

    mem_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s)
{
    char buf[94];
    int len;
    struct sockaddr_storage from;
    int fromlen = sizeof(from);
    ip_addr_t fromaddr;

    while ((len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr))) {
            memset(&fromaddr, 0, sizeof(fromaddr));

            if (from.ss_family == AF_INET6) {
                struct sockaddr_in6 *from6 = (struct sockaddr_in6 *)&from;
                inet6_addr_to_ip6addr(ip_2_ip6(&fromaddr), &from6->sin6_addr);
                IP_SET_TYPE(&fromaddr, IPADDR_TYPE_V6);
            }

            /* todo: support ICMP6 echo */
            if (IP_IS_V6_VAL(fromaddr)) {
                struct ip6_hdr *iphdr;
                struct icmp6_echo_hdr *iecho;

                iphdr = (struct ip6_hdr *)buf;
                iecho = (struct icmp6_echo_hdr *)(buf + IP6H_PLEN(iphdr));
                /* printf("\t ping: len: %d, id:%x, sepo:%x\r\n",len, iecho->id,iecho->seqno ); */

                if (iecho->type != ICMP6_TYPE_EREP) {
                    continue;
                }

                if ((iecho->id == PING_ID/*0x100*/) && (iecho->seqno == lwip_htons(ping_seq_num))) {
                    /* do some ping result processing */
                    printf("\t from %s: icmp_seq=%d time=%d ms\r\n", ipaddr_ntoa(&fromaddr), ping_seq_num, (aos_now_ms() - ping_time));
                    return;
                } else {
                    printf("\t ping: drop, len: %d\r\n", len);
                }
            }
        }

        fromlen = sizeof(from);
    }

    if (len < 0) {
        printf("\t from %s: icmp_seq=%d Destination Host Unreachable \r\n", host_ip, ping_seq_num);
    }

    /* do some ping result processing */
    PING_RESULT(0);
}

uint32_t cmd_ping6_func(ip_addr_t *ping_target)
{
    int s;
    int ret;
    int cnt = PING_REPEAT_CNT;

    ping_seq_num = 0;

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
    int timeout = PING_RCV_TIMEO;
#else
    struct timeval timeout;
    timeout.tv_sec = PING_RCV_TIMEO / 1000;
    timeout.tv_usec = (PING_RCV_TIMEO % 1000) * 1000;
#endif

    if (IP_IS_V4(ping_target) || ip6_addr_isipv6mappedipv4(ip_2_ip6(ping_target))) {
        s = socket(AF_INET6, SOCK_RAW, IP_PROTO_ICMP);
    } else {
        s = socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
    }

    ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    LWIP_ASSERT("setting receive timeout failed", ret == 0);

    while (cnt--) {
        if (ping_send(s, ping_target) == ERR_OK) {
            /* printf("\t fd %d, ping : %s \r\n\r\n", s, inet6_ntoa(ping_target)); */
            ping_time = aos_now_ms();
            ping_recv(s);
        } else {
            printf("\t ping: send to %s error\r\n", ipaddr_ntoa(ping_target));
        }

        aos_msleep(PING_DELAY);
    }

    close(s);

    return 0;
}

#endif

#endif /* defined(CONFIG_CLI) && defined(CONFIG_TCPIP) */
