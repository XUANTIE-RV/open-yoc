/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "stdint.h"
#include "errno.h"
#include "ulog/ulog.h"
#include "crypto_md.h"
#include "sec_crypto_auth.h"
#include "key_mgr.h"
//#include "mesh_occ_auth.h"
#include <sys/time.h>
#include <common/log.h>

#define TAG      "MESH_OCC_AUTH"
#define ERR_CHAR 0XFF

extern int dut_hal_mac_store(uint8_t addr[6]);

static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return ERR_CHAR;
    }
}

static int str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return 0;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }

    return i;
}

static int sc_get_auth_data(message_auth_t *msg, uint32_t *authcode)
{
    key_handle key_addr;
    uint32_t   len;
    uint8_t    vid[9];

    int ret = km_get_key(KEY_ID_MAC, &key_addr, &len);

    if (ret != KM_OK) {
        return -1;
    }

    memcpy(msg->mac, (uint8_t *)key_addr, 6);

    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &len);

    if (ret != KM_OK) {
        return -1;
    }

    memcpy(vid, (uint8_t *)key_addr + 8, 8);

    vid[8]         = 0;
    uint32_t temp  = strtol((const char *)vid, NULL, 16);
    msg->vendor_id = (temp & 0xff800) >> 11;
    uint32_t   rand_data;
    extern int bt_rand(void *buf, size_t len);
    bt_rand(&rand_data, 4);
    msg->rand = (uint8_t)(rand_data & 0xff);

    ret = sc_srv_get_message_auth(msg, authcode);

    if (ret) {
        return -1;
    }

    LOGD(TAG, "auth:%06x", *authcode);
    return 0;
}

int occ_auth_init(void)
{
    static uint8_t init_flag = 0;

    if (init_flag) {
        return -EALREADY;
    }

    km_init();
    init_flag = 1;
    return 0;
}

static int occ_auth_get_oob(uint32_t *short_oob)
{
    if (!short_oob) {
        return -1;
    }

    key_handle key_addr;
    uint32_t   len      = 0;
    uint32_t   oob_len  = 0;
    int        ret      = 0;
    uint8_t    Salt[16] = { 0 };
    uint8_t    oob[16]  = { 0x0 };

    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &len);

    if (ret != KM_OK) {
        return -1;
    }

    str2hex(Salt, (char *)key_addr, 16);
    ret = sc_srv_get_auth_key(Salt, 16, oob, &oob_len);

    if (ret) {
        LOGE(TAG, "Get oob key failed %d", ret);
        return -1;
    }
    LOGD(TAG, "Get long oob %s", bt_hex_real(oob, 16));
    *short_oob = oob[0] << 24 | oob[1] << 16 | oob[2] << 8 | oob[3];
    return 0;
}

int occ_auth_get_uuid_and_oob(uint8_t uuid[16], uint32_t *short_oob)
{
    if (!uuid) {
        return -EINVAL;
    }

    int            ret      = 0;
    message_auth_t msg      = { 0 };
    uint32_t       authcode = 0;
    ret                     = sc_get_auth_data(&msg, &authcode);
    if (ret) {
        LOGE(TAG, "sc get auth failed");
        return -1;
    }

    uuid[0] = authcode & 0x000000ff;
    uuid[1] = (authcode & 0x0000FF00) >> 8;
    uuid[2] = (authcode & 0x00FF0000) >> 16;
    uuid[3] = (authcode & 0xFF000000) >> 24;

    memcpy(uuid + 4, msg.mac, 6); // 6B MAC

    uuid[10] = msg.rand; // 1B rand
    uuid[11] = msg.vendor_id & 0x00FF;
    uuid[12] = (msg.vendor_id & 0xFF00) >> 8;
    LOGD(TAG, "kp uuid get:%s", bt_hex(uuid, 16));

    ret = occ_auth_get_oob(short_oob);
    if (ret) {
        LOGE(TAG, "sc get short oob failed %d", ret);
        return -1;
    }

    return 0;
}

int occ_auth_set_dev_mac_by_kp(void)
{
    int            ret      = 0;
    message_auth_t msg      = { 0 };
    uint32_t       authcode = 0;
    ret                     = sc_get_auth_data(&msg, &authcode);
    if (ret) {
        LOGE(TAG, "sc get auth failed");
        return -1;
    }
#if defined(CONFIG_DUT_SERVICE_ENABLE) && CONFIG_DUT_SERVICE_ENABLE > 0
    ret = dut_hal_mac_store(msg.mac);
    if (ret) {
        LOGE(TAG, "Dut Mac write failed");
        return -1;
    }
#else
    LOGE(TAG, "dut_service Components not support write");
    return -1;
#endif
    return 0;
}
