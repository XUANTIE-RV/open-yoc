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

#include <stdlib.h>
#include <unistd.h>

#include <aos/list.h>
#include <aos/version.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#include <drv/wdt.h>

#ifndef CONFIG_SOFTWDT_TASK_STACK_SIZE
#define CONFIG_SOFTWDT_TASK_STACK_SIZE 1024
#endif

#define SOFTWDT_TIME 20000
#define LOOP_TIME 1000

static const char *TAG = "swdt";

struct softwdt_node {
    long     index;
    int      left_time;
    int      max_time;
    void     (*will)(void *args);
    void     *args;
    slist_t node;
};

static struct softwdt_context {
    int          count;
    aos_task_t   task;
    aos_mutex_t  mutex;
#ifdef CONFIG_CSI_V2
    csi_wdt_t   hw_wdg_handle;
#else
    wdt_handle_t hw_wdg_handle;
#endif
    slist_t      head;
} g_softwdt_ctx;

static int g_wdt_debug = 1;
static int g_use_hw_wdt;

static struct softwdt_node *softwdt_find(long index)
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
    struct softwdt_node *node;
    struct softwdt_context *ctx = (struct softwdt_context *)arg;

    while (1) {
        aos_mutex_lock(&ctx->mutex, AOS_WAIT_FOREVER);
        if (g_use_hw_wdt) {
#ifdef CONFIG_CSI_V2
            csi_wdt_feed(&ctx->hw_wdg_handle);
#else
            if (ctx->hw_wdg_handle)
                csi_wdt_restart(ctx->hw_wdg_handle);
#endif
        }

        slist_for_each_entry(&ctx->head, node, struct softwdt_node, node) {
            if (node->max_time > 0) {
                node->left_time -= LOOP_TIME;
                if (node->left_time < 0) {
                    node->left_time = 0;
                }

                if (node->left_time * 2 < node->max_time)
                    LOGW(TAG, "%x: left_time %d", node->index, node->left_time);

                if (node->left_time <= 0) {
                    if (node->will)
                        node->will(node->args);
                    else {
                        LOGE(TAG, "softwdt %ld crash!!!", node->index);

                        if (node->args)
                            LOGE(TAG, "softwdt info: %s", (char *)node->args);
                    }
                    // use hw watchdog reboot
                    if (!g_wdt_debug) {
                        LOGW(TAG, "softwdt reboot");
                        aos_msleep(1000);
                        aos_reboot();
                    }
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

    aos_mutex_new(&g_softwdt_ctx.mutex);
    aos_task_new_ext(&g_softwdt_ctx.task, "softwdt", softwdt_task_entry, &g_softwdt_ctx,
                     CONFIG_SOFTWDT_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI - 5);
    softwdt_inited = 1;

    return 0;
}

long aos_wdt_index()
{
    long index = rand();

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

void aos_wdt_attach(long index, void (*will)(void *), void *args)
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
        }
    }

    if (node) {
        node->will = will;
        node->args = args;
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

void aos_wdt_detach(long index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    if (node) {
        slist_del(&node->node, &g_softwdt_ctx.head);
        aos_free(node);
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

int aos_wdt_exists(long index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node = softwdt_find(index);

    aos_mutex_unlock(&g_softwdt_ctx.mutex);

    return node != NULL;
}

void aos_wdt_feed(long index, int max_time)
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

void aos_wdt_show(long index)
{
    aos_wdt_init();
    aos_mutex_lock(&g_softwdt_ctx.mutex, AOS_WAIT_FOREVER);

    struct softwdt_node *node;
    slist_for_each_entry(&g_softwdt_ctx.head, node, struct softwdt_node, node) {
        if (node->index == index)
            LOGE(TAG, "softwdt uint[%ld], left_time = %d, max_time=%d\n", node->index,
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
        LOGE(TAG, "softwdt uint[%ld], left_time = %d, max_time=%d\n", node->index, node->left_time,
             node->max_time);
    }

    aos_mutex_unlock(&g_softwdt_ctx.mutex);
}

void aos_wdt_debug(int en)
{
    g_wdt_debug = en;
}

int aos_wdt_hw_enable(int id, int ms)
{
    g_use_hw_wdt = 0;
    aos_wdt_init();
#ifdef CONFIG_CSI_V2
    csi_error_t ret = csi_wdt_init(&g_softwdt_ctx.hw_wdg_handle, id);
    if (ret != CSI_OK) {
        return -1;
    }
    csi_wdt_set_timeout(&g_softwdt_ctx.hw_wdg_handle, ms);
    csi_wdt_start(&g_softwdt_ctx.hw_wdg_handle);
    aos_check_return_einval(&g_softwdt_ctx.hw_wdg_handle);
#else
    g_softwdt_ctx.hw_wdg_handle = csi_wdt_initialize(id, NULL);
    csi_wdt_set_timeout(g_softwdt_ctx.hw_wdg_handle, ms);
    csi_wdt_start(g_softwdt_ctx.hw_wdg_handle);
    aos_check_return_einval(g_softwdt_ctx.hw_wdg_handle);
#endif
    g_use_hw_wdt = 1;
    return 0;
}

void aos_wdt_hw_disable(int id)
{
    if (g_use_hw_wdt) {
#ifdef CONFIG_CSI_V2
        csi_wdt_uninit(&g_softwdt_ctx.hw_wdg_handle);
#else
        csi_wdt_uninitialize(g_softwdt_ctx.hw_wdg_handle);
        g_softwdt_ctx.hw_wdg_handle = NULL;
#endif
        g_use_hw_wdt = 0;
    }
}
