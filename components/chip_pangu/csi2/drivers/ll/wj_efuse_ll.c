/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_efuse_ll.c
 * @brief    CSI Source File for efusec Driver
 * @version  V1.0
 * @date     17. July 2020
 ******************************************************************************/

#include <wj_efuse_ll.h>
#include <drv/efuse.h>
#include <sys_clk.h>

uint32_t wj_efuse_program_bytes(csi_efuse_t *efuse, uint32_t addr, uint32_t *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);
    uint32_t i = 0U;
    uint32_t time_count = 0U;
    wj_efuse_regs_t *fbase = (wj_efuse_regs_t *)HANDLE_REG_BASE(efuse);

    if (wj_efuse_lock_status(fbase) == 0x2) {
        return CSI_ERROR;
    }

    for (i = 0U; i < size; i++) {
        time_count = 0;

        while (fbase->EFUSEC_STATUS & 0x8) {
            time_count++;

            if (time_count > 0x10000) {
                return CSI_BUSY;
            }
        }

        wj_efuse_write(fbase, addr, *data);

        while (fbase->EFUSEC_STATUS & 0x8) {
            time_count++;

            if (time_count > 0x10000) {
                return CSI_BUSY;
            }
        }

        addr += 4;
        data ++;
    }

    return i;
}

uint32_t wj_efuse_read_bits(csi_efuse_t *efuse, uint32_t addr, uint8_t *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);
    wj_efuse_regs_t *fbase = (wj_efuse_regs_t *)HANDLE_REG_BASE(efuse);
    uint32_t i = 0;
    uint32_t time_count = 0;

    for (i = 0; i < size; i ++) {
        time_count = 0;

        while (fbase->EFUSEC_STATUS & 0x8) {
            time_count++;

            if (time_count > 0x10000) {
                return CSI_BUSY;
            }
        }

        wj_efuse_read_enable(fbase, addr);
        time_count = 0;

        while (fbase->EFUSEC_STATUS & 0x8) {
            time_count++;

            if (time_count > 0x10000) {
                return CSI_BUSY;
            }
        }

        wj_efuse_read(fbase, data);
        addr ++;
        data ++;
    }

    return i;

}

int32_t wj_drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    uint8_t *read_data = (uint8_t *)data;

    return wj_efuse_read_bits(efuse, addr, read_data, size);
}

int32_t wj_drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    if (addr & 0x03) {
        return CSI_ERROR;
    }

    uint32_t cur = 0U;
    uint32_t src_buf = 0U;

    cur = size - (size & 0x03);
    wj_efuse_program_bytes(efuse, addr, (uint32_t *)data, cur >> 2);

    if (cur < size) {
        uint8_t *buffer_b = (uint8_t *)data + cur;

        for (int i = 0; i < size - cur; i++) {
            src_buf |= buffer_b[i] << i * 8;
        }

        wj_efuse_program_bytes(efuse, addr + cur, &src_buf, 1);
    }

    return size;
}
