/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_AUTH_H
#define BZ_AUTH_H

#include <stdint.h>
#include "common.h"
#include "breeze_hal_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RANDOM_SEQ_LEN       16
#define MAX_OKM_LEN          16
#define MAX_IKM_LEN          (BZ_DEV_DEVICE_SECRET_LEN + BZ_BT_MAC_LEN + RANDOM_SEQ_LEN + 2)

enum {
    BZ_AUTH_STATE_IDLE,                                // Auth idle state
    BZ_AUTH_STATE_CONNECTED,                           // BLE link connected
    BZ_AUTH_STATE_SVC_ENABLED,                         // AIS service enabled
    BZ_AUTH_STATE_RAND_SENT,                           // Auth Random send out
    BZ_AUTH_STATE_REQ_RECVD,                           // Auth rx hisrv from peer
    BZ_AUTH_STATE_DONE,                                // Auth rx ok from peer
    BZ_AUTH_STATE_FAILED,                              // Auth failed
};

enum {
    BZ_AUTH_TYPE_NONE,
    BZ_AUTH_TYPE_PER_PK,
    BZ_AUTH_TYPE_PER_DEV,
};

typedef struct auth_s {
    uint8_t state;                                     // Auth state
    tx_func_t tx_func;                                 // Auth data send, use indication
    os_timer_t timer;                                  // Auth timeout timer, start from send random
    uint8_t ikm[MAX_IKM_LEN];                          // Auth sign calc input buffer
    uint16_t ikm_len;                                  // Auth sign calc input buffer length
    uint8_t okm[MAX_OKM_LEN];                          // Auth key output
    uint8_t *p_product_key;
    uint8_t product_key_len;
    uint8_t *p_device_name;
    uint8_t device_name_len;
    uint8_t device_secret[BZ_DEV_DEVICE_SECRET_LEN];
    uint8_t device_secret_len;
    bool dyn_update_device_secret;
    bool offline_auth;
    uint8_t auth_type;
    uint8_t check_hi_server_cnt; // Check "Hi,Server" message.
} auth_t;


#ifdef EN_AUTH_OFFLINE
#define MAX_AUTH_KEYS   5
#define AUTH_ID_LEN     8
#define AUTH_KEY_LEN    16
struct auth_key_pair{
    uint8_t auth_id[AUTH_ID_LEN];
    uint8_t auth_key[AUTH_KEY_LEN];
};
typedef struct auth_key_storage_s{
    uint8_t index_to_update;
    struct auth_key_pair kv_pairs[MAX_AUTH_KEYS];
} auth_key_storage_t;
#endif

ret_code_t auth_init(ali_init_t const *p_init, tx_func_t tx_func);
void auth_reset(void);
void auth_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length);
void auth_connected(void);
void auth_service_enabled(void);
void auth_tx_done(void);
bool auth_is_authdone(void);

ret_code_t auth_get_device_name(uint8_t **pp_device_name, uint8_t *p_length);
ret_code_t auth_get_product_key(uint8_t **pp_prod_key, uint8_t *p_length);
ret_code_t auth_get_device_secret(uint8_t *p_secret, int *p_length);
int auth_calc_adv_sign(uint32_t seq, uint8_t *sign);
#ifdef CONFIG_SEC_PER_PK_TO_DN
bool get_auth_update_status(void);
ret_code_t auth_secret_update_post_process(uint8_t *p_ds, uint16_t length);
#endif

#ifdef EN_AUTH_OFFLINE
void auth_keys_init(void);
bool authkey_set(uint8_t* authid, uint8_t* authkey);
bool authkey_get(uint8_t* authid, uint8_t* authkey);
#endif

#ifdef __cplusplus
}
#endif

#endif // BZ_AUTH_H
