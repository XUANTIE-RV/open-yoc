/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __APP_INIT_H
#define __APP_INIT_H

void board_cli_init();
void board_yoc_init(void);
void led_pinmux_init(void);
void led_refresh(void);
void marquee_test(void);

#endif
