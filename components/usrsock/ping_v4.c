/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <lwip/sockets.h>

#if LWIP_IPV4 && LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/netdb.h"
#include "arpa/inet.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/api.h"

#include <aos/debug.h>

#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET
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

#define PING_REPEAT_CNT     3

/* ping variables */
static u16_t ping_seq_num;
static uint64_t ping_time;
static char *host_ip;

#if !PING_USE_SOCKETS
static struct raw_pcb *ping_pcb;
#endif /* PING_USE_SOCKETS */

#ifndef FOLD_U32T
#define FOLD_U32T(u)          (((u) >> 16) + ((u) & 0x0000ffffUL))
#endif

#ifndef SWAP_BYTES_IN_WORD
#define SWAP_BYTES_IN_WORD(w) (((w) & 0xff) << 8) | (((w) & 0xff00) >> 8)
#endif /* SWAP_BYTES_IN_WORD */

static u16_t lwip_standard_chksum(const void *dataptr, int len)
{
  const u8_t *pb = (const u8_t *)dataptr;
  const u16_t *ps;
  u16_t t = 0;
  u32_t sum = 0;
  int odd = ((mem_ptr_t)pb & 1);

  /* Get aligned to u16_t */
  if (odd && len > 0) {
    ((u8_t *)&t)[1] = *pb++;
    len--;
  }

  /* Add the bulk of the data */
  ps = (const u16_t *)(const void *)pb;
  while (len > 1) {
    sum += *ps++;
    len -= 2;
  }

  /* Consume left-over byte, if any */
  if (len > 0) {
    ((u8_t *)&t)[0] = *(const u8_t *)ps;
  }

  /* Add end bytes */
  sum += t;

  /* Fold 32-bit sum to 16 bits
     calling this twice is probably faster than if statements... */
  sum = FOLD_U32T(sum);
  sum = FOLD_U32T(sum);

  /* Swap if alignment was odd */
  if (odd) {
    sum = SWAP_BYTES_IN_WORD(sum);
  }

  return (u16_t)sum;
}


static u16_t inet_chksum(const void *dataptr, u16_t len)
{
  return (u16_t)~(unsigned int)lwip_standard_chksum(dataptr, len);
}

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t ping_send(int s, ip_addr_t *addr)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    aos_assert(ping_size <= 0xffff);
    aos_assert(!IP_IS_V6(addr));

    iecho = (struct icmp_echo_hdr *)aos_malloc((mem_size_t)ping_size);

    if (!iecho) {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ip4addr(&to.sin_addr, ip_2_ip4(addr));

    err = sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));

    aos_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s)
{
    char buf[64];
    int len = -1;
    struct sockaddr from;
    //struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    ip4_addr_t fromaddr;
    int fromlen = sizeof(from);

    while ((len = recvfrom(s, buf, sizeof(buf), 0, &from, (socklen_t *)&fromlen)) > 0) {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
            memset(&fromaddr, 0 , sizeof(ip_addr_t));

            if (from.sa_family != AF_INET) {
                /* Ping is not IPv4 */
                //printf("\tping: invalid sin_family\n");
                continue;
            } else {
                struct sockaddr_in *from4 = (struct sockaddr_in *)&from;

                inet_addr_to_ip4addr(ip_2_ip4(&fromaddr), &from4->sin_addr);
                iphdr = (struct ip_hdr *)buf;
                iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));

                if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
                    /* do some ping result processing */
                    PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
                    printf("\tfrom %s: icmp_seq=%d time=%lld ms\n", inet_ntoa(fromaddr), ping_seq_num, (aos_now_ms() - ping_time));
                    return;
                } else {
                    printf("\tping: drop\n");
                }
            }
        }

        fromlen = sizeof(from);
    }

    if (len <= 0) {
        printf("\tfrom %s: icmp_seq=%d Unreachable\n", host_ip, ping_seq_num);
    }

    /* do some ping result processing */
    PING_RESULT(0);
}

uint32_t cmd_ping4_func(ip_addr_t *ping_target)
{
    int s;
    int ret = -1;
    int cnt = PING_REPEAT_CNT;
    ping_seq_num = 0;
    host_ip = ipaddr_ntoa(ping_target);

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
    int timeout = PING_RCV_TIMEO;
#else
    struct timeval timeout;
    timeout.tv_sec = PING_RCV_TIMEO / 1000;
    timeout.tv_usec = (PING_RCV_TIMEO % 1000) * 1000;
#endif

    if ((s = socket(AF_INET, SOCK_DGRAM, IP_PROTO_ICMP)) < 0) {
        return 0;
    }

    ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if (ret < 0) {
        printf("set timeout failed %d", ret);
    }

    while (cnt--) {
        if (ping_send(s, ping_target) == ERR_OK) {
            /* printf("\tping : %s\n", inet_ntoa(ping_target)); */

            ping_time = aos_now_ms();
            ping_recv(s);
        } else {
            printf("\tping: send to %s error\n", inet_ntoa(*ping_target));
        }

        aos_msleep(PING_DELAY);
    }

    closesocket(s);

    return 0;
}

#endif
