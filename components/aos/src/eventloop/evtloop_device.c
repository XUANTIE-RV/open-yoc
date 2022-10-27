/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include "evtloop_main.h"

#ifndef DEBUG_MALLOC
#define DEBUG_MALLOC malloc
#endif

#ifndef DEBUG_FREE
#define DEBUG_FREE free
#endif

typedef struct {
    aos_mutex_t    mutex;
    void      *sem;
    int            counter;
    dlist_t        bufs;
    int            cache_count;
    dlist_t        buf_cache;
} event_dev_t;

typedef struct {
    dlist_t node;
    int len;
    char buf[];
} dev_event_t;

static event_dev_t dev_pri;
int eventloop_local_event_init(void)
{
    memset(&dev_pri, 0, sizeof(dev_pri));

    event_dev_t *pdev = &dev_pri;

    aos_mutex_new(&pdev->mutex);
    dlist_init(&pdev->bufs);
    dlist_init(&pdev->buf_cache);
    return 0;
}

int eventloop_local_event_deinit(void)
{
    int i = 0;

    event_dev_t *pdev = &dev_pri;
    dlist_t *node = NULL;

    /* clear bufs */
    for (i = 0; i < pdev->counter; i++) {
        node = (dlist_t *)pdev->bufs.next;
        dlist_del(node);
        DEBUG_FREE(node);
    }

    /* clear buf cache */
    for (i = 0; i < pdev->cache_count; i++) {
        node = (dlist_t *)pdev->buf_cache.next;
        dlist_del(node);
        DEBUG_FREE(node);
    }

    aos_mutex_free(&pdev->mutex);

    return 0;
}

int event_write(const void *buf, size_t len)
{
    event_dev_t *pdev = &dev_pri;
    aos_mutex_lock(&pdev->mutex, AOS_WAIT_FOREVER);

    dev_event_t *evt;
    evt = (dev_event_t *)pdev->buf_cache.next;

    if (pdev->cache_count > 0 && evt->len == len) {
        dlist_del(&evt->node);
        pdev->cache_count --;
    } else {
        evt = DEBUG_MALLOC(sizeof(*evt) + len);
    }

    if (evt == NULL) {
        len = -1;
        goto out;
    }

    pdev->counter ++;

    evt->len = len;
    memcpy(evt->buf, buf, len);
    dlist_add_tail(&evt->node, &pdev->bufs);

    if (pdev->sem != NULL) {
        aos_sem_signal((aos_sem_t *)pdev->sem);
    }

out:
    aos_mutex_unlock(&pdev->mutex);
    return len;
}

int event_read(void *buf, size_t len)
{
    event_dev_t *pdev = &dev_pri;

    int cnt = pdev->counter;

    if (!cnt) {
        return 0;
    }

    aos_mutex_lock(&pdev->mutex, AOS_WAIT_FOREVER);

    dev_event_t *evt = (dev_event_t *)pdev->bufs.next;
    dlist_del(&evt->node);
    cnt = (len > evt->len) ? evt->len : len;
    memcpy(buf, evt->buf, cnt);

    if (pdev->cache_count < 4) {
        dlist_add(&evt->node, &pdev->buf_cache);
        pdev->cache_count ++;
    } else {
        DEBUG_FREE(evt);
    }

    pdev->counter --;

    aos_mutex_unlock(&pdev->mutex);

    return cnt;
}

int event_poll(void *reader, bool setup, void *sem)
{
    event_dev_t *pdev = &dev_pri;

    if (!setup) {
        pdev->sem = NULL;
        return 0;
    }

    pdev->sem = sem;

    if (pdev->counter) {
        ((eventloop_sock_t *)reader)->event = 1; /* set event flag*/
        aos_sem_signal((aos_sem_t *)sem);
    }

    return 0;
}

