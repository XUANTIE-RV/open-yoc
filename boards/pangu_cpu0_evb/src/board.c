/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#ifdef CONFIG_CSI_V2
#include <drv/pin.h>
#include <sys_clk.h>
#endif

static void board_pinmux_config(void)
{
#ifdef CONFIG_CSI_V1
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
    /** for wifi sdio */
    drv_pinmux_config(PA13, PA13_SDIO_DAT2);
    drv_pinmux_config(PA14, PA14_SDIO_DAT3);
    drv_pinmux_config(PA15, PA15_SDIO_CMD); //ok
    drv_pinmux_config(PA16, PA16_SDIO_CLK);
    drv_pinmux_config(PA17, PA17_SDIO_DAT0);
    drv_pinmux_config(PA18, PA18_SDIO_DAT1);

    drv_pinmux_config(BT_UART_TXD, BT_UART_TXD_FUNC);
    drv_pinmux_config(BT_UART_RXD, BT_UART_RXD_FUNC);
#endif

#ifdef CONFIG_CSI_V2
    csi_pin_set_mux(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    csi_pin_set_mux(CONSOLE_RXD, CONSOLE_RXD_FUNC);
    /** for wifi sdio */
    csi_pin_set_mux(PA13, PA13_SDIO_DAT2);
    csi_pin_set_mux(PA14, PA14_SDIO_DAT3);
    csi_pin_set_mux(PA15, PA15_SDIO_CMD); //ok
    csi_pin_set_mux(PA16, PA16_SDIO_CLK);
    csi_pin_set_mux(PA17, PA17_SDIO_DAT0);
    csi_pin_set_mux(PA18, PA18_SDIO_DAT1);

    csi_pin_set_mux(BT_UART_TXD, BT_UART_TXD_FUNC);
    csi_pin_set_mux(BT_UART_RXD, BT_UART_RXD_FUNC);

    /* clock on */
    soc_clk_enable(RTC_CLK);
    soc_clk_enable(SDIO_CLK);
    soc_clk_enable(SDMMC_CLK);
#endif
}

/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void)
{
    board_pinmux_config();
}
