/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/dma.h>
#include <sys_clk.h>
#include <drv/codec.h>
#include <drv/rtc.h>
#include <soc.h>
#include <aos/cli.h>

csi_rtc_t rtc_hdl;
csi_dma_t dma_hdl;

void board_clk_init(void)
{
    soc_clk_init();
    soc_clk_enable(BUS_UART1_CLK);
}

void board_init(void)
{
    /* some borad preconfig */
    board_clk_init();

    csi_gpio_t gpio_handler;

    csi_gpio_init(&gpio_handler, 0);

    csi_rtc_init(&rtc_hdl, 0);
    csi_dma_init(&dma_hdl, 0);

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
    board_uart_init();
#endif
    board_flash_init();
}
