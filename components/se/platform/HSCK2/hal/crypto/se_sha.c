/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <cmd.h>
#include <se_sha.h>
#include <se_dev_internal.h>

/**
  \brief       Initialize SHA Interface. Initializes the resources needed for the SHA interface
  \param[in]   se_sha  operate handle.
  \return      error code \ref uint32_t
*/
int se_sha_init(se_sha_t *se_sha)
{
    return 0;
}

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
  \param[in]   se_sha  sha handle to operate.
  \return      none
*/
void se_sha_uninit(se_sha_t *se_sha)
{
    return ;
}

/**
  \brief       start the engine
  \param[in]   se_sha  sha handle to operate.
  \param[in]   mode    sha mode \ref sc_sha_mode_t
  \return      error code \ref uint32_t
*/
int se_sha_start(se_sha_t *se_sha, se_sha_mode_t mode)
{
    uint32_t ret;
    uint32_t hs_mode;

    if (mode == SE_SHA_MODE_1) {
        hs_mode = 0x01;
    } else if (mode == SE_SHA_MODE_256) {
        hs_mode = 0x03;
    } else {
        return -1;
    }

	se_dev_lock();
    ret = hs_hash_start(hs_mode);
    if (ret != 0) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       update the engine
  \param[in]   se_sha  sha handle to operate.
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      error code \ref uint32_t
*/
int se_sha_update(se_sha_t *se_sha, const void *input, uint32_t size)
{
    uint32_t ret;
    CHECK_PARAM(input, -1);

    ret = hs_hash_update((SGD_UCHAR *)input, size);
    if (ret != 0) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       finish the engine
  \param[in]   se_sha   sha handle to operate.
  \param[out]  output   Pointer to the result data
  \param[out]  out_size Pointer to the result data size(bytes)
  \return      error code \ref uint32_t
*/
int se_sha_finish(se_sha_t *se_sha, void *output, uint32_t *out_size)
{
    uint32_t ret;
    CHECK_PARAM(output, -1);

    ret = hs_hash_finish(output, out_size);
	se_dev_unlock();
    if (ret != 0) {
        return -1;
    } else {
        return 0;
    }

}