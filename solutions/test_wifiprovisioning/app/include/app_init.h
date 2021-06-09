#ifndef __APP_INIT_H
#define __APP_INIT_H
#include <uservice/uservice.h>
#include <yoc/netmgr.h>
extern netmgr_hdl_t app_netmgr_hdl;

void board_cli_init(void);
void board_yoc_init(void);

#endif
