/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_dev.c
 * @brief    user sock device driver
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <aos/types.h>
#include <soc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef CONFIG_USRSOCK_NUM
#include <devices/hal/usrsock_impl.h>
#else
#include <vfs.h>
#include <vfs_register.h>
#include <vfs_inode.h>
#endif

#include "devif.h"
#include "lwip/sockets.h"
#include "net_usrsock.h"
#include "usrsock.h"

struct usrsockdev_s
{
    aos_dev_t device;
    aos_sem_t devsem;              /* Lock for device node */
    uint8_t ocount;                /* The number of times the device has been opened */

    struct
    {
        const struct iovec *iov;     /* Pending request buffers */
        int iovcnt;                  /* Number of request buffers */
        size_t pos;                  /* Reader position on request buffer */
        aos_sem_t sem;               /* Request semaphore (only one outstanding request) */
        aos_sem_t acksem;            /* Request acknowledgment notification */
        uint8_t ack_xid;             /* Exchange id for which waiting ack */
        uint16_t nbusy;              /* Number of requests blocked from different threads */
    } req;

    struct usrsock_conn_s *datain_conn; /* Connection instance to receive data buffers. */

#ifdef CONFIG_USRSOCK_NUM
    void *init_cfg;
    void (*event)(aos_dev_t *dev, int event_id, void *priv);
    void *priv;
#else
#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
    struct pollfd *pollfds[CONFIG_NET_USRSOCKDEV_NPOLLWAITERS];
#endif
#endif
};

static struct usrsockdev_s *g_usrsockdev;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Character driver methods */
#ifdef CONFIG_USRSOCK_NUM
static aos_dev_t *usrsockdev_init(driver_t *drv, void *config, int id);

static void usrsockdev_uninit(aos_dev_t *adev);

static int usrsockdev_open(aos_dev_t *adev);

static int usrsockdev_close(aos_dev_t *adev);

static ssize_t usrsockdev_read(aos_dev_t *adev,
                             void *data, size_t size);

static ssize_t usrsockdev_write(aos_dev_t *adev,
                                const void *data, size_t size);

static void usrsockdev_set_event(aos_dev_t *adev,
               void (*event)(aos_dev_t *adev, int event_id, void *priv),
               void * priv);

#else
static ssize_t usrsockdev_read(struct file_t *filep, char *buffer,
                               size_t len);

static ssize_t usrsockdev_write(struct file_t *filep,
                                const char *buffer, size_t len);

static int usrsockdev_open(struct file_t *filep);

static int usrsockdev_close(struct file_t *filep);

#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
static int usrsockdev_poll(struct file_t *filep, bool flag,
                           poll_notify_t notify, struct pollfd *fd,
                           void *arg);
#endif
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef CONFIG_USRSOCK_NUM
static usrsock_driver_t g_usrsock_driver = {
    .drv = {
        .name   = "usrsock",
        .init   = usrsockdev_init,
        .uninit = usrsockdev_uninit,
        .open   = usrsockdev_open,
        .close  = usrsockdev_close,
    },
    .write           = usrsockdev_write,
    .read            = usrsockdev_read,
    .set_event       = usrsockdev_set_event,
};

#else
static const struct file_ops g_usrsockdevops =
{
  usrsockdev_open,    /* open */
  usrsockdev_close,   /* close */
  usrsockdev_read,    /* read */
  usrsockdev_write,   /* write */
  NULL                /* ioctl */
#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
  , usrsockdev_poll   /* poll */
#endif
};
static struct usrsockdev_s g_usrsockdev;
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: iovec_do() - copy to/from iovec from/to buffer.
 ****************************************************************************/

/****************************************************************************
 * Name: iovec_do() - copy to/from iovec from/to buffer.
 ****************************************************************************/

static ssize_t iovec_do(void *srcdst, size_t srcdstlen,
                        struct iovec *iov, int iovcnt, size_t pos,
                        bool from_iov)
{
    ssize_t total;
    size_t srclen;
    uint8_t *ioout = srcdst;
    uint8_t *iovbuf;

    /* Rewind to correct position. */

    while (pos > 0 && iovcnt > 0) {
        if (iov->iov_len <= pos) {
            pos -= iov->iov_len;
            iov++;
            iovcnt--;
        }
        else {
            break;
        }
    }

    if (iovcnt == 0) {
        /* Position beyond iovec. */

        return -1;
    }

    iovbuf = iov->iov_base;
    srclen = iov->iov_len;
    iovbuf += pos;
    srclen -= pos;
    iov++;
    iovcnt--;
    total = 0;

    while ((srclen > 0 || iovcnt > 0) && srcdstlen > 0) {
        size_t clen = srclen;

        if (srclen == 0) {
            /* Skip empty iovec. */

            iovbuf = iov->iov_base;
            srclen = iov->iov_len;
            iov++;
            iovcnt--;

            continue;
        }

        if (clen > srcdstlen) {
            clen = srcdstlen;
        }

        if (from_iov) {
            memmove(ioout, iovbuf, clen);
        } else {
            memmove(iovbuf, ioout, clen);
        }

        ioout += clen;
        srcdstlen -= clen;
        iovbuf += clen;
        srclen -= clen;
        total += clen;

        if (srclen == 0) {
            if (iovcnt == 0) {
                break;
            }

            iovbuf = iov->iov_base;
            srclen = iov->iov_len;
            iov++;
            iovcnt--;
        }
    }

    return total;
}

/****************************************************************************
 * Name: iovec_get() - copy from iovec to buffer.
 ****************************************************************************/

static ssize_t iovec_get(void *dst, size_t dstlen,
                         const struct iovec *iov, int iovcnt, size_t pos)
{
  return iovec_do(dst, dstlen, (struct iovec *)iov, iovcnt, pos, true);
}

/****************************************************************************
 * Name: iovec_put() - copy to iovec from buffer.
 ****************************************************************************/

static ssize_t iovec_put(struct iovec *iov, int iovcnt, size_t pos,
                         const void *src, size_t srclen)
{
  return iovec_do((void *)src, srclen, iov, iovcnt, pos, false);
}

/****************************************************************************
 * Name: usrsockdev_get_xid()
 ****************************************************************************/

static uint8_t usrsockdev_get_xid(struct usrsock_conn_s *conn)
{
    int conn_idx;

    /* Each connection can one only one request/response pending. So map
     * connection structure index to xid value.
     */

    conn_idx = usrsock_connidx(conn);

    USRSOCK_ASSERT("1 <= conn_idx + 1", 1 <= conn_idx + 1);
    USRSOCK_ASSERT("conn_idx + 1 <= UINT8_MAX", conn_idx + 1 <= UINT8_MAX);

    return conn_idx + 1;
}

/****************************************************************************
 * Name: usrsockdev_semtake() and usrsockdev_semgive()
 *
 * Description:
 *   Take/give semaphore
 *
 ****************************************************************************/

static void usrsockdev_semtake(aos_sem_t *sem)
{
    aos_sem_wait(sem, AOS_WAIT_FOREVER);
}

static void usrsockdev_semgive(aos_sem_t *sem)
{
    aos_sem_signal(sem);
}

/****************************************************************************
 * Name: usrsockdev_is_opened
 ****************************************************************************/

static bool usrsockdev_is_opened(struct usrsockdev_s *dev)
{
    bool ret = true;

    if (dev->ocount == 0) {
        ret = false; /* No usrsock daemon running. */
    }

    return ret;
}

/****************************************************************************
 * Name: usrsockdev_pollnotify
 ****************************************************************************/

static void usrsockdev_pollnotify(struct usrsockdev_s *dev,
        uint8_t eventset)
{
#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
    int i;
    for (i = 0; i < ARRAY_SIZE(dev->pollfds); i++) {
        struct pollfd *fds = dev->pollfds[i];
        if (fds) {
            fds->revents |= (fds->events & eventset);
            if (fds->revents != 0) {
                LOGE(USRSOCK_TAG, "Report events: %02x\n", fds->revents);
                aos_sem_signal(fds->sem);
            }
        }
    }
#endif
}

/****************************************************************************
 * Name: usrsockdev_read
 ****************************************************************************/
#ifdef CONFIG_USRSOCK_NUM
static ssize_t usrsockdev_read(aos_dev_t *adev, void *buffer, size_t len)
#else
static ssize_t usrsockdev_read(struct file_t *filep, char *buffer, size_t len)
#endif
{
#ifdef CONFIG_USRSOCK_NUM
    struct usrsockdev_s *dev = (struct usrsockdev_s *)adev;
#else
    struct usrsockdev_s *dev = filep->f_arg;
#endif

    USRSOCK_ASSERT("dev", dev);

    if (len == 0) {
        return 0;
    }

    if (buffer == NULL) {
        return -EINVAL;
    }

    usrsockdev_semtake(&dev->devsem);
    net_lock();

    /* Is request available? */

    if (dev->req.iov) {
        ssize_t rlen;

        /* Copy request to user-space. */

        rlen = iovec_get(buffer, len, dev->req.iov, dev->req.iovcnt,
                dev->req.pos);
        if (rlen < 0) {
            /* Tried reading beyond buffer. */

            len = 0;
        } else {
            dev->req.pos += rlen;
            len = rlen;
        }
    } else {
        len = 0;
    }

    net_unlock();
    usrsockdev_semgive(&dev->devsem);

    return len;
}

/****************************************************************************
 * Name: usrsockdev_handle_event
 ****************************************************************************/

static ssize_t usrsockdev_handle_event(struct usrsockdev_s *dev,
                                       const void *buffer,
                                       size_t len)
{
    const struct usrsock_message_common_s *common = buffer;

    switch (common->msgid) {
        case USRSOCK_MESSAGE_SOCKET_EVENT: {
                const struct usrsock_message_socket_event_s *hdr = buffer;
                struct usrsock_conn_s *conn;
                int ret;

                if (len < sizeof(*hdr)) {
                    LOGE(USRSOCK_TAG, "message too short, %d < %d.\n", len, sizeof(*hdr));

                    return -EINVAL;
                }

                /* Get corresponding usrsock connection. */

                conn = usrsock_active(hdr->usockid);
                if (!conn) {
                    LOGE(USRSOCK_TAG, "no active connection for usockid=%d.\n", hdr->usockid);

                    return -ENOENT;
                }

                /* Handle event. */

                ret = usrsock_event(conn, hdr->events & ~USRSOCK_EVENT_INTERNAL_MASK);
                if (ret < 0) {
                    return ret;
                }

                len = sizeof(*hdr);
        }
        break;

        default:
            LOGE(USRSOCK_TAG,, "Unknown event type: %d\n", common->msgid);
            return -EINVAL;
    }

    return len;
}

/****************************************************************************
 * Name: usrsockdev_handle_response
 ****************************************************************************/

static ssize_t usrsockdev_handle_response(struct usrsockdev_s *dev,
                                          struct usrsock_conn_s *conn,
                                          const void *buffer)
{
    const struct usrsock_message_req_ack_s *hdr = buffer;

    if (USRSOCK_MESSAGE_REQ_IN_PROGRESS(hdr->head.flags)) {
        /* In-progress response is acknowledgment that response was
         * received.
         */

        conn->resp.inprogress = true;
    } else {
        conn->resp.inprogress = false;
        conn->resp.xid = 0;

        /* Get result for common request. */

        conn->resp.result = hdr->result;

        /* Done with request/response. */

        (void)usrsock_event(conn, USRSOCK_EVENT_REQ_COMPLETE);
    }

    return sizeof(*hdr);
}

/****************************************************************************
 * Name: usrsockdev_handle_datareq_response
 ****************************************************************************/

static ssize_t
usrsockdev_handle_datareq_response(struct usrsockdev_s *dev,
                                   struct usrsock_conn_s *conn,
                                   const void *buffer)
{
    const struct usrsock_message_datareq_ack_s *datahdr = buffer;
    const struct usrsock_message_req_ack_s *hdr = &datahdr->reqack;
    int num_inbufs;
    int iovpos;
    ssize_t ret;

    if (USRSOCK_MESSAGE_REQ_IN_PROGRESS(hdr->head.flags)) {
        if (datahdr->reqack.result > 0) {
            LOGE(USRSOCK_TAG, "error: request in progress, and result > 0.\n");
            ret = -EINVAL;
            goto unlock_out;
        } else if (datahdr->valuelen > 0) {
            LOGE(USRSOCK_TAG, "error: request in progress, and valuelen > 0.\n");
            ret = -EINVAL;
            goto unlock_out;
        }

        /* In-progress response is acknowledgment that response was
         * received.
         */

        conn->resp.inprogress = true;

        ret = sizeof(*datahdr);
        goto unlock_out;
    }

    conn->resp.inprogress = false;
    conn->resp.xid = 0;

    /* Prepare to read buffers. */

    conn->resp.result = hdr->result;
    conn->resp.valuelen = datahdr->valuelen;
    conn->resp.valuelen_nontrunc = datahdr->valuelen_nontrunc;

    if (conn->resp.result < 0) {
        /* Error, valuelen must be zero. */

        if (datahdr->valuelen > 0 || datahdr->valuelen_nontrunc > 0) {
            LOGE(USRSOCK_TAG, "error: response result negative, and valuelen or "
                    "valuelen_nontrunc non-zero.\n");

            ret = -EINVAL;
            goto unlock_out;
        }

        /* Done with request/response. */

        (void)usrsock_event(conn, USRSOCK_EVENT_REQ_COMPLETE);

        ret = sizeof(*datahdr);
        goto unlock_out;
    }

    /* Check that number of buffers match available. */

    num_inbufs = (hdr->result > 0) + 1;

    if (conn->resp.datain.iovcnt < num_inbufs) {
        LOGE(USRSOCK_TAG, "not enough recv buffers (need: %d, have: %d).\n", num_inbufs,
                conn->resp.datain.iovcnt);

        ret = -EINVAL;
        goto unlock_out;
    }

    /* Adjust length of receiving buffers. */

    conn->resp.datain.total = 0;
    iovpos = 0;

    /* Value buffer is always the first */

    if (conn->resp.datain.iov[iovpos].iov_len < datahdr->valuelen) {
        LOGE(USRSOCK_TAG, "%dth buffer not large enough (need: %d, have: %d).\n",
                iovpos,
                datahdr->valuelen,
                conn->resp.datain.iov[iovpos].iov_len);

        ret = -EINVAL;
        goto unlock_out;
    }

    /* Adjust read size. */

    conn->resp.datain.iov[iovpos].iov_len = datahdr->valuelen;
    conn->resp.datain.total += conn->resp.datain.iov[iovpos].iov_len;
    iovpos++;

    if (hdr->result > 0) {
        /* Value buffer is always the first */

        if (conn->resp.datain.iov[iovpos].iov_len < hdr->result) {
            LOGE(USRSOCK_TAG, "%dth buffer not large enough (need: %d, have: %d).\n",
                    iovpos,
                    hdr->result,
                    conn->resp.datain.iov[iovpos].iov_len);

            ret = -EINVAL;
            goto unlock_out;
        }

        /* Adjust read size. */

        conn->resp.datain.iov[iovpos].iov_len = hdr->result;
        conn->resp.datain.total += conn->resp.datain.iov[iovpos].iov_len;
        iovpos++;
    }

    USRSOCK_ASSERT("num_inbufs == iovpos", num_inbufs == iovpos);

    conn->resp.datain.iovcnt = num_inbufs;

    /* Next written buffers are redirected to data buffers. */

    dev->datain_conn = conn;
    ret = sizeof(*datahdr);

unlock_out:
    return ret;
}

/****************************************************************************
 * Name: usrsockdev_handle_req_response
 ****************************************************************************/

static ssize_t usrsockdev_handle_req_response(struct usrsockdev_s *dev,
                                              const void *buffer,
                                              size_t len)
{
    const struct usrsock_message_req_ack_s *hdr = buffer;
    struct usrsock_conn_s *conn;
    unsigned int hdrlen;
    ssize_t ret;
    ssize_t (*handle_response)(struct usrsockdev_s *dev,
            struct usrsock_conn_s *conn,
            const void *buffer);

    switch (hdr->head.msgid) {
        case USRSOCK_MESSAGE_RESPONSE_ACK:
            hdrlen = sizeof(struct usrsock_message_req_ack_s);
            handle_response = &usrsockdev_handle_response;
            break;

        case USRSOCK_MESSAGE_RESPONSE_DATA_ACK:
            hdrlen = sizeof(struct usrsock_message_datareq_ack_s);
            handle_response = &usrsockdev_handle_datareq_response;
            break;

        default:
            LOGE(USRSOCK_TAG, "unknown message type: %d, flags: %d, xid: %02x, result: %d\n",
                    hdr->head.msgid, hdr->head.flags, hdr->xid, hdr->result);
            return -EINVAL;
    }

    if (len < hdrlen) {
        LOGE(USRSOCK_TAG, "message too short, %d < %d.\n", len, hdrlen);

        return -EINVAL;
    }

    net_lock();

    /* Get corresponding usrsock connection for this transfer */

    conn = usrsock_nextconn(NULL);
    while (conn) {
        if (conn->resp.xid == hdr->xid)
            break;

        conn = usrsock_nextconn(conn);
    }

    if (!conn) {
        /* No connection waiting for this message. */

        LOGE(USRSOCK_TAG, "Could find connection waiting for response with xid=%d\n",
                hdr->xid);

        ret = -EINVAL;
        goto unlock_out;
    }

    if (dev->req.ack_xid == hdr->xid && dev->req.iov) {
        /* Signal that request was received and read by daemon and
         * acknowledgment response was received.
         */

        dev->req.iov = NULL;

        aos_sem_signal(&dev->req.acksem);
    }

    ret = handle_response(dev, conn, buffer);

unlock_out:
    net_unlock();
    return ret;
}

/****************************************************************************
 * Name: usrsockdev_handle_message
 ****************************************************************************/

static ssize_t usrsockdev_handle_message(struct usrsockdev_s *dev,
                                         const void *buffer,
                                         size_t len)
{
    const struct usrsock_message_common_s *common = buffer;

    if (USRSOCK_MESSAGE_IS_EVENT(common->flags)) {
        return usrsockdev_handle_event(dev, buffer, len);
    }

    if (USRSOCK_MESSAGE_IS_REQ_RESPONSE(common->flags)) {
        return usrsockdev_handle_req_response(dev, buffer, len);
    }

    return -EINVAL;
}

/****************************************************************************
 * Name: usrsockdev_write
 ****************************************************************************/
#ifdef CONFIG_USRSOCK_NUM
static ssize_t usrsockdev_write(aos_dev_t *adev,
                                const void *buffer, size_t len)
#else
static ssize_t usrsockdev_write(struct file_t *filep,
                                const char *buffer, size_t len)
#endif
{
    struct usrsock_conn_s *conn;
    size_t origlen = len;
    ssize_t ret = 0;

#ifdef CONFIG_USRSOCK_NUM
    struct usrsockdev_s *dev = (struct usrsockdev_s *)adev;
#else
    struct usrsockdev_s *dev = filep->f_arg;
#endif

    if (len == 0) {
        return 0;
    }

    if (buffer == NULL) {
        return -EINVAL;
    }

    USRSOCK_ASSERT("dev", dev);

    usrsockdev_semtake(&dev->devsem);

    if (!dev->datain_conn) {
        /* Start of message, buffer length should be at least size of common
         * message header.
         */

        if (len < sizeof(struct usrsock_message_common_s)) {
            LOGE(USRSOCK_TAG, "message too short, %d < %d.\n", len,
                    sizeof(struct usrsock_message_common_s));

            ret = -EINVAL;
            goto errout;
        }

        /* Handle message. */

        ret = usrsockdev_handle_message(dev, buffer, len);
        if (ret >= 0) {
            buffer += ret;
            len -= ret;
            ret = origlen - len;
        }
    }

    /* Data input handling. */

    if (dev->datain_conn) {
        conn = dev->datain_conn;

        /* Copy data from user-space. */

        ret = iovec_put(conn->resp.datain.iov, conn->resp.datain.iovcnt,
                conn->resp.datain.pos, buffer, len);
        if (ret < 0) {
            /* Tried writing beyond buffer. */

            ret = -EINVAL;
            conn->resp.result = -EINVAL;
            conn->resp.datain.pos =
                conn->resp.datain.total;
        } else {
            conn->resp.datain.pos += ret;
            buffer += ret;
            len -= ret;
            ret = origlen - len;
        }

        if (conn->resp.datain.pos == conn->resp.datain.total) {
            dev->datain_conn = NULL;

            /* Done with data response. */

            (void)usrsock_event(conn, USRSOCK_EVENT_REQ_COMPLETE);
        }
    }

errout:
    usrsockdev_semgive(&dev->devsem);
    return ret;
}

/****************************************************************************
 * Name: usrsockdev_open
 ****************************************************************************/
#ifdef CONFIG_USRSOCK_NUM
static int usrsockdev_open(aos_dev_t *adev)
#else
static int usrsockdev_open(struct file_t *filep)
#endif
{
    int ret;
    int tmp;
#ifdef CONFIG_USRSOCK_NUM
    struct usrsockdev_s *dev = (struct usrsockdev_s *)adev;
#else
    struct inode_t *inode = filep->f_inode;

    struct usrsockdev_s *dev;

    USRSOCK_ASSERT("inode", inode);

    dev = inode->i_arg;

    filep->f_arg = dev;
#endif
    USRSOCK_ASSERT("dev", dev);
    usrsockdev_semtake(&dev->devsem);

    LOGI(USRSOCK_TAG, "opening /usr/usrsock\n");

    /* Increment the count of references to the device. */

    tmp = dev->ocount + 1;
    if (tmp > 1) {
        /* Only one reference is allowed. */

        LOGE(USRSOCK_TAG, "failed to open\n");

        ret = -EPERM;
    } else {
        dev->ocount = tmp;
        ret = OK;
    }

    usrsockdev_semgive(&dev->devsem);

    return ret;
}

/****************************************************************************
 * Name: usrsockdev_close
 ****************************************************************************/
#ifdef CONFIG_USRSOCK_NUM
static int usrsockdev_close(aos_dev_t *adev)
#else
static int usrsockdev_close(struct file_t *filep)
#endif
{
    struct usrsock_conn_s *conn;
    int ret;
#ifdef CONFIG_USRSOCK_NUM
    struct usrsockdev_s *dev = (struct usrsockdev_s *)adev;
#else
    struct inode_t *inode = filep->f_inode;
    struct usrsockdev_s *dev;

    USRSOCK_ASSERT("inode", inode);

    dev = inode->i_arg;
#endif
    USRSOCK_ASSERT("dev", dev);
    usrsockdev_semtake(&dev->devsem);

    LOGI(USRSOCK_TAG, "closing /dev/usrsock\n");

    /* Set active usrsock sockets to aborted state. */

    conn = usrsock_nextconn(NULL);
    while (conn) {
        net_lock();

        conn->resp.inprogress = false;
        conn->resp.xid = 0;
        usrsock_event(conn, USRSOCK_EVENT_ABORT);

        net_unlock();

        conn = usrsock_nextconn(conn);
    }

    net_lock();

    /* Decrement the references to the driver. */

    dev->ocount--;
    USRSOCK_ASSERT("dev->ocount == 0", dev->ocount == 0);
    ret = OK;

    do {
        /* Give other threads short time window to complete recently completed
         * requests.
         */

        ret = aos_sem_wait(&dev->req.sem, 10);
        if (ret < 0) {
            if (ret != -ETIMEDOUT && ret != -EINTR)
            {
                LOGE(USRSOCK_TAG, "net_timedwait errno: %d\n", ret);
            }
        } else {
            usrsockdev_semgive(&dev->req.sem);
        }

        /* Wake-up pending requests. */

        if (dev->req.nbusy == 0) {
            break;
        }

        dev->req.iov = NULL;
        aos_sem_signal(&dev->req.acksem);
    }
    while (true);

    net_unlock();

    /* Check if request line is active */

    if (dev->req.iov != NULL) {
        dev->req.iov = NULL;
    }

    usrsockdev_semgive(&dev->devsem);

    return ret;
}

/****************************************************************************
 * Name: usrsockdev_poll
 ****************************************************************************/

#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
static int usrsockdev_poll(struct file_t *filep, struct pollfd *fds,
                           bool setup)
{
    struct usrsockdev_s *dev;
    pollevent_t eventset;
    int ret = OK;
    int i;

    dev = filep->f_arg;

    USRSOCK_ASSERT("dev", dev);

    /* Some sanity checking */

    if (!dev || !fds)
    {
        return -ENODEV;
    }

    /* Are we setting up the poll?  Or tearing it down? */

    usrsockdev_semtake(&dev->devsem);
    net_lock();
    if (setup) {
        /* This is a request to set up the poll.  Find an available
         * slot for the poll structure reference
         */

        for (i = 0; i < ARRAY_SIZE(dev->pollfds); i++) {
            /* Find an available slot */

            if (!dev->pollfds[i]) {
                /* Bind the poll structure and this slot */

                dev->pollfds[i] = fds;
                fds->priv = &dev->pollfds[i];
                break;
            }
        }

        if (i >= ARRAY_SIZE(dev->pollfds)) {
            fds->priv = NULL;
            ret = -EBUSY;
            goto errout;
        }

        /* Should immediately notify on any of the requested events? */

        eventset = 0;

        /* Notify the POLLIN event if pending request. */

        if (dev->req.iov != NULL &&
                !(iovec_get(NULL, 0, dev->req.iov,
                        dev->req.iovcnt, dev->req.pos) < 0))
        {
            eventset |= POLLIN;
        }

        if (eventset) {
            usrsockdev_pollnotify(dev, eventset);
        }
    } else {
        /* This is a request to tear down the poll. */

        struct pollfd **slot = (FAR struct pollfd **)fds->priv;

        if (!slot) {
            ret = -EIO;
            goto errout;
        }

        /* Remove all memory of the poll setup */

        *slot = NULL;
        fds->priv = NULL;
    }

errout:
    net_unlock();
    usrsockdev_semgive(&dev->devsem);
    return ret;
}
#endif

/****************************************************************************
 * Name: usrsockdev_do_request
 ****************************************************************************/

int usrsockdev_do_request(struct usrsock_conn_s *conn,
        struct iovec *iov, unsigned int iovcnt)
{
    struct usrsockdev_s *dev = conn->dev;
    struct usrsock_request_common_s *req_head = iov[0].iov_base;
    int ret = OK;

    if (!dev)
    {
        /* Setup conn for new usrsock device. */

        USRSOCK_ASSERT("req_head->reqid == USRSOCK_REQUEST_SOCKET", req_head->reqid == USRSOCK_REQUEST_SOCKET);
        dev = g_usrsockdev;
        conn->dev = dev;
    }

    if (!usrsockdev_is_opened(dev))
    {
        LOGE(USRSOCK_TAG, "usockid=%d; daemon has closed /usr/usrsock.\n", conn->usockid);

        return -ENETDOWN;
    }

    /* Get exchange id. */

    req_head->xid = usrsockdev_get_xid(conn);

    /* Prepare connection for response. */

    conn->resp.xid = req_head->xid;
    conn->resp.result = -EACCES;

    ++dev->req.nbusy; /* net_lock held. */

    /* Set outstanding request for daemon to handle. */

    net_lockedwait(&dev->req.sem);

    if (usrsockdev_is_opened(dev)) {
        USRSOCK_ASSERT("dev->req.iov == NULL", dev->req.iov == NULL);
        dev->req.ack_xid = req_head->xid;
        dev->req.iov = iov;
        dev->req.pos = 0;
        dev->req.iovcnt = iovcnt;

        /* Notify daemon of new request. */
#ifdef CONFIG_USRSOCK_NUM
        dev->event((aos_dev_t *)dev, POLLIN, dev->priv);
#else
        usrsockdev_pollnotify(dev, POLLIN);
#endif

        /* Wait ack for request. */

        net_lockedwait(&dev->req.acksem);
    } else {
        LOGE(USRSOCK_TAG, "usockid=%d; daemon abruptly closed /usr/usrsock.\n",
                conn->usockid);
        ret = -ESHUTDOWN;
    }

    /* Free request line for next command. */

    usrsockdev_semgive(&dev->req.sem);

    --dev->req.nbusy; /* net_lock held. */

    return ret;
}

#ifdef CONFIG_USRSOCK_NUM
static aos_dev_t *usrsockdev_init(driver_t *drv, void *config, int id)
{
    struct usrsockdev_s *usrsockdev = (struct usrsockdev_s *)device_new(
                                        drv, sizeof(struct usrsockdev_s), id);

    if (usrsockdev) {
        usrsockdev->init_cfg = config;
        usrsockdev->ocount = 0;
        usrsockdev->req.nbusy = 0;
        aos_sem_new(&usrsockdev->devsem, 1);
        aos_sem_new(&usrsockdev->req.sem, 1);
        aos_sem_new(&usrsockdev->req.acksem, 0);
    }
    g_usrsockdev = usrsockdev;
    return (aos_dev_t *)usrsockdev;
}

static void usrsockdev_uninit(aos_dev_t *adev)
{
    device_free(adev);
}

static void usrsockdev_set_event(aos_dev_t *adev,
               void (*event)(aos_dev_t *adev, int event_id, void *priv),
               void *priv)
{
    struct usrsockdev_s *usrsockdev = (struct usrsockdev_s *)adev;

    usrsockdev->event = event;
    usrsockdev->priv = priv;
}
#endif

/****************************************************************************
 * Name: usrsockdev_register
 *
 * Description:
 *   Register /dev/usrsock
 *
 ****************************************************************************/

void usrsockdev_register(void)
{
    /* Initialize device private structure. */

#ifdef CONFIG_USRSOCK_NUM
    driver_register(&g_usrsock_driver.drv, NULL, 0);
#else
    struct usrsockdev_s *usrsockdev = aos_calloc(sizeof(struct usrsockdev_s), 1);
    if (!usrsockdev) {
        LOGE(USRSOCK_TAG, "Failed to allocate usrsockdev!\n");
        return;
    }
    usrsockdev->ocount = 0;
    usrsockdev->req.nbusy = 0;
    aos_sem_new(&usrsockdev->devsem, 1);
    aos_sem_new(&usrsockdev->req.sem, 1);
    aos_sem_new(&usrsockdev->req.acksem, 0);
    aos_register_driver("/dev/usrsock", &g_usrsockdevops, &usrsockdev);
    g_usrsockdev = usrsockdev;
#endif
}
