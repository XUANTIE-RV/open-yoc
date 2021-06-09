#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <pin_name.h>

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

#define CONFIG_WIFI_SMARTLIVING 1 /* 飞燕配网 */
#define CONFIG_SMARTLIVING_MQTT 1 /* 飞燕mqtt发布及订阅 */

#if defined(CONFIG_SMARTLIVING_MQTT) && CONFIG_SMARTLIVING_MQTT
#define CONFIG_SMARTLIVING_DEMO 1    /* 飞燕demo */
#define CONFIG_SMARTLIVING_AT_MODULE 1  /* 飞燕模组 */
#endif

#define CONFIG_YOC_SOFTAP_PROV 0 /* YOC配网*/


#define WLAN_ENABLE_PIN 0xFFFFFFFF
#define WLAN_POWER_PIN 0xFFFFFFFF


#define CONFIG_WIFI_SSID "OS-TEST"
#define CONFIG_WIFI_PSK "TEST1234"
#define CONFIG_MANTB_VERSION 4

#define CONSOLE_IDX 0

/* You can add user configuration items here. */
#define APP_FOTA_EN 0

/* PWM输出 */
#define APP_PWM_EN 1

#define PWM0_PIN PB0
#define PWM0_PIN_FUNC PB0_PWM
#define PWM0_CH 0

#define PWM1_PIN PB1
#define PWM1_PIN_FUNC PB1_PWM
#define PWM1_CH 1

#define PWM2_PIN PB2
#define PWM2_PIN_FUNC PB2_PWM
#define PWM2_CH 2

#endif
