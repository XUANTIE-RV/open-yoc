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
#include "ck_eflash.h"
#include "sys_freq.h"


#define ERR_EFLASH(errno) (CSI_DRV_ERRNO_EFLASH_BASE | errno)
#define EFLASH_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_EFLASH(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
#ifdef CONFIG_CHIP_CH2201
    uint16_t prog;
    uint16_t erase;
#endif
    eflash_info_t eflashinfo;
    eflash_event_cb_t cb;
    eflash_status_t status;
} ck_eflash_priv_t;

extern int32_t target_get_eflash_count(void);
extern int32_t target_get_eflash(int32_t idx, uint32_t *base, eflash_info_t *info);

static ck_eflash_priv_t eflash_handle[CONFIG_EFLASH_NUM];
/* Driver Capabilities */
static const eflash_capabilities_t driver_capabilities = {
    .event_ready = 1, /* event_ready */
    .data_width = 2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    .erase_chip = 0  /* erase_chip */
};

//
// Functions
//

static int32_t eflash_program_word(eflash_handle_t handle, uint32_t dstaddr, uint32_t *srcbuf, uint32_t len)
{
    ck_eflash_priv_t *eflash_priv = handle;
    ck_eflash_reg_t *fbase = (ck_eflash_reg_t *)eflash_priv->base;
    uint32_t i;

    for (i = 0; i < len; i++) {
        fbase->ADDR = dstaddr;
        fbase->WDATA = *srcbuf;
        fbase->WE = 1;
        srcbuf++;
        dstaddr += 4;
    }

    return (i << 2);
}

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

    ck_eflash_priv_t *eflash_priv = &eflash_handle[idx];
#ifdef CONFIG_CHIP_CH2201
    uint16_t erase, prog;
    drv_get_eflash_params(OSR_8M_CLK_24M, &prog, &erase);
    eflash_priv->prog = prog;
    eflash_priv->erase = erase;
#endif
    eflash_priv->base = base;
    eflash_priv->eflashinfo.start = info.start;
    eflash_priv->eflashinfo.end = info.end;
    eflash_priv->eflashinfo.sector_count = info.sector_count;

    /* initialize the eflash context */
    eflash_priv->cb = cb_event;
    eflash_priv->status.busy = 0;
    eflash_priv->status.error = 0U;
    eflash_priv->eflashinfo.sector_size = EFLASH_SECTOR_SIZE;
    eflash_priv->eflashinfo.page_size = EFLASH_PAGE_SIZE;
    eflash_priv->eflashinfo.program_unit = EFLASH_PROGRAM_UINT;
    eflash_priv->eflashinfo.erased_value = EFLASH_ERASED_VALUE;

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

    volatile uint8_t *src_addr = (uint8_t *)addr;
    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr || eflash_priv->eflashinfo.start > (addr + cnt - 1) || eflash_priv->eflashinfo.end <= (addr + cnt - 1)) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

    eflash_priv->status.error = 0U;

    int i;

    for (i = 0; i < cnt; i++) {
        *((uint8_t *)data + i) = *(src_addr + i);
    }

    return i;
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

    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr || eflash_priv->eflashinfo.start > (addr + cnt - 1) || eflash_priv->eflashinfo.end <= (addr + cnt - 1)) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    uint32_t cur = 0;
    uint32_t pad_buf;

    if (addr & 0x3) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

    eflash_priv->status.busy = 1U;
    eflash_priv->status.error = 0U;

#ifdef CONFIG_CHIP_CH2201
    ck_eflash_reg_t *fbase = (ck_eflash_reg_t *)eflash_priv->base;
    fbase->TRCV = eflash_priv->prog;
#endif

    if (((uint32_t)data & 0x3) == 0) {
        cur = cnt - (cnt & 0x3);
        eflash_program_word(handle, addr, (uint32_t *)data, cur >> 2);
    } else {
        uint8_t *buffer_b = (uint8_t *)data;

        for (; cur < cnt - 3; cur += 4, buffer_b += 4) {
            pad_buf = buffer_b[0] | (buffer_b[1] << 8) | (buffer_b[2] << 16) | (buffer_b[3] << 24);
            eflash_program_word(handle, addr + cur, &pad_buf, 1);
        }
    }

    if (cur < cnt) {
        uint8_t *buffer_b = (uint8_t *)(addr + cur);
        pad_buf = buffer_b[0] | (buffer_b[1] << 8) | (buffer_b[2] << 16) | (buffer_b[3] << 24);
        uint8_t *pad = (uint8_t *)&pad_buf;
        uint8_t *buff = (uint8_t *)data;
        uint8_t i;

        for (i = 0; i < (cnt - cur); i++) {
            pad[i] = buff[cur + i];
        }

        eflash_program_word(handle, addr + cur, &pad_buf, 1);
    }

    eflash_priv->status.busy = 0U;

    return cnt;
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

    ck_eflash_priv_t *eflash_priv = handle;

    if (eflash_priv->eflashinfo.start > addr || eflash_priv->eflashinfo.end <= addr) {
        return ERR_EFLASH(DRV_ERROR_PARAMETER);
    }


    addr = addr & ~(EFLASH_SECTOR_SIZE - 1);
    ck_eflash_reg_t *fbase = (ck_eflash_reg_t *)eflash_priv->base;

    if (eflash_priv->status.busy) {
        return ERR_EFLASH(DRV_ERROR_BUSY);
    }

#ifdef CONFIG_CHIP_CH2201
    fbase->TRCV = eflash_priv->erase;
#endif
    eflash_priv->status.busy = 1U;
    eflash_priv->status.error = 0U;
    fbase->ADDR = addr;
    fbase->PE = 0x1;
    eflash_priv->status.busy = 0U;

    return 0;
}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  eflash handle to operate.
  \return      error code
*/
int32_t csi_eflash_erase_chip(eflash_handle_t handle)
{
    EFLASH_NULL_PARAM_CHK(handle);

    return ERR_EFLASH(DRV_ERROR_UNSUPPORTED);
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
