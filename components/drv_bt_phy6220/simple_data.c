/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdint.h>
#include <string.h>
#include <simple_data.h>
#include <k_api.h>

void simple_data_init(simple_pool_t *pool)
{
    int i;
    simple_data_t *simple_data;

    if (pool == NULL) {
        return;
    }

    sys_slist_init(&pool->free);

    for (i = 0; i < pool->data_count; i++) {
        simple_data = &pool->simple_data_ref[i];
        memset(simple_data, 0, sizeof(simple_data_t));
        simple_data->in_use = 0;
        simple_data->pool_ref = pool;
        simple_data->data = (uint8_t *)pool->data_ref + ALIGN_UP(pool->data_size) * i;
        sys_slist_append(&pool->free, &simple_data->node);
    }

    return;
}

simple_data_t *simple_data_alloc(simple_pool_t *pool)
{
    if (pool == NULL) {
        return NULL;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    simple_data_t *simple_data = (simple_data_t *) sys_slist_get(&pool->free);

    if (simple_data) {
        simple_data->in_use = 1;
        simple_data->node.next = NULL;
        pool->alloc_count++;

        if (pool->alloc_count > pool->peek_count) {
            pool->peek_count = pool->alloc_count;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();

    return simple_data;
}

void simple_data_free(simple_data_t *simple_data)
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    simple_data->in_use = 0;
    simple_pool_t *pool = simple_data->pool_ref;
    sys_slist_append(&pool->free, &simple_data->node);
    pool->alloc_count--;
    RHINO_CPU_INTRPT_ENABLE();

}

