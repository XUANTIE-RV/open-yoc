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
#include "mtb.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mtb_log.h"
#include "yoc/partition_device.h"
#include "yoc/partition.h"
#include "verify_wrapper.h"
#include "mtb_internal.h"
#include "misc.h"
#if (CONFIG_PARITION_NO_VERIFY == 0) && (defined(CONFIG_COMP_KEY_MGR) && CONFIG_COMP_KEY_MGR)
#include <key_mgr.h>
#endif
/****************************************************************************
* Definitions
****************************************************************************/
#define PUBLIC_KEY_VALUE_SIZE 128
#define PUBLIC_KEY_MODULE_SIZE 128
#define SCH_SIGNATURE_MAX_LEN 128

#define PADDING_SIZE(src_size,padd) (((src_size+padd-1)/padd)*padd)
#define DATA_TO_SIGN_SIZE           512

const scn_type_t g_scn_img_type[] = {
    {{SCN_TYPE_NORMAL, SCN_TYPE_IMG}, SCN_SUB_TYPE_IMG_NOR},
    {{SCN_TYPE_NORMAL, SCN_TYPE_IMG}, SCN_SUB_TYPE_IMG_SAFE}
};

const scn_type_t g_scn_key_type[] = {
    {{SCN_TYPE_NORMAL, SCN_TYPE_KEY}, SCN_SUB_TYPE_KEY_BOOT}
};

const scn_type_t g_scn_part_type[] = {
    {{SCN_TYPE_NORMAL, SCN_TYPE_PART}, SCN_SUB_TYPE_IMG_PART}
};

#define GET_SECTION_TYPE_ADDR(scn_addr) ((unsigned long)&(((scn_head_t *)scn_addr)->type))
#define GET_SECTION_TYPE_PTR(scn_addr)  ((scn_type_t *)(GET_SECTION_TYPE_ADDR(scn_addr)))
#define GET_SECTION_IMG_INFO_ADDR(scn_addr) ((unsigned long)((unsigned long)scn_addr+ sizeof(scn_head_t)))
#define GET_SECTION_IMG_INFO_PTR(scn_addr)  ((scn_img_sig_info_t *)(GET_SECTION_IMG_INFO_ADDR(scn_addr)))
/****************************************************************************
 * Functions
 ****************************************************************************/
static inline int mtb_version(void)
{
    return mtb_get()->version;
}

/*
    get the first section offset
 * @param[in]    im_addr         bootrom manfest address
 * @param[out]     offset      offset
 * @return      0 on success; -1 on failure
 */
static int get_scn_offset(uint32_t im_addr, unsigned long *offset)
{
    unsigned long m_addr = mtb_get()->using_addr;
    uint32_t size_ver = 0;

    if (mtb_version() == 1) {
        size_ver = MTB_OS_VERSION_LEN;
    } else if (mtb_version() == 2) {
        size_ver = MTB_OS_VERSION_LEN_V2;
    } else if (mtb_version() == 4) {
        // there is no section in MTB V4
        MTB_LOGE("there is no section in MTB V4");
        return -1;
    } else {
        MTB_LOGE("ver e");
        return -1;
    }

    *offset = (unsigned long)(m_addr + sizeof(mhead_tb) + size_ver);
    return 0;
}

/*
    get the  section address by type or name
 * @param[in]     seach_type      seach_type
 * @param[in]     seach_name      seach_name
 * @param[out]     scn_addr      scn_addr
 * @param[in]     mode          SEACH_MODE_FIRST_TYPE,SEACH_MODE_SON_TYPE or SEACH_MODE_EXTEND_TYPE
 * @return      0 on success; -1 on failure
 */
int get_section_addr(unsigned long im_addr, const scn_type_t *seach_type, uint8_t *seach_name, unsigned long *scn_addr, uint32_t mode)
{
    unsigned long m_addr = im_addr;
    mhead_tb *mtb = (mhead_tb *)m_addr;
    scn_head_t *scn_head = NULL;
    scn_img_sig_info_t *img_info_t;
    uint32_t i = 0;
    uint32_t sum_scn;
    uint8_t s_name[MTB_IMAGE_NAME_SIZE + 1];

    if (mtb == NULL) {
        return -EINVAL;
    }
    sum_scn = mtb->scn_count;
    memset(s_name, 0, sizeof(s_name));

    if (seach_name != NULL) {
        memcpy(s_name, seach_name, MTB_IMAGE_NAME_SIZE);
    }

    if (get_scn_offset(m_addr, (unsigned long *)(&scn_head))) {
        return -1;
    }

    if (NULL != seach_name) {
        MTB_LOGD("seach_name:%.*s,seach_type:%d,mode:%d", MTB_IMAGE_NAME_SIZE, seach_name, seach_type->father_type.first_type , mode);
    }

    for (; i < sum_scn ; i++) {
        //MTB_LOGD("get_section_addr:%x",(uint32_t)scn_t);
        if (scn_head->type.father_type.first_type == seach_type->father_type.first_type) {
            if (mode == SEACH_MODE_FIRST_TYPE || ((mode == SEACH_MODE_SON_TYPE) && scn_head->type.son_type == seach_type->son_type) ||
                (mode == SEACH_MODE_EXTEND_TYPE && scn_head->type.son_type == seach_type->son_type && scn_head->type.father_type.extend_type == seach_type->father_type.extend_type)) {
                if (seach_name == NULL) {
                    goto succ;
                } else {
                    img_info_t = GET_SECTION_IMG_INFO_PTR(scn_head);

                    if (!strncmp((char *)(img_info_t->image_name), (char *)s_name, MTB_IMAGE_NAME_SIZE)) {
                        goto succ;
                    }

                    MTB_LOGD("seach_type:%d,name:%.*s,s_name:%s", seach_type->father_type.first_type, MTB_IMAGE_NAME_SIZE, (char *)img_info_t->image_name, s_name);
                }
            }
        }

        scn_head = (scn_head_t *)(NEXT_SECTION(scn_head));
    }

    MTB_LOGD("not seach_type:%d", seach_type->father_type.first_type);
    return -1;
succ:
    MTB_LOGD("s scn_t:0x%lx", (unsigned long)scn_head);
    *scn_addr = (unsigned long)scn_head;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
typedef enum {
    S_FARTHER_TYPE = 0,
    S_SON_TYPE,
} search_mode_e;

#define IMG_SCN_HEAD_TYPE       (&(scn_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_IMG }, SCN_SUB_TYPE_IMG_NOR })
#define KEY_SCN_HEAD_TYPE       (&(scn_type_t){{SCN_TYPE_NORMAL, SCN_TYPE_KEY }, SCN_SUB_TYPE_KEY_BOOT})
#define SCN_CONTENT_SIZE(scn) (scn->scn_head.size - sizeof(scn_head_t))
#define NEXT_SCN(scn)   (scn = (scn_t *)((uint8_t *)scn + scn->scn_head.size))
#define IMG_SCN(name)   _get_section(S_SON_TYPE, IMG_SCN_HEAD_TYPE, name)
#define KEY_SCN         _get_section(S_SON_TYPE, KEY_SCN_HEAD_TYPE, NULL)
#define PART_SCN        _get_section(S_SON_TYPE, PARTITION_SCN_HEAD_TYPE, NULL)

static scn_t *_get_section(search_mode_e mode, scn_type_t *type, const char *img_name)
{
    int i;
    scn_t *scn;
    mtb_head_t *head;
    unsigned long mtb_head = mtb_get()->using_addr;

    if (mtb_version() == 0x01) {
        scn = (scn_t *)(mtb_head + sizeof(mtb_head_t) + sizeof(mtb_os_version_t));
    } else if (mtb_version() == 0x02) {
        scn = (scn_t *)(mtb_head + sizeof(mtb_head_t) + sizeof(mtb_os_version2_t));
    } else if (mtb_version() == 0x04) {
        return NULL;
    } else {
        MTB_LOGE("mtb ver err, mtb_addr %lx\n", mtb_head);
        return NULL;
    }
    head = (mtb_head_t*)mtb_head;
    for (i = 0; i < head->head.scn_count; i++) {
        if ((mode == S_FARTHER_TYPE && type->father_type.first_type == scn->scn_head.type.father_type.first_type)
            || (mode == S_SON_TYPE && type->father_type.first_type == scn->scn_head.type.father_type.first_type
                && type->son_type == scn->scn_head.type.son_type)) {
            if (NULL != img_name) {
                if (!strncmp(img_name, (const char *)scn->img->image_name, MTB_IMAGE_NAME_SIZE)) {
                    MTB_LOGI("find scn 0x%lx\n", (unsigned long)scn);
                    return scn;
                }
            } else {
                MTB_LOGI("find scn 0x%lx\n", (unsigned long)scn);
                return scn;
            }
        }

        NEXT_SCN(scn);
    }

    return NULL;
}

#if (CONFIG_PARITION_NO_VERIFY == 0) && (defined(CONFIG_COMP_KEY_MGR) && CONFIG_COMP_KEY_MGR)
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

int mtb_get_key_info(key_info_t *info)
{
    int ret;
    key_handle key_addr;
    uint32_t size;

    if (info == NULL) {
        return -EINVAL;
    }

    memset(info, 0, sizeof(key_info_t));
    ret = km_get_key(KEY_ID_PUBK_TB, &key_addr, &size);
    if (ret != 0) {
        return -1;
    }

    if (size >= MTB_KEY_N_SIZE) {
        info->d = (uint8_t *)key_addr;
        km_get_key(KEY_ID_PUBK_TB, &key_addr, &size);
        info->n = (uint8_t *)key_addr;
        info->e = (uint8_t *)test_key_e;
        info->size = MTB_KEY_SIZE;
    } else if (size >= MTB_ID2KEY_BYTE) {
        info->d = (uint8_t *)key_addr;
        info->size = size;
    } else {
        scn_t *scn = KEY_SCN;

        if (scn) {
            info->d = NULL;
            info->n = scn->key->n;
            info->e = scn->key->e;
            info->size = SCN_CONTENT_SIZE(scn);
        } else {
            return -1;
        }
    }

    return 0;
}

int mtb_get_pubkey_info(key_info_t *info)
{
    int ret;
    key_handle key_addr;
    uint32_t size;

    if (info == NULL) {
        return -EINVAL;
    }

    memset(info, 0, sizeof(key_info_t));
    ret = km_get_key(KEY_ID_PUBK_TB, &key_addr, &size);
    if (ret != KM_OK) {
        return -1;
    }

    if (size >= MTB_KEY_N_SIZE) {
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
            return -1;
        }
    }

    return 0;
}
#endif /* CONFIG_PARITION_NO_VERIFY */

int mtb_get_img_info_buf(uint8_t *buf, uint32_t *size, char *name)
{
    if (!(buf && size && name)) {
        return -EINVAL;
    }

    *size = 0;

    if (mtb_version() < 4) {
        scn_t *scn = IMG_SCN(name);
        if (scn) {
            memcpy(buf, (uint8_t *)scn->img, SCN_CONTENT_SIZE(scn));
            *size = SCN_CONTENT_SIZE(scn);
            return 0;
        }
    } else {
        mtb_partition_info_t info;
        scn_img_sig_info_t out_info;

        memset(&out_info, 0, sizeof(scn_img_sig_info_t));
        if (mtb_get_partition_info(name, &info) == 0) {
            memcpy(out_info.image_name, info.name, MTB_IMAGE_NAME_SIZE);
            out_info.static_addr = info.start_addr;
            out_info.loading_addr = info.load_addr;
            out_info.image_size = info.img_size;
            memcpy(buf, &out_info, sizeof(scn_img_sig_info_t));
            *size = sizeof(scn_img_sig_info_t);
            return 0;
        }
    }
    return -1;
}
