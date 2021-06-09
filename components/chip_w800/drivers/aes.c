/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_aes.c
 * @brief    CSI Source File for aes driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <string.h>
#include "drv/aes.h"
#include "drv/irq.h"
#include "soc.h"
#include "wm_crypto_hard.h"

#define ERR_AES(errno) (CSI_DRV_ERRNO_AES_BASE | errno)
#define AES_NULL_PARA_CHK(para)  HANDLE_PARAM_CHK(para, ERR_AES(DRV_ERROR_PARAMETER))

typedef struct {
    aes_event_cb_t cb;
    aes_status_t status;

    CRYPTO_WAY dec;
    CRYPTO_MODE cbc;
    uint8_t key[16];
    uint8_t iv[16];
} ck_aes_priv_t;

static ck_aes_priv_t aes_handle[CONFIG_AES_NUM];

/* Driver Capabilities */
static const aes_capabilities_t driver_capabilities = {
    .ecb_mode = 1, /* ECB mode */
    .cbc_mode = 1, /* CBC mode */
    .ctr_mode = 1, /* CTR mode */
};

//
// Functions
//

/**
  \brief       Initialize AES Interface. 1. Initializes the resources needed for the AES interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref aes_event_cb_t
  \return      return aes handle if success
*/
aes_handle_t csi_aes_initialize(int32_t idx, aes_event_cb_t cb_event)
{
    uint8_t i;
    if (idx < 0 || idx >= CONFIG_AES_NUM) {
        return NULL;
    }

    ck_aes_priv_t *aes_priv = &aes_handle[idx];

    aes_priv->cb = cb_event;
    aes_priv->status.busy = 0;

    for (i = 0; i < 16; i++)
	{
		aes_priv->iv[i] = rand();
	}

    return (aes_handle_t)aes_priv;
}

/**
  \brief       De-initialize AES Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  aes handle to operate.
  \return      error code
*/
int32_t csi_aes_uninitialize(aes_handle_t handle)
{
    AES_NULL_PARA_CHK(handle);

    ck_aes_priv_t *aes_priv = handle;
    aes_priv->cb = NULL;

    return 0;
}

/**
  \brief       control aes power.
  \param[in]   handle  aes handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_aes_power_control(aes_handle_t handle, csi_power_stat_e state)
{
    AES_NULL_PARA_CHK(handle);
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id.
  \return      \ref aes_capabilities_t
*/
aes_capabilities_t csi_aes_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_AES_NUM) {
        aes_capabilities_t ret;
        memset(&ret, 0, sizeof(aes_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       config aes mode.
  \param[in]   handle  aes handle to operate.
  \param[in]   mode      \ref aes_mode_e
  \param[in]   keylen_bits \ref aes_key_len_bits_e
  \param[in]   endian    \ref aes_endian_mode_e
  \return      error code
*/
int32_t csi_aes_config(aes_handle_t handle, aes_mode_e mode, aes_key_len_bits_e keylen_bits, aes_endian_mode_e endian)
{
    AES_NULL_PARA_CHK(handle);

    ck_aes_priv_t *aes_priv = handle;
    CRYPTO_MODE cbc;

    /* config the aes mode */
    switch (mode) {
        case AES_MODE_CBC:
            cbc = CRYPTO_MODE_CBC;
            break;

        case AES_MODE_ECB:
            cbc = CRYPTO_MODE_ECB;
            break;

        case AES_MODE_CTR:
            cbc = CRYPTO_MODE_CTR;
            break;

        default:
            return ERR_AES(AES_ERROR_MODE);
    }

    /* config the key length */
    if (AES_KEY_LEN_BITS_128 != keylen_bits)
        return ERR_AES(AES_ERROR_DATA_BITS);

    /* config the endian mode */
    if (AES_ENDIAN_LITTLE != endian)
        return ERR_AES(AES_ERROR_ENDIAN);

    aes_priv->cbc = cbc;

    return 0;
}

/**
  \brief       set crypto key.
  \param[in]   handle    aes handle to operate.
  \param[in]   context   aes information context(NULL when hardware implementation)
  \param[in]   key       Pointer to the key buf
  \param[in]   key_len   Pointer to the aes_key_len_bits_e
  \param[in]   enc       \ref aes_crypto_mode_e
  \return      error code
*/
int32_t csi_aes_set_key(aes_handle_t handle, void *context, void *key, aes_key_len_bits_e key_len, aes_crypto_mode_e enc)
{
    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(key);

    if ((key_len != AES_KEY_LEN_BITS_128) ||
        (enc != AES_CRYPTO_MODE_ENCRYPT &&
         enc != AES_CRYPTO_MODE_DECRYPT)) {
        return ERR_AES(DRV_ERROR_PARAMETER);
    }

    ck_aes_priv_t *aes_priv = handle;

    memcpy(aes_priv->key, key, 16);

    if (AES_CRYPTO_MODE_ENCRYPT == enc)
        aes_priv->dec = CRYPTO_WAY_ENCRYPT;
    else if (AES_CRYPTO_MODE_DECRYPT == enc)
        aes_priv->dec = CRYPTO_WAY_DECRYPT;

    return 0;
}

/**
  \brief       encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   padding \ref aes_padding_mode_e.
  \return      error code
*/
int32_t csi_aes_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len)
{
    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(in);
    AES_NULL_PARA_CHK(out);
    AES_NULL_PARA_CHK(len);

    ck_aes_priv_t *aes_priv = handle;
    hsCipherContext_t ctx;

    aes_priv->status.busy = 1;

    tls_crypto_aes_init(&ctx, aes_priv->iv, aes_priv->key, 16, aes_priv->cbc);
    tls_crypto_aes_encrypt_decrypt(&ctx, in, out, len, aes_priv->dec);

    aes_priv->status.busy = 0;

    return 0;
}

/**
  \brief       aes ecb encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \return      error code
*/
int32_t csi_aes_ecb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len)
{
    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(in);
    AES_NULL_PARA_CHK(out);
    AES_NULL_PARA_CHK(len);

    return csi_aes_crypto(handle, context, in, out, len);
}

/**
  \brief       aes cbc encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector(updated after use)
  \return      error code
*/
int32_t csi_aes_cbc_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16])
{
    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(in);
    AES_NULL_PARA_CHK(out);
    AES_NULL_PARA_CHK(len);
    AES_NULL_PARA_CHK(iv);

    ck_aes_priv_t *aes_priv = handle;
    memcpy(aes_priv->iv, iv, 16);

    return csi_aes_crypto(handle, context, in, out, len);
}

/**
  \brief       aes cfb1 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector(updated after use)
  \return      error code
*/
int32_t csi_aes_cfb1_crypto(aes_handle_t handle, void *context, void *in, void *out,  uint32_t len, uint8_t iv[16])
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       aes cfb8 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector(updated after use)
  \return      error code
*/
int32_t csi_aes_cfb8_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16])
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       aes cfb128 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector(updated after use)
  \param[in]   num  the number of the 128-bit block we have used(updated after use)
  \return      error code
*/
int32_t csi_aes_cfb128_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num)
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       aes ofb encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector(updated after use)
  \param[in]   num  the number of the 128-bit block we have used(updated after use)
  \return      error code
*/
int32_t csi_aes_ofb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num)
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       aes ofb encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context(NULL when hardware implementation)
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   nonce_counter   Pointer to the 128-bit nonce and counter(updated after use)
  \param[in]   stream_block  Pointer to the saved stream-block for resuming(updated after use)
  \param[in]   num  the number of the 128-bit block we have used(updated after use)
  \return      error code
*/
int32_t csi_aes_ctr_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t nonce_counter[16],
                           unsigned char stream_block[16], uint32_t *num)
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get AES status.
  \param[in]   handle  aes handle to operate.
  \return      AES status \ref aes_status_t
*/
aes_status_t csi_aes_get_status(aes_handle_t handle)
{
    if (handle == NULL) {
        aes_status_t ret;
        memset(&ret, 0, sizeof(aes_status_t));
        return ret;
    }

    ck_aes_priv_t *aes_priv = handle;
    return aes_priv->status;
}
