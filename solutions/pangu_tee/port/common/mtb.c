/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <kp.h>
#include <mtb.h>
#include <tee_debug.h>
#include <tee_addr_map.h>
#include <crypto.h>
#include <reboot.h>

#define MTB_OK                0
#define MTB_ERR_MAGIC_NUMBER -1
#define MTB_ERR_VERSION      -2
#define MTB_ERR_INIT         -3
#define MTB_ERR_GENERIC      -4
#define MTB_ERR_NULL         -5
#define MTB_ERR_VERIFY       -6
#define MTB_ERR_NOT_FOUND    -7

typedef enum {
    S_FARTHER_TYPE = 0,
    S_SON_TYPE,
} search_mode_e;

static mtb_t g_mtb = {0};
static uint8_t g_mtb_init = 0;
static scn_t *_get_section(search_mode_e mode, scn_head_type_t *type, const char *img_name);

#define IMG_SCN_HEAD_TYPE       (&(scn_head_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_IMG }, SCN_SUB_TYPE_IMG_NOR })
#define SAFE_IMG_SCN_HEAD_TYPE  (&(scn_head_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_IMG }, SCN_SUB_TYPE_IMG_SAFE})
#define PARTITION_SCN_HEAD_TYPE (&(scn_head_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_PART}, SCN_SUB_TYPE_IMG_PART})
#define KEY_SCN_HEAD_TYPE       (&(scn_head_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_KEY }, SCN_SUB_TYPE_KEY_BOOT})

#define SCN_SIZE(scn)   (scn->scn_head.size)
#define SCN_CONTENT_SIZE(scn) (scn->scn_head.size - sizeof(scn_head_t))
#define SCN_IMG_SIGN(scn) ((uint8_t *)scn->img + sizeof(scn_img_sig_info_t))
#define SCN_IMG_SIGN_SIZE(scn) (SCN_CONTENT_SIZE(scn) - sizeof(scn_img_sig_info_t))
#define NEXT_SCN(scn)   (scn = (scn_t *)((uint8_t *)scn + scn->scn_head.size))
#define FIRST_IMG_SCN() _get_section(S_FARTHER_TYPE, IMG_SCN_HEAD_TYPE, NULL)
#define IMG_SCN(name)   _get_section(S_SON_TYPE, IMG_SCN_HEAD_TYPE, name)
#define KEY_SCN         _get_section(S_SON_TYPE, KEY_SCN_HEAD_TYPE, NULL)
#define PART_SCN        _get_section(S_SON_TYPE, PARTITION_SCN_HEAD_TYPE, NULL)

#if (CONFIG_TB_KP == 0)
const uint8_t test_key_n[] = {
    0xB9, 0xC2, 0x6A, 0x88, 0x90, 0xB0, 0xE1, 0x1B,
    0xD3, 0x3F, 0x93, 0x99, 0xF8, 0xCB, 0xC4, 0xF2,
    0x88, 0xB8, 0x56, 0x9A, 0x93, 0x24, 0x0B, 0x4A,
    0x75, 0xB4, 0x8D, 0x6C, 0x32, 0x40, 0x52, 0xA7,
    0x3D, 0x2B, 0x37, 0xCE, 0x57, 0x80, 0xFD, 0xA5,
    0x08, 0xC7, 0x4A, 0x77, 0x94, 0x34, 0xBA, 0x56,
    0x9F, 0xBA, 0x2E, 0xBA, 0x20, 0x1E, 0x80, 0x4A,
    0x09, 0x40, 0x07, 0xF5, 0x1D, 0xB6, 0xFC, 0x8E,
    0x5C, 0x4E, 0x02, 0x01, 0xD5, 0x97, 0x14, 0x83,
    0x2A, 0x32, 0x90, 0x4E, 0xED, 0xDC, 0x6F, 0xD6,
    0x0A, 0x1F, 0x44, 0x6A, 0xB1, 0xFE, 0x42, 0xBD,
    0xBF, 0xD7, 0x6F, 0xB5, 0x9B, 0x78, 0x5F, 0x19,
    0x07, 0x15, 0x4E, 0x65, 0xD2, 0x72, 0x09, 0x0C,
    0x14, 0xAD, 0xE8, 0xC7, 0x76, 0x25, 0xE8, 0x94,
    0xF8, 0xEE, 0x19, 0x93, 0xDF, 0xC9, 0x34, 0xEE,
    0x42, 0x1A, 0xA6, 0xA8, 0x70, 0xE2, 0xC0, 0x99,
};
#endif

const uint8_t test_key_e[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
};

#if (CONFIG_TB_KP == 0)
const uint8_t test_key_d[] = {
    0x0D, 0x59, 0xA1, 0x9A, 0x6A, 0xFB, 0xAB, 0x51,
    0xFA, 0x9E, 0xA9, 0x16, 0xF9, 0xC0, 0x7B, 0x49,
    0x0A, 0xF6, 0x64, 0x92, 0xFB, 0x5C, 0x2D, 0xCB,
    0x8E, 0x26, 0x09, 0xBB, 0xEA, 0xE8, 0xA0, 0x88,
    0x10, 0x67, 0x58, 0xBD, 0x65, 0xA0, 0x89, 0x9A,
    0x20, 0xC5, 0x61, 0xDE, 0xB2, 0x32, 0x0F, 0xEA,
    0x45, 0xD4, 0x75, 0xA3, 0x3E, 0xC7, 0xB2, 0x64,
    0x9F, 0xC1, 0x90, 0x50, 0xD7, 0x6F, 0xDD, 0x3A,
    0xA6, 0xA3, 0x52, 0xED, 0xB6, 0xFC, 0x2B, 0x61,
    0xA5, 0xDB, 0xE4, 0xC7, 0xF8, 0x21, 0x30, 0xEF,
    0x7F, 0x21, 0x1E, 0x5B, 0x96, 0x45, 0x43, 0x4B,
    0x74, 0x98, 0xC6, 0x1B, 0xD9, 0x66, 0x0A, 0x64,
    0x41, 0x5F, 0xF6, 0xE0, 0x87, 0xD6, 0xFA, 0x00,
    0x4D, 0x76, 0x9B, 0x4E, 0x36, 0xC5, 0x8A, 0x10,
    0x8C, 0x61, 0xA1, 0x41, 0xB0, 0x8B, 0x25, 0xEC,
    0x19, 0x25, 0x89, 0x45, 0x8E, 0xFB, 0xCF, 0xB9,
};
#endif

static int mtb_find_addr(uint32_t *mtb_addr)
{
#define FLASH_ADDR      FLASH_BASE_ADDR
#define SECTOR_SIZE     FLASH_SECTOR_SIZE
#define SECTOR_COUNT    FLASH_SECTOR_COUNT
    uint32_t flash_addr = 0;
    uint32_t flash_end;

    flash_addr = FLASH_ADDR;
    flash_end = flash_addr + SECTOR_COUNT * SECTOR_SIZE;

    while(flash_addr < flash_end) {
        mtb_head_t *phead = (mtb_head_t *)flash_addr;
        if (phead->magic == BMTB_MAGIC) {
            TEE_LOGD("find bmtb %d\n", phead->version);
            // version 1
            if (phead->version == 0x1) {
                TEE_LOGD("size %d\n", phead->size);
                if (flash_addr + phead->size < flash_end) {
                    mtb_head_t *ihead = (mtb_head_t *)(flash_addr + phead->size);
                    if (ihead->magic == MTB_MAGIC) {
                        *mtb_addr = flash_addr + phead->size;
                        TEE_LOGI("got mtb1\n");
                        break;
                    }
                }
            }
        }
        if (phead->magic == MTB_MAGIC) {
            *mtb_addr = flash_addr;
            TEE_LOGI("got mtb%d\n", phead->version);
            break;
        }
        flash_addr += SECTOR_SIZE;
    }

    if (flash_addr == flash_end) {
        TEE_LOGE("mtb find magic e\n");
        goto fail;
    }
    TEE_LOGI("mtb find 0x%x\n", *mtb_addr);
    return 0;
fail:
    return -1;
}

int mtb_init()
{
    uint32_t mtb_addr = 0;

    if (mtb_find_addr(&mtb_addr))
        return MTB_ERR_INIT;

    mtb_head_t *mtb_head = (mtb_head_t *)mtb_addr;
    TEE_LOGD("mtb_addr %x, magic %x\n", mtb_addr, mtb_head->magic);

    if (mtb_head->magic == BMTB_MAGIC) {
        if (mtb_head->scn_count == 1) {
            mtb_head = (mtb_head_t *)(mtb_addr + mtb_head->size);

            if (mtb_head->magic != MTB_MAGIC) {
                mtb_head = (mtb_head_t *)mtb_addr;
            }
        }
    } else if (mtb_head->magic == MTB_MAGIC) {
        /*nothing to do*/
    } else {
        return MTB_ERR_MAGIC_NUMBER;
    }

    memset(&g_mtb, 0, sizeof(g_mtb));
    g_mtb.head = mtb_head;

    if (mtb_head->magic == MTB_MAGIC) {
        if (mtb_head->version == 0x01) {
            g_mtb.os_version = (mtb_os_version_t *)((uint8_t *)mtb_head + sizeof(mtb_head_t));
            g_mtb.scn = (scn_t *)((uint8_t *)g_mtb.os_version + sizeof(mtb_os_version_t));
        } else if (mtb_head->version == 0x02) {
            g_mtb.os_version2 = (mtb_os_version2_t *)((uint8_t *)mtb_head + sizeof(mtb_head_t));
            g_mtb.scn = (scn_t *)((uint8_t *)g_mtb.os_version2 + sizeof(mtb_os_version2_t));
        } else if (mtb_head->version == 0x04) {
            g_mtb.headv4 = (imtb_head_v4_t *)mtb_head;
            g_mtb.os_version4 = (mtb_os_version4_t *)((uint8_t *)mtb_head + sizeof(imtb_head_v4_t));
        } else {
            TEE_LOGE("mtb ver err, mtb_addr %x\n", mtb_addr);
            return MTB_ERR_VERSION;
        }
    } else if (mtb_head->magic == BMTB_MAGIC) {
        g_mtb.img = (image_info_t *)((uint8_t *)mtb_head + sizeof(mtb_head_t));
    }

    g_mtb_init = 1;
    return MTB_OK;
}

static scn_t *_get_section(search_mode_e mode, scn_head_type_t *type, const char *img_name)
{
    int i;

    if (!g_mtb_init) {
        TEE_LOGE("mtb is not init\n");
        return NULL;
    }

    if (g_mtb.head->magic != MTB_MAGIC)
    {
        TEE_LOGE("mtb magic err\n");
        return NULL;
    }

    scn_t *scn = g_mtb.scn;

    for (i = 0; i < g_mtb.head->scn_count; i++) {
        if ((mode == S_FARTHER_TYPE && type->father_type.first_type == scn->scn_head.type.father_type.first_type)
            || (mode == S_SON_TYPE && type->father_type.first_type == scn->scn_head.type.father_type.first_type
                && type->son_type == scn->scn_head.type.son_type)) {
            if (NULL != img_name) {
                if (!strncmp(img_name, (const char *)scn->img->image_name, MTB_IMAGE_NAME_SIZE)) {
                    TEE_LOGI("find scn %x\n", scn);
                    return scn;
                }
            } else {
                TEE_LOGI("find scn %x\n", scn);
                return scn;
            }
        }

        NEXT_SCN(scn);
    }

    return NULL;
}

static int _get_partition_info(char *name, imtb_user_partition_info_v4_t *part_info)
{
    int i;

    if (NULL == name || NULL == part_info) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    memset(part_info, 0, sizeof(imtb_user_partition_info_v4_t));

    if (g_mtb.head->magic == MTB_MAGIC && g_mtb.head->version == 4) {
        imtb_partition_info_v4_t *p = (imtb_partition_info_v4_t *)((uint8_t *)g_mtb.headv4 + sizeof(imtb_head_v4_t) + sizeof(mtb_os_version4_t));
        for (i = 0; i < g_mtb.headv4->partition_count; i++) {
            if (!strncmp((char *)p->name, (char *)name, MTB_IMAGE_NAME_SIZE)) {
                strncpy((char *)part_info->name, (char *)p->name, MTB_IMAGE_NAME_SIZE);
                strncpy((char *)part_info->pub_key_name, (char *)p->pub_key_name, PUBLIC_KEY_NAME_SIZE);
                part_info->partition_type = p->partition_type;
                part_info->part_start = p->block_offset * 512 + FLASH_BASE_ADDR;
                part_info->part_size = p->block_count * 512;
                part_info->load_address = p->load_address;
                part_info->img_size = p->img_size;
                TEE_LOGI("get part %s, addr 0x%x, size 0x%x, img_size:0x%x\n", name, part_info->part_start, part_info->part_size, part_info->img_size);
                return MTB_OK;
            }
            p ++;
        }
    }
    return MTB_ERR_NOT_FOUND;
}

int mtb_get_img_info(char *name, mtb_img_info_t *img_info)
{
    int i;

    if (NULL == name || NULL == img_info) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    memset(img_info, 0, sizeof(mtb_img_info_t));

    if (g_mtb.head->magic == BMTB_MAGIC) {
        image_info_t *img = g_mtb.img;

        for (i = 0; i < g_mtb.head->scn_count; i++) {
            if (!strncmp((char *)img->image_name, (char *)name, MTB_IMAGE_NAME_SIZE)) {
                strncpy((char *)img_info->img_name, (char *)img->image_name, MTB_IMAGE_NAME_SIZE);
                img_info->img_addr = img->static_addr;
                img_info->img_load_addr = img->loading_addr;
                img_info->img_size = img->image_size;
                TEE_LOGI("find img %s, addr %x, size %x\n", name, img_info->img_addr, img_info->img_size);
                return MTB_OK;
            }

            img++;
        }

        return MTB_ERR_NOT_FOUND;
    } else if (g_mtb.head->magic == MTB_MAGIC) {
        if (g_mtb.head->version < 4) {
            scn_t *scn = IMG_SCN(name);

            if (scn) {
                strncpy((char *)img_info->img_name, (char *)scn->img->image_name, MTB_IMAGE_NAME_SIZE);
                img_info->img_addr = scn->img->static_addr;
                img_info->img_load_addr = scn->img->loading_addr;
                img_info->img_size = scn->img->image_size;
                strncpy((char *)img_info->img_ver, (char *)scn->img->image_version, MTB_IMAGE_VERSION_SIZE);
                TEE_LOGI("find img %s, addr %x, size %x, version %s\n", \
                        name, img_info->img_addr, img_info->img_size, img_info->img_ver);
                return MTB_OK;
            }
        } else {
            imtb_user_partition_info_v4_t part_info;
            if (_get_partition_info(name, &part_info) == MTB_OK) {
                img_info->img_addr = part_info.part_start;
                img_info->img_load_addr = part_info.load_address;
                img_info->img_size = part_info.img_size;
                return MTB_OK;
            }
        }

        TEE_LOGI("not find img %s\n", name);
        return MTB_ERR_NOT_FOUND;
    }

    return MTB_ERR_GENERIC;
}


int mtb_get_os_version(uint8_t *buf, uint32_t *size)
{
    if (NULL == buf || NULL == size) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    if (g_mtb.head->magic != MTB_MAGIC) {
        return MTB_ERR_MAGIC_NUMBER;
    }

    *size = 0;

    if (g_mtb.head->version == 0x01) {
        memcpy(buf, g_mtb.os_version, sizeof(mtb_os_version_t));
        *size = sizeof(mtb_os_version_t);
    } else if (g_mtb.head->version == 0x01) {
        memcpy(buf, g_mtb.os_version2, sizeof(mtb_os_version2_t));
        *size = sizeof(mtb_os_version2_t);
    } else if (g_mtb.head->version == 0x04) {
        memcpy(buf, g_mtb.os_version4, sizeof(mtb_os_version4_t));
        *size = sizeof(mtb_os_version4_t);
    }

    return MTB_OK;
}

int mtb_get_img_buf(uint8_t *buf, uint32_t *size, char *name)
{
    if (NULL == buf || NULL == size || NULL == name) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    *size = 0;

    if (g_mtb.head->magic == BMTB_MAGIC) {
        return MTB_ERR_GENERIC;
    } else if (g_mtb.head->magic == MTB_MAGIC) {
        if (g_mtb.head->version < 4) {
            scn_t *scn = IMG_SCN(name);

            if (scn) {
                memcpy(buf, (uint8_t *)scn->img, SCN_CONTENT_SIZE(scn));
                *size = SCN_CONTENT_SIZE(scn);
                return MTB_OK;
            }
        } else {
            imtb_user_partition_info_v4_t part_info;
            if (_get_partition_info(name, &part_info) == MTB_OK) {
                memcpy(buf, (uint8_t *)part_info.part_start, part_info.img_size);
                *size = part_info.img_size;
                return MTB_OK;
            }
        }

        TEE_LOGI("not find img %s\n", name);
        return MTB_ERR_NOT_FOUND;
    }

    return MTB_ERR_MAGIC_NUMBER;
}

int mtb_get_partition_buf(uint8_t *buf, uint32_t *size)
{
    if (NULL == buf || NULL == size) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    *size = 0;

    if (g_mtb.head->magic == BMTB_MAGIC) {
        *size = 0;
        return MTB_ERR_GENERIC;
    } else if (g_mtb.head->magic == MTB_MAGIC) {
        if (g_mtb.head->version < 4) {
            scn_t *scn = PART_SCN;

            if (scn) {
                memcpy(buf, (uint8_t *)scn->partition, SCN_CONTENT_SIZE(scn));
                *size = SCN_CONTENT_SIZE(scn);
                return MTB_OK;
            }
        } else {
            imtb_partition_info_v4_t *p = (imtb_partition_info_v4_t *)((uint8_t *)g_mtb.headv4 + sizeof(imtb_head_v4_t) + sizeof(mtb_os_version4_t));
            scn_partion_info_t *scn_part_info = (scn_partion_info_t *)buf;
            for (int i = 0; i < g_mtb.headv4->partition_count; i++) {
                strncpy((char *)scn_part_info->image_name, p->name, MTB_IMAGE_NAME_SIZE);
                scn_part_info->part_addr = p->block_offset * 512 + FLASH_BASE_ADDR;
                scn_part_info->part_size = p->block_count * 512;
                p ++;
                scn_part_info ++;
            }
            *size = g_mtb.headv4->partition_count * sizeof(scn_partion_info_t);
            return MTB_OK;
        }

        TEE_LOGI("not find part\n");
        return MTB_ERR_NOT_FOUND;
    }

    return MTB_ERR_MAGIC_NUMBER;
}

int mtb_get_ntw_addr(uint32_t *addr, uint32_t *load_addr, uint32_t *size)
{
    int ret;
    mtb_img_info_t info;

    if (!(addr && load_addr && size)) {
        return MTB_ERR_NULL;
    }

#ifdef CONFIG_REE_VERIFY
    ret = mtb_verify_images();

    if (ret) {
        TEE_LOGE("%s verify fail, ret %d, reboot!!\n", NTW_IMG_NAME, ret);
        reboot();
    }

#endif

    ret = mtb_get_img_info(CONFIG_NTW_IMG_NAME, &info);

    if (ret) {
        *addr = 0;
        *load_addr = 0;
        *size = 0;
    } else {
        *addr = info.img_addr;
        *load_addr = info.img_load_addr;
        *size = info.img_size;
    }

    return ret;
}

int mtb_get_key_info(key_info_t *info)
{
    int ret;
    uint32_t key_addr;

    memset(info, 0, sizeof(key_info_t));

    ret = kp_get_key(KEY_CIDPRIVKEY, &key_addr);

    if (ret >= MTB_KEY_N_SIZE) {
        info->d = (uint8_t *)key_addr;
        kp_get_key(KEY_PUBKEY, &key_addr);
        info->n = (uint8_t *)key_addr;
        info->e = (uint8_t *)test_key_e;
        info->size = MTB_KEY_SIZE;
    } else if (ret >= MTB_ID2KEY_BYTE) {
        info->d = (uint8_t *)key_addr;
        info->size = ret;
    } else {
        scn_t *scn = KEY_SCN;

        if (scn) {
            info->d = NULL;
            info->n = scn->key->n;
            info->e = scn->key->e;
            info->size = SCN_CONTENT_SIZE(scn);
        } else {
            return MTB_ERR_NOT_FOUND;
        }
    }

    return MTB_OK;
}

int mtb_get_pubkey_info(key_info_t *info)
{
    int ret;
    uint32_t key_addr;

    memset(info, 0, sizeof(key_info_t));

    ret = kp_get_key(KEY_PUBKEY, &key_addr);

    if (ret >= MTB_KEY_N_SIZE) {
        info->n = (uint8_t *)key_addr;
        info->e = (uint8_t *)test_key_e;
        info->size = MTB_KEY_SIZE;
    } else {
        scn_t *scn = KEY_SCN;

        if (scn) {
            info->n = scn->key->n;
            info->e = scn->key->e;
            info->size = SCN_CONTENT_SIZE(scn);
        } else {
            return MTB_ERR_NOT_FOUND;
        }
    }

    return MTB_OK;

}

#ifdef CONFIG_REE_VERIFY

int mtb_verify_images()
{
    int ret;

    for (int i = 0; i < g_partitions.part_num; i++) {
        ret = image_verify(g_partitions.part_info[i].name);
        if (ret < 0) {

            return -1;
        }
    }
    return 0;
}

int mtb_verify_image(const char *name)
{
    if (NULL == name) {
        return MTB_ERR_NULL;
    }

    if (!g_mtb_init) {
        return MTB_ERR_INIT;
    }

    if (g_mtb.head->magic != MTB_MAGIC) {
        return MTB_ERR_MAGIC_NUMBER;
    }

    scn_t *scn = IMG_SCN(name);

    if (scn) {
        /* image signature = SIG( HASH (Section_info+ Img_info + HASH(img content) )) */

        uint8_t digest[sizeof(scn_head_t) + sizeof(scn_img_sig_info_t) + 20];
        memcpy(digest, (uint8_t *)scn, sizeof(scn_head_t) + sizeof(scn_img_sig_info_t));

        int ret;
        hash_type_t type;
        int digest_size = 0;

        if (g_mtb.head->digest_sch == DIGEST_HASH_SHA1) {
            digest_size = 20;
            type = SHA1;
        } else if (g_mtb.head->digest_sch == DIGEST_HASH_MD5) {
            digest_size = 16;
            type = MD5;
        } else {
            return MTB_ERR_GENERIC;
        }

        ret = tee_hash_digest(type, (uint8_t *)(uint32_t)(scn->img->static_addr),
                              (size_t)scn->img->image_size,
                              digest + sizeof(scn_head_t) + sizeof(scn_img_sig_info_t));

        if (ret) {
            TEE_LOGE("image %s hash digest fail, type %d, ret %x\n", name, type, ret);
            return MTB_ERR_GENERIC;
        }

        if (g_mtb.head->signature_sch == SIGNATURE_HASH_SHA1) {
            type = SHA1;
        } else {
            return MTB_ERR_GENERIC;
        }

        ret = tee_hash_digest(type, digest,
                              sizeof(scn_head_t) + sizeof(scn_img_sig_info_t) + digest_size,
                              digest);

        if (ret) {
            TEE_LOGE("image %s sign digest fail, type %d, ret %x\n", type, ret);
            return MTB_ERR_GENERIC;
        }

        key_info_t info;
        mtb_get_key_info(&info);

        if (info.size > 0) {

            rsa_pubkey_t pubkey;
            ret = tee_rsa_init_pubkey(MTB_KEY_BIT,
                                      info.n, MTB_KEY_N_SIZE,
                                      info.e, MTB_KEY_E_SIZE,
                                      &pubkey);

            if (ret) {
                TEE_LOGE("int pubkey fail, ret %x\n", ret);
                return MTB_ERR_GENERIC;
            }

            tee_rsa_padding_t padding = {RSASSA_PKCS1_V1_5, {{SHA1}}};

            bool result = 0;
            ret =  tee_rsa_verify(&pubkey, digest, 20,
                                  SCN_IMG_SIGN(scn), MTB_KEY_BYTE,
                                  padding, &result);

            if (ret) {
                TEE_LOGE("rsa verify fail, ret %x\n", ret);
                return MTB_ERR_VERIFY;
            }

            if (result) {
                return MTB_OK;
            }
        } else {
            TEE_LOGE("no key in mtb or kp\n");
            return MTB_ERR_GENERIC;
        }

        TEE_LOGE("rsa verify fail\n");
        return MTB_ERR_VERIFY;
    } else {
        return MTB_ERR_NULL;
    }
}
#endif
