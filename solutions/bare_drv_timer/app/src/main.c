/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern int example_timer(uint8_t timer_num);
int main(void)
{
    board_init();
    printf("bare_drv_timer demo start!\r\n");
    return example_timer(1);
}
