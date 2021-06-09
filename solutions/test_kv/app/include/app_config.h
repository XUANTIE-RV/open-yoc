#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <yoc/netmgr.h>
#include <aos/kv.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <board.h>
#include <stdio.h>
#include <devices/console_uart.h>
#include <yoc/partition.h>

#include <uservice/uservice.h>

void board_cli_init(void);
void board_yoc_init(void);

#define EVENT_DATA_REPORT  (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)
#define DATA_PUSH_INTERVAL (5 * 1000)
#define DATA_PUSH_EVENT_ID (EVENT_USER + 1)


/* Allow LOGD function to output log information */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 1
#endif

#endif
