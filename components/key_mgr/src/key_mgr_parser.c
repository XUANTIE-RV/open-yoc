/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <key_mgr_parser.h>
#include "key_mgr_log.h"

#if (CONFIG_TB_KP > 0)
#include <tee_addr_map.h>
#define KP_IDX_BASE   CONFIG_OTP_BASE_ADDR
#define KP_IDX_MAGIC  0x78446E49    // "InDx"

#ifdef CONFIG_CHIP_PANGU
#include "key_mgr_efuse.h"

#include <soc.h>

static uint32_t g_parser_buf[CONFIG_OTP_BANK_SIZE >> 2];

typedef struct {
    uint16_t type : 1;
    uint16_t rsvr : 15;
} parser_key_flag_t;

typedef struct {
    uint16_t          type;
    uint16_t          size;
    parser_key_flag_t     flag;
    uint16_t          rsvr;
    uint32_t          offset;
    uint32_t          rsvr2;
} parser_key_info_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t rsv;
    uint16_t keynum;
    uint16_t digest_type;
    uint32_t total_size;
} parser_head_t;

typedef struct {
    parser_head_t head;
    parser_key_info_t key[0];
} parser_t;

__attribute__((weak)) uint32_t parser_addr(void)
{
    return CONFIG_OTP_BASE_ADDR;
}

__attribute__((weak)) uint32_t parser_otp_block_size(void)
{
    return CONFIG_OTP_BANK_SIZE;
}

__attribute__((weak)) uint8_t parser_magic_number_check(uint32_t magic_number)
{
    return (KP_IDX_MAGIC == magic_number);
}

static int parser_check(parser_t *kp)
{
    if (!kp) {
        return KP_ERR_NULL;
    }

    if (0 == parser_magic_number_check(kp->head.magic)) {
        return KP_ERR_MAGIC_NUM;
    }

    if (0x03 != kp->head.version) {
        return KP_ERR_VERSION;
    }

    return KP_OK;
}

uint32_t parser_init(void)
{
    int32_t len;

    memset(g_parser_buf, 0, sizeof(g_parser_buf));
    if ((len = efusec_read_bytes(parser_addr(), (uint8_t *)g_parser_buf, sizeof(g_parser_buf))) <= 0) {
        KM_LOGE("efuse read e..\n");
        return KM_ERR;
    }
    if (parser_check((parser_t*)g_parser_buf) != KP_OK) {
        // FIXME: for test,规定从0x8FFF000地址获取KP数据，为了测试使用
        memcpy(g_parser_buf, (void *)0x8FFF000, sizeof(g_parser_buf));
        len = sizeof(g_parser_buf);
    }

    if (len > parser_otp_block_size()) {
        KM_LOGE("efuse read size too long, %d\n", len);
        return KM_ERR_TOO_LONG;
    }
    KM_LOGD("efuse read data len:%d", len);
    // uint8_t *p = (uint8_t *)g_parser_buf;
    // for (int i = 0; i < len; i++) {
    //     if (i%16 == 0)
    //         printf("\n");
    //     printf("%02x ", p[i]);
    // }
    // printf("\n----------------------------------\n");

    return KM_OK;
}

uint32_t parser_get_key(key_type_e key_type, key_handle *key, uint32_t *key_size)
{
    parser_t *kp;
    parser_key_info_t *key_info;
    uint16_t key_num;
    uint32_t key_addr = 0;
    int i, ret;

    KM_LOGD("key type %d, key %x\n", key_type, key);

    if (key_type >= KEY_INVAILD) {
        return KP_ERR_KEY_TYPE;
    }

    if (!key) {
        return KP_ERR_NULL;
    }

    kp = (parser_t *)g_parser_buf;
    ret = parser_check(kp);

    if (KP_OK != ret) {
        return ret;
    }

    KM_LOGD("kp addr %x\n", kp);

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

    key_addr = (uint32_t)g_parser_buf + key_info->offset;
    *key = key_addr;

    KM_LOGD("find key_addr %x, size %d\n", key_addr, key_info->size);

    *key_size =  key_info->size;

    return KM_OK;
}

// #elif CONFIG_PLATFORM_HOBBIT1_2 > 0
#else

typedef struct {
    uint16_t type : 1;
    uint16_t rsvr : 15;
} parser_key_flag_t;

typedef struct {
    uint32_t info : 28;
    uint32_t type : 4;
} parser_pos_flag_t;

typedef struct {
    uint16_t          type;
    uint16_t          size;
    parser_key_flag_t     flag;
    uint16_t          rsvr;
    union {
        parser_pos_flag_t pos;
        uint32_t      key_addr;
    };
} parser_key_info_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t keynum;
} parser_head_t;

typedef struct {
    parser_head_t head;
    parser_key_info_t key[0];
} parser_t;

typedef enum {
    FUSE = 1,
    NANDFLASH,
    NORFLASH,
    SE
} parser_pos_type_e;

__attribute__((weak)) uint32_t parser_addr(void)
{
    return CONFIG_OTP_BASE_ADDR;
}

__attribute__((weak)) uint32_t parser_otp_block_size(void)
{
    return CONFIG_OTP_BANK_SIZE;
}

__attribute__((weak)) uint8_t parser_magic_number_check(uint32_t magic_number)
{
    return (KP_IDX_MAGIC == magic_number);
}

static int parser_check(parser_t *kp)
{
    if (!kp) {
        return KP_ERR_NULL;
    }

    if (0 == parser_magic_number_check(kp->head.magic)) {
        return KP_ERR_MAGIC_NUM;
    }

    if (0x01 != kp->head.version && 0x02 != kp->head.version) {
        return KP_ERR_VERSION;
    }

    return KP_OK;
    }

uint32_t parser_init(void)
{
    return 0;
}
uint32_t parser_get_key(key_type_e key_type, key_handle *key, uint32_t *key_size)
{
    parser_t *kp;
    parser_key_info_t *key_info;
    uint16_t key_num;
    uint32_t key_addr = 0;
    int i, ret;

    KM_LOGD("key type %d, key %x\n", key_type, key);

    if (key_type >= KEY_INVAILD) {
        return KP_ERR_KEY_TYPE;
    }

    if (!key) {
        return KP_ERR_NULL;
    }

    kp = (parser_t *)parser_addr();
    ret = parser_check(kp);

    if (KP_OK != ret) {
        return ret;
    }

    KM_LOGD("kp addr %x\n", kp);

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

    if (0x01 == kp->head.version) {
        if (key_info->pos.type == 0) {
            key_addr = key_info->key_addr;
        } else if (key_info->pos.type == FUSE) {
            key_addr = (key_info->pos.info >> 16) * parser_otp_block_size() \
                       + ((key_info->pos.info & 0xffff) >> 3) + (uint32_t)kp;
        } else {
            return KP_ERR_POS_TYPE;
        }
    } else if (0x02 == kp->head.version) {
        key_addr = key_info->key_addr;
    }

    *key = key_addr;
    KM_LOGD("find key_addr %x, size %d\n", key_addr, key_info->size);

    *key_size = key_info->size;

    return KM_OK;
}

#endif /* CONFIG_CHIP_PANGU */

#else

uint32_t parser_init(void)
{
    return KM_ERR;
}

uint32_t parser_get_key(key_type_e key_type, key_handle *key, uint32_t *key_size)
{
    return KM_ERR;
}

#endif /* CONFIG_TB_KP */
