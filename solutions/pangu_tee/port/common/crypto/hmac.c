/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <crypto.h>
#include <hmac.h>

/*load HMAC key from TEE internal*/
static const char *hmac_load_key(unsigned int *key_size)
{
    const char *hmac_test_key = "this is a hmac key";
    *key_size = strlen(hmac_test_key);
    return hmac_test_key;
}

int hmac_init(hmac_context_t *hmac_ctx, unsigned char type)
{
    memset(hmac_ctx, 0, sizeof(hmac_context_t));

    if (type == HMAC_MD5) {
        hmac_ctx->hash_type = MD5;
        hmac_ctx->hash_size = HMAC_MD5_SIZE;
    } else if (type == HMAC_SHA1) {
        hmac_ctx->hash_type = SHA1;
        hmac_ctx->hash_size = HMAC_SHA1_SIZE;
        hmac_ctx->ctx.hashctx = hmac_ctx->sha_ctx;
        tee_hash_init(SHA1, &hmac_ctx->ctx);
    }

    return 0;
}

int hmac_start(hmac_context_t *hmac_ctx, const unsigned char *key, unsigned int key_len)
{
    int i;
    unsigned char tmp[HMAC_SHA1_SIZE];

    hmac_context_t *ctx = hmac_ctx;

    if (!ctx) {
        return -1;
    }

    /*if key is NULL,load key from TEE internal*/
    if (!key) {
        key = (const unsigned char *)hmac_load_key(&key_len);
    }

    if (key_len > HMAC_BLOCK_SIZE) {
        tee_hash_init(ctx->hash_type, &ctx->ctx);
        tee_hash_update(key, key_len, &ctx->ctx);
        tee_hash_final(tmp, &ctx->ctx);
        key_len = ctx->hash_size;
        key = tmp;
    }

    memset(ctx->ipad, 0x36, HMAC_BLOCK_SIZE);
    memset(ctx->opad, 0x5C, HMAC_BLOCK_SIZE);

    for (i = 0; i < key_len; i++) {
        ctx->ipad[i] = (unsigned char)(ctx->ipad[i] ^ key[i]);
        ctx->opad[i] = (unsigned char)(ctx->opad[i] ^ key[i]);
    }

    memset(tmp, 0, sizeof(tmp));

    tee_hash_init(ctx->hash_type, &ctx->ctx);
    tee_hash_update(ctx->ipad, HMAC_BLOCK_SIZE, &ctx->ctx);
    return 0;
}

int hmac_update(hmac_context_t *hmac_ctx, const unsigned char *input, unsigned int input_len)
{
    hmac_context_t *ctx = (hmac_context_t *)hmac_ctx;

    if (!ctx || !input || !input_len) {
        return -1;
    }

    tee_hash_update(input, input_len, &ctx->ctx);
    return 0;
}

int hmac_finish(hmac_context_t *hmac_ctx, unsigned char *output)
{
    unsigned char tmp[HMAC_SHA1_SIZE];
    hmac_context_t *ctx = (hmac_context_t *)hmac_ctx;

    if (!ctx || !output) {
        return -1;
    }

    tee_hash_final(tmp, &ctx->ctx);
    tee_hash_init(ctx->hash_type, &ctx->ctx);
    tee_hash_update(ctx->opad, HMAC_BLOCK_SIZE, &ctx->ctx);
    tee_hash_update(tmp, ctx->hash_size, &ctx->ctx);
    tee_hash_final(output, &ctx->ctx);
    return 0;
}

