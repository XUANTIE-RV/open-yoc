/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_hmac.h
 * @brief    Header File for HMAC engine
 * @version  V1.0
 * @date     25. May 2023
 * @model    hmac
 ******************************************************************************/
#ifndef _SE_HMAC_H_
#define _SE_HMAC_H_


#include <stdint.h>
#include "se_device.h"
#include "se_sha.h"


#ifdef __cplusplus
extern "C" {
#endif

/************/

typedef struct{
    aos_mutex_t mutex;
    void        *ctx;
} se_hmac_t;

/**
  \brief       Initialize MAC Interface. Initializes the resources needed for the MAC interface
  \param[in]   se_hmac  operate handle.
  \return      0 success, Non-zero failure
*/
int se_hmac_init(se_hmac_t *se_hmac);

/**
  \brief       De-initialize MAC Interface. stops operation and releases the software resources used by the interface
  \param[in]   se_hmac  mac handle to operate.
  \return      none
*/
void se_hmac_uninit(se_hmac_t *se_hmac);

/**
  \brief       MAC      set key function.
  \param[in]   se_hmac  mac handle to operate.
  \param[in]   key      Pointer to the mac key.
  \param[in]   key_len  Length of key.
  \return      \0 success, Non-zero failure
*/
int se_hmac_set_key(se_hmac_t *se_hmac, uint8_t *key, uint32_t key_len);

/**
  \brief       MAC operation function.
  \param[in]   se_hmac  mac handle to operate.
  \param[in]   mode     se_sha_mode_t.
  \param[in]   msg      Pointer to the mac input message.
  \param[in]   msg_len  Length of msg.
  \param[out]  out      mac buffer, malloc by caller.
  \param[out]  out_len  out mac length, should 32 bytes if HMAC_SHA256 mode.
  \return      0 success, Non-zero failure
*/
int se_hmac_calc(se_hmac_t *se_hmac, se_sha_mode_t mode, uint8_t *msg,
             uint32_t msg_len, uint8_t *out, uint32_t *out_len);


/************/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_HMAC_H_ */
