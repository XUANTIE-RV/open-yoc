/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>
#include <aos/aos.h>
#include <yoc/partition.h>
#include <aos/kv.h>
#include <ulog/ulog.h>
#include <uservice/uservice.h>
#include <wifi_provisioning.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include "wbp/wbp.h"
#include "sys/app_sys.h"
#include "bt/app_bt.h"
#include "app_main.h"

#define TAG "APP"

#define WIFI_PAIR_TIMEOUT (180)

static netmgr_hdl_t app_netmgr_hdl;

static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        LOGD(TAG, "wifi pair timeout...");

    } else if (event == WIFI_RPOV_EVENT_GOT_RESULT) {
        LOGD(TAG, "wifi pair got, ssid:%s passwd:%s", result->ssid, result->password);

        /* config wifi ssid and password and start connection */
        netmgr_config_wifi(
            app_netmgr_hdl, result->ssid, strlen(result->ssid), result->password, strlen(result->password));
        rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
        rvm_hal_net_set_hostname(dev, "T-head");
        netmgr_start(app_netmgr_hdl);
    }
}

static int app_network_init()
{
    /* wifi driver and network initialization */
    board_wifi_init();

    utask_t *task  = utask_new("netmgr", 10 * 1024, 8, AOS_DEFAULT_APP_PRI);
    app_netmgr_hdl = netmgr_dev_wifi_init();
    netmgr_service_init(task);

    return 0;
}

int main(int argc, char *argv[])
{
    board_yoc_init();
    LOGI(TAG, "wifi ble provisioning demo");

    /* partition and kv initialization */
    app_sys_init();

    /* network initialization */
    app_network_init();

    /* ble stack initialization */
    app_bt_init();

    /* cmd line initialization */
    app_cli_init();

    /* wifi ble provisionng initialization */
    wbp_init();

    /* start wifi provisioning with wifi ble provisioning method*/
    wifi_prov_start(wifi_prov_get_method_id("wifi_ble_prov"), wifi_pair_callback, WIFI_PAIR_TIMEOUT);

    return 0;
}
