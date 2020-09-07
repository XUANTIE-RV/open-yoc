/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     rptun.c
 * @brief    CSI Source File for rptun Driver
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <soc.h>
#include <aos/kernel.h>
#include <vfs.h>
#include <vfs_register.h>
#include <vfs_inode.h>
#include <metal/utilities.h>
#include <openamp.h>
#include <rptun.h>

#ifndef MAX
#  define MAX(a,b)              ((a) > (b) ? (a) : (b))
#endif

#ifndef ALIGN_UP
#  define ALIGN_UP(s, a)        (((s) + (a) - 1) & ~((a) - 1))
#endif

#define RPTUN_EVENT             0x00000001
#define RPTUN_NAME_SIZE         16

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct rptun_priv_s
{
    char                         name[RPTUN_NAME_SIZE];
    struct rptun_dev_s           *dev;
    struct remoteproc            rproc;
    struct rpmsg_virtio_device   vdev;
    struct rpmsg_virtio_shm_pool shm_pool;
    struct metal_list            bind;
    struct metal_list            node;
    aos_task_t                   handle;
    aos_event_t                  event;
    bool                         started;
};

struct rptun_bind_s
{
    char              name[RPMSG_NAME_SIZE];
    uint32_t          dest;
    struct metal_list node;
};

struct rptun_cb_s
{
    void              *priv;
    rpmsg_dev_cb_t    device_created;
    rpmsg_dev_cb_t    device_destroy;
    rpmsg_bind_cb_t   ns_bind;
    struct metal_list node;
};

struct rptun_store_s
{
    int  fd;
    char *buf;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static struct remoteproc *rptun_init(struct remoteproc *rproc,
                                     struct remoteproc_ops *ops,
                                     void *arg);
static void rptun_remove(struct remoteproc *rproc);
static int rptun_mmap(struct remoteproc *rproc,
                      metal_phys_addr_t *pa, metal_phys_addr_t *da,
                      void **va, size_t size, unsigned int attribute,
                      struct metal_io_region **io_);
static int rptun_config(struct remoteproc *rproc, void *data);
static int rptun_start(struct remoteproc *rproc);
static int rptun_stop(struct remoteproc *rproc);
static int rptun_notify(struct remoteproc *rproc, uint32_t id);

static void rptun_ns_bind(struct rpmsg_device *rdev,
                          const char *name, uint32_t dest);

static int rptun_dev_start(struct remoteproc *rproc);
static int rptun_dev_stop(struct remoteproc *rproc);
static int rptun_dev_ioctl(file_t *filep, int cmd, unsigned long arg);

static int rptun_store_open(void *store_, const char *path,
                            const void **img_data);
static void rptun_store_close(void *store_);
static int rptun_store_load(void *store_, size_t offset,
                            size_t size, const void **data,
                            metal_phys_addr_t pa,
                            struct metal_io_region *io,
                            char is_blocking);

static metal_phys_addr_t rptun_pa_to_da(struct rptun_dev_s *dev, metal_phys_addr_t pa);
static metal_phys_addr_t rptun_da_to_pa(struct rptun_dev_s *dev, metal_phys_addr_t da);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct remoteproc_ops g_rptun_ops =
{
    .init   = rptun_init,
    .remove = rptun_remove,
    .mmap   = rptun_mmap,
    .config = rptun_config,
    .start  = rptun_start,
    .stop   = rptun_stop,
    .notify = rptun_notify,
};

static const file_ops_t g_rptun_devops =
{
    .ioctl = rptun_dev_ioctl,
};

static struct image_store_ops g_rptun_storeops =
{
    .open     = rptun_store_open,
    .close    = rptun_store_close,
    .load     = rptun_store_load,
    .features = SUPPORT_SEEK,
};

static aos_mutex_t g_rptun_sem;

static METAL_DECLARE_LIST(g_rptun_cb);
static METAL_DECLARE_LIST(g_rptun_priv);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void rptun_thread(void *arg)
{
    struct rptun_priv_s *priv;
    uint32_t flag;
    int ret;

    priv = (struct rptun_priv_s *)arg;

    while(1) {
        ret = aos_event_get(&priv->event, RPTUN_EVENT,
                              AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        if (ret == 0) {
            remoteproc_get_notification(&priv->rproc, RPTUN_NOTIFY_ALL);
        }
    }
}

static int rptun_callback(void *arg, uint32_t vqid)
{
    struct rptun_priv_s *priv;

    priv = (struct rptun_priv_s *)arg;
    return aos_event_set(&priv->event, RPTUN_EVENT, AOS_EVENT_OR);
}

static struct remoteproc *rptun_init(struct remoteproc *rproc,
                                     struct remoteproc_ops *ops,
                                     void *arg)
{
    rproc->ops = ops;
    rproc->priv = arg;

    return rproc;
}

static void rptun_remove(struct remoteproc *rproc)
{
    rproc->priv = NULL;
}

static int rptun_mmap(struct remoteproc *rproc,
                      metal_phys_addr_t *pa, metal_phys_addr_t *da,
                      void **va, size_t size, unsigned int attribute,
                      struct metal_io_region **io_)
{
    struct rptun_priv_s *priv = rproc->priv;
    struct metal_io_region *io = metal_io_get_region();

    if (*pa != METAL_BAD_PHYS) {
        *da = rptun_pa_to_da(priv->dev, *pa);
        *va = metal_io_phys_to_virt(io, *pa);
        if (!*va)
            return -RPROC_EINVAL;
    } else if (*da != METAL_BAD_PHYS) {
        *pa = rptun_da_to_pa(priv->dev, *da);
        *va = metal_io_phys_to_virt(io, *pa);
        if (!*va)
            return -RPROC_EINVAL;
    } else if (*va) {
        *pa = metal_io_virt_to_phys(io, *va);
        if (*pa == METAL_BAD_PHYS)
            return -RPROC_EINVAL;
        *da = rptun_pa_to_da(priv->dev, *pa);
    } else {
        return -RPROC_EINVAL;
    }

    if (io_)
        *io_ = io;

    return 0;
}

static int rptun_config(struct remoteproc *rproc, void *data)
{
    struct rptun_priv_s *priv = rproc->priv;

    if (RPTUN_IS_MASTER(priv->dev)) {
        return RPTUN_CONFIG(priv->dev, data);
    }

    return 0;
}

static int rptun_start(struct remoteproc *rproc)
{
    struct rptun_priv_s *priv = rproc->priv;

    if (RPTUN_IS_MASTER(priv->dev)) {
        return RPTUN_START(priv->dev);
    }

    return 0;
}

static int rptun_stop(struct remoteproc *rproc)
{
    struct rptun_priv_s *priv = rproc->priv;

    if (RPTUN_IS_MASTER(priv->dev)) {
        return RPTUN_STOP(priv->dev);
    }

    return 0;
}

static int rptun_notify(struct remoteproc *rproc, uint32_t id)
{
    struct rptun_priv_s *priv = rproc->priv;

    RPTUN_NOTIFY(priv->dev, RPTUN_NOTIFY_ALL);

    return 0;
}

static void *rptun_get_priv_by_rdev(struct rpmsg_device *rdev)
{
    struct rpmsg_virtio_device *rvdev;
    struct virtio_device *vdev;
    struct remoteproc_virtio *rpvdev;
    struct remoteproc *rproc;

    rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);
    vdev  = rvdev->vdev;
    if (!vdev) {
        return NULL;
    }

    rpvdev = metal_container_of(vdev, struct remoteproc_virtio, vdev);
    rproc  = rpvdev->priv;
    if (!rproc) {
        return NULL;
    }

    return rproc->priv;
}

static void rptun_ns_bind(struct rpmsg_device *rdev,
                          const char *name, uint32_t dest)
{
    struct rptun_priv_s *priv = rptun_get_priv_by_rdev(rdev);
    struct rptun_bind_s *bind;

    bind = aos_malloc(sizeof(struct rptun_bind_s));
    if (bind) {
        struct metal_list *node;
        struct rptun_cb_s *cb;

        bind->dest = dest;
        strncpy(bind->name, name, RPMSG_NAME_SIZE);

        aos_mutex_lock(&g_rptun_sem, AOS_WAIT_FOREVER);

        metal_list_add_tail(&priv->bind, &bind->node);

        metal_list_for_each(&g_rptun_cb, node) {
            cb = metal_container_of(node, struct rptun_cb_s, node);
            if (cb->ns_bind) {
                cb->ns_bind(rdev, cb->priv, name, dest);
            }
        }

        aos_mutex_unlock(&g_rptun_sem);
    }
}

static int rptun_dev_start(struct remoteproc *rproc)
{
    struct rptun_priv_s *priv = rproc->priv;
    struct virtio_device *vdev;
    struct rptun_rsc_s *rsc;
    struct metal_list *node;
    struct rptun_cb_s *cb;
    unsigned int role = RPMSG_REMOTE;
    int ret;

    if (RPTUN_GET_FIRMWARE(priv->dev)) {
        struct rptun_store_s store = {0};

        ret = remoteproc_load(rproc, RPTUN_GET_FIRMWARE(priv->dev),
                              &store, &g_rptun_storeops, NULL);
        if (ret) {
            return ret;
        }

        rsc = rproc->rsc_table;
    } else {
        rsc = RPTUN_GET_RESOURCE(priv->dev);
        if (!rsc) {
            return -EINVAL;
        }

        ret = remoteproc_set_rsc_table(rproc, (struct resource_table *)rsc,
                                       sizeof(struct rptun_rsc_s));
        if (ret) {
            return ret;
        }
    }

    /* Update resource table on MASTER side */

    if (RPTUN_IS_MASTER(priv->dev)) {
        uint32_t tbsz, v0sz, v1sz, shbufsz;
        metal_phys_addr_t da0, da1;
        uint32_t align0, align1;
        void *va0, *va1;
        void *shbuf;

        align0 = B2C(rsc->rpmsg_vring0.align);
        align1 = B2C(rsc->rpmsg_vring1.align);

        tbsz = ALIGN_UP(sizeof(struct rptun_rsc_s), MAX(align0, align1));
        v0sz = ALIGN_UP(vring_size(rsc->rpmsg_vring0.num, align0), align0);
        v1sz = ALIGN_UP(vring_size(rsc->rpmsg_vring1.num, align1), align1);

        va0 = (char *)rsc + tbsz;
        va1 = (char *)rsc + tbsz + v0sz;

        da0 = da1 = METAL_BAD_PHYS;

        remoteproc_mmap(rproc, NULL, &da0, &va0, v0sz, 0, NULL);
        remoteproc_mmap(rproc, NULL, &da1, &va1, v1sz, 0, NULL);

        rsc->rpmsg_vring0.da = da0;
        rsc->rpmsg_vring1.da = da1;

        shbuf   = (char *)rsc + tbsz + v0sz + v1sz;
        shbufsz = rsc->buf_size * (rsc->rpmsg_vring0.num + rsc->rpmsg_vring1.num);

        rpmsg_virtio_init_shm_pool(&priv->shm_pool, shbuf, shbufsz);

        role = RPMSG_MASTER;
    }

    /* Remote proc create */

    vdev = remoteproc_create_virtio(rproc, 0, role, NULL);
    if (!vdev) {
        return -ENOMEM;
    }

    ret = rpmsg_init_vdev(&priv->vdev, vdev, rptun_ns_bind,
                          metal_io_get_region(), &priv->shm_pool);
    if (ret) {
        remoteproc_remove_virtio(rproc, vdev);
        return ret;
    }

    /* Remote proc start */

    ret = remoteproc_start(rproc);
    if (ret) {
        remoteproc_remove_virtio(rproc, vdev);
        return ret;
    }

    aos_mutex_lock(&g_rptun_sem, AOS_WAIT_FOREVER);

    /* Add priv to list */

    metal_list_add_tail(&g_rptun_priv, &priv->node);

    /* Broadcast device_created to all registers */

    metal_list_for_each(&g_rptun_cb, node) {
        cb = metal_container_of(node, struct rptun_cb_s, node);
        if (cb->device_created) {
            cb->device_created(&priv->vdev.rdev, cb->priv);
        }
    }

    aos_mutex_unlock(&g_rptun_sem);

    /* Register callback to mbox for receving remote message */

    RPTUN_REGISTER_CALLBACK(priv->dev, rptun_callback, priv);

    priv->started = true;
    return 0;
}

static int rptun_dev_stop(struct remoteproc *rproc)
{
    struct rptun_priv_s *priv = rproc->priv;
    struct metal_list *node;
    struct rptun_cb_s *cb;

    /* Unregister callback from mbox */

    RPTUN_UNREGISTER_CALLBACK(priv->dev);

    aos_mutex_lock(&g_rptun_sem, AOS_WAIT_FOREVER);

    /* Remove priv from list */

    metal_list_del(&priv->node);

    /* Broadcast device_destroy to all registers */

    metal_list_for_each(&g_rptun_cb, node) {
        cb = metal_container_of(node, struct rptun_cb_s, node);
        if (cb->device_destroy) {
            cb->device_destroy(&priv->vdev.rdev, cb->priv);
        }
    }

    aos_mutex_unlock(&g_rptun_sem);

    /* Remote proc stop */

    remoteproc_stop(rproc);

    /* Remote proc remove */

    remoteproc_remove_virtio(rproc, priv->vdev.vdev);
    rpmsg_deinit_vdev(&priv->vdev);

    /* Free bind list */

    metal_list_for_each(&priv->bind, node) {
        struct rptun_bind_s *bind;

        bind = metal_container_of(node, struct rptun_bind_s, node);
        aos_free(bind);
    }

    priv->started = false;
    return 0;
}

static int rptun_dev_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    inode_t *inode = filep->node;
    struct rptun_priv_s *priv = inode->i_arg;
    int ret = -ENOTTY;

    switch (cmd) {
        case RPTUNIOC_START:
            if (!priv->started) {
                ret = rptun_dev_start(&priv->rproc);
            }
            break;
        case RPTUNIOC_STOP:
            if (priv->started) {
                ret = rptun_dev_stop(&priv->rproc);
            }
            break;
    }

    return ret;
}

static int rptun_store_open(void *store_, const char *path,
                            const void **img_data)
{
    struct rptun_store_s *store = store_;
    int len = 0x100;

    store->fd = aos_open(path, O_RDONLY);
    if (store->fd < 0) {
        return -EINVAL;
    }

    store->buf = aos_malloc(len);
    if (!store->buf) {
        aos_close(store->fd);
        return -ENOMEM;
    }

    *img_data = store->buf;

    return aos_read(store->fd, store->buf, len);
}

static void rptun_store_close(void *store_)
{
    struct rptun_store_s *store = store_;

    aos_free(store->buf);
    aos_close(store->fd);
}

static int rptun_store_load(void *store_, size_t offset,
                            size_t size, const void **data,
                            metal_phys_addr_t pa,
                            struct metal_io_region *io,
                            char is_blocking)
{
    struct rptun_store_s *store = store_;
    char *tmp;

    if (pa == METAL_BAD_PHYS) {
        tmp = aos_realloc(store->buf, size);
        if (!tmp) {
          return -ENOMEM;
        }

        store->buf = tmp;
        *data = tmp;
    } else {
        tmp = metal_io_phys_to_virt(io, pa);
        if (!tmp) {
            return -EINVAL;
        }
    }

    aos_lseek(store->fd, offset, SEEK_SET);
    return aos_read(store->fd, tmp, size);
}

static metal_phys_addr_t rptun_pa_to_da(struct rptun_dev_s *dev, metal_phys_addr_t pa)
{
    const struct rptun_addrenv_s *addrenv;
    uint32_t i;

    addrenv = RPTUN_GET_ADDRENV(dev);
    if (!addrenv) {
        return pa;
    }

    for (i = 0; addrenv[i].size; i++) {
        if (pa - addrenv[i].pa < addrenv[i].size) {
            return addrenv[i].da + (pa - addrenv[i].pa);
        }
    }

    return pa;
}

static metal_phys_addr_t rptun_da_to_pa(struct rptun_dev_s *dev, metal_phys_addr_t da)
{
    const struct rptun_addrenv_s *addrenv;
    uint32_t i;

    addrenv = RPTUN_GET_ADDRENV(dev);
    if (!addrenv) {
        return da;
    }

    for (i = 0; addrenv[i].size; i++) {
        if (da - addrenv[i].da < addrenv[i].size) {
            return addrenv[i].pa + (da - addrenv[i].da);
        }
    }

    return da;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

const char *rpmsg_get_cpuname(struct rpmsg_device *rdev)
{
    struct rptun_priv_s *priv = rptun_get_priv_by_rdev(rdev);

    return RPTUN_GET_CPUNAME(priv->dev);
}

int rpmsg_register_callback(void *priv_,
                            rpmsg_dev_cb_t device_created,
                            rpmsg_dev_cb_t device_destroy,
                            rpmsg_bind_cb_t ns_bind)
{
    struct metal_list *node, *bnode;
    struct rptun_cb_s *cb;

    cb = aos_zalloc(sizeof(struct rptun_cb_s));
    if (!cb) {
        return -ENOMEM;
    }

    cb->priv           = priv_;
    cb->device_created = device_created;
    cb->device_destroy = device_destroy;
    cb->ns_bind        = ns_bind;

    aos_mutex_lock(&g_rptun_sem, AOS_WAIT_FOREVER);

    metal_list_add_tail(&g_rptun_cb, &cb->node);

    metal_list_for_each(&g_rptun_priv, node) {
        struct rptun_priv_s *priv;

        priv = metal_container_of(node, struct rptun_priv_s, node);
        if (device_created) {
            device_created(&priv->vdev.rdev, priv_);
        }

        if (ns_bind) {
            metal_list_for_each(&priv->bind, bnode) {
                struct rptun_bind_s *bind;

                bind = metal_container_of(bnode, struct rptun_bind_s, node);
                ns_bind(&priv->vdev.rdev, priv_, bind->name, bind->dest);
            }
        }
    }

    aos_mutex_unlock(&g_rptun_sem);

    return 0;
}

void rpmsg_unregister_callback(void *priv_,
                               rpmsg_dev_cb_t device_created,
                               rpmsg_dev_cb_t device_destroy,
                               rpmsg_bind_cb_t ns_bind)
{
    struct metal_list *node, *pnode;

    aos_mutex_lock(&g_rptun_sem, AOS_WAIT_FOREVER);

    metal_list_for_each(&g_rptun_cb, node) {
        struct rptun_cb_s *cb = NULL;

        cb = metal_container_of(node, struct rptun_cb_s, node);
        if (cb->priv == priv_ &&
            cb->device_created == device_created &&
            cb->device_destroy == device_destroy &&
            cb->ns_bind == ns_bind) {
            if (device_destroy) {
                metal_list_for_each(&g_rptun_priv, pnode) {
                    struct rptun_priv_s *priv;

                    priv = metal_container_of(pnode, struct rptun_priv_s, node);
                    device_destroy(&priv->vdev.rdev, priv_);
                }
            }

            metal_list_del(&cb->node);
            aos_free(cb);
        }
    }

    aos_mutex_unlock(&g_rptun_sem);
}

int rptun_initialize(struct rptun_dev_s *dev)
{
    struct metal_init_params params = METAL_INIT_DEFAULTS;
    struct rptun_priv_s *priv;
    char name[16];
    int ret;

    ret = metal_init(&params);
    if (ret) {
        return ret;
    }

    priv = aos_zalloc(sizeof(struct rptun_priv_s));
    if (priv == NULL) {
        return -ENOMEM;
    }

    ret = aos_event_new(&priv->event, 0);
    if (ret < 0) {
        aos_free(priv);
        return ret;
    }

    sprintf(priv->name, "rptun%s", RPTUN_GET_CPUNAME(dev));

    ret = aos_task_new_ext(&(priv->handle), priv->name, rptun_thread, priv,
                           CONFIG_RPTUN_STACKSIZE, CONFIG_RPTUN_PRIORITY);
    if (ret < 0) {
        aos_event_free(&priv->event);
        aos_free(priv);
        return ret;
    }

    priv->dev = dev;

    aos_mutex_new(&g_rptun_sem);

    metal_list_init(&priv->bind);

    remoteproc_init(&priv->rproc, &g_rptun_ops, priv);
    remoteproc_config(&priv->rproc, NULL);

    if (RPTUN_IS_AUTOSTART(dev)) {
        rptun_dev_start(&priv->rproc);
    }

    sprintf(name, "/dev/rptun%s", RPTUN_GET_CPUNAME(dev));
    return aos_register_driver(name, &g_rptun_devops, priv);
}

int rptun_boot(const char *cpuname)
{
    int file;
    char name[16];
    int ret;

    if (!cpuname) {
        return -EINVAL;
    }

    sprintf(name, "/dev/rptun%s", cpuname);

    file = aos_open(name, O_RDONLY);
    if (file < 0) {
        return -EINVAL;
    }

    ret = aos_ioctl(file, RPTUNIOC_START, 0);
    aos_close(file);

    return ret;
}
