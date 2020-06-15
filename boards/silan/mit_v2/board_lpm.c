/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>

#include <pinmux.h>
#include <drv/gpio.h>
#include <yoc/lpm.h>
#include <syscfg.h>
#include <sysctrl.h>
#include <devices/drv_snd_add2010.h>

#include "app_main.h"

#define TAG "blpm"


typedef enum {
    OUTPUT_LOW = 0,
    OUTPUT_HIGH = 1,
    HIGH_RESIST,
    UNCHANGE,
} gpio_state_t;

static gpio_state_t deep_sleep_gpio_stat[] = {
    HIGH_RESIST,    // PA0,
    UNCHANGE,    // PA1 KEY,
    HIGH_RESIST,    // PA2 battery vol,
    UNCHANGE,    // PA3 KEY,
    UNCHANGE,    // PA4 UART2,
    UNCHANGE,    // PA5 UART2,
    HIGH_RESIST,    // PA6 Wifi Wake host,
    HIGH_RESIST,    // PA7,
    HIGH_RESIST,    // PB0 UART1-wifi,
    HIGH_RESIST,    // PB1 UART1-wifi,
    HIGH_RESIST,    // PB2,
    HIGH_RESIST,    // PB3,
    HIGH_RESIST,    // PB4 PCM I2S,
    HIGH_RESIST,    // PB5 PCM I2S,
    OUTPUT_LOW,     // PB6 LDO power control,
    HIGH_RESIST,    // PB7 PCM I2S,
    HIGH_RESIST,    // PB8 touch pad,
    OUTPUT_LOW,     // PC0 WIFI_POWER !!,
    HIGH_RESIST,    // PC1 Wifi SDIO,
    HIGH_RESIST,    // PC2 Wifi SDIO,
    HIGH_RESIST,    // PC3 Wifi SDIO,
    HIGH_RESIST,    // PC4 Wifi SDIO,
    HIGH_RESIST,    // PC5 Wifi SDIO,
    HIGH_RESIST,    // PC6 Wifi SDIO,
    OUTPUT_LOW,     // PC7 BLE_EN !!,
    OUTPUT_LOW,     // PC8 WIFI_EN !!,
    HIGH_RESIST,    // PC9 TF D0,
    HIGH_RESIST,    // PC10 TF CLK,
    HIGH_RESIST,    // PC11 TF CMD,
    HIGH_RESIST,    // PC12 I2C,
    HIGH_RESIST,    // PC13 I2C,
    HIGH_RESIST,    // PD0 JTAG,
    OUTPUT_LOW,     // PD1 AW9523 RSTN !!,
    HIGH_RESIST,    // PD2 JTAG,
    HIGH_RESIST,    // PD3 JTAG,
    HIGH_RESIST,    // PD4 JTAG,
    HIGH_RESIST,    // PD5,
    HIGH_RESIST,    // PD6,
    OUTPUT_LOW,     // PD7 PA_MUTE !!,
    HIGH_RESIST,    // PD8,
    HIGH_RESIST,    // PD9,
    HIGH_RESIST     // PD10
};

static gpio_state_t lpm_gpio_stat[] = {
    HIGH_RESIST,    // PA0,
    UNCHANGE,       // PA1 KEY,
    UNCHANGE,       // PA2 battery vol,
    UNCHANGE,       // PA3 KEY,
    UNCHANGE,       // PA4 UART2,
    UNCHANGE,       // PA5 UART2,
    HIGH_RESIST,    // PA6 Wifi Wake host,
    HIGH_RESIST,    // PA7,
    HIGH_RESIST,    // PB0 UART1-wifi,
    HIGH_RESIST,    // PB1 UART1-wifi,
    HIGH_RESIST,    // PB2,
    HIGH_RESIST,    // PB3,
    HIGH_RESIST,    // PB4 PCM I2S,
    HIGH_RESIST,    // PB5 PCM I2S,
    UNCHANGE,       // PB6 LDO power control,
    HIGH_RESIST,    // PB7 PCM I2S,
    HIGH_RESIST,    // PB8 touch pad,
    UNCHANGE,       // PC0 WIFI_POWER !!,
    UNCHANGE,       // PC1 Wifi SDIO,
    UNCHANGE,       // PC2 Wifi SDIO,
    UNCHANGE,       // PC3 Wifi SDIO,
    UNCHANGE,       // PC4 Wifi SDIO,
    UNCHANGE,       // PC5 Wifi SDIO,
    UNCHANGE,       // PC6 Wifi SDIO,
    OUTPUT_LOW,     // PC7 BLE_EN !!,
    UNCHANGE,       // PC8 WIFI_EN !!,
    UNCHANGE,       // PC9 TF D0,
    UNCHANGE,       // PC10 TF CLK,
    UNCHANGE,       // PC11 TF CMD,
    UNCHANGE,       // PC12 I2C,
    UNCHANGE,       // PC13 I2C,
    HIGH_RESIST,    // PD0 JTAG,
    OUTPUT_LOW,     // PD1 AW9523 RSTN !!,
    HIGH_RESIST,    // PD2 JTAG,
    HIGH_RESIST,    // PD3 JTAG,
    HIGH_RESIST,    // PD4 JTAG,
    HIGH_RESIST,    // PD5,
    HIGH_RESIST,    // PD6,
    OUTPUT_LOW,     // PD7 PA_MUTE !!,
    HIGH_RESIST,    // PD8 WL WAKE HOST,
    HIGH_RESIST,    // PD9,
    HIGH_RESIST     // PD10
};

static void set_gpio_lpm_state(gpio_state_t *stat_map)
{
    /* put all idle gpio to right state */
    gpio_pin_handle_t pin_hdl;
    for(int i = PA0; i <= PD10; i++) {
        if (stat_map[i] == UNCHANGE) {
            continue;
        }

        drv_pinmux_config(i, PIN_FUNC_GPIO);
        pin_hdl = csi_gpio_pin_initialize(i, NULL);

        switch (stat_map[i]) {
            case OUTPUT_LOW:
            case OUTPUT_HIGH:
                csi_gpio_pin_config_mode(pin_hdl, GPIO_MODE_PUSH_PULL);
                csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_OUTPUT);
                csi_gpio_pin_write(pin_hdl, stat_map[i]);
                break;

            case HIGH_RESIST:
                csi_gpio_pin_config_mode(pin_hdl, GPIO_MODE_PULLNONE);
                csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_INPUT);            
                break;

            default:
                break;
        }

        csi_gpio_pin_uninitialize(pin_hdl);
    }
}

void board_enter_lpm(pm_policy_t policy)
{
#ifdef BOARD_MIT_V3
            app_subboard_ldo_enable(0);
#else
            aw2013_low_power(&g_i2c_dev);
            aw9523_shutdown();
#endif

    switch(policy) {
        case LPM_POLICY_LOW_POWER:
            set_gpio_lpm_state(lpm_gpio_stat);
            break;

        case LPM_POLICY_DEEP_SLEEP:
            set_gpio_lpm_state(deep_sleep_gpio_stat);
            break;

        default: break;
    }
}

void board_leave_lpm(pm_policy_t policy)
{
    switch(policy) {
        case LPM_POLICY_LOW_POWER:
                drv_pinmux_config(PB4, PB4_PCM_I2S_SCLK);
                drv_pinmux_config(PB5, PB5_PCM_I2S_WS);
                drv_pinmux_config(PB7, PB7_PCM_I2S_SDO);
            break;

        case LPM_POLICY_DEEP_SLEEP:
            break;

        default: break;
    }    

#ifdef BOARD_MIT_V3
    app_subboard_ldo_enable(1);
    add2010_init(&g_i2c_dev);
#endif

    aw2013_init(&g_i2c_dev);
    aw9523_init(&g_i2c_dev);
}
