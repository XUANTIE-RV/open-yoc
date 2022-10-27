/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/dma.h>
#include <drv/wdt.h>
#include <sys_clk.h>
#include "bl606p_glb.h"

static void board_clock_config(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART2);
    GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0);
}

void board_gpio_init(void)
{
    // csi_gpio_t gpio_handler;

    // soc_clk_init();
    // csi_gpio_init(&gpio_handler, 0);

    GLB_UART_Sig_Swap_Set(GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23, 1);
    GLB_UART_Sig_Swap_Set(GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45, 1);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG1, 0xffffffff);// SIG7-SIG0
    BL_WR_REG(GLB_BASE, GLB_UART_CFG2, 0x0000ffff);// SIG11-SIG8

    /* pin12 ~ pin 23 and pin36 ~ pin45 must +6 to get actual sig num */
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_2, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART0_TXD); // (20 + 6)%12 = 2
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_3, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART0_RXD); // (21 + 6)%12 = 3
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_6, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART2_TXD); // (12 + 6)%12 = 6
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_11, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART2_RXD);// (11)%12 = 11
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_FUN_UART2_TXD, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_6);

    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_UART;

    gpio_cfg.gpioPin = GLB_GPIO_PIN_20;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_21;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_11;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_12;
    GLB_GPIO_Init(&gpio_cfg);
}

void board_pwm_init(void)
{
    /* FIXME: pwm aoshal not support pwm1*/
    csi_pin_set_mux(GPIO_PIN_17, GPIO17_PWM1_CH1P);
    csi_pin_set_mux(GPIO_PIN_18, GPIO18_PWM1_CH2P);
    csi_pin_set_mux(GPIO_PIN_19, GPIO19_PWM1_CH3P);
}

void board_adc_init(void)
{
    csi_pin_set_mux(GPIO_PIN_16, GPIO16_ADC_CH8);
}

void board_dma_init(void)
{
    // csi_dma_t dma;
    // csi_dma_init(&dma, 0);
}

void board_wdt_init(void)
{
    csi_wdt_t wdt;
    csi_wdt_init(&wdt, 0);
    csi_wdt_stop(&wdt);
}

void board_init(void)
{
    board_clock_config();
    board_gpio_init();
    board_dma_init();
    board_wdt_init();
}
