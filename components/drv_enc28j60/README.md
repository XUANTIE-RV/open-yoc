# 概述

enc28j60驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/enc28j60.h>
#include <yoc/netmgr.h>
#include <pin.h>

void board_yoc_init(void)
{
    /* kv config check */
    char strip[24] = "";
    aos_kv_getstring("ip_addr", strip, sizeof(strip));
    aos_kv_setint("eth_en", 1);
    if (strlen(strip) < 8) {
        aos_kv_setint("dhcp_en", 1);
    }
    aos_kv_setint("wifi_en", 0);  

    char str[KV_STR_LEN];
    int len;

    enc28j60_pin_t pin_param = {
        .enc28j60_spi_idx = 0,
        .enc28j60_spi_rst = ENC28J60_ETH_PIN_RST,
        .enc28j60_spi_cs  = ENC28J60_ETH_SPI_CS,
        .enc28j60_spi_interrupt = ENC28J60_ETH_PIN_INT,
    };
    eth_config_t eth_param;

    eth_param.net_pin = &pin_param;

    memset(str, 0, KV_STR_LEN);
    len = KV_STR_LEN;

    if (aos_kv_get("eth_mac", str, &len) == 0 && len >= 17) {
        //uint8_t *mac = eth_param.mac;
        //sscanf take about 2.5K code size
        //sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
        //    mac, mac+1, mac+2, mac+3, mac+4, mac+5);
        str2mac(str, eth_param.mac);
    } else {
        int random = rand();

        eth_param.mac[0] = 0x00;
        eth_param.mac[1] = 0x11;
        eth_param.mac[2] = 0x22;
        eth_param.mac[3] = (random >> 16) & 0xFF;
        eth_param.mac[4] = (random >> 8) & 0xFF;
        eth_param.mac[5] = (random) & 0xFF;
    }

    eth_enc28j60_register(&eth_param);
    netmgr_hdl_t netmgr_hdl = netmgr_dev_eth_init();
    if (netmgr_hdl_t) {
        netmgr_service_init(NULL);
        netmgr_start(netmgr_hdl_t);
    }
}
```
