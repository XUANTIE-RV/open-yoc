/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#ifndef __OTA_AB_IMG_H__
#define __OTA_AB_IMG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_AB_IMG_HEAD_SIZE 1024UL
#define OTA_AB_IMG_INFO_OFFSET_GET(sector) (sector << 1)

typedef struct {
#define IMG_NAME_MAX_LEN    16
    char img_name[IMG_NAME_MAX_LEN];
    uint32_t offset;
    uint32_t size;
} pack_header_imginfo_v2_t;

typedef struct {
#define PACK_IMG_MAX_COUNT 15U
#define PACK_HEAD_MAGIC 0x4B434150
    uint32_t      magic;          // "PACK"  0x4B434150
    uint16_t      head_version;   // 2
    uint16_t      head_size;      // length of header
    uint32_t      head_checksum;  // the checksum for header, fill 0 when calculate checksum
    uint32_t      image_count;    // image count to pack
    unsigned char md5sum[16];     // the md5sum for md5sum(image1+image2+image3...)
    uint16_t      digest_type;    // the digest type
    uint16_t      signature_type; // the signature type
    unsigned char signature[512]; // the signature for header + image, fill 0 when calculate checksum or calculate signature
    uint32_t      rsv[29];        // reverse
    pack_header_imginfo_v2_t image_info[PACK_IMG_MAX_COUNT]; // 24*15=360B
} pack_header_v2_t; // 1024Bytes

typedef struct {
#define IMG_MAX_COUNT 8
    uint32_t image_count;
    uint32_t head_size;
    unsigned char md5sum[16];
    uint16_t digest_type;
    uint16_t signature_type;
    unsigned char signature[512];
    struct {
        char partition_name[IMG_NAME_MAX_LEN + 4];
        uint32_t img_offset;
        uint32_t img_size;
        uint32_t write_size;
        uint32_t read_size;
        int partition_handle;
        uint32_t partition_size;
        unsigned long partition_abs_addr;
    } img_info[IMG_MAX_COUNT];
} download_img_info_t;

typedef struct {
    int envab_handle;
    download_img_info_t dl_imgs_info;
} pack_private_t;

#ifdef __cplusplus
}
#endif
#endif