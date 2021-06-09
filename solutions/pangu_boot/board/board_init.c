/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     board_init.c
 * @brief    CSI Source File for board init
 * @version  V1.0
 * @date     31. June 2018
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <soc.h>
#include <csi_core.h>
#include <board.h>
#include <drv/uart.h>
#include <drv/spiflash.h>
#include <drv/gpio.h>
#include "board_config.h"
#ifdef CONFIG_CSI_V2
#include <drv/pin.h>
#include <drv/tick.h>
#include <sys_clk.h>
#endif

enum {
    PLL720M_240_180 = 0,    // cpu0,cpu1 240M, cpu2 180M
    PLL840M_210_168,
    PLL840M_168_168,
    PLL840M_120_168,
    PLL480M_96_160,
    PLL576M_144_144,
    PLL288M_144_144,
    PLL498M_166_166,
    PLL672M_168_168,
    PLL576M_192_144,
    PLL540M_270_180,
    PLL528M_264_176,
    PLL504M_252_168,
    PLL540M_180_180,
    PLL_CFG_MAX
};
static struct {
    uint32_t reg_a_value;     //0x3000000c
    uint32_t reg_b_value;     //0x30000004
} g_pll_reg[PLL_CFG_MAX] = {
    {0x03c01118, 0x112111},
    {0x04601118, 0x213121},
    {0x04601118, 0x213131},
    {0x04601118, 0x213151},
    {0x02801118, 0x111131},
    {0x03001118, 0x012121},
    {0x01801198, 0x010101},
    {0x05301128, 0x111111},
    {0x03801198, 0x112121},
    {0x03001118, 0x012111},
    {0x02d01118, 0x011101},
    {0x02c01118, 0x011101},
    {0x02a01118, 0x011101},
    {0x02d01118, 0x011111},
};

#define PLL_CONFIG_USE PLL498M_166_166

#define CONFIG_PANGU_16M_SDRAM

static csi_uart_t g_console_handle;
// extern void ioreuse_initial(void);
static void board_pinmux_config(void)
{
#ifdef CONFIG_CSI_V1
    //console
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
#endif

#ifdef CONFIG_CSI_V2
    csi_pin_set_mux(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    csi_pin_set_mux(CONSOLE_RXD, CONSOLE_RXD_FUNC);
#endif
}

void pll_init(void)
{
    // pll_config_t config;
    // config.bypass_ehs_en = 0;
    // config.prescale = 1;
    // config.freq_doubling = 70;
    // config.pll_clk_div1 = 1;
    // config.pll_clk_div2 = 1;
    // yun_pll_config(&config);
    // yun_set_cpu0_cpu1_clk_div(4);
    // yun_set_cpu2_clk_div(5);
    // yun_qspi_clk_div(4);

    *(volatile uint32_t *)0x31000000 = 0x01;
    *(volatile uint32_t *)0x31000004 = 0x500;

    *(volatile uint32_t *)0x3000000c = g_pll_reg[PLL_CONFIG_USE].reg_a_value;
    *(volatile uint32_t *)0x30000070 = 0xffff;
    *(volatile uint32_t *)0x30000014 |= 0x04;

    *(volatile uint32_t *)0x30000004 = g_pll_reg[PLL_CONFIG_USE].reg_b_value;

    while((*(volatile uint32_t *)0x3000000c & 0x01) == 0);
    for (volatile int i = 0; i < 5000; i++);

    *(volatile uint32_t *)0x31000004 = 0x510;
    *(volatile uint32_t *)0x31000000 = 0x0;
    *(volatile uint32_t *)0x31000004 = 0x210;

    // FIXME: for hd test
    // *(volatile uint32_t *)0x3b000000 |= 0x00000018;
    // *(volatile uint32_t *)0x3b000008 &= 0xfffffc3f;
    // *(volatile uint32_t *)0x3b000008 |= 0x000002c0;

    /* 默认为3分频 */
    if (soc_get_cpu_freq(2) == 168000000UL) {
        *(volatile uint32_t *)0x8B000004 = 0x6;   //默认3分频  7分频,支持1.5M波特率
    } else if (soc_get_cpu_freq(0) == 166000000UL) {
        soc_set_apb0_clk_div(6); //2 分频 分频后apb是24mhz
        soc_set_apb1_clk_div(6); //6 分频 分频后apb是32mhz
    } else if (soc_get_cpu_freq(2) == 180000000UL) {
        *(volatile uint32_t *)0x8B000004 = 0x2;   //默认3分频  3分频
    }
}

void sdram_init(void)
{
    // FIXME: sdram init
    *(volatile uint32_t *)0x31000014 = 0x1;
    *(volatile uint32_t *)0x1a000100 = 0x2000000;
#ifdef CONFIG_PANGU_16M_SDRAM
    *(volatile uint32_t *)0x1a000054 = 0x9;
    *(volatile uint32_t *)0x1a000000 = 0x1c1168;
#else
    *(volatile uint32_t *)0x1a000054 = 0x8;
    *(volatile uint32_t *)0x1a000000 = 0x1c0f68;
#endif
#if defined(PLL_210M)   // FIXME: just for test
    *(volatile uint32_t *)0x1a000004 = 0x029e945f;
#else
    *(volatile uint32_t *)0x1a000004 = 0x029e949f;
#endif
    *(volatile uint32_t *)0x1a00000c = 0x3009;

    while ((*(volatile uint32_t *)0x1a00000c) & 0x01);
}

static void flash_init(void)
{

}

static void mpu_init(void)
{
    mpu_region_attr_t attr;

    attr.nx = 0;
    attr.ap = AP_BOTH_RW;
    attr.s = 0;

    csi_mpu_config_region(0, 0U, REGION_SIZE_4GB, attr, 1);
    attr.nx = 1;
    csi_mpu_config_region(1, 0x20000000, REGION_SIZE_512MB, attr, 1);
    csi_mpu_config_region(2, 0x40000000, REGION_SIZE_1GB, attr, 1);
    attr.ap = AP_BOTH_INACCESSIBLE;
    //attention: not protect 0x1A000000, lpm would modify it.
    //csi_mpu_config_region(3, 0x1A000000, REGION_SIZE_4KB, attr, 1);
    csi_mpu_enable();
}

static void pin_driver_strength_init(void)
{
    /* qspi io driver strength: low */

    csi_pin_drive(PA7,PIN_DRIVE_LV0);
    csi_pin_drive(PA8,PIN_DRIVE_LV0);
    csi_pin_drive(PA9,PIN_DRIVE_LV0);
    csi_pin_drive(PA10,PIN_DRIVE_LV0);
    csi_pin_drive(PA11,PIN_DRIVE_LV0);
    csi_pin_drive(PA12,PIN_DRIVE_LV0);

}

#if 0
//FIXME: this function will cause jtag connecting fail
static void pin_pull_init(void)
{
    extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);
    drv_pin_config_mode(PORTA, 30, GPIO_MODE_PULLDOWN);
}
#endif

void board_init(void)
{
    pin_driver_strength_init();
//    pin_pull_init();

    if (PLL_CONFIG_USE == PLL720M_240_180) {
        *(volatile uint32_t *)0x30000014 |= 0x10;  //1.2--��1.3V
    }

    board_pinmux_config();

    *(volatile uint32_t *)0x8b000010=0x01;
    *(volatile uint32_t *)0x93000004=0x100000;
    *(volatile uint32_t *)0x93000000=0x00;

    pll_init();
    soc_clkgate_config();

    /* init the console */
    if (csi_uart_init(&g_console_handle, CONSOLE_UART_IDX)) {

    }

    /* config the UART */
    csi_uart_baud(&g_console_handle, CONFIG_CLI_USART_BAUD);
    csi_uart_format(&g_console_handle, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);

    sdram_init();

    flash_init();

    extern int boot_flash_init(void);
    boot_flash_init();

    mpu_init();
}

int fputc(int ch, FILE *stream)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);

    return 0;
}

void actions_before_application(void)
{
    CACHE->CRCR[1] = 0x0; /* spiflash: noncacheable */
}

