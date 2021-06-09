/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file 	wj_sha.c
 * @brief	source file for sha csi driver
 * @version V2.0
 * @date	14. Sept 2020
 * ******************************************************/
#ifndef CONFIG_TEE_CA
#include <drv/sha.h>
#include <drv/irq.h>
#include "wj_sha_ll.h"

/**
 * Different modes get different plaintext blocks
 */
#define WJ_SHA_GET_BOLOCK_SIZE_BYTES(_mod_)      (((_mod_) < SHA_MODE_512) ? (uint32_t)64 : (uint32_t)128)

/**
 * Different modes get different data lengths
 * \note SHA-1\SHA-224\SHA-256 need 2 * 4 = 8 bytes
 *       SHA-512\SHA-384       need 4 * 4 = 16 bytes
 */
#define WJ_SHA_GET_MSGLEN_TAIL_4BYTES(_mod_)    (((_mod_) < SHA_MODE_512) ? (uint32_t)2 : (uint32_t)4)

/**
 * Number of result message digest bytes retrieved by sha mode
 */
#define WJ_SHA_GET_MSGDIGEST_BYTES(_mod_)       (((_mod_) == SHA_MODE_1  ) ? (uint32_t)20 :    \
        ((_mod_) == SHA_MODE_256) ? (uint32_t)32 :    \
        ((_mod_) == SHA_MODE_224) ? (uint32_t)28 :    \
        ((_mod_) == SHA_MODE_512) ? (uint32_t)64 :    \
        ((_mod_) == SHA_MODE_384) ? (uint32_t)48 :    \
        (uint32_t)20)

#define WJ_SHA_ALG_ALIGN_SORT(_idx_)            (_idx_ % 2U ? ((_idx_ - 1U) >> 1) : ((_idx_ >> 1) + 8U))

#define WJ_SHA_ALG_SWAP(_x_, _y_)               {   \
        uint8_t z = (uint8_t)*_x_;                  \
        *_x_ = (uint8_t)*_y_;                       \
        *_y_ = z;                                   \
    }

#define WJ_SHA_ALG_ALIGN_4BYTE(_len_)           (((_len_) % 4U) ? ((((_len_) >> 2U) << 2U) + 4U) : (_len_))


#define WJ_SHA_ALG_SELECT_MIN(_x_, _y_)         (((_x_) < (_y_)) ? (_x_) : (_y_))

#define WJ_SHA_WAIT_WRITED_10S                                                        (10000U)

/*<! Private function documentation */

/**
  \brief       SHA data sort
  \param[in]   d_addr       operate data dest
  \param[in]   s_addr       operate data source
  \param[in]   idx          current data idx
  \return      null
*/
void wj_sha_alg_sort(uint8_t *d_addr, uint8_t *s_addr, uint8_t idx)
{
    *(d_addr + (idx << 2) + 0U) = *(s_addr + 0U);
    *(d_addr + (idx << 2) + 1U) = *(s_addr + 1U);
    *(d_addr + (idx << 2) + 2U) = *(s_addr + 2U);
    *(d_addr + (idx << 2) + 3U) = *(s_addr + 3U);
}

/**
  \brief       SHA data flipping
  \param[in]   p       operate start address
  \param[in]   n       operate data length
  \return      null
*/
void wj_sha_alg_reverse(uint8_t *p, uint8_t n)
{
    if (n) {
        WJ_SHA_ALG_SWAP((p + 0U), (p + 3U));        ///< BYTE0 <<---SWAP--->> BYTE3
        WJ_SHA_ALG_SWAP((p + 1U), (p + 2U));        ///< BYTE1 <<---SWAP--->> BYTE2

        wj_sha_alg_reverse((p + 4U), (n - 1U));     ///< offset next address(uint32_t) and reduce operate data count
    }
}

/**
  \brief       SHA interrupt handling function
  \param[in]   arg        Callback function member variables
  \return      null
*/
void wj_sha_irq_handler(void *arg)
{
    csi_sha_t *sha = (csi_sha_t *)arg;
    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)sha->dev.reg_base;

    if (wj_sha_state_get_int_done(sha_base)) {
        if (sha->callback) {
            sha->callback(sha, SHA_EVENT_COMPLETE, sha->arg);
        }

        wj_sha_state_clr_int_done(sha_base);
    }
}

/**
  \brief       update the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      \ref csi_error_t
*/
csi_error_t wj_sha_update_sub(csi_sha_t *sha, csi_sha_context_t *context, const void *input, uint32_t size)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    uint8_t *p_data_in = (uint8_t *)input;

    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);

    sha->state.busy = 1U;

    *(p_data_in + size) = 0x80U;

    wj_sha_write_data_value(sha_base, (uint32_t *)p_data_in);

    wj_sha_ctrl_start_cal(sha_base);

    while(wj_sha_ctrl_get_cal(sha_base));

    sha->state.busy = 0U;

    return ret;
}

/*<! Public function documentation */

/**
  \brief       Initialize SHA Interface. 1. Initializes the resources needed for the SHA interface 2.registers event callback function
  \param[in]   sha  operate handle.
  \param[in]   idx index of sha
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_init(csi_sha_t *sha, uint32_t idx)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);

    if (0 == target_get(DEV_WJ_SHA_TAG, idx, &sha->dev)) {
        sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);
        memset((void *)sha_base, 0, sizeof(wj_sha_regs_t));

        sha->state.busy = 0U;
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
  \param[in]   sha  sha handle to operate.
  \return      none
*/
void csi_sha_uninit(csi_sha_t *sha)
{
    CSI_PARAM_CHK_NORETVAL(sha);
    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);

    memset((void *)sha_base, 0, sizeof(wj_sha_regs_t));
}

/**
  \brief       attach the callback handler to SHA
  \param[in]   sha          operate handle.
  \param[in]   callback     callback function
  \param[in]   arg          callback's param
  \return      error code
*/
csi_error_t csi_sha_attach_callback(csi_sha_t *sha, void *callback, void *arg)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);

    sha->callback = callback;
    sha->arg = arg;
    csi_irq_attach((uint32_t)sha->dev.irq_num, &wj_sha_irq_handler, &sha->dev);
    csi_irq_enable((uint32_t)sha->dev.irq_num);

    return CSI_OK;
}

/**
  \brief       detach the callback handler
  \param[in]   sha  operate handle.
*/
void csi_sha_detach_callback(csi_sha_t *sha)
{
    CSI_PARAM_CHK_NORETVAL(sha);
    sha->callback = NULL;
    sha->arg = NULL;
    csi_irq_disable((uint32_t)sha->dev.irq_num);
    csi_irq_detach((uint32_t)sha->dev.irq_num);
}

/**
  \brief       config sha mode.
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context
  \param[in]   mode    sha mode \ref csi_sha_mode_t
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_start(csi_sha_t *sha, csi_sha_context_t *context, csi_sha_mode_t mode)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);
    csi_error_t ret = CSI_OK;

    switch (mode) {
        case SHA_MODE_1:
        case SHA_MODE_256:
        case SHA_MODE_224:
        case SHA_MODE_512:
        case SHA_MODE_384:
            wj_sha_mode_sel(sha_base, (uint32_t)mode);
            wj_sha_clear_hash_value(sha_base);
            wj_sha_clear_data1_value(sha_base);
            wj_sha_clear_data2_value(sha_base);
            memset((void *)&context->mode, 0, sizeof(csi_sha_context_t));
            context->mode = mode;
            wj_sha_ctrl_interrupt_en(sha_base);

            wj_sha_mode_en_initial(sha_base);

            memcpy((uint8_t *)context->state, wj_sha_get_hash_address(sha_base), sizeof(context->state));
            break;

        case SHA_MODE_512_256:
        case SHA_MODE_512_224:
            ret = CSI_UNSUPPORTED;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

/**
  \brief       update the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_update(csi_sha_t *sha, csi_sha_context_t *context, const void *input, uint32_t size)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    uint8_t *p_data_in = (uint8_t *)input;
    uint32_t block_size;
    uint32_t left;
    uint32_t length;

    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);

    sha->state.busy = 1U;

    wj_sha_mode_sel(sha_base, (uint32_t)context->mode);
    memcpy(wj_sha_get_hash_address(sha_base), (uint8_t *)context->state, sizeof(context->state));

    block_size = WJ_SHA_GET_BOLOCK_SIZE_BYTES(context->mode);

    left = context->total[0] % block_size;
    context->total[0] += size;

    /**
     * If there is any unused message, it will be added to the new message for calculation
    */
    if (left) {
        memcpy((uint8_t *)context->buffer + left, p_data_in, WJ_SHA_ALG_SELECT_MIN(block_size - left, size)); ///< pad input message to complete block
        size += left;                                           ///< input message size need add original message size(only unused)
    } else {
        memcpy((uint8_t *)context->buffer, p_data_in, WJ_SHA_ALG_SELECT_MIN(block_size, size));
    }

    length = size;                                               ///< message size

    if (length >= block_size) {                                   ///< if length > block size, need accumulate two times least

        do {
            wj_sha_write_data_value(sha_base, (uint32_t *)context->buffer);

            wj_sha_ctrl_start_cal(sha_base);

            while(wj_sha_ctrl_get_cal(sha_base));

            length -= block_size;
            p_data_in += (block_size - left);                    ///< input address offset
            left = 0U;

            memcpy((uint8_t *)context->buffer, p_data_in, block_size);
        } while (length >= block_size);

        memset((uint8_t *)context->buffer, 0, block_size);

        memcpy((uint8_t *)context->buffer, p_data_in, (size & (block_size - 1U)));

    }

    memcpy((uint8_t *)context->state, wj_sha_get_hash_address(sha_base), sizeof(context->state));

    sha->state.busy = 0U;

    return ret;
}

/**
  \brief       accumulate the engine (async mode)
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_update_async(csi_sha_t *sha, csi_sha_context_t *context, const void *input, uint32_t size)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);

    csi_error_t ret = CSI_UNSUPPORTED;

    return ret;
}

/**
  \brief       finish the engine
  \param[in]   sha      sha handle to operate.
  \param[in]   context Pointer to the sha context
  \param[out]  output   Pointer to the result data
  \param[out]  out_size Pointer to the result data size(bytes)
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_finish(csi_sha_t *sha, csi_sha_context_t *context, void *output, uint32_t *out_size)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(context, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    uint32_t *p_data_in = (uint32_t *)context->buffer;
    uint8_t *out_buf = (uint8_t *)output;
    uint32_t i;
    uint32_t msg_length;
    uint32_t block_size;
    uint32_t length;
    uint32_t size = context->total[0];
    uint32_t pad_buf[4] = {0U};
    uint64_t pad_bit_len;

    wj_sha_regs_t *sha_base = (wj_sha_regs_t *)HANDLE_REG_BASE(sha);

    wj_sha_mode_sel(sha_base, (uint32_t)context->mode);
    memcpy(wj_sha_get_hash_address(sha_base), (uint8_t *)context->state, sizeof(context->state));

    block_size = WJ_SHA_GET_BOLOCK_SIZE_BYTES(context->mode);
    msg_length = WJ_SHA_GET_MSGLEN_TAIL_4BYTES(context->mode);

    pad_bit_len = (uint64_t)size << 3U;                                    ///< write message length into memory behind message

    length = ((size % block_size) + (msg_length << 2U)) + 1U;                          ///< message size + extra length + format(0x80)

    if (length > block_size) {
        wj_sha_update_sub(sha, context, p_data_in, size%block_size);
        memset((uint8_t *)context->buffer, 0, sizeof(context->buffer));
    } else {
        pad_buf[0] = 0x80U;
        memcpy((uint8_t *)p_data_in + (size % block_size), &pad_buf[0], sizeof(uint32_t)); ///< add tail(msg bit length)
    }

    pad_buf[1] = pad_bit_len >> 32;
    pad_buf[2] = (uint32_t)pad_bit_len & 0xFFFFFFFF;
    wj_sha_alg_reverse((uint8_t *)&pad_buf[1], 2U);

    memcpy((uint8_t *)p_data_in + block_size - 8U, &pad_buf[1], sizeof(uint32_t) << 1); ///< add tail(msg bit length)

    wj_sha_write_data_value(sha_base, p_data_in);

    wj_sha_ctrl_start_cal(sha_base);

    while(wj_sha_ctrl_get_cal(sha_base));

    *out_size = WJ_SHA_GET_MSGDIGEST_BYTES(context->mode);

    if (*out_size < WJ_SHA_GET_MSGDIGEST_BYTES(SHA_MODE_384)) {
        memcpy(context->buffer, wj_sha_get_hash_address(sha_base), *out_size);
        wj_sha_alg_reverse(context->buffer, ((uint8_t)*out_size) >> 2U);   ///< Flip the last result data
        memcpy(out_buf, context->buffer, *out_size);
    } else {
        memcpy(context->buffer, wj_sha_get_hash_address(sha_base), 64);
        wj_sha_alg_reverse(context->buffer, 16U);   ///< Flip the last result data

        for (i = 0U; i < (*out_size >> 2); i++) {
            wj_sha_alg_sort(out_buf, context->buffer + (WJ_SHA_ALG_ALIGN_SORT(i) << 2), i);
        }
    }

    wj_sha_clear_hash_value(sha_base);
    wj_sha_clear_data1_value(sha_base);
    wj_sha_clear_data2_value(sha_base);

    /**
     * clean cache
    */
    memset((uint8_t *)context->total, 0, sizeof(context->total));
    memset((uint8_t *)context->state, 0, sizeof(context->state));
    memset((uint8_t *)context->buffer, 0, sizeof(context->buffer));

    return ret;
}

/**
  \brief       Get SHA state.
  \param[in]   sha  sha handle to operate.
  \param[out]  state    sha state \ref csi_sha_state_t.
  \return      \ref csi_error_t
*/
csi_error_t csi_sha_get_state(csi_sha_t *sha, csi_sha_state_t *state)
{
    CSI_PARAM_CHK(sha, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);

    state->busy = sha->state.busy;
    state->error = sha->state.error;

    return CSI_OK;
}

#ifdef CONFIG_PM
csi_error_t wj_sha_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 25U);
            break;

        case PM_DEV_RESUME:
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 25U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_sha_enable_pm(csi_sha_t *sha)
{
    return csi_pm_dev_register(&sha->dev, wj_sha_pm_action, 25U, 0U);
}

void csi_sha_disable_pm(csi_sha_t *sha)
{
    csi_pm_dev_unregister(&sha->dev);
}
#endif

#endif /* CONFIG_TEE_CA */