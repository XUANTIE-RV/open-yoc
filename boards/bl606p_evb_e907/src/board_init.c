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
#include <blyoc_flash.h>
#include <hal_code/hal_board.h>

#include <aos/yloop.h>
#include <aos/kernel.h>

static void board_clock_config(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART2);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART1);
    GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0);
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

#if 0
//c906_uart_init(2);
void c906_uart_init(int id)
{
    static csi_uart_t uart_c906;
    csi_uart_init(&uart_c906, id);
}
#endif


static void yloop_thread(void *arg)
{
    aos_loop_run();
}

void board_init(void)
{
    GLB_Set_PSram_CLK(1,1,0,1); //set psram clock 240M Hz

    board_clock_config();

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

    board_dma_init();
    board_wdt_init();

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
    board_uart_init();
#endif
	board_spi_init();

    board_flash_init();
    bl_flash_clear_status_register_for_winbond();
    hal_board_cfg(0);

    /* start yloop, wifi driver deps */
    aos_loop_init();
    static aos_task_t task_handle;
    aos_task_new_ext(&task_handle, "yloop", yloop_thread, NULL, 6 * 1024, AOS_DEFAULT_APP_PRI);
}
