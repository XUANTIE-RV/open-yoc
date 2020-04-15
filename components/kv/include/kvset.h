/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_KVSET_H
#define AOS_KVSET_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct kvset kv_t;
typedef struct flash_ops flash_ops_t;
typedef struct kvblock kvblock_t;
typedef struct kvnode kvnode_t;
#include <block.h>

struct kvnode {
    uint8_t    rw;
    uint8_t    version;
    uint16_t   val_size; /* value size */
    uint32_t   erase_flag;

    uint16_t  head_offset; /* offset base block */
    uint16_t  value_offset;
    uint16_t  next_offset;
    uint16_t  node_size;

    kvblock_t *block;
};

struct kvset {
    kvblock_t   *blocks;
    int          num;
    int          bid; // current block_id
    int          gc_bid;
    int          handle;
    uint8_t     *mem;
    flash_ops_t *ops;
};

int kv_init(kv_t *kv, uint8_t *mem, int block_num, int block_size);
int kv_reset(kv_t *kv);
int kv_set(kv_t *kv, const char *key, void *value, int size);
int kv_get(kv_t *kv, const char *key, void *value, int size);
int kv_rm(kv_t *kv, const char *key);
int kv_find(kv_t *kv, const char *key, kvnode_t *node);
int kv_iter(kv_t *kv, int (*fn)(kvnode_t *, void *), void *data);

int  kv_gc(kv_t *kv);
void kv_dump(kv_t *kv);
void kv_show_data(kv_t *kv);

#define KVNODE_OFFSET2CACHE(kv_node, kv_offset) ((kv_node)->block->mem_cache + (kv_node)->kv_offset)
#define KVNODE_CACHE2OFFSET(kv_node, mem_addr) (mem_addr - (kv_node)->block->mem_cache)
#ifdef __cplusplus
}
#endif

#endif
