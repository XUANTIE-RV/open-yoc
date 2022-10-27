/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __ADC_KEY_DRV__
#define __ADC_KEY_DRV__

typedef enum {
    KEY_TYPE_ADC, /* ADC按键 */
    KEY_TYPE_GPIO,/* GPIO按键 */
    KEY_TYPE_GPIO_SWITCH /* GPIO电平变化检测 */
} key_pin_type_t;

typedef enum {
    KEY_EVT_NULL = 0,
    KEY_S_CLICK,      /* 单键 */
    KEY_LONG_PRESS,   /* 长按 */
    KEY_COMBO_CLICK,  /* 组合按键 */
    KEY_SWITCH_CHANGE,/* GPIO电平变化 */
    KEY_EVT_END
} key_event_type_t;

typedef void (*key_scan_cb_t) (void);
typedef int (*key_event_cb_t) (key_event_type_t event_type, int key_id, int rt_value);
typedef int (*key_adc_read_imp_t)(int pin, int *vol);

/**
 * 按键服务初始化
 *
 * @param[in]  event_cb  按键事件回调
 *
 * @return  0 成功, 其他失败.
 */
int key_srv_init(key_event_cb_t event_cb);

/**
 * 启动按键服务
 *
 * @return  0 成功, 其他失败.
 */
int key_srv_start(void);

/**
 * 停止按键服务
 *
 * @return  0 成功, 其他失败.
 */
int key_srv_stop(void);

/**
 * 添加按键配置
 *
 * @param[in]  pin 引脚号
 * @param[in]  type 引脚类型，具体类型参见枚举key_pin_type_t
 * @param[in]  value 按键按下对应的值，ADC为采用值，GPIO为电平0|1
 * @param[in]  key_id 给按键编号，按键值为(1<<?)形式，事件回调函数传入该值用于按键判断
 * @return  0 成功, 其他失败.
 */
int key_srv_add_keyid(int pin, key_pin_type_t type, int value, int key_id);

/**
 * 按键服务配置，若值为<=0，不配置，使用默认值
 *
 * @param[in]  scan_interval 扫描间隔
 * @param[in]  long_press_timeout  长按超时
 * @param[in]  press_timeout  短按超时
 * @param[in]  adc_range  允许ADC按键值的偏差范围
 * @return  0 成功, 其他失败.
 */
int key_srv_config(int scan_interval, int long_press_timeout, int press_timeout, int adc_range);

#endif
