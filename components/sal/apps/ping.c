/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <devices/netdrv.h>

int ping(int type, char *remote_ip)
{
    aos_dev_t *wifi_dev = device_find("wifi", 0);
    if (wifi_dev != NULL) {
        hal_net_ping(wifi_dev, type, remote_ip);
    }

	aos_dev_t *gprs_dev = device_find("gprs", 0);
    if (gprs_dev != NULL) {
        hal_net_ping(gprs_dev, type, remote_ip);
    }

    return 0;
}

