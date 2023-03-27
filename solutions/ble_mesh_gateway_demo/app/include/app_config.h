#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "board.h"

/*
 *"wiz_config.h" file is used for CDK system configuration.
 *Makefile compilation needs to keep the file empty.
 */
//#include "wiz_config.h"

/* Allow LOGD function to output log information */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 1
#endif

#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
/* You can add user configuration items here. */
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
#define CONFIG_WIFI_SMARTLIVING 1 /* smartliving provisioning */
#define CONFIG_SMARTLIVING_MQTT 1 /* smartliving mqtt subscribe and publish */
#define CONFIG_SMARTLIVING_DEMO 1 /* smartliving demo */
#if defined(CONFIG_ERS_CONNECTIVITY) && CONFIG_ERS_CONNECTIVITY
#define CONFIG_SMARTLIVING_AT_MODULE 1 /* smartliving at module */
#else
#define CONFIG_SMARTLIVING_AT_MODULE 0 /* smartliving at module */
#endif
#define CONFIG_SMARTLIVING_OTA 1
#define CONFIG_YOC_SOFTAP_PROV 0 /* YOC provisioning*/
#define CONFIG_WIFI_PROV_BLE   1 /* BLE for wifi provisioning */
#else
#define CONFIG_YOC_SOFTAP_PROV 1 /* YOC provisioning*/
#endif
#else
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
#error "using smartliving should enable network first"
#endif
#endif

#if defined(CONFIG_SAVE_JS_TO_RAM) && CONFIG_SAVE_JS_TO_RAM
#if !defined(CONFIG_OCC_AUTH) || !CONFIG_OCC_AUTH
#error "using js should enable CONFIG_OCC_AUTH first"
#endif
#endif

#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
#if !defined(CONFIG_OCC_AUTH) || !CONFIG_OCC_AUTH
#error "using GW OCC Fota should enable CONFIG_OCC_AUTH first"
#endif
#if !defined(CONFIG_GW_NETWORK_SUPPORT) || !CONFIG_GW_NETWORK_SUPPORT
#error "using GW OCC Fota should enable CONFIG_GW_NETWORK_SUPPORT first"
#endif
#endif

/* PWM输出 */
#define APP_PWM_EN 1

#ifndef MAX_DEVICES_META_NUM
#define MAX_DEVICES_META_NUM CONFIG_GW_MAX_DEVICES_META_NUM
#endif

#endif
