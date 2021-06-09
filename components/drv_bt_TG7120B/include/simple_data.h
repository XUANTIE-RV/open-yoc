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
#ifndef _SIMPLE_DATA_H_
#define _SIMPLE_DATA_H_

#include <misc/slist.h>

typedef struct _simple_data_t {
    sys_snode_t node;
    void *pool_ref;
    uint8_t in_use;
    uint8_t reserve;
    uint16_t data_len;
    uint8_t *data;
} simple_data_t;

typedef struct _simple_pool_t {
    uint16_t data_count;
    uint16_t data_size;
    uint32_t alloc_count;
    uint32_t peek_count;
    sys_slist_t free;
    simple_data_t *simple_data_ref;
    void *data_ref;
} simple_pool_t;

#define ALIGN_MASK       (sizeof(void*)-1)
#define ALIGN_UP(a)   (((a) + ALIGN_MASK) & ~ALIGN_MASK)

#define SIMPLE_POOL_INIT(_name,_count,_data_size)    \
    static uint32_t _name##_data[_count][ALIGN_UP(_data_size) / 4] = {0};  \
    static simple_data_t  _name##_simple_data[_count]; \
    static simple_pool_t _name = {                 \
                                                   .data_count = _count,                      \
                                                   .data_size = _data_size,                        \
                                                   .simple_data_ref = (simple_data_t *)_name##_simple_data,  \
                                                   .data_ref = _name##_data,  \
                                                   .alloc_count = 0,  \
                                 };

void simple_data_init(simple_pool_t *pool);

simple_data_t *simple_data_alloc(simple_pool_t *pool);

void simple_data_free(simple_data_t *simple_data);

#endif
