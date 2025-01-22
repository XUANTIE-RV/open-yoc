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

#include <yoc/partition.h>
#include <yoc/partition_device.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <drv/sasc.h>
#include "mtb_internal.h"
#include "verify.h"
#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
#endif

#ifndef SHOW_PART_INFO_EN
#define SHOW_PART_INFO_EN 0
#endif

static struct {
    partition_info_t *scn_list;
    uint8_t num;
} g_partion_array;
static uint32_t g_scn_buf[sizeof(partition_info_t) * CONFIG_MAX_PARTITION_NUM / 4];

int partition_init(void)
{
#define BUF_SIZE (sizeof(sys_partition_info_t) * CONFIG_MAX_PARTITION_NUM)
    int i, ret, num, size;
    storage_info_t storage_info;
    sys_partition_info_t *part_info;
    partition_device_info_t cur_flash_info = {0};
    void *temp_flash_dev;
    static int g_part_init_ok = 0;
    partition_info_t *scn;

    if (g_part_init_ok == 1) {
        return g_partion_array.num;
    }
#ifdef CONFIG_PARTITION_BUF_ALLOC
    uint8_t *buf = malloc(BUF_SIZE);
    if (buf == NULL) {
        return -1;
    }
#else
    uint8_t buf[BUF_SIZE];
#endif

    storage_info.type = 0;
    storage_info.id = 0;
    storage_info.hot_plug = 0;
#if CONFIG_PARTITION_SUPPORT_EFLASH
    partition_eflash_register();
    storage_info.type = MEM_DEVICE_TYPE_EFLASH;
#endif
#if CONFIG_PARTITION_SUPPORT_SPINORFLASH
    partition_flash_register();
    storage_info.type = MEM_DEVICE_TYPE_SPI_NOR_FLASH;
#endif
#if CONFIG_PARTITION_SUPPORT_SPINANDFLASH
    partition_spinandflash_register();
    storage_info.type = MEM_DEVICE_TYPE_SPI_NAND_FLASH;
#endif
#if CONFIG_PARTITION_SUPPORT_EMMC
    partition_emmc_register();
    storage_info.type = MEM_DEVICE_TYPE_EMMC;
#endif
#if CONFIG_PARTITION_SUPPORT_SD
    partition_sd_register();
    storage_info.type = MEM_DEVICE_TYPE_SD;
#endif
#if CONFIG_PARTITION_SUPPORT_USB
    partition_usb_register();
    storage_info.type = MEM_DEVICE_TYPE_USB;
#endif

#ifdef CONFIG_TEE_CA
    ret = csi_tee_get_sys_partition(buf, (uint32_t *)&size);
#else
    ret = mtb_init();
    if (ret == 0)
        ret = get_sys_partition(buf, (uint32_t *)&size);
#endif
    if (ret < 0 || size > BUF_SIZE) {
        goto failure;
    }

    part_info = (sys_partition_info_t *)buf;
    num = size / sizeof(sys_partition_info_t);
    g_partion_array.scn_list = (partition_info_t *)g_scn_buf;

#if SHOW_PART_INFO_EN
    printf("#############################[part_num:%d]\n", num);
#endif
    for (i = 0; i < num; i++) {
        memset(&cur_flash_info, 0, sizeof(partition_device_info_t));
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
        memcpy(&storage_info, &part_info[i].storage_info, sizeof(storage_info_t));
#endif
        if (!storage_info.hot_plug) {
            temp_flash_dev = partition_device_find(&storage_info);
            if (partition_device_info_get(temp_flash_dev, &cur_flash_info)) {
                goto failure;
            }
        } else {
            goto hot_pulg_info;
        }

        if ((cur_flash_info.base_addr <= part_info[i].part_addr) &&
            ((part_info[i].part_addr + part_info[i].part_size) <= (cur_flash_info.base_addr + cur_flash_info.device_size))) {
hot_pulg_info:
            scn = &g_partion_array.scn_list[i];
            memcpy(scn->description, part_info[i].image_name, MTB_IMAGE_NAME_SIZE);
            scn->base_addr = cur_flash_info.base_addr;
            scn->start_addr = part_info[i].part_addr - cur_flash_info.base_addr;
            scn->length = part_info[i].part_size;
            scn->sector_size = cur_flash_info.sector_size;
            scn->block_size = cur_flash_info.block_size;
            scn->erase_size = cur_flash_info.erase_size;
            scn->load_addr = part_info[i].load_addr;
            scn->image_size = part_info[i].image_size;
            scn->preload_size = part_info[i].preload_size;
            // for compatible
            memset(&scn->storage_info, 0, sizeof(storage_info_t));
            scn->storage_info.type = MEM_DEVICE_TYPE_SPI_NOR_FLASH;
            scn->storage_info.hot_plug = 0;
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
            memcpy(&scn->storage_info, &part_info[i].storage_info, sizeof(storage_info_t));
#endif
#if CONFIG_PARTITION_SUPPORT_EMMC
            scn->boot_area_size = cur_flash_info.boot_area_size;
#endif
#if SHOW_PART_INFO_EN
            printf("------------------>%s\n", scn->description);
            printf("scn->base_addr:0x%"PRIX64"\n", scn->base_addr);
            printf("scn->start_addr:0x%"PRIX64"\n", scn->start_addr);
            printf("scn->length:0x%"PRIX64"\n", scn->length);
            printf("scn->sector_size:0x%x\n", scn->sector_size);
            printf("scn->block_size:0x%x\n", scn->block_size);
            printf("scn->erase_size:0x%x\n", scn->erase_size);
            printf("scn->load_addr:0x%x\n", scn->load_addr);
            printf("scn->image_size:0x%x\n", scn->image_size);
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
            printf("scn->storage_info.type:%d\n", scn->storage_info.type);
            printf("scn->storage_info.id:%d\n", scn->storage_info.id);
            printf("scn->storage_info.area:%d\n", scn->storage_info.area);
            printf("scn->storage_info.hot_plug:%d\n", scn->storage_info.hot_plug);
#endif
#if CONFIG_PARTITION_SUPPORT_EMMC
            printf("scn->boot_area_size:0x%x\n", scn->boot_area_size);
#endif
#endif /*SHOW_PART_INFO_EN*/
            if(!storage_info.hot_plug && !(scn->length && scn->erase_size && strlen(scn->description) > 0 && (scn->sector_size | scn->block_size))) {
                goto failure;
            }
            g_partion_array.num ++;
        }
    }

    g_part_init_ok = 1;
#if SHOW_PART_INFO_EN
    printf("g_partion_array.num:%d\n", g_partion_array.num);
#endif
#ifdef CONFIG_PARTITION_BUF_ALLOC
    free(buf);
#endif
    return g_partion_array.num;
failure:
#ifdef CONFIG_PARTITION_BUF_ALLOC
    free(buf);
#endif
    return -1;
}

partition_info_t *partition_info_get(partition_t partition)
{
#if defined(CONFIG_KERNEL_NONE)
    if (partition >= 0 && partition < g_partion_array.num)
#else
    if (partition >= 0 && partition < g_partion_array.num && g_partion_array.scn_list[partition].flash_dev)
#endif
    {
        return &g_partion_array.scn_list[partition];
    }
    return NULL;
}

static int __update_hot_plug_device(partition_t partition, partition_device_ops_t *dev_ops)
{
    partition_device_info_t cur_flash_info = {0};
    partition_info_t *scn = &g_partion_array.scn_list[partition];

    if (partition_device_info_get(dev_ops, &cur_flash_info)) {
        return -1;
    }
    if ((cur_flash_info.base_addr <= scn->start_addr) &&
        ((scn->start_addr + scn->length) <= (cur_flash_info.base_addr + cur_flash_info.device_size))) {
        scn->base_addr = cur_flash_info.base_addr;
        if (scn->base_addr != 0) {
            assert(false);
        }
        scn->sector_size = cur_flash_info.sector_size;
        scn->block_size = cur_flash_info.block_size;
        scn->erase_size = cur_flash_info.erase_size;
        if (scn->length == 0)
            scn->length = cur_flash_info.device_size;
    #if SHOW_PART_INFO_EN
        printf("------------update hot_plug------>%s\n", scn->description);
        printf("scn->base_addr:0x%"PRIX64"\n", scn->base_addr);
        printf("scn->start_addr:0x%"PRIX64"\n", scn->start_addr);
        printf("scn->length:0x%"PRIX64"\n", scn->length);
        printf("scn->sector_size:0x%x\n", scn->sector_size);
        printf("scn->block_size:0x%x\n", scn->block_size);
        printf("scn->erase_size:0x%x\n", scn->erase_size);
        printf("scn->load_addr:0x%x\n", scn->load_addr);
        printf("scn->image_size:0x%x\n", scn->image_size);
        printf("scn->storage_info.type:%d\n", scn->storage_info.type);
        printf("scn->storage_info.id:%d\n", scn->storage_info.id);
        printf("scn->storage_info.area:%d\n", scn->storage_info.area);
    #endif /*SHOW_PART_INFO_EN*/
        if(!(scn->length && scn->erase_size && strlen(scn->description) > 0 && (scn->sector_size | scn->block_size))) {
            return -1;
        }
        if (scn->start_addr + scn->length > cur_flash_info.device_size) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

partition_t partition_open(const char *name)
{
    int len;

    if (name == NULL) {
        return -EINVAL;
    }
    len = strlen(name);
    len = len > MTB_IMAGE_NAME_SIZE ? MTB_IMAGE_NAME_SIZE : len;    
    for (int i = 0; i < g_partion_array.num; i++) {
        if (memcmp(name, g_partion_array.scn_list[i].description, len) == 0) {
            if (g_partion_array.scn_list[i].flash_dev == NULL) {
                void *flash_dev = partition_device_find(&g_partion_array.scn_list[i].storage_info);
                if (g_partion_array.scn_list[i].storage_info.hot_plug) {
                    if (__update_hot_plug_device(i, flash_dev))
                        return -1;
                }
                g_partion_array.scn_list[i].flash_dev = flash_dev;
            }
            return i;
        }
    }
    return -1;
}

void partition_close(partition_t partition)
{
    partition_info_t *node = partition_info_get(partition);
    if (node) {
        partition_device_close(node->flash_dev);
        node->flash_dev = NULL;
    }
}

int partition_read(partition_t partition, off_t off_set, void *data, size_t size)
{
    if (size == 0) {
        return 0;
    }
    if (off_set < 0 || data == NULL) {
        return -EINVAL;
    }
    partition_info_t *node = partition_info_get(partition);
    if (node != NULL && off_set + size <= node->length) {
        return partition_device_read(node->flash_dev, node->start_addr + off_set, data, size);
    }
    return -1;
}

int partition_write(partition_t partition, off_t off_set, void *data, size_t size)
{
    if (size == 0) {
        return 0;
    }
    if (off_set < 0 || data == NULL) {
        return -EINVAL;
    }

    partition_info_t *node = partition_info_get(partition);
    if (node != NULL && off_set >= 0 && off_set + size <= node->length) {
        return partition_device_write(node->flash_dev, node->start_addr + off_set, data, size);
    }
    return -1;
}

int partition_erase_size(partition_t partition, off_t off_set, size_t size)
{
    if (size == 0) {
        return 0;
    }
    if (off_set < 0) {
        return -EINVAL;
    }

    partition_info_t *node = partition_info_get(partition);
    if (size % node->erase_size) {
        size = (size / node->erase_size + 1) * node->erase_size;
    }
    if (off_set % node->erase_size) {
        return -EINVAL;
    }
    if (node != NULL && off_set >= 0 && off_set + size <= node->length) {
        return partition_device_erase(node->flash_dev, node->start_addr + off_set, size);
    }
    return -1;
}

int partition_erase(partition_t partition, off_t off_set, uint32_t erase_unit_count)
{
    if (erase_unit_count == 0) {
        return 0;
    }
    if (off_set < 0) {
        return -EINVAL;
    }

    partition_info_t *node = partition_info_get(partition);
    if (off_set % node->erase_size) {
        return -EINVAL;
    }
    size_t len = erase_unit_count * node->erase_size;
    if (node != NULL && off_set >= 0 && off_set + len <= node->length) {
        return partition_device_erase(node->flash_dev, node->start_addr + off_set, len);
    }
    return -1;
}

int partition_get_digest(partition_t partition, uint8_t *out_hash, uint32_t *out_len)
{
#define READ_SIZE (1024 + 32)
#define TAIL_BUFF_SIZE (sizeof(partition_tail_head_t) + 128)
    bool got_flag;
    uint8_t *buffer;
    int offset;
    uint8_t tail_buf[TAIL_BUFF_SIZE];
    partition_header_t *phead;
    partition_tail_head_t *tail = NULL;

    if (!(out_hash && out_len)) {
        return -EINVAL;
    }
    buffer = malloc(READ_SIZE);
    if (buffer == NULL) {
        return -ENOMEM;
    }
    if (partition_read(partition, 0, buffer, READ_SIZE) != 0) {
        goto fail;
    }
    got_flag = false;
    offset = 0;
    while (offset < READ_SIZE) {
        phead = (partition_header_t *)&buffer[offset];
        if (memcmp((uint8_t *)&phead->magic, PARTITION_HEAD_MAGIC, 4) == 0
            && memcmp((uint8_t *)&phead->size, (const void *)PARTITION_HEAD_MAGIC, 4) != 0) {
            got_flag = true;
            break;
        }
        offset += 1;
    }

    if (got_flag) {
        uint32_t img_content_size = phead->size;
        uint32_t tail_offset = sizeof(partition_header_t) + img_content_size + offset;
        partition_info_t *node = partition_info_get(partition);
        if (img_content_size > node->length) {
            goto fail;
        }
        tail = (partition_tail_head_t *)tail_buf;
        if (partition_read(partition, tail_offset, (void *)tail, TAIL_BUFF_SIZE) == 0) {
            switch(tail->digestType) {
                case DIGEST_HASH_SHA1:
                    *out_len = 20;
                    memcpy(out_hash, tail->digest_data, 20);
                    break;
                case DIGEST_HASH_MD5:
                    *out_len = 16;
                    memcpy(out_hash, tail->digest_data, 16);
                    break;
                case DIGEST_HASH_SHA224:
                    *out_len = 28;
                    memcpy(out_hash, tail->digest_data, 28);
                    break;
                case DIGEST_HASH_SHA256:
                    *out_len = 32;
                    memcpy(out_hash, tail->digest_data, 32);
                    break;
                case DIGEST_HASH_SHA384:
                    *out_len = 48;
                    memcpy(out_hash, tail->digest_data, 48);
                    break;
                case DIGEST_HASH_SHA512:
                    *out_len = 64;
                    memcpy(out_hash, tail->digest_data, 64);
                    break;
                default:
                    goto fail;
            }
            free(buffer);
            return 0;
        }
    }

fail:
    free(buffer);
    return -1;
}

int partition_verify(partition_t partition)
{
    int ret;
    partition_info_t *info = partition_info_get(partition);

    ret = -1;
    if (info) {
        ret = mtb_image_verify(info->description);
    }

    return ret;
}

int partition_all_verify(void)
{
    if (g_partion_array.scn_list && g_partion_array.num > 0) {
        for (int i = 0; i < g_partion_array.num; i++) {
            partition_device_ops_t *flash_dev = partition_device_find(&g_partion_array.scn_list[i].storage_info);
            if (g_partion_array.scn_list[i].storage_info.hot_plug) {
                if (!flash_dev || (flash_dev && !flash_dev->dev_hdl)) {
                    continue;
                }
                if (__update_hot_plug_device(i, flash_dev))
                    return -1;
            }
            g_partion_array.scn_list[i].flash_dev = flash_dev;

            if (partition_verify(i) != 0) {
                g_partion_array.scn_list[i].flash_dev = NULL;
                return -1;
            }
            g_partion_array.scn_list[i].flash_dev = NULL;
        }
        return 0;
    }
    return -EINVAL;
}

int partition_set_region_safe(partition_t partition)
{
#if (CONFIG_NOT_SUPORRT_SASC == 0)
    partition_info_t *node = partition_info_get(partition);
    if (node != NULL) {
        uint32_t addr = node->start_addr + node->base_addr;
        int size = node->length;
        int sector_size = node->sector_size;
#ifdef CONFIG_CSI_V2
        csi_sasc_attr_t attr;

        attr.super_ap  = SASC_RW;
        attr.user_ap   = SASC_AP_DENY;
        attr.super_di  = SASC_DI;
        attr.user_di   = SASC_DI;
        attr.is_secure = 1;

        return csi_sasc_flash_config(0, addr, (size + sector_size - 1) / sector_size + SASC_FLASH_1S - 1, attr);
#endif
#ifdef CONFIG_CSI_V1
        return drv_sasc_config_region(addr,
                (size + sector_size - 1) / sector_size + SASC_FLASH_1S - 1 ,
                SASC_AP_RW, SASC_AP_DN, SASC_CD_DI, 1);
#endif
    }
#endif /* CONFIG_NOT_SUPORRT_SASC */
    return -EINVAL;
}

#ifndef PARTITION_SPLIT_DEBUG
#define PARTITION_SPLIT_DEBUG 0
#endif
#if PARTITION_SPLIT_DEBUG
#define SPLIT_PRINT(...) printf(__VA_ARGS__)
#else
#define SPLIT_PRINT(...)
#endif
#define IMG_HEADER_LEN 48

#if defined(CONFIG_XZ_CMP)
#include <xz_dec.h>
struct xz_ext_info_t {
    partition_t partition;
    partition_info_t *info;
};
static int __data_read(unsigned long read_addr, void *buffer, size_t size, void *ext_info)
{
    struct xz_ext_info_t *ext = ext_info;
    if (ext_info == NULL) {
        return -1;
    }
    int ret = partition_read(ext->partition, read_addr - ext->info->base_addr - ext->info->start_addr, buffer, size);
    return ret;
}
#endif

#if defined(CONFIG_LZ4_COMP)
#include <lz4.h>
#ifndef CONFIG_LZ4_BUFFER_ADDR
#error "Please define CONFIG_LZ4_BUFFER_ADDR in package.yaml"
#endif
#endif

bool partition_check_firmware_is_packed(partition_t partition)
{
    int ret;
    uint8_t header[IMG_HEADER_LEN];
    pack_compress_list_t *pack_compress_list;

    partition_info_t *info = partition_info_get(partition);
    if (info) {
        ret = partition_read(partition, 0, header, IMG_HEADER_LEN);
        if (ret != 0) {
            return false;
        }
        pack_compress_list = (pack_compress_list_t *)header;
        if (pack_compress_list->pack_info.magic != PACK_LIST_MAGIC) {
            return false;
        }
        return true;
    }
    return false;
}

int partition_split_and_get(partition_t partition, int index, unsigned long *offset, size_t *olen, unsigned long *run_address)
{
    int ret, count;
    unsigned long pre_offset;
    uint8_t header[IMG_HEADER_LEN];
    pack_compress_list_t *pack_compress_list;

    if (!(offset || olen || run_address)) {
        return -EINVAL;
    }
    SPLIT_PRINT("Split and Get index [%d]\n", index);
    partition_info_t *info = partition_info_get(partition);
    if (info) {
        count = 0;
        pre_offset = 0;
        do {
            SPLIT_PRINT("current index:%d:\n", count);
            SPLIT_PRINT("pre_offset:0x%lx\n", pre_offset);
            ret = partition_read(partition, pre_offset, header, IMG_HEADER_LEN);
            if (ret != 0) {
                return ret;
            }
            pack_compress_list = (pack_compress_list_t *)header;
            if (pack_compress_list->pack_info.magic != PACK_LIST_MAGIC) {
                return -1;
            }
            if (offset) *offset = pre_offset;
            if (olen) *olen = pack_compress_list->pack_info.compressed_size + sizeof(pack_compress_list_t);
            if (run_address) {
                *run_address = 0;
    #if (defined(__riscv) && (__riscv_xlen == 64)) || defined(__ARM_ARCH_ISA_A64)
                *run_address = pack_compress_list->pack_info.run_address_h;
                *run_address <<= 32;
    #endif
                *run_address |= pack_compress_list->pack_info.run_address_l;
            }
            if (count == index) {
                SPLIT_PRINT("Split success get one.\n");
                return 0;
            }
            count ++;
            pre_offset += pack_compress_list->pack_info.next_offset + sizeof(pack_compress_list_t);
        } while (pack_compress_list->pack_info.next_offset != 0);
    }
    return -1;
}

int partition_split_and_copy(partition_t partition, int index)
{
    int ret, count, olen;
    unsigned long run_address;
    unsigned long pre_offset;
    uint8_t header[IMG_HEADER_LEN];
    pack_compress_list_t *pack_compress_list;

    SPLIT_PRINT("Split and Copy index [%d]\n", index);
    partition_info_t *info = partition_info_get(partition);
    if (info) {
        count = 0;
        pre_offset = 0;
        do {
            SPLIT_PRINT("current index:%d:\n", count);
            SPLIT_PRINT("pre_offset:0x%lx\n", pre_offset);
            ret = partition_read(partition, pre_offset, header, IMG_HEADER_LEN);
            if (ret != 0) {
                return ret;
            }
            pack_compress_list = (pack_compress_list_t *)header;
            if (pack_compress_list->pack_info.magic != PACK_LIST_MAGIC) {
                return -1;
            }
            olen = pack_compress_list->pack_info.origin_size + sizeof(pack_compress_list_t);
            run_address = 0;
#if (defined(__riscv) && (__riscv_xlen == 64)) || defined(__ARM_ARCH_ISA_A64)
            run_address = pack_compress_list->pack_info.run_address_h;
            run_address <<= 32;
#endif
            run_address |= pack_compress_list->pack_info.run_address_l;
            // SPLIT_PRINT("the run address: %"PRIx64"\n", run_address);
            SPLIT_PRINT("the run address: 0x%lx\n", run_address);
            if (count == index) {
                memcpy((void *)run_address, header, sizeof(pack_compress_list_t));
                if (pack_compress_list->pack_info.compress_type == CAR_XZ) {
#if defined(CONFIG_XZ_CMP)
                    uint32_t out_len;
                    struct xz_ext_info_t ext_info;
                    unsigned long src_addr = pre_offset + info->base_addr + info->start_addr + sizeof(pack_compress_list_t);
                    SPLIT_PRINT("start xz decompress src addr 0x%lx, compressed_size:%d\n", src_addr, pack_compress_list->pack_info.compressed_size);
                    ext_info.info = info;
                    ext_info.partition = partition;
                    ret = xz_decompress((uint8_t *)src_addr, pack_compress_list->pack_info.compressed_size, __data_read,
                                        (uint8_t *)(run_address + sizeof(pack_compress_list_t)), &out_len, NULL, &ext_info);
                    if(ret) {
                        SPLIT_PRINT("xz decompress or write faild %d\n", ret);
                        return ret;
                    }
                    if (out_len != pack_compress_list->pack_info.origin_size) {
                        SPLIT_PRINT("xz decompress failed, the out len is not match.[%d, %d]\n", out_len, pack_compress_list->pack_info.origin_size);
                        return -1;
                    }
                    SPLIT_PRINT("Decompress and copy one image from %d to %d ok.\n", pack_compress_list->pack_info.compressed_size, out_len);
#else
                    SPLIT_PRINT("xz decompress not enable.\n");
                    return -1;
#endif /*CONFIG_XZ_CMP*/
                } else if (pack_compress_list->pack_info.compress_type == CAR_LZ4) {
#if defined(CONFIG_LZ4_COMP)
#if PARTITION_SPLIT_DEBUG
                    unsigned long src_addr = pre_offset + info->base_addr + info->start_addr + sizeof(pack_compress_list_t);
                    SPLIT_PRINT("start lz4 decompress src addr 0x%lx, compressed_size:%d\n", src_addr, pack_compress_list->pack_info.compressed_size);
#endif
                    off_t read_offset = sizeof(pack_compress_list_t) + pre_offset;
#if CONFIG_LZ4_BUFFER_ADDR == -1
#ifdef CONFIG_FLASH_XIP_BASE
                    char* const cmpBuf = (char *)(read_offset + info->base_addr + info->start_addr + CONFIG_FLASH_XIP_BASE);
#else
                    char* const cmpBuf = (char *)(read_offset + info->base_addr + info->start_addr);
#endif /*CONFIG_FLASH_XIP_BASE*/
                    SPLIT_PRINT("lz4 buffer use flash direct.\n");
#else
                    char* const cmpBuf = (char *)CONFIG_LZ4_BUFFER_ADDR;
                    ret = partition_read(partition, read_offset, cmpBuf, pack_compress_list->pack_info.compressed_size);
                    if (ret) {
                        SPLIT_PRINT("lz4 partition read e. ret:%d, read_offset:0x%lx, size:%d\n", ret, read_offset, pack_compress_list->pack_info.compressed_size);
                        return -1;
                    }
#endif /*CONFIG_LZ4_BUFFER_ADDR*/
                    char* const decBuf = (char *)(run_address + sizeof(pack_compress_list_t));

                    SPLIT_PRINT("src:0x%p, dst:0x%p, src_size:%d, dst_size:%d\n", cmpBuf, decBuf, pack_compress_list->pack_info.compressed_size, pack_compress_list->pack_info.origin_size);
                    int decompressed_size = LZ4_decompress_safe(cmpBuf, decBuf, pack_compress_list->pack_info.compressed_size, pack_compress_list->pack_info.origin_size);
                    if (decompressed_size < 0) {
                        SPLIT_PRINT("lz4 decompress e. decompressed_size:%d\n", decompressed_size);
                        return -1;
                    }
                    if (decompressed_size != pack_compress_list->pack_info.origin_size) {
                        SPLIT_PRINT("lz4 decompress failed, the out len is not match.[%d, %d]\n", decompressed_size, pack_compress_list->pack_info.origin_size);
                        return -1;
                    }
                    SPLIT_PRINT("Decompress and copy one image from %d to %d ok.\n", pack_compress_list->pack_info.compressed_size, decompressed_size);
#else
                    SPLIT_PRINT("lz4 decompress not enable.\n");
                    return -1;
#endif /*CONFIG_LZ4_COMP*/
                } else if (pack_compress_list->pack_info.compress_type > CAR_NULL) {
                    SPLIT_PRINT("Not support Compression Alogrithm Routine: %d\n", pack_compress_list->pack_info.compress_type);
                    return -1;
                } else {
                    SPLIT_PRINT("Copy normal image\n");
                    ret = partition_read(partition, pre_offset, (void *)run_address, olen);
                    if (ret) {
                        return ret;
                    }
                    SPLIT_PRINT("Copy normal image ok.\n");
                }
                return 0;
            }
            count ++;
            pre_offset += pack_compress_list->pack_info.next_offset + sizeof(pack_compress_list_t);
        } while (pack_compress_list->pack_info.next_offset != 0);
        SPLIT_PRINT("Nothing to copy.\n");
    }
    return -1;
}
