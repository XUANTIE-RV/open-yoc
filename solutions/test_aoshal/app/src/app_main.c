/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <ulog/ulog.h>
#include <aos/kernel.h>

#include <drv/gpio.h>
#include <soc.h>
// #include <pinmux.h>
#include "app_main.h"
#include <yoc/atserver.h>

#define TAG "led"

#define AT_BACK_BOOT()                        printf("\r\nBOOT_IND\r\n")

extern void at_cmd_callback(char *cmd, int type, char *data);
extern void test_hal_rtc(char *cmd, int type, char *data);
extern void test_hal_timer(char *cmd, int type, char *data);
extern void test_hal_spiflash(char *cmd, int type, char *data);
extern void test_hal_spi_master(char *cmd, int type, char *data);
extern void test_hal_spi_slave(char *cmd, int type, char *data);
extern void test_hal_pwm(char *cmd, int type, char *data);
extern void test_hal_pwm_b(char *cmd, int type, char *data);
extern void test_hal_wdt(char *cmd, int type, char *data);
// extern void test_hal_adc(char *cmd, int type, char *data);
extern void test_hal_iic_master(char *cmd, int type, char *data);
extern void test_hal_iic_slave(char *cmd, int type, char *data);
extern void test_hal_gpio(char *cmd, int type, char *data);
extern void test_hal_rng(char *cmd, int type, char *data);
extern void test_hal_uart_slaver(char *cmd, int type, char *data);
extern void test_hal_uart_master(char *cmd, int type, char *data);

#define AT {"AT", at_cmd_callback}
#define AT_HELP {"AT+HELP", at_cmd_callback}
// #define AT_ADC {"AT+ADC", test_hal_adc}
#define AT_RTC {"AT+RTC", test_hal_rtc}
#define AT_TIMER {"AT+TIMER", test_hal_timer}
#define AT_SPIFLASH {"AT+SPIFLASH", test_hal_spiflash}
#define AT_SPI_MASTER {"AT+SPI_MASTER", test_hal_spi_master}
#define AT_SPI_SLAVE {"AT+SPI_SLAVE", test_hal_spi_slave}
#define AT_PWM {"AT+PWM", test_hal_pwm}
#define AT_PWM_B {"AT+PWM_B", test_hal_pwm_b}
#define AT_WDT {"AT+WDT", test_hal_wdt}
#define AT_I2C_MASTER {"AT+I2C_MASTER", test_hal_iic_master}
#define AT_I2C_SLAVER {"AT+I2C_SLAVER", test_hal_iic_slave}
#define AT_UART_SLAVER {"AT+UART_SLAVER", test_hal_uart_slaver}
#define AT_UART_MASTER {"AT+UART_MASTER", test_hal_uart_master}
#define AT_GPIO {"AT+GPIO", test_hal_gpio}
#define AT_RNG {"AT+RNG", test_hal_rng}

const atserver_cmd_t at_cmd[] = {
        AT,
        AT_HELP,
        // AT_ADC,
        AT_RTC,
        AT_TIMER,
        AT_SPIFLASH,
        AT_SPI_MASTER,
        AT_SPI_SLAVE,
        AT_PWM,
        AT_PWM_B,
        AT_WDT,
        AT_I2C_MASTER,
        AT_I2C_SLAVER,
        AT_UART_MASTER,
        AT_UART_SLAVER,
        AT_GPIO,
        AT_RNG,

};

void main()
{
    board_yoc_init();
    LOGI(TAG, "enter test\n");

    int gpio_val = 0;
    int ret = 0;
    
    ret = atserver_add_command(at_cmd);
    if (ret == 0){
        LOGD(TAG,"add command success!");
    }else{
        LOGD(TAG,"add command failed!");
    }  

    AT_BACK_BOOT();
}
