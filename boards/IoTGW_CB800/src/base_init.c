/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <devices/devicelist.h>

void board_base_init(void)
{
    uart_csky_register(0); /* UART1 */
    uart_csky_register(1); /* UART2 */
    uart_csky_register(4); /* UART4 */
    return;
}
