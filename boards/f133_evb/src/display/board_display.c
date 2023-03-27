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

    // Touch IIC
	csi_pin_set_mux(PE12, PE12_TWI2_SCK);
    csi_pin_set_mux(PE13, PE13_TWI2_SDA);

    d1_touch_config_t config = {
        .iic_addr = 0x48,
        .iic_port = 2,
        .int_pin  = PB2,
        .rst_pin  = PD16,
    };

    drv_d1_480p_display_register();

    drv_d1_ft6336_480p_touch_register(&config);
}
#endif
