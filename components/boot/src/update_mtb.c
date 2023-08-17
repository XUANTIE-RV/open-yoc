/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if (CONFIG_NO_OTA_UPGRADE == 0)
#include <stdlib.h>
#include <string.h>
#include "mtb.h"
#include "update_mtb.h"
#include "update_misc_man.h"
#include "update_diff.h"
#include "boot_flash_porting.h"
#include "verify_wrapper.h"
#include "update_log.h"



/**
 * write flash
 *
 * @param[in]   man_flash_addr        manifest_flash_addr
 * @param[in]   update_mtb        write update_mtb
 * @param[in]   m_len        write m_len
 * @return      0 on success; -1 on failure
 */
static int program_manifest_tb(uint32_t man_flash_offset, uint8_t *update_mtb, uint32_t m_len)
{
    uint8_t * new_m = update_mtb;
    uint8_t * m_addr;
    mhead_tb *mtb;
    uint32_t  magic_offset;
    uint32_t  magic_data;

    m_addr = mtb_get_bmtb_size(new_m) + new_m;
    magic_offset = mtb_get_bmtb_size(new_m) + man_flash_offset;
    mtb = (mhead_tb *)m_addr;
    magic_data = mtb->magic;

    if (boot_device_erase(&mtb_get()->storage_info, man_flash_offset, m_len)) {
        UPD_LOGE("e e");
        return -1;
    }

    if (boot_device_write(&mtb_get()->storage_info, man_flash_offset + 4, new_m + 4, m_len - 4)) {
        UPD_LOGE("e w i");
        return -1;
    }

    if (boot_device_write(&mtb_get()->storage_info, magic_offset, (uint8_t *)(&magic_data), 4)) {
        UPD_LOGE("e w m");
        return -1;
    }

#if 1 //check flash
    uint8_t *tmpbuf = malloc(m_len);
    if (tmpbuf) {
        boot_device_read(&mtb_get()->storage_info, man_flash_offset, tmpbuf, m_len);
        if (memcmp(tmpbuf, update_mtb, m_len) != 0) {
            UPD_LOGE("check backup imtb error.");
            free(tmpbuf);
            return -1;
        }
        free(tmpbuf);
    }
#endif
    return 0;
}

/**
 * update  manifest.tb
 *
 * @param[in]   is_prim         is_prim or is_rcvy
 */
int update_img_mtb(uint32_t is_prim)
{
    return 0;
}

int mtb_update_ram(mtb_t *mtb)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    UPD_LOGD("update ram mtb");
    if (boot_device_read(&mtb->storage_info,
                        mtb->prim_offset, (void *)mtb->using_addr,
                        mtb->one_size > MAX_MANTB_BYTE_SIZE ? MAX_MANTB_BYTE_SIZE : mtb->one_size)) {
        UPD_LOGE("e read flash");
        return -1;
    }
#endif
    return 0;
}

int mtb_update_backup(mtb_t *mtb, const char *img_name, mtb_partition_info_t *partition_new_info, int *update_flag)
{
    int i;
    uint8_t *mtb_ram;
    imtb_head_v4_t *head;
    imtb_partition_info_v4_t *part_at;
    mtb_partition_info_t part_info;
    boot_device_info_t flash_info;

    UPD_LOGI("update backup mtb, %s", img_name);

    *update_flag = 1;
    mtb_ram = malloc(mtb->one_size);
    if (mtb_ram == NULL) {
        UPD_LOGE("mem overflow");
        return -1;
    }
    memset(mtb_ram, 0, mtb->one_size);
    if (boot_device_read(&mtb->storage_info, mtb->prim_offset, mtb_ram, mtb->one_size)) {
        UPD_LOGE("e read flash");
        free(mtb_ram);
        return -1;
    }
    if (!mtb_get_partition_info(img_name, &part_info)) {
        UPD_LOGD("part_info.img_size:0x%x, partition_new_info->img_size:0x%x", part_info.img_size, partition_new_info->img_size);
        if (partition_new_info->start_addr != part_info.start_addr ||
            partition_new_info->end_addr != part_info.end_addr ||
            partition_new_info->load_addr != part_info.load_addr ||
            partition_new_info->img_size != part_info.img_size
#if 0
            || strncmp(partition_new_info->pub_key_name, part_info.pub_key_name, PUBLIC_KEY_NAME_SIZE)
#else
            || partition_new_info->storage_info.id != part_info.storage_info.id
            || partition_new_info->storage_info.type != part_info.storage_info.type
            || partition_new_info->storage_info.area != part_info.storage_info.area
            || partition_new_info->preload_size != part_info.preload_size
#endif
            )
            {
            head = (imtb_head_v4_t *)mtb_ram;
            part_at = (imtb_partition_info_v4_t *)((uint8_t *)head + sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4);

            for (i = 0; i < head->partition_count; i++) {
                if (!strncmp(part_at->name, img_name, MTB_IMAGE_NAME_SIZE)) {
                    if (boot_device_info_get(&partition_new_info->storage_info, &flash_info)) {
                        free(mtb_ram);
                        return -1;
                    }
#if 0
                    strncpy(part_at->pub_key_name, partition_new_info->pub_key_name, PUBLIC_KEY_NAME_SIZE);
#else
                    memcpy(&part_at->storage_info, &partition_new_info->storage_info, sizeof(storage_info_t));
                    part_at->preload_size = partition_new_info->preload_size;
#endif
                    uint32_t blk_cnt = (partition_new_info->end_addr - partition_new_info->start_addr) / 512;
                    part_at->block_count_h = (blk_cnt >> 16) & 0xFFFF;
                    part_at->block_count = blk_cnt & 0xFFFF;
                    part_at->block_offset =  (partition_new_info->start_addr - flash_info.base_addr) / 512;
                    part_at->load_address = partition_new_info->load_addr;
                    part_at->img_size = partition_new_info->img_size;
                    UPD_LOGI("part_info.img_size:0x%x, partition_new_info->img_size:0x%x", part_info.img_size, partition_new_info->img_size);
                    break;
                }
                part_at ++;
            }
        }
    }
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#else
    // update version & crc32
    uint8_t *app_version_at;
    uint8_t version[MTB_OS_VERSION_LEN_V4];
    uint32_t olen;

    // update app version
    app_version_at = mtb_ram + sizeof(imtb_head_v4_t);
    misc_get_app_version(version, &olen);
    if (olen > MTB_OS_VERSION_LEN_V4) {
        olen = MTB_OS_VERSION_LEN_V4;
        version[MTB_OS_VERSION_LEN_V4 - 1] = 0;
    }
    UPD_LOGI("version:[%s]", version);
    memcpy(app_version_at, version, olen);
#endif
    imtb_head_v4_t *m_head = (imtb_head_v4_t *)mtb_ram;
    int crc_content_len = sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4 + \
                            m_head->partition_count * sizeof(imtb_partition_info_v4_t);
    uint8_t *crc_at = (uint8_t *)(mtb_ram + crc_content_len);
    uint32_t crc_cla;

    crc32_calc_start(mtb_ram, crc_content_len, &crc_cla);

    crc_at[0] = (crc_cla >> 24) & 0xFF;
    crc_at[1] = (crc_cla >> 16) & 0xFF;
    crc_at[2] = (crc_cla >> 8) & 0xFF;
    crc_at[3] = (crc_cla) & 0xFF;

    if (program_manifest_tb(mtb->backup_offset, mtb_ram, mtb->one_size)) {
        UPD_LOGE("program mtb e");
        free(mtb_ram);
        return -1;
    }
    free(mtb_ram);
    return 0;
}

int mtb_update_valid(mtb_t *mtb)
{
    uint8_t *mtb_ram = malloc(mtb->one_size);
    if (!mtb_ram) {
        UPD_LOGE("mem out");
        return -1;
    }
    UPD_LOGI("update valid mtb");
    if (mtb_ram == NULL) {
        UPD_LOGE("mem overflow");
        return -1;
    }
    memset(mtb_ram, 0, mtb->one_size);

    if (boot_device_read(&mtb_get()->storage_info, mtb->backup_offset, mtb_ram, mtb->one_size)) {
        UPD_LOGE("e r");
        goto fail;
    }
    if (program_manifest_tb(mtb->prim_offset, mtb_ram, mtb->one_size)) {
        UPD_LOGE("program mtb e");
        goto fail;
    }
    free(mtb_ram);
    return 0;
fail:
    free(mtb_ram);
    return -1;
}

#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <yoc/ota_ab_img.h>
int update_mtb_for_ab(const char *ab)
{
    int update_flag;
    download_img_info_t dl_img_info;

    int ret = otaab_env_img_info_init(ab, &dl_img_info, NULL);
    if (ret < 0) {
        UPD_LOGI("do not need update mtb.");
        return 0;
    }
    for (int i = 0; i < dl_img_info.image_count; i++) {
        mtb_partition_info_t part_old_info;
        if (mtb_get_partition_info(dl_img_info.img_info[i].partition_name, &part_old_info)) {
            UPD_LOGE("mtb get part info e");
            return -1;
        }
        if (part_old_info.img_size != dl_img_info.img_info[i].img_size
            || part_old_info.preload_size != dl_img_info.img_info[i].preload_size) {
            part_old_info.img_size = dl_img_info.img_info[i].img_size;
            part_old_info.preload_size = dl_img_info.img_info[i].preload_size;
            if (mtb_update_backup(mtb_get(), dl_img_info.img_info[i].partition_name, &part_old_info, &update_flag)) {
                UPD_LOGE("mtb update backup e");
                return -1;
            }
            if (mtb_update_valid(mtb_get())) {
                UPD_LOGE("mtb update valid e");
                return -1;
            }
            if (mtb_update_ram(mtb_get())) {
                UPD_LOGE("mtb update ram e");
                return -1;
            }
        }
    }
    return 0;
}
#endif

#endif