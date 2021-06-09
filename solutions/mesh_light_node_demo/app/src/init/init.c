
#include <stdbool.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <devices/devicelist.h>
#include <aos/hal/pwm.h>
#include <board_config.h>
#include "app_init.h"
#include "app_main.h"
#include "drv_light.h"


const char *TAG = "INIT";

#ifndef CONSOLE_IDX

#define CONSOLE_IDX 0

#endif

static int init_flag = 1;

#define ON              1
#define OFF             0

static pwm_dev_t pmw_light[3];

static pwm_port_func_t pwm_channel_config[] = {
    {PIN_LED_R, PIN_LED_R_CHANNEL, PWM_LED_R_PORT},
    {PIN_LED_G, PIN_LED_G_CHANNEL, PWM_LED_G_PORT},
    {PIN_LED_B, PIN_LED_B_CHANNEL, PWM_LED_B_PORT},
};

static led_light_cfg_t led_light_config[] = {
    LED_LIGHT_MODEL(RGB_LIGHT, &pwm_channel_config[0], LOW_LIGHT, &pmw_light[0], 3)   \
};

static void _led_init(void)
{
    if(init_flag){
        led_light_init(led_light_config);
        init_flag = 0;
    }
}

static void _led_set(bool on,uint8_t r_s,uint8_t g_s,uint8_t b_s,uint8_t r_a,uint8_t g_a,uint8_t b_a)
{
    struct genie_rgb_op rgb_config;

    if (on) {
        rgb_config.rgb_config[0].power_switch = r_s;
        rgb_config.rgb_config[1].power_switch = g_s;
        rgb_config.rgb_config[2].power_switch = b_s;
        rgb_config.rgb_config[0].led_actual = r_a;
        rgb_config.rgb_config[1].led_actual = g_a;
        rgb_config.rgb_config[2].led_actual = b_a;
    }else {
        rgb_config.rgb_config[0].power_switch = 0;
        rgb_config.rgb_config[1].power_switch = 0;
        rgb_config.rgb_config[2].power_switch = 0;
        rgb_config.rgb_config[0].led_actual  = 0;
        rgb_config.rgb_config[1].led_actual  = 0;
        rgb_config.rgb_config[2].led_actual  = 0;
    }

    led_light_control((void *)&rgb_config);
}

void app_set_led_state(int state)
{
    _led_init();
    switch (state) {
        case LED_OFF:
            _led_set(OFF,OFF,OFF,OFF,0,0,0);
            break;

        case LED_ON:
            _led_set(ON,ON,ON,ON,255,255,255);
            break;

        case LED_ATTENTION_ON:
            _led_set(ON,ON,OFF,OFF,255,0,0);
            break;

        case LED_ATTENTION_OFF:
            _led_set(OFF,OFF,OFF,OFF,0,0,0);
            break;

        case LED_UNPROVED:
            _led_set(ON,OFF,ON,OFF,0,255,0);
            break;

        case LED_PROVED:
            _led_set(ON,OFF,OFF,ON,0,0,255);
            break;

        default:
            _led_set(OFF,OFF,OFF,OFF,0,0,0);
            break;
    }

    /* LED lights are working all the time,no need to close.*/
    //led_close(led_dev);
}

void board_yoc_init()
{
    int ret;

    board_init();

    console_init(CONSOLE_UART_IDX, 115200, 0);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    /* load partition */
    ret = partition_init();

    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* kvfs init */
    aos_kv_init("kv");

#ifdef MESH_NODE_REPEAT_RESET
    extern void reset_by_repeat_init(void);
    reset_by_repeat_init();
#endif

    LOGI(TAG, "Build:%s,%s", __DATE__, __TIME__);
}

