/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_conn.c
 * @brief    user sock connection helper functions
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <aos/aos.h>
#include <soc.h>

#include "devif.h"
#include "net_usrsock.h"
#include "usrsock.h"

static aos_sem_t g_free_sem;
static aos_sem_t g_net_sem;
static dlist_t g_free_usrsock_connections;
static dlist_t g_active_usrsock_connections;
/* The array containing all usrsock connections. */

static struct usrsock_conn_s g_usrsock_connections[CONFIG_NET_USRSOCK_CONNS];
static void *g_holder;
static unsigned int g_count;

static void _usrsock_semtake(aos_sem_t *sem)
{
    net_lockedwait(sem);
}

static void _usrsock_semgive(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

/****************************************************************************
 * Name: usrsock_alloc()
 *
 * Description:
 *   Allocate a new, uninitialized usrsock connection structure.  This is
 *   normally something done by the implementation of the socket() API
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_alloc(void)
{
    struct usrsock_conn_s *conn;

    /* The free list is protected by a semaphore (that behaves like a mutex). */

    _usrsock_semtake(&g_free_sem);
    conn = dlist_first_entry(&g_free_usrsock_connections, struct usrsock_conn_s, node);
    if (conn) {
        /* Make sure that the connection is marked as uninitialized */

        memset(conn, 0, sizeof(*conn));
        aos_sem_new(&conn->resp.sem, 1);
        conn->dev = NULL;
        conn->usockid = -1;
        conn->state = USRSOCK_CONN_STATE_UNINITIALIZED;
        conn->list = NULL;
        conn->connected = false;

        /* Enqueue the connection into the active list */

        dlist_add_tail(&conn->node, &g_active_usrsock_connections);
    }

    _usrsock_semgive(&g_free_sem);
    return conn;
}

/****************************************************************************
 * Name: usrsock_free()
 *
 * Description:
 *   Free a usrsock connection structure that is no longer in use. This should
 *   be done by the implementation of close().
 *
 ****************************************************************************/

void usrsock_free(struct usrsock_conn_s *conn)
{
    /* The free list is protected by a semaphore (that behaves like a mutex). */

    USRSOCK_ASSERT("conn->crefs == 0", conn->crefs == 0);

    _usrsock_semtake(&g_free_sem);

    /* Remove the connection from the active list */

    dlist_del(&conn->node);

    /* Reset structure */

    aos_sem_free(&conn->resp.sem);
    memset(conn, 0, sizeof(*conn));
    conn->dev = NULL;
    conn->usockid = -1;
    conn->state = USRSOCK_CONN_STATE_UNINITIALIZED;
    conn->list = NULL;

    /* Free the connection */

    dlist_add_tail(&conn->node, &g_free_usrsock_connections);
    _usrsock_semgive(&g_free_sem);
}

/****************************************************************************
 * Name: usrsock_nextconn()
 *
 * Description:
 *   Traverse the list of allocated usrsock connections
 *
 * Assumptions:
 *   This function is called from usrsock device logic.
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_nextconn(struct usrsock_conn_s *conn)
{
    if (!conn)
    {
        return (struct usrsock_conn_s *)g_active_usrsock_connections.prev;
    }
    else
    {
        return (struct usrsock_conn_s *)conn->node.next;
    }
}

/****************************************************************************
 * Name: usrsock_connidx()
 ****************************************************************************/

int usrsock_connidx(struct usrsock_conn_s *conn)
{
    int idx = conn - g_usrsock_connections;

    USRSOCK_ASSERT("idx >= 0", idx >= 0);
    USRSOCK_ASSERT("idx < ARRAY_SIZE(g_usrsock_connections))", idx < ARRAY_SIZE(g_usrsock_connections));

    return idx;
}

/****************************************************************************
 * Name: usrsock_active()
 *
 * Description:
 *   Find a connection structure that is the appropriate
 *   connection for usrsock
 *
 * Assumptions:
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_active(int16_t usockid)
{
    struct usrsock_conn_s *conn = NULL;

    while ((conn = usrsock_nextconn(conn)) != NULL)
    {
        if (conn->usockid == usockid)
            return conn;
    }

    return NULL;
}



/****************************************************************************
 * Name: usrsock_setup_request_callback()
 ****************************************************************************/

int usrsock_setup_request_callback(struct usrsock_conn_s *conn,
        struct usrsock_reqstate_s *pstate,
        devif_callback_event_t event,
        uint16_t flags)
{
    int ret = -EBUSY;

    aos_sem_new(&pstate->recvsem, 0);

    pstate->conn   = conn;
    pstate->result = -EAGAIN;
    pstate->completed = false;
    pstate->unlock = false;

    /* Set up the callback in the connection */

    pstate->cb = devif_callback_alloc(&conn->list);
    if (pstate->cb)
    {
        /* Take a lock since only one outstanding request is allowed */

        if ((flags & USRSOCK_EVENT_REQ_COMPLETE) != 0)
        {
            _usrsock_semtake(&conn->resp.sem);
            pstate->unlock = true;
        }

        /* Set up the connection event handler */

        pstate->cb->flags = flags;
        pstate->cb->priv  = (void *)pstate;
        pstate->cb->event = event;

        ret = OK;
    }

    return ret;
}

/****************************************************************************
 * Name: usrsock_setup_data_request_callback()
 ****************************************************************************/

int usrsock_setup_data_request_callback(struct usrsock_conn_s *conn,
        struct usrsock_data_reqstate_s *pstate,
        devif_callback_event_t event,
        uint16_t flags)
{
    pstate->valuelen = 0;
    pstate->valuelen_nontrunc = 0;
    return usrsock_setup_request_callback(conn, &pstate->reqstate, event,
            flags);
}

/****************************************************************************
 * Name: usrsock_teardown_request_callback()
 ****************************************************************************/

void usrsock_teardown_request_callback(struct usrsock_reqstate_s *pstate)
{
    struct usrsock_conn_s *conn = pstate->conn;

    if (pstate->unlock)
    {
        _usrsock_semgive(&conn->resp.sem);
    }

    /* Make sure that no further events are processed */

    devif_conn_callback_free(pstate->cb, &conn->list);
    aos_sem_free(&pstate->recvsem);

    pstate->cb = NULL;
}

/****************************************************************************
 * Name: usrsock_setup_datain
 ****************************************************************************/

void usrsock_setup_datain(struct usrsock_conn_s *conn,
                          struct iovec *iov, unsigned int iovcnt)
{
    unsigned int i;

    conn->resp.datain.iov = iov;
    conn->resp.datain.pos = 0;
    conn->resp.datain.total = 0;
    conn->resp.datain.iovcnt = iovcnt;

    for (i = 0; i < iovcnt; i++)
    {
        conn->resp.datain.total += iov[i].iov_len;
    }
}

void net_lock(void)
{
    if (g_holder == aos_task_self().hdl) {
        g_count++;
    } else {
        aos_sem_wait(&g_net_sem, AOS_WAIT_FOREVER);
        g_holder = aos_task_self().hdl;
        g_count = 1;
    }
}

void net_unlock(void)
{
    if (g_count == 1) {
        g_holder = NULL;
        g_count = 0;
        aos_sem_signal(&g_net_sem);
    } else {
        g_count--;
    }
}

int net_breaklock(unsigned int *count)
{
    uint32_t flags;
    int ret = -EPERM;

    flags = csi_irq_save();
    if (g_holder == aos_task_self().hdl) {

        /* Return the lock setting */

        *count   = g_count;

        /* Release the network lock  */

        g_holder = NULL;
        g_count = 0;

        aos_sem_signal(&g_net_sem);
        ret = OK;
    }

    csi_irq_restore(flags);
    return ret;
}

void net_restorelock(unsigned int count)
{
    /* Recover the network lock at the proper count */

    aos_sem_wait(&g_net_sem, AOS_WAIT_FOREVER);
    g_holder = aos_task_self().hdl;
    g_count = count;
}

int net_timedwait(aos_sem_t *sem, int timeout)
{
    unsigned int count;
    uint32_t     flags;
    int          blresult;
    int          ret;

    flags = csi_irq_save();         /* No interrupts */

    /* Release the network lock, remembering my count.  net_breaklock will
     * return a negated value if the caller does not hold the network lock.
     */

    blresult = net_breaklock(&count);

    /* Now take the semaphore, waiting if so requested. */
    if (timeout == 0) {
        ret = aos_sem_wait(sem, AOS_WAIT_FOREVER);
    } else {
        ret = aos_sem_wait(sem, timeout);
    }

    /* Recover the network lock at the proper count (if we held it before) */

    if (blresult >= 0) {
        net_restorelock(count);
    }

    csi_irq_restore(flags);

    return ret;

}

int net_lockedwait(aos_sem_t *sem)
{
    return net_timedwait(sem, 0);
}

/****************************************************************************
 * Name: usrsock_initialize()
 *
 * Description:
 *   Initialize the User Socket connection structures.  Called once and only
 *   from the networking layer.
 *
 ****************************************************************************/

void usrsock_initialize(void)
{
    int i;

    devif_initialize();

    /* Initialize the queues */

    dlist_init(&g_free_usrsock_connections);
    dlist_init(&g_active_usrsock_connections);
    aos_sem_new(&g_free_sem, 1);
    aos_sem_new(&g_net_sem, 1);

    for (i = 0; i < CONFIG_NET_USRSOCK_CONNS; i++)
    {
        struct usrsock_conn_s *conn = &g_usrsock_connections[i];

        /* Mark the connection closed and move it to the free list */

        memset(conn, 0, sizeof(*conn));
        conn->dev     = NULL;
        conn->usockid = -1;
        conn->state   = USRSOCK_CONN_STATE_UNINITIALIZED;
        conn->list    = NULL;
        conn->flags   = 0;
        dlist_add_tail(&conn->node, &g_free_usrsock_connections);
    }

    /* Register /dev/usrsock character device. */

    usrsockdev_register();
}

/****************************************************************************
 * Name: usrsock_event
 *
 * Description:
 *   Handler for received connection events
 *
 ****************************************************************************/

int usrsock_event(struct usrsock_conn_s *conn, uint16_t events)
{
    LOGI(USRSOCK_TAG, "events: %04X\n", events);

    if (!events) {
        return OK;
    }

    net_lock();

    /* Generic state updates. */

    if (events & USRSOCK_EVENT_REQ_COMPLETE) {
        if (conn->state == USRSOCK_CONN_STATE_CONNECTING) {
            conn->state = USRSOCK_CONN_STATE_READY;
            events |= USRSOCK_EVENT_CONNECT_READY;

            if (conn->resp.result == 0)
            {
                conn->connected = true;
            }
        }
    }

    if (events & USRSOCK_EVENT_ABORT) {
        conn->state = USRSOCK_CONN_STATE_ABORTED;
    }

    if (events & USRSOCK_EVENT_REMOTE_CLOSED) {
        /* After reception of remote close event, clear input/output flags. */

        conn->flags &= ~(USRSOCK_EVENT_SENDTO_READY |
                USRSOCK_EVENT_RECVFROM_AVAIL);

        conn->flags |= USRSOCK_EVENT_REMOTE_CLOSED;
    }

    if ((conn->state == USRSOCK_CONN_STATE_READY ||
                conn->state == USRSOCK_CONN_STATE_CONNECTING) &&
            !(conn->flags & USRSOCK_EVENT_REMOTE_CLOSED)) {
        if (events & USRSOCK_EVENT_SENDTO_READY) {
            conn->flags |= USRSOCK_EVENT_SENDTO_READY;
        }

        if (events & USRSOCK_EVENT_RECVFROM_AVAIL) {
            conn->flags |= USRSOCK_EVENT_RECVFROM_AVAIL;
        }
    }

    /* Send events to callbacks */

    (void)devif_conn_event(conn, events, conn->list);
    net_unlock();

    return OK;
}

