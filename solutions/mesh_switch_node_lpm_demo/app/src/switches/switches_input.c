/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <aos/hal/gpio.h>

#include "common/log.h"
#include "pinmux.h"
#include "mesh.h"
#include "switches_queue.h"
#include "switches_input.h"


#include "ulog/ulog.h"

#define TAG "SWITCHES_INPUT"

typedef struct _input_key_s {
    uint8_t key_last_status;
    uint8_t key_status;//0 released 1 pressed
    uint8_t key_pressd_time;
    uint8_t press_check_timer_status;
    gpio_dev_t input_io;
    aos_timer_t press_check_timer;
} _input_key_t;

static sb_input_event_t input_event;
static _input_key_t g_key[INPUT_IO_NUM];

static int input_event_send(uint8_t port, switch_press_type_e press_type)
{
    queue_mesg_t mesg;

    mesg.type = QUEUE_MESG_TYPE_INPUT;
    mesg.data = (port & 0x0F) | ((press_type & 0x0F) << 4);
    return queue_send_data(&mesg);
}

inline static bool input_io_status_get(gpio_dev_t dev)
{
    uint32_t key_value = 0;
    hal_gpio_input_get(&dev, &key_value);
    return key_value;
}

static void input_event_key_check_timer_cb(void *timer, void *arg)
{
    _input_key_t* key =  (_input_key_t*) arg;
    uint8_t key_pressed = 0;
    if (((key->input_io.config == INPUT_PULL_UP) && !input_io_status_get(key->input_io)) \
            || ((key->input_io.config == INPUT_PULL_DOWN) && !input_io_status_get(key->input_io))) {
        key_pressed = 1;
    }
    if (key_pressed) {
        key->key_pressd_time++;
        aos_timer_start(&key->press_check_timer);
    } else {
        if(key->key_pressd_time < INPUT_EVENT_KEY_LONG_PRESS) { //short press release
            input_event_send(key->input_io.port,SWITCH_PRESS_ONCE);
        } else if (key->key_pressd_time > INPUT_EVENT_KEY_LONG_PRESS) {// long press release
            input_event_send(key->input_io.port,SWITCH_PRESS_LONG);
        }
        aos_timer_stop(&key->press_check_timer);
        key->press_check_timer_status = 0;
    }

}

__attribute__((section(".__sram.code")))  void _key_pres_process(uint8_t port)
{
    for(int index =0 ; index < INPUT_IO_NUM ; index++) {
        if(g_key[index].input_io.port == port && g_key[index].press_check_timer_status == 0) {
            aos_timer_start(&g_key[index].press_check_timer);
            g_key[index].press_check_timer_status = 1;
            g_key[index].key_pressd_time = 0;
        }
    }
}
__attribute__((section(".__sram.code"))) void io_irq_handler(void *arg)
{
    gpio_dev_t* io = (gpio_dev_t* )arg;
    _key_pres_process(io->port);
}

static int input_event_gpio_init(void)
{
    for(int i=0; i < INPUT_IO_NUM; i++) {
        hal_gpio_init(&g_key[i].input_io);
        hal_gpio_enable_irq(&g_key[i].input_io, IRQ_TRIGGER_FALLING_EDGE, io_irq_handler, &g_key[i].input_io);
    }
    return 0;
}

int32_t input_event_init(void)
{
    int32_t ret = 0;

    memset(&input_event, 0, sizeof(sb_input_event_t));

    g_key[0].input_io.port = INPUT_EVENT_PIN_1;
    g_key[0].input_io.config = INPUT_PULL_UP;
    g_key[1].input_io.port = INPUT_EVENT_PIN_2;
    g_key[1].input_io.config = INPUT_PULL_UP;
    g_key[2].input_io.port = INPUT_EVENT_PIN_3;
    g_key[2].input_io.config = INPUT_PULL_UP;

    for (int index = 0; index < INPUT_IO_NUM; index++) {
        aos_timer_new(&g_key[index].press_check_timer, input_event_key_check_timer_cb, &g_key[index], INPUT_EVENT_KEY_CHECK_TIMEROUT, 1);
        aos_timer_stop(&g_key[index].press_check_timer);
        g_key[index].key_status = KEY_RELEASED;
        g_key[index].key_last_status = KEY_RELEASED;
        g_key[index].key_pressd_time = 0;
        g_key[index].press_check_timer_status = 0;
    }
    ret = input_event_gpio_init();
    if (ret != 0) {
        LOGE(TAG,"input event gpio init fail");
        return ret;
    }

    return ret;
}
