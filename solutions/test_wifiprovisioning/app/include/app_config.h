#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <pin_name.h>
#include <yoc/netmgr.h>
#define CONSOLE_IDX 0
#define EVENT_DATA_REPORT  (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)
#define DATA_PUSH_INTERVAL (5 * 1000)
#define DATA_PUSH_EVENT_ID (EVENT_USER + 1)


/* Allow LOGD function to output log information */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 1
#endif

/* You can add user configuration items here. */

//#define CONFIG_WIFI_SSID "o"
//#define CONFIG_WIFI_PSK "12345678"

#endif
