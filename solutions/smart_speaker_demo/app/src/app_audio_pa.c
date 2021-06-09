/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"

#define TAG "app"

void app_audio_pa_ctrl(int enabled)
{
#ifdef PIN_PA_EN
    if (enabled < 0) {
        return;
    }

    gpio_pin_handle_t pin_hdl = csi_gpio_pin_initialize(PIN_PA_EN, NULL);

    if (pin_hdl == NULL) {
        LOGE(TAG, "gpio init error");
        return;
    }

    csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pin_hdl, enabled);
    csi_gpio_pin_uninitialize(pin_hdl);
#endif

}
int app_subboard_ldo_enable(int enabled)
{
#ifdef PIN_SUBBOARD_LDO_ENABLE
    static gpio_pin_handle_t hdl;

    if (!hdl) {
        drv_pinmux_config(PIN_SUBBOARD_LDO_ENABLE, PIN_FUNC_GPIO);
        hdl = csi_gpio_pin_initialize(PIN_SUBBOARD_LDO_ENABLE, NULL);
        CHECK_RET_TAG_WITH_RET(hdl != NULL, -1);
    }

    csi_gpio_pin_config_mode(hdl, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(hdl, GPIO_DIRECTION_OUTPUT);

    if (enabled) {
        csi_gpio_pin_write(hdl, 1);
    } else {
        csi_gpio_pin_write(hdl, 0);
    }

    aos_msleep(5);
#endif
    return 0;
}
