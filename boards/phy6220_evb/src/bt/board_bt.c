/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>

void board_bt_init(void)
{
    extern int hci_driver_phy6220_register(int idx);
    extern int hci_h4_driver_init();
    hci_driver_phy6220_register(0);
    hci_h4_driver_init();
}
