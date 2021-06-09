#ifndef __APP_INIT_H
#define __APP_INIT_H
#include <yoc/netmgr_service.h>
#include <uservice/uservice.h>

extern netmgr_hdl_t app_netmgr_hdl;
void board_cli_init();
void board_yoc_init(void);
void cli_reg_cmd_player(void);

void _player(char *url);
void _player_stop(void);
#endif
