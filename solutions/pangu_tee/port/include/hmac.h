/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef HMAC_H
#define HMAC_H

#define HMAC_BLOCK_SIZE 64
#define HMAC_MD5_SIZE   16
#define HMAC_SHA1_SIZE  20

enum {
    HMAC_MD5 = 0,
    HMAC_SHA1 = 1,
};

typedef struct {
    unsigned char ipad[HMAC_BLOCK_SIZE];
    unsigned char opad[HMAC_BLOCK_SIZE];
    unsigned char hash_type;
    unsigned char hash_size;
    hash_context ctx;
    unsigned char sha_ctx[224];
} hmac_context_t;

int hmac_init(hmac_context_t *hmac_ctx, unsigned char type);

int hmac_start(hmac_context_t *hmac_ctx, const unsigned char *key, unsigned int key_len);

int hmac_update(hmac_context_t *hmac_ctx, const unsigned char *input, unsigned int input_len);

int hmac_finish(hmac_context_t *hmac_ctx, unsigned char *output);

#endif
