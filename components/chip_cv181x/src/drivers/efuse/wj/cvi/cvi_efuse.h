/*
 * Copyright (C) 2017-2019 Cvitek Group Holding Limited
 */

/******************************************************************************
 * @file     cvi_efuse.h
 * @brief    head file for cvitek efuse
 * @version  V1.0
 * @date     2022/09/06
 ******************************************************************************/
#ifndef _CVI_EFUSE_H_
#define _CVI_EFUSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cvi_type.h>
#include "drv/common.h"

#define EFUSE_SW_INFO_SD_DL_SHIFT       22
#define EFUSE_SW_INFO_USB_DL_SHIFT      24
#define EFUSE_SW_INFO_UART_DL_SHIFT     26

enum SD_USB_UART_DL_MODE_E {
    SD_USB_UART_DL_ENABLE,
    SD_USB_UART_DL_FASTBOOT,
    SD_USB_UART_DL_DISABLE,
};

csi_error_t cvi_efuse_pwr_on();
csi_error_t cvi_efuse_wait_idle();
csi_error_t cvi_efuse_refresh_shadow();
csi_error_t cvi_efuse_read_word_from_efuse(uint32_t addr, uint32_t *data);
csi_error_t cvi_efuse_read_word_from_shadow(uint32_t addr, uint32_t *data);
csi_error_t cvi_efuse_program_word(uint32_t addr, const uint32_t data);
csi_error_t cvi_efuse_program_bit(uint32_t addr, const uint32_t bit);
csi_error_t cvi_efuse_pwr_off();

csi_error_t cvi_efuse_sd_dl_config(enum SD_USB_UART_DL_MODE_E mode);
csi_error_t cvi_efuse_usb_dl_config(enum SD_USB_UART_DL_MODE_E mode);
csi_error_t cvi_efuse_uart_dl_config(enum SD_USB_UART_DL_MODE_E mode);
uint32_t cvi_efuse_get_sd_dl_config();
uint32_t cvi_efuse_get_usb_dl_config();
uint32_t cvi_efuse_get_uart_dl_config();

csi_error_t cvi_efuse_disable_uart_dl();
csi_error_t cvi_efuse_is_uart_dl_enable();
csi_error_t cvi_efuse_set_sd_dl_button();
csi_error_t cvi_efuse_is_sd_dl_button();
CVI_S32 cvi_efuse_get_chip_sn(void *data, uint32_t *size);

#ifdef __cplusplus
}
#endif
#endif
