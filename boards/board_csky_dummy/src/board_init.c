/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>

void board_init(void)
{
    /* some borad preconfig */
    // board_xxx();

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
    board_uart_init();
#endif

#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
    board_pwm_init();
#endif

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
    board_adc_init();
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0
    board_button_init();
#endif

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
    board_led_init();
#endif

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT > 0
    board_audio_init();
#endif
}
