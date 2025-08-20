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
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "mtb.h"
#include "mtb_internal.h"
#include "mtb_log.h"
#include "yoc/partition_device.h"
#include "yoc/partition.h"
#include "verify.h"
#include "verify_wrapper.h"
#if (CONFIG_PARITION_NO_VERIFY == 0) && (defined(CONFIG_COMP_KEY_MGR) && CONFIG_COMP_KEY_MGR)
#include <key_mgr.h>
#endif

int mtbv4_partition_count(void)
{
    imtb_head_v4_t *head;

    head = (imtb_head_v4_t *)mtb_get()->using_addr;
    return head->partition_count;
}

int mtbv4_init(void)
{
    mtb_t *mtb;
    mtb_partition_info_t part_info;
    partition_device_info_t flash_info;

    mtb = mtb_get();

    MTB_LOGD("imtb using:0x%lx, valid offset:0x%lx", mtb->using_addr, mtb->prim_offset);

    if (mtbv4_get_partition_info(MTB_IMAGE_NAME_IMTB, &part_info)) {
        MTB_LOGE("mtb f `imtb` e");
        return -1;
    }
    MTB_LOGD("part_info.storage_info.type:%d", part_info.storage_info.type);
    MTB_LOGD("part_info.storage_info.id:%d", part_info.storage_info.id);
    MTB_LOGD("part_info.storage_info.area:%d", part_info.storage_info.area);
    void *handle = partition_device_find(&part_info.storage_info);
    if (partition_device_info_get(handle, &flash_info)) {
        MTB_LOGE("mtb get dev info e");
        return -1;
    }
    MTB_LOGD("part_info.end_addr:0x%"PRIX64, part_info.end_addr);
    MTB_LOGD("part_info.start_addr:0x%"PRIX64, part_info.start_addr);
    MTB_LOGD("flash_info.base_addr:0x%"PRIX64, flash_info.base_addr);
    mtb->one_size = (part_info.end_addr - part_info.start_addr) >> 1;
    if (mtb->prim_offset == part_info.start_addr - flash_info.base_addr) {
        mtb->backup_offset = part_info.start_addr - flash_info.base_addr + mtb->one_size;
    } else {
        mtb->backup_offset = part_info.start_addr - flash_info.base_addr;
    }
    memcpy(&mtb->storage_info, &part_info.storage_info, sizeof(storage_info_t));
    MTB_LOGD("imtb backup_offset:0x%lx", mtb->backup_offset);
    MTB_LOGD("imtb one-size:0x%x", mtb->one_size);
    MTB_LOGD("mtb init over");
    return 0;
}

int mtbv4_get_partition_info(const char *name, mtb_partition_info_t *part_info)
{
    int i, count;
    storage_info_t storage_info;
    imtb_head_v4_t *head;
    imtb_partition_info_v4_t *pp;
    partition_device_info_t flash_info;

    if (name && part_info) {
        head = (imtb_head_v4_t *)mtb_get()->using_addr;
        pp = (imtb_partition_info_v4_t *)(mtb_get()->using_addr + sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4);
        count = head->partition_count;
        for (i = 0; i < count; i++) {
            if (strncmp(pp->name, name, MTB_IMAGE_NAME_SIZE) == 0) {
#if 0
                memcpy(part_info->pub_key_name, pp->pub_key_name, PUBLIC_KEY_NAME_SIZE);
#else
                memcpy(&part_info->storage_info, &pp->storage_info, sizeof(storage_info_t));
                part_info->preload_size = pp->preload_size;
#endif
                storage_info.id = 0;
                storage_info.type = mtb_get_default_device_type();
                storage_info.hot_plug = 0;
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
                memcpy(&storage_info, &part_info->storage_info, sizeof(storage_info_t));
#else
                memcpy(&part_info->storage_info, &storage_info, sizeof(storage_info_t));
#endif
                if (!storage_info.hot_plug) {
                    void *handle = partition_device_find(&storage_info);
                    if (partition_device_info_get(handle, &flash_info)) {
                        MTB_LOGE("get [%s]'s device[%d,%d,%d] info e!!", pp->name, storage_info.type, storage_info.id, storage_info.area);
                        return -1;
                    }
                } else {
                    flash_info.base_addr = 0;
                }

                part_info->start_addr = flash_info.base_addr + pp->block_offset * 512;
                uint32_t blk_cnt = (uint32_t)pp->block_count_h << 16 | pp->block_count;
                part_info->end_addr = part_info->start_addr + (uint64_t)blk_cnt * 512;
                part_info->load_addr = pp->load_address;
                memcpy(part_info->name, pp->name, MTB_IMAGE_NAME_SIZE);
                part_info->img_size = pp->img_size;
                return 0;
            }
            pp ++;
        }
    }
    MTB_LOGE("arg e.");
    return -EINVAL;
}

int mtbv4_get_partition_info_with_index(int index, mtb_partition_info_t *part_info)
{
    int count;
    storage_info_t storage_info;
    imtb_head_v4_t *head;
    imtb_partition_info_v4_t *pp;
    partition_device_info_t flash_info;

    if (part_info) {
        head = (imtb_head_v4_t *)mtb_get()->using_addr;
        pp = (imtb_partition_info_v4_t *)(mtb_get()->using_addr + sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4);
        count = head->partition_count;
        if (index < count) {
            pp += index;
#if 0
            memcpy(part_info->pub_key_name, pp->pub_key_name, PUBLIC_KEY_NAME_SIZE);
#else
            memcpy(&part_info->storage_info, &pp->storage_info, sizeof(storage_info_t));
            part_info->preload_size = pp->preload_size;
#endif
            storage_info.id = 0;
            storage_info.type = mtb_get_default_device_type();
            storage_info.hot_plug = 0;
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
            memcpy(&storage_info, &part_info->storage_info, sizeof(storage_info_t));
#else
            memcpy(&part_info->storage_info, &storage_info, sizeof(storage_info_t));
#endif
            if (!storage_info.hot_plug) {
                void *handle = partition_device_find(&storage_info);
                if (partition_device_info_get(handle, &flash_info)) {
                    MTB_LOGE("get [%s]'s device[%d,%d,%d] info e!!", pp->name, storage_info.type, storage_info.id, storage_info.area);
                    return -1;
                }
            } else {
                flash_info.base_addr = 0;
            }

            part_info->start_addr = flash_info.base_addr + pp->block_offset * 512;
            uint32_t blk_cnt = (uint32_t)pp->block_count_h << 16 | pp->block_count;
            part_info->end_addr = part_info->start_addr + (uint64_t)blk_cnt * 512;
            part_info->load_addr = pp->load_address;
            memcpy(part_info->name, pp->name, MTB_IMAGE_NAME_SIZE);
            part_info->img_size = pp->img_size;
            return 0;
        }
    }
    MTB_LOGE("arg e.");
    return -EINVAL;
}

int mtbv4_crc_check(void)
{
    mtb_t *mtb;
    imtb_head_v4_t *m_head;
    int crc_content_len;
    uint8_t *ocrc;
    uint32_t crc_cla;
    uint32_t crc;

    mtb = mtb_get();
    m_head = (imtb_head_v4_t *)mtb->using_addr;
    crc_content_len = sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4 + \
                          m_head->partition_count * sizeof(imtb_partition_info_v4_t);
    ocrc = (uint8_t *)m_head + crc_content_len;
    crc = ocrc[0];

    crc32_calc_start((uint8_t *)m_head, crc_content_len, &crc_cla);

    crc <<= 8;
    crc |= ocrc[1];
    crc <<= 8;
    crc |= ocrc[2];
    crc <<= 8;
    crc |= ocrc[3];

    if (crc != crc_cla) {
        MTB_LOGE("crc verify e[0x%08x,0x%08x]", crc_cla, crc);
        return -1;
    }
    MTB_LOGD("imtb crc verify ok");
    return 0;
}

int mtbv4_verify(void)
{
#if (CONFIG_PARITION_NO_VERIFY == 0)
    MTB_LOGD("mtb verify ok");
#endif
    return 0;
}

__attribute__((weak)) bool check_is_need_verify(const char *name)
{
    return false;
}

#if (CONFIG_PARITION_NO_VERIFY == 0)
int mtbv4_image_verify(const char *name)
{
#define READ_MAX_SIZE 32
#define PART_HEAD_SIZE (sizeof(partition_header_t))
#define PART_TAIL_HEAD_SIZE (sizeof(partition_tail_head_t))
    int        offset, need_verify;
    uint32_t   olen;
    int        digest_type, sig_type;
    uint8_t    buf[READ_MAX_SIZE + PART_HEAD_SIZE];
    uint8_t    old_sha[128];
    uint8_t    part_tail_buf[PART_TAIL_HEAD_SIZE];
    uint32_t   custom_offset;
    uint32_t   step;

    custom_offset = 0;
    step = 4;
#ifdef CONFIG_CHIP_D1
    custom_offset = 1024;
    step = 1;
#endif

    partition_header_t *p_head;
    partition_tail_head_t *p_tail_head;
    partition_info_t *part_info;

    partition_t part = partition_open(name);
    part_info = partition_info_get(part);
    if (!part_info) {
        MTB_LOGE("get partition:%d info failed.", part);
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    if (partition_read(part, custom_offset, buf, sizeof(buf))) {
        MTB_LOGE("read partition:%d failed.", part);
        return -1;
    }
#if 0
    printf("---------------------------------------------------\n");
    dump_data((uint8_t *)buf, sizeof(buf));
    printf("---------------------------------------------------\n");
#endif
    offset      = 0;
    need_verify = 0;
    while (offset < READ_MAX_SIZE) {
        p_head = (partition_header_t *)&buf[offset];

        if (memcmp(&p_head->jump, PARTITION_HEAD_MAGIC, 4) &&
            memcmp(&p_head->magic, PARTITION_HEAD_MAGIC, 4) == 0 &&
            memcmp(&p_head->size, PARTITION_HEAD_MAGIC, 4)) {
            need_verify = 1;
            break;
        }
        offset += step;
    }
    MTB_LOGD("--- %s", name);
    MTB_LOGD("offset:%d, need_verify:%d", offset, need_verify);
    MTB_LOGD("custom_offset=0x%x", custom_offset);

    if (check_is_need_verify(name) && need_verify == 0) {
        MTB_LOGE("need verify, but not signed.");
        return -1;
    }

    if (need_verify == 1) {
        uint32_t img_content_size = p_head->size;
        uint32_t img_content_addr = part_info->base_addr + part_info->start_addr + PART_HEAD_SIZE + offset + custom_offset;
        uint32_t part_tail_addr   = img_content_addr + img_content_size;

        MTB_LOGD("p_head->size=0x%x", p_head->size);
        MTB_LOGD("part_tail_addr:0x%x", part_tail_addr);

        if (img_content_size > part_info->length) {
            MTB_LOGE("the image content size is overflow!");
            return -1;
        }

        if (partition_read(part, part_tail_addr - (part_info->base_addr + part_info->start_addr), part_tail_buf, PART_TAIL_HEAD_SIZE)) {
            return -1;
        }
        p_tail_head = (partition_tail_head_t *)part_tail_buf;

        MTB_LOGD("start verify [%s]", name);
        digest_type = p_tail_head->digestType;
        sig_type    = p_tail_head->signatureType;
        if (sig_type == SIGNATURE_RSA_1024 || sig_type == SIGNATURE_RSA_2048) {
            uint8_t sha[64];
            uint32_t hash_addr = part_tail_addr + PART_TAIL_HEAD_SIZE;
            uint32_t sig_addr  = hash_addr;
            int      hash_len  = get_length_with_digest_type(digest_type);
            sig_addr += hash_len;
            MTB_LOGD("start check hash");

            hash_calc_start(digest_type, (const uint8_t *)((unsigned long)img_content_addr),
                            img_content_size, sha, &olen, 0, part_info);
            if (olen != hash_len) {
                MTB_LOGE("sha len calc error:%d,%d", hash_len, olen);
                return -1;
            }

            if (partition_read(part, hash_addr - (part_info->base_addr + part_info->start_addr), old_sha, hash_len)) {
                return -1;
            }
#if 0
            printf("old_sha---------------------------------------------------\n");
            dump_data((uint8_t *)old_sha, hash_len);
            printf("calc_sha---------------------------------------------------\n");
            dump_data((uint8_t *)sha, hash_len);
            printf("---------------------------------------------------\n");
#endif
            if (memcmp(old_sha, sha, hash_len)) {
                MTB_LOGE("img hash verify fail [%s]", name);
                return -1;
            }
#if CONFIG_IMG_AUTHENTICITY_NOT_CHECK == 0 && (defined(CONFIG_COMP_KEY_MGR) && CONFIG_COMP_KEY_MGR)
            key_handle key_addr;
            uint32_t key_size;
            uint8_t old_sig[256];
            // char *pub_key_name = part_info.pub_key_name;
            char *pub_key_name = (char *)DEFAULT_PUBLIC_KEY_NAME_IN_OTP;
            int sig_len = get_length_with_signature_type(sig_type);
            if (km_get_pub_key_by_name(pub_key_name, &key_addr, &key_size) == KM_OK) {
                MTB_LOGD("key_addr:0x%lx", (unsigned long)key_addr);
                if (partition_read(part, sig_addr - (part_info->base_addr + part_info->start_addr), old_sig, sig_len)) {
                    return -1;
                }
                MTB_LOGD("sig_offset:0x%x",sig_addr);
#if 0
                printf("key---------------------------------------------------\n");
                dump_data((uint8_t *)key_addr, key_size);
                printf("old_sig---------------------------------------------------\n");
                dump_data((uint8_t *)old_sig, sig_len);
                printf("---------------------------------------------------\n");
#endif
                if (signature_verify_start(digest_type, sig_type, (uint8_t *)(key_addr),
                                           key_size, sha, hash_len, old_sig, sig_len) != 0) {
                    MTB_LOGE("img rsa verify e");
                    return -1;
                }
            }  else {
                MTB_LOGE("no pubkey found, %s", pub_key_name);
                MTB_LOGE("verify failed!");
                return -1;
            }
#endif
            MTB_LOGI("verify [%s] ok", name);
        } else {
            MTB_LOGE("sig type e:%d", p_tail_head->signatureType);
            return -1;
        }
        MTB_LOGD("img verify ok");
    } else {
        MTB_LOGD("img do not need verify.");
    }

    return 0;
}
#endif /* CONFIG_PARITION_NO_VERIFY */

// 为了兼容老的只有一种eflash或者spiflash时使用
int mtb_get_default_device_type(void)
{
    int device_type = 0;

#if CONFIG_PARTITION_SUPPORT_EFLASH
    device_type = MEM_DEVICE_TYPE_EFLASH;
#elif CONFIG_PARTITION_SUPPORT_SPINORFLASH
    device_type = MEM_DEVICE_TYPE_SPI_NOR_FLASH;
#endif

    return device_type;
}