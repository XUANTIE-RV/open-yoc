/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <board_config.h>
#include "drv/gpio.h"
#include "pin_name.h"
#include "pinmux.h"

#define TAG "board_factory"

int board_ftmode_check(void)
{
    gpio_pin_handle_t pin_hd;
    bool pin_val = 0;

    drv_pinmux_config(PA7, PIN_FUNC_GPIO);
    pin_hd = csi_gpio_pin_initialize(PA7, NULL);

    if (pin_hd == NULL) {
        LOGE(TAG, "init gpio fail");
        return -1;
    }

    csi_gpio_pin_config_direction(pin_hd, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_config_mode(pin_hd, GPIO_MODE_PULLDOWN);
    csi_gpio_pin_read(pin_hd, &pin_val);

    if (pin_val == 1) {
        LOGI(TAG, "enter factory mode");
        return 1;
    } else {
        LOGI(TAG, "enter nomal mode");
        return 0;
    }
}

int board_gpio_test(void)
{
    gpio_pin_handle_t     handle1, handle2;

    drv_pinmux_config(PA1, PIN_FUNC_GPIO);
    drv_pinmux_config(PA4, PIN_FUNC_GPIO);
    handle1 = csi_gpio_pin_initialize(PA1, NULL);
    handle2 = csi_gpio_pin_initialize(PA4, NULL);

    if ((handle1 == NULL) || (handle2 == NULL)) {
        LOGE(TAG, "gpio init fail");
        return -1;
    }

    csi_gpio_pin_config_mode(handle1, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(handle1, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(handle1, 1);

    aos_msleep(100);

    bool bval = false;
    csi_gpio_pin_config_mode(handle2, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(handle2, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_read(handle2, &bval);

    if (bval != 1) {
        LOGE(TAG, "gpio read err");
        return -1;
    }

    csi_gpio_pin_write(handle1, 0);
    aos_msleep(100);
    csi_gpio_pin_read(handle2, &bval);

    if (bval != 0) {
        LOGE(TAG, "gpio read err");
        return -1;
    }

    csi_gpio_pin_uninitialize(handle1);
    csi_gpio_pin_uninitialize(handle2);

    drv_pinmux_config(PB9, PIN_FUNC_GPIO);
    drv_pinmux_config(PB10, PIN_FUNC_GPIO);
    handle1 = csi_gpio_pin_initialize(PB9, NULL);
    handle2 = csi_gpio_pin_initialize(PB10, NULL);

    if ((handle1 == NULL) || (handle2 == NULL)) {
        LOGE(TAG, "gpio init fail");
        return -1;
    }

    csi_gpio_pin_config_mode(handle1, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(handle1, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(handle1, 1);

    aos_msleep(100);

    bval = false;
    csi_gpio_pin_config_mode(handle2, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(handle2, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_read(handle2, &bval);

    if (bval != 1) {
        LOGE(TAG, "gpio read err");
        return -1;
    }

    csi_gpio_pin_write(handle1, 0);
    aos_msleep(100);
    csi_gpio_pin_read(handle2, &bval);

    if (bval != 0) {
        LOGE(TAG, "gpio read err");
        return -1;
    }

    csi_gpio_pin_uninitialize(handle1);
    csi_gpio_pin_uninitialize(handle2);

    return 0;
}


#define PSRAM_TEST_VALUE 0xA5

int board_ex_sram_test(void)
{
    uint32_t addr = 0x30000000;
    int i = 0;

    memset((uint8_t *)addr, PSRAM_TEST_VALUE, 0x1000);

    for (i = 0; i < 0x1000; i++) {
        if (*(uint8_t *)(addr + i) != PSRAM_TEST_VALUE) {
            LOGI(TAG, "read val %#x", *(uint8_t *)(addr + i));
            return -1;
        }
    }

    addr = 0x30400000;
    memset((uint8_t *)addr, PSRAM_TEST_VALUE, 0x1000);

    for (i = 0; i < 0x1000; i++) {
        if (*(uint8_t *)(addr + i) != PSRAM_TEST_VALUE) {
            LOGI(TAG, "read val %#x", *(uint8_t *)(addr + i));
            return -1;
        }
    }

    addr = 0x307FF000;
    memset((uint8_t *)addr, PSRAM_TEST_VALUE, 0x1000);

    for (i = 0; i < 0x1000; i++) {
        if (*(uint8_t *)(addr + i) != PSRAM_TEST_VALUE) {
            LOGI(TAG, "read val %#x", *(uint8_t *)(addr + i));
            return -1;
        }
    }

    return 0;
}

/********************************************************************************/
/* for fct test end
********************************************************************************/


