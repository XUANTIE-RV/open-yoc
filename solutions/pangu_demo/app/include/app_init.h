#ifndef __APP_INIT_H
#define __APP_INIT_H
#include <yoc/netmgr.h>
#include <yoc/uservice.h>

extern netmgr_hdl_t app_netmgr_hdl;
void board_cli_init(utask_t *task);
void board_yoc_init(void);
void cli_reg_cmd_player(void);

void _player(char *url);
void _player_stop(void);
#endif
