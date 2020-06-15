#ifndef __APP_INIT_H
#define __APP_INIT_H
#include <yoc/netmgr_service.h>

extern netmgr_hdl_t app_netmgr_hdl;
void board_cli_init(utask_t *task);
void board_yoc_init(void);

#endif
