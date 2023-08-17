/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/

/**
 * @brief  init the vendor cv params
 * @return
 */
void cv_params_init();

/**
 * @brief  get the chixiao board configuration
 * @return configuration char string
 */
const char *get_cx_board_config();

/**
 * @brief  get the chixiao service configuration
 * @return configuration char string
 */
const char *get_cx_service_config();

/**
 * @brief  get the chixiao input channel configuration
 * @return configuration char string
 */
const char *get_cx_input_channel_config();


/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

