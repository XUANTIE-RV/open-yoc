/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <stdbool.h>
#include <uservice/uservice.h>
#include <aos/cli.h>
#include <aos/console_uart.h>
#include <drv/uart.h>
#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif

#define TAG "init"

static void stduart_init(void)
{
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();

    stduart_init();
    aos_cli_init();

    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#if defined(CONFIG_COMP_LOG_IPC) && CONFIG_COMP_LOG_IPC
    board_logipc_init();
#endif

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM
#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
    //board_pwm_init();
#endif
#endif

#if defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC
#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
    //board_adc_init();
#endif
#endif

#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON
#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0
    //board_button_init();
#endif
#endif

#if defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED
#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
    //board_led_init();
#endif
#endif

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR
    board_vendor_init();
#endif

}
