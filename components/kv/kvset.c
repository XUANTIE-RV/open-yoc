/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "kvset.h"
#include "block.h"

static void kv_verify(kv_t *kv);

int kv_init(kv_t *kv, uint8_t *mem, int block_num, int block_size)
{
    int gc_size = 0;
    kv->num    = block_num;
    kv->blocks = calloc(kv->num, sizeof(kvblock_t));
    if (kv->blocks == NULL) {
        return -1;
    }
    kv->bid    = 0;
    kv->gc_bid = -1;

    for (int i = 0; i < block_num; i++) {
        kv->blocks[i].kv = kv;
        kv->blocks[i].id = i;
        kvblock_init(kv->blocks + i, mem + i * block_size, block_size);

        int v = kvblock_free_size(kv->blocks + i);
        if (v >= gc_size && v > 4) {
            gc_size = v;
            kv->gc_bid = i;
        } else if (kv->bid == 0)
            kv->bid = i;
    }

    // printf("gc_bid = %d, bid = %d\n", kv->gc_bid, kv->bid);
    kv_verify(kv);

    return 0;
}

extern int block_read(kvblock_t *block, int offset, void *data, int size);
static void kv_verify(kv_t *kv)
{
    for (int i = 0; i < kv->num; i++) {
        kvnode_t node_1;

        /* load block 1 */
        kvblock_t *block_1 = &kv->blocks[i];
        kvblock_cache_malloc(block_1);

        uint8_t *next = block_1->mem_cache;
        while (kvblock_search(block_1, next, &node_1) == 0) {
            if (NODE_VAILD(&node_1)) {
                for (int j = i; j < kv->num; j++) {
                    kvnode_t node_2;

                    /* load block 2 */
                    kvblock_t *block_2 = &kv->blocks[j];
                    kvblock_cache_malloc(block_2);

                    uint8_t *next = j == i ? KVNODE_OFFSET2CACHE(&node_1, next_offset) :block_2->mem_cache;
                    while (kvblock_search(block_2, next, &node_2) == 0) {
                        if (NODE_VAILD(&node_2)) {
                            if (kvblock_check_version(&node_1, &node_2) == &node_1) {

                                /* free block 2 */
                                kvblock_cache_free(block_2);
                                goto out;
                            }
                        }
                        next = KVNODE_OFFSET2CACHE(&node_2, next_offset);
                    } /* while block2 */

                    /* free block 2 */
                    kvblock_cache_free(block_2);
                }
            }
        out:
            next = KVNODE_OFFSET2CACHE(&node_1, next_offset);
        }/* while block1 */

        /* free block 1 */
        kvblock_cache_free(block_1);
    }
}

int kv_iter(kv_t *kv, int (*fn)(kvnode_t *, void *), void *data)
{
    int ret = -1;
    for (int i = 0; i < kv->num; i++) {
        ret = kvblock_iter(kv->blocks + i, NODE_EXISTS, fn, data);
        if (ret != 0)
            break;
    }

    return ret;
}

int kv_find(kv_t *kv, const char *key, kvnode_t *node)
{
    int found = -1;
    for (int i = 0; i < kv->num; i++) {
        if (kvblock_find(kv->blocks + i, key, node) == 0){
            found = 0;
            if (node->rw != 0)
                return 0;
        }
    }

    return found;
}

static int _kvblock_deep_gc(kvnode_t *node, void *data)
{
    kvblock_t *block = (kvblock_t *)data;

    if (node->rw == 1) {
        int version = node->version == 255 ? 1 : node->version + 1;

        if (kvblock_set(block, (const char *)KVNODE_OFFSET2CACHE(node, head_offset), KVNODE_OFFSET2CACHE(node, value_offset), node->val_size, version) >= 0)
            kvnode_rm(node);
    }

    return 0;
}

// 将从最小使用的块开始，将所有可以放到整理块的块，都放到整理块中
int kv_gc(kv_t *kv)
{
    int new_gc_id = -1;

    while (1) {
        int min_id   = -1;
        int min_size = kv->blocks[kv->gc_bid].size - kv->blocks[kv->gc_bid].write_offset;

        // get min used block
        for (int i = 0; i < kv->num; i++) {
            if (kv->blocks[i].ro_count == 0 && i != kv->gc_bid && \
                    kv->blocks[i].kv_size > 0 && kv->blocks[i].kv_size < min_size) {
                min_size = kv->blocks[i].kv_size;
                min_id   = i;
            }
        }

        if (min_id != -1) {
            kvblock_iter(kv->blocks + min_id, 1, _kvblock_deep_gc, kv->blocks + kv->gc_bid);
            if (kv->blocks[min_id].count == 0)
                new_gc_id = min_id;
        } else {
            break;
        }
    }

    if (new_gc_id != -1) {
        kv->gc_bid = new_gc_id;

        return 0;
    }

    return -1;
}

int kv_set(kv_t *kv, const char *key, void *value, int size)
{
    if (key == NULL || value == NULL || size <= 0 || strlen(key) + size >= 498)
        return -1;

    kvnode_t node;
    int      kv_exist;
    int      version  = 0;
    int      gc_count = 0;
    int      start_bid;

start1:
    kv_exist = kv_find(kv, key, &node) == 0;
    /* kvnode rm no mem opt, ignore call kvblock_cache_malloc */

    if (kv_exist) {
        // if (node.size == size && memcmp(node.value, value, size) == 0)
        //     return size;
        version = node.version;
        if (version == 255)
            version = 0;
    }
    version++;

start2:
    start_bid = kv->bid;
    while (1) {
        if (kv->blocks[kv->bid].ro_count == 0 && kv->bid != kv->gc_bid &&
            kvblock_set(kv->blocks + kv->bid, key, value, size, version) >= 0) {
            if (kv_exist)
                kvnode_rm(&node);

            return size;
        }

        kv->bid = (kv->bid + 1) % kv->num;
        if (kv->bid == start_bid)
            break;
    }

    if (gc_count == 0 && kv_gc(kv) == 0) {
        gc_count = 1;
        if (kv_exist)
            goto start1;
        else
            goto start2;
    }

    return -1;
}

int kv_get(kv_t *kv, const char *key, void *value, int size)
{
    kvnode_t node;

    if (kv_find(kv, key, &node) == 0) {

        /* kv find result node, block mem_cache maybe NULL*/
        kvblock_cache_malloc(node.block);

        uint8_t* node_value = KVNODE_OFFSET2CACHE(&node, value_offset);
        if (node.rw) {
            //rw normal node
            memcpy(value, node_value, node.val_size < size ? node.val_size : size);

            kvblock_cache_free(node.block);
            return node.val_size;
        } else {
            //ro node, string mode, key1="123", key2=123
            if (node.val_size > 0) {

                int ret = node.val_size;
                if (node_value[0] == '\"') {
                    //string value
                    if (node_value[ret - 1] == '\r') {
                        ret --;
                    }

                    if (node_value[ret - 1] == '\"') {
                        ret --;
                    }

                    ret --; /* del first " */
                    memcpy(value, node_value + 1, ret < size ? ret : size);

                    kvblock_cache_free(node.block);
                    return ret;

                } else {
                    //number
                    int num = atoi((char *)node_value);
                    memcpy(value, &num, 4 < size ? 4 : size);

                    kvblock_cache_free(node.block);
                    return 4;
                }
            } /* val_size > 0 */
        }/* else rw */
    }/* kv find */
    return -1;
}

int kv_rm(kv_t *kv, const char *key)
{
    kvnode_t node;
    int ret = kv_find(kv, key, &node);

    /* kvnode rm no mem opt, ignore call kvblock_cache_malloc */
    if (ret == 0)
        kvnode_rm(&node);

    return ret;
}

int kv_reset(kv_t *kv)
{
    for (int i = 0; i < kv->num; i++)
        kvblock_reset(kv->blocks + i);

    return 0;
}

void kv_dump(kv_t *kv)
{
    printf("bid=%d, gc_id=%d\n", kv->bid, kv->gc_bid);
    for (int i = 0; i < kv->num; i++) {
        printf("block id: %d, diry_size = %d, kv_size = %d, count = %d\n", i,
               kv->blocks[i].dirty_size, kv->blocks[i].kv_size, kv->blocks[i].count);
        kvblock_dump(kv->blocks + i);
    }
}

void kv_show_data(kv_t *kv)
{
    for (int i = 0; i < kv->num; i++) {
        kvblock_show_data(kv->blocks + i, 8);

        printf("\n");
    }
}
