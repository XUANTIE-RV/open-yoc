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
#include "mtb.h"
#include "mtb_internal.h"
#include "mtb_log.h"
#include "yoc/partition_device.h"
#include "yoc/partition.h"

static mtb_t g_mtb;
#ifdef CONFIG_NON_ADDRESS_FLASH
uint8_t __attribute__((aligned(4))) g_manfest_tb_ram[MAX_MANTB_BYTE_SIZE];
#endif

static int mtb_verify(void);
static int mtb_crc_check(void);

int mtb_init(void)
{
    int i;
    int ret = -1;
    uint32_t cur_offset;
    uint64_t flash_length;
    storage_info_t storage_info;
    partition_device_ops_t *handle;
    partition_device_info_t flash_info;

    // MTB must be in device0; must be in user area if eMMC
    i = 0;
    while(i < MEM_DEVICE_TYPE_MAX) {
#if !defined(CONFIG_MTB_IN_SD_DEVICE)
        if (i == MEM_DEVICE_TYPE_SD) {
            MTB_LOGD("mtb skip to find dev type: %d", i);
            i++;
            continue;
        }
#endif
#if !defined(CONFIG_MTB_IN_USB_DEVICE)
        if (i == MEM_DEVICE_TYPE_USB) {
            MTB_LOGD("mtb skip to find dev type: %d", i);
            i++;
            continue;
        }
#endif
        storage_info.type = i;
        storage_info.id = 0;
        storage_info.area = MEM_EMMC_DEVICE_AREA_USER;
        MTB_LOGD("mtb try to find dev type:%d, id:%d", storage_info.type, storage_info.id);
        handle = partition_device_find(&storage_info);
        if (handle && handle->dev_hdl) {
            MTB_LOGD("find type:%d, id:%d ok.", storage_info.type, storage_info.id);
            break;
        }
        i++;
    }
    if (i == MEM_DEVICE_TYPE_MAX) {
        MTB_LOGE("can't find mtb in devices.");
        return -1;
    }
    ret = partition_device_info_get(handle, &flash_info);
    if (ret < 0) {
        MTB_LOGE("get device info e.[%d]", ret);
        return -1;
    }

    mtb_head_t m_buf;
    int bmtb_size = 0;
    int imtb_size = 0;
    i = 0;
    flash_length = flash_info.device_size;
    while(1) {
        if (flash_info.sector_size > 0) {
            if ((uint64_t)i * flash_info.sector_size == flash_info.device_size) {
                MTB_LOGE("mtb cant find magic");
                return -1;
            }
            cur_offset = i * flash_info.sector_size;
            ret = partition_device_read(handle, cur_offset, &m_buf, sizeof(mtb_head_t));
        } else if (flash_info.block_size > 0) {
            if ((uint64_t)i * flash_info.block_size == flash_info.device_size) {
                MTB_LOGE("mtb cant find magic");
                return -1;
            }
            cur_offset = i * flash_info.block_size;
            ret = partition_device_read(handle, cur_offset, &m_buf, sizeof(mtb_head_t));
        } else {
            MTB_LOGE("device sector or block size error.");
            return -1;
        }
        g_mtb.using_addr = cur_offset;
        if (!ret) {
            if (m_buf.head.magic == BMTB_MAGIC) {
                MTB_LOGD("find bmtb %d", m_buf.head.version);
                if (m_buf.head.version == 1) {
                    if ((uint64_t)g_mtb.using_addr + m_buf.head.size < flash_length) {
                        mtb_head_t i_buf;
                        if (!partition_device_read(handle, cur_offset + m_buf.head.size,
                                                &i_buf, sizeof(mtb_head_t))) {
                            if (i_buf.head.magic == MTB_MAGIC) {
                                bmtb_size = m_buf.head.size;
                                g_mtb.prim_offset = cur_offset;
                                imtb_size = i_buf.head.size;
                                g_mtb.version = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (m_buf.head.magic == MTB_MAGIC) {
                MTB_LOGD("find imtb %d", m_buf.head.version);
                g_mtb.prim_offset = cur_offset;
                g_mtb.version = m_buf.head.version;
                if (m_buf.head.version < 0x4) {
                    imtb_size = m_buf.head.size;
                } else {
                    imtb_size = m_buf.headv4.size;
                }
                MTB_LOGD("imtb_size:%d", imtb_size);
                break;
            }
        }
        i++;
    }

    if (imtb_size > MAX_MANTB_BYTE_SIZE) {
        MTB_LOGE("mtb size overflow");
        return -1;
    }
#ifdef CONFIG_NON_ADDRESS_FLASH
    if (partition_device_read(handle, g_mtb.using_addr + bmtb_size, g_manfest_tb_ram, imtb_size)) {
        return -1;
    }
    g_mtb.using_addr = (unsigned long)g_manfest_tb_ram;
#else
    g_mtb.using_addr += flash_info.base_addr;
#endif
    g_mtb.i_offset = bmtb_size;

    if (g_mtb.version < 4) {
        MTB_LOGE("mtb version must be >= 4.");
        return -1;
    } else {
        ret = mtbv4_init();
    }
    if (ret == 0) {
        ret = mtb_crc_check();
    }
    return ret;
}

static int mtb_crc_check(void)
{
    int ret = 0;
    ret = mtbv4_crc_check();
    return ret;
}

static int mtb_verify(void)
{
    int ret = -1;
    ret = mtbv4_verify();
    return ret;
}

int mtb_image_verify(const char *name)
{
    int ret = 0;

    if (name == NULL) {
        return -EINVAL;
    }

    if (strncmp(name, MTB_IMAGE_NAME_IMTB, MTB_IMAGE_NAME_SIZE) == 0) {
        ret = mtb_verify();
    } else {
#if (CONFIG_PARITION_NO_VERIFY == 0)
        ret = mtbv4_image_verify(name);
#endif
    }
    return ret;
}

uint32_t mtb_get_bmtb_size(const uint8_t *data)
{
    mhead_tb *phead = (mhead_tb *)data;

    if (phead && phead->version == 1) {
        return phead->size;
    }
    return 0;
}

uint32_t mtb_get_size(void)
{
    uint32_t size = 0;
    mhead_tb *phead = (mhead_tb *)g_mtb.using_addr;

    size = phead->size;

    return size;
}

int mtb_get_partition_info(const char *name, mtb_partition_info_t *part_info)
{
    if (!(name && part_info)) {
        return -EINVAL;
    }
    return mtbv4_get_partition_info(name, part_info);
}

int mtb_get_img_info(const char *name, img_info_t *img_info)
{
    if (!(name && img_info)) {
        return -EINVAL;
    }
    mtb_partition_info_t part_info;
    if (mtb_get_partition_info((const char *)name, &part_info)) {
        return -1;
    }
    strncpy((char *)(img_info->img_name), part_info.name, sizeof((img_info->img_name)));
    img_info->img_name[sizeof((img_info->img_name)) - 1] = '\0';
    img_info->img_addr =  part_info.start_addr;
    img_info->img_size =  part_info.img_size;
    img_info->img_type =  1; // FIXME:
    img_info->img_part_size = part_info.end_addr - part_info.start_addr;
    memcpy(&img_info->storage_info, &part_info.storage_info, sizeof(storage_info_t));
    return 0;
}

int get_section_buf(uint32_t addr, uint32_t img_len, scn_type_t *scn_type, uint32_t *scn_size, img_info_t *img_info)
{
    int ret;
    uint32_t size;
    uint8_t buf[sizeof(scn_head_t)];
    partition_device_info_t device_info;

    if (!(scn_type && scn_size && img_len > 0)) {
        return -EINVAL;
    }

    MTB_LOGD("addr:0x%x, img_len:%d", addr, img_len);
    MTB_LOGD("device_type:%d, device_id:%d, area:%d", img_info->storage_info.type, img_info->storage_info.id, img_info->storage_info.area);
    void *hdl = partition_device_find(&img_info->storage_info);
    if (partition_device_info_get(hdl, &device_info)) {
        MTB_LOGE("get dev info e");
        return -1;
    }
    MTB_LOGD("r offset:0x%x", (uint32_t)(addr - device_info.base_addr));
    ret = partition_device_read(hdl, (uint32_t)(addr - device_info.base_addr), buf, sizeof(scn_head_t));
    if (ret < 0) {
        MTB_LOGE("read dev e");
        return -1;
    }
    memcpy(&scn_type->father_type, buf, sizeof(scn_father_type_t));
    scn_type->son_type = buf[1];

    size = buf[4] + ((uint32_t)buf[5] << 8) + ((uint32_t)buf[6] << 16) + ((uint32_t)buf[7] << 24);
    MTB_LOGD("fatype=%d,sontype=%d,size=%d", *(uint8_t *)(&scn_type->father_type), scn_type->son_type, size);

    if (size > img_len) {
        MTB_LOGE("section size larger, size:%d, img_len:%d", size, img_len);
        return -1;
    }

    *scn_size = size;
    return 0;
}

int mtb_get_img_scn_addr(uint8_t *mtb_buf, const char *name, unsigned long *scn_addr)
{
    int ret;

    if (!(mtb_buf && name && scn_addr)) {
        return -EINVAL;
    }
    ret = get_section_addr((unsigned long)mtb_buf, &(g_scn_img_type[SCN_SUB_TYPE_IMG_NOR]), (uint8_t *)name,
                     (unsigned long *)scn_addr, SEACH_MODE_FIRST_TYPE);
    return ret;
}

int get_sys_partition(uint8_t *out, uint32_t *out_len)
{
    int ret = 0;

    MTB_LOGD("get_sys_partition----------");
    MTB_LOGD("mtb.prim_offset: 0x%lx", g_mtb.prim_offset);
    MTB_LOGD("mtb.backup_offset: 0x%lx", g_mtb.backup_offset);
    MTB_LOGD("mtb.using_addr: 0x%lx", g_mtb.using_addr);
    MTB_LOGD("mtb.one_size: 0x%x", g_mtb.one_size);
    MTB_LOGD("mtb.version: 0x%x", g_mtb.version);
    MTB_LOGD("mtb.device_type: 0x%x", g_mtb.storage_info.type);
    MTB_LOGD("mtb.device_id: 0x%x", g_mtb.storage_info.id);
    MTB_LOGD("mtb.device_area: 0x%x", g_mtb.storage_info.area);

    if (!(out && out_len)) {
        return -EINVAL;
    }

    int i, count, len;
    sys_partition_info_t *p = (sys_partition_info_t *)out;

    count = mtbv4_partition_count();

    for (i = 0; i < count; i++) {
        mtb_partition_info_t part_info;
        if (mtbv4_get_partition_info_with_index(i, &part_info)) {
            return -1;
        }
        len = strlen(part_info.name);
        len = len > (MTB_IMAGE_NAME_SIZE - 1) ? (MTB_IMAGE_NAME_SIZE - 1) : len;
        memcpy(p->image_name, part_info.name, len);
        p->image_name[len] = 0;

        p->part_addr = part_info.start_addr;
        p->part_size = part_info.end_addr - part_info.start_addr;
        p->image_size = part_info.img_size;
        p->load_addr = part_info.load_addr;
        p->preload_size = part_info.preload_size;
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
        memcpy(&p->storage_info, &part_info.storage_info, sizeof(storage_info_t));
#endif
        p ++;
    }
    *out_len = sizeof(sys_partition_info_t) * count;

	return ret;
}

int get_app_version(uint8_t *out, uint32_t *out_len)
{
    int head_size;
    int version_max_size = MTB_OS_VERSION_LEN_V4;

    if (!(out && out_len)) {
        return -EINVAL;
    }

    head_size = sizeof(imtb_head_v4_t);
    version_max_size = MTB_OS_VERSION_LEN_V4;

    memcpy(out, (uint8_t *)(g_mtb.using_addr + head_size), version_max_size);
    out[version_max_size - 1] = 0;
    *out_len = version_max_size;
    return 0;
}

mtb_t *mtb_get(void)
{
    return &g_mtb;
}