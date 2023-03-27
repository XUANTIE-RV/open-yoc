/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>
#include <aos/aos.h>
#include <aos/cli.h>

extern void cli_reg_cmd_bt_stack(void);
extern void cli_reg_cmd_player(void);

void app_cli_init()
{
#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
    cli_reg_cmd_player();
#endif
}
