/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sec_crypto_mac.h
 * @brief    Header File for MAC
 * @version  V1.0
 * @date     26. May 2023
 * @model    mac
 ******************************************************************************/
#ifndef _SC_MAC_H_
#define _SC_MAC_H_
#include <stdint.h>
#include "sec_crypto_errcode.h"
#include "sec_crypto_sha.h"
#if defined(CONFIG_WITH_SE)
#include <se_hmac.h>
#endif

#define SC_MAC_KEY_LEN_MAX      (64)
#define HMAC_SHA1_BLOCK_SIZE    (64)
#define HMAC_SHA224_BLOCK_SIZE  (64)
#define HMAC_SM3_BLOCK_SIZE     (64)
#define HMAC_SHA256_BLOCK_SIZE  (64)
#define HMAC_MD5_BLOCK_SIZE     (64)
#define HMAC_SHA384_BLOCK_SIZE  (128)
#define HMAC_SHA512_BLOCK_SIZE  (128)
#define HMAC_MAX_BLOCK_SIZE     (128)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sc_mac {
	sc_sha_t sha;
	uint8_t key[HMAC_MAX_BLOCK_SIZE];
    sc_sha_mode_t mode;
#if defined(CONFIG_WITH_SE)
	se_hmac_t se_hmac;
#endif
} sc_mac_t;

#define MAC_CONTEXT_SIZE sizeof(sc_sha_context_t)
typedef struct {
	uint8_t ctx[MAC_CONTEXT_SIZE];
} sc_mac_context_t;

/**
  \brief       Initialize MAC Interface. Initializes the resources needed for the MAC interface
  \param[in]   mac  operate handle.
  \param[in]   idx  index of mac
  \return      error code \ref uint32_t
*/
uint32_t sc_mac_init(sc_mac_t *mac, uint32_t idx);

/**
  \brief       De-initialize MAC Interface. stops operation and releases the software resources used by the interface
  \param[in]   mac  mac handle to operate.
  \return      none
*/
void sc_mac_uninit(sc_mac_t *mac);

/**
  \brief       MAC      set key function.
  \param[in]   mac      mac handle to operate.
  \param[in]   key      Pointer to the mac key.
  \param[in]   key_len  Length of key.
  \return      error code
*/
uint32_t sc_mac_set_key(sc_mac_t *mac, uint8_t *key, uint32_t key_len);

/**
  \brief       MAC operation function.
  \param[in]   mac      mac handle to operate.
  \param[in]   mode     sc_sha_mode_t.
  \param[in]   msg      Pointer to the mac input message.
  \param[in]   msg_len  Length of msg.
  \param[out]  out      mac buffer, malloc by caller.
  \param[out]  out_len  out mac length, should 32 bytes if HMAC_SHA256 mode.
  \return      error code
*/
uint32_t sc_mac_calc(sc_mac_t *mac, sc_sha_mode_t mode, uint8_t *msg,
		     uint32_t msg_len, uint8_t *out, uint32_t *out_len);

/**
  \brief       MAC start operation function.
  \param[in]   mac 		  mac handle to operate.
  \param[in]   context 	mac context pointer.
  \param[in]   mode 	  sc_sha_mode_t.
  \return      error code
*/
uint32_t sc_mac_start(sc_mac_t *mac, sc_mac_context_t *context, sc_sha_mode_t mode);

/**
  \brief       MAC start operation function.
  \param[in]   mac      mac handle to operate.
  \param[in]   context  mac context pointer.
  \param[in]   msg      Pointer to the mac input message.
  \param[in]   msg_len  Length of msg.
  \return      error code
*/
uint32_t sc_mac_update(sc_mac_t *mac, sc_mac_context_t *context, uint8_t *msg, uint32_t msg_len);

/**
  \brief       MAC start operation function.
  \param[in]   mac 		mac handle to operate.
  \param[in]   context 	mac context.
  \param[out]  out 		mac buffer, malloc by caller.
  \param[out]  out_len  out mac length,
  \return      error code
*/
uint32_t sc_mac_finish(sc_mac_t *mac, sc_mac_context_t *context, uint8_t *out, uint32_t *out_len);
#ifdef __cplusplus
}
#endif

#endif /* _SC_MAC_H_ */
