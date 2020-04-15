/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     song_rptun.c
 * @brief    CSI Source File for ck rptun Driver
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <soc.h>
#include <drv/mailbox.h>

#include <rptun.h>
#include <song_rptun.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

#define CONFIG_RPTUN_NUM            2
struct song_rptun_dev_s
{
  struct rptun_dev_s               rptun;
  const struct song_rptun_config_s *config;
  mailbox_handle_t                 mbox;
  rptun_callback_t                 callback;
  void                             *arg;
};

/************************************************************************************
 * Private Function Prototypes
 ************************************************************************************/

static const char *song_rptun_get_cpuname(struct rptun_dev_s *dev);
static const char *song_rptun_get_firmware(struct rptun_dev_s *dev);
static const struct rptun_addrenv_s *song_rptun_get_addrenv(struct rptun_dev_s *dev);
static struct rptun_rsc_s *song_rptun_get_resource(struct rptun_dev_s *dev);
static bool song_rptun_is_autostart(struct rptun_dev_s *dev);
static bool song_rptun_is_master(struct rptun_dev_s *dev);
static int song_rptun_config(struct rptun_dev_s *dev, void *data);
static int song_rptun_start(struct rptun_dev_s *dev);
static int song_rptun_stop(struct rptun_dev_s *dev);
static int song_rptun_notify(struct rptun_dev_s *dev, uint32_t vqid);
static int song_rptun_registercallback(struct rptun_dev_s *dev,
                                    rptun_callback_t callback, void *arg);
static int song_rptun_vring_isr(void *arg);

/************************************************************************************
 * Private Data
 ************************************************************************************/

static const struct rptun_ops_s g_song_rptun_ops =
{
    .get_cpuname       = song_rptun_get_cpuname,
    .get_firmware      = song_rptun_get_firmware,
    .get_addrenv       = song_rptun_get_addrenv,
    .get_resource      = song_rptun_get_resource,
    .is_autostart      = song_rptun_is_autostart,
    .is_master         = song_rptun_is_master,
    .config            = song_rptun_config,
    .start             = song_rptun_start,
    .stop              = song_rptun_stop,
    .notify            = song_rptun_notify,
    .register_callback = song_rptun_registercallback,
};

static struct song_rptun_dev_s g_song_rptun_dev[CONFIG_RPTUN_NUM];

/************************************************************************************
 * Private Functions
 ************************************************************************************/

static const char *song_rptun_get_cpuname(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return config->cpuname;
}

static const char *song_rptun_get_firmware(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return config->firmware;
}

static const struct rptun_addrenv_s *song_rptun_get_addrenv(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return config->addrenv;
}

static struct rptun_rsc_s *song_rptun_get_resource(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return config->rsc;
}

static bool song_rptun_is_autostart(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return !config->nautostart;
}

static bool song_rptun_is_master(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return config->master;
}

static int song_rptun_config(struct rptun_dev_s *dev, void *data)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    if (config->config) {
        return config->config(config, data);
    }

    return 0;
}

static int song_rptun_start(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    if (config->start) {
        return config->start(config);
    }

    return 0;
}

static int song_rptun_stop(struct rptun_dev_s *dev)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    if (config->stop) {
        return config->stop(config);
    }

    return 0;
}

static int song_rptun_notify(struct rptun_dev_s *dev, uint32_t vqid)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;
    const struct song_rptun_config_s *config = priv->config;

    return csi_mailbox_send(priv->mbox, config->mailbox, NULL, 0);
}

static int song_rptun_registercallback(struct rptun_dev_s *dev,
                                    rptun_callback_t callback, void *arg)
{
    struct song_rptun_dev_s *priv = (struct song_rptun_dev_s *)dev;

    priv->callback = callback;
    priv->arg      = arg;

    csi_mailbox_chnl_enable(priv->mbox, priv->config->mailbox);

    return 0;
}

static int song_rptun_vring_isr(void *arg)
{
    struct song_rptun_dev_s *priv = arg;

    if (priv->callback) {
        priv->callback(priv->arg, RPTUN_NOTIFY_ALL);
    }

    return 0;
}

static void song_rptun_mailbox_callback(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event)
{
    struct song_rptun_dev_s *priv;
    int i;

    for (i = 0; i < CONFIG_RPTUN_NUM; i++) {
        priv = &g_song_rptun_dev[i];

        if (mailbox_id == priv->config->mailbox) {
            song_rptun_vring_isr(priv);
            break;
        }
    }
}

/************************************************************************************
 * Public Functions
 ************************************************************************************/

void song_rptun_initialize(const struct song_rptun_config_s *config)
{
    struct song_rptun_dev_s *priv;
    mailbox_handle_t mailbox;
    int i, ret;

    mailbox = csi_mailbox_initialize(song_rptun_mailbox_callback);
    if (mailbox == NULL) {
        return;
    }

    for (i = 0; i < CONFIG_RPTUN_NUM; i++) {
        priv = &g_song_rptun_dev[i];

        priv->rptun.ops = &g_song_rptun_ops;
        priv->config    = config + i;
        priv->mbox      = mailbox;

        ret = rptun_initialize((struct rptun_dev_s *)priv);
        if (ret < 0) {
            return;
        }
    }
}