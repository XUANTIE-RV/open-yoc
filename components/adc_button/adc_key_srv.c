/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <pin_name.h>
#include <pinmux.h>

#include <drv/adc.h>
#include <drv/gpio.h>

#include <yoc/adc_key_srv.h>
#include <aos/kernel.h>
#include <aos/hal/adc.h>

#define TAG "adcbt"

#define DEF_KEY_CLICK_TMOUT (100)           //ms
#define DEF_KEY_LONG_PRESS_TMOUT (1000 * 2) //ms
#define DEF_KEY_SACN_TMOUT (30)             //ms
#define DEF_KEY_ADC_RANGE (100)

typedef struct {
    int            click_tmout;
    int            long_press_tmout;
    int            scan_tmout;
    int            adc_range;
    key_event_cb_t key_event_cb;
    key_scan_cb_t  key_scan_cb;
    key_adc_read_imp_t key_adc_read_cb;
} key_config_t;

typedef struct {
    int              evt_flag;
    int              cnt;
    int              rt_value; /* 引导对应的当前值,扫描后保存在此 */
    key_event_type_t evt_type; /* 保存当前触发事件的类型 */
                               /* init member */
    int            pin;        /* 引脚号 */
    key_pin_type_t type;       /* 引脚类型 */
    int            value;      /* ADC模式是采用值, GPIO模式表示按下的电平(0/1) */
    int            key_id;     /* 用户定义按键ID，回调函数中返回 */
} key_srv_t;

//static int g_kval_print = 0;

#define MAX_KEY_COUNT (10)
static key_srv_t    key[MAX_KEY_COUNT] = {0};
static key_config_t g_key_config       = {DEF_KEY_CLICK_TMOUT,
                                    DEF_KEY_LONG_PRESS_TMOUT,
                                    DEF_KEY_SACN_TMOUT,
                                    DEF_KEY_ADC_RANGE,
                                    NULL,
                                    NULL};
static int          g_add_count        = 0;
static aos_timer_t  g_scan_timer       = {NULL};

static int adc_value_read(int pin, int *vol)
{
    int ret;

    adc_dev_t adc_hd = {0, {0}, NULL};
    adc_hd.port = hal_adc_pin2channel(pin);
    adc_hd.config.sampling_cycle = 0;

    ret = hal_adc_init(&adc_hd);

    if(ret == 0) {
        ret = hal_adc_value_get(&adc_hd, vol, 0);
    }

    hal_adc_finalize(&adc_hd);

    return 0;
}


static int key_adc_read(int pin, int *val)
{
    int ret;

    if (g_key_config.key_adc_read_cb) {
        ret = g_key_config.key_adc_read_cb(pin, val);
    } else {
        ret = adc_value_read(pin, val);
    }
    return ret;
}

static int key_gpio_read(int pin, int *val)
{
    gpio_pin_handle_t     handle;

    bool bval = false;
    drv_pinmux_config(pin, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(pin, NULL);
    if (handle) {
        csi_gpio_pin_config_mode(handle, GPIO_MODE_PULLNONE);
        csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_INPUT);
        csi_gpio_pin_read(handle, &bval);
        csi_gpio_pin_uninitialize(handle);
        *val = (int)bval;
    } else {
        return -1;
    }

    return 0;
}

static int key_down_check(int id)
{
    int is_down = 0;

    int key_fix_value = key[id].value;
    int key_rt_value  = key[id].rt_value;
    int type          = key[id].type;
    int key_adc_range = g_key_config.adc_range;

    if (type == KEY_TYPE_ADC) {
        if (key_rt_value > (key_fix_value - key_adc_range) &&
            key_rt_value < (key_fix_value + key_adc_range)) {
            is_down = 1;
        }
        //printf(">>%d rt=%d fix=%d range=%d down=%d\n", id, key_rt_value, key_fix_value, key_adc_range, is_down );
    } else if (type == KEY_TYPE_GPIO) {
        if (key_fix_value == key_rt_value) {
            is_down = 1;
        }
    } else {
        ;
    }

    return is_down;
}

static void key_scan(void *timer, void *arg)
{
    int ret = 0, i = 0, j = 0;

    if (g_key_config.key_scan_cb) {
        g_key_config.key_scan_cb();
    }

    /* 更新实时扫描值 */
    for (i = 0; i < g_add_count; i++) {
        if (key[i].type != KEY_TYPE_GPIO_SWITCH) {
            key[i].rt_value = -1;
        }
    }

    for (i = 0; i < g_add_count; i++) {
        if (key[i].rt_value >= 0 && key[i].type != KEY_TYPE_GPIO_SWITCH) {
            continue;
        }

        /* 读取引脚值 */
        int vol = 0;
        if (key[i].type == KEY_TYPE_ADC) {
            ret = key_adc_read(key[i].pin, &vol);
        } else if (key[i].type == KEY_TYPE_GPIO) {
            ret = key_gpio_read(key[i].pin, &vol);
        } else if (key[i].type == KEY_TYPE_GPIO_SWITCH) {
            ret = key_gpio_read(key[i].pin, &vol);
            if (key[i].rt_value != vol) {
                key[i].evt_type = KEY_SWITCH_CHANGE;
            }
        } else {
            continue;
        }

        /* 与当前引脚相同的都更新一下，避免反复读取 */
        if (ret >= 0) {
            for (j = i; j < g_add_count; j++) {
                if (key[j].pin == key[i].pin) {
                    key[j].rt_value = vol;
                }
            }
        }
    }

    static int combo_key_ids[MAX_KEY_COUNT];
    static int combo_key_count = 0;

    int down_count = 0;
    for (i = 0; i < g_add_count; i++) {
        //printf(">>%d %d %d %d %d\n", i, key[i].evt_flag, key[i].cnt, key[i].rt_value, key[i].value);
        /* GPIO 状态检查 */
        if (key[i].evt_type == KEY_SWITCH_CHANGE) {
            key[i].evt_type = KEY_EVT_NULL;
            if (g_key_config.key_event_cb) {
                g_key_config.key_event_cb(KEY_SWITCH_CHANGE, key[i].key_id, key[i].rt_value);
            }
        }

        /* 按键检查 */
        if (key_down_check(i)) {
            /* 不是组合键状态，记录按下的键 */
            if (combo_key_count == 0) {
                combo_key_ids[down_count] = i;
            }
            down_count++;

            /* 按钮被按下，计数 */
            key[i].cnt++;

            /* 长按检查， evt_flag保证长按不被重复发生 */
            if (key[i].evt_flag == 0 &&
                (key[i].cnt * g_key_config.scan_tmout) > g_key_config.long_press_tmout) {
                key[i].evt_type = KEY_LONG_PRESS;

                key[i].cnt      = 0;
                key[i].evt_flag = 1;
            }
        } else {
            /* 没按下的按钮，短按检查 */
            if (key[i].evt_flag == 0 &&
                (key[i].cnt * g_key_config.scan_tmout) > g_key_config.click_tmout) {
                key[i].evt_type = KEY_S_CLICK;
            }

            key[i].cnt      = 0;
            key[i].evt_flag = 0;
        }
    }

    if (combo_key_count) {
        /* 在组合键状态，确没有任何按键，组合键弹起 */
        if (down_count == 0) {
            int combo_id = 0;
            for(j = 0; j < combo_key_count; j++){
                int id = combo_key_ids[j];

                combo_id |= key[id].key_id;
                key[id].evt_type = KEY_EVT_NULL;
            }
            combo_key_count = 0;

            if (g_key_config.key_event_cb) {
                g_key_config.key_event_cb(KEY_COMBO_CLICK, combo_id, -1);
            }
        }
    } else {
        /* 无组合键状态，有多按键记录 */
        if (down_count > 1) {
            combo_key_count = down_count;
        } else {
            /* 单键 */
            int id = combo_key_ids[0];
            if (key[id].evt_type != KEY_EVT_NULL) {
                g_key_config.key_event_cb(key[id].evt_type, key[id].key_id, -1);
                key[id].evt_type = KEY_EVT_NULL;
            }
        }
    }
}

int key_srv_init(key_event_cb_t event_cb)
{
    if (event_cb == NULL) {
        return -1;
    }

    g_key_config.key_event_cb = event_cb;
    g_key_config.key_adc_read_cb = NULL;

    return 0;
}

int key_srv_start(void)
{
    int ret;

    ret = aos_timer_new(&g_scan_timer, key_scan, NULL, g_key_config.scan_tmout, 1);
    if (!ret) {
        return ret;
    }

    ret = aos_timer_start(&g_scan_timer);

    return ret;
}

int key_srv_add_keyid(int pin, key_pin_type_t type, int value, int key_id)
{
    if (g_add_count >= MAX_KEY_COUNT) {
        return -1;
    }

    if (pin < 0) {
        return -1;
    }

    if (type < KEY_TYPE_ADC || type > KEY_TYPE_GPIO_SWITCH) {
        return -1;
    }

    key[g_add_count].pin      = pin;
    key[g_add_count].type     = type;
    key[g_add_count].value    = value;
    key[g_add_count].rt_value = value;
    key[g_add_count].key_id   = key_id;

    g_add_count++;

    return 0;
}

int key_srv_stop(void)
{
    g_add_count = 0;
    if (g_scan_timer.hdl != NULL) {
        aos_timer_stop(&g_scan_timer);
        aos_timer_free(&g_scan_timer);
        g_scan_timer.hdl = NULL;
    }

    return 0;
}

int key_srv_config(int scan_interval, int long_press_timeout, int press_timeout, int adc_range)
{
    if (scan_interval > 0) {
        g_key_config.scan_tmout = scan_interval;
    }

    if (long_press_timeout > 0) {
        g_key_config.long_press_tmout = long_press_timeout;
    }

    if (press_timeout > 0) {
        g_key_config.click_tmout = press_timeout;
    }

    if (adc_range > 0) {
        g_key_config.adc_range = adc_range;
    }

    return 0;
}

int key_srv_scan_register(key_scan_cb_t cb)
{
    g_key_config.key_scan_cb = cb;
    return 0;
}

int key_srv_adcread_register(key_adc_read_imp_t cb)
{
    g_key_config.key_adc_read_cb = cb;
    return 0;
}
