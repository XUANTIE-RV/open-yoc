/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>

/**
 * @brief 板级厂商初始化，用于扩展未定义board_xxx_init等外设的初始化
 *        或板级需要测试验证的命令行初始化
 *        支持改函数的解决方案，在board_yoc_init函数的末尾调用
 */
extern void wifidrv_register_iwpriv_cmd(void);
extern void cli_reg_cmd_bt_stack(void);
extern void cli_reg_cmd_vendor(void);

void board_vendor_init()
{
#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT
    cli_reg_cmd_bt_stack();
#endif
#if defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI
    wifidrv_register_iwpriv_cmd();
#endif
#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR
    cli_reg_cmd_vendor();
#endif
}
