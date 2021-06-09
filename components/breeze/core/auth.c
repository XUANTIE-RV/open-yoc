/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bz_sha256.h"

#include "auth.h"
#include "core.h"
#include "bz_utils.h"
#include "breeze_hal_ble.h"

extern core_t g_core;
auth_t g_auth;

static uint8_t g_ds[BZ_DEV_DEVICE_SECRET_LEN] = { 0 };
static uint8_t g_ds_len = 0;
static uint8_t g_ps[BZ_DEV_PRODUCT_SECRET_LEN]  = { 0 };
static uint8_t g_ps_len = 0;
bool g_dn_complete = false;
char bz_hi_client_str[]="Hi,Client";

void store_auth_key(void);

static void on_timeout(void *arg1, void *arg2)
{
    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_2, BZ_ETIMEOUT);
}

static void ikm_init(ali_init_t const *p_init)
{
    if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == BZ_DEV_DEVICE_SECRET_LEN){
        g_ds_len = g_auth.device_secret_len;
        memcpy(g_ds, g_auth.device_secret, g_ds_len);
    }

    g_ps_len = p_init->product_secret.length;
    memcpy(g_ps, p_init->product_secret.p_data, g_ps_len);

    memcpy(g_auth.ikm + g_auth.ikm_len, p_init->product_secret.p_data, p_init->product_secret.length);
    g_auth.ikm_len += p_init->product_secret.length;

    g_auth.ikm[g_auth.ikm_len++] = ',';
}

static void update_aes_key(bool use_device_key)
{
    uint8_t rand_backup[RANDOM_SEQ_LEN];
    SHA256_CTX context;
    uint8_t okm[SHA256_BLOCK_SIZE];

    memcpy(rand_backup, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    g_auth.ikm_len = 0;

    if (use_device_key) {
        memcpy(g_auth.ikm + g_auth.ikm_len, g_ds, g_ds_len);
        g_auth.ikm_len += g_ds_len;
    } else {
        memcpy(g_auth.ikm + g_auth.ikm_len, g_ps, g_ps_len);
        g_auth.ikm_len += g_ps_len;
    }

    g_auth.ikm[g_auth.ikm_len++] = ',';
    memcpy(g_auth.ikm + g_auth.ikm_len, rand_backup, sizeof(rand_backup));

    sha256_init(&context);
    sha256_update(&context, g_auth.ikm, g_auth.ikm_len + RANDOM_SEQ_LEN);
    sha256_final(&context, okm);
    memcpy(g_auth.okm, okm, MAX_OKM_LEN);

    // notify key updated
    transport_update_key(g_auth.okm);
}

ret_code_t auth_init(ali_init_t const *p_init, tx_func_t tx_func)
{
    int len;
    char tmp_ds[BZ_DEV_DEVICE_SECRET_LEN];
    ret_code_t ret = BZ_SUCCESS;

    len = sizeof(tmp_ds);
    memset(tmp_ds, 0, len);
    memset(&g_auth, 0, sizeof(auth_t));
    g_auth.state = BZ_AUTH_STATE_IDLE;
    g_auth.tx_func = tx_func;
    g_auth.dyn_update_device_secret = false;
#ifdef EN_AUTH_OFFLINE
    g_auth.offline_auth = false;
#endif

    if ((p_init->product_key.p_data != NULL) && (p_init->product_key.length > 0)) {
        g_auth.p_product_key = g_core.product_key;
        g_auth.product_key_len = g_core.product_key_len;
    }
    if ((p_init->device_name.p_data != NULL) && (p_init->device_name.length > 0)) {
        g_auth.p_device_name = g_core.device_name;
        g_auth.device_name_len = g_core.device_name_len;
    }

    /*
     * Secret have 2 resources: internal KV storge and external initialization. Consider below conditions:
     * 1.secret per device, no DS in KV: need 1. update DS from external initialization.
     * 2.secret per device, have DS in KV:if 1.DS is the same choose either one. 2.DS is not the same, report err or choose one.
     * 3.secret per product, no DS in KV, : Doing nothing.
     * 4.secret per producet, have DS in KV:need 1. update dynamic secret flag, update from interval KV storage.
     */
    g_auth.device_secret_len = p_init->device_secret.length;
    memset(tmp_ds, 0, sizeof(tmp_ds));
    if(g_auth.device_secret_len == BZ_DEV_DEVICE_SECRET_LEN){
        if(auth_get_device_secret((uint8_t *)&tmp_ds, &len) != 0 ){
            // Read ds from kv fail, but ds set by upper layer
            memcpy(g_auth.device_secret, p_init->device_secret.p_data, BZ_DEV_DEVICE_SECRET_LEN);
        } else if(memcmp(tmp_ds, p_init->device_secret.p_data, g_auth.device_secret_len) == 0){
            // Read ds from kv success, and ds in auth memory is the same
            memcpy(g_auth.device_secret, p_init->device_secret.p_data, BZ_DEV_DEVICE_SECRET_LEN);
        } else{
            BREEZE_ERR("DS from KV not match user's input %s", __func__);
            return BZ_EINVALIDPARAM;
        }
    } else if(g_auth.device_secret_len == 0){
        if(auth_get_device_secret((uint8_t *)&tmp_ds, &len) == 0){ //case 4.
            g_auth.device_secret_len = len;
            memcpy(g_auth.device_secret, tmp_ds, len);
            g_auth.dyn_update_device_secret = true;
        } //case 3
    } else{
        BREEZE_ERR("Auth type not per product or per device %s", __func__);
        return BZ_EINVALIDPARAM;//err case when DS length is not BZ_DEV_DEVICE_SECRET_LEN or 0
    }

    ikm_init(p_init);
#ifdef EN_AUTH_OFFLINE
    auth_keys_init();
#endif
    ret = os_timer_new(&g_auth.timer, on_timeout, &g_auth, BZ_AUTH_TIMEOUT);
    return ret;
}

void auth_reset(void)
{
    g_auth.state = BZ_AUTH_STATE_IDLE;
    os_timer_stop(&g_auth.timer);
}

void auth_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    uint32_t err_code;
#ifdef EN_AUTH_OFFLINE
    uint8_t rekey_rsp[] = {0x01, 0x02};
    uint8_t authkey[AUTH_KEY_LEN]={0};
    uint8_t authid[AUTH_ID_LEN] = {0};
#endif

    if (length == 0 || (cmd & BZ_CMD_TYPE_MASK) != BZ_CMD_AUTH) {
        return;
    }

    switch (g_auth.state) {
        case BZ_AUTH_STATE_RAND_SENT:
#ifdef EN_AUTH_OFFLINE
            if(cmd == BZ_CMD_AUTH_REKEY){/*check whether auth-key according to auth-id*/
                 if(length != AUTH_ID_LEN) {
                      err_code = g_auth.tx_func(BZ_CMD_AUTH_REKEY_RSP, &rekey_rsp[1] , 1);
                      BREEZE_ERR("[BZ auth]: rx offline key len mismatch(%d)", err_code);
                      return;
                }
                memcpy(authid, p_data, AUTH_ID_LEN);
                if(authkey_get(authid, authkey) == true){
                    //("[BZ auth]: rx offline key\n");
                    g_dn_complete = true;
                    g_auth.state = BZ_AUTH_STATE_REQ_RECVD;
                    g_auth.offline_auth = true;
                    
                    /*1.update current auth key*/
                    transport_update_key(authkey);
                    /*2.response with "HI Client"*/
                    err_code = g_auth.tx_func(BZ_CMD_AUTH_RSP, (uint8_t *)bz_hi_client_str, strlen(bz_hi_client_str));
                    BREEZE_DEBUG("[BZ auth]: tx (%s) (%s)", bz_hi_client_str, err_code ? "fail": "success");
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                        return;
                    }
                    err_code = os_timer_start(&g_auth.timer);
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                        return;
                    }
                 } else{
                    err_code = g_auth.tx_func(BZ_CMD_AUTH_REKEY_RSP, &rekey_rsp[0] , 1);
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                        return;
                    }
                    /*back to re-auth state*/
                    os_timer_stop(&g_auth.timer);
                    auth_connected();
                 }
            } else
#endif
            if (cmd == BZ_CMD_AUTH_REQ &&
                memcmp(BZ_HI_SERVER_STR, p_data, MIN(length, strlen(BZ_HI_SERVER_STR))) == 0) {
                BREEZE_DEBUG("[BZ auth]: rx (%s)", BZ_HI_SERVER_STR);
                g_dn_complete = true;
                g_auth.state = BZ_AUTH_STATE_REQ_RECVD;
                err_code = g_auth.tx_func(BZ_CMD_AUTH_RSP, (uint8_t *)bz_hi_client_str, strlen(bz_hi_client_str));
                BREEZE_DEBUG("[BZ auth]: tx (%s) (%s)", bz_hi_client_str, err_code ? "fail": "success");
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                err_code = os_timer_start(&g_auth.timer);
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                    return;
                }
            } else if (g_auth.check_hi_server_cnt == 0) {
                // Will check "Hi,Server" message again
                g_auth.check_hi_server_cnt = 1;
                BREEZE_WARN("hi server message check failed");
            } else {
                g_auth.state = BZ_AUTH_STATE_FAILED;
            }
            break;

        case BZ_AUTH_STATE_REQ_RECVD:
            if (cmd == BZ_CMD_AUTH_CFM && memcmp(BZ_OK_STR, p_data, MIN(length, strlen(BZ_OK_STR))) == 0) {
                BREEZE_DEBUG("[BZ auth]: rx (%s)", BZ_OK_STR);
                g_auth.state = BZ_AUTH_STATE_DONE;
            } else {
                g_auth.state = BZ_AUTH_STATE_FAILED;
            }
            break;

        default:
            err_code = g_auth.tx_func(BZ_CMD_ERR, NULL, 0);
            if (err_code != BZ_SUCCESS) {
                core_handle_err(ALI_ERROR_SRC_AUTH_SEND_ERROR, err_code);
                return;
            }
            break;
    }

    if (g_auth.state == BZ_AUTH_STATE_DONE) {
#ifdef EN_AUTH_OFFLINE
        if(g_auth.offline_auth == false){
            store_auth_key();
        } else{
            g_auth.offline_auth = false;
        }
#endif
        core_event_notify(BZ_EVENT_AUTHENTICATED, NULL, 0);
        os_timer_stop(&g_auth.timer);
    } else if (g_auth.state == BZ_AUTH_STATE_FAILED) {
        os_timer_stop(&g_auth.timer);
        ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
    }
}
void auth_connected(void)
{
    uint32_t err_code;

    err_code = os_timer_start(&g_auth.timer);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_0, err_code);
        return;
    }

    g_auth.state = BZ_AUTH_STATE_CONNECTED;
    get_random(g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    update_aes_key(false);
}

void auth_service_enabled(void)
{
    uint32_t err_code;

    g_auth.state = BZ_AUTH_STATE_SVC_ENABLED;
    err_code = g_auth.tx_func(BZ_CMD_AUTH_RAND, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    BREEZE_DEBUG("[BZ auth]: tx rand(%s)", err_code ? "fail": "success");
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }
}

void auth_tx_done(void)
{
    uint32_t err_code;
    
    if(g_auth.state == BZ_AUTH_STATE_CONNECTED){
        auth_service_enabled();
    } else if (g_auth.state == BZ_AUTH_STATE_SVC_ENABLED) {
        g_auth.state = BZ_AUTH_STATE_RAND_SENT;

        if((g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == BZ_DEV_DEVICE_SECRET_LEN)
            && (g_auth.auth_type == BZ_AUTH_TYPE_PER_DEV)){
            err_code = g_auth.tx_func(BZ_CMD_AUTH_KEY, g_auth.p_device_name, g_auth.device_name_len);
            BREEZE_DEBUG("[BZ auth]: tx DN (%s)", err_code ? "fail": "success");
            if (err_code != BZ_SUCCESS) {
                core_handle_err(ALI_ERROR_SRC_AUTH_SEND_KEY, err_code);
            }
            return;
        }
    } else if (g_auth.state == BZ_AUTH_STATE_RAND_SENT) {
        if((g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == BZ_DEV_DEVICE_SECRET_LEN)
            && (g_auth.auth_type == BZ_AUTH_TYPE_PER_DEV)){
            BREEZE_DEBUG("[BZ auth]: update AES-auth-key");
            update_aes_key(true);
        } else{
            return;
        }
    }
}

bool auth_is_authdone(void)
{
    return (bool)(g_auth.state == BZ_AUTH_STATE_DONE);
}

ret_code_t auth_get_device_name(uint8_t **pp_device_name, uint8_t *p_length)
{
    if(g_auth.device_name_len){
        *pp_device_name = g_auth.p_device_name;
        *p_length = g_auth.device_name_len;
        return BZ_SUCCESS;
    } else{
        return BZ_EDATASIZE;
    }
}

ret_code_t auth_get_product_key(uint8_t **pp_prod_key, uint8_t *p_length)
{
    *pp_prod_key = g_auth.p_product_key;
    *p_length = BZ_DEV_PRODUCT_KEY_LEN;
    return BZ_SUCCESS;
}

ret_code_t auth_get_device_secret(uint8_t *p_secret, int *p_length)
{
    if(p_secret == NULL || p_length == NULL){
        return BZ_EINVALIDPARAM;
    }
    uint8_t secret[2 * BZ_DEV_DEVICE_SECRET_LEN + 1];
    //int len = HAL_GetDeviceSecret(secret);
    extern int HAL_GetDeviceSecret(char *device_secret);
    HAL_GetDeviceSecret((char *)secret);
    memcpy(p_secret, secret, BZ_DEV_DEVICE_SECRET_LEN);
    return *p_length != BZ_DEV_DEVICE_SECRET_LEN;
    // return os_kv_get(BZ_DEVICE_SECRET_STR, p_secret, p_length);
}

#ifdef CONFIG_AIS_SECURE_ADV
static void make_seq_le(uint32_t *seq)
{
    uint32_t test_num = 0x01020304;
    uint8_t *byte = (uint8_t *)(&test_num);

    if (*byte == 0x04)
        return;
    uint32_t tmp = *seq;
    SET_U32_LE(seq, tmp);
}

int auth_calc_adv_sign(uint32_t seq, uint8_t *sign)
{
    SHA256_CTX context;
    uint8_t full_sign[32], i, *p;

    make_seq_le(&seq);
    sha256_init(&context);

    sha256_update(&context, BZ_DEVICE_NAME_STR, strlen(BZ_DEVICE_NAME_STR));
    sha256_update(&context, g_auth.p_device_name, g_auth.device_name_len);

    if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == BZ_DEV_DEVICE_SECRET_LEN){
        sha256_update(&context, BZ_DEVICE_SECRET_STR, strlen(BZ_DEVICE_SECRET_STR));
        sha256_update(&context, g_auth.device_secret, BZ_DEV_DEVICE_SECRET_LEN);
    } else{
        sha256_update(&context, BZ_PRODUCT_SECRET_STR, strlen(BZ_PRODUCT_SECRET_STR));
        sha256_update(&context, g_ps, BZ_DEV_PRODUCT_SECRET_LEN);
    }

    sha256_update(&context, BZ_PRODUCT_KEY_STR, strlen(BZ_PRODUCT_KEY_STR));
    sha256_update(&context, g_auth.p_product_key, BZ_DEV_PRODUCT_KEY_LEN);

    sha256_update(&context, BZ_SEQUENCE_STR, strlen(BZ_SEQUENCE_STR));
    sha256_update(&context, &seq, sizeof(seq));

    sha256_final(&context, full_sign);

    memcpy(sign, full_sign, 4);

    return 0;
}
#endif

#ifdef CONFIG_SEC_PER_PK_TO_DN
bool get_auth_update_status(void)
{
    return g_auth.dyn_update_device_secret;
}

ret_code_t auth_secret_update_post_process(uint8_t* p_ds, uint16_t len)
{
    /*
     * 1.update secret bit in adv, this will do in restart adv logic, not here
     * 2.update IKM with product secret for secret per device
     * 3.update g_auth.dyn_update_device_secret flag in g_auth struct
     */
    g_auth.dyn_update_device_secret = true;
    g_ds_len = len;
    g_auth.device_secret_len = len;
    memcpy(g_ds, p_ds, len);
    memcpy(g_auth.device_secret, p_ds, len);

    BREEZE_DEBUG("Auth updated :per product ->device %d len(%d)",g_auth.dyn_update_device_secret, g_ds_len);
    return BZ_SUCCESS;
}

#endif


#ifdef EN_AUTH_OFFLINE
void auth_keys_init(void)
{
    auth_key_storage_t auth_keys;
    int len = sizeof(auth_keys);
    memset(&auth_keys, 0, len);
    if (os_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) != 0){
        if(os_kv_set(AUTH_KEY_KV_PREFIX, &auth_keys, len, 1) != 0){
            BREEZE_ERR("[BZ auth]: init keys");
            return;
        }
    }
}

bool authkey_set(uint8_t* authid, uint8_t* authkey)
{
    int32_t ret;
    auth_key_storage_t auth_keys;
    int len = sizeof(auth_keys);
    if(authid == NULL || authkey == NULL){
        return false;
    }
    memset(&auth_keys, 0, len);
    if (os_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) == 0){
        uint32_t index = auth_keys.index_to_update;
        memcpy(auth_keys.kv_pairs[index].auth_id, authid, AUTH_ID_LEN);
        memcpy(auth_keys.kv_pairs[index].auth_key, authkey, AUTH_KEY_LEN);
        auth_keys.index_to_update ++;
        if(index == MAX_AUTH_KEYS -1){
           auth_keys.index_to_update = 0;
        }
        ret = os_kv_set(AUTH_KEY_KV_PREFIX, &auth_keys, len, 1);
        BREEZE_ERR("[BZ auth]: keys KV set (%s)", ret ? "fail": "success");
        return true;
    }
    return false;
}

bool authkey_get(uint8_t* authid, uint8_t* authkey)
{
    //int32_t ret;
    uint32_t index = 0;
    auth_key_storage_t auth_keys;
    int len = sizeof(auth_keys);
    if(authid == NULL || authkey == NULL){
        return false;
    }
    if (os_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) == 0){
        for(; index < MAX_AUTH_KEYS; ++index){
             if(!memcmp(auth_keys.kv_pairs[index].auth_id, authid, AUTH_ID_LEN)){
                memcpy(authkey, auth_keys.kv_pairs[index].auth_key, AUTH_KEY_LEN);
                return true;
             }
        }
        if(index == MAX_AUTH_KEYS){
            return false;
        }
    }
    return false;
}

void store_auth_key(void)
{
    uint8_t rand_backup[RANDOM_SEQ_LEN];
    SHA256_CTX context;
    uint8_t auth_id[AUTH_ID_LEN];
    uint8_t okm[SHA256_BLOCK_SIZE];
    memcpy(rand_backup, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    sha256_init(&context);
    sha256_update(&context, rand_backup, sizeof(rand_backup));
    sha256_final(&context, okm);
    memcpy(auth_id, okm, AUTH_ID_LEN);
    authkey_set(auth_id, g_auth.okm);
}

#endif
