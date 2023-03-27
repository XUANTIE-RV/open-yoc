#ifndef __APP_INIT_H
#define __APP_INIT_H

#define DEVICE_NAME "YoC Light Ext NODE"

#define LIGHT_DEV_UUID                                                                                                 \
    {                                                                                                                  \
        0xcf, 0xa0, 0xe3, 0x7e, 0x17, 0xd9, 0x11, 0xe8, 0x86, 0xd1, 0x5f, 0x1c, 0xe2, 0x8a, 0xde, 0x02                 \
    }

void board_cli_init();
void board_yoc_init(void);

#endif
