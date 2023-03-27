/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY > 0
#include <stdio.h>
#include <drv/pin.h>
#include <drv/d1_display.h>

void board_display_init(void)
{
    csi_pin_set_mux(PG14, PIN_FUNC_GPIO);
    csi_pin_set_mux(PG15, PIN_FUNC_GPIO);

    d1_touch_config_t config = {
        .iic_addr = 0x5D,
        .iic_port = 2,
        .int_pin  = PG14,
        .rst_pin  = PG15,
    };

    drv_d1_display_register();

    drv_d1_gt9xx_touch_register(&config);
}
#endif
