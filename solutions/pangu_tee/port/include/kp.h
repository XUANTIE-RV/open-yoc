/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef KP_H
#define KP_H

#define TRUST_BOOT_PUBKEY_N_SIZE    128
#define TRUST_BOOT_PUBKEY_E_SIZE    128

typedef enum {
    KEY_MANIFEST = 1,
    KEY_JTAGKEY,
    KEY_PUBKEY,
    KEY_CID,
    KEY_CIDPRIVKEY,
    KEY_SOCCONFIG,
    KEY_LPM,
    KEY_PUBKEY_MODULS,
    KEY_MTB_ADDR = 12,
    KEY_MTB_BACKUP_ADDR,
    KEY_INVAILD
} key_type_e;

#define KP_OK             0
#define KP_ERR_MAGIC_NUM -1
#define KP_ERR_NOT_FOUND -2
#define KP_ERR_VERSION   -3
#define KP_ERR_NULL      -4
#define KP_ERR_POS_TYPE  -5
#define KP_ERR_KEY_SIZE  -6
#define KP_ERR_KEY_TYPE  -7

/**
  \brief       TEE get key from kp
  \param[in]   key_type  key type,see \ref key_type_e
  \param[out]  key     Pointer to key
  \return      return key size if > 0, else error code
*/
int kp_get_key(key_type_e key_type, uint32_t *key);


int kp_init(void);

#endif
