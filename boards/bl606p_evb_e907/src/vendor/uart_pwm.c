#if 0
/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include "aos/hal/uart.h"
#include "aos/hal/pwm.h"
#include <bl606p_gpio.h>
#include <bl606p_glb.h>
#include <drv/pin.h>
#include <soc.h>

static uart_dev_t _uart;
static pwm_dev_t pwm;

static int gpio_uart_func_check(uint32_t regval1, uint32_t regval2)
{
    uint8_t uart_func[12];

    for (uint8_t i = 0; i < 8; i++) {
        uart_func[i] = (regval1 >> (i * 4)) & 0x0f;
    }

    for (uint8_t i = 8; i < 12; i++) {
        uart_func[i] = (regval2 >> ((i - 8) * 4)) & 0x0f;
    }

    for (uint8_t j = 0; j < 11; j++) {
        for (uint8_t k = j + 1; k < 12; k++) {
            if (uart_func[j] == uart_func[k] && (uart_func[j] != 0x0f)) {
                return -1;
            }
        }
    }
    return 0;
}

void uart_gpio_init(uint8_t pin, GLB_UART_SIG_FUN_Type uart_func)
{
    GLB_GPIO_Cfg_Type gpio_cfg;
    uint8_t sig = 0;

    gpio_cfg.gpioFun = GPIO_FUN_UART;
    gpio_cfg.gpioPin = pin;
    gpio_cfg.drive = 1;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.outputMode = 0;

    if ((pin >= GLB_GPIO_PIN_12 && pin <= GLB_GPIO_PIN_23) ||
        (pin >= GLB_GPIO_PIN_36 && pin <= GLB_GPIO_PIN_45)) {
        sig = (pin + 6) % 12;
    } else {
        sig = pin % 12;
    }

    GLB_UART_Fun_Sel(sig, uart_func);

    if (gpio_uart_func_check(BL_RD_REG(GLB_BASE, GLB_UART_CFG1), BL_RD_REG(GLB_BASE, GLB_UART_CFG2)) == -1) {
        while (1) {
        }
    }
    GLB_GPIO_Init(&gpio_cfg);
}

static void uart1_task(void *arg)
{   
    uint8_t data[1024];
    uint32_t recv_size = 0;

    while (1) {
        memset(data, 0, 1024);
        hal_uart_recv_II(&_uart, data, 1024, &recv_size, 100);       
        if (recv_size) {
            hal_uart_send(&_uart, data, recv_size, 100);
        }
        aos_msleep(50);
    }
}

static void cmd_peripherals(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;
    
    printf(" hal_uart1_app_static_out start\r\n");
    
    uart_gpio_init(16, GLB_UART_SIG_FUN_UART1_TXD);
    uart_gpio_init(17, GLB_UART_SIG_FUN_UART1_RXD);
    
    _uart.port                = 1;
    _uart.config.baud_rate    = 2000000;
    _uart.config.mode         = MODE_TX_RX;
    _uart.config.flow_control = FLOW_CONTROL_DISABLED;
    _uart.config.stop_bits    = STOP_BITS_1;
    _uart.config.parity       = NO_PARITY;
    _uart.config.data_width   = DATA_WIDTH_8BIT;
    
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART1);
    ret = hal_uart_init(&_uart);   
    if (ret) {
        printf("uart1 init error\r\n");
        return;
    }

    aos_task_new( "uart1 task", uart1_task, NULL, 4096);
    
    printf(" hal_pwm_app_static_out start\r\n");
    
    pwm.port = 0;
    pwm.config.freq = 1000;
    pwm.config.duty_cycle = 0.5;
    pwm.priv = NULL;

    csi_pin_set_mux(GPIO_PIN_3, GPIO3_PWM0_CH3P);
    ret = hal_pwm_init(&pwm);
    if(ret){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return;
    }

    hal_pwm_start(&pwm);
}

void cli_reg_cmd_peripherals(void)
{
    static const struct cli_command cmd_list[] = {
        {"uart_pwm", "uart1 pwm test", cmd_peripherals},
    };

    aos_cli_register_commands(cmd_list, sizeof(cmd_list)/sizeof(struct cli_command));
}
#endif
