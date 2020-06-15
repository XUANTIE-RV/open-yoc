/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <aos/kernel.h>
#include <devices/driver.h>

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

aos_dev_t *device_new(driver_t *drv, int size, int id)
{
    aos_dev_t *dev = (aos_dev_t*)aos_zalloc(size);

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

void device_free(aos_dev_t *dev)
{
    aos_mutex_free(&dev->mutex);
    aos_free(dev);
}

int device_lock(aos_dev_t *dev)
{
    return aos_mutex_lock(&dev->mutex, AOS_WAIT_FOREVER);
}

int device_unlock(aos_dev_t *dev)
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
        aos_dev_t *node;
        slist_for_each_entry(&device_list, node,  aos_dev_t, node) {
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

int driver_register(driver_t *drv, void *config, int idx)
{
    aos_check_param(drv && drv->init);

    /* name exist check */
    if (device_find(drv->name, idx) != NULL)
        return -1;

    aos_dev_t *dev = drv->init(drv, config, idx);

    if (dev) {
        dev->id = idx;
        ((driver_t *)(dev->drv))->device_id = alloc_device_id();
        LIST_LOCK();
        slist_add_tail(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

aos_dev_t *device_find(const char *name, int id)
{
    aos_dev_t *node;
    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  aos_dev_t, node) {
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

int driver_unregister(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if (n == NULL)
        return -ENOMEM;

    aos_dev_t *dev = device_find(name, id);
    aos_free(n);

    if (dev) {
        driver_t *drv = (driver_t *)dev->drv;
        aos_assert(drv);
        if (drv->uninit)
            drv->uninit(dev);

        LIST_LOCK();
        slist_del(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

aos_dev_t *device_open(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if (n != NULL) {
        aos_dev_t *dev = device_open_id(n, id);
        aos_free(n);
        return dev;
    }

    return NULL;
}

aos_dev_t *device_open_id(const char *name, int id)
{
    aos_dev_t *dev = device_find(name, id);

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

int device_close(aos_dev_t *dev)
{
    int ret = -EBADF;

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

int device_valid(aos_dev_t *dev, const char *name)
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

int device_tags(aos_dev_t *dev)
{
    driver_t *drv = (driver_t*)dev->drv;
    aos_check_param(drv);

    return drv->device_id;
}

int device_is_busy(void)
{
    aos_dev_t *node;
    int busy = 0;

    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  aos_dev_t, node) {
        if (node->busy == 1) {
            busy = 1;
            break;
        }
    }
    LIST_UNLOCK();

    return busy;
}

int device_lpm(aos_dev_t *dev, int pm_state)
{
    int ret = -EBADF;

    if (dev) {
        device_lock(dev);
        driver_t *drv = (driver_t*)dev->drv;
        
        aos_check_param(drv);
        ret = DRIVER(dev)->lpm(dev, pm_state);
        device_unlock(dev);
    }

    return ret;
}

void device_manage_power(int pm_state)
{
    aos_dev_t *node;

    LIST_LOCK();
    slist_for_each_entry(&device_list, node,  aos_dev_t, node) {
        driver_t *drv = (driver_t *)node->drv;
        aos_check_param(drv);
        if (drv->lpm) {
            drv->lpm(node, pm_state);
        }
    }
    LIST_UNLOCK();
}

