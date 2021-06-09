/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#ifndef _RINGBLK_BUF_H_
#define _RINGBLK_BUF_H_

/*
 * Introduction:
 * The rbb is the ring buffer which is composed with many blocks. It is different from the ring buffer.
 * The ring buffer is only composed with chars. The rbb put and get supported zero copies. So the rbb
 * is very suitable for put block and get block by a certain order. Such as DMA block transmit,
 * communicate frame send/recv, and so on.
 */

#include <stdint.h>
#include <stddef.h>
#include <aos/list.h>

#ifdef __cplusplus
extern "C" {
#endif

enum rbb_status {
    /* unused status when first initialize or after blk_free() */
    RBB_BLK_UNUSED,
    /* initialized status after blk_alloc() */
    RBB_BLK_INITED,
    /* put status after blk_put() */
    RBB_BLK_PUT,
    /* get status after blk_get() */
    RBB_BLK_GET,
};
typedef enum rbb_status rbb_status_t;

/**
 * the block of rbb
 */
struct rbb_blk {
    rbb_status_t status :8;
    /* less then 2^24 */
    size_t size :24;
    uint8_t *buf;
    slist_t list;
};
typedef struct rbb_blk *rbb_blk_t;

/**
 * Rbb block queue: the blocks (from block1->buf to blockn->buf) memory which on this queue is continuous.
 */
struct rbb_blk_queue {
    rbb_blk_t blocks;
    size_t blk_num;
};
typedef struct rbb_blk_queue *rbb_blk_queue_t;

/**
 * ring block buffer
 */
struct _rbb {
    uint8_t *buf;
    size_t buf_size;
    /* all of blocks */
    rbb_blk_t blk_set;
    size_t blk_max_num;
    /* saved the initialized and put status blocks */
    slist_t blk_list;
};
typedef struct _rbb *rbb_t;

/* rbb (ring block buffer) API */
void rbb_init(rbb_t rbb, uint8_t *buf, size_t buf_size, rbb_blk_t block_set, size_t blk_max_num);
rbb_t rbb_create(size_t buf_size, size_t blk_max_num);
void rbb_destroy(rbb_t rbb);
size_t rbb_get_buf_size(rbb_t rbb);

/* rbb block API */
rbb_blk_t rbb_blk_alloc(rbb_t rbb, size_t blk_size);
void rbb_blk_put(rbb_blk_t block);
rbb_blk_t rbb_blk_get(rbb_t rbb);
size_t rbb_blk_size(rbb_blk_t block);
uint8_t *rbb_blk_buf(rbb_blk_t block);
void rbb_blk_free(rbb_t rbb, rbb_blk_t block);

/* rbb block queue API */
size_t rbb_blk_queue_get(rbb_t rbb, size_t queue_data_len, rbb_blk_queue_t blk_queue);
size_t rbb_blk_queue_len(rbb_blk_queue_t blk_queue);
uint8_t *rbb_blk_queue_buf(rbb_blk_queue_t blk_queue);
void rbb_blk_queue_free(rbb_t rbb, rbb_blk_queue_t blk_queue);
size_t rbb_next_blk_queue_len(rbb_t rbb);


#ifdef __cplusplus
}
#endif

#endif /* _RINGBLK_BUF_H_ */
