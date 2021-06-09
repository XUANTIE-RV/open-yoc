#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__


#include <yoc/netmgr.h>

#include <app_config.h>
#include <unistd.h>
#include <string.h>
#include <devices/wifi.h>

#include <devices/netdrv.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <aos/kv.h>
#include <aos/aos.h>
#include <yoc/partition.h>

#include <devices/devicelist.h>
#include <board_config.h>

///>用户修改驱动头文件
#include <devices/rtl8723ds.h>

///>用户修改网络注册函数
void test_wifi_register(void);
void board_cli_init(void);

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
