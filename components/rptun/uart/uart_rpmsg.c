/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     uart_rpmsg.c
 * @brief    uart rpmsg Driver
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>
#include <vfs.h>
#include <vfs_register.h>
#include <vfs_inode.h>
#include <openamp.h>
#include <tioctl.h>
#include "serial.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define UART_RPMSG_DEV_CONSOLE          "/dev/console"
#define UART_RPMSG_DEV_PREFIX           "/dev/tty"
#define UART_RPMSG_EPT_PREFIX           "rpmsg-tty"

#define UART_RPMSG_TTY_WRITE            0
#define UART_RPMSG_TTY_WAKEUP           1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct uart_rpmsg_header_s
{
    uint32_t command : 31;
    uint32_t response : 1;
    int32_t  result;
    uint64_t cookie;
} __attribute__((__packed__));

struct uart_rpmsg_write_s
{
    struct uart_rpmsg_header_s header;
    uint32_t                   count;
    uint32_t                   resolved;
    char                       data[0];
} __attribute__((__packed__));

struct uart_rpmsg_wakeup_s
{
    struct uart_rpmsg_header_s header;
} __attribute__((__packed__));

struct uart_rpmsg_priv_s
{
    struct rpmsg_endpoint ept;
    const char            *devname;
    const char            *cpuname;
    void                  *recv_data;
    bool                  last_upper;
#ifdef CONFIG_SERIAL_TERMIOS
    struct termios        termios;
#endif
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

#ifndef CONFIG_SERIAL_USE_VFS
static int  uart_rpmsg_early_init(struct serial_dev_s *dev);
#endif
static int  uart_rpmsg_setup(struct serial_dev_s *dev);
static void uart_rpmsg_shutdown(struct serial_dev_s *dev);
static int  uart_rpmsg_attach(struct serial_dev_s *dev);
static void uart_rpmsg_detach(struct serial_dev_s *dev);
static int  uart_rpmsg_ioctl(file_t *filep, int cmd, unsigned long arg);
static void uart_rpmsg_rxint(struct serial_dev_s *dev, bool enable);
static bool uart_rpmsg_rxflowcontrol(struct serial_dev_s *dev,
                    unsigned int nbuffered, bool upper);
static void uart_rpmsg_dmasend(struct serial_dev_s *dev);
static void uart_rpmsg_dmareceive(struct serial_dev_s *dev);
static void uart_rpmsg_dmarxfree(struct serial_dev_s *dev);
static void uart_rpmsg_dmatxavail(struct serial_dev_s *dev);
static void uart_rpmsg_send(struct serial_dev_s *dev, int ch);
static void uart_rpmsg_txint(struct serial_dev_s *dev, bool enable);
static bool uart_rpmsg_txready(struct serial_dev_s *dev);
static bool uart_rpmsg_txempty(struct serial_dev_s *dev);
static void uart_rpmsg_device_created(struct rpmsg_device *rdev, void *priv_);
static void uart_rpmsg_device_destroy(struct rpmsg_device *rdev, void *priv_);
static int  uart_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                              size_t len, uint32_t src, void *priv_);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct serial_ops_s g_uart_rpmsg_ops =
{
#ifndef CONFIG_SERIAL_USE_VFS
    .earlyinit     = uart_rpmsg_early_init,
#endif
    .setup         = uart_rpmsg_setup,
    .shutdown      = uart_rpmsg_shutdown,
    .attach        = uart_rpmsg_attach,
    .detach        = uart_rpmsg_detach,
    .ioctl         = uart_rpmsg_ioctl,
    .rxint         = uart_rpmsg_rxint,
#ifdef CONFIG_SERIAL_IFLOWCONTROL
    .rxflowcontrol = uart_rpmsg_rxflowcontrol,
#endif
#ifdef CONFIG_SERIAL_DMA
    .dmasend       = uart_rpmsg_dmasend,
    .dmareceive    = uart_rpmsg_dmareceive,
    .dmarxfree     = uart_rpmsg_dmarxfree,
    .dmatxavail    = uart_rpmsg_dmatxavail,
#endif
    .send          = uart_rpmsg_send,
    .txint         = uart_rpmsg_txint,
    .txready       = uart_rpmsg_txready,
    .txempty       = uart_rpmsg_txempty,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifndef CONFIG_SERIAL_USE_VFS
static int uart_rpmsg_early_init(struct serial_dev_s *dev)
{
    return rpmsg_register_callback((void*)dev,
                                   uart_rpmsg_device_created,
                                   uart_rpmsg_device_destroy,
                                   NULL);
}
#endif

static int uart_rpmsg_setup(struct serial_dev_s *dev)
{
    return OK;
}

static void uart_rpmsg_shutdown(struct serial_dev_s *dev)
{
}

static int uart_rpmsg_attach(struct serial_dev_s *dev)
{
    return OK;
}

static void uart_rpmsg_detach(struct serial_dev_s *dev)
{
}

static int uart_rpmsg_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    int ret = -ENOTTY;

#ifdef CONFIG_SERIAL_TERMIOS
    struct serial_dev_s *dev = filep->node->i_arg;
    struct uart_rpmsg_priv_s *priv = dev->priv;

    switch (cmd) {
        case TCGETS: {
            struct termios *termiosp = (struct termios *)arg;

            if (termiosp) {
                *termiosp = priv->termios;
                ret = OK;
            } else {
                ret = -EINVAL;
            }
        }
        break;

        case TCSETS: {
            struct termios *termiosp = (struct termios *)arg;

            if (termiosp) {
                priv->termios = *termiosp;
                ret = OK;
            } else {
                ret = -EINVAL;
            }
        }
        break;
    }
#endif

    return ret;
}

static void uart_rpmsg_rxint(struct serial_dev_s *dev, bool enable)
{
}

static bool uart_rpmsg_rxflowcontrol(struct serial_dev_s *dev,
                unsigned int nbuffered, bool upper)
{
    struct uart_rpmsg_priv_s *priv = dev->priv;
    struct uart_rpmsg_wakeup_s msg;

    if (!upper && upper != priv->last_upper) {
        memset(&msg, 0, sizeof(msg));

        msg.header.command = UART_RPMSG_TTY_WAKEUP;
        if (is_rpmsg_ept_ready(&priv->ept)) {
            rpmsg_send(&priv->ept, &msg, sizeof(msg));
        }
    }

    priv->last_upper = upper;
    return false;
}

#ifdef CONFIG_SERIAL_DMA
static void uart_rpmsg_dmasend(struct serial_dev_s *dev)
{
    struct uart_rpmsg_priv_s *priv = dev->priv;
    struct serial_dmaxfer_s *xfer = &dev->dmatx;
    struct uart_rpmsg_write_s *msg;
    size_t len = xfer->length + xfer->nlength;
    uint32_t space;

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        dev->dmatx.length = 0;
        return;
    }

    memset(msg, 0, sizeof(*msg));

    space = C2B(space - sizeof(*msg));

    if (len > space) {
        len = space;
    }

    if (len > xfer->length) {
        memcpy(msg->data, xfer->buffer, xfer->length);
        memcpy(msg->data + B2C_OFF(xfer->length) + B2C_REM(xfer->length),
               xfer->nbuffer, len - xfer->length);
    } else {
        memcpy(msg->data, xfer->buffer, len);
    }

    msg->count          = len;
    msg->header.command = UART_RPMSG_TTY_WRITE;
    msg->header.result  = -ENXIO;
    msg->header.cookie  = (uintptr_t)dev;

    rpmsg_send_nocopy(&priv->ept, msg, sizeof(*msg) + B2C(len));
}

static void uart_rpmsg_dmareceive(struct serial_dev_s *dev)
{
    struct uart_rpmsg_priv_s *priv = dev->priv;
    struct serial_dmaxfer_s *xfer = &dev->dmarx;
    struct uart_rpmsg_write_s *msg = priv->recv_data;
    uint32_t len = msg->count;
    size_t space = xfer->length + xfer->nlength;

    if (len > space) {
        len = space;
    }

    if (len > xfer->length) {
        memcpy(xfer->buffer, msg->data, xfer->length);
        memcpy(xfer->nbuffer,
               msg->data + B2C_OFF(xfer->length) + B2C_REM(xfer->length),
               len - xfer->length);
    } else {
        memcpy(xfer->buffer, msg->data, len);
    }

    xfer->nbytes = len;
    serial_recvchars_done(dev);

    msg->header.result = len;

    if (len != msg->count) {
        uart_rpmsg_rxflowcontrol(dev, 0, true);
    }
}

static void uart_rpmsg_dmarxfree(struct serial_dev_s *dev)
{
    struct uart_rpmsg_priv_s *priv = dev->priv;

    if (is_rpmsg_ept_ready(&priv->ept)) {
        struct uart_rpmsg_wakeup_s msg = {
            .header = {
                .command = UART_RPMSG_TTY_WAKEUP,
            },
        };

        rpmsg_send(&priv->ept, &msg, sizeof(msg));
    }
}

static void uart_rpmsg_dmatxavail(struct serial_dev_s *dev)
{
    struct uart_rpmsg_priv_s *priv = dev->priv;

    if (is_rpmsg_ept_ready(&priv->ept) && dev->dmatx.length == 0) {
        serial_xmitchars_dma(dev);
    }
}
#endif

static void uart_rpmsg_send(struct serial_dev_s *dev, int ch)
{
    int nexthead;

    nexthead = dev->xmit.head + 1;
    if (nexthead >= dev->xmit.size) {
        nexthead = 0;
    }

    if (nexthead != dev->xmit.tail) {
        /* No.. not full.  Add the character to the TX buffer and return. */

        dev->xmit.buffer[dev->xmit.head] = ch;
        dev->xmit.head = nexthead;
    }

    uart_rpmsg_dmatxavail(dev);
}

static void uart_rpmsg_txint(struct serial_dev_s *dev, bool enable)
{
}

static bool uart_rpmsg_txready(struct serial_dev_s *dev)
{
    int nexthead;

    nexthead = dev->xmit.head + 1;
    if (nexthead >= dev->xmit.size) {
        nexthead = 0;
    }

    return nexthead != dev->xmit.tail;
}

static bool uart_rpmsg_txempty(struct serial_dev_s *dev)
{
    return true;
}

static void uart_rpmsg_device_created(struct rpmsg_device *rdev, void *priv_)
{
    struct serial_dev_s *dev = priv_;
    struct uart_rpmsg_priv_s *priv = dev->priv;
    char eptname[RPMSG_NAME_SIZE];

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        priv->ept.priv = dev;
        sprintf(eptname, "%s%s", UART_RPMSG_EPT_PREFIX, priv->devname);
        rpmsg_create_ept(&priv->ept, rdev, eptname,
                         RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
                         uart_rpmsg_ept_cb, NULL);
    }
}

static void uart_rpmsg_device_destroy(struct rpmsg_device *rdev, void *priv_)
{
    struct serial_dev_s *dev = priv_;
    struct uart_rpmsg_priv_s *priv = dev->priv;

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        rpmsg_destroy_ept(&priv->ept);
    }
}

static int uart_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                             size_t len, uint32_t src, void *priv_)
{
    struct serial_dev_s *dev = priv_;
    struct uart_rpmsg_header_s *header = data;
    struct uart_rpmsg_write_s *msg = data;

    if (header->response) {
        /* Get write-cmd reponse, this tell how many data have sent */

        dev->dmatx.nbytes = header->result;
        if (header->result < 0) {
            dev->dmatx.nbytes = 0;
        }

        serial_xmitchars_done(dev);

        /* If have sent some data succeed, then continue send */

        if (msg->count == header->result) {
            uart_rpmsg_dmatxavail(dev);
        }
    } else if (header->command == UART_RPMSG_TTY_WRITE) {
        struct uart_rpmsg_priv_s *priv = dev->priv;

        /* Get write-cmd, there are some data, we need receive them */

        priv->recv_data = data;
        serial_recvchars_dma(dev);
        priv->recv_data = NULL;

        header->response = 1;
        rpmsg_send(ept, msg, sizeof(*msg));
    } else if (header->command == UART_RPMSG_TTY_WAKEUP) {
        /* Remote core have space, then wakeup current core, continue send */

        uart_rpmsg_dmatxavail(dev);
    }

    return 0;
}

/****************************************************************************
 * Public Funtions
 ****************************************************************************/

#ifdef CONFIG_SERIAL_USE_VFS
int uart_rpmsg_init(const char *cpuname, const char *devname,
                    int buf_size, bool isconsole)
{
    struct uart_rpmsg_priv_s *priv;
    struct serial_dev_s *dev;
    char dev_name[32];
    int ret = -ENOMEM;

    dev = aos_zalloc(sizeof(struct serial_dev_s));
    if (!dev) {
        return ret;
    }

    dev->ops       = &g_uart_rpmsg_ops;
    dev->isconsole = isconsole;
    dev->recv.size = buf_size;
    dev->xmit.size = buf_size;

    dev->recv.buffer = aos_malloc(dev->recv.size);
    if (!dev->recv.buffer) {
        goto fail;
    }

    dev->xmit.buffer = aos_malloc(dev->xmit.size);
    if (!dev->xmit.buffer) {
        goto fail;
    }

    priv = aos_zalloc(sizeof(struct uart_rpmsg_priv_s));
    if (!priv) {
        goto fail;
    }

    priv->cpuname = cpuname;
    priv->devname = devname;

    dev->priv = priv;

    ret = rpmsg_register_callback(dev,
                                  uart_rpmsg_device_created,
                                  uart_rpmsg_device_destroy,
                                  NULL);
    if (ret < 0) {
        goto fail;
    }

    sprintf(dev_name, "%s%s", UART_RPMSG_DEV_PREFIX, devname);
    serial_register(dev_name, dev);

    if (dev->isconsole) {
        serial_register(UART_RPMSG_DEV_CONSOLE, dev);
    }

    return OK;

fail:
    aos_free(dev->recv.buffer);
    aos_free(dev->xmit.buffer);
    aos_free(dev->priv);
    aos_free(dev);

    return ret;
}
#else
int uart_rpmsg_init(const char *cpuname, const char *devname,
                    int uart_idx, int buf_size, bool isconsole)
{
    struct uart_rpmsg_priv_s *priv;
    struct serial_dev_config_s config;
    char dev_name[32];
    int ret = -ENOMEM;

    priv = aos_zalloc(sizeof(struct uart_rpmsg_priv_s));
    if (!priv) {
        return ret;
    }

    priv->cpuname = cpuname;
    priv->devname = devname;

    sprintf(dev_name, "%s%s", UART_RPMSG_DEV_PREFIX, devname);

    config.path = dev_name;
    config.ops = &g_uart_rpmsg_ops;
    config.priv = priv;
    config.buf_size = buf_size;
    config.uart_idx = uart_idx;
    config.isconsole = isconsole;
    serial_register(&config);

    if (isconsole) {
        config.path = UART_RPMSG_DEV_CONSOLE;
        serial_register(&config);
    }

    return OK;
}
#endif

