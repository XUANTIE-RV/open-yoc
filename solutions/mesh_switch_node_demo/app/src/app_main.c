/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include <aos/aos.h>
#include <app_main.h>
#include <app_init.h>
#include "mesh_node.h"
#include <api/mesh.h>
#include "mesh_ctrl.h"
#include "cli_mesh_ctrl.h"

#define TAG "DEMO"

struct {
    uint8_t button1_press: 1;
    uint8_t button2_press: 1;
    uint8_t on_off: 1;
    aos_sem_t sync_sem;
} switch_state = {0};

void prepare_onoff_state(uint8_t onoff)
{
    switch_state.on_off = onoff;
}

int main()
{
    int ret;

    /* Board Hardware and yoc modules init */
    board_yoc_init();

    /* Mesh Models and Device parameter setting, include models event callback regiter */
    ret = mesh_dev_init();

    if (ret) {
        LOGE(TAG, "mesh dev init faild\n");
    }

    /* Mesh CLI command handle register */
    cli_reg_cmd_switch_ctrl();

    LOGI(TAG, "Mesh Switch node demo\n");

    aos_sem_new(&switch_state.sync_sem, 0);

    /* Wait for button signal to publish on/off command to default group */
    while (1) {
        aos_sem_wait(&switch_state.sync_sem, AOS_WAIT_FOREVER);

        if (switch_state.button1_press) {
            switch_state.button1_press = 0;
            ret = gen_onoff_set(switch_state.on_off, false);

            if (ret) {
                LOGE(TAG, "send unack msg LED faild");
            } else {
                LOGI(TAG, "send unack msg LED %s", switch_state.on_off ? "ON" : "OFF");
            }

            prepare_onoff_state(!switch_state.on_off);
        }

        if (switch_state.button2_press) {
            switch_state.button2_press = 0;
            ret = gen_onoff_set(switch_state.on_off, true);

            if (ret) {
                LOGE(TAG, "send ack msg LED faild");
            } else {
                LOGI(TAG, "send ack msg LED %s", switch_state.on_off ? "ON" : "OFF");
            }
        }
    }

    return 0;
}

