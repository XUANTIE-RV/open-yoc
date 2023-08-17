/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/dma.h>
#include <sys_clk.h>
#include <pinctrl-mars.h>

// void board_clk_init(void)
// {
//     //soc_clk_init();
//     //soc_clk_enable(BUS_UART1_CLK);

//     /* adjust uart clock source to 170MHz */
//     mmio_write_32(0x30020a8, 0x70109);
// }

void board_init(void)
{
    // board_clk_init();
    // Enable JTAG
    PINMUX_CONFIG(IIC0_SCL, CV_SCL0__CR_4WTDI);
    PINMUX_CONFIG(IIC0_SDA, CV_SDA0__CR_4WTDO);
    PINMUX_CONFIG(JTAG_CPU_TMS, CV_2WTMS_CR_4WTMS);
    PINMUX_CONFIG(JTAG_CPU_TCK, CV_2WTCK_CR_4WTCK);
    PINMUX_CONFIG(JTAG_CPU_TRST, JTAG_CPU_TRST);
}
