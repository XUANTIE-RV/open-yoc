/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     misc_rpmsg.c
 * @brief    CSI Source File for misc rpmsg
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <addrenv.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <enviro.h>
#include <vfs.h>
#include <vfs_register.h>
#include <vfs_inode.h>
#include <openamp.h>
#include <rptun.h>
#include <metal/utilities.h>
#include <misc_rpmsg.h>

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define TAG "misc_rpmsg"

#define MISC_RPMSG_MAX_PRIV             2

#define MISC_RPMSG_EPT_NAME             "rpmsg-misc"

#define MISC_RPMSG_RETENT_ADD           0
#define MISC_RPMSG_RETENT_SET           1
#define MISC_RPMSG_REMOTE_BOOT          2
#define MISC_RPMSG_REMOTE_CLOCKSYNC     3
#define MISC_RPMSG_REMOTE_ENVSYNC       4
#define MISC_RPMSG_REMOTE_INFOWRITE     5
#define MISC_RPMSG_REMOTE_RAMFLUSH      6

#define MISC_RETENT_MAGIC               (0xdeadbeef)

#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x)                 (sizeof(x) / sizeof((x)[0]))
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct misc_rpmsg_head_s
{
    uint32_t command;
} __attribute__((__packed__));

struct misc_rpmsg_retent_add_s
{
    uint32_t command;
    uint32_t blkid;
    uintptr_t base;
    uint32_t size;
    uint32_t dma;
} __attribute__((__packed__));

struct misc_rpmsg_retent_set_s
{
    uint32_t command;
    uint32_t blkid;
    uint32_t flush;
} __attribute__((__packed__));

struct misc_rpmsg_remote_boot_s
{
    uint32_t command;
    char     name[16];
} __attribute__((__packed__));

#define misc_rpmsg_remote_clocksync_s misc_rpmsg_head_s

struct misc_rpmsg_remote_envsync_s
{
    uint32_t command;
    uint32_t response;
    uint32_t result;
    char     name[16];
    char     value[32];
} __attribute__((__packed__));

struct misc_rpmsg_remote_infowrite_s
{
    uint32_t command;
    char     name[16];
    uint8_t  value[32];
    uint32_t len;
} __attribute__((__packed__));

struct misc_rpmsg_remote_ramflush_s
{
    uint32_t command;
    char     fpath[64];
} __attribute__((__packed__));

struct misc_rpmsg_s
{
    struct misc_dev_s     dev;
    struct rpmsg_endpoint ept;
    struct metal_list     blks;
    aos_work_t            worker;
    misc_ramflush_cb_t    ramflush_cb;
    char                  fpath[64];
    const char            *cpuname;
    bool                  server;
};

struct misc_retent_blk_s
{
    uint32_t magic;
    uintptr_t base;
    uint32_t size;
    uint32_t dma;
    uint32_t crc;
    uint32_t flush;
    uint32_t blkid;
    struct metal_list node;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void misc_rpmsg_device_created(struct rpmsg_device *rdev, void *priv_);
static void misc_rpmsg_device_destroy(struct rpmsg_device *rdev, void *priv_);
static int misc_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                             size_t len, uint32_t src, void *priv_);

static int misc_retent_add_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_);
static int misc_retent_set_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_);
static int misc_remote_boot_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_);
static int misc_remote_clocksync_handler(struct rpmsg_endpoint *ept,
                                         void *data, size_t len,
                                         uint32_t src, void *priv_);
static int misc_remote_envsync_handler(struct rpmsg_endpoint *ept,
                                       void *data, size_t len,
                                       uint32_t src, void *priv_);
static int misc_remote_ramflush_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv_);

static void misc_ramflush_work(void *arg);
static int misc_ramflush_register(struct misc_dev_s *dev,
                                  misc_ramflush_cb_t cb);

static int misc_retent_add(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_retent_set(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_remote_boot(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_remote_clocksync(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_remote_envsync(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_remote_infowrite(struct misc_rpmsg_s *priv, unsigned long arg);
static int misc_dev_ioctl(file_t *filep, int cmd, unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const rpmsg_ept_cb g_misc_rpmsg_handler[] =
{
    [MISC_RPMSG_RETENT_ADD]       = misc_retent_add_handler,
    [MISC_RPMSG_RETENT_SET]       = misc_retent_set_handler,
    [MISC_RPMSG_REMOTE_BOOT]      = misc_remote_boot_handler,
    [MISC_RPMSG_REMOTE_CLOCKSYNC] = misc_remote_clocksync_handler,
    [MISC_RPMSG_REMOTE_ENVSYNC]   = misc_remote_envsync_handler,
    [MISC_RPMSG_REMOTE_RAMFLUSH]  = misc_remote_ramflush_handler,
};

static const struct misc_ops_s g_misc_ops =
{
    .ramflush_register = misc_ramflush_register,
};

static const file_ops_t g_misc_devops =
{
    .ioctl = misc_dev_ioctl,
};

static struct misc_rpmsg_s *g_misc_priv[MISC_RPMSG_MAX_PRIV];
static int g_misc_idx;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void misc_rpmsg_device_created(struct rpmsg_device *rdev, void *priv_)
{
    struct misc_rpmsg_s *priv = priv_;

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        priv->ept.priv = priv;

        rpmsg_create_ept(&priv->ept, rdev, MISC_RPMSG_EPT_NAME,
                         RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
                         misc_rpmsg_ept_cb, NULL);

        if (priv->server) {
            /* Server */

            if (g_misc_idx >= MISC_RPMSG_MAX_PRIV) {
                return;
            }

            g_misc_priv[g_misc_idx++] = priv;
        }
    }
}

static void misc_rpmsg_device_destroy(struct rpmsg_device *rdev, void *priv_)
{
    struct misc_rpmsg_s *priv = priv_;

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        rpmsg_destroy_ept(&priv->ept);
    }
}

static int misc_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                             size_t len, uint32_t src, void *priv)
{
    struct misc_rpmsg_head_s *head = data;
    uint32_t command = head->command;

    if (command < ARRAY_SIZE(g_misc_rpmsg_handler)) {
        return g_misc_rpmsg_handler[command](ept, data, len, src, priv);
    }

    return -EINVAL;
}

static int misc_retent_add_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_)
{
    struct misc_rpmsg_retent_add_s *msg = data;
    struct misc_rpmsg_s *priv = priv_;
    struct misc_retent_blk_s *blk;

    blk = aos_zalloc(sizeof(struct misc_retent_blk_s));
    if (!blk) {
        return -ENOMEM;
    }

    blk->blkid = msg->blkid;
    blk->base  = (uintptr_t)up_addrenv_pa_to_va(msg->base);
    blk->size  = msg->size;
    blk->dma   = msg->dma;
    blk->flush = true;

    metal_list_add_tail(&priv->blks, &blk->node);
    return 0;
}

static int misc_retent_set_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_)
{
    struct misc_rpmsg_retent_set_s *msg = data;
    struct misc_rpmsg_s *priv = priv_;
    struct metal_list *node;

    metal_list_for_each(&priv->blks, node) {
        struct misc_retent_blk_s *blk;

        blk = metal_container_of(node, struct misc_retent_blk_s, node);
        if (blk->blkid == msg->blkid) {
            blk->flush = msg->flush;
        }
    }

    return 0;
}

static int misc_remote_boot_handler(struct rpmsg_endpoint *ept,
                                   void *data, size_t len,
                                   uint32_t src, void *priv_)
{
    struct misc_rpmsg_remote_boot_s *msg = data;

    if (rptun_boot(msg->name)) {
        LOGE(TAG, "Boot core err, name %s", msg->name);
    }

    return 0;
}

static int misc_remote_clocksync_(const char *cpuname)
{
    int i;
    struct misc_rpmsg_remote_clocksync_s msg =
    {
        .command = MISC_RPMSG_REMOTE_CLOCKSYNC,
    };

    for (i = 0; i < g_misc_idx; i++) {
        if (!cpuname || strcmp(cpuname, g_misc_priv[i]->cpuname)) {
            rpmsg_send(&g_misc_priv[i]->ept, &msg, sizeof(msg));
        }
    }

    return 0;
}

static int misc_remote_clocksync_handler(struct rpmsg_endpoint *ept,
                                         void *data, size_t len,
                                         uint32_t src, void *priv_)
{
#ifdef CONFIG_RTC
    struct misc_rpmsg_s *priv = priv_;
    misc_remote_clocksync_(priv->cpuname);
#endif
    return 0;
}

static int misc_remote_envsync_handler(struct rpmsg_endpoint *ept,
                                       void *data, size_t len,
                                       uint32_t src, void *priv_)
{
    struct misc_rpmsg_remote_envsync_s *msg = data;

    if (msg->response) {
        if (!msg->result) {
            setenv(msg->name, msg->value, 1);
        }
        return 0;
    } else {
        msg->response = 1;

        char *value = getenv(msg->name);
        if (!value) {
            msg->result = -EINVAL;
        } else {
            strncpy(msg->value, value, 32);
        }
        return rpmsg_send(ept, msg, sizeof(*msg));
    }
}

static int misc_remote_ramflush_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv_)
{
    struct misc_rpmsg_remote_ramflush_s *msg = data;
    struct misc_rpmsg_s *priv = priv_;

    memcpy(priv->fpath, msg->fpath, sizeof(priv->fpath));
    aos_work_init(&priv->worker, misc_ramflush_work, priv, 0);
    aos_work_sched(&priv->worker);

    return 0;
}

static void misc_ramflush_work(void *arg)
{
    struct misc_rpmsg_s *priv = arg;

    if (priv->ramflush_cb) {
        priv->ramflush_cb(priv->fpath);
    }

    aos_work_destroy(&priv->worker);
}

static int misc_ramflush_register(struct misc_dev_s *dev,
                                  misc_ramflush_cb_t cb)
{
    struct misc_rpmsg_s *priv = (struct misc_rpmsg_s *)dev;

    priv->ramflush_cb = cb;

    return 0;
}

static int misc_retent_add(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_retent_add_s *add = (struct misc_retent_add_s *)arg;
    struct misc_rpmsg_retent_add_s msg;

    msg.command = MISC_RPMSG_RETENT_ADD;
    msg.blkid   = add->blkid;
    msg.base    = (uintptr_t)up_addrenv_va_to_pa(add->base);
    msg.size    = add->size;
    msg.dma     = add->dma;

    return rpmsg_send(&priv->ept, &msg, sizeof(struct misc_rpmsg_retent_add_s));
}

static int misc_retent_set(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_retent_set_s *set = (struct misc_retent_set_s *)arg;
    struct misc_rpmsg_retent_set_s msg;

    msg.command = MISC_RPMSG_RETENT_SET;
    msg.blkid   = set->blkid;
    msg.flush   = set->flush;

    return rpmsg_send(&priv->ept, &msg, sizeof(struct misc_rpmsg_retent_set_s));
}

static int misc_remote_boot(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_remote_boot_s *remote = (struct misc_remote_boot_s *)arg;
    struct misc_rpmsg_remote_boot_s msg;

    msg.command = MISC_RPMSG_REMOTE_BOOT;
    strncpy(msg.name, remote->name, 16);

    return rpmsg_send(&priv->ept, &msg, sizeof(struct misc_rpmsg_remote_boot_s));
}

static int misc_remote_clocksync(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_rpmsg_remote_clocksync_s msg =
    {
        .command = MISC_RPMSG_REMOTE_CLOCKSYNC,
    };

    return rpmsg_send(&priv->ept, &msg, sizeof(msg));
}

static int misc_remote_envsync(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_remote_envsync_s *env = (struct misc_remote_envsync_s *)arg;
    struct misc_rpmsg_remote_envsync_s msg;

    msg.command  = MISC_RPMSG_REMOTE_ENVSYNC;
    msg.response = 0;
    msg.result   = 0;
    strncpy(msg.name, env->name, 16);

    return rpmsg_send(&priv->ept, &msg, sizeof(msg));
}

static int misc_remote_infowrite(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_remote_infowrite_s *env = (struct misc_remote_infowrite_s *)arg;
    struct misc_rpmsg_remote_infowrite_s msg;
    uint32_t len;

    msg.command = MISC_RPMSG_REMOTE_INFOWRITE;
    strncpy(msg.name, env->name, 16);

    len = env->len > sizeof(msg.value) ? sizeof(msg.value) : env->len;
    memcpy(msg.value, env->value, len);
    msg.len = len;

    return rpmsg_send(&priv->ept, &msg, sizeof(msg));
}

static int misc_remote_ramflush(struct misc_rpmsg_s *priv, unsigned long arg)
{
    struct misc_remote_ramflush_s *flush = (struct misc_remote_ramflush_s *)arg;
    struct misc_rpmsg_remote_ramflush_s msg;

    msg.command = MISC_RPMSG_REMOTE_RAMFLUSH;
    strncpy(msg.fpath, flush->fpath, 64);

    return rpmsg_send(&priv->ept, &msg, sizeof(msg));
}

static int misc_dev_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    inode_t *inode = filep->node;
    struct misc_rpmsg_s *priv = inode->i_arg;
    int ret = -ENOTTY;

    switch (cmd) {
        case MISC_RETENT_ADD:
            ret = misc_retent_add(priv, arg);
            break;
        case MISC_RETENT_SET:
            ret = misc_retent_set(priv, arg);
            break;
        case MISC_REMOTE_CLOCKSYNC:
            ret = misc_remote_clocksync(priv, arg);
            break;
        case MISC_REMOTE_BOOT:
            ret = misc_remote_boot(priv, arg);
            break;
        case MISC_REMOTE_ENVSYNC:
            ret = misc_remote_envsync(priv, arg);
            break;
        case MISC_REMOTE_INFOWRITE:
            ret = misc_remote_infowrite(priv, arg);
            break;
        case MISC_REMOTE_RAMFLUSH:
            ret = misc_remote_ramflush(priv, arg);
            break;
    }

    return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

struct misc_dev_s *misc_rpmsg_initialize(const char *cpuname,
                                         bool devctl)
{
    struct misc_rpmsg_s *priv;
    int ret;

    priv = aos_zalloc(sizeof(struct misc_rpmsg_s));
    if (!priv) {
        return NULL;
    }

    metal_list_init(&priv->blks);

    priv->cpuname = cpuname;
    priv->dev.ops = &g_misc_ops;
    priv->server  = !devctl;

    ret = rpmsg_register_callback(priv,
                                  misc_rpmsg_device_created,
                                  misc_rpmsg_device_destroy,
                                  NULL);
    if (ret) {
        aos_free(priv);
        return NULL;
    }

    if (devctl) {
        /* Client */

        aos_register_driver("/dev/misc", &g_misc_devops, priv);
    }

    return &priv->dev;
}

int misc_rpmsg_clocksync(void)
{
    int fd;
    int ret;

    fd = aos_open("/dev/misc", O_RDONLY);
    if (fd < 0) {
        /* Server */

        return misc_remote_clocksync_(NULL);
    }

    /* Client */

    ret = aos_ioctl(fd, MISC_REMOTE_CLOCKSYNC, 0);
    aos_close(fd);

    return ret;
}
