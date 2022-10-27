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
static int program_manifest_tb(uint32_t man_flash_addr, uint8_t *update_mtb, uint32_t m_len)
{
    uint8_t * new_m = update_mtb;
    uint8_t * m_addr;
    mhead_tb *mtb;
    uint32_t  magic_address;
    uint32_t  magic_data;

    m_addr = mtb_get_bmtb_size(new_m) + new_m;
    magic_address = mtb_get_bmtb_size(new_m) + man_flash_addr;
    mtb = (mhead_tb *)m_addr;
    magic_data = mtb->magic;

    if (boot_flash_erase(man_flash_addr, m_len)) {
        UPD_LOGE("e e");
        return -1;
    }

#if (CONFIG_MANTB_VERSION == 1)
    uint32_t bm_size = mtb_get_bmtb_size((uint32_t)update_mtb);

    if (boot_flash_write(man_flash_addr, new_m, bm_size)) {
        UPD_LOGE("e w i");
        return -1;
    }

    if (boot_flash_write(man_flash_addr + bm_size + 4, new_m + bm_size + 4, m_len - bm_size - 4)) {
        UPD_LOGE("e w i");
        return -1;
    }

    if (boot_flash_write(magic_address, (uint8_t *)(&magic_data), 4)) {
        UPD_LOGE("e w m");
        return -1;
    }
#elif (CONFIG_MANTB_VERSION == 2 || CONFIG_MANTB_VERSION == 4)
    if (boot_flash_write(man_flash_addr + 4, new_m + 4, m_len - 4)) {
        UPD_LOGE("e w i");
        return -1;
    }

    if (boot_flash_write(magic_address, (uint8_t *)(&magic_data), 4)) {
        UPD_LOGE("e w m");
        return -1;
    }
#endif /* CONFIG_MANTB_VERSION */

#if 1 //check flash
    uint8_t *tmpbuf = malloc(m_len);
    if (tmpbuf) {
        boot_flash_read(man_flash_addr, tmpbuf, m_len);
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
 * update img info by name
 *
 * @param[in]   im_addr         bootrom manifest.tb addr
 * @param[in]   type            type:0 update img info :1 update version
 * @param[in]   name         name
 * @param[in]   section           section  buf
 * @param[in]   section_len         section_len
 * @return      0 suc ; -1 on failure
 */
int update_imginfo(uint32_t im_addr, uint32_t type, uint8_t *name, uint8_t *section,
                   uint32_t section_len)
{
#if (CONFIG_MANTB_VERSION < 4)
    uint8_t *mtb_ram;
    uint32_t bmtb_len = mtb_get_size();
    uint32_t scn_img_addr;

    mtb_ram = malloc(bmtb_len);

    if (!mtb_ram) {
        UPD_LOGE("mem out");
        goto fail;
    }

    if (boot_flash_read(im_addr, mtb_ram, bmtb_len)) {
        UPD_LOGE("e r");
        goto fail;
    }

    if (type == UPDATE_TYPE_IMG_INFO) {
        if (mtb_get_img_scn_addr(mtb_ram, name, &scn_img_addr)) {
            goto fail;
        }
    } else if (type == UPDATE_TYPE_OS_VERSIN) {
        scn_img_addr = mtb_get_bmtb_size(mtb_ram) + sizeof(mhead_tb);
    } else {
        UPD_LOGE("e u t");
        goto fail;
    }

    memcpy((uint8_t *)scn_img_addr, section, section_len);

    if (program_manifest_tb(im_addr, (uint8_t *)mtb_ram, bmtb_len)) {
        UPD_LOGE("e u imginfo");
        return -1;
    }

    if (mtb_ram) {
        free(mtb_ram);
    }

    return 0;
fail:

    if (mtb_ram) {
        free(mtb_ram);
    }

    return -1;
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
#if (CONFIG_MANTB_VERSION < 4)
    uint32_t pim_addr;
    uint32_t rim_addr;

    if (is_prim) {
        pim_addr = mtb_get_addr(1);
        rim_addr = mtb_get_addr(0);
    } else {
        rim_addr = mtb_get_addr(1);
        pim_addr = mtb_get_addr(0);
    }

    if (program_manifest_tb(pim_addr,
                            (uint8_t *)rim_addr,
                            mtb_get()->one_size)) {
        UPD_LOGE("e u prim:%d\n", is_prim);
        return -1;
    }
#endif
    return 0;
}

int mtb_update_ram(mtb_t *mtb)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    UPD_LOGD("update ram mtb");
    if (boot_flash_read(mtb->prim_addr, 
                        (void *)mtb->using_addr,
                        mtb->one_size > MAX_MANTB_BYTE_SIZE ? MAX_MANTB_BYTE_SIZE : mtb->one_size)) {
        UPD_LOGE("e read flash");
        return -1;
    }
#endif
    return 0;
}

int mtb_update_backup(mtb_t *mtb, const char *img_name, mtb_partition_info_t *partition_info, int *update_flag)
{
    int i;
    uint8_t *mtb_ram;
    imtb_head_v4_t *head;
    imtb_partition_info_v4_t *part_at;
    mtb_partition_info_t part_info;
    boot_flash_info_t flash_info;

    UPD_LOGI("update backup mtb, %s", img_name);

    *update_flag = 1;
    mtb_ram = malloc(mtb->one_size);
    if (mtb_ram == NULL) {
        UPD_LOGE("mem overflow");
        return -1;
    }
    memset(mtb_ram, 0, mtb->one_size);
    if (boot_flash_read(mtb->prim_addr, mtb_ram, mtb->one_size)) {
        UPD_LOGE("e read flash");
        free(mtb_ram);
        return -1;
    }
    if (!mtb_get_partition_info(img_name, &part_info)) {
        UPD_LOGD("part_info.img_size:0x%x, partition_info->img_size:0x%x", part_info.img_size, partition_info->img_size);
        if (partition_info->start_addr != part_info.start_addr ||
            partition_info->end_addr != part_info.end_addr ||
            partition_info->load_addr != part_info.load_addr ||
            partition_info->img_size != part_info.img_size ||
            partition_info->part_type.father_type.first_type != part_info.part_type.father_type.first_type ||
            partition_info->part_type.father_type.extend_type != part_info.part_type.father_type.extend_type ||
            partition_info->part_type.son_type != part_info.part_type.son_type ||
            strncmp(partition_info->pub_key_name, part_info.pub_key_name, PUBLIC_KEY_NAME_SIZE))
            {
            head = (imtb_head_v4_t *)mtb_ram;
            part_at = (imtb_partition_info_v4_t *)((uint8_t *)head + sizeof(imtb_head_v4_t) + MTB_OS_VERSION_LEN_V4);

            for (i = 0; i < head->partition_count; i++) {
                if (!strncmp(part_at->name, img_name, MTB_IMAGE_NAME_SIZE)) {
                    int flashid = partition_info->part_type.son_type;
                    boot_flash_info_get(flashid, &flash_info);
                    strncpy(part_at->pub_key_name, partition_info->pub_key_name, PUBLIC_KEY_NAME_SIZE);
                    part_at->partition_type.father_type = partition_info->part_type.father_type;
                    part_at->partition_type.son_type = partition_info->part_type.son_type;
                    part_at->block_count = (partition_info->end_addr - partition_info->start_addr) / 512;
                    part_at->block_offset =  (partition_info->start_addr - flash_info.start_addr) / 512;
                    part_at->load_address = partition_info->load_addr;
                    part_at->img_size = partition_info->img_size;
                    UPD_LOGI("part_info.img_size:0x%x, partition_info->img_size:0x%x", part_info.img_size, partition_info->img_size);
                    break;
                }
                part_at ++;
            }
        }
    }
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

    if (program_manifest_tb(mtb->backup_addr, mtb_ram, mtb->one_size)) {
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

    if (boot_flash_read(mtb->backup_addr, mtb_ram, mtb->one_size)) {
        UPD_LOGE("e r");
        goto fail;
    }
    if (program_manifest_tb(mtb->prim_addr, mtb_ram, mtb->one_size)) {
        UPD_LOGE("program mtb e");
        goto fail;
    }
    free(mtb_ram);
    return 0;
fail:
    free(mtb_ram);
    return -1;
}
#endif