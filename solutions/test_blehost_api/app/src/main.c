/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "app_init.h"
#define TAG "app"

extern void board_base_init(void);

int main()
{
    board_base_init();
    board_yoc_init();
	return 0;
}

