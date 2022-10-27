/*******************************************************************************
  Filename:       ll_enc.c
  Revised:
  Revision:

  Description:    This file contains the BLE encryption API for the LL.


*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
//#include "jump_function.h"

/*******************************************************************************
 * MACROS
 */

//#include <csi_config.h>
#include <string.h>
#include <soc.h>
#include <drv/aes.h>
#include <clock.h>

#define ERR_AES(errno) (CSI_DRV_ERRNO_AES_BASE | errno)
#define AES_NULL_PARA_CHK(para)  HANDLE_PARAM_CHK(para, ERR_AES(DRV_ERROR_PARAMETER))

volatile static uint8_t block_cal_done = 0;

typedef struct {
    uint32_t base;
    uint32_t irq;
    void *iv;
    uint8_t *result_out;
    uint32_t len;
    aes_event_cb_t cb;
    aes_mode_e mode;
    aes_key_len_bits_e keylen;
    aes_endian_mode_e endian;
    aes_crypto_mode_e enc;
    aes_status_t status;
    uint8_t key[16];
} ck_aes_priv_t;

#define LL_ENC_BASE         0x40040000               // LL HW AES engine Base address 
extern int32_t target_get_aes_count(void);
extern int32_t target_get_aes(int32_t idx, uint32_t *base, uint32_t *irq);

static ck_aes_priv_t aes_handle[CONFIG_AES_NUM];

/* Driver Capabilities */
static const aes_capabilities_t driver_capabilities = {
    .ecb_mode = 1, /* ECB mode */
    .cbc_mode = 0, /* CBC mode */
    .cfb1_mode = 0, /* CFB1 mode */
    .cfb8_mode = 0, /* CFB8 mode */
    .cfb128_mode = 0, /* CFB128 mode */
    .ofb_mode = 0, /* OFB mode */
    .ctr_mode = 0, /* CTR mode */
    .bits_128 = 1, /* 128bits key length mode */
    .bits_192 = 0, /* 192bits key lenght mode */
    .bits_256 = 0  /* 256bits key length mode */
};

typedef enum {
    AES_PADDING_MODE_NO     = 0,        ///< NO-PADDING
    AES_PADDING_MODE_ZERO,              ///< ZERO-PADDING
    AES_PADDING_MODE_PKCS5              ///< PKCS5-PADDING
} aes_padding_mode_e;

/**
  \brief       Initialize AES Interface. 1. Initializes the resources needed for the AES interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref aes_event_cb_t
  \return      return aes handle if success
*/
aes_handle_t csi_aes_initialize(int32_t idx, aes_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_AES_NUM) {
        return NULL;
    }

    uint32_t irq = 0u;
    uint32_t base = 0u;

    ck_aes_priv_t *aes_priv = &aes_handle[idx];

    aes_priv->base = base;
    aes_priv->irq  = irq;

    /* initialize the aes context */
    aes_priv->cb = cb_event;
    aes_priv->iv = NULL;
    aes_priv->len = 16;
    aes_priv->result_out = NULL;
    aes_priv->mode = AES_MODE_CBC;
    aes_priv->keylen = AES_KEY_LEN_BITS_128;
    aes_priv->endian = AES_ENDIAN_LITTLE;
    aes_priv->status.busy = 0;
    hal_clk_gate_enable(MOD_AES);
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

    //hal_clk_gate_disable(MOD_AES);

    return 0;
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

    ck_aes_priv_t *aes_priv = &aes_handle[0];

    switch (mode) {
        case AES_MODE_ECB:
            break;

        default:
            return ERR_AES(DRV_ERROR_UNSUPPORTED);
    }

    switch (keylen_bits) {
        case AES_KEY_LEN_BITS_128:
            break;

        default:
            return ERR_AES(DRV_ERROR_UNSUPPORTED);
    }

    /* config the endian mode */
    if (endian == AES_ENDIAN_LITTLE || endian == AES_ENDIAN_BIG) {
        aes_priv->endian = endian;
    } else {
        return ERR_AES(AES_ERROR_ENDIAN);
    }

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

    if ((key_len != AES_KEY_LEN_BITS_128)) {
        return ERR_AES(DRV_ERROR_PARAMETER);
    }

    ck_aes_priv_t *aes_priv = (ck_aes_priv_t *)handle;
    memcpy(aes_priv->key,key,16);
    aes_priv->enc = enc;
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
int32_t csi_aes_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, aes_padding_mode_e padding)
{
    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(in);
    AES_NULL_PARA_CHK(out);
    AES_NULL_PARA_CHK(len);

    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}

int32_t csi_aes_ecb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len)
{
    uint8_t *plaintext;
    uint8_t *ciphertext;

    int ret = 0;

    AES_NULL_PARA_CHK(handle);
    AES_NULL_PARA_CHK(in);
    AES_NULL_PARA_CHK(out);
    AES_NULL_PARA_CHK(len);

    ck_aes_priv_t *aes_priv = &aes_handle[0];

    aes_priv->status.busy = 1;

    plaintext = (uint8_t *)in;
    ciphertext = (uint8_t *)out;
    int i = 0;

    if(aes_priv->enc == AES_CRYPTO_MODE_ENCRYPT) {
       for (i = 0; i < len; i += 16) {
            extern int bt_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
		                                uint8_t enc_data[16]);
            ret = bt_encrypt_be(aes_priv->key,plaintext, ciphertext);
            if(ret) {
                return ret;
            }
            plaintext += 16;
            ciphertext += 16;
        }
	}else if(aes_priv->enc == AES_CRYPTO_MODE_DECRYPT){
        for (i = 0; i < len; i += 16) {
            extern int bt_decrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
		                                uint8_t enc_data[16]);
            ret = bt_decrypt_be(aes_priv->key,plaintext, ciphertext);
            if(ret) {
                return ret;
            }
            plaintext += 16;
            ciphertext += 16;
        }
    }

    return 0;
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

    return ERR_AES(DRV_ERROR_UNSUPPORTED);
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

/**
  \brief       control aes power.
  \param[in]   handle  aes handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_aes_power_control(aes_handle_t handle, csi_power_stat_e state)
{
    return ERR_AES(DRV_ERROR_UNSUPPORTED);
}
