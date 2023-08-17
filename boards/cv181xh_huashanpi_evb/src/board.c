/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/dma.h>
#include <drv/wdt.h>
#include <sys_clk.h>
#include <mmio.h>
#include "media_video.h"
#include "platform.h"

#include <aos/kernel.h>

csi_dma_t dma_hdl;
void board_dma_init(void)
{
    csi_dma_init(&dma_hdl, 0);
}

void board_clk_init(void)
{
    //soc_clk_init();
    //soc_clk_enable(BUS_UART1_CLK);

    /* adjust uart clock source to 170MHz */
    mmio_write_32(0x30020a8, 0x70109);
}

void board_init(void)
{
    board_clk_init();

    board_dma_init();

    board_uart_init();

    board_flash_init();

    PLATFORM_IoInit();

#ifdef CONFIG_BOARD_VO_OSD
	//load cfg
	PARAM_LoadCfg();
	//media video sys init
	MEDIA_VIDEO_SysInit();
	//custom_evenet_pre
	//media video
	MEDIA_VIDEO_Init();
#endif
}
