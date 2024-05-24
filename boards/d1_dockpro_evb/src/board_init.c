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

#ifdef CONFIG_UTEST
    extern int utest_init(void);
    utest_init();
#endif /* CONFIG_UTEST */
}

#if defined(CONFIG_KERNEL_RTTHREAD)
#include <rthw.h>
#include <rtthread.h>
#include <devices/uart.h>
#include <aos/console_uart.h>

extern unsigned long __heap_start;
extern unsigned long __heap_end;

void rt_hw_board_init(void)
{
    /* initalize interrupt */
    rt_hw_interrupt_init();

#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_system_heap_init((void *)&__heap_start, (void *)&__heap_end);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

extern int32_t aos_debug_printf(const char *fmt, ...);
void rt_hw_console_output(const char *str)
{
    aos_debug_printf("\r%s", str);
}

char rt_hw_console_getchar(void)
{
    int ch = -1;

    while (rvm_hal_uart_recv(console_get_uart(), &ch, 1, -1) != 1);

    return ch;
}
#endif /* CONFIG_KERNEL_RTTHREAD */
