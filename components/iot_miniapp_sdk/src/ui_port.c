/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include <aos/kernel.h>
#include <drv/porting.h>
#include <devices/input.h>
#include <devices/display.h>

#include "ui_port.h"
#include "jsapi_register.h"
#include "port/jquick_thread.h"

#ifndef CONFIG_UI_TASK_STACK_SIZE
#define CONFIG_UI_TASK_STACK_SIZE 95536
#endif

#define TAG "Miniapp"

static JQuick_Thread g_ui_thread;

static JQuick_Thread jquick_thread_create_with_stack_size_prio(
        const char* name,
        Runner runner,
        void* args,
        int stackSize,
        int prio)
{
    pthread_t thread;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, stackSize);
    int v = pthread_create(&thread, &attr, runner, args);
    if (v == 0) {
        if (name != NULL) {
            pthread_setname_np(thread, name);
            pthread_setschedprio(thread, prio);
        }
        return (JQuick_Thread)thread;
    }

    LOGE(TAG, "JQuick_Thread: Create JQuick_Thread failed\n");

    return NULL;
}

static void *ui_task(void *arg)
{
    extern int falcon_entry(int argc, char *argv[]);
    LOGD(TAG, "Miniapp build time: %s, %s\r\n", __DATE__, __TIME__);

    /* 注册JSAPI */
    register_aiot_wifi_jsapi();
    register_aiot_http_jsapi();
    register_aiot_power_jsapi();
    register_aiot_system_kv_jsapi();
    register_aiot_screen_jsapi();
#if defined(CONFIG_COMP_VOICE_WRAPPER) && CONFIG_COMP_VOICE_WRAPPER
    register_aiot_voice_jsapi();
#endif
    register_aiot_smta_jsapi();
    register_aiot_fota_jsapi();
#if defined(CONFIG_BT_MESH) && CONFIG_BT_MESH
    register_aiot_gateway_jsapi();
#endif
    
    LOGD(TAG, "Miniapp entry here!\r\n");
    falcon_entry(0, NULL);

    return NULL;
}

/** Miniapp entry task */
void ui_task_run(void)
{
    /* aos defalult prio = 32,  pthread prio = 62 - 32 = 30 */
    g_ui_thread = jquick_thread_create_with_stack_size_prio("ui-task", ui_task, NULL, CONFIG_UI_TASK_STACK_SIZE, 30);
}