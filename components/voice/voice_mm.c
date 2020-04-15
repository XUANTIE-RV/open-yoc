/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <aos/list.h>

#include "voice_ap.h"

typedef struct {
    slist_t next;
    void *data_bake;
    void *data_cur;
} voice_heap_t;

slist_t g_voice_heap_head;

void *voice_malloc(unsigned int size)
{
    voice_heap_t *node = aos_malloc(sizeof(voice_heap_t));

    node->data_bake = aos_malloc_check(size + VOICE_DCACHE_OFFSET*2);

    if (((uint32_t)node->data_bake & 0xfffffff0) != 0) {
        node->data_cur = (char *)(((int)node->data_bake + VOICE_DCACHE_OFFSET) & 0xfffffff0);
    } else {
        node->data_cur = node->data_bake;
    }

    slist_add(&node->next, &g_voice_heap_head);

    return node->data_cur;
}

void voice_free(void *data)
{
    voice_heap_t *node;
    slist_t *tmp;

    slist_for_each_entry_safe(&g_voice_heap_head, tmp, node, voice_heap_t, next) {
        if (data == node->data_cur) {
            aos_free(node->data_bake);
            slist_del(&node->next, &g_voice_heap_head);
            break;
        }
    }
}