/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef MTB_H
#define MTB_H

#ifdef __cplusplus
extern "C" {
#endif

#define BMTB_MAGIC             0x74427254
#define MTB_MAGIC              0x74427251

#define MTB_OS_VERSION_LEN     32    /* for version 1.0, os version length is 32*/
#define MTB_OS_VERSION_LEN_V2  64 /* for version 2.0, os version length is 64*/
#define MTB_OS_VERSION_LEN_V4  64 /* for version 4.0, os version length is 64*/
#define MTB_IMAGE_VERSION_SIZE 12
#define MTB_IMAGE_NAME_SIZE    8
#define PUBLIC_KEY_NAME_SIZE   8

#define MTB_KEY_BIT            1024
#define MTB_KEY_BYTE           (MTB_KEY_BIT >> 3)
#define MTB_KEY_N_SIZE         MTB_KEY_BYTE
#define MTB_KEY_E_SIZE         MTB_KEY_BYTE
#define MTB_KEY_D_SIZE         (MTB_KEY_BYTE * 2)
#define MTB_KEY_SIZE           (MTB_KEY_N_SIZE + MTB_KEY_E_SIZE)

#define MTB_ID2KEY_BYTE      16


#pragma pack(1)
typedef struct {
    uint16_t    index;
    uint16_t    reserve;
    uint32_t    image_size;
    uint8_t     image_name[MTB_IMAGE_NAME_SIZE];
    uint64_t    static_addr;
    uint64_t    loading_addr;
} image_info_t;

typedef struct {
    uint8_t  image_name[MTB_IMAGE_NAME_SIZE];
    uint64_t static_addr;
    uint64_t loading_addr;
    uint32_t image_size;
    uint8_t  image_version[MTB_IMAGE_VERSION_SIZE];
} scn_img_sig_info_t;

typedef struct {
    uint8_t  image_name[MTB_IMAGE_NAME_SIZE];
    uint64_t part_addr;
    uint64_t part_size;
} scn_partion_info_t;

typedef struct {
    uint8_t n[MTB_KEY_N_SIZE];
    uint8_t e[MTB_KEY_E_SIZE];
} scn_key_info_t;

typedef enum {
    SCN_TYPE_IMG = 0,
    SCN_TYPE_KEY = 4,
    SCN_TYPE_PART = 5,
} scn_type_first_e;

typedef enum {
    SCN_TYPE_NORMAL = 0,
} scn_type_extend_e;

typedef enum {
    SCN_SUB_TYPE_IMG_NOR = 0,
    SCN_SUB_TYPE_IMG_SAFE = 1
} scn_sub_type_img_e;

typedef enum {
    SCN_SUB_TYPE_KEY_BOOT = 0,
} scn_sub_type_key_e;

typedef enum {
    SCN_SUB_TYPE_IMG_PART = 0,
} scn_sub_type_part_e;

typedef struct {
    uint8_t extend_type: 4;
    uint8_t first_type: 4;
} scn_f_type_t;

typedef struct {
    scn_f_type_t father_type;
    uint8_t son_type;
} scn_head_type_t;

typedef struct {
    scn_head_type_t type;
    uint16_t index;
    uint32_t size;
} scn_head_t;

typedef struct {
    scn_head_t scn_head;
    union {
        scn_img_sig_info_t img[0];
        scn_partion_info_t partition[0];
        scn_key_info_t key[0];
    };
} scn_t;

typedef struct {
    uint8_t v[MTB_OS_VERSION_LEN];
} mtb_os_version_t;

typedef struct {
    uint8_t v[MTB_OS_VERSION_LEN_V2];
} mtb_os_version2_t;

typedef struct {
    uint8_t v[MTB_OS_VERSION_LEN_V4];
} mtb_os_version4_t;

typedef struct {
    uint16_t encrypted: 1;
    uint16_t reserve: 7;
    uint16_t update_flag: 1;
    uint16_t reserve2: 7;
} tb_flag;

typedef enum {
    DIGEST_HASH_NONE = 0,
    DIGEST_HASH_SHA1 = 1,
    DIGEST_HASH_MD5  = 2,
    DIGEST_HASH_SHA224  = 3,
    DIGEST_HASH_SHA256  = 4,
    DIGEST_HASH_SHA384  = 5,
    DIGEST_HASH_SHA512  = 6
} digest_sch_e;

typedef enum {
    SIGNATURE_HASH_NONE = 0,
    SIGNATURE_HASH_SHA1 = 1
} signature_sch_e;

typedef struct {
    uint32_t magic;
    uint16_t version;
    tb_flag  flag;
    uint16_t scn_count;
    uint16_t digest_sch;
    uint16_t signature_sch;
    uint16_t size;
} mtb_head_t;

// imtb v4
typedef struct {
    uint32_t    magic;
    uint16_t    version;
    tb_flag     flag;
    uint16_t    digest_sch;
    uint16_t    signature_sch;
    char        pkey_name[PUBLIC_KEY_NAME_SIZE];
    uint16_t    partition_count;
    uint16_t    size;
} imtb_head_v4_t;

typedef struct {
    char        name[MTB_IMAGE_NAME_SIZE];
    char        pub_key_name[PUBLIC_KEY_NAME_SIZE];
    scn_head_type_t  partition_type;
    uint16_t    block_count;
    uint32_t    block_offset;
    uint32_t    load_address;
    uint32_t    img_size;
} imtb_partition_info_v4_t;

typedef struct {
    imtb_head_v4_t              head;
    uint8_t                     app_version[MTB_OS_VERSION_LEN_V4];
    imtb_partition_info_v4_t    *partition_info;
    uint8_t                     *crc32;
    uint8_t                     *signature;
} imtb_v4_t;

typedef struct {
    char        name[MTB_IMAGE_NAME_SIZE];
    char        pub_key_name[PUBLIC_KEY_NAME_SIZE];
    scn_head_type_t  partition_type;
    uint32_t    part_start;
    uint32_t    part_size;
    uint32_t    load_address;
    uint32_t    img_size;
} imtb_user_partition_info_v4_t;

#pragma pack()

typedef struct {
    union {
        mtb_head_t *head;
        imtb_head_v4_t *headv4;
    };
    union {
        mtb_os_version_t *os_version;
        mtb_os_version2_t *os_version2;
        mtb_os_version4_t *os_version4;
    };
    union {
        scn_t *scn;
        image_info_t *img;
    };
} mtb_t;

typedef struct {
    uint8_t img_name[MTB_IMAGE_NAME_SIZE];
    uint8_t img_ver[MTB_IMAGE_VERSION_SIZE];
    uint32_t img_addr;
    uint32_t img_load_addr;
    uint32_t img_size;
} mtb_img_info_t;

typedef struct {
    uint8_t *n;
    uint8_t *e;
    uint8_t *d;
    uint32_t size;
} key_info_t;

int mtb_init();
int mtb_get_img_info(char *name, mtb_img_info_t *img_info);
int mtb_get_os_version(uint8_t *buf, uint32_t *size);
int mtb_get_img_buf(uint8_t *buf, uint32_t *size, char *name);
int mtb_get_partition_buf(uint8_t *buf, uint32_t *size);
int mtb_get_ntw_addr(uint32_t *addr, uint32_t *load_addr, uint32_t *size);
int mtb_verify_image(const char *name);
int mtb_verify_images(void);
int mtb_get_key_info(key_info_t *info);
int mtb_get_pubkey_info(key_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
