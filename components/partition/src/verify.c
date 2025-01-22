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
#include "verify_wrapper.h"
#include "crc32.h"
#include <yoc/partition_device.h>
#include "yoc/partition.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mtb_log.h"
#if defined(CONFIG_WITH_SE)
#include <se_rsa.h>
#include <se_keystore.h>
#endif

//#define DEBUG_WITH_MBEDTLS

#ifdef DEBUG_WITH_MBEDTLS
#include "sha1.h"
#include "sha256.h"
#endif

#ifdef CONFIG_NON_ADDRESS_FLASH
#undef CONFIG_SHA_UPDATE_ONCE
#endif

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

int get_length_with_digest_type(digest_sch_e type)
{
    switch (type) {
        default:
        case DIGEST_HASH_NONE:
            break;
        case DIGEST_HASH_SHA1:
            return 20;
        case DIGEST_HASH_MD5:
            return 16;
        case DIGEST_HASH_SHA224:
            return 28;
        case DIGEST_HASH_SHA256:
            return 32;
        case DIGEST_HASH_SHA384:
            return 48;
        case DIGEST_HASH_SHA512:
            return 64;
    }
    return 0;
}

int get_length_with_signature_type(signature_sch_e type)
{
    switch (type) {
        default:
        case SIGNATURE_NONE:
            break;
        case SIGNATURE_RSA_1024:
            return 128;
        case SIGNATURE_RSA_2048:
            return 256;
        case SIGNATURE_ECC_160:
            return 40;
    }

    return 0;
}

#if defined(CONFIG_COMP_SEC_CRYPTO) && CONFIG_COMP_SEC_CRYPTO

static int copy_data(void *dst, void *src, size_t size, int from_mem, partition_info_t *part_info)
{
    int ret = 0;

    if (from_mem) {
        memcpy(dst, src, size);
        return ret;
    }

#ifdef CONFIG_NON_ADDRESS_FLASH
    if (part_info == NULL) {
        return -EINVAL;
    }
    void *handle = partition_device_find(&part_info->storage_info);
    ret = partition_device_read(handle, (unsigned long)src - part_info->base_addr, dst, size);
#else
    memcpy(dst, src, size);
#endif /* CONFIG_NON_ADDRESS_FLASH */

    return ret;
}

sha_context_t* sha_init(digest_sch_e ds, partition_info_t *part_info)
{
    sha_context_t *sha_ctx = NULL;
#if defined(CONFIG_KERNEL_NONE)
    static sha_context_t g_sha_ctx;
    sha_ctx = &g_sha_ctx;
#else
    sha_ctx = malloc(sizeof(sha_context_t));

    if (sha_ctx == NULL) {
        return NULL;
    }
#endif
    memset(sha_ctx, 0, sizeof(sha_context_t));
    sc_sha_init(&sha_ctx->sc_sha, 0);
    sha_ctx->ds_type = ds;
    sha_ctx->priv = part_info;

    return sha_ctx;
}

int sha_start(sha_context_t *ctx)
{
    sc_sha_mode_t mode = SC_SHA_MODE_1;
    switch (ctx->ds_type)
    {
    default:
    case DIGEST_HASH_SHA1:
        mode = SC_SHA_MODE_1;
        break;
    case DIGEST_HASH_SHA256:
        mode = SC_SHA_MODE_256;
        break;
    }
    if (sc_sha_start(&ctx->sc_sha, &ctx->sc_ctx, mode) != SC_OK) {
        return -1;
    }
    return 0;
}

int sha_update(sha_context_t *ctx, const void *input, uint32_t ilen, int from_mem)
{
    uint32_t ret;
#ifndef CONFIG_SHA_UPDATE_ONCE
    uint8_t temp_buffer[1024] __attribute__((aligned(4)));
    uint8_t *p_addr = (uint8_t *)input;
    uint32_t image_size = ilen;
#endif

#ifdef CONFIG_SHA_UPDATE_ONCE
    ret = sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, input, ilen);
    if (ret != SC_OK) {
        return -1;
    }
#else
    if (image_size > sizeof(temp_buffer)) {
        if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, ctx->priv)) {
            return -1;
        }

        ret = sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, temp_buffer, sizeof(temp_buffer));
        if (ret != SC_OK) {
            return -1;
        }
        p_addr += sizeof(temp_buffer);
        image_size -= sizeof(temp_buffer);

        while (image_size > sizeof(temp_buffer)) {
            if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, ctx->priv)) {
                return -1;
            }
            ret = sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, temp_buffer, sizeof(temp_buffer));
            if (ret != SC_OK) {
                return -1;
            }
            p_addr += sizeof(temp_buffer);
            image_size -= sizeof(temp_buffer);
        }
        if (copy_data(temp_buffer, p_addr, image_size, from_mem, ctx->priv)) {
            return -1;
        }
        ret = sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, temp_buffer, image_size);
        if (ret != SC_OK) {
            return -1;
        }
    } else {
        if (copy_data(temp_buffer, p_addr, image_size, from_mem, ctx->priv)) {
            return -1;
        }
        ret = sc_sha_update(&ctx->sc_sha, &ctx->sc_ctx, temp_buffer, image_size);
        if (ret != SC_OK) {
            return -1;
        }
    }
#endif /* CONFIG_SHA_UPDATE_ONCE */

    return 0;
}

int sha_finish(sha_context_t *ctx, void *output, uint32_t *olen)
{
    if (sc_sha_finish(&ctx->sc_sha, &ctx->sc_ctx, output, olen) != SC_OK) {
        return -1;
    }
    return 0;
}

int sha_deinit(sha_context_t *ctx)
{
    if (ctx == NULL) {
        return -1;
    }

    sc_sha_uninit(&ctx->sc_sha);
#if !defined(CONFIG_KERNEL_NONE)
    free(ctx);
#endif
    ctx = NULL;
    return 0;
}

__attribute__((weak)) int hash_calc_start(digest_sch_e ds, const unsigned char *input, int ilen,
                                          unsigned char *output, uint32_t *olen, int from_mem,
                                          partition_info_t *part_info)
{
#ifndef DEBUG_WITH_MBEDTLS
    int ret;
    sha_context_t *ctx;

    MTB_LOGD("ds:%d, ilen:%d, from_mem:%d", ds, ilen, from_mem);
    if (ds >= DIGEST_HASH_MAX || !(input && ilen > 0 && output && olen)) {
        return -EINVAL;
    }

    ctx = sha_init(ds, part_info);
    if (ctx) {
        ret = sha_start(ctx);
        if (ret != 0) {
            MTB_LOGE("hash start e.");
            return -1;
        }
        ret = sha_update(ctx, input, ilen, from_mem);
        if (ret != 0) {
            MTB_LOGE("hash update e.");
            return -1;
        }
        ret = sha_finish(ctx, output, olen);
        if (ret != 0) {
            MTB_LOGE("hash finish e.");
            return -1;
        }
        ret = sha_deinit(ctx);
        if (ret != 0) {
            MTB_LOGE("hash deinit e.");
            return -1;
        }
        MTB_LOGD("sha ok, type:%d, outlen:%d", ds, *olen);
        return 0;
    }
    MTB_LOGE("hash e.");
    return -1;

#else
    // for test
#ifdef CONFIG_SHA1_UPDATE_ONCE
    if (ds == DIGEST_HASH_SHA1) {
        mbedtls_sha1(input, in_length, output);
        *olen = 20;
    } else if (ds == DIGEST_HASH_SHA256) {
        mbedtls_sha256(input, in_length, output, 0);
        *olen = 32;
    } else {
        MTB_LOGE("digest type e[%d]", type);
        return -1;
    }
#else
    uint8_t temp_buffer[1024] __attribute__((aligned(4)));
    uint8_t *p_addr = (uint8_t *)input;
    uint32_t image_size = ilen;
    if (ds == DIGEST_HASH_SHA1) {
        mbedtls_sha1_context ctx;

        mbedtls_sha1_init(&ctx);
        mbedtls_sha1_starts(&ctx);
        if (image_size > sizeof(temp_buffer)) {
            if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, part_info)) {
                return -1;
            }

            mbedtls_sha1_update(&ctx, temp_buffer, sizeof(temp_buffer));
            p_addr += sizeof(temp_buffer);
            image_size -= sizeof(temp_buffer);

            while (image_size > sizeof(temp_buffer)) {
                if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, part_info)) {
                    return -1;
                }

                mbedtls_sha1_update(&ctx, temp_buffer, sizeof(temp_buffer));
                p_addr += sizeof(temp_buffer);
                image_size -= sizeof(temp_buffer);
            }

            if (copy_data(temp_buffer, p_addr, image_size, from_mem, part_info)) {
                return -1;
            }
            mbedtls_sha1_update(&ctx, temp_buffer, image_size);
        } else {
            if (copy_data(temp_buffer, p_addr, image_size, from_mem, part_info)) {
                return -1;
            }
            mbedtls_sha1_update(&ctx, temp_buffer, image_size);
        }
        mbedtls_sha1_finish(&ctx, output);
        mbedtls_sha1_free(&ctx);
        *olen = 20;
    }
    else if (ds == DIGEST_HASH_SHA256) {
        mbedtls_sha256_context ctx;

        mbedtls_sha256_init(&ctx);
        mbedtls_sha256_starts(&ctx, 0);
        if (image_size > sizeof(temp_buffer)) {
            if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, part_info)) {
                return -1;
            }

            mbedtls_sha256_update(&ctx, temp_buffer, sizeof(temp_buffer));
            p_addr += sizeof(temp_buffer);
            image_size -= sizeof(temp_buffer);

            while (image_size > sizeof(temp_buffer)) {
                if (copy_data(temp_buffer, p_addr, sizeof(temp_buffer), from_mem, part_info)) {
                    return -1;
                }

                mbedtls_sha256_update(&ctx, temp_buffer, sizeof(temp_buffer));
                p_addr += sizeof(temp_buffer);
                image_size -= sizeof(temp_buffer);
            }

            if (copy_data(temp_buffer, p_addr, image_size, from_mem, part_info)) {
                return -1;
            }
            mbedtls_sha256_update(&ctx, temp_buffer, image_size);
        } else {
            if (copy_data(temp_buffer, p_addr, image_size, from_mem, part_info)) {
                return -1;
            }
            mbedtls_sha256_update(&ctx, temp_buffer, image_size);
        }
        mbedtls_sha256_finish(&ctx, output);
        mbedtls_sha256_free(&ctx);
        *olen = 32;
    }
    else {
        MTB_LOGE("digest type e[%d]", ds);
        return -1;
    }
#endif /* CONFIG_SHA1_UPDATE_ONCE */
    return 0;
#endif
}

#if (CONFIG_PARITION_NO_VERIFY == 0)
#include <sec_crypto_rsa.h>
typedef struct {
    sc_rsa_t sc_rsa;
    sc_rsa_context_t sc_ctx;
} rsa_context_t;

#if defined(CONFIG_WITH_SE)
#else
static uint8_t g_key_module_trustboot[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
};
#endif

__attribute__((weak)) int signature_verify_start(digest_sch_e ds, signature_sch_e ss,
                                                uint8_t *key_buf, int key_size,
                                                const uint8_t *src, int src_size,
                                                const uint8_t *signature, int sig_size)
{
#if defined(CONFIG_WITH_SE)
    int ret = 0;
    se_rsa_t rsa;
    se_rsa_context_t context;
	se_rsa_hash_type_t hash_type;
	uint32_t key_index;

    MTB_LOGD("ds:%d, ss:%d, src_size:%d, sig_size:%d", ds, ss, src_size, sig_size);
    if (ds >= DIGEST_HASH_MAX || ss >= SIGNATURE_MAX || !(key_buf && key_size > 0 && src && src_size > 0 && signature && sig_size > 0)) {
        MTB_LOGE("signature verify val e.");
        return -EINVAL;
    }

    switch (ds)
    {
    default:
    case DIGEST_HASH_NONE:
        return 0;
    case DIGEST_HASH_SHA1:
        hash_type = SE_RSA_HASH_TYPE_SHA1;
        break;
    case DIGEST_HASH_SHA256:
        hash_type = SE_RSA_HASH_TYPE_SHA256;
        break;
    }

    switch (ss)
    {
    default:
    case SIGNATURE_NONE:
        return 0;
    case SIGNATURE_RSA_1024:
        context.key_bits = SE_RSA_KEY_BITS_1024;
		key_index = SE_RSA_1024_KEY_SBOOT;
        break;
    case SIGNATURE_RSA_2048:
        context.key_bits = SE_RSA_KEY_BITS_2048;
		key_index = SE_RSA_2048_KEY_SBOOT;
        break;
    }
	context.key_bits = SE_RSA_KEY_BITS_1024;
	context.padding_type = SE_RSA_PADDING_MODE_PKCS1;

    ret = se_rsa_init(&rsa);
	if (ret)
		return -1;
    if (se_rsa_verify(&rsa, &context, (void*)src, src_size, (void *)signature, hash_type, key_index)) {
        MTB_LOGD("sc verify ok..");
        ret = 0;
    } else {
        MTB_LOGE("rsa verify failed..");
        ret = -1;
    }

    se_rsa_uninit(&rsa);
#else
    int      ret;
    uint8_t *key_n = key_buf;
    uint8_t *key_e;
    sc_rsa_hash_type_t hash_type;
    sc_rsa_key_bits_t key_bits;
    rsa_context_t ctx;
    uint32_t bits = 1024;

    MTB_LOGD("ds:%d, ss:%d, src_size:%d, sig_size:%d", ds, ss, src_size, sig_size);
    if (ds >= DIGEST_HASH_MAX || ss >= SIGNATURE_MAX || !(key_buf && key_size > 0 && src && src_size > 0 && signature && sig_size > 0)) {
        MTB_LOGE("signature verify val e.");
        return -EINVAL;
    }

    switch (ds)
    {
    default:
    case DIGEST_HASH_NONE:
        return 0;
    case DIGEST_HASH_SHA1:
        hash_type = SC_RSA_HASH_TYPE_SHA1;
        break;
    case DIGEST_HASH_SHA256:
        hash_type = SC_RSA_HASH_TYPE_SHA256;
        break;
    }

    switch (ss)
    {
    default:
    case SIGNATURE_NONE:
        return 0;
    case SIGNATURE_RSA_1024:
        key_bits = SC_RSA_KEY_BITS_1024;
        bits = 1024;
        key_e = &g_key_module_trustboot[128];
        break;
    case SIGNATURE_RSA_2048:
        key_bits = SC_RSA_KEY_BITS_2048;
        bits = 2048;
        key_e = &g_key_module_trustboot[0];
        break;
    }

    sc_rsa_init(&ctx.sc_rsa, 0, key_bits);
    memset(&ctx.sc_ctx, 0, sizeof(sc_rsa_context_t));

    ctx.sc_ctx.key_bits     = bits;
    ctx.sc_ctx.padding_type = SC_RSA_PADDING_MODE_PKCS1;
    ctx.sc_ctx.n            = key_n;
    ctx.sc_ctx.e            = key_e;
    ctx.sc_ctx.hash_type    = hash_type;

    MTB_LOGD("rsa verify start..ds:%d, ss:%d", ds, ss);
    // dump_data((uint8_t *)key_buf, key_size);
    MTB_LOGD("key_size==%d,src_size==%d, sig_size==%d", key_size, src_size, sig_size);
    if (sc_rsa_verify(&ctx.sc_rsa, &ctx.sc_ctx, (void *)src, src_size, (void *)signature, sig_size, hash_type)) {
        MTB_LOGD("sc verify ok..");
        ret = 0;
    } else {
        MTB_LOGE("rsa verify failed..");
        ret = -1;
    }

    sc_rsa_uninit(&ctx.sc_rsa);
#endif
    return ret;
}

#else

__attribute__((weak)) int signature_verify_start(digest_sch_e ds, signature_sch_e ss,
                            uint8_t *key_buf, int key_size,
                            const uint8_t *src, int src_size,
                            const uint8_t *signature, int sig_size)
{
    MTB_LOGD("rsa verify do nothing.");
    return 0;
}

#endif /* CONFIG_PARITION_NO_VERIFY */

#endif /* CONFIG_COMP_SEC_CRYPTO */

__attribute__((weak)) int crc32_calc_start(const uint8_t *input, uint32_t ilen, uint32_t *output)
{
    if (input && ilen > 0 && output) {
        *output = pcrc32(0, (uint8_t *)input, ilen);
        return 0;
    }

    return -1;
}
