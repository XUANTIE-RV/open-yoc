/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     syslog_rpmsg_server.c
 * @brief    CSI Source File for syslog rpmsg server
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <aos/kernel.h>
#include <aos/debug.h>
#include <openamp.h>
#include "syslog_rpmsg.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SYSLOG_RPMSG_MAXLEN             256

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct syslog_rpmsg_server_s
{
    struct rpmsg_endpoint ept;
    char                  *tmpbuf;
    unsigned int          nextpos;
    unsigned int          alloced;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void syslog_rpmsg_write(const char *buf1, size_t len1,
                               const char *buf2, size_t len2);
static void syslog_rpmsg_ns_bind(struct rpmsg_device *rdev, void *priv_,
                                 const char *name, uint32_t dest);
static void syslog_rpmsg_ns_unbind(struct rpmsg_endpoint *ept);
static int  syslog_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                                size_t len, uint32_t src, void *priv_);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void syslog_write(const char *buffer, size_t buflen)
{
    size_t nwritten;

    for (nwritten = 0; nwritten < buflen; nwritten++) {
        extern int uart_putc(int ch);
        uart_putc(*buffer++);
    }
}

static void syslog_rpmsg_write(const char *buf1, size_t len1,
                               const char *buf2, size_t len2)
{
    const char *nl;
    size_t len;

    nl = memchr(buf2, '\n', len2);
    aos_assert(nl != NULL);
    len = nl + 1 - buf2;

    if (len1 + len <= SYSLOG_RPMSG_MAXLEN) {
        char tmpbuf[SYSLOG_RPMSG_MAXLEN];

        /* Ensure each syslog_write's buffer end with '\n' */

        memcpy(tmpbuf, buf1, len1);
        memcpy(tmpbuf + len1, buf2, len);
        syslog_write(tmpbuf, len1 + len);

        if (len < len2) {
            syslog_write(nl + 1, len2 - len);
        }
    } else {
        /* Give up, the merge buffer is too big */

        syslog_write(buf1, len1);
        syslog_write(buf2, len2);
    }
}

static void syslog_rpmsg_ns_bind(struct rpmsg_device *rdev, void *priv_,
                                 const char *name, uint32_t dest)
{
    struct syslog_rpmsg_server_s *priv;
    int ret;

    if (strcmp(name, SYSLOG_RPMSG_EPT_NAME)) {
        return;
    }

    priv = aos_zalloc(sizeof(struct syslog_rpmsg_server_s));
    if (!priv) {
        return;
    }

    priv->ept.priv = priv;

    ret = rpmsg_create_ept(&priv->ept, rdev, SYSLOG_RPMSG_EPT_NAME,
                           RPMSG_ADDR_ANY, dest,
                           syslog_rpmsg_ept_cb, syslog_rpmsg_ns_unbind);
    if (ret) {
        aos_free(priv);
    }
}

static void syslog_rpmsg_ns_unbind(struct rpmsg_endpoint *ept)
{
    struct syslog_rpmsg_server_s *priv = ept->priv;

    if (priv->nextpos) {
        syslog_rpmsg_write(priv->tmpbuf, priv->nextpos, "\n", 1);
    }

    rpmsg_destroy_ept(ept);

    aos_free(priv->tmpbuf);
    aos_free(priv);
}

static int syslog_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                               size_t len, uint32_t src, void *priv_)
{
    struct syslog_rpmsg_server_s *priv = priv_;
    struct syslog_rpmsg_header_s *header = data;

    if (header->command == SYSLOG_RPMSG_TRANSFER) {
        struct syslog_rpmsg_transfer_s *msg = data;
        struct syslog_rpmsg_header_s done;
        unsigned int copied = msg->count;
        unsigned int printed = 0;
        const char *nl;

        nl = memchr(msg->data, '\n', msg->count);
        if (nl != NULL) {
            printed = nl + 1 - msg->data;
            copied = msg->count - printed;

            if (priv->nextpos) {
                syslog_rpmsg_write(priv->tmpbuf, priv->nextpos, msg->data, printed);
                priv->nextpos = 0;
            } else {
                syslog_write(msg->data, printed);
            }
        }

        if (copied != 0) {
            unsigned int newsize = priv->nextpos + copied;
            if (newsize > priv->alloced) {
                char *newbuf = aos_realloc(priv->tmpbuf, newsize);
                if (newbuf != NULL) {
                    priv->tmpbuf  = newbuf;
                    priv->alloced = newsize;
                } else {
                    copied = priv->alloced - priv->nextpos;
                }
            }
            memcpy(priv->tmpbuf + priv->nextpos, msg->data + printed, copied);
            priv->nextpos += copied;
        }

        done.command = SYSLOG_RPMSG_TRANSFER_DONE;
        done.result  = printed + copied;
        rpmsg_send(ept, &done, sizeof(done));
    }

    return 0;
}

/****************************************************************************
 * Public Funtions
 ****************************************************************************/

int syslog_rpmsg_server_init(void)
{
  return rpmsg_register_callback(NULL,
                                 NULL,
                                 NULL,
                                 syslog_rpmsg_ns_bind);
}
