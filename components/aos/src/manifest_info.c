/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     tee_if.c
 * @brief
 * @version  V1.0
 * @date     25. May 2018
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <aos/log.h>
#include <aos/debug.h>
#include <yoc/manifest_info.h>
#include <devices/flash.h>

#ifndef CONFIG_TEE_CA

static const char *TAG = "MTB";

/*
如果flash可直接地址访问, mtb_addr,mtb_value可以直接是bmtb地址，
返回 mtb_addr + bmtb 得到imtb起始地址

如果flash不能直接地址访问，mtb_addr 可以是flash地址，
mtb_value 必须是ram地址(预先bmtb头到ram)，最后获取imtb地址
*/

#define PADDING_SIZE(src_size,padd) 			(((src_size+padd-1)/padd)*padd)
#define PADDING_FLASH(src) 						PADDING_SIZE(src,FLASH_IMG_SETCOR)

#define NEXT_SECTION(scn_addr) ((uint32_t)scn_addr + ((scn_head_t *)scn_addr)->size)
#define GET_SECTION_IMG_INFO_ADDR(scn_addr) ((uint32_t)((uint32_t)scn_addr+ sizeof(scn_head_t)))
#define GET_SECTION_IMG_INFO_PTR(scn_addr)  ((scn_img_sig_info_t *)(GET_SECTION_IMG_INFO_ADDR(scn_addr)))
#define GET_PARTION_SCN_ADDR(mtb,scn_addr) get_section_addr(mtb,&(g_scn_part_type[SCN_SUB_TYPE_IMG_PART]),NULL,(uint32_t *)scn_addr,SEACH_MODE_EXTEND_TYPE)

const scn_info_t g_scn_part_type[] = {
    {{SCN_TYPE_NORMAL, SCN_TYPE_PART}, SCN_SUB_TYPE_IMG_PART}
};

#ifdef CONFIG_NON_ADDRESS_FLASH
static uint32_t  g_manfest_tb_ram[MAX_MANTB_BYTE_SIZE/4];
#endif
static mtb_t g_mtb = {0};
/*
    get the first section offset
 * @param[in]    mtb         mtb handle
 * @param[out]     offset      offset
 * @return      0 on success; -1 on failure
 */
static int get_scn_offset(mtb_t *mtb, uint32_t *offset)
{
    uint32_t m_addr = mtb->using_addr;
    uint32_t size_ver = 0;

    if (mtb->version == 0x1) {
        size_ver = sizeof(os_version_t1);
    } else if (mtb->version == 0x2) {
        size_ver = sizeof(os_version_t2);
    } else if (mtb->version == 0x4) {
        // there is no scn in mtbv4
        return -1;
    } else {
        LOGE(TAG, "ver e");
        return -1;
    }

    *offset = (uint32_t)(m_addr + sizeof(mhead_tb) + size_ver);
    return 0;
}

/*
    get the  section address by type or name
 * @param[in]    im_addr         bootrom manfest address
 * @param[in]     seach_type      seach_type
 * @param[in]     seach_name      seach_name
 * @param[out]     scn_addr      scn_addr
 * @param[in]     mode          SEACH_MODE_FIRST_TYPE,SEACH_MODE_SON_TYPE or SEACH_MODE_EXTEND_TYPE
 * @return      0 on success; -1 on failure
 */
static int get_section_addr(mtb_t *mtb, const scn_info_t *seach_type, uint8_t *seach_name, uint32_t *scn_addr, uint32_t mode)
{
    uint32_t m_addr = mtb->using_addr;
    scn_head_t *scn_t = NULL;
    scn_img_sig_info_t *img_info_t;
    uint32_t i = 0;
    uint32_t sum_scn = ((mhead_tb *)m_addr)->scn_count;

    uint32_t name_len;
    uint8_t s_name[MTB_IMAGE_NAME_SIZE];

    memset(s_name, 0, sizeof(s_name));

    if (seach_name == NULL) {
        name_len = 0;
    } else {
        name_len = strlen((const char *)seach_name);

        if (name_len > MTB_IMAGE_NAME_SIZE) {
            name_len = MTB_IMAGE_NAME_SIZE - 1;
        }

        memcpy(s_name, seach_name, name_len);
    }

    if (get_scn_offset(mtb, (uint32_t *)(&scn_t))) {
        return -1;
    }

    //printf("seach_name:%s,seach_type:%d,mode:%d \n", seach_name, seach_type->father_type.first_type , mode);

    for (; i < sum_scn ; i++) {
        //printf("get_section_addr:%x \n",(uint32_t)scn_t);
        if (scn_t->type.father_type.first_type == seach_type->father_type.first_type) {
            if (mode == SEACH_MODE_FIRST_TYPE || ((mode == SEACH_MODE_SON_TYPE) && scn_t->type.son_type == seach_type->son_type) ||
                (mode == SEACH_MODE_EXTEND_TYPE && scn_t->type.son_type == seach_type->son_type && scn_t->type.father_type.extend_type == seach_type->father_type.extend_type)) {
                if (seach_name == NULL) {
                    goto succ;
                } else {
                    img_info_t = GET_SECTION_IMG_INFO_PTR(scn_t);
                    //printf("seach_type:%d,name:%s,s_name:%s \n", seach_type->father_type.first_type, (char *)(img_info_t->image_name), s_name);

                    if (!strncmp((char *)(img_info_t->image_name), (char *)s_name, MTB_IMAGE_NAME_SIZE)) {
                        goto succ;
                    }
                }
            }
        }

        scn_t = (scn_head_t *)(NEXT_SECTION(scn_t));
    }

    //printf("not seach_type:%d \n", seach_type->father_type.first_type);
    return -1;
succ:
    //printf("s scn_t:%x \n", (uint32_t)scn_t);
    *scn_addr = (uint32_t)scn_t;
    return 0;
}

static int mtb_find_addr(mtb_t *mtb)
{
    uint32_t flash_addr = 0;
    uint32_t flash_end;
    aos_dev_t *flash_dev = NULL;
    flash_dev_info_t flash_info;

    flash_dev = flash_open("eflash0"); // FIXME:
    if (flash_dev == NULL) {
        LOGE(TAG, "flash open e");
        return -1;
    }

    int ret = flash_get_info(flash_dev, &flash_info);
    if (ret != 0) {
        goto fail;
    }

    flash_addr = flash_info.start_addr;
    flash_end = flash_addr + flash_info.block_size * flash_info.block_count;
    mtb->flash_start = flash_info.start_addr;

    LOGD(TAG, "flash_info.start_addr:0x%08x", flash_info.start_addr);
    LOGD(TAG, "flash_info.block_size:0x%x", flash_info.block_size);
    LOGD(TAG, "flash_info.block_count:%d", flash_info.block_count);
    LOGD(TAG, "flash_end:0x%x", flash_end);
#ifdef CONFIG_NON_ADDRESS_FLASH
    mtb_head_t m_buf;
    int bmtb_size = 0;
    int imtb_size = 0;

    while(flash_addr < flash_end) {
        if (!flash_read(flash_dev, \
                        flash_addr - flash_info.start_addr, (uint8_t *)&m_buf, \
                        sizeof(mtb_head_t))) {
            if (m_buf.head.magic == BMTB_MAGIC) {
                LOGD(TAG,"find bmtb %d", m_buf.head.version);
                if (m_buf.head.version == 1) {
                    if (flash_addr + m_buf.head.size < flash_end) {
                        mtb_head_t i_buf;
                        if (!flash_read(flash_dev, \
                                        flash_addr - flash_info.start_addr + m_buf.head.size, \
                                        (uint8_t *)&i_buf, sizeof(mtb_head_t))) {
                            if (i_buf.head.magic == MTB_MAGIC) {
                                bmtb_size = m_buf.head.size;
                                mtb->valid_addr = flash_addr;
                                imtb_size = i_buf.head.size;
                                mtb->version = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (m_buf.head.magic == MTB_MAGIC) {
                LOGD(TAG,"find imtb %d", m_buf.head.version);
                mtb->valid_addr = flash_addr;
                mtb->version = m_buf.head.version;
                if (m_buf.head.version < 0x4) {
                    imtb_size = m_buf.head.size;
                } else {
                    imtb_size = m_buf.headv4.size;
                }
                LOGD(TAG, "imtb_size:%d", imtb_size);
                break;
            }
        }
        flash_addr += flash_info.block_size;
    }

    if (flash_addr == flash_end) {
        LOGE(TAG, "mtb cant find magic");
        goto fail;
    }

    if (imtb_size > MAX_MANTB_BYTE_SIZE) {
        LOGE(TAG, "mtb size overflow");
        goto fail;
    }

    if (flash_read(flash_dev, \
                    flash_addr - flash_info.start_addr + bmtb_size, \
                    (uint8_t *)g_manfest_tb_ram, imtb_size)) {
        goto fail;
    }

    mtb->using_addr = (uint32_t)g_manfest_tb_ram;

#else
    while(flash_addr < flash_end) {
        mhead_tb *phead = (mhead_tb *)flash_addr;
        if (phead->magic == BMTB_MAGIC) {
            LOGD(TAG,"find bmtb %d", phead->version);
            // version 1
            if (phead->version == 0x1) {
                LOGD(TAG,"size %d", phead->size);
                if (flash_addr + phead->size < flash_end) {
                    mhead_tb *ihead = (mhead_tb *)(flash_addr + phead->size);
                    if (ihead->magic == MTB_MAGIC) {
                        mtb->valid_addr = flash_addr;
                        mtb->using_addr = flash_addr + phead->size;
                        mtb->version = 1;
                        LOGD(TAG, "got mtb1");
                        break;
                    }
                }
            }
        }
        if (phead->magic == MTB_MAGIC) {
            mtb->valid_addr = flash_addr;
            mtb->using_addr = flash_addr;
            mtb->version = phead->version;
            LOGD(TAG, "got mtb%d", mtb->version);
            break;
        }
        flash_addr += flash_info.block_size;
    }

    if (flash_addr == flash_end) {
        LOGE(TAG, "mtb find magic e\n");
        goto fail;
    }
#endif
    LOGD(TAG, "mtb find 0x%x", mtb->valid_addr);
    flash_close(flash_dev);
    return 0;
fail:
    flash_close(flash_dev);
    return -1;
}

static int partition_info_init(mtb_t *mtb, uint8_t *buf, uint32_t *buf_size)
{
    int i;
    uint32_t size;
    imtb_head_v4_t *m_head = (imtb_head_v4_t *)mtb->using_addr;
    imtb_partition_info_v4_t *m_partitions = (imtb_partition_info_v4_t *)(mtb->using_addr + sizeof(imtb_head_v4_t) + MTB_APP_VERSION_MAX_SIZE);
    struct partion_info_t *p = (struct partion_info_t *)buf;

    for (i = 0; i < m_head->partition_count; i++) {
        memcpy(p->image_name, m_partitions->name, MTB_IMAGE_NAME_SIZE);
        p->part_addr = mtb->flash_start + m_partitions->block_offset * 512;
        p->part_size = m_partitions->block_count * 512;
        m_partitions ++;
        p ++;
    }
    size = m_head->partition_count * sizeof(struct partion_info_t);
    LOGD(TAG, "mtb->using_addr:            0x%08x", mtb->using_addr);
    LOGD(TAG, "mtb->valid_addr:            0x%08x", mtb->valid_addr);
    LOGD(TAG, "m_head->partition_count:    %d", m_head->partition_count);
    if (size > *buf_size) {
        LOGD(TAG, "partition init size overflow!!!");
        return -1;
    }
    *buf_size = size;
    return 0;
}

/**
  \brief       TEE obtain partition buffer from manifest table
  \param[out]  out     Pointer to partition buffer
  \param[out]  out_len Partition buffer length,if info obtain successfully,
                       out_len is updated to actual partition buffer sizes
  \return      return 0 if successful,otherwise error code
*/
int32_t get_sys_partition(uint8_t *out, uint32_t *out_len)
{
    int ret = 0;
    LOGD(TAG, "get_sys_partition----------");

    if (mtb_find_addr(&g_mtb)) {
        LOGE(TAG, "cant find mtb");
        return -1;
    }
    LOGD(TAG, "mtb.valid_addr: 0x%x", g_mtb.valid_addr);
    LOGD(TAG, "mtb.backup_addr: 0x%x", g_mtb.backup_addr);
    LOGD(TAG, "mtb.using_addr: 0x%x", g_mtb.using_addr);
    LOGD(TAG, "mtb.one_size: 0x%x", g_mtb.one_size);
    LOGD(TAG, "mtb.flash_start: 0x%x", g_mtb.flash_start);
    LOGD(TAG, "mtb.version: 0x%x", g_mtb.version);
    if (g_mtb.version < 4) {
        scn_head_t *partition_addr;
        uint8_t *ret_addr = NULL;

        if (GET_PARTION_SCN_ADDR(&g_mtb, &ret_addr) < 0) {
            return -1;
        }
        partition_addr = (scn_head_t *)ret_addr;
        ret_addr += sizeof(scn_head_t);
        *out_len = partition_addr->size - sizeof(scn_head_t);
        memcpy(out, ret_addr, *out_len);
    } else {
        ret = partition_info_init(&g_mtb, out, out_len);
    }

	return ret;
}

int32_t get_app_version(uint8_t *out, uint32_t *out_len)
{
    uint16_t mtb_version = g_mtb.version;
    int head_size;
    int version_max_size;

    if (mtb_version < 4) {
        head_size = sizeof(mhead_tb);
        version_max_size = MTB_OS_VERSION2;
        if (mtb_version == 1) {
            version_max_size = MTB_OS_VERSION1;
        }
    } else {
        head_size = sizeof(imtb_head_v4_t);
        version_max_size = MTB_APP_VERSION_MAX_SIZE;
    }

    memcpy(out, (uint8_t *)(g_mtb.using_addr + head_size), version_max_size);
    out[version_max_size - 1] = 0;
    *out_len = version_max_size;
    return 0;
}

#endif
