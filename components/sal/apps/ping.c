/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <devices/netdrv.h>

int ping(int type, char *remote_ip)
{
    rvm_dev_t *wifi_dev = rvm_hal_device_find("wifi", 0);
    if (wifi_dev != NULL) {
        rvm_hal_net_ping(wifi_dev, type, remote_ip);
    }

	rvm_dev_t *gprs_dev = rvm_hal_device_find("gprs", 0);
    if (gprs_dev != NULL) {
        rvm_hal_net_ping(gprs_dev, type, remote_ip);
    }

    return 0;
}

