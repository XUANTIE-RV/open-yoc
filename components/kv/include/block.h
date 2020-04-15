/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef KV_BLOCK_H
#define KV_BLOCK_H

#include <stdint.h>

#include "kvset.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ERASE_FLAG 0

enum {
    NODE_ALL = 0,
    NODE_EXISTS = 1,
};

#define NODE_VAILD(n) ((n)->erase_flag != ERASE_FLAG)


struct flash_ops {
    int (*erase)(kvblock_t *block, int pos, int size);
    int (*write)(kvblock_t *block, int pos, void *data, int size);
    int (*read)(kvblock_t *block, int pos, void *data, int size);
} ;

struct kvblock {
    int          id;
    uint8_t      *mem;
    uint8_t      *mem_cache;
    uint8_t      mem_cache_ref;
    uint16_t     size;
    uint16_t     write_offset;
    uint16_t     kv_size;
    uint16_t     dirty_size;
    uint16_t     count;
    uint16_t     ro_count; // readonly kv count
    kv_t        *kv;
};

void kvblock_cache_malloc(kvblock_t *block);
void kvblock_cache_free(kvblock_t *block);

void kvblock_init(kvblock_t *block, uint8_t *mem, int size);
void kvblock_reset(kvblock_t *block);
void kvblock_calc(kvblock_t *block);
void kvblock_dump(kvblock_t *block);

static inline int kvblock_free_size(kvblock_t *block)
{
    return block->size - block->write_offset;
}

int       kvblock_find(kvblock_t *block, const char *key, kvnode_t *node);
int       kvblock_search(kvblock_t *block, uint8_t *c, kvnode_t *node);
kvnode_t *kvblock_check_version(kvnode_t *node1, kvnode_t *node2);

int  kvblock_alloc_node(kvblock_t *block, int size);
int  kvblock_set(kvblock_t *block, const char *key, void *value, int size, int version);
int  kvblock_iter(kvblock_t *block, int exists, int (*fn)(kvnode_t *, void *), void *data);
void kvblock_show_data(kvblock_t *block, int num);

void kvnode_rm(kvnode_t *node);
void kvnode_show(kvnode_t *node);

#ifdef __cplusplus
}
#endif

#endif
