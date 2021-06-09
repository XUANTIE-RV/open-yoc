#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <uservice/uservice.h>
#include <yoc/netmgr_service.h>
#include <uservice/eventid.h>
#include <aos/kv.h>
#include <stdint.h>
#include <yoc/fota.h>
#include <aos/cli.h>

void board_cli_init();
extern void yunit_test_api(void);
extern netmgr_hdl_t app_netmgr_hdl;
void get_sensor_value(char *name, int *temp, int *humi);
void app_exception_event(uint32_t event_id);
void board_yoc_init(void);
void app_fota_init(void);
void app_fota_start(void);

#endif
