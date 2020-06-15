#ifndef __APP_INIT_H
#define __APP_INIT_H
#include <yoc/netmgr.h>
#include <yoc/uservice.h>

void board_cli_init(utask_t *task);
void board_yoc_init(void);
#endif
