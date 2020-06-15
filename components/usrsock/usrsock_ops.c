/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_ops.c
 * @brief    user sock main functions
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/log.h>

#include "devif.h"
#include "usrsock.h"
#include "net_usrsock.h"
#include "usrsock2lwip.h"
#include "lwip/netdb.h"
#include "usrsock_helper.h"

#define NUM_SOCKETS MEMP_NUM_NETCONN

#define USRSOCK_POLLIN       (0x01)  /* NuttX does not make priority distinctions */
#define USRSOCK_POLLOUT      (0x02)  /* NuttX does not make priority distinctions */

#define USRSOCK_POLLERR      (0x04)
#define USRSOCK_POLLHUP      (0x08)

#define USRSOCK_POLLFD       (0x00)
#define USRSOCK_POLLMASK     (0xC0)

struct usrsock_s
{
    uint8_t       s_domain;    /* IP domain: PF_INET, PF_INET6, or PF_PACKET */
    uint8_t       s_type;      /* Protocol type: Only SOCK_STREAM or SOCK_DGRAM */
    void         *s_conn;      /* Connection: struct tcp_conn_s or udp_conn_s */
};

typedef struct usrsock_sockaddr_s
{
    uint16_t    sa_family;       /* Address family: See AF_* definitions */
    char        sa_data[14];     /* 14-bytes data (actually variable length) */
} usrsock_sockaddr_t;

struct usrsock_pollfd
{
    /* Standard fields */

    int          fd;      /* The descriptor being polled */
    uint8_t      events;  /* The input event flags */
    uint8_t      revents; /* The output event flags */

    /* Non-standard fields used internally by NuttX */

    void        *ptr;     /* The psock or file being polled */
    aos_sem_t   *sem;     /* Pointer to semaphore used to post output event */
    void        *priv;    /* For use by drivers */
};

struct usrsock_poll_s
{
    struct usrsock_conn_s *conn; /* Needed to handle loss of connection */
    struct usrsock_pollfd *fds;  /* Needed to handle poll events */
    struct devif_callback_s *cb; /* Needed to teardown the poll */
};

struct usrsock_s usrsockets[NUM_SOCKETS];

typedef struct opt_map_s {
    int ori_opt;
    int map_opt;
} opt_map_t;

static opt_map_t opt_map[] = {
    { SO_DEBUG,       2 },
    { SO_ACCEPTCONN,  0 },
    { SO_DONTROUTE,   3 },
    { SO_LINGER   ,   6 },
    { SO_OOBINLINE,   7 },
    { SO_REUSEADDR,  11 },
    { SO_SNDBUF,     12 },
    { SO_RCVBUF,      8 },
    { SO_SNDLOWAT,   13 },
    { SO_RCVLOWAT,    9 },
    { SO_SNDTIMEO,   14 },
    { SO_RCVTIMEO,   10 },
    { SO_ERROR,       4 },
    { SO_TYPE,       15 },
};

static int sock_opt_map(int opt)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(opt_map); i++) {
        if (opt == opt_map[i].ori_opt) {
            return opt_map[i].map_opt;
        }
    }

    return -1;
}

static long net_timeval2msec(const struct timeval *tv)
{
    return tv->tv_sec * MSEC_PER_SEC + tv->tv_usec / USEC_PER_MSEC;
}

/****************************************************************************
 * Name: do_socket_request
 ****************************************************************************/

static int do_socket_request(struct usrsock_conn_s *conn, int domain,
                             int type, int protocol)
{
    struct usrsock_request_socket_s req =
    {
    };
    struct iovec bufs[1];

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_SOCKET;
    req.domain = domain;
    req.type = type;
    req.protocol = protocol;

    if (req.domain != domain) {
        return -EINVAL;
    }

    if (req.type != type) {
        return -EINVAL;
    }

    if (req.protocol != protocol) {
        return -EINVAL;
    }

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

static int alloc_usrsockets(struct usrsock_conn_s *conn)
{
    int i;

    /* allocate a new socket identifier */
    for (i = 0; i < NUM_SOCKETS; ++i) {
        /* Protect socket array */

        if (!usrsockets[i].s_conn) {
            usrsockets[i].s_conn = conn;
            /* The socket is not yet known to anyone, so no need to protect
               after having marked it as used. */
            usrsockets[i].s_type = SOCK_USRSOCK_TYPE;
            usrsockets[i].s_domain = PF_USRSOCK_DOMAIN;
            return i + USRSOCK_SOCKET_OFFSET;
        }
    }

    return -1;
}

static uint16_t socket_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "socket aborted.\n");

        pstate->result = -ENETDOWN;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "socket request completed.\n");

        pstate->result = conn->resp.result;
        if (pstate->result >= 0) {
            /* We might start getting events for this socket right after
             * returning to daemon, so setup 'conn' already here.
             */

            conn->state   = USRSOCK_CONN_STATE_READY;
            conn->usockid = pstate->result;
        }

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: usrsocket_socket
 *
 * Description:
 *   socket() creates an endpoint for communication and returns a socket
 *   structure.
 *
 * Input Parameters:
 *   domain   - (see sys/socket.h)
 *   type     - (see sys/socket.h)
 *   protocol - (see sys/socket.h)
 *
 * Returned Value:
 *   0 on success; negative error-code on error
 *
 *   EACCES
 *     Permission to create a socket of the specified type and/or protocol
 *     is denied.
 *   EAFNOSUPPORT
 *     The implementation does not support the specified address family.
 *   EINVAL
 *     Unknown protocol, or protocol family not available.
 *   EMFILE
 *     Process file table overflow.
 *   ENFILE
 *     The system limit on the total number of open files has been reached.
 *   ENOBUFS or ENOMEM
 *     Insufficient memory is available. The socket cannot be created until
 *     sufficient resources are freed.
 *   EPROTONOSUPPORT
 *     The protocol type or the specified protocol is not supported within
 *     this domain.
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_socket(int domain, int type, int protocol)
{
    int err = 0;
    int sock_index = 0;

    struct usrsock_reqstate_s state =
    {
    };
    struct usrsock_conn_s *conn;

    /* Allocate the usrsock socket connection structure and save in the new
     * socket instance.
     */

    conn = usrsock_alloc();
    if (!conn) {
        /* Failed to reserve a connection structure */

        return -ENOMEM;
    }

    net_lock();

    /* Set up event callback for usrsock. */

    err = usrsock_setup_request_callback(conn, &state, socket_event,
                                         USRSOCK_EVENT_ABORT |
                                         USRSOCK_EVENT_REQ_COMPLETE);
    if (err < 0) {
        goto errout_free_conn;
    }

    /* Request user-space daemon for new socket. */

    err = do_socket_request(conn, domain, type, protocol);
    if (err < 0) {
        goto errout_teardown_callback;
    }

    /* Wait for completion of request. */
    net_lockedwait(&state.recvsem);

    if (state.result < 0) {
        err = state.result;
        goto errout_teardown_callback;
    }

    sock_index = alloc_usrsockets(conn);
    if (sock_index == -1) {
        goto errout_teardown_callback;
    }

    conn->type    = type;
    conn->crefs   = 1;

    usrsock_teardown_request_callback(&state);

    net_unlock();

    return sock_index;

errout_teardown_callback:
    usrsock_teardown_request_callback(&state);
errout_free_conn:
    usrsock_free(conn);
    net_unlock();

    return err;
}

static struct usrsock_s *get_socket(int s)
{
    struct usrsock_s *sock;

    s -= USRSOCK_SOCKET_OFFSET;

    if ((s < 0) || (s >= NUM_SOCKETS)) {
        LOGE(USRSOCK_TAG, "get_socket(%d): invalid", s + USRSOCK_SOCKET_OFFSET);
        return NULL;
    }

    sock = &usrsockets[s];

    if (!sock->s_conn) {
        LOGE(USRSOCK_TAG, "get_socket(%d): not active", s + USRSOCK_SOCKET_OFFSET);
        return NULL;
    }

    return sock;
}

static uint16_t bind_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "bind socket aborted.\n");

        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "bind request completed.\n");

        pstate->result = conn->resp.result;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_bind_request
 ****************************************************************************/

static int do_bind_request(struct usrsock_conn_s *conn,
                           const struct sockaddr *addr,
                           socklen_t addrlen)
{
    struct usrsock_request_bind_s req =
    {
    };
    struct iovec bufs[2];

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_BIND;
    req.usockid = conn->usockid;
    req.addrlen = addrlen;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);
    bufs[1].iov_base = (void *)addr;
    bufs[1].iov_len = req.addrlen;

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_bind
 *
 * Description:
 *   usrsock_bind() gives the socket 'conn' the local address 'addr'. 'addr'
 *   is 'addrlen' bytes long. Traditionally, this is called "assigning a name
 *   to a socket." When a socket is created with socket, it exists in a name
 *   space (address family) but has no name assigned.
 *
 * Input Parameters:
 *   s        Socket index
 *   addr     Socket local address
 *   addrlen  Length of 'addr'
 *
 * Returned Value:
 *   0 on success; -1 on error with errno set appropriately
 *
 *   EACCES
 *     The address is protected, and the user is not the superuser.
 *   EADDRINUSE
 *     The given address is already in use.
 *   EINVAL
 *     The socket is already bound to an address.
 *   ENOTSOCK
 *     psock is a descriptor for a file, not a socket.
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_bind(int s, const struct sockaddr *addr, socklen_t addrlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    ssize_t ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_request_callback(conn, &state, bind_event,
            USRSOCK_EVENT_ABORT |
            USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout_unlock;
    }

    /* Request user-space daemon to close socket. */

    ret = do_bind_request(conn, addr, addrlen);
    if (ret >= 0) {
        /* Wait for completion of request. */

        net_lockedwait(&state.recvsem);
        ret = state.result;
    }

    usrsock_teardown_request_callback(&state);

errout_unlock:
    net_unlock();
    return ret;
}

static uint16_t connect_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "connect socket aborted.\n");

        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "connect request completed.\n");

        pstate->result = conn->resp.result;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_connect_request
 ****************************************************************************/

static int do_connect_request(struct usrsock_conn_s *conn,
                              const struct sockaddr *addr,
                              socklen_t addrlen)
{
    struct usrsock_request_connect_s req =
    {
    };
    usrsock_sockaddr_t usrsock_addr;
    struct iovec bufs[2];

    if (addrlen > 65535) {
        addrlen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_CONNECT;
    req.usockid = conn->usockid;
    req.addrlen = addrlen;

    if (addrlen) {
        usrsock_addr.sa_family = addr->sa_family;
        memcpy(usrsock_addr.sa_data, addr->sa_data, sizeof(addr->sa_data));
    }

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);
    bufs[1].iov_base = addrlen ? (void *)&usrsock_addr : NULL;
    bufs[1].iov_len = addrlen ? sizeof(usrsock_addr) : 0;

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_connect
 *
 * Description:
 *   Perform a usrsock connection
 *
 * Input Parameters:
 *   s       Socket index
 *   addr    The address of the remote server to connect to
 *   addrlen Length of address buffer
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_connect(int s, const struct sockaddr *addr, socklen_t addrlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNREFUSED;
        goto errout_unlock;
    }

    if (conn->connected &&
            (conn->type == SOCK_STREAM)) {
        /* Already connected. */

        ret = -EISCONN;
        goto errout_unlock;
    }

    if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
        /* Already connecting. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket already connecting.\n",
                conn->usockid);

        ret = -EALREADY;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_request_callback(conn, &state, connect_event,
            USRSOCK_EVENT_ABORT |
            USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);

        goto errout_unlock;
    }

    /* Mark conn as connecting one. */

    conn->state = USRSOCK_CONN_STATE_CONNECTING;

    /* Send request. */

    ret = do_connect_request(conn, addr, addrlen);
    if (ret < 0) {
        goto errout_teardown;
    }

    /* Do not block on waiting for request completion if nonblocking socket. */

    if (!conn->resp.inprogress) {
        /* Wait for completion of request (or signal). */

        net_lockedwait(&state.recvsem);
        ret = state.result;
    } else {
        /* Request not completed and socket is non-blocking. */

        ret = -EINPROGRESS;
    }

errout_teardown:
    usrsock_teardown_request_callback(&state);
errout_unlock:
    net_unlock();
    return ret;
}

static uint16_t close_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "close socket aborted.\n");

        conn->state = USRSOCK_CONN_STATE_ABORTED;
        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "close request completed.\n");

        pstate->result = conn->resp.result;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_close_request
 ****************************************************************************/

static int do_close_request(struct usrsock_conn_s *conn)
{
    struct usrsock_request_close_s req =
    {
    };
    struct iovec bufs[1];

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_CLOSE;
    req.usockid = conn->usockid;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_close
 *
 * Description:
 *
 ****************************************************************************/

int usrsocket_close(int s)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Already closed? */

        LOGE(USRSOCK_TAG, "usockid=%d; already closed.\n", conn->usockid);

        ret = OK;
        goto close_out;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_request_callback(conn, &state, close_event,
            USRSOCK_EVENT_ABORT |
            USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0)
    {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout;
    }

    /* Request user-space daemon to close socket. */

    ret = do_close_request(conn);
    if (ret < 0) {
        ret = OK; /* Error? return OK for close. */
    }
    else {
        /* Wait for completion of request. */

        net_lockedwait(&state.recvsem);
    }

    usrsock_teardown_request_callback(&state);

close_out:
    conn->state = USRSOCK_CONN_STATE_UNINITIALIZED;
    conn->usockid = -1;

errout:
    net_unlock();
    return ret;
}

static uint16_t listen_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "listen socket aborted.\n");

        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "listen request completed.\n");

        pstate->result = conn->resp.result;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_listen_request
 ****************************************************************************/

static int do_listen_request(struct usrsock_conn_s *conn, int backlog)
{
    struct usrsock_request_listen_s req =
    {
    };
    struct iovec bufs[1];

    if (backlog > 65535) {
        backlog = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_LISTEN;
    req.usockid = conn->usockid;
    req.backlog = backlog;

    bufs[0].iov_base = &req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_listen
 *
 * Description:
 *   To accept connections, a socket is first created with psock_socket(), a
 *   willingness to accept incoming connections and a queue limit for
 *   incoming connections are specified with psock_listen(), and then the
 *   connections are accepted with psock_accept().  For the case of AFINET
 *   and AFINET6 sockets, psock_listen() calls this function.  The
 *   psock_listen() call applies only to sockets of type SOCK_STREAM or
 *   SOCK_SEQPACKET.
 *
 * Parameters:
 *   s        Reference to an socket index.
 *   backlog  The maximum length the queue of pending connections may grow.
 *            If a connection request arrives with the queue full, the client
 *            may receive an error with an indication of ECONNREFUSED or,
 *            if the underlying protocol supports retransmission, the request
 *            may be ignored so that retries succeed.
 *
 * Returned Value:
 *   On success, zero is returned. On error, a negated errno value is
 *   returned.  See list() for the set of appropriate error values.
 *
 ****************************************************************************/

int usrsocket_listen(int s, int backlog)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; listen() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNREFUSED;
        goto errout_unlock;
    }

    if (conn->connected) {
        /* Already connected. */

        ret = -EISCONN;
        goto errout_unlock;
    }

    if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
        /* Already connecting. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket already connecting.\n",
                conn->usockid);

        ret = -EALREADY;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_request_callback(conn, &state, listen_event,
            USRSOCK_EVENT_ABORT |
            USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);

        goto errout_unlock;
    }

    /* Mark conn as listening(reuse connecting here) one. */

    conn->state = USRSOCK_CONN_STATE_CONNECTING;

    /* Send request. */

    ret = do_listen_request(conn, backlog);
    if (ret < 0)
    {
        goto errout_teardown;
    }

    /* Wait for completion of request (or signal). */

    net_lockedwait(&state.recvsem);

    ret = state.result;

errout_teardown:
    usrsock_teardown_request_callback(&state);
errout_unlock:
    net_unlock();
    return ret;
}

static uint16_t getpeername_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_data_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "getpeername socket aborted.\n");

        pstate->reqstate.result = -ECONNABORTED;
        pstate->valuelen = 0;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "getpeername request completed.\n");

        pstate->reqstate.result = conn->resp.result;
        if (pstate->reqstate.result < 0) {
            pstate->valuelen = 0;
            pstate->valuelen_nontrunc = 0;
        } else {
            pstate->valuelen = conn->resp.valuelen;
            pstate->valuelen_nontrunc = conn->resp.valuelen_nontrunc;
        }

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_getsockopt_request
 ****************************************************************************/

static int do_getpeername_request(struct usrsock_conn_s *conn,
                                  socklen_t addrlen)
{
    struct usrsock_request_getpeername_s req =
    {
    };
    struct iovec bufs[1];

    if (addrlen > 65535) {
        addrlen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_GETPEERNAME;
    req.usockid = conn->usockid;
    req.max_addrlen = addrlen;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_getpeername
 *
 * Description:
 *   The getpeername() function retrieves the remote-connected name of the
 *   specified socket, stores this address in the sockaddr structure pointed
 *   to by the 'addr' argument, and stores the length of this address in the
 *   object pointed to by the 'addrlen' argument.
 *
 *   If the actual length of the address is greater than the length of the
 *   supplied sockaddr structure, the stored address will be truncated.
 *
 *   If the socket has not been bound to a local name, the value stored in
 *   the object pointed to by address is unspecified.
 *
 * Parameters:
 *   s        usrsock sockets index
 *   addr     sockaddr structure to receive data [out]
 *   addrlen  Length of sockaddr structure [in/out]
 *
 ****************************************************************************/

int usrsocket_getpeername(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_data_reqstate_s state =
    {
    };
    struct iovec inbufs[1];
    ssize_t ret;
    socklen_t outaddrlen = 0;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; usrsock_getpeername() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_data_request_callback(
            conn, &state, getpeername_event,
            USRSOCK_EVENT_ABORT | USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout_unlock;
    }

    inbufs[0].iov_base = (void *)addr;
    inbufs[0].iov_len = *addrlen;

    usrsock_setup_datain(conn, inbufs, ARRAY_SIZE(inbufs));

    /* Request user-space daemon to close socket. */

    ret = do_getpeername_request(conn, *addrlen);
    if (ret >= 0) {
        /* Wait for completion of request. */

        net_lockedwait(&state.reqstate.recvsem);

        ret = state.reqstate.result;

        USRSOCK_ASSERT("state.valuelen <= *addrlen", state.valuelen <= *addrlen);
        USRSOCK_ASSERT("state.valuelen <= state.valuelen_nontrunc" , state.valuelen <= state.valuelen_nontrunc);

        if (ret >= 0) {
            /* Store length of data that was written to 'value' buffer. */

            outaddrlen = state.valuelen_nontrunc;
        }
    }

    usrsock_teardown_datain(conn);
    usrsock_teardown_data_request_callback(&state);

errout_unlock:
    net_unlock();

    *addrlen = outaddrlen;
    return ret;
}

static uint16_t getsockname_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_data_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "getsockname socket aborted.\n");

        pstate->reqstate.result = -ECONNABORTED;
        pstate->valuelen = 0;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "getsockname request completed.\n");

        pstate->reqstate.result = conn->resp.result;
        if (pstate->reqstate.result < 0) {
            pstate->valuelen = 0;
            pstate->valuelen_nontrunc = 0;
        } else {
            pstate->valuelen = conn->resp.valuelen;
            pstate->valuelen_nontrunc = conn->resp.valuelen_nontrunc;
        }

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_getsockopt_request
 ****************************************************************************/

static int do_getsockname_request(struct usrsock_conn_s *conn,
                                  socklen_t addrlen)
{
    struct usrsock_request_getsockname_s req =
    {
    };
    struct iovec bufs[1];

    if (addrlen > 65535) {
        addrlen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_GETSOCKNAME;
    req.usockid = conn->usockid;
    req.max_addrlen = addrlen;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_getsockname
 *
 * Description:
 *   The getsockname() function retrieves the locally-bound name of the
 *   specified socket, stores this address in the sockaddr structure pointed
 *   to by the 'addr' argument, and stores the length of this address in the
 *   object pointed to by the 'addrlen' argument.
 *
 *   If the actual length of the address is greater than the length of the
 *   supplied sockaddr structure, the stored address will be truncated.
 *
 *   If the socket has not been bound to a local name, the value stored in
 *   the object pointed to by address is unspecified.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   addr     sockaddr structure to receive data [out]
 *   addrlen  Length of sockaddr structure [in/out]
 *
 ****************************************************************************/

int usrsocket_getsockname(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_data_reqstate_s state =
    {
    };
    struct iovec inbufs[1];
    ssize_t ret;
    socklen_t outaddrlen = 0;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_data_request_callback(
            conn, &state, getsockname_event,
            USRSOCK_EVENT_ABORT | USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout_unlock;
    }

    inbufs[0].iov_base = (void *)addr;
    inbufs[0].iov_len = *addrlen;

    usrsock_setup_datain(conn, inbufs, ARRAY_SIZE(inbufs));

    /* Request user-space daemon to close socket. */

    ret = do_getsockname_request(conn, *addrlen);
    if (ret >= 0) {
        /* Wait for completion of request. */

        net_lockedwait(&state.reqstate.recvsem);

        ret = state.reqstate.result;

        USRSOCK_ASSERT("state.valuelen <= *addrlen", state.valuelen <= *addrlen);
        USRSOCK_ASSERT("state.valuelen <= state.valuelen_nontrunc", state.valuelen <= state.valuelen_nontrunc);

        if (ret >= 0) {
            /* Store length of data that was written to 'value' buffer. */

            outaddrlen = state.valuelen_nontrunc;
        }
    }

    usrsock_teardown_datain(conn);
    usrsock_teardown_data_request_callback(&state);

errout_unlock:
    net_unlock();

    *addrlen = outaddrlen;
    return ret;
}

static uint16_t setsockopt_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "setsockopt socket aborted.\n");

        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }
    else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "setsockopt request completed.\n");

        pstate->result = conn->resp.result;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_setsockopt_request
 ****************************************************************************/

static int do_setsockopt_request(struct usrsock_conn_s *conn, int level, int option,
                                 const void *value, socklen_t value_len)
{
    struct usrsock_request_setsockopt_s req =
    {
    };
    struct iovec bufs[2];

    if (level < -32768 || level > 32767) {
        return -EINVAL;
    }

    if (option < -32768 || option > 32767) {
        return -EINVAL;
    }

    if (value_len > 65535) {
        value_len = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_SETSOCKOPT;
    req.usockid = conn->usockid;
    req.level = level;
    req.option = option;
    req.valuelen = value_len;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);
    bufs[1].iov_base = (void *)value;
    bufs[1].iov_len = req.valuelen;

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_setsockopt
 *
 * Description:
 *   usrsock_setsockopt() sets the option specified by the 'option' argument,
 *   at the protocol level specified by the 'level' argument, to the value
 *   pointed to by the 'value' argument for the socket on the 'psock'
 *   argument.
 *
 *   The 'level' argument specifies the protocol level of the option. To set
 *   options at the socket level, specify the level argument as SOL_SOCKET.
 *
 *   See <sys/socket.h> a complete list of values for the 'option' argument.
 *
 * Input Parameters:
 *   s         usrsock sockets index
 *   level     Protocol level to set the option
 *   option    identifies the option to set
 *   value     Points to the argument value
 *   value_len The length of the argument value
 *
 ****************************************************************************/

int usrsocket_setsockopt(int s, int level, int option,
                       const void *value, socklen_t value_len)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    ssize_t ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_request_callback(conn, &state, setsockopt_event,
            USRSOCK_EVENT_ABORT |
            USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout_unlock;
    }

    /* Request user-space daemon to close socket. */

    if (level == SOL_SOCKET)
        level = 0;
    if (option == SO_RCVTIMEO || option == SO_SNDTIMEO) {
        const struct timeval *tv = (const struct timeval *)value;
        long timeo;

        timeo = net_timeval2msec(tv);
        if (option == SO_RCVTIMEO) {
            conn->s_rcvtimeo = timeo;
        }
        else {
            conn->s_sndtimeo = timeo;
        }
    }

    ret = do_setsockopt_request(conn, level, sock_opt_map(option), value, value_len);
    if (ret >= 0) {
        /* Wait for completion of request. */

        net_lockedwait(&state.recvsem);

        ret = state.result;
    }

    usrsock_teardown_request_callback(&state);

errout_unlock:
    net_unlock();
    return ret;
}

static uint16_t getsockopt_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_data_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "getsockopt socket aborted.\n");

        pstate->reqstate.result = -ECONNABORTED;
        pstate->valuelen = 0;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "getsockopt request completed.\n");

        pstate->reqstate.result = conn->resp.result;
        if (pstate->reqstate.result < 0) {
            pstate->valuelen = 0;
        } else {
            pstate->valuelen = conn->resp.valuelen;
        }

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_getsockopt_request
 ****************************************************************************/

static int do_getsockopt_request(struct usrsock_conn_s *conn, int level,
                                 int option, socklen_t value_len)
{
    struct usrsock_request_getsockopt_s req =
    {
    };
    struct iovec bufs[1];

    if (level < -32768 || level > 32767)
    {
        return -EINVAL;
    }

    if (option < -32768 || option > 32767)
    {
        return -EINVAL;
    }

    if (value_len > 65535)
    {
        value_len = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_GETSOCKOPT;
    req.usockid = conn->usockid;
    req.level = level;
    req.option = option;
    req.max_valuelen = value_len;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_getsockopt
 *
 * Description:
 *   getsockopt() retrieve thse value for the option specified by the
 *   'option' argument for the socket specified by the 'psock' argument. If
 *   the size of the option value is greater than 'value_len', the value
 *   stored in the object pointed to by the 'value' argument will be silently
 *   truncated. Otherwise, the length pointed to by the 'value_len' argument
 *   will be modified to indicate the actual length of the'value'.
 *
 *   The 'level' argument specifies the protocol level of the option. To
 *   retrieve options at the socket level, specify the level argument as
 *   SOL_SOCKET.
 *
 *   See <sys/socket.h> a complete list of values for the 'option' argument.
 *
 * Input Parameters:
 *   s         usrsock sockets index
 *   level     Protocol level to set the option
 *   option    identifies the option to get
 *   value     Points to the argument value
 *   value_len The length of the argument value
 *
 ****************************************************************************/

int usrsocket_getsockopt(int s, int level, int option,
                         void *value, socklen_t *value_len)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_data_reqstate_s state =
    {
    };
    struct iovec inbufs[1];
    ssize_t ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    /* Set up event callback for usrsock. */

    ret = usrsock_setup_data_request_callback(
            conn, &state, getsockopt_event,
            USRSOCK_EVENT_ABORT | USRSOCK_EVENT_REQ_COMPLETE);
    if (ret < 0) {
        LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
        goto errout_unlock;
    }

    inbufs[0].iov_base = (void *)value;
    inbufs[0].iov_len = *value_len;

    usrsock_setup_datain(conn, inbufs, ARRAY_SIZE(inbufs));

    /* Request user-space daemon to close socket. */

    if (level == SOL_SOCKET)
        level = 0;

    ret = do_getsockopt_request(conn, level, sock_opt_map(option), *value_len);
    if (ret >= 0) {
        /* Wait for completion of request. */

        net_lockedwait(&state.reqstate.recvsem);

        ret = state.reqstate.result;

        USRSOCK_ASSERT("state.valuelen <= *value_len", state.valuelen <= *value_len);

        if (ret >= 0) {
            /* Store length of data that was written to 'value' buffer. */

            *value_len = state.valuelen;
        }
    }

    usrsock_teardown_datain(conn);
    usrsock_teardown_data_request_callback(&state);

errout_unlock:
    net_unlock();

    return ret;
}

static uint16_t recvfrom_event(void *pvconn, void *pvpriv, uint16_t flags)
{
  struct usrsock_data_reqstate_s *pstate = pvpriv;
  struct usrsock_conn_s *conn = pvconn;

  if (flags & USRSOCK_EVENT_ABORT) {
      LOGI(USRSOCK_TAG, "recvfrom socket aborted.\n");

      pstate->reqstate.result = -ECONNABORTED;
      pstate->valuelen = 0;
      pstate->valuelen_nontrunc = 0;

      /* Stop further callbacks */

      pstate->reqstate.cb->flags   = 0;
      pstate->reqstate.cb->priv    = NULL;
      pstate->reqstate.cb->event   = NULL;

      /* Wake up the waiting thread */

      aos_sem_signal(&pstate->reqstate.recvsem);
  } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
      LOGI(USRSOCK_TAG, "recvfrom request completed. status = %d\n", conn->resp.result);

      pstate->reqstate.result = conn->resp.result;
      if (pstate->reqstate.result < 0) {
          pstate->valuelen = 0;
          pstate->valuelen_nontrunc = 0;
      } else {
          pstate->valuelen = conn->resp.valuelen;
          pstate->valuelen_nontrunc = conn->resp.valuelen_nontrunc;
      }

      if (pstate->reqstate.result >= 0 ||
              pstate->reqstate.result == -EAGAIN) {
          /* After reception of data, mark input not ready. Daemon will
           * send event to restore this flag.
           */

          conn->flags &= ~USRSOCK_EVENT_RECVFROM_AVAIL;
      }

      /* Stop further callbacks */

      pstate->reqstate.cb->flags   = 0;
      pstate->reqstate.cb->priv    = NULL;
      pstate->reqstate.cb->event   = NULL;

      /* Wake up the waiting thread */

      aos_sem_signal(&pstate->reqstate.recvsem);
  } else if (flags & USRSOCK_EVENT_REMOTE_CLOSED) {
      LOGI(USRSOCK_TAG, "remote closed.\n");

      pstate->reqstate.result = -EPIPE;

      /* Stop further callbacks */

      pstate->reqstate.cb->flags   = 0;
      pstate->reqstate.cb->priv    = NULL;
      pstate->reqstate.cb->event   = NULL;

      /* Wake up the waiting thread */

      aos_sem_signal(&pstate->reqstate.recvsem);
  } else if (flags & USRSOCK_EVENT_RECVFROM_AVAIL) {
      LOGI(USRSOCK_TAG, "recvfrom avail.\n");

      flags &= ~USRSOCK_EVENT_RECVFROM_AVAIL;

      /* Stop further callbacks */

      pstate->reqstate.cb->flags   = 0;
      pstate->reqstate.cb->priv    = NULL;
      pstate->reqstate.cb->event   = NULL;

      /* Wake up the waiting thread */

      aos_sem_signal(&pstate->reqstate.recvsem);
  }

  return flags;
}

/****************************************************************************
 * Name: do_recvfrom_request
 ****************************************************************************/

static int do_recvfrom_request(struct usrsock_conn_s *conn, size_t buflen,
                               socklen_t addrlen)
{
    struct usrsock_request_recvfrom_s req =
    {
    };
    struct iovec bufs[1];

    if (addrlen > 65535) {
        addrlen = 65535;
    }

    if (buflen > 65535) {
        buflen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_RECVFROM;
    req.usockid = conn->usockid;
    req.max_addrlen = addrlen;
    req.max_buflen = buflen;

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Name: usrsocket_recvfrom
 *
 * Description:
 *   recvfrom() receives messages from a socket, and may be used to receive
 *   data on a socket whether or not it is connection-oriented.
 *
 *   If from is not NULL, and the underlying protocol provides the source
 *   address, this source address is filled in. The argument fromlen
 *   initialized to the size of the buffer associated with from, and modified
 *   on return to indicate the actual size of the address stored there.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   buf      Buffer to receive data
 *   len      Length of buffer
 *   flags    Receive flags (ignored)
 *   from     Address of source (may be NULL)
 *   fromlen  The length of the address structure
 *
 ****************************************************************************/

int usrsocket_recvfrom(int s, void *buf, size_t len,
                       int flags, struct sockaddr *from,
                       socklen_t *fromlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_data_reqstate_s state =
    {
    };
    usrsock_sockaddr_t usrsock_addr;
    struct iovec inbufs[2];
    socklen_t addrlen = 0;
    socklen_t outaddrlen = 0;
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    if (fromlen) {
        if (*fromlen > 0 && from == NULL) {
            return -EINVAL;
        }

        addrlen = sizeof(usrsock_addr);
    }

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    if (conn->type == SOCK_STREAM) {
        if (!conn->connected) {
            if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
                /* Connecting. */

                LOGE(USRSOCK_TAG, "usockid=%d; socket still connecting.\n",
                        conn->usockid);

                ret = -EAGAIN;
                goto errout_unlock;
            } else {
                /* Not connected. */

                LOGE(USRSOCK_TAG, "usockid=%d; socket not connected.\n",
                        conn->usockid);

                ret = -ENOTCONN;
                goto errout_unlock;
            }
        }
    }

    if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
        /* Non-blocking connecting. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket still connecting.\n",
                conn->usockid);

        ret = -EAGAIN;
        goto errout_unlock;
    }

    do
    {
        /* Check if remote end has closed connection. */

        if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED) {
            LOGE(USRSOCK_TAG, "usockid=%d; remote closed (EOF).\n", conn->usockid);

            ret = 0;
            goto errout_unlock;
        }

        /* Check if need to wait for receive data to become available. */

        if (!(conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL))
        {
            /* Wait recv to become avail. */

            ret = usrsock_setup_data_request_callback(
                    conn, &state, recvfrom_event,
                    USRSOCK_EVENT_ABORT | USRSOCK_EVENT_RECVFROM_AVAIL |
                    USRSOCK_EVENT_REMOTE_CLOSED);
            if (ret < 0) {
                LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
                goto errout_unlock;
            }

            /* Wait for receive-avail (or abort, or timeout, or signal). */

            ret = net_timedwait(&state.reqstate.recvsem, conn->s_rcvtimeo ? (int)conn->s_rcvtimeo : 0);
            if (ret < 0)
            {
                LOGE(USRSOCK_TAG, "recvfrom net_timedwait errno: %d\n", ret);
                goto errout_unlock;
            }

            usrsock_teardown_data_request_callback(&state);

            /* Did wait timeout or got signal? */

            if (ret != 0) {
                goto errout_unlock;
            }

            /* Was socket aborted? */

            if (conn->state == USRSOCK_CONN_STATE_ABORTED) {
                ret = -EPIPE;
                goto errout_unlock;
            }

            /* Did remote disconnect? */

            if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED) {
                ret = 0;
                goto errout_unlock;
            }

            USRSOCK_ASSERT("conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL",
                            conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL);
        }

        /* Set up event callback for usrsock. */

        ret = usrsock_setup_data_request_callback(
                conn, &state, recvfrom_event,
                USRSOCK_EVENT_ABORT | USRSOCK_EVENT_REQ_COMPLETE);
        if (ret < 0) {
            LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
            goto errout_unlock;
        }

        inbufs[0].iov_base = addrlen ? (void *)&usrsock_addr : NULL;
        inbufs[0].iov_len = addrlen ? sizeof(usrsock_addr) : 0;
        inbufs[1].iov_base = (void *)buf;
        inbufs[1].iov_len = len;

        usrsock_setup_datain(conn, inbufs, ARRAY_SIZE(inbufs));

        /* Request user-space daemon to close socket. */

        ret = do_recvfrom_request(conn, len, addrlen);
        if (ret >= 0) {
            /* Wait for completion of request. */

            net_lockedwait(&state.reqstate.recvsem);

            ret = state.reqstate.result;

            USRSOCK_ASSERT("ret <= (int)len", ret <= (int)len);
            USRSOCK_ASSERT("state.valuelen <= addrlen", state.valuelen <= addrlen);
            USRSOCK_ASSERT("state.valuelen <= state.valuelen_nontrunc",
                            state.valuelen <= state.valuelen_nontrunc);

            if (ret >= 0) {
                /* Store length of 'from' address that was available at
                 * daemon-side.
                 */

                outaddrlen = sizeof(struct sockaddr);
                if (addrlen && from) {
                    from->sa_family = usrsock_addr.sa_family;
                    memcpy(from->sa_data, usrsock_addr.sa_data, sizeof(from->sa_data));
                }
            }
        }

        usrsock_teardown_datain(conn);
        usrsock_teardown_data_request_callback(&state);
    }
    while (ret == -EAGAIN);

errout_unlock:
    net_unlock();

    if (fromlen)
    {
        *fromlen = outaddrlen;
    }

    return ret;
}

int usrsocket_recv(int s, void *mem, size_t len, int flags)
{
    return usrsocket_recvfrom(s, mem, len, flags, NULL, NULL);
}

static uint16_t sendto_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "sendto socket aborted.\n");

        pstate->result = -ECONNABORTED;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "sendto request completed. status = %d\n", conn->resp.result);

        pstate->result = conn->resp.result;

        if (pstate->result >= 0 || pstate->result == -EAGAIN)
        {
            /* After reception of data, mark input not ready. Daemon will
             * send event to restore this flag.
             */

            conn->flags &= ~USRSOCK_EVENT_SENDTO_READY;
        }

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_REMOTE_CLOSED) {
        LOGI(USRSOCK_TAG, "remote closed.\n");

        pstate->result = -EPIPE;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    } else if (flags & USRSOCK_EVENT_SENDTO_READY) {
        LOGI(USRSOCK_TAG, "sendto ready.\n");

        /* Do not let other waiters to claim new data. */

        flags &= ~USRSOCK_EVENT_SENDTO_READY;

        /* Stop further callbacks */

        pstate->cb->flags   = 0;
        pstate->cb->priv    = NULL;
        pstate->cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_sendto_request
 ****************************************************************************/

static int do_sendto_request(struct usrsock_conn_s *conn,
                             const void *buf, size_t buflen,
                             const struct sockaddr *addr,
                             socklen_t addrlen)
{
    struct usrsock_request_sendto_s req =
    {
    };
    usrsock_sockaddr_t usrsock_addr;
    struct iovec bufs[3];

    if (addrlen > 65535) {
        addrlen = 65535;
    }

    if (buflen > 65535) {
        buflen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_SENDTO;
    req.usockid = conn->usockid;
    req.addrlen = addrlen;
    req.buflen = buflen;

    if (addrlen && addr) {
        usrsock_addr.sa_family = addr->sa_family;
        memcpy(usrsock_addr.sa_data, addr->sa_data, sizeof(addr->sa_data));
    }

    bufs[0].iov_base = (void *)&req;
    bufs[0].iov_len = sizeof(req);
    bufs[1].iov_base = addrlen ? (void *)&usrsock_addr : NULL;
    bufs[1].iov_len = addrlen ? sizeof(usrsock_addr) : 0;
    bufs[2].iov_base = (void *)buf;
    bufs[2].iov_len = buflen;

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: usrsocket_sendto
 *
 * Description:
 *   If sendto() is used on a connection-mode (SOCK_STREAM, SOCK_SEQPACKET)
 *   socket, the parameters to and 'tolen' are ignored (and the error EISCONN
 *   may be returned when they are not NULL and 0), and the error ENOTCONN is
 *   returned when the socket was not actually connected.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   buf      Data to send
 *   len      Length of data to send
 *   flags    Send flags (ignored)
 *   to       Address of recipient
 *   tolen    The length of the address structure
 *
 ****************************************************************************/

int usrsocket_sendto(int s, const void *buf, size_t len,
                   int flags, const struct sockaddr *to,
                   socklen_t tolen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_reqstate_s state =
    {
    };
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; connect() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    if (conn->type == SOCK_STREAM) {
        if (!conn->connected) {
            if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
                /* Connecting. */

                LOGE(USRSOCK_TAG, "usockid=%d; socket still connecting.\n",
                        conn->usockid);

                ret = -EAGAIN;
                goto errout_unlock;
            } else {
                /* Not connected. */

                ret = -ENOTCONN;
                goto errout_unlock;
            }
        }

        if (to || tolen) {
            /* Address provided for connection-mode socket */

            ret = -EISCONN;
            goto errout_unlock;
        }
    }

    if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
        /* Non-blocking connecting. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket still connecting.\n", conn->usockid);

        ret = -EAGAIN;
        goto errout_unlock;
    }

    do
    {
        /* Check if remote end has closed connection. */

        if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED)
        {
            LOGE(USRSOCK_TAG, "usockid=%d; remote closed.\n", conn->usockid);

            ret = -EPIPE;
            goto errout_unlock;
        }

        /* Check if need to wait for send to become ready. */

        if (!(conn->flags & USRSOCK_EVENT_SENDTO_READY))
        {
            /* Wait send to become ready. */

            ret = usrsock_setup_request_callback(conn, &state, sendto_event,
                    USRSOCK_EVENT_ABORT |
                    USRSOCK_EVENT_SENDTO_READY |
                    USRSOCK_EVENT_REMOTE_CLOSED);
            if (ret < 0) {
                LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
                goto errout_unlock;
            }

            /* Wait for send-ready (or abort, or timeout, or signal). */
            ret = net_timedwait(&state.recvsem, conn->s_sndtimeo ? (int)conn->s_sndtimeo : 0);
            if (ret < 0) {
                LOGE(USRSOCK_TAG, "sendto net_timedwait errno: %d\n", ret);
                goto errout_unlock;
            }

            usrsock_teardown_request_callback(&state);

            /* Did wait timeout or got signal? */

            if (ret != 0) {
                goto errout_unlock;
            }

            /* Was socket aborted? */

            if (conn->state == USRSOCK_CONN_STATE_ABORTED) {
                ret = -EPIPE;
                goto errout_unlock;
            }

            /* Did remote disconnect? */

            if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED) {
                ret = -EPIPE;
                goto errout_unlock;
            }

            USRSOCK_ASSERT("conn->flags & USRSOCK_EVENT_SENDTO_READY",
                            conn->flags & USRSOCK_EVENT_SENDTO_READY);
        }

        /* Set up event callback for usrsock. */

        ret = usrsock_setup_request_callback(conn, &state, sendto_event,
                USRSOCK_EVENT_ABORT |
                USRSOCK_EVENT_REQ_COMPLETE);
        if (ret < 0) {
            LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
            goto errout_unlock;
        }

        /* Request user-space daemon to close socket. */

        ret = do_sendto_request(conn, buf, len, to, tolen);
        if (ret >= 0) {
            /* Wait for completion of request. */

            net_lockedwait(&state.recvsem);

            ret = state.result;

            USRSOCK_ASSERT("ret <= (int)len", ret <= (int)len);
        }

        usrsock_teardown_request_callback(&state);
    }
    while (ret == -EAGAIN);

errout_unlock:
    net_unlock();
    return ret;
}

int usrsocket_send(int s, const void *data, size_t size, int flags)
{
    return usrsocket_sendto(s, data, size, flags, NULL, 0);
}

int usrsocket_shutdown(int s, int how)
{
    LOGI(USRSOCK_TAG, "%s call stub for now\n", __func__);
    return 0;
}

static uint16_t accept_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_data_reqstate_s *pstate = pvpriv;
    struct usrsock_conn_s *conn = pvconn;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGI(USRSOCK_TAG, "accept socket aborted.\n");

        pstate->reqstate.result = -ECONNABORTED;
        pstate->valuelen = 0;
        pstate->valuelen_nontrunc = 0;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_REQ_COMPLETE) {
        LOGI(USRSOCK_TAG, "accept request completed.\n");

        pstate->reqstate.result = conn->resp.result;
        if (pstate->reqstate.result < 0) {
            pstate->valuelen = 0;
            pstate->valuelen_nontrunc = 0;
        } else {
            pstate->valuelen = conn->resp.valuelen;
            pstate->valuelen_nontrunc = conn->resp.valuelen_nontrunc;
        }

        if (pstate->reqstate.result >= 0 ||
                pstate->reqstate.result == -EAGAIN) {
            /* After reception of connection, mark input not ready. Daemon will
             * send event to restore this flag.
             */

            conn->flags &= ~USRSOCK_EVENT_RECVFROM_AVAIL;
        }

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_REMOTE_CLOSED) {
        LOGI(USRSOCK_TAG, "remote closed.\n");

        pstate->reqstate.result = -EPIPE;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    } else if (flags & USRSOCK_EVENT_RECVFROM_AVAIL) {
        LOGI(USRSOCK_TAG, "accept avail.\n");

        flags &= ~USRSOCK_EVENT_RECVFROM_AVAIL;

        /* Stop further callbacks */

        pstate->reqstate.cb->flags   = 0;
        pstate->reqstate.cb->priv    = NULL;
        pstate->reqstate.cb->event   = NULL;

        /* Wake up the waiting thread */

        aos_sem_signal(&pstate->reqstate.recvsem);
    }

    return flags;
}

/****************************************************************************
 * Name: do_accept_request
 ****************************************************************************/

static int do_accept_request(struct usrsock_conn_s *conn,
                             socklen_t addrlen)
{
    struct usrsock_request_accept_s req =
    {
    };
    struct iovec bufs[1];

    if (addrlen > 65535)
    {
        addrlen = 65535;
    }

    /* Prepare request for daemon to read. */

    req.head.reqid = USRSOCK_REQUEST_ACCEPT;
    req.usockid = conn->usockid;
    req.max_addrlen = addrlen;

    bufs[0].iov_base = &req;
    bufs[0].iov_len = sizeof(req);

    return usrsockdev_do_request(conn, bufs, ARRAY_SIZE(bufs));
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: usrsocket_accept
 *
 * Description:
 *   The usrsock_sockif_accept function is used with connection-based socket
 *   types (SOCK_STREAM, SOCK_SEQPACKET and SOCK_RDM). It extracts the first
 *   connection request on the queue of pending connections, creates a new
 *   connected socket with mostly the same properties as 'sockfd', and
 *   allocates a new socket descriptor for the socket, which is returned. The
 *   newly created socket is no longer in the listening state. The original
 *   socket 'sockfd' is unaffected by this call.  Per file descriptor flags
 *   are not inherited across an inet_accept.
 *
 *   The 'sockfd' argument is a socket descriptor that has been created with
 *   socket(), bound to a local address with bind(), and is listening for
 *   connections after a call to listen().
 *
 *   On return, the 'addr' structure is filled in with the address of the
 *   connecting entity. The 'addrlen' argument initially contains the size
 *   of the structure pointed to by 'addr'; on return it will contain the
 *   actual length of the address returned.
 *
 *   If no pending connections are present on the queue, and the socket is
 *   not marked as non-blocking, inet_accept blocks the caller until a
 *   connection is present. If the socket is marked non-blocking and no
 *   pending connections are present on the queue, inet_accept returns
 *   EAGAIN.
 *
 * Parameters:
 *   s        usrsock sockets index
 *   addr     Receives the address of the connecting client
 *   addrlen  Input: allocated size of 'addr', Return: returned size of 'addr'
 *   newsock  Location to return the accepted socket information.
 *
 * Returned Value:
 *   Returns 0 (OK) on success.  On failure, it returns a negated errno
 *   value.  See accept() for a description of the appropriate error value.
 *
 * Assumptions:
 *   The network is locked.
 *
 ****************************************************************************/

int usrsocket_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;
    struct usrsock_data_reqstate_s state =
    {
    };
    struct usrsock_conn_s *newconn;
    struct iovec inbufs[2];
    socklen_t inaddrlen = 0;
    socklen_t outaddrlen = 0;
    int ret;

    sock = get_socket(s);
    if (sock == NULL) {
        return ERR_ARG;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);

    if (addrlen) {
        if (*addrlen > 0 && addr == NULL) {
            return -EINVAL;
        }

        inaddrlen = *addrlen;
    }

    net_lock();

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
            conn->state == USRSOCK_CONN_STATE_ABORTED) {
        /* Invalid state or closed by daemon. */

        LOGE(USRSOCK_TAG, "usockid=%d; accept() with uninitialized usrsock.\n",
                conn->usockid);

        ret = (conn->state == USRSOCK_CONN_STATE_ABORTED) ? -EPIPE :
            -ECONNRESET;
        goto errout_unlock;
    }

    if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
        /* Connecting. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket still connecting.\n",
                conn->usockid);

        ret = -EAGAIN;
        goto errout_unlock;
    }

    if (!conn->connected) {
        /* Not connected. */

        LOGE(USRSOCK_TAG, "usockid=%d; socket not connected.\n",
                conn->usockid);

        ret = -ENOTCONN;
        goto errout_unlock;
    }

    /* Allocate the usrsock socket connection structure and save in the new
     * socket instance.
     */

    newconn = usrsock_alloc();
    if (!newconn)
    {
        /* Failed to reserve a connection structure */

        ret = -ENOMEM;
        goto errout_unlock;
    }

    do
    {
        /* Check if remote end has closed connection. */

        if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED)
        {
            LOGE(USRSOCK_TAG, "usockid=%d; remote closed (EOF).\n", conn->usockid);

            ret = -EPIPE;
            goto errout_free_conn;
        }

        /* Check if need to wait for connection to become available. */

        if (!(conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL)) {
            /* Wait accept to become avail. */

            ret = usrsock_setup_data_request_callback(
                    conn, &state, accept_event,
                    USRSOCK_EVENT_ABORT | USRSOCK_EVENT_RECVFROM_AVAIL |
                    USRSOCK_EVENT_REMOTE_CLOSED);
            if (ret < 0) {
                LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
                goto errout_free_conn;
            }

            /* Wait for receive-avail (or abort, or timeout, or signal). */
            net_lockedwait(&state.reqstate.recvsem);

            usrsock_teardown_data_request_callback(&state);

            /* Did wait timeout or got signal? */

            if (ret != 0) {
                goto errout_free_conn;
            }

            /* Was socket aborted? */

            if (conn->state == USRSOCK_CONN_STATE_ABORTED) {
                ret = -EPIPE;
                goto errout_free_conn;
            }

            /* Did remote disconnect? */

            if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED) {
                ret = -EPIPE;
                goto errout_free_conn;
            }

            USRSOCK_ASSERT("conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL",
                            conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL);
        }

        /* Set up event callback for usrsock. */

        ret = usrsock_setup_data_request_callback(
                conn, &state, accept_event,
                USRSOCK_EVENT_ABORT | USRSOCK_EVENT_REQ_COMPLETE);
        if (ret < 0) {
            LOGE(USRSOCK_TAG, "usrsock_setup_request_callback failed: %d\n", ret);
            goto errout_free_conn;
        }

        inbufs[0].iov_base = addr;
        inbufs[0].iov_len = inaddrlen;
        inbufs[1].iov_base = &newconn->usockid;
        inbufs[1].iov_len = sizeof(newconn->usockid);

        usrsock_setup_datain(conn, inbufs, ARRAY_SIZE(inbufs));

        /* We might start getting events for this socket right after
         * returning to daemon, so setup 'newconn' already here.
         */

        newconn->state = USRSOCK_CONN_STATE_READY;

        /* Request user-space daemon to accept socket. */

        ret = do_accept_request(conn, inaddrlen);
        if (ret >= 0) {
            /* Wait for completion of request. */

            net_lockedwait(&state.reqstate.recvsem);

            ret = state.reqstate.result;

            USRSOCK_ASSERT("state.valuelen <= inaddrlen", state.valuelen <= inaddrlen);
            USRSOCK_ASSERT("state.valuelen <= state.valuelen_nontrunc",
                            state.valuelen <= state.valuelen_nontrunc);

            if (ret >= 0) {
                newconn->connected = true;
                newconn->type      = conn->type;
                newconn->crefs     = 1;
                ret = alloc_usrsockets(newconn);
                if (ret == -1) {
                    usrsock_teardown_datain(conn);
                    usrsock_teardown_data_request_callback(&state);
                }

                usrsockets[ret].s_type    = sock->s_type;
                usrsockets[ret].s_domain  = sock->s_domain;
                usrsockets[ret].s_conn    = newconn;

                /* Store length of 'from' address that was available at
                 * daemon-side.
                 */

                outaddrlen = state.valuelen_nontrunc;

                ret = OK;
            }
        }

        usrsock_teardown_datain(conn);
        usrsock_teardown_data_request_callback(&state);
    }
    while (ret == -EAGAIN);

    if (ret >= 0)
    {
        goto errout_unlock;
    }

errout_free_conn:
    usrsock_free(newconn);

errout_unlock:
    net_unlock();

    if (addrlen)
    {
        *addrlen = outaddrlen;
    }

    return ret;
}

struct hostent *usrsocket_gethostbyname(const char *name)
{
    return gethostbyname_impl(name);
}

void usrsocket_freeaddrinfo(struct addrinfo *ai)
{
    return freeaddrinfo_impl(ai);
}

int usrsocket_getaddrinfo(const char *hostname, const char *servname,
                          const struct addrinfo *hint, struct addrinfo **res)
{
    return getaddrinfo_impl(hostname, servname, hint, res);
}


static uint16_t poll_event(void *pvconn, void *pvpriv, uint16_t flags)
{
    struct usrsock_poll_s *info = (struct usrsock_poll_s *)pvpriv;
    struct usrsock_conn_s *conn = pvconn;
    uint8_t eventset = 0;

    USRSOCK_ASSERT("!info || (info->conn && info->fds)",
                    !info || (info->conn && info->fds));

    if (!info)
        return flags;

    if (flags & USRSOCK_EVENT_ABORT) {
        LOGE(USRSOCK_TAG, "socket aborted.\n");

        /* Socket forcefully terminated. */

        eventset |= (USRSOCK_POLLERR | USRSOCK_POLLHUP);
    } else if ((flags & USRSOCK_EVENT_CONNECT_READY) && !conn->connected) {
        LOGE(USRSOCK_TAG, "socket connect failed.\n");

        /* Non-blocking connect failed. */

        eventset |= (USRSOCK_POLLERR | USRSOCK_POLLHUP);
    } else if (flags & USRSOCK_EVENT_REMOTE_CLOSED) {
        LOGE(USRSOCK_TAG, "remote closed.\n");

        /* Remote closed. */

        eventset |= (USRSOCK_POLLHUP | USRSOCK_POLLIN);
    }
    else {
        /* Check data events. */

        if (flags & USRSOCK_EVENT_RECVFROM_AVAIL) {
            LOGD(USRSOCK_TAG, "socket recv avail.\n");

            eventset |= USRSOCK_POLLIN;
        }

        if (flags & USRSOCK_EVENT_SENDTO_READY) {
            // LOGD(USRSOCK_TAG, "socket send ready.\n");

            eventset |= USRSOCK_POLLOUT;
        }
    }

    /* Filter I/O events depending on requested events. */

    eventset &= (~(USRSOCK_POLLOUT | USRSOCK_POLLIN) | info->fds->events);

    /* POLLOUT and PULLHUP are mutually exclusive. */

    if ((eventset & USRSOCK_POLLOUT) && (eventset & USRSOCK_POLLHUP)) {
        eventset &= ~USRSOCK_POLLOUT;
    }

    /* Awaken the caller of poll() is requested event occurred. */

    if (eventset) {
        info->fds->revents |= eventset;
        aos_sem_signal(info->fds->sem);
    }

    return flags;
}

static int usrsock_pollsetup(struct usrsock_conn_s *conn, struct usrsock_pollfd *fds)
{
    struct usrsock_poll_s *info;
    struct devif_callback_s *cb;
    int ret = OK;

    /* Allocate a container to hold the poll information */

    info = (struct usrsock_poll_s *)aos_malloc(sizeof(struct usrsock_poll_s));
    if (!info) {
        return -ENOMEM;
    }

    net_lock();

    /* Allocate a usrsock callback structure */

    cb = devif_callback_alloc(&conn->list);
    if (cb == NULL) {
        ret = -EBUSY;
        aos_free(info); /* fds->priv not set, so we need to free info here. */
        goto errout_unlock;
    }

    /* Initialize the poll info container */

    info->conn   = conn;
    info->fds    = fds;
    info->cb     = cb;

    /* Initialize the callback structure.  Save the reference to the info
     * structure as callback private data so that it will be available during
     * callback processing.
     */

    cb->flags    = USRSOCK_EVENT_ABORT | USRSOCK_EVENT_CONNECT_READY |
                   USRSOCK_EVENT_SENDTO_READY | USRSOCK_EVENT_RECVFROM_AVAIL |
                   USRSOCK_EVENT_REMOTE_CLOSED;
    cb->priv     = (void *)info;
    cb->event    = poll_event;

    /* Save the reference in the poll info structure as fds private as well
     * for use during poll teardown as well.
     */

    fds->priv    = (void *)info;

    /* Check if socket is in error state */

    if (conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ||
        conn->state == USRSOCK_CONN_STATE_ABORTED) {
        LOGE(USRSOCK_TAG, "socket %s.\n",
                conn->state == USRSOCK_CONN_STATE_UNINITIALIZED ?
                "uninitialized" : "aborted");

        fds->revents |= (USRSOCK_POLLERR | USRSOCK_POLLHUP);
    }

    /* Stream sockets need to be connected or connecting (or listening). */

    else if ((conn->type == SOCK_STREAM) &&
            !(conn->connected || conn->state == USRSOCK_CONN_STATE_CONNECTING)) {
        LOGE(USRSOCK_TAG, "stream socket not connected and not connecting.\n");

        fds->revents |= (USRSOCK_POLLOUT | USRSOCK_POLLIN | USRSOCK_POLLHUP);
    } else if (conn->flags & USRSOCK_EVENT_REMOTE_CLOSED) {
        LOGE(USRSOCK_TAG, "socket remote closed.\n");

        /* Remote closed. */

        fds->revents |= (USRSOCK_POLLHUP | USRSOCK_POLLIN);
    } else {
        /* Check if daemon has room for send data or has data to receive. */

        if (conn->flags & USRSOCK_EVENT_SENDTO_READY) {
            // LOGD(USRSOCK_TAG, "socket send ready.\n");

            fds->revents |= (USRSOCK_POLLOUT & fds->events);
        }

        if (conn->flags & USRSOCK_EVENT_RECVFROM_AVAIL) {
            LOGD(USRSOCK_TAG,"socket recv avail.\n");

            fds->revents |= (USRSOCK_POLLIN & fds->events);
        }
    }

    /* Filter I/O events depending on requested events. */

    fds->revents &= (~(USRSOCK_POLLOUT | USRSOCK_POLLIN) | info->fds->events);

    /* POLLOUT and PULLHUP are mutually exclusive. */

    if ((fds->revents & USRSOCK_POLLOUT) && (fds->revents & USRSOCK_POLLHUP)) {
        fds->revents &= ~USRSOCK_POLLOUT;
    }

    /* Check if any requested events are already in effect */

    if (fds->revents != 0) {
        /* Yes.. then signal the poll logic */
        aos_sem_signal(fds->sem);
    }

errout_unlock:
    net_unlock();
    return ret;
}

static int usrsock_pollteardown(struct usrsock_conn_s *conn, struct usrsock_pollfd *fds)
{
    struct usrsock_poll_s *info;

    /* Recover the socket descriptor poll state info from the poll structure */

    info = (struct usrsock_poll_s *)fds->priv;
    USRSOCK_ASSERT("info && info->fds && info->cb", info && info->fds && info->cb);
    if (info) {
        /* Release the callback */

        net_lock();
        devif_conn_callback_free(info->cb, &conn->list);
        net_unlock();

        /* Release the poll/select data slot */

        info->fds->priv = NULL;

        /* Then free the poll info container */

        aos_free(info);
    }

    return OK;
}

static int usrsock_poll(struct usrsock_conn_s *conn, struct usrsock_pollfd *fds, bool setup)
{
    if (setup) {
        return usrsock_pollsetup(conn, fds);
    } else {
        return usrsock_pollteardown(conn, fds);
    }
}

static int net_poll(int sockfd, struct usrsock_pollfd *fds, bool setup)
{
    struct usrsock_s *sock;
    struct usrsock_conn_s *conn;

    sock = get_socket(sockfd);
    if (sock == NULL) {
        return -EINVAL;
    }

    conn = sock->s_conn;

    USRSOCK_ASSERT("conn", conn);
    USRSOCK_ASSERT("fds != NULL", fds != NULL);

    /* Then let psock_poll() do the heavy lifting */

    return usrsock_poll(conn, fds, setup);
}

static int poll_fdsetup(int fd, struct usrsock_pollfd *fds, bool setup)
{
    /* Check for a valid file descriptor */

    if ((unsigned int)fd >= LWIP_SOCKET_OFFSET) {
        /* Perform the socket ioctl */

        if ((unsigned int)fd < (LWIP_SOCKET_OFFSET + NUM_SOCKETS)) {
            return net_poll(fd, fds, setup);
        } else {
            return -EBADF;
        }
    }

    return -EINVAL;
}

static inline int poll_setup(struct usrsock_pollfd *fds, unsigned int nfds, aos_sem_t *sem)
{
    unsigned int i;
    unsigned int j;
    int ret = OK;

    /* Process each descriptor in the list */

    for (i = 0; i < nfds; i++) {
        /* Setup the poll descriptor */

        fds[i].sem     = sem;
        fds[i].revents = 0;
        fds[i].priv    = NULL;

        /* Check for invalid descriptors. "If the value of fd is less than 0,
         * events shall be ignored, and revents shall be set to 0 in that entry
         * on return from poll()."
         *
         * NOTE:  There is a potential problem here.  If there is only one fd
         * and if it is negative, then poll will hang.  From my reading of the
         * spec, that appears to be the correct behavior.
         */

        switch (fds[i].events & USRSOCK_POLLMASK) {
            case USRSOCK_POLLFD:
                if (fds[i].fd >= 0) {
                    ret = poll_fdsetup(fds[i].fd, &fds[i], true);
                }
                break;

            default:
                ret = -EINVAL;
                break;
        }

        if (ret < 0) {
            /* Setup failed for fds[i]. We now need to teardown previously
             * setup fds[0 .. (i - 1)] to release allocated resources and
             * to prevent memory corruption by access to freed/released 'fds'
             * and 'sem'.
             */

            for (j = 0; j < i; j++) {
                switch (fds[j].events & USRSOCK_POLLMASK) {
                    case USRSOCK_POLLFD:
                        (void)poll_fdsetup(fds[j].fd, &fds[j], false);
                        break;

                    default:
                        break;
                }
            }

            /* Indicate an error on the file descriptor */

            fds[i].revents |= USRSOCK_POLLERR;
            return ret;
        }
    }

    return OK;
}

static inline int poll_teardown(struct usrsock_pollfd *fds, unsigned int nfds,
                                int *count, int ret)
{
    unsigned int i;
    int status = OK;

    /* Process each descriptor in the list */

    *count = 0;
    for (i = 0; i < nfds; i++) {
        switch (fds[i].events & USRSOCK_POLLMASK) {
            case USRSOCK_POLLFD:
                if (fds[i].fd >= 0) {
                    status = poll_fdsetup(fds[i].fd, &fds[i], false);
                }
                break;

            default:
                status = -EINVAL;
                break;
        }

        if (status < 0) {
            ret = status;
        }
        /* Check if any events were posted */

        if (fds[i].revents != 0) {
            (*count)++;
        }

        /* Un-initialize the poll structure */

        fds[i].sem = NULL;
    }

    return ret;
}

static int poll_impl(struct usrsock_pollfd *fds, unsigned int nfds, int timeout)
{
    aos_sem_t sem;
    int count = 0;
    int errcode;
    int ret;

    USRSOCK_ASSERT("nfds == 0 || fds != NULL", nfds == 0 || fds != NULL);

    /* This semaphore is used for signaling and, hence, should not have
     * priority inheritance enabled.
     */

    aos_sem_new(&sem, 0);

    ret = poll_setup(fds, nfds, &sem);
    if (ret >= 0) {
        if (timeout == 0) {
            /* Poll returns immediately whether we have a poll event or not. */
            ret = OK;
        } else if (timeout > 0) {
            ret = aos_sem_wait(&sem, timeout);
            if (ret < 0) {
                if (ret == -ETIMEDOUT) {
                    /* Return zero (OK) in the event of a timeout */

                    ret = OK;
                }

                /* EINTR is the only other error expected in normal operation */
            }
        } else {
            /* Wait for the poll event or signal with no timeout */

            aos_sem_wait(&sem, AOS_WAIT_FOREVER);
        }

        /* Teardown the poll operation and get the count of events.  Zero will
         * be returned in the case of a timeout.
         *
         * Preserve ret, if negative, since it holds the result of the wait.
         */

        errcode = poll_teardown(fds, nfds, &count, ret);
        if (errcode < 0 && ret >= 0) {
            ret = errcode;
        }
    }

    aos_sem_free(&sem);

    /* Check for errors */

    if (ret < 0) {
        return ret;
    }

    return count;
}

int select_impl(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout)
{
    struct usrsock_pollfd *pollset = NULL;
    int errcode = OK;
    int fd;
    int npfds;
    int msec;
    int ndx;
    int ret;

    /* Initialize the descriptor list for poll() */

    for (fd = LWIP_SOCKET_OFFSET, npfds = 0; fd < nfds; fd++) {
        /* Check if any monitor operation is requested on this fd */

        if ((readfds && FD_ISSET(fd, readfds))  ||
            (writefds  && FD_ISSET(fd, writefds)) ||
            (exceptfds && FD_ISSET(fd, exceptfds))) {
            /* Yes.. increment the count of pollfds structures needed */

            npfds++;
        }
    }

    /* Allocate the descriptor list for poll() */

    if (npfds > 0) {
        pollset = (struct usrsock_pollfd *)aos_zalloc(npfds * sizeof(struct usrsock_pollfd));

        if (pollset == NULL) {
            errcode = ENOMEM;
            goto errout;
        }
    }

    /* Initialize the descriptor list for poll() */

    for (fd = LWIP_SOCKET_OFFSET, ndx = 0; fd < nfds; fd++) {
        int incr = 0;

        /* The readfs set holds the set of FDs that the caller can be assured
         * of reading from without blocking.  Note that POLLHUP is included as
         * a read-able condition.  POLLHUP will be reported at the end-of-file
         * or when a connection is lost.  In either case, the read() can then
         * be performed without blocking.
         */

        if (readfds && FD_ISSET(fd, readfds)) {
            pollset[ndx].fd      = fd;
            pollset[ndx].events |= USRSOCK_POLLIN;
            incr                 = 1;
        }

        /* The writefds set holds the set of FDs that the caller can be assured
         * of writing to without blocking.
         */

        if (writefds && FD_ISSET(fd, writefds)) {
            pollset[ndx].fd      = fd;
            pollset[ndx].events |= USRSOCK_POLLOUT;
            incr                 = 1;
        }

        /* The exceptfds set holds the set of FDs that are watched for exceptions */

        if (exceptfds && FD_ISSET(fd, exceptfds)) {
            pollset[ndx].fd      = fd;
            incr                  = 1;
        }

        ndx += incr;
    }

    USRSOCK_ASSERT("ndx == npfds", ndx == npfds);

    /* Convert the timeout to milliseconds */
    if (timeout) {
        /* Calculate the timeout in milliseconds */

        msec = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
    } else {
        /* Any negative value of msec means no timeout */

        msec = -1;
    }

    /* Then let poll do all of the real work. */

    ret = poll_impl(pollset, npfds, msec);
    if (ret < 0) {
        /* poll() failed! Save the errno value */

        errcode = ret;
    }

    /* Now set up the return values */

    if (readfds) {
        memset(readfds, 0, sizeof(fd_set));
    }

    if (writefds) {
        memset(writefds, 0, sizeof(fd_set));
    }

    if (exceptfds) {
        memset(exceptfds, 0, sizeof(fd_set));
    }

    /* Convert the poll descriptor list back into selects 3 bitsets */

    if (ret > 0) {
        ret = 0;
        for (ndx = 0; ndx < npfds; ndx++) {
            /* Check for read conditions.  Note that POLLHUP is included as a
             * read condition.  POLLHUP will be reported when no more data will
             * be available (such as when a connection is lost).  In either
             * case, the read() can then be performed without blocking.
             */

            if (readfds) {
                if (pollset[ndx].revents & (USRSOCK_POLLIN | USRSOCK_POLLHUP)) {
                    FD_SET(pollset[ndx].fd, readfds);
                    ret++;
                }
            }

            /* Check for write conditions */

            if (writefds) {
                if (pollset[ndx].revents & USRSOCK_POLLOUT) {
                    FD_SET(pollset[ndx].fd, writefds);
                    ret++;
                }
            }

            /* Check for exceptions */

            if (exceptfds) {
                if (pollset[ndx].revents & USRSOCK_POLLERR) {
                    FD_SET(pollset[ndx].fd, exceptfds);
                    ret++;
                }
            }
        }
    }

    aos_free(pollset);

    /* Did poll() fail above? */

    if (ret >= 0) {
        return ret;
    }

errout:
    return errcode;
}

int usrsocket_select(int nfds, fd_set *readfds, fd_set *writefds,
                     fd_set *exceptfds, struct timeval *timeout)
{
    return select_impl(nfds, readfds, writefds, exceptfds, timeout);
}

