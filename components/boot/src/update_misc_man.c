/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if (CONFIG_NO_OTA_UPGRADE == 0)
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "update_diff.h"
#include "update_mtb.h"
#include "update_misc_man.h"
#include "update.h"
#include "update_log.h"
#include "update_defs.h"
#include "boot_flash_porting.h"
#include "verify_wrapper.h"
#include "yoc/partition.h"
#if (CONFIG_PARITION_NO_VERIFY == 0)
#include <key_mgr.h>
#endif
/****************************************************************************
* Definitions
****************************************************************************/

typedef struct {
    uint8_t reserve: 2;
    uint8_t flash_padding: 1;
    uint8_t update_type: 1; // update type
    uint8_t img_type: 4; // img type
} sub_misc_img_type_t;

typedef union {
    sub_misc_img_type_t type_t;
    uint8_t type;
} misc_img_type_t;

typedef struct {
    uint8_t img_name[8];
    uint32_t img_off;
    uint8_t img_type;
    uint8_t rsv_d[3];
} misc_img_info_t;
typedef  uint32_t  MISC_FD_T;

#define MISC_IMG_TYPE_SIZE 1
#define MISC_IMG_NAME_SIZE 8
#define MISC_IMG_ADDR_SIZE 4

#define MISC_IMG_TYPE_OFFSET 0
#define MISC_IMG_NAME_OFFSET (MISC_IMG_TYPE_OFFSET+MISC_IMG_TYPE_SIZE)
#define MISC_IMG_ADDR_OFFSET (MISC_IMG_NAME_OFFSET+MISC_IMG_NAME_SIZE)

#define SIZE_OF_IMAGE_NAME MISC_IMG_NAME_SIZE
#define GET_INT_VALUE(a)                   c2int32(a)

#define MISC_FD_HEAD_LEN                  (3)                         //sizeof(misc_img_info_t)/4
#define AUTOGRAPH_NULL                    (0)
#define AUTOGRAPH_SHA1                    (1)

#define __FMIT(misc)                      ((misc_head_info_t *)misc) //FLASH MISC INFO T == FMIT
#define __FMIT_IMG_START(misc)            ((uint32_t)misc+MISC_FLASH_HEADLEN(misc))
#define __FMIT_IMG(misc,num)              ((__FMIT_FD(__FMIT_IMG_START(misc)))[num])
#define __FMIT_FD(fd)                     ((misc_img_info_t *)fd)
#define MISC_FILE_HEADER_STRING           0x45474d49   //"IMGE" = 0x45474d49
#define MISC_FILE_HEADER_INT              ((uint32_t)MISC_FILE_HEADER_STRING)
#define MISC_FLASH_VALID(misc)            ((__FMIT(misc)->magic) == MISC_FILE_HEADER_INT)
#define MISC_FLASH_FOTA_VERSION(misc)     (__FMIT(misc)->ver)
#define MISC_FLASH_HEADLEN(misc)          (__FMIT(misc)->head_len)
#define MISC_FLASH_SUM_FD(misc)           ((MISC_FD_T)&(__FMIT(misc)->image_sum))
#define MISC_FLASH_FD_SUM(misc)           (__FMIT(misc)->image_sum)
#define MISC_FLASH_PADTYPE(misc)          (__FMIT(misc)->pad_type)
#define MISC_FLASH_MNFT_OFFSET_FD(misc)   ((MISC_FD_T)&(__FMIT(misc)->mnft_off))
#define MISC_FLASH_MNFT_OFFSET(misc)      (__FMIT(misc)->mnft_off)
#define MISC_FLASH_FIRST_FD(misc)         ((MISC_FD_T)(&(__FMIT_IMG(misc,0)))) //
#define MISC_NEXT_FD(fd)                  ((MISC_FD_T)&(__FMIT_FD(fd)[1]))
#define MISC_NEXT_FD_PRE(fd)              ((MISC_FD_T)((uint32_t)fd-sizeof(misc_img_info_t)))
#define MISC_FLASH_FD_LAST(misc)          ((MISC_FD_T)&(__FMIT_IMG(misc,(MISC_FLASH_FD_SUM(misc)-1))))

#define MISC_FD_TYPE(fd)                    (__FMIT_FD(fd)->img_type)
#define MISC_FD_NAME_FDS(fd)                (__FMIT_FD(fd)->img_name)//((__FMIT_FD(fd)->img_info)+MISC_IMG_NAME_OFFSET)
#define MISC_FD_OFFSET(fd)                  (__FMIT_FD(fd)->img_off) //GET_INT_VALUE((__FMIT_FD(fd)->img_info)+MISC_IMG_ADDR_OFFSET)
#define MISC_FD_NEXT_OFFSET(fd)             MISC_FD_OFFSET(MISC_NEXT_FD(fd))
#define MISC_FD_PRE_OFFSET(fd)             MISC_FD_OFFSET(MISC_NEXT_FD_PRE(fd))
#define MISC_FLASH_NUM_FD(misc,num)          ((MISC_FD_T)(&(__FMIT_IMG(misc,num)))) //
#define MISC_FLASH_FD_NUM(misc,fd)           ((((uint32_t)(fd)) - __FMIT_IMG_START(misc)) / sizeof(misc_img_info_t))


#define MISC_OS_VERSION_ADDR(misc)           ((uint32_t)misc+sizeof(misc_head_info_t))

// update type {DIFF, FULL}
#define UPDATE_TYPE_DIFF   1
#define UPDATE_TYPE_NORMAL 0

mtb_t *g_upd_mtb;

static uint32_t g_flash_misc_section_size;
static uint32_t g_flash_misc_addr;
static uint32_t g_flash_misc_fota_data_addr;
static uint32_t g_flash_misc_size;
#ifdef CONFIG_NON_ADDRESS_FLASH
static uint8_t g_fota_tb_ram[MAX_FOTA_TB_SIZE] = {0};
#endif

typedef struct {
    uint32_t magic;
    uint32_t ver;
    uint32_t flags;
    uint32_t image_sum;
    uint32_t mnft_off;
    misc_img_info_t img_inf[FLASH_MISC_IMAGER_MAX];
    uint8_t public_key[MISC_FILE_KEY_LEN];
    uint8_t signature[MISC_FILE_SIG_LEN];
} flash_misc_info_t;

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
} misc_head_info_t;

typedef struct {
    uint8_t value[32];
} misc_os_ver1_t;

typedef struct {
    uint32_t type[4];
} misc_control_status_s;
typedef struct {
    uint32_t status;
    misc_control_status_s fd_info[0];
} misc_control_s;

static int misc_get_update_status(bool *is_continue);
static uint32_t misc_first_fd(void);


/****************************************************************************
 * Functions
 ****************************************************************************/
#if 0
static int endian_check(void)
{
    short a = 0x1234;
    char *c = (char *)&a;

    if (c[0] == 0x12) {
        return 0;    // big endian
    } else {
        return 1;
    }
}

static uint32_t c2int32(uint8_t *buf)
{
    union {
        char c[4];
        uint32_t num;
    } util;

    if (endian_check()) {
        memcpy(util.c, buf, 4);
    } else {
        int i = 0;

        for (; i < 4; ++i) {
            util.c[3 - i] = buf[i];
        }
    }

    return util.num;
}
#endif

/**
 * update image
 * @param[in]    part_addr        update partition base_addr
 * @param[in]    part_len         update partition len
 * @param[in]    img_addr         update img base_addr
 * @param[in]    img_size          update img len
 * @return      0 on success; -1 on failure
 */
static int update_image(uint32_t part_addr , uint32_t part_len, uint32_t img_addr, uint32_t img_size , uint32_t update_type , uint32_t max_part_size)
{
    UPD_LOGD("update image update_type:%d", update_type);
    if (update_type == UPDATE_TYPE_DIFF) {
        UPD_LOGI("start DIFF update");
#if (CONFIG_OTA_NO_DIFF == 0)
        return diff_updata_img(part_addr, part_len, max_part_size, img_addr, img_size);
#else
        UPD_LOGI("DIFF not support.");
        return UPDATE_NOT_SUPPORT;
#endif
    }
    UPD_LOGI("start FULL update");

    uint8_t *buf = NULL;
    if (img_size > max_part_size) {
        UPD_LOGE("too imglen");
        goto fail;
    }

    /*modif it with your update mode*/
    if (boot_flash_erase(part_addr, max_part_size)) {
        UPD_LOGE("e e flash");
        goto fail;
    }

#ifdef CONFIG_NON_ADDRESS_FLASH
    boot_flash_info_t flash_info;
    uint32_t tmp_size;
    uint32_t offset = 0;

    boot_flash_info_get(&flash_info);
    tmp_size = flash_info.sector_size;
    buf = (uint8_t *)malloc(tmp_size);
    if (!buf) {
        UPD_LOGE("mem out");
        return -1;
    }
    while(img_size > tmp_size) {
        if (boot_flash_read(img_addr + offset, buf, tmp_size)) {
            goto fail;
        }
        if (boot_flash_write(part_addr + offset, buf, tmp_size)) {
            goto fail;
        }
        offset += tmp_size;
        img_size -= tmp_size;
    }
    if (boot_flash_read(img_addr + offset, buf, img_size)) {
        goto fail;
    }
    if (boot_flash_write(part_addr + offset, buf, img_size)) {
        goto fail;
    }
#else
    if (boot_flash_write(part_addr, (uint8_t *)img_addr, img_size)) {
        UPD_LOGE("e w flash");
        return -1;
    }
#endif
    if (buf) {
        free(buf);
    }
    return 0;

fail:
    if (buf) {
        free(buf);
    }
    return -1;
}

/**
 * reset misc
 *
 * @return      0 on success; -1 on failure
 */
int misc_reset(void)
{
    uint32_t misc_addr = g_flash_misc_addr;
    UPD_LOGD("misc_addr:0x%x, size:%d", misc_addr, g_flash_misc_size);
    return boot_flash_erase(misc_addr, g_flash_misc_size);
}

/**
 * init misc
 *
 * @param[in]    misc_addr         misc_base
 * @return      0 on success; -1 on failure
 */
int misc_init(uint32_t misc_addr , uint32_t flash_size , uint32_t flash_section)
{
    g_flash_misc_addr = misc_addr;
    g_flash_misc_fota_data_addr = misc_addr + (flash_section << 1);
    g_flash_misc_section_size = flash_section;
    g_flash_misc_size = flash_size;
#ifdef CONFIG_NON_ADDRESS_FLASH
    if (boot_flash_read(g_flash_misc_fota_data_addr, g_fota_tb_ram, MAX_FOTA_TB_SIZE)) {
        return -1;
    }
#endif
    g_upd_mtb = mtb_get();
    UPD_LOGD("g_flash_misc_addr:0x%x", g_flash_misc_addr);
    UPD_LOGD("g_flash_misc_fota_data_addr:0x%x", g_flash_misc_fota_data_addr);
    UPD_LOGD("g_flash_misc_section_size:0x%x", g_flash_misc_section_size);
    UPD_LOGD("g_flash_misc_size:0x%x", g_flash_misc_size);
    return 0;
}

inline static uint32_t get_fota_tb_address()
{
    uint32_t addr;
#ifdef CONFIG_NON_ADDRESS_FLASH
    addr = (uint32_t)g_fota_tb_ram;
#else
    addr = g_flash_misc_fota_data_addr;
#endif
    return addr;
}

/**
 * check misc imager  from misc
 *
 * @return      PATH_BOOT_PRIM on success prim path;
 * @return      PATH_BOOT_RCVY on success rcvy path;
 * @return      -1 on failure
 */
int misc_file_check()
{
    bool is_continue;
    uint32_t misc_addr;

    misc_addr = get_fota_tb_address();
    UPD_LOGD("misc_addr=0x%08x", misc_addr);
    if (!MISC_FLASH_VALID(misc_addr) || MISC_FLASH_FD_SUM(misc_addr) > FLASH_MISC_IMAGER_MAX) {
        UPD_LOGW("misc fota data e");
        return -1;
    }

    if (misc_get_update_status(&is_continue)) {
        return -1;
    }
    UPD_LOGD("is_continue:%d", is_continue);

    if (!is_continue) {
        uint8_t  signature[256];
        uint8_t  hash[128];
        uint8_t  calc_hash[128];
        uint32_t hash_len;
        uint32_t olen;
        uint32_t fota_data_addr;
        uint32_t fota_data_size;
        uint32_t signature_addr, signature_len;
        // signature_sch_e sig_type;
        digest_sch_e digest_type;
        uint32_t hash_addr = INVALID_ADDR;

        // sig_type    = __FMIT(misc_addr)->signature_type;
        digest_type = __FMIT(misc_addr)->digest_type;

        fota_data_addr = g_flash_misc_fota_data_addr;

    #if (CONFIG_MANTB_VERSION == 1) || (CONFIG_MANTB_VERSION == 4)
        if ((MISC_FLASH_FOTA_VERSION(misc_addr)) == 0) {
            signature_len  = MISC_FILE_SIG_LEN;
            fota_data_size = MISC_FLASH_MNFT_OFFSET(misc_addr);
            signature_addr = MISC_FLASH_MNFT_OFFSET(misc_addr)+fota_data_addr;
            hash_addr      = signature_addr + 256;
        } else if ((MISC_FLASH_FOTA_VERSION(misc_addr)) == 1) {
    #elif (CONFIG_MANTB_VERSION == 2)
        if ((MISC_FLASH_FOTA_VERSION(misc_addr)) == 1) {
    #endif
            signature_len  = MISC_FILE_SIG_LEN;
            fota_data_size = MISC_FLASH_MNFT_OFFSET(misc_addr);
            signature_addr = MISC_FLASH_MNFT_OFFSET(misc_addr)+fota_data_addr;
        } else {
            UPD_LOGE("e ver:%d", MISC_FLASH_FOTA_VERSION(misc_addr));
            return -1;
        }
        hash_len = get_length_with_digest_type(digest_type);

        UPD_LOGD("signature_addr:0x%x, hash_addr:0x%x, hash_len:%d", signature_addr, hash_addr, hash_len);
        UPD_LOGD("fota_data_addr:0x%x, fota_data_size:%d", fota_data_addr, fota_data_size);
#ifdef CONFIG_NON_ADDRESS_FLASH
        if (boot_flash_read(signature_addr, signature, signature_len)) {
            return -1;
        }
        if (boot_flash_read(hash_addr, hash, hash_len)) {
            return -1;
        }
#else
        memcpy(signature, (uint8_t *)signature_addr, signature_len);
        memcpy(hash, (uint8_t *)hash_addr, hash_len);
#endif
        hash_calc_start(digest_type, (uint8_t *)fota_data_addr, fota_data_size, calc_hash, &olen, 0);
#if (CONFIG_MANTB_VERSION == 4)
        if (memcmp(hash, calc_hash, hash_len)) {
            UPD_LOGE("fota data hash v e");
            return -1;
        }
#endif
        UPD_LOGI("fota data hash verify ok");
#if (CONFIG_PARITION_NO_VERIFY == 0)
        key_handle key_addr;
        uint32_t key_len;
        signature_sch_e sig_type;

        sig_type = __FMIT(misc_addr)->signature_type;
        if (km_get_pub_key_by_name(DEFAULT_PUBLIC_KEY_NAME_IN_OTP, &key_addr, &key_len) != KM_OK) {
            UPD_LOGE("e got key.");
            return -1;
        }
        if (signature_verify_start(digest_type, sig_type, (uint8_t *)key_addr, key_len, hash, hash_len, signature, signature_len)) {
            UPD_LOGE("fota data verify e");
            return -1;
        }
        UPD_LOGI("fota data verify ok");
#endif /* CONFIG_PARITION_NO_VERIFY */     
    }

    UPD_LOGD("pmtb");
    return PATH_BOOT_PRIM;
}

/**
 * get next imager fd  path from misc
 *
 * @param[in]    fd         misc fd
 * @return      imager fd
 */
uint32_t misc_next_imager(uint32_t fd)
{
    uint32_t misc_addr = get_fota_tb_address();
    UPD_LOGD("fd in:%x", fd);

    if ((MISC_FLASH_FD_NUM(misc_addr, fd)) == 0) {
        UPD_LOGD("fd no");
        return 0;
    }

    UPD_LOGD("fd out:%x", MISC_NEXT_FD_PRE(fd));
    return MISC_NEXT_FD_PRE(fd);
}

/**
 * get first imager fd  path from misc
 *
 * @return      imager first fd
 */
static uint32_t misc_first_fd()
{
    uint32_t misc_addr = get_fota_tb_address();
    UPD_LOGD("sum:%d,imgstart:0x%x,first_fd:0x%x", MISC_FLASH_FD_SUM(misc_addr), __FMIT_IMG_START(misc_addr), MISC_FLASH_FIRST_FD(misc_addr));
    return MISC_FLASH_FD_LAST(misc_addr);
}

/**
 * get first imager fd  path from misc
 *
 * @return      imager first fd
 */
static int32_t misc_fd_check(uint32_t fd)
{
    uint32_t misc_addr = get_fota_tb_address();
    uint32_t fd_sum = MISC_FLASH_FD_SUM(misc_addr);
    uint32_t fd_num = MISC_FLASH_FD_NUM(misc_addr, fd);

    if (fd_num < fd_sum) {
        return 0;
    }

    return -1;
}


static uint8_t misc_get_padtype()
{
    uint32_t misc_addr = get_fota_tb_address();
    UPD_LOGD("PAD type:%d", MISC_FLASH_PADTYPE(misc_addr));
    return MISC_FLASH_PADTYPE(misc_addr);
}

/**
 * get imager info  path from misc
 *
 * @param[in]    fd         misc fd
 * @param[out]   img_f      misc info
 * @return      0 on success; -1 on failure
 */
int misc_get_imager_info(uint32_t fd, img_info_t *img_f)
{
    uint32_t misc_addr = get_fota_tb_address();
    uint8_t  *img_name = MISC_FD_NAME_FDS(fd);
    UPD_LOGD("got fd %x", fd);

    strncpy((char *)(img_f->img_name), (char *)img_name, SIZE_OF_IMAGE_NAME);
    img_f->img_addr = MISC_FD_OFFSET(fd) + g_flash_misc_fota_data_addr;
    img_f->img_type = MISC_FD_TYPE(fd);

    if (((MISC_FLASH_FD_NUM(misc_addr, fd)) + 1) == MISC_FLASH_FD_SUM(misc_addr)) {
        img_f->img_size = MISC_FLASH_MNFT_OFFSET(misc_addr) - MISC_FD_OFFSET(fd);
    } else {
        img_f->img_size = MISC_FD_NEXT_OFFSET(fd) - MISC_FD_OFFSET(fd);
    }

    if ((img_f->img_size) & 0x80000000) {
        return -1;
    }

    UPD_LOGD("got img_addr:0x%x,img_type:%x,img_size:%d,", img_f->img_addr, img_f->img_type, img_f->img_size);
    return 0;
}

int misc_get_scn_img_info(uint32_t img_addr, scn_img_t *scn_img)
{
    boot_flash_read(img_addr, (uint8_t *)scn_img, sizeof(scn_img_t));
    return 0;
}

#define __PADDING_LEN(A,B) (((A+B-1)/B)*B)

/**
 * update osversion from misc
 *
 * @param[in]   bm_addr      bm_addr
 * @return      0 on success; -1 on failure
 */
int misc_update_os_version(uint32_t bm_addr)
{
#if (CONFIG_MANTB_VERSION < 4)
    uint32_t misc_addr = g_flash_misc_fota_data_addr;
    uint32_t os_ver_addr = MISC_OS_VERSION_ADDR(misc_addr);
    uint32_t os_ver_len = 0;

#if (CONFIG_MANTB_VERSION == 1)
    if (MISC_FLASH_FOTA_VERSION(misc_addr) == 0) {
        os_ver_len = MTB_OS_VERSION_LEN;
    } else if (MISC_FLASH_FOTA_VERSION(misc_addr) == 1) {
#elif (CONFIG_MANTB_VERSION == 2)
    if (MISC_FLASH_FOTA_VERSION(misc_addr) == 1)
#endif /* CONFIG_MANTB_VERSION */
        os_ver_len = MTB_OS_VERSION_LEN_V2;
    }

    UPD_LOGD("new os_vers:%.*s", MTB_IMAGE_VERSION_SIZE, (char *)os_ver_addr);

    if (update_imginfo(bm_addr, UPDATE_TYPE_OS_VERSIN, NULL, (uint8_t *)os_ver_addr, os_ver_len)) {
        UPD_LOGE("e update os_ver:%d", os_ver_len);
        return -1;
    }

    if (update_img_mtb(1)) {
        return -1;
    }
#else
    UPD_LOGD("mtb update app version...");
    if (mtb_update_other(g_upd_mtb, 1)) {
        UPD_LOGE("mtb update other e");
        return -1;
    }
    if (mtb_update_valid(g_upd_mtb)) {
        UPD_LOGE("mtb update valid e");
        return -1;
    }
#endif /*CONFIG_MANTB_VERSION < 4*/
    return 0;
}

/**
 * update path from misc
 *
 * @param[in]   img_f      misc info
 * @param[in]   path       which path
 * @return      0 on success; -1 on failure
 */
int misc_update_path(uint32_t bm_addr, img_info_t *img_update, uint32_t type, uint32_t fd)
{
    int ret;
    img_info_t img_old_v;
    img_info_t *img_old = &img_old_v;

    misc_img_type_t update_type;
    update_type.type = img_update->img_type;
    uint32_t update_img_size = 0;
    uint32_t update_img_addr = 0;
    scn_type_t tmp_scn_type;
    uint32_t tmp_size , tmp_addr;
    uint8_t pad_type;
    uint32_t type_flag = type;
    int update_flag = 0;
    int flash_sector;
    boot_flash_info_t flash_info;

    boot_flash_info_get(&flash_info);
    flash_sector = flash_info.sector_size;

    UPD_LOGD("update_type.type:0x%x,update_type.type_t.img_type:%d,update_type.type_t.update_type:%d,reserve:%d", update_type.type, update_type.type_t.img_type, update_type.type_t.update_type, update_type.type_t.reserve);

    pad_type = misc_get_padtype();

    if (update_type.type_t.img_type == 0) {
        UPD_LOGD("img_type 0");
        update_img_addr = img_update->img_addr;
        update_img_size = img_update->img_size;

        if (mtb_get_img_info((const char *)img_update->img_name, img_old)) {
            UPD_LOGE("e got img info%.*s", MTB_IMAGE_NAME_SIZE, img_update->img_name);
            return -1;
        }

        UPD_LOGD("update_img_addr:0x%x, update_img_size:%d", update_img_addr, update_img_size);

		// TODO: update_img_addr, update_img_size ?? need resize??
        ret = update_image(img_old->img_addr, img_old->img_size, update_img_addr, update_img_size, update_type.type_t.update_type, img_old->img_part_size);
        if (ret < 0) {
            UPD_LOGE("e up nimg:%.*s", MTB_IMAGE_NAME_SIZE, img_update->img_name);
            return ret;
        }
    } else if (update_type.type_t.img_type == 1) {
        UPD_LOGD("img_type 1");
        if (type_flag == MISC_STATUS_IMG_SET) {
            if (misc_set_update_fd(MISC_UPDATA_CONTROL_SET, fd, type_flag)) {
                return -1;
            }

            if (get_section_buf(img_update->img_addr, img_update->img_size, &tmp_scn_type, &tmp_size)) {
                UPD_LOGE("e got mate info%.*s scntype:%d", MTB_IMAGE_NAME_SIZE, img_update->img_name, (*(uint32_t *)&tmp_scn_type));
                return -1;
            }

            /* need sector padding */
            if (0 == pad_type) {
                tmp_addr = img_update->img_addr - g_flash_misc_fota_data_addr + tmp_size;
				// FIXME: (g_flash_misc_section_size > FLASH_IMG_SETCOR ? g_flash_misc_section_size : FLASH_IMG_SETCOR)???
                update_img_addr = g_flash_misc_fota_data_addr + __PADDING_LEN(tmp_addr , (g_flash_misc_section_size > flash_sector ? g_flash_misc_section_size : flash_sector));
                update_img_size = img_update->img_size - (update_img_addr - img_update->img_addr);
                UPD_LOGD("tmp_addr:0x%x,update_img_addr:0x%x,update_img_size:%d,tmp_size:%d", tmp_addr, update_img_addr, update_img_size, tmp_size);
            } else {
                tmp_size = __PADDING_LEN(tmp_size , 4);
                update_img_addr = img_update->img_addr + tmp_size;
                update_img_size = img_update->img_size - tmp_size;
            }

            if (mtb_get_img_info((const char *)img_update->img_name, img_old)) {
                UPD_LOGE("e got img info: %s", img_update->img_name);
                return -1;
            }

            UPD_LOGD("update img part_addr:0x%x-img_name:%s-img_addr:%x-img_size:%d-max_size:%d-update_type:%d\n", img_old->img_addr, img_update->img_name, update_img_addr, update_img_size, img_old->img_part_size, update_type.type_t.update_type);
            ret = update_image(img_old->img_addr, img_old->img_size, update_img_addr, update_img_size, update_type.type_t.update_type, img_old->img_part_size);
            if (ret < 0) {
                UPD_LOGE("e up dimg: %s", img_update->img_name);
                return ret;
            }

            type_flag = MISC_STATUS_RCVY_MANFEST_SET;
        }
        
        if (type_flag == MISC_STATUS_RCVY_MANFEST_SET) {
            if (misc_set_update_fd(MISC_UPDATA_CONTROL_SET, fd, type_flag)) {
                return -1;
            }
            if (get_section_buf(img_update->img_addr, img_update->img_size, &tmp_scn_type, &tmp_size)) {
                UPD_LOGE("e got mate info%.*s scntype:%d", MTB_IMAGE_NAME_SIZE, img_update->img_name, (*(uint32_t *)&tmp_scn_type));
                return -1;
            }
            UPD_LOGD("update mate info img_update->img_addr:%x-img_name:%.*s-img_size:%d",
                      img_update->img_addr, MTB_IMAGE_NAME_SIZE, img_update->img_name, tmp_size);
#if (CONFIG_MANTB_VERSION == 4)
            mtb_partition_info_t part_info;
            if (mtb_get_partition_info((const char *)img_update->img_name, &part_info)) {
                UPD_LOGE("get part info e");
                return -1;
            }
            scn_img_t scn_img;
            misc_get_scn_img_info(img_update->img_addr, &scn_img);
            UPD_LOGD("part_info.img_size:0x%x, scn_img.imginfo.image_size:0x%x", part_info.img_size, scn_img.imginfo.image_size);
            part_info.img_size = scn_img.imginfo.image_size;
            part_info.start_addr = scn_img.imginfo.static_addr;
            part_info.load_addr = scn_img.imginfo.loading_addr;
            part_info.part_type = scn_img.head.type;
            if (mtb_update_backup(g_upd_mtb, (const char *)img_update->img_name, &part_info, &update_flag)) {
                UPD_LOGE("e update mate %.*s", MTB_IMAGE_NAME_SIZE, img_update->img_name);
                return -1;
            }
#else
            if (update_imginfo(bm_addr, UPDATE_TYPE_IMG_INFO, img_update->img_name, (uint8_t *)(img_update->img_addr), tmp_size)) {
                UPD_LOGE("e update mate%.*s", MTB_IMAGE_NAME_SIZE, img_update->img_name);
                return -1;
            }
#endif
            type_flag = MISC_STATUS_PRIM_MANFEST_SET;
        }

        if (type_flag == MISC_STATUS_PRIM_MANFEST_SET) {
            if (misc_set_update_fd(MISC_UPDATA_CONTROL_SET, fd, type_flag)) {
                return -1;
            }
#if (CONFIG_MANTB_VERSION == 4)
            if (update_flag == 1) {
                if (mtb_update_valid(g_upd_mtb)) {
                    UPD_LOGE("update valid mtb e");
                    return -1;
                }
            }
#else
            if (update_img_mtb(1)) {
                return -1;
            }
#endif
        }
    }

    return 0;
}

static int get_real_misc_ctrl_ptr(misc_control_s *misc_ctrl, uint32_t address)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    if (boot_flash_read(address, (uint8_t *)misc_ctrl, sizeof(misc_control_s))) {
        return -1;
    }
#else
    memcpy(misc_ctrl, (void *)address, sizeof(misc_control_s));
#endif
    return 0;
}

/**
 * get misc status
 *
 * @return      0 on success; -1 on failure
 */
static int misc_get_update_status(bool *is_continue)
{
    misc_control_s misc_control;

    if (get_real_misc_ctrl_ptr(&misc_control, g_flash_misc_addr)) {
        return -1;
    }
    UPD_LOGD("misc_control->status:0x%x", misc_control.status);
    if (misc_control.status != MISC_UPDATA_STATUS_CONTINUE) {
        *is_continue = false;
        return 0;
    }
    *is_continue = true;
    return 0;
}

static uint32_t get_real_fd(uint32_t fd)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    return fd + (uint32_t)g_fota_tb_ram;
#else
    return fd;
#endif
}

static uint32_t set_real_fd(uint32_t fd)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    return (fd >= (uint32_t)g_fota_tb_ram) ? fd - (uint32_t)g_fota_tb_ram : fd;
#else
    return fd;
#endif
}

/**
 * update misc fd
 *
 * @param[in]   img_f      misc info
 * @param[in]   path       which path
 * @return      0 on success; -1 on failure
 */
int misc_get_update_fd(uint32_t *fd, uint32_t *status)
{
    uint32_t addr = g_flash_misc_addr;
    misc_control_s *misc_control;
    misc_control_s misc_control2;
    // misc_control_s *misc_control = (misc_control_s *)addr;
    // misc_control_s *misc_control2 = (misc_control_s *)(addr + g_flash_misc_section_size);
    uint32_t imisc_addr = get_fota_tb_address();
    uint32_t sum_fd = MISC_FLASH_FD_SUM(imisc_addr);
    uint32_t tmp_status = MISC_STATUS_IMG_SET;
    int32_t control_state = 0, i;
    uint8_t *buf = NULL;
    uint32_t word;

    buf = (uint8_t *)malloc(g_flash_misc_section_size);
    if (!buf) {
        UPD_LOGE("mem out");
        return -1;
    }
    misc_control = (misc_control_s *)buf;

    boot_flash_read(addr, (uint8_t *)misc_control, g_flash_misc_section_size);
    get_real_misc_ctrl_ptr(&misc_control2, addr);

    boot_flash_read(addr + g_flash_misc_section_size - 4, (uint8_t *)&word, 4);
    if ((misc_control->status == MISC_UPDATA_STATUS_CONTINUE) && word == MISC_STATUS_MAGIC_TAIL) {
        control_state |= 1;
    }
    boot_flash_read(addr + (g_flash_misc_section_size << 1) - 4, (uint8_t *)&word, 4);
    if ((misc_control2.status == MISC_UPDATA_STATUS_CONTINUE) && word == MISC_STATUS_MAGIC_TAIL) {
        control_state |= 2;
    }

    UPD_LOGD("control_state:%d", control_state);
    if (control_state == 2) {
        if (boot_flash_write(addr, (uint8_t *)&misc_control2, g_flash_misc_section_size)) {
            UPD_LOGE("e w ctl");
            free(buf);
            return UPDATE_PATCH_FAIL;
        }
    }

    if (control_state == 0) {
		// misc update init
        if (misc_set_update_fd(MISC_UPDATA_CONTROL_REST, 0, 0)) {
            free(buf);
            UPD_LOGE("UPDATE_CHECK_FAIL");
            return UPDATE_CHECK_FAIL;
        }

		*fd = misc_first_fd();
        UPD_LOGD("misc_first_fd=0x%x", *fd);
        UPD_LOGD("==================>>>>%s", ((misc_img_info_t *)*fd)->img_name);
		*status = MISC_STATUS_IMG_SET;
    } else {
        for (i = sum_fd - 1; i > -1; i--) {
            if (misc_control->fd_info[i].type[MISC_STATUS_IMG_SET] != MISC_STATUS_MAGIC) {
                tmp_status = MISC_STATUS_IMG_SET;
                break;
            }

            if (misc_control->fd_info[i].type[MISC_STATUS_RCVY_MANFEST_SET] != MISC_STATUS_MAGIC) {
                tmp_status = MISC_STATUS_RCVY_MANFEST_SET;
                break;
            }

            if (misc_control->fd_info[i].type[MISC_STATUS_PRIM_MANFEST_SET] != MISC_STATUS_MAGIC) {
                tmp_status = MISC_STATUS_PRIM_MANFEST_SET;
                break;
            }
        }

        UPD_LOGD("seach misc_control->fd:%x,type:%d,i:%d", get_real_fd(misc_control->fd_info[i].type[MISC_STATUS_FD_ADDR]), tmp_status, i);


        if (i < 0) {
            i = 0;
            *fd = get_real_fd(misc_control->fd_info[i].type[MISC_STATUS_FD_ADDR]);
            *status = MISC_STATUS_PRIM_MANFEST_SET;
        } else {
            if (tmp_status == MISC_STATUS_IMG_SET) {
                if (i != sum_fd - 1) {
                    *fd = get_real_fd(misc_control->fd_info[i + 1].type[MISC_STATUS_FD_ADDR]);
                    *status = MISC_STATUS_PRIM_MANFEST_SET;
                } else {
                    *fd = misc_first_fd();
                    *status = MISC_STATUS_IMG_SET;
                }
            } else {
                *status = tmp_status - 1;
                *fd = get_real_fd(misc_control->fd_info[i].type[MISC_STATUS_FD_ADDR]);
            }
        }

        UPD_LOGD("got misc_control->fd:%x,type:%d", get_real_fd(misc_control->fd_info[i].type[MISC_STATUS_FD_ADDR]), tmp_status);

        if (misc_fd_check(*fd) || *status > MISC_STATUS_PRIM_MANFEST_SET) {
            UPD_LOGE("e fd info");
            free(buf);
            return UPDATE_PATCH_FAIL;
        }

    }
    UPD_LOGD("misc get fd over, 0x%x", buf);
    free(buf);
    return 0;
}

/**
 * update misc status
 *
 * @param[in]   img_f      misc info
 * @param[in]   path       which path
 * @return      0 on success; -1 on failure
 */

int misc_set_update_fd(uint32_t type, uint32_t fd, uint32_t status)
{
    uint32_t addr = g_flash_misc_addr;
    int ret = UPDATE_CHECK_FAIL;
    uint32_t fd_num;
    uint8_t  buf[g_flash_misc_section_size];

    boot_flash_read(addr, buf, g_flash_misc_section_size);
    misc_control_s *misc_control = (misc_control_s *)buf;
    UPD_LOGD("type=%d, g_flash_misc_addr=0x%x, g_flash_misc_section_size=0x%x", type, addr, g_flash_misc_section_size);
    if (type == MISC_UPDATA_CONTROL_SET) {
        ret = UPDATE_PATCH_FAIL;
        UPD_LOGD("set fd:%x,stas:%d", fd, status);

        if (status > MISC_STATUS_PRIM_MANFEST_SET) {
            UPD_LOGE("e fd type");
            return ret;
        }

        if (misc_control->status == MISC_UPDATA_STATUS_CONTINUE) {
            fd_num = MISC_FLASH_FD_NUM(get_fota_tb_address(), fd);

            UPD_LOGI("fd:%x,fd_num:%d", fd, fd_num);

            if (misc_fd_check(fd)) {
                UPD_LOGE("e fd info");
                return ret;
            }

            if (status == MISC_STATUS_IMG_SET) {
                misc_control->fd_info[fd_num].type[MISC_STATUS_FD_ADDR] = set_real_fd(fd);
            }

            misc_control->fd_info[fd_num].type[status] = MISC_STATUS_MAGIC;
        }
    } else if (type == MISC_UPDATA_CONTROL_REST) {
        if (boot_flash_erase(addr, g_flash_misc_section_size << 1)) {
            return ret;
        }

        memset(buf, 0xff, g_flash_misc_section_size);
        misc_control->status = MISC_UPDATA_STATUS_CONTINUE;
    } else {
        return ret;
    }

    *(uint32_t *)(buf + g_flash_misc_section_size - 4) = MISC_STATUS_MAGIC_TAIL;

    if (boot_flash_erase(addr + g_flash_misc_section_size , g_flash_misc_section_size)) {
        UPD_LOGE("e w mcbk stas");
        return ret;
    }

    if (boot_flash_write(addr + g_flash_misc_section_size , (uint8_t *)buf, g_flash_misc_section_size)) {
        UPD_LOGE("e w mcbk stas");
        return ret;
    }

    if (boot_flash_erase(addr, g_flash_misc_section_size)) {
        UPD_LOGE("e w mcbk stas");
        return ret;
    }

    if (boot_flash_write(addr, (uint8_t *)buf, g_flash_misc_section_size)) {
        UPD_LOGE("e w mcbk stas");
        return ret;
    }

    return 0;
}

/*
返回mem中crc和signature的地址
 */
uint8_t *misc_get_imtb_crc_and_sig(uint32_t *olen)
{
    // |4bytes erase value|4bytes crc32|
    uint8_t tail_f[8];
    static uint8_t crc32_v[4];

    *olen = 4;
    // 读取末尾8个字节，最后1个word为保存着的新的crc32校验值;
    // 倒数第2个word为flash擦除后的值，不加密的就是0xFFFFFFFF
    boot_flash_read(g_upd_mtb->prim_addr + g_upd_mtb->one_size - 8, tail_f, 8);
    if (memcmp(&tail_f[0], &tail_f[4], 4) == 0) {
        return NULL;
    }
    memcpy(crc32_v, &tail_f[4], 4);
    UPD_LOGD("### imtb crc32 = 0x%x%x%x%x", crc32_v[0], crc32_v[1], crc32_v[2], crc32_v[3]);
    return crc32_v;
}

int misc_get_app_version(uint8_t *out, uint32_t *olen)
{
    uint32_t addr = get_fota_tb_address();
    uint32_t ver_addr = MISC_OS_VERSION_ADDR(addr);
    int fota_ver = MISC_FLASH_FOTA_VERSION(addr);
    int app_ver_size = 32;

    if (fota_ver == 1) {
        app_ver_size = 64;
    }

    memcpy(out, (uint8_t *)ver_addr, app_ver_size);
    out[app_ver_size - 1] = 0;
    *olen = app_ver_size;
    return 0;
}
#endif