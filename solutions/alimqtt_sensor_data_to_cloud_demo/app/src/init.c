#include <stdbool.h>
#include "pin_name.h"
#include "board.h"
#include "app_main.h"
#include <devices/devicelist.h>
#include <devices/esp8266.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <aos/hal/flash.h>
#include <aos/kv.h>
#include <aos/cli.h>
#define TAG "init"

netmgr_hdl_t app_netmgr_hdl;

void cli_reg_cmd_kvtool(void);

// const hal_logic_partition_t hal_partitions[] =
// {
//     [HAL_PARTITION_PARAMETER_2] =
//     {
//         .partition_owner      = HAL_FLASH_QSPI,
//         .partition_description  = "kv",
//         .partition_start_addr  = 0x1003EA00,
//         .partition_length     = 0x000C00,
//         .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
//     },
// };

static void network_init()
{
    // network init

    /* kv config check */
    
    aos_kv_setint("wifi_en", 1);
    aos_kv_setint("gprs_en", 0);
    aos_kv_setint("eth_en", 0);

    esp_wifi_param_t esp_param;

    esp_param.device_name    = "uart2";
    esp_param.baud           = 115200;
    esp_param.buf_size       = 4096;
    esp_param.enable_flowctl = 0;
    esp_param.reset_pin      = WIFI_ESP8266_RESET;
    esp_param.smartcfg_pin   = WIFI_ESP8266_SMARTCFG;

    wifi_esp8266_register(NULL, &esp_param);
    app_netmgr_hdl = netmgr_dev_wifi_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 1 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        // netmgr_config_wifi(app_netmgr_hdl, "TEST", 4, "TEST1234", 8);
        netmgr_start(app_netmgr_hdl);
    }
}

void board_yoc_init(void)
{
	board_init();
    event_service_init(NULL);

    console_init(CONSOLE_UART_IDX, 115200, 512);
    aos_cli_init();
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "[YoC]Build:%s,%s",__DATE__, __TIME__);

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
