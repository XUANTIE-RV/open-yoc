/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/cli.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <dut/hal/common.h>
#include "mesh_node.h"

#define TRIPLE_OFFSET       (0)
#define TRIPLE_CRC_MAGIC    (0xA6A5)
#define TRIPLE_MAX_DN_SIZE  32
#define TRIPLE_MAX_PK_SIZE  20
#define TRIPLE_MAX_PS_SIZE  64
#define TRIPLE_CRC_LEN      2
#define MAX_SIZE_TRI_TRUPLE (2 + TRIPLE_MAX_DN_SIZE + 2 + TRIPLE_MAX_PK_SIZE + 2 + TRIPLE_MAX_PS_SIZE + TRIPLE_CRC_LEN)

#define TRIGET_FLAG "triget:"

enum
{
    GET_SUCCESS,
    GET_FAILED,
} triple_get_status_en;

enum
{
    TRIPLE_TYPE_DN = 0x1,
    TRIPLE_TYPE_PK = 0x2,
    TRIPLE_TYPE_PS = 0x4,
} triple_type_en;

typedef struct _triple_s {
    char dn[TRIPLE_MAX_DN_SIZE + 1];
    char pk[TRIPLE_MAX_PK_SIZE + 1];
    char ps[TRIPLE_MAX_PS_SIZE + 1];
} triple_t;

#define TAG "CLI_TRIPLE"

static uint16_t triple_crc16_compute(uint8_t const *p_data, uint32_t size, uint16_t const *p_crc)
{
    uint16_t crc = (p_crc == NULL) ? 0xFFFF : *p_crc;

    for (uint32_t i = 0; i < size; i++) {
        crc = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (uint8_t)(crc & 0xFF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xFF) << 4) << 1;
    }

    return crc;
}

static int32_t hal_flash_read_otp(uint32_t off_set, void *out_buf, uint32_t out_buf_len)
{
    return dut_hal_factorydata_read(off_set, out_buf, out_buf_len);
}

static int32_t hal_flash_write_otp(uint32_t off_set, const void *in_buf, uint32_t in_buf_len)
{
    int32_t retval = 0;

    if (off_set + in_buf_len > MAX_SIZE_TRI_TRUPLE) {
        LOGE(TAG, "param err");
        return -1;
    }

    retval = dut_hal_factorydata_store(off_set, (uint8_t *)in_buf, in_buf_len);

    return retval;
}

static int32_t hal_flash_read_triples(void *out_buf, uint32_t out_buf_len)
{
    return hal_flash_read_otp(TRIPLE_OFFSET, out_buf, out_buf_len);
}

static int32_t hal_flash_write_triples(const void *in_buf, uint32_t in_buf_len)
{
    return hal_flash_write_otp(TRIPLE_OFFSET, in_buf, in_buf_len);
}

int triple_write(char *d_n, char *p_k, char *p_s)
{
    uint8_t  total_len                 = 0;
    uint16_t triple_crc                = 0;
    uint16_t triple_crc_magic          = TRIPLE_CRC_MAGIC;
    uint8_t  data[MAX_SIZE_TRI_TRUPLE] = { 0x00 };

    memset(data, 0x0, sizeof(data));
    data[total_len++] = TRIPLE_TYPE_DN;
    data[total_len++] = strlen(d_n);
    memcpy(data + total_len, d_n, strlen(d_n));
    total_len += strlen(d_n);

    data[total_len++] = TRIPLE_TYPE_PK;
    data[total_len++] = strlen(p_k);
    memcpy(data + total_len, p_k, strlen(p_k));
    total_len += strlen(p_k);

    data[total_len++] = TRIPLE_TYPE_PS;
    data[total_len++] = strlen(p_s);
    memcpy(data + total_len, p_s, strlen(p_s));
    total_len += strlen(p_s);

    if (MAX_SIZE_TRI_TRUPLE - total_len < 2) {
        LOGE(TAG, "No space left for crc");
        return -1;
    }

    total_len = MAX_SIZE_TRI_TRUPLE - 2;

    triple_crc = triple_crc16_compute(data, total_len, &triple_crc_magic);
    memcpy(data + total_len, (unsigned char *)&triple_crc, TRIPLE_CRC_LEN);
    total_len += TRIPLE_CRC_LEN;
    return hal_flash_write_triples(data, MAX_SIZE_TRI_TRUPLE);
}

int triple_read(char *d_n, char *p_k, char *p_s)
{
    uint16_t triple_crc       = 0;
    uint16_t triple_read_crc  = 0;
    uint16_t triple_crc_magic = TRIPLE_CRC_MAGIC;
    char *   p_data           = NULL;
    int      ret;
    uint8_t  total_offset                  = 0;
    uint8_t  data_len                      = 0;
    char     data[MAX_SIZE_TRI_TRUPLE + 1] = { 0x0 };

    ret = hal_flash_read_triples(data, MAX_SIZE_TRI_TRUPLE);
    if (0 != ret) {
        LOGE(TAG, "read triple fail:%d", ret);
        return ret;
    }

    triple_crc = triple_crc16_compute((const uint8_t *)data, MAX_SIZE_TRI_TRUPLE - TRIPLE_CRC_LEN, &triple_crc_magic);

    p_data = data + MAX_SIZE_TRI_TRUPLE - TRIPLE_CRC_LEN;

    triple_read_crc = p_data[0] | (p_data[1] << 8);
    if (triple_crc != triple_read_crc) {
        LOGE(TAG, "read triple crc calc fail");
        return -EIO;
    }

    if (data[total_offset++] != TRIPLE_TYPE_DN) {
        LOGE(TAG, "read triple dn fail");
        return -EIO;
    } else {
        data_len = data[total_offset++];
        memcpy(d_n, data + total_offset, data_len);
        total_offset += data_len;
    }

    if (data[total_offset++] != TRIPLE_TYPE_PK) {
        LOGE(TAG, "read triple pk fail");
        return -EIO;
    } else {
        data_len = data[total_offset++];
        memcpy(p_k, data + total_offset, data_len);
        total_offset += data_len;
    }

    if (data[total_offset++] != TRIPLE_TYPE_PS) {
        LOGE(TAG, "read triple ps fail");
        return -EIO;
    } else {
        data_len = data[total_offset++];
        memcpy(p_s, data + total_offset, data_len);
        total_offset += data_len;
    }
    return 0;
}

int triple_read_by_flag(uint8_t flag, char *out, uint8_t *len)
{
    int  ret                        = 0;
    char dn[TRIPLE_MAX_DN_SIZE + 1] = { 0x0 };
    char pk[TRIPLE_MAX_PK_SIZE + 1] = { 0x0 };
    char ps[TRIPLE_MAX_PS_SIZE + 1] = { 0x0 };
    ret                             = triple_read(dn, pk, ps);
    if (ret) {
        LOGE(TAG, "triple read failed");
        return -1;
    }
    uint8_t total_len = 0;

    if (flag & TRIPLE_TYPE_DN) {
        out[total_len++] = TRIPLE_TYPE_DN;
        out[total_len++] = strlen(dn);
        memcpy(out + total_len, dn, strlen(dn));
        total_len += strlen(dn);
    }

    if (flag & TRIPLE_TYPE_PK) {
        out[total_len++] = TRIPLE_TYPE_PK;
        out[total_len++] = strlen(pk);
        memcpy(out + total_len, pk, strlen(pk));
        total_len += strlen(pk);
    }

    if (flag & TRIPLE_TYPE_PS) {
        out[total_len++] = TRIPLE_TYPE_PS;
        out[total_len++] = strlen(ps);
        memcpy(out + total_len, ps, strlen(ps));
        total_len += strlen(ps);
    }

    *len = total_len;
    return 0;
}

void triples_get_process(model_message *message)
{
    int ret = 0;

    uint8_t status          = 0;
    char    triples_out[50] = { 0x0 };
    uint8_t len             = 0;
    char *  data            = (char *)message->ven_data.user_data;
    if (!memcmp(data, TRIGET_FLAG, strlen(TRIGET_FLAG))) {
        char    flag     = (char)data[strlen(TRIGET_FLAG)];
        uint8_t flag_hex = flag - '0';
        ret              = triple_read_by_flag(flag_hex, triples_out, &len);
        if (ret) {
            status = GET_FAILED;
        } else {
            status = GET_SUCCESS;
        }
    } else {
        return;
    }

    struct bt_mesh_model *vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_SRV, CONFIG_CID_TAOBAO);
    if (!vendor_model) {
        LOGE(TAG, "get vendor model failed");
        return;
    }

    vnd_model_msg vnd_data     = { 0 };
    uint8_t       total_len    = 0;
    uint8_t       ven_data[50] = { 0 };
    memcpy(ven_data, TRIGET_FLAG, strlen(TRIGET_FLAG));
    total_len             = strlen(TRIGET_FLAG);
    ven_data[total_len++] = status;
    memcpy(ven_data + total_len, triples_out, len);
    total_len += len;
    vnd_data.netkey_idx = 0;
    vnd_data.appkey_idx = 0;
    vnd_data.dst_addr   = message->source_addr;
    vnd_data.model      = vendor_model;
    vnd_data.opid       = VENDOR_OP_ATTR_TRANS_MSG;
    vnd_data.data       = ven_data;
    vnd_data.retry      = 0;
    vnd_data.len        = total_len;
    vnd_data.trans      = message->trans;
    ble_mesh_vendor_srv_model_msg_send(&vnd_data);
}

static int _set_triple(char *pwbuf, int blen, int argc, char **argv)
{
    triple_t triple_set = { 0x0 };

    if (argc != 5) {
        LOGE(TAG, "para err");
        return -1;
    }

    /* set dn */
    if (strlen(argv[2]) > TRIPLE_MAX_DN_SIZE) {
        LOGE(TAG, "dn len err");
        return -1;
    }

    memcpy(triple_set.dn, argv[2], strlen(argv[2]));

    /* set pk */
    if (strlen(argv[3]) > TRIPLE_MAX_PK_SIZE) {
        LOGE(TAG, "pk len err");
        return -1;
    }

    memcpy(triple_set.pk, argv[3], strlen(argv[3]));

    /* set ps */
    if (strlen(argv[4]) > TRIPLE_MAX_PS_SIZE) {
        LOGE(TAG, "PS len err");
        return -1;
    }

    memcpy(triple_set.ps, argv[4], strlen(argv[4]));

    return triple_write(triple_set.dn, triple_set.pk, triple_set.ps);
}

static void _get_triple(char *pwbuf, int blen, int argc, char **argv)
{
    int      ret    = 0;
    triple_t triple = { 0x0 };

    memset(&triple, 0, sizeof(triple_t));

    ret = triple_read(triple.dn, triple.pk, triple.ps);
    if (ret != 0) {
        LOGE(TAG, "read triple fail(%d)", ret);
        return;
    }

    LOGD("", "dn:%s", triple.dn);
    LOGD("", "pk:%s", triple.pk);
    LOGD("", "ps:%s", triple.ps);
}

static void cmd_set_get_triple(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc < 2) {
        LOGE(TAG, "err input");
        return;
    }

    if (0 == strcmp(argv[1], "set")) {
        _set_triple(pwbuf, blen, argc, argv);
    }
    _get_triple(pwbuf, blen, argc, argv);
}

void cli_reg_cmd_triples(void)
{
    static const struct cli_command cmd_info = { "triple", "triple tool", cmd_set_get_triple };

    aos_cli_register_command(&cmd_info);
}
