/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_rpmsg_client.h
 * @brief    user sock callback functions
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <unistd.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/types.h>
#include <openamp.h>

#include "usrsock_rpmsg.h"
#include <devices/usrsock.h>
#include <lwip/sockets.h>
/****************************************************************************
 * Private Types
 ****************************************************************************/

struct usrsock_rpmsg_s
{
    struct rpmsg_endpoint ept;
    const char           *cpuname;
    aos_event_t           event;
    aos_sem_t             sem;
    aos_sem_t             read_sem;
    aos_dev_t*            adev;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int usrsock_rpmsg_default_handler(struct rpmsg_endpoint *ept, void *data,
                                         size_t len, uint32_t src, void *priv_);

static void usrsock_rpmsg_device_created(struct rpmsg_device *rdev,
                                         void *priv_);
static void usrsock_rpmsg_device_destroy(struct rpmsg_device *rdev,
                                         void *priv_);
static int usrsock_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                                size_t len, uint32_t src, void *priv);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void usrsock_rpmsg_device_created(struct rpmsg_device *rdev,
                                         void *priv_)
{
    struct usrsock_rpmsg_s *priv = priv_;
    int ret;

    if (!strcmp(priv->cpuname, rpmsg_get_cpuname(rdev))) {
        priv->ept.priv = priv;

        ret = rpmsg_create_ept(&priv->ept, rdev, USRSOCK_RPMSG_EPT_NAME,
                RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
                usrsock_rpmsg_ept_cb, NULL);
        if (ret == 0)
        {
            aos_sem_signal(&priv->sem);
        }
    }
}

static void usrsock_rpmsg_device_destroy(struct rpmsg_device *rdev,
                                         void *priv_)
{
    struct usrsock_rpmsg_s *priv = priv_;

    if (!strcmp(priv->cpuname, rpmsg_get_cpuname(rdev))) {
        rpmsg_destroy_ept(&priv->ept);
    }
}

static int usrsock_rpmsg_default_handler(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv_)
{
    struct usrsock_rpmsg_s *priv = priv_;

    while (len > 0) {
        ssize_t ret = usrsock_write(priv->adev, data, len);
        if (ret < 0) {
            return ret;
        }
        data = (char*)data + ret;
        len  -= ret;
    }

    return 0;
}

static int usrsock_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                                size_t len, uint32_t src, void *priv)
{
    return usrsock_rpmsg_default_handler(ept, data, len, src, priv);
}

static void usrsock_rpmsg_read_event(aos_dev_t *dev, int event_id, void *priv_)
{
    struct usrsock_rpmsg_s *priv = priv_;

    if (event_id == POLLIN)
        aos_sem_signal(&priv->read_sem);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void usrsock_main(void *arg)
{
    struct usrsock_rpmsg_s priv = {};
    aos_dev_t *adev;
    int ret;

    priv.cpuname = (const char *)arg;

    aos_sem_new(&priv.sem, 0);
    aos_sem_new(&priv.read_sem, 0);

    ret = rpmsg_register_callback(&priv,
            usrsock_rpmsg_device_created,
            usrsock_rpmsg_device_destroy,
            NULL);
    if (ret < 0) {
        goto destroy_sem;
    }

    while (1) {
        /* Wait until the rpmsg channel is ready */
        do {
            ret = aos_sem_wait(&priv.sem, AOS_WAIT_FOREVER);
            if (ret < 0) {
                ret = -errno;
            }
        }
        while (ret == -EINTR);

        if (ret < 0) {
            goto unregister_callback;
        }

        /* Open the kernel channel */
        adev = usrsock_open("usrsock0");
        if (!adev) {
            ret = -errno;
            goto destroy_ept;
        }
        priv.adev = adev;

        usrsock_set_event(adev, usrsock_rpmsg_read_event, &priv);

        /* Forward the packet from kernel to remote */
        while (1) {
            void  *buf;
            size_t len;

            aos_sem_wait(&priv.read_sem, AOS_WAIT_FOREVER);

            /* Read the packet from kernel */
            buf = rpmsg_get_tx_payload_buffer(&priv.ept, &len, true);
            if (!buf) {
                ret = -ENOMEM;
                break;
            }

            ret = usrsock_read(priv.adev, buf, len);
            if (ret < 0) {
                break;
            }

            /* Send the packet to remote */
            ret = rpmsg_send_nocopy(&priv.ept, buf, ret);
            if (ret < 0) {
                break;
            }
        }

        /* Reclaim the resource */
        usrsock_close(priv.adev);

        if (is_rpmsg_ept_ready(&priv.ept)) {
            goto destroy_ept;
        }

        /* The remote side crash, loop to wait it restore */
    }

destroy_ept:
    rpmsg_destroy_ept(&priv.ept);

unregister_callback:
    rpmsg_unregister_callback(&priv,
            usrsock_rpmsg_device_created,
            usrsock_rpmsg_device_destroy,
            NULL);
destroy_sem:
    aos_sem_free(&priv.read_sem);
    aos_sem_free(&priv.sem);
    return;
}
