/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <board_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (defined (CONFIG_GW_SMARTLIVING_SUPPORT) && (CONFIG_GW_SMARTLIVING_SUPPORT) \
    || defined (CONFIG_GW_FOTA_EN) && (CONFIG_GW_FOTA_EN)                       \
    || defined (CONFIG_SUPPORT_YMODEM) && (CONFIG_SUPPORT_YMODEM))
#define CONFIG_BOARD_OTA_SUPPORT 1
#endif    
/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

/**
 * @brief  check if in factory mode.
 * re-implement if need.
 * @return
 */
int board_ftmode_check(void);

/**
 * @brief  gpio test on board.
 * re-implement if need.
 * @return
 */
int board_gpio_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

