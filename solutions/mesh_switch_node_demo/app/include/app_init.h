/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __APP_INIT_H
#define __APP_INIT_H

#define DEVICE_NAME "YoC Mesh Switch"

#define SWITCH_DEV_UUID                                                                                                \
    {                                                                                                                  \
        0xcf, 0xa0, 0xea, 0x72, 0x17, 0xd9, 0x11, 0xe8, 0x86, 0xd1, 0x5f, 0x1c, 0xe2, 0x8a, 0xdf, 0x00                 \
    }

/* Board hardware and yoc modules initilization */
void board_yoc_init(void);
void board_cli_init();

/* Switch the on off state for next round operation */
void prepare_onoff_state(uint8_t onoff);

#endif
