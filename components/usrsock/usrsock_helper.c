/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_helper.c
 * @brief    user sock help functions
 * @version  V1.0
 * @date     15. Mar 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <soc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/list.h>

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/prot/dns.h"
#include "usrsock2lwip.h"

#define DNS_DEFAULT_PORT                    53

#define DNS_CLASS_IN                         1 /* RFC 1035 Internet */
#define DNS_RECTYPE_A                        1
#define DNS_RECTYPE_AAAA                    28

#define NETDB_LENGTH                       128
#define NETDB_DNSSERVER_IPv4ADDR    0x771d1d1d
#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define NETDB_DNSCLIENT_MAXRESPONSE        176
#define NETDB_DNSCLIENT_NAMESIZE            64
#define NETDB_DNSCLIENT_ENTRIES              8
#define NETDB_DNSCLIENT_RETRIES              3

#define SEND_BUFFER_SIZE (16 + NETDB_DNSCLIENT_NAMESIZE + 2)
#define RECV_BUFFER_SIZE NETDB_DNSCLIENT_MAXRESPONSE

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct hostent_info_s
{
    char *hi_addrlist[2];
    char hi_data[1];
};

#if LWIP_IPV4
const in_addr_t g_lo_ipv4addr = PP_HTONL(0x7f000001);
#elif LWIP_IPV6
typedef uint16_t net_ipv6addr_t[8];
const net_ipv6addr_t g_lo_ipv6addr =
{
    htons(0), htons(0), htons(0), htons(0),
    htons(0), htons(0), htons(0), htons(1)
};
#endif

union dns_addr_u
{
  struct sockaddr     addr;        /* Common address representation */
#if LWIP_IPV4
  struct sockaddr_in  ipv4;        /* IPv4 address */
#endif
#if LWIP_IPV6
  struct sockaddr_in6 ipv6;        /* IPv6 address */
#endif
};

struct dns_query_s
{
    int sd;                         /* DNS server socket */
    int result;                     /* Explanation of the failure */
    const char *hostname;           /* Hostname to lookup */
    union dns_addr_u *addr;         /* Location to return host address */
    int *naddr;                     /* Number of returned addresses */
};

struct dns_query_info_s
{
    union
    {
#if LWIP_IPV4
        struct in_addr srv_ipv4;                     /* DNS server address */
#endif
#if LWIP_IPV6
        struct in6_addr srv_ipv6;                    /* DNS server address */
#endif
    } u;
    in_port_t srv_port;                            /* DNS server port */
    uint16_t id;                                   /* Query ID */
    uint16_t rectype;                              /* Queried record type */
    uint16_t qnamelen;                             /* Queried hostname length */
    char qname[NETDB_DNSCLIENT_NAMESIZE+2];        /* Queried hostname in encoded
                                                    * format + NUL */
};

struct dns_header_s
{
    uint16_t id;
    uint8_t  flags1;
    uint8_t  flags2;
    uint16_t numquestions;
    uint16_t numanswers;
    uint16_t numauthrr;
    uint16_t numextrarr;
};

struct dns_question_s
{
    uint16_t type;
    uint16_t class;
};

struct dns_answer_s
{
    uint16_t type;
    uint16_t class;
    uint16_t ttl[2];
    uint16_t len;

    union
    {
#if LWIP_IPV4
        struct in_addr ipv4;
#endif
#if LWIP_IPV6
        struct in6_addr ipv6;
#endif
    } u;
} __attribute__((__packed__));

struct dq_queue_s
{
    dlist_t *head;
    dlist_t *tail;
};

typedef struct dq_queue_s dq_queue_t;

#define dq_peek(q)  ((q)->head)
#define dq_tail(q)  ((q)->tail)

typedef int (*dns_callback_t)(void *arg,
        struct sockaddr *addr,
        socklen_t addrlen);

struct dns_notify_s
{
    dlist_t entry;   /* Supports a doubly linked list */
    dns_callback_t callback;
    void *arg;
};

struct ai_s
{
    struct addrinfo ai;
    union
    {
#if LWIP_IPV4
        struct sockaddr_in sin;
#endif
#if LWIP_IPV6
        struct sockaddr_in6 sin6;
#endif
    } sa;
};

struct servent {
    char  *s_name;       /* Official name of the service. */
    char **s_aliases;    /* A pointer to an array of pointers to
                          * alternative service names, terminated by a
                          * null pointer. */
    int    s_port;       /* The port number at which the service resides,
                          * in network byte order. */
    char  *s_proto;      /* The name of the protocol to use when
                          * contacting the service. */
};

struct services_db_s {
    const char *s_name;
    int s_port;
    int s_protocol;
};

struct dns_cache_s
{
#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
    time_t            ctime;      /* Creation time */
#endif
    char              name[NETDB_DNSCLIENT_NAMESIZE];
    uint8_t           naddr;      /* How many addresses per name */
    union dns_addr_u  addr[1];    /* Resolved address */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static union dns_addr_u g_dns_server;
static bool g_dns_address;

static uint8_t g_dns_head;        /* Head of the circular, DNS resolver cache */
static uint8_t g_dns_tail;        /* Tail of the circular, DNS resolver cache */

static struct dns_cache_s g_dns_cache[8];

static dq_queue_t g_dns_notify;
static aos_sem_t g_dns_sem;

static struct hostent g_hostent;
static char g_hostbuffer[NETDB_LENGTH];

int h_errno;

const static struct services_db_s g_services_db[] =
{
    { "ntp", 123, IPPROTO_TCP },
    { "ntp", 123, IPPROTO_UDP },
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int lib_numeric_address(const char *name, struct hostent *host,
                               char *buf, size_t buflen)
{
    struct hostent_info_s *info;
    char *ptr;
    socklen_t addrlen;
    int namelen;
    int ret;

    /* Verify that we have a buffer big enough to get started (it still may not
     * be big enough).
     */

    if (buflen <= sizeof(struct hostent_info_s)) {
        return -ERANGE;
    }

    info    = (struct hostent_info_s *)buf;
    ptr     = info->hi_data;
    buflen -= (sizeof(struct hostent_info_s) - 1);

    memset(host, 0, sizeof(struct hostent));
    memset(info, 0, sizeof(struct hostent_info_s));

    /* If the address contains a colon, then it might be a numeric IPv6
     * address
     */

    if (strchr(name, ':') != NULL) {
        /* Make sure that space remains to hold the IPv6 address */

        addrlen = sizeof(struct in6_addr);
        if (buflen < addrlen) {
            return -ERANGE;
        }

        ret = inet_pton(AF_INET6, name, ptr);

        /* The inet_pton() function returns 1 if the conversion succeeds. It
         * will return 0 if the input is not a valid IP address string, or -1
         * if the address family argument is unsupported.
         */

        if (ret < 1) {
            /* Conversion failed.  Must not be a IPv6 address */

            return 1;
        }

        host->h_addrtype  = AF_INET6;
    }

    /* If the address contains a colon, then it might be a numeric IPv6
     * address.
     */

    else if (strchr(name, '.') != NULL) {
        /* Make sure that space remains to hold the IPv4 address */

        addrlen = sizeof(struct in_addr);
        if (buflen < addrlen) {
            return -ERANGE;
        }

        ret = inet_pton(AF_INET, name, ptr);

        /* The inet_pton() function returns 1 if the conversion succeeds. It
         * will return 0 if the input is not a valid IP address string, or -1
         * if the address family argument is unsupported.
         */

        if (ret < 1) {
            /* Conversion failed.  Must not be an IPv4 address */

            return 1;
        }

        host->h_addrtype  = AF_INET;
    }

    /* No colon?  No period?  Can't be a numeric address */

    else {
        return 1;
    }

    info->hi_addrlist[0] = ptr;
    host->h_addr_list    = info->hi_addrlist;
    host->h_length       = addrlen;

    ptr                 += addrlen;
    buflen              -= addrlen;

    /* And copy name */

    namelen = strlen(name);
    if ((namelen + 1) > buflen) {
        return -ERANGE;
    }

    strncpy(ptr, name, buflen);

    /* Set the address to h_name */

    host->h_name = ptr;
    return 0;
}

static int lib_localhost(const char *name, struct hostent *host,
                         char *buf, size_t buflen)
{
    struct hostent_info_s *info;
    socklen_t addrlen;
    const char *src;
    char *dest;
    int namelen;

    if (strcmp(name, "localhost") == 0) {
        /* Yes.. it is the localhost */

#if LWIP_IPV4
        /* Setup to transfer the IPv4 address */

        addrlen          = sizeof(struct in_addr);
        src              = (const char *)&g_lo_ipv4addr;
        host->h_addrtype = AF_INET;

#elif LWIP_IPV6
        /* Setup to transfer the IPv6 address */

        addrlen          = sizeof(struct in6_addr);
        src              = (const char *)&g_lo_ipv6addr;
        host->h_addrtype = AF_INET6;
#endif

        /* Make sure that space remains to hold the hostent structure and
         * the IP address.
         */

        if (buflen <= (sizeof(struct hostent_info_s) + addrlen)) {
            return -ERANGE;
        }

        info             = (struct hostent_info_s *)buf;
        dest             = info->hi_data;
        buflen          -= (sizeof(struct hostent_info_s) - 1);

        memset(host, 0, sizeof(struct hostent));
        memset(info, 0, sizeof(struct hostent_info_s));
        memcpy(dest, src, addrlen);

        info->hi_addrlist[0] = dest;
        host->h_addr_list    = info->hi_addrlist;
        host->h_length       = addrlen;

        dest                += addrlen;
        buflen              -= addrlen;

        /* And copy name */

        namelen = strlen(name);
        if ((namelen + 1) > buflen) {
            return -ERANGE;
        }

        strncpy(dest, name, buflen);

        /* Set the address to h_name */

        host->h_name = dest;
        return 0;
    }

    return 1;
}

static void dns_semtake(void)
{
    int errcode = 0;
    int ret;

    if (g_dns_sem.hdl == NULL)
        aos_sem_new(&g_dns_sem, 1);
    do {
        ret = aos_sem_wait(&g_dns_sem, AOS_WAIT_FOREVER);
        if (ret < 0) {
            errcode = ret;
        }
    }
    while (ret < 0 && errcode == EINTR);
}

static void dns_semgive(void)
{
    aos_sem_signal(&g_dns_sem);
}

static void dns_notify_nameserver(const struct sockaddr *addr, socklen_t addrlen)
{
    dlist_t *entry;

    dns_semtake();
    for (entry = dq_peek(&g_dns_notify); entry; entry = entry->next)
    {
        struct dns_notify_s *notify = (struct dns_notify_s *)entry;
        notify->callback(notify->arg, (struct sockaddr *)addr, addrlen);
    }

    dns_semgive();
}

static int dns_add_nameserver(const struct sockaddr *addr, socklen_t addrlen)
{
    uint16_t *pport;
    size_t copylen;

    aos_assert(addr != NULL);

    /* Copy the new server IP address into our private global data structure */

#if LWIP_IPV4
    /* Check for an IPv4 address */

    if (addr->sa_family == AF_INET) {
        /* Set up for the IPv4 address copy */

        copylen = sizeof(struct sockaddr_in);
        pport   = &g_dns_server.ipv4.sin_port;
    }
    else
#endif

#if LWIP_IPV6
        /* Check for an IPv6 address */

        if (addr->sa_family == AF_INET6) {
            /* Set up for the IPv6 address copy */

            copylen = sizeof(struct sockaddr_in6);
            pport   = &g_dns_server.ipv6.sin6_port;
        }
        else
#endif
        {
            return -ENOSYS;
        }

    /* Copy the IP address */

    if (addrlen < copylen) {
        return -EINVAL;
    }

    memcpy(&g_dns_server.addr, addr, copylen);

    /* A port number of zero means to use the default DNS server port number */

    if (*pport == 0) {
        *pport = htons(DNS_DEFAULT_PORT);
    }

    /* We now have a valid DNS address */

    g_dns_address = true;
    dns_notify_nameserver(addr, addrlen);
    return OK;
}

static bool dns_initialize(void)
{
    /* Has the DNS server IP address been assigned? */

    if (!g_dns_address)
    {
#if LWIP_IPV4
        struct sockaddr_in addr4;
        int ret;

        /* No, configure the default IPv4 DNS server address */

        addr4.sin_family      = AF_INET;
        addr4.sin_port        = htons(DNS_DEFAULT_PORT);
        addr4.sin_addr.s_addr = htonl(NETDB_DNSSERVER_IPv4ADDR);

        ret = dns_add_nameserver((struct sockaddr *)&addr4,
                sizeof(struct sockaddr_in));
        if (ret < 0) {
            return false;
        }

#elif LWIP_IPV6
        struct sockaddr_in6 addr6;
        int ret;

        /* No, configure the default IPv6 DNS server address */

        addr6.sin6_family = AF_INET6;
        addr6.sin6_port   = htons(DNS_DEFAULT_PORT);
        memcpy(addr6.sin6_addr.s6_addr, g_ipv6_hostaddr, 16);

        ret = dns_add_nameserver((struct sockaddr *)&addr6,
                sizeof(struct sockaddr_in6));
        if (ret < 0) {
            return false;
        }

#else
        /* Then we are not ready to perform DNS queries */

        return false;
#endif
    }

    return true;
}

int dns_find_answer(const char *hostname, union dns_addr_u *addr,
                    int *naddr)
{
    struct dns_cache_s *entry;
#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
    struct timespec now;
    uint32_t elapsed;
#endif
    int next;
    int ndx;
    int ret;

    /* If DNS not initialized, no need to proceed */

    if (!dns_initialize()) {
        return -EAGAIN;
    }

    /* Get exclusive access to the DNS cache */

    dns_semtake();

#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
    ret = clock_gettime(CLOCK_MONOTONIC, &now);
#endif

    for (ndx = g_dns_tail; ndx != g_dns_head; ndx = next) {
        entry = &g_dns_cache[ndx];

        /* Advance the index for the next time through the loop, handling
         * wrapping to the beginning of the circular buffer.
         */

        next = ndx + 1;
        if (next >= NETDB_DNSCLIENT_ENTRIES) {
            next = 0;
        }

#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
        /* Check if this entry has expired
         * REVISIT: Does not this calculation assume that the sizeof(time_t)
         * is equal to the sizeof(uint32_t)?
         */

        elapsed = (uint32_t)now.tv_sec - (uint32_t)entry->ctime;
        if (ret >= 0 && elapsed > CONFIG_NETDB_DNSCLIENT_LIFESEC) {
            /* This entry has expired.  Increment the tail index to exclude
             * this entry on future traversals.
             */

            g_dns_tail = next;
        }
        else
#endif
        {
            /* The entry has not expired, check for a name match.  Notice that
             * because the names are truncated to NETDB_DNSCLIENT_NAMESIZE,
             * this has the possibility of aliasing two names and returning
             * the wrong entry from the cache.
             */

            if (strncmp(hostname, entry->name, NETDB_DNSCLIENT_NAMESIZE) == 0) {
                /* We have a match.  Return the resolved host address */

                /* Make sure that the address will fit in the caller-provided
                 * buffer.
                 */

                *naddr = MIN(*naddr, entry->naddr);

                /* Return the address information */

                memcpy(addr, &entry->addr, *naddr * sizeof(*addr));

                dns_semgive();
                return OK;
            }
        }
    }

    ret = -ENOENT;

    dns_semgive();
    return ret;
}

static int lib_find_answer(const char *name, struct hostent *host,
                           char *buf, size_t buflen)
{
    struct hostent_info_s *info;
    char *ptr;
    void *addrdata;
    socklen_t addrlen;
    int naddr;
    int addrtype;
    int namelen;
    int ret;
    int i;

    /* Verify that we have a buffer big enough to get started (it still may not
     * be big enough).
     */

    if (buflen <= sizeof(struct hostent_info_s)) {
        return -ERANGE;
    }

    /* Initialize buffers */

    info    = (struct hostent_info_s *)buf;
    ptr     = info->hi_data;
    buflen -= (sizeof(struct hostent_info_s) - 1);

    /* Verify again that there is space for at least one address. */

    if (buflen < sizeof(union dns_addr_u)) {
        return -ERANGE;
    }

    memset(host, 0, sizeof(struct hostent));
    memset(info, 0, sizeof(struct hostent_info_s));

    /* Try to get the host address using the DNS name server */

    naddr = buflen / sizeof(union dns_addr_u);
    ret = dns_find_answer(name, (union dns_addr_u *)ptr, &naddr);
    if (ret < 0) {
        /* No, nothing found in the cache */

        return ret;
    }

    aos_assert(naddr <= 1);

    /* Get the address type. */

    for (i = 0; i < naddr; i++) {
#if LWIP_IPV4
#if LWIP_IPV6
        if (((struct sockaddr_in *)ptr)->sin_family == AF_INET)
#endif
        {
            addrlen  = sizeof(struct sockaddr_in);
            addrtype = AF_INET;
            addrdata = &((struct sockaddr_in *)ptr)->sin_addr;
        }
#endif

#if LWIP_IPV6
#if LWIP_IPV4
        else
#endif
        {
            addrlen  = sizeof(struct sockaddr_in6);
            addrtype = AF_INET6;
            addrdata = &((struct sockaddr_in6 *)ptr)->sin6_addr;
        }
#endif

        /* REVISIT: This assumes addresses are all either IPv4 or IPv6. */

        info->hi_addrlist[i] = addrdata;
        host->h_addrtype     = addrtype;
        host->h_length       = addrlen;

        ptr                 += sizeof(union dns_addr_u);
        buflen              -= sizeof(union dns_addr_u);
    }

    host->h_addr_list        = info->hi_addrlist;

    /* And copy name */

    namelen = strlen(name);
    if ((namelen + 1) > buflen) {
        return -ERANGE;
    }

    strncpy(ptr, name, buflen);

    /* Set the address to h_name */

    host->h_name = ptr;
    return OK;
}

static int dns_bind(void)
{
    struct timeval tv;
    int sd;
    int ret;

    /* Has the DNS client been properly initialized? */

    if (!dns_initialize()) {
        return -EDESTADDRREQ;
    }

    /* Create a new socket */

    sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        return sd;
    }

    /* Set up a receive timeout */

    tv.tv_sec  = 3;
    tv.tv_usec = 0;

    ret = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    if (ret < 0) {
        closesocket(sd);
        return ret;
    }

    return sd;
}

static int dns_foreach_nameserver(dns_callback_t callback, void *arg)
{
    int ret = OK;

    if (g_dns_address) {
#if LWIP_IPV4
        /* Check for an IPv4 address */

        if (g_dns_server.addr.sa_family == AF_INET) {
            /* Perform the callback */

            ret = callback(arg, (struct sockaddr *)&g_dns_server.ipv4,
                    sizeof(struct sockaddr_in));
        }
        else
#endif

#if LWIP_IPV6
            /* Check for an IPv6 address */

            if (g_dns_server.addr.sa_family == AF_INET6) {
                /* Perform the callback */

                ret = callback(arg, (struct sockaddr *)&g_dns_server.ipv6,
                        sizeof(struct sockaddr_in6));
            }
            else
#endif
            {
                ret = -ENOSYS;
            }
    }

    return ret;
}

static inline uint16_t dns_alloc_id(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint32_t)ts.tv_nsec + ((uint32_t)ts.tv_nsec >> 16);
}

static int dns_send_query(int sd, const char *name,
                          union dns_addr_u *uaddr, uint16_t rectype,
                          struct dns_query_info_s *qinfo)
{
    struct dns_header_s *hdr;
    uint8_t *dest;
    uint8_t *nptr;
    char *qname;
    char *qptr;
    const char *src;
    uint8_t buffer[SEND_BUFFER_SIZE];
    uint16_t id;
    socklen_t addrlen;
    int ret;
    int len;
    int n;

    /* Get a new ID for query */

    id = dns_alloc_id();

    /* Initialize the request header */

    hdr               = (struct dns_header_s *)buffer;
    memset(hdr, 0, sizeof(*hdr));
    hdr->id           = htons(id);
    hdr->flags1       = DNS_FLAG1_RD;
    hdr->numquestions = htons(1);

    /* Convert hostname into suitable query format.
     *
     * There is space for NETDB_DNSCLIENT_NAMESIZE
     * plus one pre-pended name length and NUL-terminator
     * (other pre-pended name lengths replace dots).
     */

    src   = name - 1;
    dest  = buffer + sizeof(*hdr);
    qname = qinfo->qname;
    len   = 0;
    do
    {
        /* Copy the name string to both query and saved info. */

        src++;
        nptr = dest++;
        qptr = qname++;
        len++;

        for (n = 0;
                *src != '.' && *src != 0 &&
                len <= NETDB_DNSCLIENT_NAMESIZE;
                src++) {
            *dest++  = *(uint8_t *)src;
            *qname++ = *(uint8_t *)src;
            n++;
            len++;
        }

        /* Pre-pend the name length */

        *nptr = n;
        *qptr = n;
    }
    while (*src != '\0' && len <= NETDB_DNSCLIENT_NAMESIZE);

    /* Add NUL termination */

    *dest++  = '\0';
    *qname++ = '\0';

    /* Store name length to saved info */

    aos_assert(len <= NETDB_DNSCLIENT_NAMESIZE + 1);
    aos_assert(qname - qinfo->qname == len + 1);
    qinfo->qnamelen = len;

    /* Add DNS record type, and DNS class */

    *dest++ = (rectype >> 8);        /* DNS record type (big endian) */
    *dest++ = (rectype & 0xff);
    *dest++ = (DNS_CLASS_IN >> 8);   /* DNS record class (big endian) */
    *dest++ = (DNS_CLASS_IN & 0xff);

    qinfo->rectype = htons(rectype);
    qinfo->id      = hdr->id;

    /* Send the request */

#if LWIP_IPV4
#if LWIP_IPV6
    if (uaddr->ipv4.sin_family == AF_INET)
#endif
    {
        addrlen           = sizeof(struct sockaddr_in);
        qinfo->u.srv_ipv4 = uaddr->ipv4.sin_addr;
        qinfo->srv_port   = uaddr->ipv4.sin_port;
    }
#endif

#if LWIP_IPV6
#if LWIP_IPV4
    else
#endif
    {
        addrlen           = sizeof(struct sockaddr_in6);
        qinfo->u.srv_ipv6 = uaddr->ipv6.sin6_addr;
        qinfo->srv_port   = uaddr->ipv6.sin6_port;
    }
#endif

    ret = sendto(sd, buffer, dest - buffer, 0, &uaddr->addr, addrlen);

    /* Return the negated errno value on sendto failure */

    if (ret < 0) {
        return ret;
    }

    return OK;
}

static uint8_t *dns_parse_name(uint8_t *query, uint8_t *queryend)
{
    uint8_t n;

    while (query < queryend) {
        n = *query++;

        /* Check for a leading or trailing pointer.*/

        if ((n & 0xc0) != 0) {
            /* Eat second pointer byte and terminate search */

            query++;
            break;
        }

        /* Check for final label with zero-length */

        if (!n) {
            break;
        }

        /* Eat non-empty label */

        query += n;
    }

    if (query >= queryend) {
        /* Always return `queryend` in case of errors */
        query = queryend;
    }

    return query;
}

static int dns_recv_response(int sd, union dns_addr_u *addr, int *naddr,
                             struct dns_query_info_s *qinfo)
{
    uint8_t *p;
    uint8_t *nameptr;
    uint8_t *namestart;
    uint8_t *endofbuffer;
    char buffer[RECV_BUFFER_SIZE];
    struct dns_answer_s *ans;
    struct dns_answer_s tmp_ans;
    struct dns_header_s *hdr;
    struct dns_question_s *que;
    struct dns_question_s tmp_que;
    uint16_t nquestions;
    uint16_t nanswers;
    union dns_addr_u recvaddr;
    socklen_t raddrlen;
    int naddr_read;
    int ret, i;

    /* Receive the response */

    raddrlen = sizeof(recvaddr.addr);
    ret      = recvfrom(sd, buffer, RECV_BUFFER_SIZE, 0,
                        &recvaddr.addr, &raddrlen);
    if (ret < 0) {
        return ret;
    }

#if LWIP_IPV4
    /* Check for an IPv4 address */

    if (recvaddr.addr.sa_family == AF_INET) {
        if (memcmp(&recvaddr.ipv4.sin_addr, &qinfo->u.srv_ipv4,
                    sizeof(recvaddr.ipv4.sin_addr)) != 0) {
            /* Not response from DNS server. */

            return -EBADMSG;
        }

        if (recvaddr.ipv4.sin_port != qinfo->srv_port) {
            /* Not response from DNS server. */

            return -EBADMSG;
        }
    }
#endif
#if LWIP_IPV6
    /* Check for an IPv6 address */

    if (recvaddr.addr.sa_family == AF_INET6) {
        if (memcmp(&recvaddr.ipv6.sin6_addr, &qinfo->u.srv_ipv6,
                    sizeof(recvaddr.ipv6.sin6_addr)) != 0) {
            /* Not response from DNS server. */

            return -EBADMSG;
        }

        if (recvaddr.ipv6.sin6_port != qinfo->srv_port) {
            /* Not response from DNS server. */

            return -EBADMSG;
        }
    }
#endif

    if (ret < sizeof(*hdr)) {
        /* DNS header can't fit in received data */

        return -EILSEQ;
    }

    hdr         = (struct dns_header_s *)buffer;
    endofbuffer = (uint8_t*)buffer + ret;

    /* Check for error */

    if ((hdr->flags2 & DNS_FLAG2_ERR_MASK) != 0) {
        return -EPROTO;
    }

    /* Check for matching ID. */

    if (hdr->id != qinfo->id) {
        return -EBADMSG;
    }

    /* We only care about the question(s) and the answers. The authrr
     * and the extrarr are simply discarded.
     */

    nquestions = htons(hdr->numquestions);
    nanswers   = htons(hdr->numanswers);

    /* We only ever send queries with one question. */

    if (nquestions != 1) {
        return -EBADMSG;
    }

    /* Skip the name in the answer, but do check that it
     * matches against the name in the question.
     */

    namestart = (uint8_t *)buffer + sizeof(*hdr);
    nameptr   = dns_parse_name(namestart, endofbuffer);
    if (nameptr == endofbuffer) {
        return -EILSEQ;
    }

    /* Since dns_parse_name() skips any pointer bytes,
     * we cannot compare for equality here.
     */

    if (nameptr - namestart < qinfo->qnamelen) {
        return -EBADMSG;
    }

    /* qname is NUL-terminated and we must include NUL to the comparison. */

    if (memcmp(namestart, qinfo->qname, qinfo->qnamelen + 1) != 0) {
        return -EBADMSG;
    }

    p = (uint8_t *)&tmp_que;
    /* Validate query type and class */
    for (i = 0; i < sizeof(tmp_que); i++) {
        p[i] = nameptr[i];
    }
    que = &tmp_que;

    if (que->type  != qinfo->rectype ||
            que->class != htons(DNS_CLASS_IN)) {
        return -EBADMSG;
    }

    /* Skip over question */

    nameptr += sizeof(struct dns_question_s);

    ret = OK;
    naddr_read = 0;

    for (; nanswers > 0; nanswers--) {
        /* Each answer starts with a name */

        nameptr = dns_parse_name(nameptr, endofbuffer);
        if (nameptr == endofbuffer) {
            ret = -EILSEQ;
            break;
        }

        p = (uint8_t *)&tmp_ans;
        for (i = 0; i < sizeof(tmp_ans); i++) {
            p[i] = nameptr[i];
        }
        ans = &tmp_ans;

        /* Check for IPv4/6 address type and Internet class. Others are
         * discarded.
         */

#if LWIP_IPV4
        if (ans->type  == htons(DNS_RECTYPE_A) &&
                ans->class == htons(DNS_CLASS_IN) &&
                ans->len   == htons(4) &&
                nameptr + 10 + 4 <= endofbuffer) {
            nameptr += 10 + 4;

            if (naddr_read < *naddr) {
                struct sockaddr_in *inaddr;

                inaddr                   = (struct sockaddr_in *)&addr[naddr_read].addr;
                inaddr->sin_family       = AF_INET;
                inaddr->sin_port         = 0;
                inaddr->sin_addr.s_addr  = ans->u.ipv4.s_addr;

                naddr_read++;
                if (naddr_read >= *naddr) {
                    break;
                }
            } else {
                ret = -ERANGE;
                break;
            }
        }
        else
#endif
#if LWIP_IPV6
            if (ans->type  == htons(DNS_RECTYPE_AAAA) &&
                    ans->class == htons(DNS_CLASS_IN) &&
                    ans->len   == htons(16) &&
                    nameptr + 10 + 16 <= endofbuffer) {
                nameptr += 10 + 16;

                if (naddr_read < *naddr) {
                    struct sockaddr_in6 *inaddr;

                    inaddr                   = (struct sockaddr_in6 *)&addr[naddr_read].addr;
                    inaddr->sin6_family      = AF_INET6;
                    inaddr->sin6_port        = 0;
                    memcpy(inaddr->sin6_addr.s6_addr, ans->u.ipv6.s6_addr, 16);

                    naddr_read++;
                    if (naddr_read >= *naddr)
                    {
                        break;
                    }
                } else {
                    ret = -ERANGE;
                    break;
                }
            }
            else
#endif
            {
                nameptr = nameptr + 10 + htons(ans->len);
            }
    }

    if (naddr_read == 0 && ret == OK) {
        ret = -EADDRNOTAVAIL;
    }

    *naddr = naddr_read;
    return (naddr_read > 0) ? naddr_read : ret;
}

void dns_save_answer(const char *hostname,
                     const union dns_addr_u *addr, int naddr)
{
    struct dns_cache_s *entry;
#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
  struct timespec now;
#endif
    int next;
    int ndx;

    naddr = MIN(naddr, 1);
    aos_assert(naddr >= 1 && naddr <= UCHAR_MAX);

    /* Get exclusive access to the DNS cache */

    dns_semtake();

    /* Get the index to the new head of the list */

    ndx  = g_dns_head;
    next = ndx + 1;
    if (next >= NETDB_DNSCLIENT_ENTRIES) {
        next = 0;
    }

    /* If the next head pointer would match the tail index, then increment
     * the tail index, discarding the oldest mapping in the cache.
     */

    if (next == g_dns_tail) {
        int tmp = g_dns_tail + 1;
        if (tmp >= NETDB_DNSCLIENT_ENTRIES) {
            tmp = 0;
        }

        g_dns_tail = tmp;
    }

    /* Save the answer in the cache */

    entry = &g_dns_cache[ndx];

#if CONFIG_NETDB_DNSCLIENT_LIFESEC > 0
  /* Get the current time, using CLOCK_MONOTONIC if possible */

  (void)clock_gettime(CLOCK_MONOTONIC, &now);
  entry->ctime = (time_t)now.tv_sec;
#endif

    strncpy(entry->name, hostname, NETDB_DNSCLIENT_NAMESIZE);
    memcpy(&entry->addr, addr, naddr * sizeof(*addr));
    entry->naddr = naddr;
    /* Save the updated head index */

    g_dns_head = next;
    dns_semgive();
}

static int dns_query_callback(void *arg, struct sockaddr *addr,
                              socklen_t addrlen)
{
    struct dns_query_s *query = (struct dns_query_s *)arg;
    struct dns_query_info_s qinfo;
    int retries;
    int ret;

    /* Loop while receive timeout errors occur and there are remaining
     * retries.
     */

    for (retries = 0; retries < NETDB_DNSCLIENT_RETRIES; retries++) {
#if LWIP_IPV4
        /* Is this an IPv4 address? */

        if (addr->sa_family == AF_INET) {
            /* Yes.. verify the address size */

            if (addrlen < sizeof(struct sockaddr_in)) {
                /* Return zero to skip this address and try the next
                 * nameserver address in resolv.conf.
                 */

                query->result = -EINVAL;
                return 0;
            }

            /* Send the IPv4 query */

            ret = dns_send_query(query->sd, query->hostname,
                    (union dns_addr_u *)addr,
                    DNS_RECTYPE_A, &qinfo);
            if (ret < 0) {
                /* Return zero to skip this address and try the next
                 * nameserver address in resolv.conf.
                 */

                query->result = ret;
                return 0;
            }

            /* Obtain the IPv4 response */

            ret = dns_recv_response(query->sd, query->addr, query->naddr,
                    &qinfo);
            if (ret >= 0) {
                /* IPv4 response received successfully */

                /* Save the answer in the DNS cache */

                dns_save_answer(query->hostname, query->addr,
                        *query->naddr);
                /* Return 1 to indicate to (1) stop the traversal, and (2)
                 * indicate that the address was found.
                 */

                return 1;
            }

            /* Handle errors */

            if (ret == -EADDRNOTAVAIL) {
                /* The IPv4 address is not available.  Return zero to
                 * continue the tranversal with the next nameserver
                 * address in resolv.conf.
                 */

                query->result = -EADDRNOTAVAIL;
                return 0;
            }
            else if (ret != -ETIMEDOUT) {
                /* Some failure other than receive timeout occurred.  Return
                 * zero to skip this address and try the next nameserver
                 * address in resolv.conf.
                 */

                query->result = ret;
                return 0;
            }
        }
        else
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
            /* Is this an IPv6 address? */

            if (addr->sa_family == AF_INET6) {
                /* Yes.. verify the address size */

                if (addrlen < sizeof(struct sockaddr_in6)) {
                    /* Return zero to skip this address and try the next
                     * nameserver address in resolv.conf.
                     */

                    query->result = -EINVAL;
                    return 0;
                }

                /* Send the IPv6 query */

                ret = dns_send_query(query->sd, query->hostname,
                        (union dns_addr_u *)addr,
                        DNS_RECTYPE_AAAA, &qinfo);
                if (ret < 0) {
                    /* Return zero to skip this address and try the next
                     * nameserver address in resolv.conf.
                     */

                    query->result = ret;
                    return 0;
                }

                /* Obtain the IPv6 response */

                ret = dns_recv_response(query->sd, query->addr, query->naddr,
                        &qinfo);

                if (ret >= 0) {
                    /* IPv6 response received successfully */

                    /* Save the answer in the DNS cache */

                    dns_save_answer(query->hostname, query->addr, *query->naddr);

                    /* Return 1 to indicate to (1) stop the traversal, and (2)
                     * indicate that the address was found.
                     */

                    return 1;
                }

                /* Handle errors */

                if (ret == -EADDRNOTAVAIL) {
                    /* The IPv6 address is not available.  Return zero to
                     * continue the tranversal with the next nameserver
                     * address in resolv.conf.
                     */

                    query->result = -EADDRNOTAVAIL;
                    return 0;
                } else if (ret != -ETIMEDOUT) {
                    /* Some failure other than receive timeout occurred.  Return
                     * zero to skip this address and try the next nameserver
                     * address in resolv.conf.
                     */

                    query->result = ret;
                    return 0;
                }
            }
            else
#endif
            {
                /* Unsupported address family. Return zero to continue the
                 * tranversal with the next nameserver address in resolv.conf.
                 */

                return 0;
            }
    }

    /* We tried and could not communicate with this nameserver. Perhaps it
     * is down?  Return zero to continue with the next address in the
     * resolv.conf file.
     */

    query->result = -ETIMEDOUT;
    return 0;
}

static int dns_query(int sd, const char *hostname, union dns_addr_u *addr,
                     int *naddr)
{
    struct dns_query_s query;
    int ret;

    /* Set up the query info structure */

    query.sd       = sd;
    query.result   = -EADDRNOTAVAIL;
    query.hostname = hostname;
    query.addr     = addr;
    query.naddr    = naddr;

    /* Perform the query. dns_foreach_nameserver() will return:
     *
     *  1 - The query was successful.
     *  0 - Look up failed
     * <0 - Some other failure (?, shouldn't happen)
     */

    ret = dns_foreach_nameserver(dns_query_callback, &query);
    if (ret > 0) {
        /* The lookup was successful */

        ret = OK;
    }
    else if (ret == 0) {
        ret = query.result;
    }

    return ret;
}

static int lib_dns_query(const char *hostname,
                         union dns_addr_u *addr, int *naddr)
{
    int sd;
    int ret;

    /* Create and bind a socket to the DNS server */

    sd = dns_bind();
    if (sd < 0) {
        return sd;
    }

    /* Perform the query to get the IP address */

    ret = dns_query(sd, hostname, addr, naddr);

    /* Release the socket */

    closesocket(sd);
    return ret;
}

static int lib_dns_lookup(const char *name, struct hostent *host,
                          char *buf, size_t buflen)
{
    struct hostent_info_s *info;
    char *ptr;
    void *addrdata;
    socklen_t addrlen;
    int naddr;
    int addrtype;
    int namelen;
    int ret;
    int i;

    /* Verify that we have a buffer big enough to get started (it still may not
     * be big enough).
     */

    if (buflen <= sizeof(struct hostent_info_s)) {
        return -ERANGE;
    }

    /* Initialize buffers */

    info    = (struct hostent_info_s *)buf;
    ptr     = info->hi_data;
    buflen -= (sizeof(struct hostent_info_s) - 1);

    /* Verify again that there is space for at least one address. */

    if (buflen < sizeof(union dns_addr_u)) {
        return -ERANGE;
    }

    memset(host, 0, sizeof(struct hostent));
    memset(info, 0, sizeof(struct hostent_info_s));

    /* Try to get the host address using the DNS name server */

    naddr = buflen / sizeof(union dns_addr_u);
    ret = lib_dns_query(name, (union dns_addr_u *)ptr, &naddr);
    if (ret < 0) {
        return ret;
    }

    /* We can read more than maximum, limit here. */

    naddr = MIN(naddr, 1);

    for (i = 0; i < naddr; i++) {
#if LWIP_IPV4
#if LWIP_IPV6
        if (((struct sockaddr_in *)ptr)->sin_family == AF_INET)
#endif
        {
            addrlen  = sizeof(struct sockaddr_in);
            addrtype = AF_INET;
            addrdata = &((struct sockaddr_in *)ptr)->sin_addr;
        }
#endif

#if LWIP_IPV6
#if LWIP_IPV4
        else
#endif
        {
            addrlen  = sizeof(struct sockaddr_in6);
            addrtype = AF_INET6;
            addrdata = &((struct sockaddr_in6 *)ptr)->sin6_addr;
        }
#endif

        /* REVISIT: This assumes addresses are all either IPv4 or IPv6. */

        info->hi_addrlist[i] = addrdata;
        host->h_addrtype     = addrtype;
        host->h_length       = addrlen;

        ptr                 += sizeof(union dns_addr_u);
        buflen              -= sizeof(union dns_addr_u);
    }

    host->h_addr_list        = info->hi_addrlist;

    /* And copy name */

    namelen = strlen(name);
    if ((namelen + 1) > buflen) {
        return -ERANGE;
    }

    strncpy(ptr, name, buflen);

    /* Set the address to h_name */

    host->h_name = ptr;

    return OK;
}

static int getservbyname_r(const char *name, const char *proto,
                              struct servent *result_buf, char *buf,
                              size_t buflen, struct servent **result)
{
    char *end = "";
    int protocol;
    int i;

    aos_assert(name != NULL && buf != NULL);
    aos_assert(result_buf != NULL && result != NULL);

    /* Linux man page says result must be NULL in case of failure. */

    *result = NULL;

    /* We need space for two pointers for hostalias strings. */

    if (buflen < 2 * sizeof(char *)) {
        return ERANGE;
    }

    /* Numeric port number strings are not service records. */

    strtoul(name, &end, 10);
    if (*end == '\0') {
        return ENOENT;
    }

    if (proto == NULL) {
        protocol = 0;
    } else if (strcmp(proto, "tcp") == 0) {
        protocol = IPPROTO_TCP;
    }
    else if (strcmp(proto, "udp") == 0) {
        protocol = IPPROTO_UDP;
    } else {
        return EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(g_services_db); i++) {
        if (strcmp(name, g_services_db[i].s_name) == 0 &&
            (protocol == 0 || protocol == g_services_db[i].s_protocol)) {
            result_buf->s_name = (char *)name;
            result_buf->s_aliases = (void *)buf;
            result_buf->s_aliases[0] = (char *)name;
            result_buf->s_aliases[1] = NULL;
            result_buf->s_port = htons(g_services_db[i].s_port);

            if (g_services_db[i].s_protocol == IPPROTO_TCP) {
                result_buf->s_proto = "tcp";
            } else {
                result_buf->s_proto = "udp";
            }

            *result = result_buf;
            return 0;
        }
    }

    return ENOENT;
}

static struct servent *getservbyname(const char *name, const char *proto)
{
    static struct servent ent;
    static char *buf[2];
    struct servent *res;
    int ret;

    ret = getservbyname_r(name, proto, &ent, (void *)buf, sizeof buf, &res);
    return (ret != OK) ? NULL : res;
}

static struct ai_s *alloc_ai(int family, int socktype, int protocol,
                             int port, void *addr)
{
    struct ai_s *ai;
    socklen_t addrlen;

#if LWIP_IPV4
    addrlen = sizeof(struct sockaddr_in);
#elif LWIP_IPV6
    addrlen = sizeof(struct sockaddr_in6);
#endif

    ai = aos_zalloc(sizeof(struct ai_s));
    if (ai == NULL) {
        return ai;
    }

    ai->ai.ai_addr            = (struct sockaddr *)&ai->sa;
    ai->ai.ai_addrlen         = addrlen;
    ai->ai.ai_addr->sa_family = ai->ai.ai_family = family;
    ai->ai.ai_socktype        = socktype;
    ai->ai.ai_protocol        = protocol;

    switch (family) {
#if LWIP_IPV4
        case AF_INET:
            ai->sa.sin.sin_family = AF_INET;
            ai->sa.sin.sin_port   = port;  /* Already network order */
            memcpy(&ai->sa.sin.sin_addr, addr, sizeof(ai->sa.sin.sin_addr));
            break;
#endif
#if LWIP_IPV6
        case AF_INET6:
            ai->sa.sin6.sin6_family = AF_INET6;
            ai->sa.sin6.sin6_port   = port;  /* Already network order */
            memcpy(&ai->sa.sin6.sin6_addr, addr, sizeof(ai->sa.sin6.sin6_addr));
            break;
#endif
    }

    return ai;
}

struct hostent *gethostbyname_impl(const char *name)
{
    struct hostent *host = &g_hostent;
    char *buf = g_hostbuffer;
    size_t buflen = NETDB_LENGTH;
    int *h_errnop = &h_errno;
    int ret;

    aos_assert(name != NULL && host != NULL && buf != NULL);

    /* Make sure that the h_errno has a non-error code */

    if (h_errnop) {
        *h_errnop = 0;
    }

    /* Check for a numeric hostname */

    if (lib_numeric_address(name, host, buf, buflen) == 0) {
        /* Yes.. we are done */

        return host;
    }

    /* Check for the local loopback host name */

    if (lib_localhost(name, host, buf, buflen) == 0) {
        /* Yes.. we are done */

        return host;
    }

    /* Try to find the name in the HOSTALIASES environment variable */
    /* REVISIT: Not implemented */

    /* Check if we already have this hostname mapping cached */

    ret = lib_find_answer(name, host, buf, buflen);
    if (ret >= 0) {
        /* Found the address mapping in the cache */

        return host;
    }

    /* Try to get the host address using the DNS name server */

    ret = lib_dns_lookup(name, host, buf, buflen);
    if (ret >= 0) {
        /* Successful DNS lookup! */

        return host;
    }

    /* The host file file is not supported.  The host name mapping was not
     * found from any lookup heuristic
     */

    if (h_errnop) {
        *h_errnop = HOST_NOT_FOUND;
    }

    return NULL;
}


void freeaddrinfo_impl(struct addrinfo *ai)
{
    struct addrinfo *p;

    aos_assert(ai != NULL);

    do {
        p = ai;
        ai = ai->ai_next;
        aos_free(p);
    }
    while (ai != NULL);
}

int getaddrinfo_impl(const char *hostname, const char *servname,
                     const struct addrinfo *hint, struct addrinfo **res)
{
    int family = AF_UNSPEC;
    int port = 0;
    int flags = 0;
    int proto = 0;
    int socktype = 0;
    struct hostent *hp;
    struct ai_s *ai;
    struct ai_s *prev_ai = NULL;
    const int valid_flags = AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST |
                            AI_NUMERICSERV | AI_V4MAPPED | AI_ALL |
                            AI_ADDRCONFIG;
    int i;

    if (hostname == NULL && servname == NULL) {
        return EAI_NONAME;
    }

    if (hint) {
        family   = hint->ai_family;
        flags    = hint->ai_flags;
        proto    = hint->ai_protocol;
        socktype = hint->ai_socktype;

        if ((flags & valid_flags) != flags) {
            return EAI_BADFLAGS;
        }

        if (family != AF_INET &&
            family != AF_INET6 &&
            family != AF_UNSPEC) {
            return EAI_FAMILY;
        }
    }

    if (servname != NULL) {
        char *endp;
        struct servent *sp;

        port = strtol(servname, &endp, 10);
        if (port > 0 && port <= 65535 && *endp == '\0') {
            /* Force network byte order */

            port = htons(port);
        } else if ((flags & AI_NUMERICSERV) != 0) {
            return EAI_NONAME;
        } else if ((sp = getservbyname(servname, NULL)) != NULL) {
            /* The sp_port field of struct servent is required to
             * be in network byte order (per OpenGroup.org)
             */

            port = sp->s_port;
        } else {
            return EAI_SERVICE;
        }
    }

    *res = NULL;

    /* If hostname is not NULL, then the AI_PASSIVE flag is ignored. */

    if ((flags & AI_PASSIVE) != 0 && hostname == NULL) {
        struct in6_addr addr;

        memset(&addr, 0, sizeof(struct in6_addr));

#if LWIP_IPV4
        if (family == AF_INET || family == AF_UNSPEC) {
            ai = alloc_ai(AF_INET, socktype, proto, port, (void *)&addr);
            if (ai == NULL) {
                return EAI_MEMORY;
            }

            *res = (struct addrinfo *)ai;
        }
#endif
#if LWIP_IPV6
        if (family == AF_INET6 || family == AF_UNSPEC) {
            ai = alloc_ai(AF_INET6, socktype, proto, port, (void *)&addr);
            if (ai == NULL) {
                return (*res != NULL) ? OK : EAI_MEMORY;
            }

            /* Can return both IPv4 and IPv6 loopback. */

            if (*res != NULL) {
                (*res)->ai_next = (struct addrinfo *)ai;
            } else {
                *res = (struct addrinfo *)ai;
            }
        }
#endif
        return OK;
    }

    if (hostname == NULL) {
        /* Local service. */

#if LWIP_IPV4
        if (family == AF_INET || family == AF_UNSPEC) {
            ai = alloc_ai(AF_INET, socktype, proto, port, (void *)&g_lo_ipv4addr);
            if (ai == NULL) {
                return EAI_MEMORY;
            }

            *res = (struct addrinfo *)ai;
        }
#endif
#if LWIP_IPV6
        if (family == AF_INET6 || family == AF_UNSPEC) {
            ai = alloc_ai(AF_INET6, socktype, proto, port, (void *)&g_lo_ipv6addr);
            if (ai == NULL) {
                return (*res != NULL) ? OK : EAI_MEMORY;
            }

            /* Can return both IPv4 and IPv6 loopback. */

            if (*res != NULL) {
                (*res)->ai_next = (struct addrinfo *)ai;
            } else {
                *res = (struct addrinfo *)ai;
            }
        }
#endif
        return (*res != NULL) ? OK : EAI_FAMILY;
    }

    /* REVISIT: no check for AI_NUMERICHOST flag. */

    /* REVISIT: use gethostbyname_r with own buffer of refactor all
     * public APIs to use internal lookup function.
     */

    hp = gethostbyname_impl(hostname);
    if (hp && hp->h_name && hp->h_name[0] && hp->h_addr_list[0]) {
        for (i = 0; hp->h_addr_list[i]; i++) {
            if (family != AF_UNSPEC && hp->h_addrtype != family) {
                /* Filter by protocol family. */

                continue;
            }

            /* REVISIT: filter by socktype and protocol not implemented. */

            ai = alloc_ai(hp->h_addrtype, socktype, proto, port, hp->h_addr_list[i]);
            if (ai == NULL) {
                if (*res) {
                    freeaddrinfo_impl(*res);
                }

                return EAI_MEMORY;
            }

            /* REVISIT: grok canonical name.
             *
             * OpenGroup: "if the canonical name is not available, then ai_canonname shall
             * refer to the hostname argument or a string with the same contents."
             */

            ai->ai.ai_canonname = (char *)hostname;

            /* Add result to linked list.
             * TODO: RFC 3484/6724 destination address sort not implemented.
             */

            if (prev_ai != NULL) {
                prev_ai->ai.ai_next = (struct addrinfo *)ai;
            } else {
                *res = (struct addrinfo *)ai;
            }

            prev_ai = ai;
        }

        return OK;
    }

    return EAI_AGAIN;
}
