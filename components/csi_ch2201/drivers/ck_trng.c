/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_trng.c
 * @brief    CSI Source File for TRNG Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "drv/trng.h"
#include "ck_trng.h"


#define ERR_TRNG(errno) (CSI_DRV_ERRNO_TRNG_BASE | errno)
#define TRNG_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_TRNG(DRV_ERROR_PARAMETER))

typedef struct {
#ifdef CONFIG_LPM
    uint8_t trng_power_status;
    uint32_t trng_regs_saved[1];
#endif
    uint32_t base;
    trng_event_cb_t cb;
    trng_status_t status;
} ck_trng_priv_t;

extern int32_t target_get_trng(int32_t idx, uint32_t *base);

static ck_trng_priv_t trng_handle[CONFIG_TRNG_NUM];

/* Driver Capabilities */
static const trng_capabilities_t driver_capabilities = {
    .lowper_mode = 0 /* low power mode */
};

//
// Functions
//

ck_trng_reg_t *trng_reg = NULL;

static int32_t trng_enable(void)
{
    trng_reg->TCR |= TRNG_EN;
    return 0;
}

static int32_t trng_get_data(void)
{
    int data = trng_reg->TDR;
    return data;
}

static int32_t trng_data_is_ready(void)
{
    int flag = (trng_reg->TCR & TRNG_DATA_READY);
    return flag;
}

#ifdef CONFIG_LPM
static void manage_clock(trng_handle_t handle, uint8_t enable)
{
    drv_clock_manager_config(CLOCK_MANAGER_TRNG, enable);
}

static void do_prepare_sleep_action(trng_handle_t handle)
{
    ck_trng_priv_t *trng_priv = handle;
    uint32_t *tbase = (uint32_t *)(trng_priv->base);
    registers_save(trng_priv->trng_regs_saved, tbase, 1);
}

static void do_wakeup_sleep_action(trng_handle_t handle)
{
    ck_trng_priv_t *trng_priv = handle;
    uint32_t *tbase = (uint32_t *)(trng_priv->base);
    registers_restore(tbase, trng_priv->trng_regs_saved, 1);
}
#endif

/**
  \brief       Initialize TRNG Interface. 1. Initializes the resources needed for the TRNG interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref trng_event_cb_t
  \return      pointer to trng handle
*/
trng_handle_t csi_trng_initialize(int32_t idx, trng_event_cb_t cb_event)
{

    if (idx < 0 || idx >= CONFIG_TRNG_NUM) {
        return NULL;
    }

    /* obtain the trng information */
    uint32_t base = 0u;
    int32_t real_idx = target_get_trng(idx, &base);

    if (real_idx != idx) {
        return NULL;
    }

    ck_trng_priv_t *trng_priv = &trng_handle[idx];
    trng_priv->base = base;

    /* initialize the trng context */
    trng_reg = (ck_trng_reg_t *)(trng_priv->base);
    trng_priv->cb = cb_event;
    trng_priv->status.busy = 0;
    trng_priv->status.data_valid = 0;
#ifdef CONFIG_LPM
    csi_trng_power_control(trng_priv, DRV_POWER_FULL);
#endif

    return (trng_handle_t)trng_priv;
}

/**
  \brief       De-initialize TRNG Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  trng handle to operate.
  \return      error code
*/
int32_t csi_trng_uninitialize(trng_handle_t handle)
{
    TRNG_NULL_PARAM_CHK(handle);

    ck_trng_priv_t *trng_priv = handle;
    trng_priv->cb = NULL;

#ifdef CONFIG_LPM
    csi_trng_power_control(trng_priv, DRV_POWER_OFF);
#endif
    return 0;
}

/**
  \brief       control trng power.
  \param[in]   handle  trng handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_trng_power_control(trng_handle_t handle, csi_power_stat_e state)
{
    TRNG_NULL_PARAM_CHK(handle);

#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_TRNG(DRV_ERROR_UNSUPPORTED);
#endif
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id.
  \return      \ref trng_capabilities_t
*/
trng_capabilities_t csi_trng_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_TRNG_NUM) {
        trng_capabilities_t ret;
        memset(&ret, 0, sizeof(trng_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Get data from the TRNG.
  \param[in]   handle  trng handle to operate.
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items to obtain
  \return      error code
*/
int32_t csi_trng_get_data(trng_handle_t handle, void *data, uint32_t num)
{
    TRNG_NULL_PARAM_CHK(handle);
    TRNG_NULL_PARAM_CHK(data);
    TRNG_NULL_PARAM_CHK(num);

    ck_trng_priv_t *trng_priv = handle;

    trng_priv->status.busy = 1U;
    trng_priv->status.data_valid = 0U;

    uint8_t left_len = (uint32_t)data & 0x3;
    uint32_t result = 0;
    volatile uint32_t timeout = 0x1000000;

    /* if the data addr is not aligned by word */
    if (left_len) {
        trng_enable();

        while ((!trng_data_is_ready())) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        result = trng_get_data();

        /* wait the data is ready */
        while (trng_data_is_ready()) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        if (num > (4 - left_len)) {
            memcpy(data, &result, 4 - left_len);
        } else {
            memcpy(data, &result, num);
            trng_priv->status.busy = 0U;
            trng_priv->status.data_valid = 1U;

            if (trng_priv->cb) {
                trng_priv->cb(0, TRNG_EVENT_DATA_GENERATE_COMPLETE);
            }

            return 0;
        }

        num -= (4 - left_len);
        data = (void *)((uint32_t)data + (4 - left_len));
    }

    uint32_t word_len = num >> 2;
    left_len = num & 0x3;

    /* obtain the data by word */
    while (word_len--) {
        trng_enable();

        while ((!trng_data_is_ready())) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        result = trng_get_data();

        while (trng_data_is_ready()) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        *(uint32_t *)data = result;
        data = (void *)((uint32_t)data + 4);
    }

    /* if the num is not aligned by word */
    if (left_len) {
        trng_enable();

        while ((!trng_data_is_ready())) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        result = trng_get_data();

        while (trng_data_is_ready()) {
            if (!(timeout--)) {
                return ERR_TRNG(DRV_ERROR_TIMEOUT);
            }
        }

        timeout = 0x1000000;

        memcpy(data, &result, left_len);
    }

    trng_priv->status.busy = 0U;
    trng_priv->status.data_valid = 1U;

    if (trng_priv->cb) {
        trng_priv->cb(0, TRNG_EVENT_DATA_GENERATE_COMPLETE);
    }

    return 0;
}

/**
  \brief       Get TRNG status.
  \param[in]   handle  trng handle to operate.
  \return      TRNG status \ref trng_status_t
*/
trng_status_t csi_trng_get_status(trng_handle_t handle)
{
    if (handle == NULL) {
        trng_status_t ret;
        memset(&ret, 0, sizeof(trng_status_t));
        return ret;
    }

    ck_trng_priv_t *trng_priv = handle;
    return trng_priv->status;
}
