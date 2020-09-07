/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <kp.h>
#include "efuse.h"
#include <tee_addr_map.h>
#include <tee_debug.h>

#if (CONFIG_TB_KP > 0)

#define KP_IDX_BASE   OTP_BASE_ADDR
#define KP_IDX_MAGIC  0x78446E49    // "InDx"

static uint32_t g_kp_buf[OTP_BANK_SIZE >> 2];

typedef struct {
    uint16_t type : 1;
    uint16_t rsvr : 15;
} kp_key_flag_t;

typedef struct {
    uint16_t          type;
    uint16_t          size;
    kp_key_flag_t     flag;
    uint16_t          rsvr;
    uint32_t          offset;
    uint32_t          rsvr2;
} kp_key_info_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t rsv;
    uint16_t keynum;
    uint16_t digest_type;
    uint32_t total_size;
} kp_head_t;

typedef struct {
    kp_head_t head;
    kp_key_info_t key[0];
} kp_t;

__attribute__((weak)) uint32_t kp_addr(void)
{
    return OTP_BASE_ADDR;
}

__attribute__((weak)) uint32_t kp_otp_block_size(void)
{
    return OTP_BANK_SIZE;
}

__attribute__((weak)) uint8_t kp_magic_number_check(uint32_t magic_number)
{
    return (KP_IDX_MAGIC == magic_number);
}

static int kp_check(kp_t *kp)
{
    if (!kp) {
        return KP_ERR_NULL;
    }

    if (0 == kp_magic_number_check(kp->head.magic)) {
        return KP_ERR_MAGIC_NUM;
    }

    if (0x03 != kp->head.version) {
        return KP_ERR_VERSION;
    }

    return KP_OK;
}

int kp_init(void)
{
    int32_t len;
    
    memset(g_kp_buf, 0, sizeof(g_kp_buf));
    if ((len = efusec_read_bytes(kp_addr(), (uint8_t *)g_kp_buf, sizeof(g_kp_buf))) <= 0) {
        TEE_LOGE("efuse read e..\n");
        return -1;
    }
    if (kp_check((kp_t*)g_kp_buf) != KP_OK) {
        // FIXME: for test,规定从0x8FFF000地址获取KP数据，为了测试使用
        memcpy(g_kp_buf, (void *)0x8FFF000, sizeof(g_kp_buf));
        len = sizeof(g_kp_buf);
    }

    if (len > kp_otp_block_size()) {
        TEE_LOGE("efuse read size too long, %d\n", len);
        return -1;
    }
    TEE_LOGD("efuse read data len:%d\n", len);
    // uint8_t *p = (uint8_t *)g_kp_buf;
    // for (int i = 0; i < len; i++) {
    //     if (i%16 == 0)
    //         printf("\n");
    //     printf("%02x ", p[i]);
    // }
    // printf("\n----------------------------------\n");

    return 0;
}

int kp_get_key(key_type_e key_type, uint32_t *key)
{
    kp_t *kp;
    kp_key_info_t *key_info;
    uint16_t key_num;
    uint32_t key_addr = 0;
    int i, ret;

    TEE_LOGD("key type %d, key %x\n", key_type, key);

    if (key_type >= KEY_INVAILD) {
        return KP_ERR_KEY_TYPE;
    }

    if (!key) {
        return KP_ERR_NULL;
    }

    kp = (kp_t *)g_kp_buf;
    ret = kp_check(kp);

    if (KP_OK != ret) {
        return ret;
    }

    TEE_LOGD("kp addr %x\n", kp);

    key_info = kp->key;
    key_num = kp->head.keynum;

    for (i = 0; i < key_num; i++) {
        if (key_info->type == key_type) {
            break;
        }

        key_info++;
    }

    if (i >= key_num) {
        return KP_ERR_NOT_FOUND;
    }

    if (0 == key_info->size) {
        return KP_ERR_KEY_SIZE;
    }

    if (0x03 != kp->head.version) {
        return KP_ERR_VERSION;
    }

    key_addr = (uint32_t)g_kp_buf + key_info->offset;
    *key = key_addr;

    TEE_LOGD("find key_addr %x, size %d\n", key_addr, key_info->size);

    return key_info->size;
}

#endif
