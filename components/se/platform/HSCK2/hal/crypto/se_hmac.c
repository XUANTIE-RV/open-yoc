/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <cmd.h>
#include <se_hmac.h>
#include <se_dev_internal.h>

/**
  \brief       Initialize MAC Interface. Initializes the resources needed for the MAC interface
  \param[in]   se_hmac  operate handle.
  \return      \ref 0 success, Non-zero failure
*/
int se_hmac_init(se_hmac_t *se_hmac)
{
    return 0;
}

/**
  \brief       De-initialize MAC Interface. stops operation and releases the software resources used by the interface
  \param[in]   se_hmac  mac handle to operate.
  \return      none
*/
void se_hmac_uninit(se_hmac_t *se_hmac)
{
    return;
}

/**
  \brief       MAC      set key function.
  \param[in]   se_hmac  mac handle to operate.
  \param[in]   key      Pointer to the mac key.
  \param[in]   key_len  Length of key.
  \return      \ref 0 success, Non-zero failure
*/
int se_hmac_set_key(se_hmac_t *se_hmac, uint8_t *key, uint32_t key_len)
{
    uint32_t ret;
    CHECK_PARAM(key, -1);
    CHECK_PARAM(key_len <= 0x40, -1);

	se_dev_lock();
    ret = hs_hmac_set_key(key, key_len);
	se_dev_unlock();
    if (ret != 0) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       MAC operation function.
  \param[in]   se_hmac  mac handle to operate.
  \param[in]   mode     se_sha_mode_t.
  \param[in]   msg      Pointer to the mac input message.
  \param[in]   msg_len  Length of msg.
  \param[out]  out      mac buffer, malloc by caller.
  \param[out]  out_len  out mac length, should 32 bytes if HMAC_SHA256 mode.
  \return      \ref 0 success, Non-zero failure
*/
int se_hmac_calc(se_hmac_t *se_hmac, se_sha_mode_t mode, uint8_t *msg,
             uint32_t msg_len, uint8_t *out, uint32_t *out_len)
{
    uint32_t ret;
    CHECK_PARAM(msg, -1);
    CHECK_PARAM(msg_len <= 0x800, -1);
    CHECK_PARAM(out, -1);
    CHECK_PARAM(mode == SE_SHA_MODE_256, -1);

	se_dev_lock();
    ret = hs_hmac_calc(msg, msg_len, out, out_len);
	se_dev_unlock();
    if (ret != 0) {
        return -1;
    } else {
        return 0;
    }
}

