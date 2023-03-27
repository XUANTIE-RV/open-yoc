/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include <aos/aos.h>

#include "api/mesh.h"
#include "mesh_node.h"
#include "ota_version.h"
#include "app_init.h"
#include "app_main.h"
#include "mesh_ctrl.h"

#define TAG "DEMO"

static aos_sem_t sync_sem;

int main()
{
    int ret;

    board_yoc_init();

    LOGI(TAG, "Mesh light node demo %s", aos_get_app_version());

    app_set_led_state(LED_OFF);

    /* Mesh Models and Device parameter setting, include models event callback regiter */
    ret = mesh_dev_init();

    if (ret) {
        LOGE(TAG, "mesh dev init failed");
    }

    aos_sem_new(&sync_sem, 0);

    while (1) {
        aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    }

    return 0;
}
