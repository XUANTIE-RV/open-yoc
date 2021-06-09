/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_efusec_v2.c
 * @brief    CSI Source File for efusec Driver
 * @version  V1.0
 * @date     22. Mar 2019
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#include <drv_efusec.h>
#include <wj_efuse.h>
#include <drv/efuse.h>

#define CONFIG_EFUSEC_NUM 1

typedef struct {
    uint32_t base;
    efusec_status_t status;
    efusec_info_t efusecinfo;
} ck_efusec_v2_priv_t;

static ck_efusec_v2_priv_t efusec_handle[CONFIG_EFUSEC_NUM];

static int32_t efusec_program_bytes(efusec_handle_t handle, uint32_t dstaddr, uint8_t *srcbuf, uint32_t len)
{
    ck_efusec_v2_priv_t *efusec_priv = handle;
    ck_efusec_v2_reg_t *fbase = (ck_efusec_v2_reg_t *)efusec_priv->base;
    uint32_t i;
    uint8_t data = *srcbuf;
    uint32_t time_count = 0;
    fbase->WP_A = EFUSEC_WP_A_MAGIC;
    fbase->WP_B = EFUSEC_WP_B_MAGIC;
    fbase->AVD_PAD = 0xe09;

    for (i = 0; i < len; i++) {
        for (int j = 0; j < 8; j++) {
            time_count = 0;

            while (!(fbase->IDLE & EFUSEC_STA_BUSY)) {
                time_count++;

                if (time_count > 0x100000) {
                    return -1;
                }
            }

            if (data & 0x1) {
                fbase->ADDR = (j * 128 + dstaddr);
                fbase->WEN = EFUSEC_CTRL_PROGRAM_ENABLE;
                time_count = 0;

                while (!(fbase->IDLE & EFUSEC_STA_BUSY)) {
                    time_count++;

                    if (time_count > 0x100000) {
                        return -1;
                    }
                }
            }

            data = data >> 1;
        }

        srcbuf++;
        data = *srcbuf;
        dstaddr += 4;
    }

    fbase->WP_CLR = 1;

    return i;
}

static int32_t efusec_read_bytes(efusec_handle_t handle, uint32_t dstaddr, uint8_t *srcbuf, uint32_t len)
{
    ck_efusec_v2_priv_t *efusec_priv = handle;
    ck_efusec_v2_reg_t *fbase = (ck_efusec_v2_reg_t *)efusec_priv->base;
    uint32_t i;
    uint32_t time_count = 0;

    for (i = 0; i < len; i++) {
        time_count = 0;

        while (!(fbase->IDLE & 0x1)) {
            time_count++;

            if (time_count > 0x100000) {
                return -1;
            }
        }

        time_count = 0;
        fbase->ADDR = dstaddr;
        fbase->REN = EFUSEC_CTRL_READ_ENABLE;

        while (!(fbase->IDLE & 0x1)) {
            time_count++;

            if (time_count > 0x100000) {
                return -1;
            }
        }

        *srcbuf = fbase->RDATA;
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
#define EFUSE_START 0
#define EFUSE_END 128

efusec_handle_t drv_efusec_initialize(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_EFUSEC_NUM) {
        return NULL;
    }

    csi_dev_t dev;
    target_get(DEV_WJ_EFUSE_TAG, idx, &dev);

    ck_efusec_v2_priv_t *efusec_priv = &efusec_handle[idx];

    efusec_priv->base = dev.reg_base;
    efusec_priv->efusecinfo.start = EFUSE_START;
    efusec_priv->efusecinfo.end = EFUSE_END;

    /* initialize the efusec context */
    efusec_priv->status.lock_shadow = 0U;
    efusec_priv->status.lock_efuse  = 0U;

    return (efusec_handle_t)efusec_priv;
}

csi_error_t drv_efuse_init(csi_efuse_t *efuse, int32_t idx)
{
    drv_efusec_initialize(idx);
    efuse->dev.idx = idx;
    return CSI_OK;
}

void drv_efuse_uninit(csi_efuse_t *efuse)
{
;
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
    ck_efusec_v2_priv_t *efusec_priv = handle;

    if (efusec_priv->efusecinfo.start > addr || efusec_priv->efusecinfo.end <= addr
        || efusec_priv->efusecinfo.start > (addr + cnt - 1)
        || efusec_priv->efusecinfo.end <= (addr + cnt - 1)) {
        return -1;
    }

    efusec_priv->status.mode = 0U;

    uint8_t *read_data = (uint8_t *)data;
    return  efusec_read_bytes(handle, addr, (uint8_t *)read_data, cnt);
}


int32_t drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size)
{
    ck_efusec_v2_priv_t *efusec_priv = &efusec_handle[efuse->dev.idx];
    return drv_efusec_read((void *)efusec_priv, addr, data, size);
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
    ck_efusec_v2_priv_t *efusec_priv = handle;

    if (efusec_priv->efusecinfo.start > addr || efusec_priv->efusecinfo.end <= addr
        || efusec_priv->efusecinfo.start > (addr + cnt - 1)
        || efusec_priv->efusecinfo.end <= (addr + cnt - 1)) {
        return -1;
    }

    efusec_priv->status.mode = 1U;

    uint8_t *write_data = (uint8_t *)data;
    efusec_program_bytes(handle, addr, (uint8_t *)write_data, cnt);

    return (int32_t)cnt;
}

int32_t drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size)
{
    ck_efusec_v2_priv_t *efusec_priv = &efusec_handle[efuse->dev.idx];
    return drv_efusec_program((void *)efusec_priv, addr, data, size);
}

/**
  \brief       Get Flash information.
  \param[in]   handle  efusec handle to operate.
  \return      Pointer to Flash information \ref efusec_info_t
*/
efusec_info_t *drv_efusec_get_info(efusec_handle_t handle)
{
    ck_efusec_v2_priv_t *efusec_priv = handle;
    efusec_info_t *efusec_info = &(efusec_priv->efusecinfo);

    return efusec_info;
}

csi_error_t drv_efuse_get_info(csi_efuse_t *efuse, csi_efuse_info_t *info)
{
    ck_efusec_v2_priv_t *efusec_priv = &efusec_handle[efuse->dev.idx];
    efusec_info_t *p;
    p = drv_efusec_get_info((void *)efusec_priv);
    info->end = p->end;
    info->start = p->start;
    return CSI_OK;
}


