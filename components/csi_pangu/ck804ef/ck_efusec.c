/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_efusec.c
 * @brief    CSI Source File for efusec Driver
 * @version  V1.0
 * @date     22. Mar 2019
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <drv/efusec.h>
#include <ck_efusec.h>
#include <sys_freq.h>

#define ERR_EFUSEC(errno) (CSI_DRV_ERRNO_EFUSEC_BASE | errno)
#define EFUSEC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_EFUSEC(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    efusec_status_t status;
    efusec_info_t efusecinfo;
} ck_efusec_priv_t;

extern int32_t target_get_efusec(int32_t idx, uint32_t *base, efusec_info_t *info);

static ck_efusec_priv_t efusec_handle[CONFIG_EFUSEC_NUM];
/* Driver Capabilities */
static const efusec_capabilities_t driver_capabilities = {
    .lock_shadow = 1
};

static int32_t efusec_program_word(efusec_handle_t handle, uint32_t dstaddr, uint32_t *srcbuf, uint32_t len)
{
    ck_efusec_priv_t *efusec_priv = handle;
    ck_efusec_reg_t *fbase = (ck_efusec_reg_t *)efusec_priv->base;
    uint32_t i;

    uint32_t time_count = 0;

    for (i = 0; i < len; i++) {
        time_count = 0;

        while (fbase->EFUSEC_STA & EFUSEC_STA_BUSY) {
            time_count++;

            if (time_count > 0x100000) {
                return ERR_EFUSEC(DRV_ERROR_BUSY);
            }
        }

        fbase->EFUSEC_ADDR = dstaddr;
        fbase->EFUSEC_WDATA = *srcbuf;
        fbase->EFUSEC_CTRL = EFUSEC_CTRL_PROGRAM_ENABLE;
        time_count = 0;

        while (1) {
            if ((fbase->EFUSEC_STA & EFUSEC_STA_PROGRAM_DONE)) {
                break;
            }

            time_count++;

            if (time_count > 0x100000) {
                return ERR_EFUSEC(DRV_ERROR_BUSY);
            }
        }

        srcbuf++;
        dstaddr += 4;
    }

    return i;
}

static int32_t efusec_read_bits(efusec_handle_t handle, uint32_t dstaddr, uint8_t *srcbuf, uint32_t len)
{
    ck_efusec_priv_t *efusec_priv = handle;
    ck_efusec_reg_t *fbase = (ck_efusec_reg_t *)efusec_priv->base;
    uint32_t i;
    uint32_t time_count = 0;

    for (i = 0; i < len; i++) {
        time_count = 0;

        while (fbase->EFUSEC_STA & EFUSEC_STA_BUSY) {
            time_count++;

            if (time_count > 0x100000) {
                return ERR_EFUSEC(DRV_ERROR_BUSY);
            }
        }

        time_count = 0;
        fbase->EFUSEC_ADDR = dstaddr;
        fbase->EFUSEC_CTRL |= EFUSEC_CTRL_READ_ENABLE;

        while (1) {
            if ((fbase->EFUSEC_STA & EFUSEC_STA_READ_DONE)) {
                break;
            }

            time_count++;

            if (time_count > 0x100000) {
                return ERR_EFUSEC(DRV_ERROR_BUSY);
            }
        }

        *srcbuf = fbase->EFUSEC_RDATA;
        srcbuf++;
        dstaddr += 1;
    }

    return i;
}
/**
  \brief       Initialize EFUSEC Interface. 1. Initializes the resources needed for the EFUSEC interface
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref efusec_event_cb_t
  \return      pointer to efusec handle
*/
efusec_handle_t drv_efusec_initialize(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_EFUSEC_NUM) {
        return NULL;
    }

    /* obtain the efusec information */
    uint32_t base = 0u;
    efusec_info_t info;
    int32_t real_idx = target_get_efusec(idx, &base, &info);

    if (real_idx != idx) {
        return NULL;
    }

    ck_efusec_priv_t *efusec_priv = &efusec_handle[idx];

    efusec_priv->base = base;
    efusec_priv->efusecinfo.start = info.start;
    efusec_priv->efusecinfo.end = info.end;

    /* initialize the efusec context */
    efusec_priv->status.lock_shadow = 0U;
    efusec_priv->status.lock_efuse  = 0U;
    efusec_priv->efusecinfo.page_size = info.page_size;
    efusec_priv->efusecinfo.program_unit = info.program_unit;

    return (efusec_handle_t)efusec_priv;
}

/**
  \brief       De-initialize EFUSEC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  efusec handle to operate.
  \return      error code
*/
int32_t csi_efusec_uninitialize(efusec_handle_t handle)
{
    EFUSEC_NULL_PARAM_CHK(handle);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref efusec_capabilities_t
*/
efusec_capabilities_t drv_efusec_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_EFUSEC_NUM) {
        efusec_capabilities_t ret = {0};
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  efusec handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or error code
*/
int32_t drv_efusec_read(efusec_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    EFUSEC_NULL_PARAM_CHK(handle);
    EFUSEC_NULL_PARAM_CHK(data);

    ck_efusec_priv_t *efusec_priv = handle;

    if (efusec_priv->efusecinfo.start > addr || efusec_priv->efusecinfo.end <= addr || efusec_priv->efusecinfo.start > (addr + cnt - 1) || efusec_priv->efusecinfo.end <= (addr + cnt - 1)) {
        return ERR_EFUSEC(DRV_ERROR_PARAMETER);
    }

    efusec_priv->status.mode = 0U;

    uint8_t *read_data = (uint8_t *)data;
    return  efusec_read_bits(handle, addr, (uint8_t *)read_data, cnt);
}

/**
  \brief       Program data to Flash.
  \param[in]   handle  efusec handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t drv_efusec_program(efusec_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    EFUSEC_NULL_PARAM_CHK(handle);
    EFUSEC_NULL_PARAM_CHK(data);

    ck_efusec_priv_t *efusec_priv = handle;

    if (efusec_priv->efusecinfo.start > addr || efusec_priv->efusecinfo.end <= addr || efusec_priv->efusecinfo.start > (addr + cnt - 1) || efusec_priv->efusecinfo.end <= (addr + cnt - 1)) {
        return ERR_EFUSEC(DRV_ERROR_PARAMETER);
    }

    if (addr & 0x3 || cnt & 3) {
        return ERR_EFUSEC(DRV_ERROR_PARAMETER);
    }

    ck_efusec_reg_t *fbase = (ck_efusec_reg_t *)efusec_priv->base;

    if (fbase->LOCK_CTRL & 0x2) {
        return ERR_EFUSEC(DRV_ERROR_BUSY);
    }

    efusec_priv->status.mode = 1U;

    uint32_t i;
    uint32_t *write_data = (uint32_t *)data;

    for (i = 0; i < (cnt / efusec_priv->efusecinfo.program_unit); i++) {
        efusec_program_word(handle, addr, write_data, 1);
        write_data++;
    }

    return (int32_t)cnt;
}

/**
  \brief      lock efusec.
  \param[in]   handle  efusec handle to operate.
  \param[in]   lock   efusec lock type \ref efusec_lock_e.
  \return      error code
*/
int32_t drv_efusec_lock(efusec_handle_t handle, efusec_lock_e lock)
{
    EFUSEC_NULL_PARAM_CHK(handle);

    if (!(lock == EFUSEC_LOCK_SHADOW || lock == EFUSEC_LOCK_EFUSE)) {
        return ERR_EFUSEC(DRV_ERROR_PARAMETER);
    }

    ck_efusec_priv_t *efusec_priv = handle;
    ck_efusec_reg_t *fbase = (ck_efusec_reg_t *)efusec_priv->base;
    uint32_t data;

    if (lock == EFUSEC_LOCK_SHADOW) {
        if (fbase->LOCK_CTRL & LOCK_CTRL_ENABLE_LOCK0) {
            return ERR_EFUSEC(DRV_ERROR_BUSY);
        }

        data = LOCK_CTRL_ENABLE_LOCK0;
        efusec_priv->status.mode = 1U;
        efusec_priv->status.lock_shadow = 1U;
        efusec_program_word(handle, 0x270, &data, 1);
    } else {
        if (fbase->LOCK_CTRL & LOCK_CTRL_ENABLE_LOCK1) {
            return ERR_EFUSEC(DRV_ERROR_BUSY);
        }

        data = LOCK_CTRL_ENABLE_LOCK1;
        efusec_priv->status.mode = 1U;
        efusec_priv->status.lock_efuse = 1U;
        efusec_program_word(handle, 0x270, &data, 1);
    }

    uint32_t time_count = 0;
    /* read from efuse date to shadow registers */
    fbase->EFUSEC_CTRL |= EFUSEC_CTRL_RELOAD_ENABLE;

    while (1) {
        if ((fbase->EFUSEC_STA & EFUSEC_STA_RELOAD_DONE)) {
            break;
        }

        time_count++;

        if (time_count > 0x100000) {
            return ERR_EFUSEC(DRV_ERROR_BUSY);
        }
    }

    return 0;
}

/**
  \brief       Get Flash information.
  \param[in]   handle  efusec handle to operate.
  \return      Pointer to Flash information \ref efusec_info_t
*/
efusec_info_t *drv_efusec_get_info(efusec_handle_t handle)
{
    HANDLE_PARAM_CHK(handle, NULL);

    ck_efusec_priv_t *efusec_priv = handle;
    efusec_info_t *efusec_info = &(efusec_priv->efusecinfo);

    return efusec_info;
}

/**
  \brief       Get EFUSEC status.
  \param[in]   handle  efusec handle to operate.
  \return      EFUSEC status \ref efusec_status_t
*/
efusec_status_t drv_efusec_get_status(efusec_handle_t handle)
{
    if (handle == NULL) {
        efusec_status_t ret = {0, 0};
        return ret;
    }

    ck_efusec_priv_t *efusec_priv = handle;

    return efusec_priv->status;
}
