 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <aos/kernel.h>
#include <devices/device.h>
#include <dev_internal.h>

#ifndef CONFIG_MAX_DEV_COUNT
#define CONFIG_MAX_DEV_COUNT 8
#endif

static slist_t device_list;

#define CONFIG_DEVICE_THREAD_SAFE 1

#ifdef CONFIG_DEVICE_THREAD_SAFE
static aos_mutex_t device_mutex;

#define LIST_LOCK()                                                                                \
    do {                                                                                           \
        if (!aos_mutex_is_valid(&device_mutex))                                                    \
            aos_mutex_new(&device_mutex);                                                          \
        aos_mutex_lock(&device_mutex, AOS_WAIT_FOREVER);                                           \
    } while (0)

#define LIST_UNLOCK()                                                                              \
    do {                                                                                           \
        aos_mutex_unlock(&device_mutex);                                                           \
    } while (0)

#else

#define LIST_LOCK()
#define LIST_UNLOCK()

#endif

rvm_dev_t *rvm_hal_device_new(driver_t *drv, int size, int id)
{
    rvm_dev_t *dev = (rvm_dev_t*)aos_zalloc(size);

    if (dev) {
        dev->id = id;
        dev->drv = drv;

        if (aos_mutex_new(&dev->mutex) != 0) {
            aos_free(dev);
            return NULL;
        }
    }

    return dev;
}

void rvm_hal_device_free(rvm_dev_t *dev)
{
    aos_mutex_free(&dev->mutex);
    aos_free(dev);
}

int device_lock(rvm_dev_t *dev)
{
    return aos_mutex_lock(&dev->mutex, AOS_WAIT_FOREVER);
}

int device_unlock(rvm_dev_t *dev)
{
    return aos_mutex_unlock(&dev->mutex);
}

static int alloc_device_id()
{
    uint32_t index = rand() & 0xFFFF;

    LIST_LOCK();
    while (1) {
        index++;

        int found = 0;
        rvm_dev_t *node;
        slist_for_each_entry(&device_list, node,  rvm_dev_t, node) {
            driver_t *drv = (driver_t *)node->drv;
            aos_check_param(drv);
            if (drv->device_id == index << 16) {
                found = 1;
                break;
            }
        }

        if (found == 0)
            break;
    }
    LIST_UNLOCK();

    return index << 16;
}

int rvm_driver_register(driver_t *drv, void *config, int idx)
{
    aos_check_param(drv && drv->init);

    /* name exist check */
    if (rvm_hal_device_find(drv->name, idx) != NULL)
        return 0;

    rvm_dev_t *dev = drv->init(drv, config, idx);

    if (dev) {
        dev->id = idx;
        ((driver_t *)(dev->drv))->device_id = alloc_device_id();
        LIST_LOCK();
        slist_add_tail(&dev->node, &device_list);
        LIST_UNLOCK();

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
        extern int devices_fs_init(rvm_dev_t *dev, const char *name, int id);
        int ret = devices_fs_init(dev, drv->name, idx);
        if (ret) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%s%d", drv->name, idx);
            rvm_driver_unregister(buf);
            return ret;
        }
#endif
        return 0;
    }

    return -1;
}

rvm_dev_t *rvm_hal_device_find(const char *name, int id)
{
    rvm_dev_t *node;
    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  rvm_dev_t, node) {
        driver_t *drv = (driver_t *)node->drv;
        aos_assert(drv);
        if (strcmp(drv->name, name) == 0 && node->id == id)
            break;
    }
    LIST_UNLOCK();

    return node;
}

static char *dev_name_parse(const char *name, int *id)
{
    char *n = strdup(name);

    if (n == NULL)
        return NULL;

    int len = strlen(n);

    for (int i = 0; i < len; i++) {
        if (n[i] >= '0' && n[i] <= '9') {
            *id = atoi(n + i);
            n[i] = '\0';
            break;
        }
    }

    return n;
}

int rvm_driver_unregister(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if (n == NULL)
        return -ENOMEM;

    rvm_dev_t *dev = rvm_hal_device_find(n, id);
    aos_free(n);

    if (dev) {
        driver_t *drv = (driver_t *)dev->drv;
        aos_assert(drv);

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
    if (devfs_node_is_valid(&dev->devfs_node))
        (void)devfs_remove_node(&dev->devfs_node);
#endif

        if (drv->uninit)
            drv->uninit(dev);

        LIST_LOCK();
        slist_del(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

rvm_dev_t *device_open_id(const char *name, int id)
{
    rvm_dev_t *dev = rvm_hal_device_find(name, id);

    if (dev) {
        device_lock(dev);
        if (dev->ref == 0) {
            if (DRIVER(dev)->open(dev) != 0) {
                device_unlock(dev);
                return NULL;
            }
        }
        dev->ref++;
        device_unlock(dev);
    }

    return dev;
}

rvm_dev_t *rvm_hal_device_open(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if (n != NULL) {
        rvm_dev_t *dev = device_open_id(n, id);
        aos_free(n);
        return dev;
    }

    return NULL;
}

int rvm_hal_device_close(rvm_dev_t *dev)
{
    int ret = 0;

    if (dev) {
        device_lock(dev);
        if (dev->ref > 0) {
            dev->ref--;
            if (dev->ref == 0)
                ret = DRIVER(dev)->close(dev);
        }
        device_unlock(dev);
    }

    return ret;
}

int device_valid(rvm_dev_t *dev, const char *name)
{
    if (dev && dev->drv) {
        const char *n = NULL;
        driver_t *drv = (driver_t*)dev->drv;
        if (drv->type)
            n = drv->type;
        else
            n = drv->name;

        if (n)
            return strcmp(n, name);
    }

    return -1;
}

int device_is_busy(void)
{
    rvm_dev_t *node;
    int busy = 0;

    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  rvm_dev_t, node) {
        if (node->busy == 1) {
            busy = 1;
            break;
        }
    }
    LIST_UNLOCK();

    return busy;
}

int rvm_hal_device_lpm_enable(rvm_dev_t *dev, int pm_state)
{
    int ret = -EBADF;

    if (dev) {
        device_lock(dev);
        driver_t *drv = (driver_t*)dev->drv;
        aos_check_param(drv);
        ret = drv->lpm(dev, pm_state);
        device_unlock(dev);
    }

    return ret;
}

void rvm_hal_devices_lpm_enable(int pm_state)
{
    rvm_dev_t *node;

    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  rvm_dev_t, node) {
        driver_t *drv = (driver_t *)node->drv;
        aos_check_param(drv);
        if (drv->lpm) {
            drv->lpm(node, pm_state);
        }
    }
    LIST_UNLOCK();
}

