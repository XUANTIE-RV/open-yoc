/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <unistd.h>

#include <aos/list.h>
#include <aos/log.h>
#include <aos/version.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#include <drv/wdt.h>

#define SOFTWDT_TASK_STACK_SIZE 512
#define SOFTWDT_TIME 20000
#define LOOP_TIME 1000

// FIXME:
#define CONFIG_USE_HW

static const char *TAG = "swdt";

struct softwdt_node {
    uint32_t index;
    int      left_time;
    int      max_time;
    void (*will)(void *args);
    void   *args;
    slist_t node;
};

static struct softwdt_context {
    int          count;
    aos_task_t   task;
    aos_mutex_t  mutex;
    aos_event_t  wait_event;
#ifdef CONFIG_USE_HW
    wdt_handle_t hw_wdg_handle;
#endif
    slist_t      head;
} g_softwdt_ctx;

static struct softwdt_node *softwdt_find(int index)
{
    struct softwdt_node *node;
    slist_for_each_entry(&g_softwdt_ctx.head, node, struct softwdt_node, node) {
        if (node->index == index)
            return node;
    }

    return NULL;
}

static void softwdt_task_entry(void *arg)
{
    struct softwdt_context *ctx = (struct softwdt_context *)arg;

    while (1) {
        aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER);
        if (ctx->count == 0) {
            unsigned int actl_flags = 0;
            aos_mutex_unlock(&ctx->mutex);
            aos_event_get(&ctx->wait_event, 1, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);

            continue;
        }

#ifdef CONFIG_USE_HW
        if (ctx->hw_wdg_handle)
            csi_wdt_restart(ctx->hw_wdg_handle);
#endif
        struct softwdt_node *node;
        slist_for_each_entry(&ctx->head, node, struct softwdt_node, node) {
            if (node->max_time > 0) {
                node->left_time -= LOOP_TIME;

                if (node->left_time * 2 < node->max_time)
                    LOGW(TAG, "%x: left_time %d", node->index, node->left_time);

                if (node->left_time <= 0) {
                    if (node->will)
                        node->will(node->args);
                    else {
                        LOGE(TAG, "softwdt %u crash!!!", node->index);

                        if (node->args)
                            LOGE(TAG, "softwdt info: %s", (char *)node->args);
                    }
                    // use hw watchdog reboot
                    aos_reboot();
                }
            }
        }
        aos_mutex_unlock(&ctx->mutex);
        aos_msleep(LOOP_TIME);
    }
}

static int aos_wdt_init(void)
{
    static uint8_t softwdt_inited = 0;

    if (softwdt_inited != 0)
        return 0;

    memset(&g_softwdt_ctx, 0, sizeof(g_softwdt_ctx));

    aos_event_new(&g_softwdt_ctx.wait_event, 0);
    aos_mutex_new(&g_softwdt_ctx.mutex);
    aos_task_new_ext(&g_softwdt_ctx.task, "softwdt", softwdt_task_entry, &g_softwdt_ctx,
                     SOFTWDT_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI - 5);
    softwdt_inited = 1;

    return 0;
}

uint32_t aos_wdt_index()
{
    uint32_t index = rand();

    aos_wdt_init();

    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);
    while (1) {
        if (index == 0)
            index++;
        index++;

        if (softwdt_find(index) == NULL)
            break;
    }
    aos_mutex_unlock(&g_softwdt_ctx.mutex);

    return index;
}

void aos_wdt_attach(uint32_t index, void (*will)(void *), void *args)
{
    aos_wdt_init();

    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    if (node == NULL) {
        node = (struct softwdt_node *)aos_zalloc(sizeof(struct softwdt_node));

        if (node) {
            node->left_time = 0;
            node->max_time  = 0;
            node->index     = index;

            slist_add_tail(&node->node, &g_softwdt_ctx.head);

            g_softwdt_ctx.count++;

            if (g_softwdt_ctx.count == 1) {
                aos_event_set(&g_softwdt_ctx.wait_event, 1, AOS_EVENT_OR);
            }
        }
    }

    if (node) {
        node->will = will;
        node->args = args;
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

void aos_wdt_detach(uint32_t index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    if (node) {
        slist_del(&node->node, &g_softwdt_ctx.head);
        aos_free(node);

        g_softwdt_ctx.count--;
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

int aos_wdt_exists(uint32_t index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    aos_mutex_unlock(&g_softwdt_ctx.mutex);

    return node != NULL;
}

void aos_wdt_feed(uint32_t index, int max_time)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    if (node) {
        node->max_time  = max_time;
        node->left_time = node->max_time;
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

void aos_wdt_show(uint32_t index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node;
    slist_for_each_entry(&g_softwdt_ctx.head, node, struct softwdt_node, node) {
        if (node->index == index)
            LOGE(TAG, "softwdt uint[%d], left_time = %d, max_time=%d\n", node->index,
                 node->left_time, node->max_time);
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

void aos_wdt_showall()
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node;
    slist_for_each_entry(&g_softwdt_ctx.head, node, struct softwdt_node, node) {
        LOGE(TAG, "softwdt uint[%d], left_time = %d, max_time=%d\n", node->index, node->left_time,
             node->max_time);
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

int aos_wdt_hw_enable(int id)
{
#ifdef CONFIG_USE_HW
    g_softwdt_ctx.hw_wdg_handle = csi_wdt_initialize(0, NULL);
    csi_wdt_set_timeout(g_softwdt_ctx.hw_wdg_handle, SOFTWDT_TIME);
    csi_wdt_start(g_softwdt_ctx.hw_wdg_handle);
    aos_check_return_einval(g_softwdt_ctx.hw_wdg_handle);

    return 0;
#else
    return -1;
#endif
}

void aos_wdt_hw_disable(int id)
{
#ifdef CONFIG_USE_HW
    csi_wdt_uninitialize(g_softwdt_ctx.hw_wdg_handle);
    g_softwdt_ctx.hw_wdg_handle = NULL;
#endif
}
