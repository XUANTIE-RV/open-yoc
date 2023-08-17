/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <string.h>
#include "yoc/fota.h"
#include <ulog/ulog.h>
#include <aos/debug.h>
#if CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK == 0
#include <key_mgr.h>
#endif

#define TAG "fotav"

#ifndef __linux__
#include <yoc/partition.h>
#include <verify.h>
#include <verify_wrapper.h>

#define DUMP_DATA_EN 0

typedef struct {
    uint32_t magic;
    uint8_t ver;
    uint8_t image_sum;
    uint8_t digest_type;
    uint8_t signature_type;
    uint32_t mnft_off;
    uint8_t head_len;
    uint8_t pad_type;
    uint8_t rsvd[2];
} fota_head_info_t;

#if DUMP_DATA_EN
static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}
#endif

#if defined(CONFIG_FOTA_DATA_IN_RAM) && CONFIG_FOTA_DATA_IN_RAM > 0
/**
 * @brief  获取存储fota数据的ram地址
 * @return address
 */
__attribute__((weak)) unsigned long fota_data_address_get(void)
{
    return 0;
}
#endif

#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <stdio.h>
#include <yoc/ota_ab_img.h>
#include <bootab.h>
#include <mbedtls/md5.h>

extern int images_info_init(uint8_t *header_buffer, download_img_info_t *dl_img_info);

#if CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK != 0
static int __read_partition(const char *name, off_t offset, void *buffer, size_t size)
{
    int ret;

    partition_t p = partition_open(name);
    ret = partition_read(p, offset, buffer, size);
    partition_close(p);
    if (ret < 0) {
        LOGE(TAG, "read flash error, ret:%d, %s, %d", ret, name, p);
    }
    return ret;
}
#endif /* CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK */

__attribute__((weak)) int fota_data_verify(void)
{
    int i;
    int ret;
    uint8_t *temp_buffer = NULL;
    download_img_info_t *dl_img_info = NULL;

    LOGD(TAG, "come to image verify.");
    partition_t handle = partition_open(B_ENVAB_NAME);
    if (handle < 0) {
        return -1;
    }
    partition_info_t *lp = partition_info_get(handle);
    aos_assert(lp);
    temp_buffer = aos_zalloc(sizeof(pack_header_v2_t));
    if (temp_buffer == NULL) {
        return -ENOMEM;
    }
    ret = partition_read(handle, OTA_AB_IMG_INFO_OFFSET_GET(lp->erase_size), temp_buffer, sizeof(pack_header_v2_t));
    partition_close(handle);
    if (ret < 0) {
        return ret;
    }
    dl_img_info = aos_zalloc(sizeof(download_img_info_t));
    if (dl_img_info == NULL) {
        return -ENOMEM;
    }
    ret = images_info_init(temp_buffer, dl_img_info);
    if (ret < 0) {
        goto errout;
    }

    LOGD(TAG, "dl_img_info->image_count:%d", dl_img_info->image_count);
    for (i = 0; i < dl_img_info->image_count; i++) {
        LOGD(TAG, "%s, size: %d", dl_img_info->img_info[i].partition_name, dl_img_info->img_info[i].img_size);
    }
    LOGD(TAG, "dl_img_info->digest_type:%d", dl_img_info->digest_type);
#if CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK == 0
    if (dl_img_info->digest_type > 0) {
        uint32_t olen;
        uint8_t hash_out[128];
        sha_context_t *ctx;

        LOGD(TAG, "come to verify signature.");
        if (dl_img_info->digest_type >= DIGEST_HASH_MAX) {
            LOGE(TAG, "the digest type %d is error", dl_img_info->digest_type);
            ret = -1;
            goto errout;
        }
        ctx = sha_init(dl_img_info->digest_type, lp);
        if (!ctx) {
            ret = -1;
            goto errout;
        }
        ret = sha_start(ctx);
        if (ret != 0) {
            LOGE(TAG, "hash start e.");
            ret = -1;
            goto errout;
        }
        // SHA header first
        memset(((pack_header_v2_t *)temp_buffer)->signature, 0, sizeof(((pack_header_v2_t *)temp_buffer)->signature));
        ret = sha_update(ctx, temp_buffer, sizeof(pack_header_v2_t), 1);
        if (ret != 0) {
            LOGE(TAG, "hash update e.");
            ret = -1;
            goto errout;
        }
        for (i = 0; i < dl_img_info->image_count; i++) {
            ret = sha_update(ctx, (const void *)dl_img_info->img_info[i].partition_abs_addr, dl_img_info->img_info[i].img_size, 0);
            if (ret != 0) {
                LOGE(TAG, "hash update e.");
                ret = -1;
                goto errout;
            }
        }
        ret = sha_finish(ctx, hash_out, &olen);
        if (ret != 0) {
            LOGE(TAG, "hash finish e.");
            ret = -1;
            goto errout;
        }
        ret = sha_deinit(ctx);
        if (ret != 0) {
            LOGE(TAG, "hash deinit e.");
            ret = -1;
            goto errout;
        }
        LOGD(TAG, "sha ok, type:%d, outlen:%d", dl_img_info->digest_type, olen);
        int hash_len = get_length_with_digest_type(dl_img_info->digest_type);
        int signature_len = get_length_with_signature_type(dl_img_info->signature_type);
        if (hash_len != olen) {
            LOGE(TAG, "hash length is not matched.");
            ret = -1;
            goto errout;
        }
        key_handle key_addr;
        uint32_t key_size;
        if (km_get_pub_key_by_name(DEFAULT_PUBLIC_KEY_NAME_IN_OTP, &key_addr, &key_size) == KM_OK) {
            ret = signature_verify_start(dl_img_info->digest_type, dl_img_info->signature_type,
                                        (uint8_t *)key_addr, key_size,
                                        hash_out, hash_len,
                                        dl_img_info->signature, signature_len);
            if (ret != 0) {
                LOGE(TAG, "signature verify failed.");
                goto errout;
            }
        } else {
            LOGE(TAG, "there is no pubkey found!");
            ret = -1;
            goto errout;
        }
    } else {
        LOGE(TAG, "signature error, %d", dl_img_info->digest_type);
        ret = -1;
        goto errout;
    }
#else
    {
#define TEMP_BUFFER_SIZE 8192
        off_t offset;
        uint8_t md5_out[16];
        mbedtls_md5_context md5;

        LOGD(TAG, "come to MD5 verify.");
        aos_free(temp_buffer);
        temp_buffer = aos_malloc(TEMP_BUFFER_SIZE);
        if (temp_buffer == NULL) {
            LOGE(TAG, "no mem");
            ret = -ENOMEM;
            goto errout;
        }
        mbedtls_md5_init(&md5);
        mbedtls_md5_starts(&md5);
        for (i = 0; i < dl_img_info->image_count; i++) {
            offset = 0;
            int image_size = dl_img_info->img_info[i].img_size;
            if (image_size > TEMP_BUFFER_SIZE) {
                if (__read_partition((const char *)dl_img_info->img_info[i].partition_name, offset, temp_buffer, TEMP_BUFFER_SIZE) < 0) {
                    ret = -1;
                    goto errout;
                }
                mbedtls_md5_update(&md5, temp_buffer, TEMP_BUFFER_SIZE);
                image_size -= TEMP_BUFFER_SIZE;
                offset += TEMP_BUFFER_SIZE;

                while (image_size > TEMP_BUFFER_SIZE) {
                    if (__read_partition((const char *)dl_img_info->img_info[i].partition_name, offset, temp_buffer, TEMP_BUFFER_SIZE) < 0) {
                        ret = -1;
                        goto errout;
                    }
                    mbedtls_md5_update(&md5, temp_buffer, TEMP_BUFFER_SIZE);
                    image_size -= TEMP_BUFFER_SIZE;
                    offset += TEMP_BUFFER_SIZE;
                }
                if (__read_partition((const char *)dl_img_info->img_info[i].partition_name, offset, temp_buffer, image_size) < 0) {
                    ret = -1;
                    goto errout;
                }
                mbedtls_md5_update(&md5, temp_buffer, image_size);
                offset += image_size;
            } else {
                if (__read_partition((const char *)dl_img_info->img_info[i].partition_name, offset, temp_buffer, image_size) < 0) {
                    ret = -1;
                    goto errout;
                }
                mbedtls_md5_update(&md5, temp_buffer, image_size);
                offset += image_size;
            }
            LOGD(TAG, "md5sum update, offset:%d, %s, img_size: %d", offset, dl_img_info->img_info[i].partition_name, dl_img_info->img_info[i].img_size);
        }
        mbedtls_md5_finish(&md5, md5_out);
        mbedtls_md5_free(&md5);
        if (memcmp(dl_img_info->md5sum, md5_out, 16) != 0) {
            printf("origin md5sum:\n");
            for (int kk = 0; kk < 16; kk++) {
                printf("0x%02x ", dl_img_info->md5sum[kk]);
            }
            printf("\r\n");
            printf("calculate md5sum:\n");
            for (int kk = 0; kk < 16; kk++) {
                printf("0x%02x ", md5_out[kk]);
            }
            printf("\r\n");
            LOGE(TAG, "image md5sum verify failed.");
            ret = -1;
            goto errout;
        }
    }
#endif /* CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK */
    LOGD(TAG, "image verify ok.");
    if (dl_img_info) {
        aos_free(dl_img_info);
    }
    if (temp_buffer) {
        aos_free(temp_buffer);
    }
    return 0;
errout:
    LOGD(TAG, "image verify error.");
    if (dl_img_info) {
        aos_free(dl_img_info);
    }
    if (temp_buffer) {
        aos_free(temp_buffer);
    }
    return ret;
}
#else
__attribute__((weak)) int fota_data_verify(void)
{
#define FOTA_DATA_MAGIC 0x45474d49
#define BUF_SIZE 512
    int ret;
    uint8_t *buffer;
    uint8_t *hash_out;
    uint32_t olen;
    int fota_data_offset, image_size;
    int signature_len, hash_len;
    int signature_offset, hash_offset;
    digest_sch_e digest_type;
    signature_sch_e sign_type;
    partition_t partition;
    partition_info_t *partition_info;
    fota_head_info_t *head;
    unsigned long data_address_start;

    LOGD(TAG, "start fota verify...");
    ret = 0;
    buffer = NULL;
    hash_out = NULL;
#if CONFIG_FOTA_DATA_IN_RAM > 0
    (void)partition;
    (void)partition_info;
    data_address_start = fota_data_address_get();
    if (data_address_start == 0) {
        LOGE(TAG, "fota data address[0x%x] error.", data_address_start);
        return -1;
    }
    head = (fota_head_info_t *)data_address_start;
    fota_data_offset = 0;
    buffer = aos_malloc(BUF_SIZE);
#else
    (void)data_address_start;
    partition = partition_open("misc");
    if (partition < 0) {
        LOGE(TAG, "flash open e.");
        return -1;
    }
    partition_info = partition_info_get(partition);
    if (partition_info == NULL) {
        ret = -EIO;
        goto out;
    }
    fota_data_offset = partition_info->erase_size << 1;
    buffer = aos_malloc(BUF_SIZE);
    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }
    if (partition_read(partition, fota_data_offset, buffer, BUF_SIZE) < 0) {
        ret = -EIO;
        goto out;
    }
    head = (fota_head_info_t *)buffer;
#endif
    if (head->magic != FOTA_DATA_MAGIC) {
        LOGE(TAG, "app fota data magic e");
        ret = -1;
        goto out;
    }
    image_size = head->mnft_off;
    digest_type = head->digest_type;
    sign_type = head->signature_type;
    hash_len = get_length_with_digest_type(digest_type);
    signature_len = get_length_with_signature_type(sign_type);
    signature_offset = image_size + fota_data_offset;
    hash_offset = signature_offset + 256;

    LOGD(TAG, "image_size:%d", image_size);
    LOGD(TAG, "digest_type:%d", digest_type);
    LOGD(TAG, "sign_type:%d", sign_type);
    LOGD(TAG, "hash_len:%d", hash_len);
    LOGD(TAG, "signature_len:%d", signature_len);
    LOGD(TAG, "signature_offset:%d", signature_offset);
    LOGD(TAG, "hash_offset:%d", hash_offset);

    hash_out = aos_malloc(hash_len);
    if (hash_out == NULL) {
        ret = -ENOMEM;
        goto out;
    }
#if CONFIG_FOTA_DATA_IN_RAM > 0
    ret = hash_calc_start(digest_type, (const uint8_t *)(fota_data_offset + data_address_start),
                          image_size, hash_out, &olen, 1, NULL);
    if (ret != 0) {
        LOGE(TAG, "hash calc failed.");
        goto out;
    }
    memcpy(buffer, (void *)(hash_offset + data_address_start), hash_len);
#else
    ret = hash_calc_start(digest_type,
                          (const uint8_t *)((long)(fota_data_offset + partition_info->base_addr + partition_info->start_addr)),
                          image_size, hash_out, &olen, 0, partition_info);
    if (ret != 0) {
        LOGE(TAG, "hash calc failed.");
        goto out;
    }
    if (partition_read(partition, hash_offset, buffer, hash_len) < 0) {
        ret = -EIO;
        goto out;
    }
#endif

#if CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK == 0
    key_handle key_addr;
    uint32_t key_size;
    if (km_get_pub_key_by_name(DEFAULT_PUBLIC_KEY_NAME_IN_OTP, &key_addr, &key_size)) {
        LOGE(TAG, "there is no pubkey found!");
        return -1;
    }
#if CONFIG_FOTA_DATA_IN_RAM > 0
    memcpy(buffer, (void *)(signature_offset + data_address_start), signature_len);
#else
    if (partition_read(partition, signature_offset, buffer, signature_len) < 0) {
        ret = -EIO;
        goto out;
    }
#endif
#if DUMP_DATA_EN
    dump_data(buffer, signature_len);
#endif
    ret = signature_verify_start(digest_type, sign_type, (uint8_t *)key_addr, key_size, hash_out, hash_len, buffer, signature_len);
    if (ret != 0) {
        LOGE(TAG, "signature verify failed.");
        goto out;
    }
    LOGD(TAG, "fota data verify ok...");
#else
    (void)sign_type;
    (void)signature_len;
#if DUMP_DATA_EN
    dump_data(buffer, hash_len);
    dump_data(hash_out, hash_len);
#endif
    if (memcmp(hash_out, buffer, hash_len) != 0) {
        LOGE(TAG, "!!!fota data hash v e..");
        ret = -EIO;
        goto out;
    }
    LOGI(TAG, "###fota data hash v ok.");
#endif /*CONFIG_FOTA_IMG_AUTHENTICITY_NOT_CHECK*/

out:
    if (buffer)
        aos_free(buffer);
    if (hash_out)
        aos_free(hash_out);
#if CONFIG_FOTA_DATA_IN_RAM == 0
    partition_close(partition);
#endif
    return ret;
}
#endif /*CONFIG_OTA_AB*/

#else

__attribute__((weak)) int fota_data_verify(void)
{
    return 0;
}

#endif /*__linux__*/