/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     efuse.c
 * @brief    CSI Source File for efusec Driver
 * @version  V1.0
 * @date     17. July 2020
 ******************************************************************************/

#include <drv/efuse.h>
#include <sys_clk.h>
#include <wj_efuse_ll.h>


csi_error_t drv_efuse_init(csi_efuse_t *efuse, int32_t idx)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    target_get(DEV_WJ_EFUSE_TAG, idx, &efuse->dev);

    efuse->info.start = 0x00;
    efuse->info.end = 0x27f;

    return ret;
}

void drv_efuse_uninit(csi_efuse_t *efuse)
{
    CSI_PARAM_CHK_NORETVAL(efuse);

    memset(efuse, 0, sizeof(csi_efuse_t));
}

int32_t drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse,CSI_ERROR);

    if (addr < efuse->info.start || addr > efuse->info.end || addr + size - 1 < efuse->info.start || addr + size - 1 > efuse->info.end) {
        return CSI_ERROR;
    }

    return wj_drv_efuse_read(efuse,addr,data,size);
}

int32_t drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse,CSI_ERROR);

    if (addr < efuse->info.start || addr > efuse->info.end || addr + size - 1 < efuse->info.start || addr + size - 1 > efuse->info.end) {
        return CSI_ERROR;
    }

    return wj_drv_efuse_program(efuse,addr,data,size);
}

csi_error_t drv_efuse_get_info(csi_efuse_t *efuse, csi_efuse_info_t *info)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    info->start =  efuse->info.start;
    info->end =  efuse->info.end;
    return CSI_OK;
}

