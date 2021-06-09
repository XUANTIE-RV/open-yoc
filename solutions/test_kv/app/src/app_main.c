/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>
#define TAG "app"

extern void board_yoc_init(void);
int main()
{
    board_yoc_init();
	return 0;
}

