/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file    wj_aes.c
 * @brief   source file for aes csi driver
 * @version V1.0
 * @date    2. July 2020
 * ******************************************************/
#ifndef CONFIG_TEE_CA
#include <drv/aes.h>
#include <drv/irq.h>
#include "wj_aes_ll.h"


#define AES_READ_CACHE_LITTLE(_addr_)   \
    (((uint32_t) * (_addr_ + 0U) << 0)  | ((uint32_t) * (_addr_ + 1U) << 8) | \
     ((uint32_t) * (_addr_ + 2U) << 16) | ((uint32_t) * (_addr_ + 3U) << 24))

#define AES_WRITE_CACHE_LITTLE(_addr_, _data_) \
    do {  \
        *(_addr_+0U) = (uint8_t)*(_data_+0U);  \
        *(_addr_+1U) = (uint8_t)*(_data_+1U);  \
        *(_addr_+2U) = (uint8_t)*(_data_+2U);  \
        *(_addr_+3U) = (uint8_t)*(_data_+3U);  \
    } while(0)

#define AES_IS_16BYTES_ALIGNMENT(_size_)     ((_size_ % 16U) ? -1 : 0)


static csi_error_t csi_aes_encrypt_mode(wj_aes_regs_t *aes_base)
{
    csi_error_t ret = CSI_OK;

    wj_aes_opt_code_encrypt(aes_base);

    return ret;
}

static csi_error_t csi_aes_decrypt_mode(wj_aes_regs_t *aes_base)
{
    csi_error_t ret = CSI_OK;
    uint32_t timeout = 10000U;

    wj_aes_opt_code_key_expand(aes_base);

    wj_aes_start_work(aes_base);

    do {
        if (!(timeout--)) {
            ret = CSI_ERROR;
            break;
        }

        mdelay(1U);
    } while (wj_aes_get_key_int(aes_base));

    wj_aes_opt_code_decrypt(aes_base);

    return ret;
}

/**
  \brief       Initialize AES Interface. Initializes the resources needed for the AES interface
  \param[in]   aes    operate handle
  \param[in]   idx    device id
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_init(csi_aes_t *aes, uint32_t idx)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = NULL;

    if (0 == target_get(DEV_WJ_AES_TAG, idx, &aes->dev)) {
        aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

        memset((void *)aes_base, 0, sizeof(wj_aes_regs_t));
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       De-initialize AES Interface. stops operation and releases the software resources used by the interface
  \param[in]   aes    aes handle to operate
  \return      None
*/
void csi_aes_uninit(csi_aes_t *aes)
{
    CSI_PARAM_CHK_NORETVAL(aes);

    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

    memset((void *)aes_base, 0, sizeof(wj_aes_regs_t));
}
/**
  \brief       Set encrypt key
  \param[in]   aes        aes handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref csi_aes_key_bits_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_set_encrypt_key(csi_aes_t *aes, void *key, csi_aes_key_bits_t key_len)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);
    uint32_t i;
    uint32_t keynum = 0U;
    uint32_t temp = 0U;


    switch (key_len) {
        case AES_KEY_LEN_BITS_128:
            wj_aes_key_len_128bit(aes_base);
            keynum = 4U;
            break;

        case AES_KEY_LEN_BITS_192:
            wj_aes_key_len_192bit(aes_base);
            keynum = 6U;
            break;

        case AES_KEY_LEN_BITS_256:
            wj_aes_key_len_256bit(aes_base);
            keynum = 8U;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    if (CSI_OK == ret) {
        for (i = 0U; i < 8U; i++) {
            wj_aes_write_key(aes_base, i, 0U);          ///< clear key value cache
        }

        for (i = 0U; i < keynum; i++) {
            temp = AES_READ_CACHE_LITTLE((uint8_t *)key + (i << 2));

            wj_aes_write_key(aes_base, keynum - 1U - i, temp);
        }

        ret = csi_aes_encrypt_mode(aes_base);

        wj_aes_stop_work(aes_base);
    }

    return ret;
}
/**
  \brief       Set decrypt key
  \param[in]   aes        aes handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref csi_aes_key_bits_t
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_set_decrypt_key(csi_aes_t *aes, void *key, csi_aes_key_bits_t key_len)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);
    uint32_t i;
    uint32_t keynum = 0U;
    uint32_t temp = 0U;

    switch (key_len) {
        case AES_KEY_LEN_BITS_128:
            wj_aes_key_len_128bit(aes_base);
            keynum = 4U;
            break;

        case AES_KEY_LEN_BITS_192:
            wj_aes_key_len_192bit(aes_base);
            keynum = 6U;
            break;

        case AES_KEY_LEN_BITS_256:
            wj_aes_key_len_256bit(aes_base);
            keynum = 8U;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    if (CSI_OK == ret) {
        for (i = 0U; i < 8U; i++) {
            wj_aes_write_key(aes_base, i, 0U);          ///< clear key value cache
        }

        for (i = 0U; i < keynum; i++) {
            temp = AES_READ_CACHE_LITTLE((uint8_t *)key + (i << 2));

            wj_aes_write_key(aes_base, keynum - 1U - i, temp);
        }

        ret = csi_aes_decrypt_mode(aes_base);

        wj_aes_stop_work(aes_base);
    }


    return ret;
}

/**
  \brief       Aes ecb encrypt or decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ecb_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t n = 0U;
    uint32_t temp;
    uint32_t timeout = 10000U;

    if (0 == AES_IS_16BYTES_ALIGNMENT(size)) {

        wj_aes_mode_ecb(aes_base);
        ret = csi_aes_encrypt_mode(aes_base);

        /* set the text before aes calculating */
        for (i = 0U; i < size; i = i + 16U) {
            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = AES_READ_CACHE_LITTLE((uint8_t *)in + n);
                wj_aes_write_data_in(aes_base, 3U - j, temp);
            }

            wj_aes_start_work(aes_base);

            do {
                if (!(timeout--)) {
                    ret = CSI_ERROR;
                    break;
                }

                mdelay(1U);
            } while (wj_aes_get_busy_sta(aes_base));

            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = wj_aes_read_data_out(aes_base, 3U - j);
                AES_WRITE_CACHE_LITTLE((uint8_t *)out + n, (uint8_t *)&temp);
            }
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       Aes ecb decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ecb_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t n = 0U;
    uint32_t temp;
    uint32_t timeout = 10000U;

    if (0 == AES_IS_16BYTES_ALIGNMENT(size)) {

        wj_aes_mode_ecb(aes_base);
        ret = csi_aes_decrypt_mode(aes_base);

        /* set the text before aes calculating */
        for (i = 0U; i < size; i = i + 16U) {
            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = AES_READ_CACHE_LITTLE((uint8_t *)in + n);
                wj_aes_write_data_in(aes_base, 3U - j, temp);
            }

            wj_aes_start_work(aes_base);

            do {
                if (!(timeout--)) {
                    ret = CSI_ERROR;
                    break;
                }

                mdelay(1U);
            } while (wj_aes_get_busy_sta(aes_base));

            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = wj_aes_read_data_out(aes_base, 3U - j);
                AES_WRITE_CACHE_LITTLE((uint8_t *)out + n, (uint8_t *)&temp);
            }
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       Aes cbc encrypt or decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cbc_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t n = 0U;
    uint32_t temp;
    uint32_t timeout = 10000U;

    if (0 == AES_IS_16BYTES_ALIGNMENT(size)) {
        for (i = 0U; i < 4U; i++) {
            wj_aes_write_iv(aes_base, i, 0U);          ///< clear iv value cache
        }

        wj_aes_mode_cbc(aes_base);
        ret = csi_aes_encrypt_mode(aes_base);

        /* set iv if the mode is CBC */
        for (i = 0U; i < 4U; i++) {
            temp = AES_READ_CACHE_LITTLE((uint8_t *)iv + (i << 2));
            wj_aes_write_iv(aes_base, 3U - i, temp);
        }

        /* set the text before aes calculating */
        for (i = 0U; i < size; i = i + 16U) {
            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = AES_READ_CACHE_LITTLE((uint8_t *)in + n);
                wj_aes_write_data_in(aes_base, 3U - j, temp);
            }

            wj_aes_start_work(aes_base);

            do {
                if (!(timeout--)) {
                    ret = CSI_ERROR;
                    break;
                }

                mdelay(1U);
            } while (wj_aes_get_busy_sta(aes_base));

            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = wj_aes_read_data_out(aes_base, 3U - j);
                AES_WRITE_CACHE_LITTLE((uint8_t *)out + n, (uint8_t *)&temp);
            }
        }

        for (i = 0U; i < 4U; i++) {
            temp = wj_aes_read_iv(aes_base, 3U - i);
            AES_WRITE_CACHE_LITTLE((uint8_t *)iv + (i << 2), (uint8_t *)&temp);
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       Aes cbc decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cbc_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv)
{
    CSI_PARAM_CHK(aes, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_aes_regs_t *aes_base = (wj_aes_regs_t *)HANDLE_REG_BASE(aes);

    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t n = 0U;
    uint32_t temp;
    uint32_t timeout = 10000U;

    if (0 == AES_IS_16BYTES_ALIGNMENT(size)) {
        for (i = 0U; i < 4U; i++) {
            wj_aes_write_iv(aes_base, i, 0U);          ///< clear iv value cache
        }

        wj_aes_mode_cbc(aes_base);
        ret = csi_aes_decrypt_mode(aes_base);

        /* set iv if the mode is CBC */
        for (i = 0U; i < 4U; i++) {
            temp = AES_READ_CACHE_LITTLE((uint8_t *)iv + (i << 2));
            wj_aes_write_iv(aes_base, 3U - i, temp);
        }

        /* set the text before aes calculating */
        for (i = 0U; i < size; i = i + 16U) {
            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = AES_READ_CACHE_LITTLE((uint8_t *)in + n);
                wj_aes_write_data_in(aes_base, 3U - j, temp);
            }

            wj_aes_start_work(aes_base);

            do {
                if (!(timeout--)) {
                    ret = CSI_ERROR;
                    break;
                }

                mdelay(1U);
            } while (wj_aes_get_busy_sta(aes_base));

            for (j = 0U; j < 4U; j++) {
                n = (j << 2) + i;            ///< j:each time offset 4bytes, i:each time offset 16bytes
                temp = wj_aes_read_data_out(aes_base, 3U - j);
                AES_WRITE_CACHE_LITTLE((uint8_t *)out + n, (uint8_t *)&temp);
            }
        }

        for (i = 0U; i < 4U; i++) {
            temp = wj_aes_read_iv(aes_base, 3U - i);
            AES_WRITE_CACHE_LITTLE((uint8_t *)iv + (i << 2), (uint8_t *)&temp);
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       Aes cfb1 encrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb1_encrypt(csi_aes_t *aes, void *in, void *out,  uint32_t size, void *iv)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes cfb1 decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb1_decrypt(csi_aes_t *aes, void *in, void *out,  uint32_t size, void *iv)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes cfb8 encrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb8_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes cfb8 decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb8_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes cfb128 decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[out]  num     the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb128_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes cfb128 encrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[out]  num     the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_cfb128_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes ofb encrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[out]  num     the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ofb_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes ofb decrypt
  \param[in]   aes     aes handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[out]  num     the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ofb_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes ctr encrypt
  \param[in]   aes              aes handle to operate
  \param[in]   in               Pointer to the Source data
  \param[out]  out              Pointer to the Result data
  \param[in]   size             the Source data size
  \param[in]   nonce_counter    Pointer to the 128-bit nonce and counter
  \param[in]   stream_block     Pointer to the saved stream-block for resuming
  \param[in]   iv               init vector
  \param[out]  num              the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ctr_encrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, uint8_t nonce_counter[16], uint8_t stream_block[16], void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
/**
  \brief       Aes ctr decrypt
  \param[in]   aes              aes handle to operate
  \param[in]   in               Pointer to the Source data
  \param[out]  out              Pointer to the Result data
  \param[in]   size             the Source data size
  \param[in]   nonce_counter    Pointer to the 128-bit nonce and counter
  \param[in]   stream_block     Pointer to the saved stream-block for resuming
  \param[in]   iv               init vecotr
  \param[out]  num              the number of the 128-bit block we have used
  \return      error code \ref csi_error_t
*/
csi_error_t csi_aes_ctr_decrypt(csi_aes_t *aes, void *in, void *out, uint32_t size, uint8_t nonce_counter[16], uint8_t stream_block[16], void *iv, uint32_t *num)
{
    return CSI_UNSUPPORTED;
}
#endif