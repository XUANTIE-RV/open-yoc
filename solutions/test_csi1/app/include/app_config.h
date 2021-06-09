#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__
#include <board.h>
/*
 *"wiz_config.h" file is used for CDK system configuration.
 *Makefile compilation needs to keep the file empty.
*/
//#include "wiz_config.h"

/* Allow LOGD function to output log information */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 1
#endif

/* You can add user configuration items here. */

//#define CONFIG_WIFI_SSID "o"
//#define CONFIG_WIFI_PSK "12345678"

//#define CONFIG_WIFI_SSID "OS-TEST"
//#define CONFIG_WIFI_PSK "TEST1234"


#endif
