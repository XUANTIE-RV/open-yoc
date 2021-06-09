#include <stdbool.h>
#include <aos/kv.h>
#include "board.h"
#include "app_main.h"
#include <devices/devicelist.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <devices/rtl8723ds.h>
#ifdef CONFIG_CSI_V2
#include <drv/pin.h>
#endif
#ifdef CONFIG_CSI_V1
#include "pin_name.h"
#endif

#define TAG "init"

netmgr_hdl_t app_netmgr_hdl;

static void network_init()
{
    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);


    app_netmgr_hdl = netmgr_dev_wifi_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
     }

}

void board_yoc_init(void)
{
    board_init();
    event_service_init(NULL);
    console_init(CONSOLE_UART_IDX, 115200, 512);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");
    network_init();

    board_cli_init();
}
