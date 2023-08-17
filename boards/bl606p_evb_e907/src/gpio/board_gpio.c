/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
#include <stdlib.h>
#include <stdio.h>

#include <sys_clk.h>
#include "bl606p_glb.h"

void board_gpio_pin_init(void)
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
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_6, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART2_RXD); // (12 + 6)%12 = 6
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_11, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART2_TXD);// (11)%12 = 11

    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_0, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART1_RXD);
    GLB_UART_Fun_Sel((GLB_UART_SIG_Type)GLB_UART_SIG_1, (GLB_UART_SIG_FUN_Type)GLB_UART_SIG_FUN_UART1_TXD);

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

    gpio_cfg.gpioPin = GLB_GPIO_PIN_0;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_1;
    GLB_GPIO_Init(&gpio_cfg);

	/* Config for spi */
    gpio_cfg.gpioFun = GPIO_FUN_SPI0;
    gpio_cfg.gpioPin = GLB_GPIO_PIN_25;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_26;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_27;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_28;
    GLB_GPIO_Init(&gpio_cfg);
}
#endif
