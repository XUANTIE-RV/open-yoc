
/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/ble.h>

#define DEVICE_NAME       "WBP_DEMO"

void app_bt_init(void)
{
    /* ble stack initialization */
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = NULL,
        .conn_num_max = 1,
    };

    ble_stack_init(&init);

    ble_stack_setting_load();
}
