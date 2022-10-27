/**
* Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
*/

#ifndef __KM_PUBKEY_H__
#define __KM_PUBKEY_H__

#include <stdint.h>
#include "key_mgr.h"
#include "key_mgr_port.h"

#define MANIFEST_IDX_MAGIC      0x78446E49 // "InDx"
#define PUBLIC_KEY_NAME_SIZE    8
#define OTP_PUB_REGION_MAGIC    0xFDE2C04F
#define PKEY_PART_MAGIC         0xECE2C04F

typedef enum {
    OTP_HASH_SHA1   = 1,
    OTP_HASH_MD5    = 2,
    OTP_HASH_SHA224 = 3,
    OTP_HASH_SHA256 = 4,
    OTP_HASH_SHA384 = 5,
    OTP_HASH_SHA512 = 6,
    OTP_RSA1024     = 10,
    OTP_RSA2048     = 11,
    OTP_ECC256      = 12,
    OTP_ECC160      = 13,
    OTP_POINT       = 20
} otp_pubkey_type_e;

uint32_t km_pub_key_init();
uint32_t km_get_pub_key_with_name(const char *pub_key_name, key_handle *key_addr, uint32_t *key_len);
void km_show_pub_key_info(void);

#endif