/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>

#include <devices/rtl8723ds_bt.h>
#include <drv/pin.h>

void board_bt_init(void)
{
    csi_pin_set_mux(PG6, PG6_UART1_TX);
    csi_pin_set_mux(PG7, PG7_UART1_RX);

    csi_pin_set_mux(PG8, PG8_UART1_RTS);
    csi_pin_set_mux(PG9, PG9_UART1_CTS);

    csi_pin_set_mux(PG18, PIN_FUNC_GPIO);

    rtl8723ds_bt_config config = {
        .uart_id    = 1,
        .bt_dis_pin = PG15,
    };

    bt_rtl8723ds_register(&config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
}
#endif
