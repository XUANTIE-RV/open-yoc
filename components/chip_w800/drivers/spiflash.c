/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spi.c
 * @brief    CSI Source File for SPI Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <drv/spiflash.h>
#include <drv/irq.h>

#include <soc.h>
#include <csi_core.h>

#include "wm_flash.h"

#define ERR_SPIFLASH(errno) (CSI_DRV_ERRNO_SPIFLASH_BASE | errno)

typedef struct {
    int32_t idx;
    spiflash_event_cb_t cb_event;

    spiflash_info_t spiflash_info;

    uint8_t busy;
    uint8_t error;
} dw_spiflash_priv_t;

static dw_spiflash_priv_t spiflash_instance[CONFIG_SPIFLASH_NUM];

static const spiflash_capabilities_t spiflash_capabilities = {
    .event_ready = 1,            ///< Signal Flash Ready event
    .data_width = 0,           ///< Data width: 0=8-bit, 1=16-bit, 2=32-bit
    .erase_chip = 1,            ///< Supports EraseChip operation
};

extern int32_t target_get_spiflash(int32_t idx, uint32_t *base, spiflash_info_t *info);

/**
  \brief       Initialize SPIFLASH Interface. 1. Initializes the resources needed for the SPIFLASH interface 2.registers event callback function
  \param[in]   idx  device id
  \param[in]   cb_event  Pointer to \ref spiflash_event_cb_t
  \return      pointer to spiflash handle
*/
spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM)
        return NULL;

    /* obtain the eflash information */
    uint32_t base = 0u;
    spiflash_info_t info;
    int32_t real_idx = target_get_spiflash(idx, &base, &info);

    if (real_idx != idx) {
        return NULL;
    }

    tls_spifls_init();

    dw_spiflash_priv_t *spiflash_priv = &spiflash_instance[idx];

    spiflash_priv->idx = idx;
    spiflash_priv->cb_event = cb_event;

    spiflash_priv->spiflash_info.start = info.start;
    spiflash_priv->spiflash_info.end = info.end;
    spiflash_priv->spiflash_info.sector_count = info.sector_count;
    spiflash_priv->spiflash_info.sector_size = info.sector_size;
    spiflash_priv->spiflash_info.page_size = info.page_size;
    spiflash_priv->spiflash_info.program_unit = info.program_unit;
    spiflash_priv->spiflash_info.erased_value = info.erased_value;

    spiflash_priv->busy = 0;
    spiflash_priv->error = 0;

    if (cb_event)
        cb_event(idx, SPIFLASH_EVENT_READY);

    return spiflash_priv;
}

/**
  \brief       De-initialize SPIFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
{
    if (!handle)
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);

    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_priv->cb_event = NULL;

    tls_spifls_exit();

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   handle spiflash handle to operate.
  \return      \ref spiflash_capabilities_t
*/
spiflash_capabilities_t csi_spiflash_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        spiflash_capabilities_t ret;
        memset(&ret, 0, sizeof(spiflash_capabilities_t));
        return ret;
    }

    return spiflash_capabilities;
}

/**
  \brief       Set QSPI data line
  \param[in]   handle spiflash handle to operate
  \param[in]   line   spiflash data line mode
  \return      error code
*/
int32_t csi_spiflash_config_data_line(spiflash_handle_t handle, spiflash_data_line_e line)
{
    return ERR_SPIFLASH(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or error code
*/
int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    if (!handle || !data || !cnt)
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);

    int ret;
    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_priv->error = 0;
    spiflash_priv->busy = 1;

    ret = tls_spifls_read(addr, data, cnt);

    spiflash_priv->busy = 0;

    if (ret != TLS_FLS_STATUS_OK)
    {
        spiflash_priv->error = 1;

        if (spiflash_priv->cb_event)
            spiflash_priv->cb_event(spiflash_priv->idx, SPIFLASH_EVENT_ERROR);
    }

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_SPIFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Program data to Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    if (!handle || !data || !cnt)
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);

    int ret;
    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_priv->error = 0;
    spiflash_priv->busy = 1;

    ret = tls_spifls_write(addr, (u8 *)data, cnt);

    spiflash_priv->busy = 0;

    if (ret != TLS_FLS_STATUS_OK)
    {
        spiflash_priv->error = 1;

        if (spiflash_priv->cb_event)
            spiflash_priv->cb_event(spiflash_priv->idx, SPIFLASH_EVENT_ERROR);
    }

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_SPIFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Sector address
  \return      error code
*/
int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
{
    if (!handle)
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);

    int ret;
    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_priv->error = 0;
    spiflash_priv->busy = 1;

    ret = tls_spifls_erase(addr / spiflash_priv->spiflash_info.sector_size);

    spiflash_priv->busy = 0;

    if (ret != TLS_FLS_STATUS_OK)
        spiflash_priv->error = 1;

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_SPIFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_erase_chip(spiflash_handle_t handle)
{
    if (!handle)
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);

    int ret;
    int i;
    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_priv->error = 0;
    spiflash_priv->busy = 1;

    for (i = 0; i < ((spiflash_priv->spiflash_info.end - spiflash_priv->spiflash_info.start) / spiflash_priv->spiflash_info.sector_size); i++)
    {
        ret = tls_spifls_erase((spiflash_priv->spiflash_info.start + (spiflash_priv->spiflash_info.sector_size * i)) / spiflash_priv->spiflash_info.sector_size);
        if (ret != TLS_FLS_STATUS_OK)
            break;
    }

    spiflash_priv->busy = 0;

    if (ret != TLS_FLS_STATUS_OK)
        spiflash_priv->error = 1;

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_SPIFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Flash power down.
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_power_down(spiflash_handle_t handle)
{
    return ERR_SPIFLASH(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Flash release power down.
  \param[in]   handle  spiflash handle to operate.
  \return      error code
*/
int32_t csi_spiflash_release_power_down(spiflash_handle_t handle)
{
    return ERR_SPIFLASH(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get Flash information.
  \param[in]   handle  spiflash handle to operate.
  \return      Pointer to Flash information \ref spiflash_info_t
*/
spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)
{
    if (!handle)
        return NULL;

    dw_spiflash_priv_t *spiflash_priv = handle;
    return &spiflash_priv->spiflash_info;
}

/**
  \brief       Get SPIFLASH status.
  \param[in]   handle  spiflash handle to operate.
  \return      SPIFLASH status \ref spiflash_status_t
*/
spiflash_status_t csi_spiflash_get_status(spiflash_handle_t handle)
{
    spiflash_status_t spiflash_status = {0};

    if (!handle)
        return spiflash_status;

    dw_spiflash_priv_t *spiflash_priv = handle;

    spiflash_status.busy = spiflash_priv->busy;
    spiflash_status.error = spiflash_priv->error;

    return spiflash_status;
}

