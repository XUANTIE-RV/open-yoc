/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include "soc.h"

/*******************************************************************************
 * function: ioreuse config
 *******************************************************************************/
csi_error_t csi_pin_set_mux(pin_name_t pin_name, pin_func_t pin_func)
{
    ///< TODO：设置引脚功能
    return CSI_OK;
}


csi_error_t csi_pin_mode(pin_name_t pin_name, csi_gpio_mode_t mode)
{
    ///< TODO：设置引脚模式
    csi_error_t ret = CSI_OK;

    switch (mode) {
        case GPIO_MODE_PULLNONE:
            ///< TODO：设置引脚模式为浮空输入
            ret = CSI_OK;
            break;

        case GPIO_MODE_OPEN_DRAIN:
            ///< TODO：设置引脚模式为开漏输出
            ret = CSI_OK;
            break;

        case GPIO_MODE_PUSH_PULL:
            ///< TODO：设置引脚模式为推挽输出
            ret = CSI_OK;
            break;

        case GPIO_MODE_PULLUP:
            ///< TODO：设置引脚模式为上拉输入
            ret = CSI_OK;
            break;

        case GPIO_MODE_PULLDOWN:
            ///< TODO：设置引脚模式为下拉输入
            ret = CSI_OK;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}


/**
  \brief       get the pin function.
  \param[in]   pin       refs to pin_name_e.
  \return      pin function count
*/
pin_func_t csi_pin_get_mux(pin_name_t   pin_name)
{
    ///< TODO：获取引脚功能
    return PIN_FUNC_GPIO;
}

/**
  \brief       set pin speed
  \param[in]   pin_name pin name, defined in soc.h.
  \param[in]   speed    io speed
  \return      error code
*/
csi_error_t csi_pin_speed(pin_name_t pin_name, csi_pin_speed_t speed)
{
    ///< TODO：设置引脚速度
    return CSI_OK;
}


/**
  \brief       set pin drive
  \param[in]   pin_name pin name, defined in soc.h.
  \param[in]   drive    io drive
  \return      error code
*/
csi_error_t csi_pin_drive(pin_name_t pin_name, csi_pin_drive_t drive)
{
    csi_error_t ret = CSI_OK;

    switch (drive) {
        case PIN_DRIVE_LV0:
            ///< TODO：设置引脚的驱动能力为LV0
            break;

        case PIN_DRIVE_LV1:
            ///< TODO：设置引脚的驱动能力为LV1
            break;

        case PIN_DRIVE_LV2:
            ///< TODO：设置引脚的驱动能力为LV2
            break;

        case PIN_DRIVE_LV3:
            ///< TODO：设置引脚的驱动能力为LV3
            break;

        default:
            break;
    }

    return ret;
}
