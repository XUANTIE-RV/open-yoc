/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "core.h"
#include "transport.h"
#include "auth.h"
#include "extcmd.h"
#include "common.h"
#include "ble_service.h"
#include "breeze_hal_ble.h"
#include "bzopt.h"

#include "bz_utils.h"
#ifdef CONFIG_AIS_SECURE_ADV
#include "sha256.h"
#endif

core_t g_core;
extern auth_t g_auth;

#ifdef CONFIG_AIS_SECURE_ADV
#define AIS_SEQ_KV_KEY      "ais_adv_seq"
#define AIS_SEQ_UPDATE_FREQ (1 * 60 * 60) /* in second uint */
static uint32_t g_seq = 0;
static os_timer_t g_secadv_timer;
#endif

//static ali_init_t const *g_ali_init;

void core_event_notify(uint8_t event_type, uint8_t *data, uint16_t length)
{
    ali_event_t event;

    event.type = event_type;
    event.rx_data.p_data = data;
    event.rx_data.length = length;
    g_core.event_handler(&event);
}

static uint32_t tx_func_indicate(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    return transport_tx(TX_INDICATION, cmd, p_data, length);
}

static uint32_t ais_init(ali_init_t const *p_init)
{
    ble_ais_init_t init_ais;

    memset(&init_ais, 0, sizeof(ble_ais_init_t));
    init_ais.mtu = p_init->max_mtu;
    return ble_ais_init(&init_ais);
}

#ifdef CONFIG_AIS_SECURE_ADV
static void update_seq(void *arg1, void *arg2)
{
    os_kv_set(AIS_SEQ_KV_KEY, &g_seq, sizeof(g_seq), 1);
    os_timer_start(&g_secadv_timer);
}

static void init_seq_number(uint32_t *seq)
{
    int len = sizeof(uint32_t);

    if (!seq)
        return;

    if (os_kv_get(AIS_SEQ_KV_KEY, seq, &len) != 0) {
        *seq = 0;
        len  = sizeof(uint32_t);
        os_kv_set(AIS_SEQ_KV_KEY, seq, len, 1);
    }

    os_timer_new(&g_secadv_timer, update_seq, NULL, AIS_SEQ_UPDATE_FREQ);
    os_timer_start(&g_secadv_timer);
}

void set_adv_sequence(uint32_t seq)
{
    g_seq = seq;
    os_kv_set(AIS_SEQ_KV_KEY, &g_seq, sizeof(g_seq), 1);
}
#endif

ret_code_t core_init(ali_init_t const *p_init)
{
    // breeze core base infomation init
    memset(&g_core, 0, sizeof(core_t));
    g_core.event_handler = p_init->event_handler;
    memcpy(g_core.adv_mac, p_init->adv_mac, BZ_BT_MAC_LEN);
    g_core.product_id = p_init->model_id;
    // core device info init
    if((p_init->product_key.p_data != NULL) && (p_init->product_key.length > 0)) {
        g_core.product_key_len = p_init->product_key.length;
        memcpy(g_core.product_key, p_init->product_key.p_data, g_core.product_key_len);
    }
    if((p_init->product_secret.p_data != NULL) && (p_init->product_secret.length > 0)) {
        g_core.product_secret_len = p_init->product_secret.length;
        memcpy(g_core.product_secret, p_init->product_secret.p_data, g_core.product_secret_len);
    }
    if((p_init->device_name.p_data != NULL) && (p_init->device_name.length > 0)) {
        g_core.device_name_len = p_init->device_name.length;
        memcpy(g_core.device_name, p_init->device_name.p_data, g_core.device_name_len);
    }
    if((p_init->device_secret.p_data != NULL) && (p_init->device_secret.length > 0)) {
        g_core.device_secret_len = p_init->device_secret.length;
        memcpy(g_core.device_secret, p_init->device_secret.p_data, g_core.device_secret_len);
    }

#ifdef CONFIG_AIS_SECURE_ADV
    init_seq_number(&g_seq);
#endif

    ais_init(p_init);
    bz_transport_init(p_init);

#if BZ_ENABLE_AUTH
    auth_init(p_init, tx_func_indicate);
#endif

    extcmd_init(p_init, tx_func_indicate);

    return BZ_SUCCESS;
}


void core_reset(void)
{
#if BZ_ENABLE_AUTH
    auth_reset();
#endif
    transport_reset();
    g_core.admin_checkin = 0;
    g_core.guest_checkin = 0;
}

void core_handle_err(uint8_t src, uint8_t code)
{
    uint8_t err;

    BREEZE_ERR("err at 0x%04x, code 0x%04x", src, code);
    switch (src & BZ_ERR_MASK) {
        case BZ_TRANS_ERR:
            if (code != BZ_EINTERNAL) {
                if (src == ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC) {
                    core_event_notify(BZ_EVENT_ERR_DISCONT, NULL, 0);
                }
                err = transport_tx(TX_NOTIFICATION, BZ_CMD_ERR, NULL, 0);
                if (err != BZ_SUCCESS) {
                    BREEZE_ERR("err at 0x%04x, code 0x%04x", ALI_ERROR_SRC_TRANSPORT_SEND, code);
                }
            }
            break;
#if BZ_ENABLE_AUTH
        case BZ_AUTH_ERR:
            BREEZE_ERR("BZ_AUTH_ERR");
            auth_reset();
            if (code == BZ_ETIMEOUT) {
                ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
            }
            break;
#endif
        case BZ_EXTCMD_ERR:
            BREEZE_ERR("BZ_EXTCMD_ERR");
            break;
        default:
            BREEZE_ERR("unknow bz err\r\n");
            break;
    }
}

void core_create_bz_adv_data(uint8_t sub_type, uint8_t sec_type, uint8_t bind_state)
{
    uint16_t idx;
    uint8_t version = 0;
    uint8_t fmsk = 0;
    char* p;
    char* ver_str = NULL;

    SET_U16_LE(g_core.adv_data, BZ_ALI_COMPANY_ID);
    idx = sizeof(uint16_t);

    // extract Breeze version from BZ_VERSION
    char t_ver_info[20] = { 0 };
    strncpy(t_ver_info, BZ_VERSION, sizeof(t_ver_info) - 1);
    p = strtok(t_ver_info, ".");
    while((p = strtok(NULL, ".")) != NULL){
        ver_str = p;
    }
    version = (uint8_t)atoi(ver_str);
    g_core.adv_data[idx++] = (version<<BZ_SDK_VER_Pos) | (sub_type<<BZ_SUB_TYPE_Pos);

    // FMSK byte
    fmsk = BZ_BLUETOOTH_VER << BZ_FMSK_BLUETOOTH_VER_Pos;
#if BZ_ENABLE_OTA
    fmsk |= 1 << BZ_FMSK_OTA_Pos;
#endif
#if BZ_ENABLE_AUTH
    fmsk |= 1 << BZ_FMSK_SECURITY_Pos;
    if(sec_type == BZ_SEC_TYPE_DEVICE) {
        BREEZE_DEBUG("Breeze adv per device");
        fmsk |= 1 << BZ_FMSK_SECRET_TYPE_Pos;
        g_auth.auth_type = BZ_AUTH_TYPE_PER_DEV;
    } else if (sec_type == BZ_SEC_TYPE_PRODUCT) {
        BREEZE_DEBUG("Breeze adv per product");
        fmsk &= ~(1 << BZ_FMSK_SECRET_TYPE_Pos);
        g_auth.auth_type = BZ_AUTH_TYPE_PER_PK;
    } else {
        BREEZE_ERR("Breeze adv sec type err");
        g_auth.auth_type = BZ_AUTH_TYPE_NONE;
    }
#endif
#ifdef CONFIG_AIS_SECURE_ADV
    fmsk |= 1 << BZ_FMSK_SEC_ADV_Pos;
#endif
    if(bind_state && (version >= 6)){
        BREEZE_DEBUG("Breeze binded");
        fmsk |= 1 << BZ_FMSK_BIND_STATE_Pos;
    } else{
        BREEZE_DEBUG("Breeze unbind");
        fmsk &= ~(1 << BZ_FMSK_BIND_STATE_Pos);
    }   

    g_core.adv_data[idx++] = fmsk;

    SET_U32_LE(g_core.adv_data + idx, g_core.product_id);
    idx += sizeof(uint32_t);

    if (g_core.adv_mac[0]==0 && g_core.adv_mac[1]==0 && g_core.adv_mac[2]==0
        && g_core.adv_mac[3]==0 && g_core.adv_mac[4]==0 && g_core.adv_mac[5]==0) {
        ble_get_mac(g_core.adv_mac);
    }
    memcpy(&g_core.adv_data[idx], g_core.adv_mac, 6);
    idx += 6;
    g_core.adv_data_len = idx;
}


ret_code_t core_get_bz_adv_data(uint8_t *p_data, uint16_t *length)
{
#ifdef CONFIG_AIS_SECURE_ADV
    if (*length < (g_core.adv_data_len + 4 + 4)) {
#else
    if (*length < g_core.adv_data_len) {
#endif
        return BZ_ENOMEM;
    }

#ifdef CONFIG_AIS_SECURE_ADV
    uint8_t  sign[4];
    uint32_t seq;

    seq = (++g_seq);
#if BZ_ENABLE_AUTH
    auth_calc_adv_sign(seq, sign);
#endif
    memcpy(p_data, g_core.adv_data, g_core.adv_data_len);
    memcpy(p_data + g_core.adv_data_len, sign, 4);
    memcpy(p_data + g_core.adv_data_len + 4, &seq, 4);
    *length = g_core.adv_data_len + 4 + 4;
#else
    memcpy(p_data, g_core.adv_data, g_core.adv_data_len);
    *length = g_core.adv_data_len;
#endif

    return BZ_SUCCESS;
}

