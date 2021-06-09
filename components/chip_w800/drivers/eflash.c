/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_eflash.c
 * @brief    CSI Source File for Embedded Flash Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "drv/eflash.h"
#include "sys_freq.h"

#include "wm_type_def.h"
#include "wm_internal_flash.h"

#define EFLASH_SKIP_LEN     8192
#define EFLASH_SKIP_BASE  0x8000000
#define EFLASH_SKIP_END   0x8002000

#define ERR_EFLASH(errno) (CSI_DRV_ERRNO_EFLASH_BASE | errno)
#define EFLASH_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_EFLASH(DRV_ERROR_PARAMETER))

typedef struct {
    eflash_info_t eflashinfo;

    int32_t idx;
    eflash_event_cb_t cb;
    eflash_status_t status;
} ck_eflash_priv_t;

extern int32_t target_get_eflash(int32_t idx, uint32_t *base, eflash_info_t *info);
extern void w800_board_init(void);

static ck_eflash_priv_t eflash_handle[CONFIG_EFLASH_NUM];

/* Driver Capabilities */
static const eflash_capabilities_t driver_capabilities = {
    .event_ready = 1, /* event_ready */
    .data_width = 0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    .erase_chip = 1  /* erase_chip */
};

//
// Functions
//

/**
  \brief       Initialize EFLASH Interface. 1. Initializes the resources needed for the EFLASH interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref eflash_event_cb_t
  \return      pointer to eflash handle
*/
eflash_handle_t csi_eflash_initialize(int32_t idx, eflash_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_EFLASH_NUM) {
        return NULL;
    }

    /* obtain the eflash information */
    uint32_t base = 0u;
    eflash_info_t info;
    int32_t real_idx = target_get_eflash(idx, &base, &info);

    if (real_idx != idx) {
        return NULL;
    }

    w800_board_init();

    ck_eflash_priv_t *eflash_priv = &eflash_handle[idx];

    eflash_priv->eflashinfo.start = info.start;
    eflash_priv->eflashinfo.end = info.end;
    eflash_priv->eflashinfo.sector_count = info.sector_count;

    /* initialize the eflash context */
    eflash_priv->idx = idx;
    eflash_priv->cb = cb_event;
    eflash_priv->status.busy = 0U;
    eflash_priv->status.error = 0U;
    eflash_priv->eflashinfo.sector_size = info.sector_size;
    eflash_priv->eflashinfo.page_size = info.page_size;
    eflash_priv->eflashinfo.program_unit = info.sector_size;
    eflash_priv->eflashinfo.erased_value = info.erased_value;

    if (cb_event)
        cb_event(idx, EFLASH_EVENT_READY);

    return (eflash_handle_t)eflash_priv;
}

/**
  \brief       De-initialize EFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  eflash handle to operate.
  \return      error code
*/
int32_t csi_eflash_uninitialize(eflash_handle_t handle)
{
    EFLASH_NULL_PARAM_CHK(handle);

    ck_eflash_priv_t *eflash_priv = handle;
    eflash_priv->cb = NULL;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref eflash_capabilities_t
*/
eflash_capabilities_t csi_eflash_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_EFLASH_NUM) {
        eflash_capabilities_t ret = {0,0,0};
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or error code
*/
int32_t csi_eflash_read(eflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    EFLASH_NULL_PARAM_CHK(handle);
    EFLASH_NULL_PARAM_CHK(data);

    int ret;
    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr || eflash_priv->eflashinfo.start > (addr + cnt - 1) || eflash_priv->eflashinfo.end <= (addr + cnt - 1)) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

    eflash_priv->status.error = 0U;
    eflash_priv->status.busy = 1U;

    ret = tls_fls_read(addr, data, cnt);

    eflash_priv->status.busy = 0U;

    if (ret != TLS_FLS_STATUS_OK)
    {
        eflash_priv->status.error = 1U;

        if (eflash_priv->cb)
            eflash_priv->cb(eflash_priv->idx, EFLASH_EVENT_ERROR);
    }

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_EFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Program data to Flash.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t csi_eflash_program(eflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    EFLASH_NULL_PARAM_CHK(handle);
    EFLASH_NULL_PARAM_CHK(data);

    int ret;
    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr || eflash_priv->eflashinfo.start > (addr + cnt - 1) || eflash_priv->eflashinfo.end <= (addr + cnt - 1)) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

    eflash_priv->status.busy = 1U;
    eflash_priv->status.error = 0U;

    ret = tls_fls_write_without_erase(addr, (u8 *)data, cnt);

    eflash_priv->status.busy = 0U;

    if (ret != TLS_FLS_STATUS_OK)
    {
        eflash_priv->status.error = 1U;

        if (eflash_priv->cb)
            eflash_priv->cb(eflash_priv->idx, EFLASH_EVENT_ERROR);
    }

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_EFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  eflash handle to operate.
  \param[in]   addr  Sector address
  \return      error code
*/
int32_t csi_eflash_erase_sector(eflash_handle_t handle, uint32_t addr)
{
    EFLASH_NULL_PARAM_CHK(handle);

    int ret;
    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

    eflash_priv->status.error = 0U;

    if ((addr >= EFLASH_SKIP_BASE) && (addr < EFLASH_SKIP_END))
        return 0;

    eflash_priv->status.busy = 1U;

    ret = tls_fls_erase(addr / INSIDE_FLS_SECTOR_SIZE);

    eflash_priv->status.busy = 0U;

    if (ret != TLS_FLS_STATUS_OK)
        eflash_priv->status.error = 1U;

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_EFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  eflash handle to operate.
  \return      error code
*/
int32_t csi_eflash_erase_chip(eflash_handle_t handle)
{
    EFLASH_NULL_PARAM_CHK(handle);

    int i;
    int ret;
    ck_eflash_priv_t *eflash_priv = handle;

    eflash_priv->status.error = 0U;
    eflash_priv->status.busy = 1U;

    for (i = 0; i < (((eflash_priv->eflashinfo.end - eflash_priv->eflashinfo.start) - EFLASH_SKIP_LEN) / eflash_priv->eflashinfo.sector_size); i++)
    {
        ret = tls_fls_erase((eflash_priv->eflashinfo.start + EFLASH_SKIP_LEN + (eflash_priv->eflashinfo.sector_size* i)) / eflash_priv->eflashinfo.sector_size);
        if (ret != TLS_FLS_STATUS_OK)
            break;
    }

    eflash_priv->status.busy = 0U;

    if (ret != TLS_FLS_STATUS_OK)
        eflash_priv->status.error = 1U;

    return (ret == TLS_FLS_STATUS_OK) ? 0 : ERR_EFLASH(DRV_ERROR_SPECIFIC);
}

/**
  \brief       Get Flash information.
  \param[in]   handle  eflash handle to operate.
  \return      Pointer to Flash information \ref eflash_info_t
*/
eflash_info_t *csi_eflash_get_info(eflash_handle_t handle)
{
    HANDLE_PARAM_CHK(handle, NULL);

    ck_eflash_priv_t *eflash_priv = handle;
    eflash_info_t *eflash_info = &(eflash_priv->eflashinfo);

    return eflash_info;
}

/**
  \brief       Get EFLASH status.
  \param[in]   handle  eflash handle to operate.
  \return      EFLASH status \ref eflash_status_t
*/
eflash_status_t csi_eflash_get_status(eflash_handle_t handle)
{
    if (handle == NULL) {
        eflash_status_t ret = {0, 0};
        return ret;
    }

    ck_eflash_priv_t *eflash_priv = handle;

    return eflash_priv->status;
}
