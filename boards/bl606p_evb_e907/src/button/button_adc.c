/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

/*************************************************
 * 按键驱动产生的事件包括短按、长按、组合键
 * 转化成keymsg_id，通过app_key_msg_send发送到消息处理
 *************************************************/
typedef struct _button_adc_conf_ {
    int   button_id;   /*物理按键ID定义*/
    char *adc_name;    /*adc的设备名称*/
    int   adc_channel; /*adc的通道*/
    int   vol_ref;     /*按键按下，adc采集到的电压值*/
    int   vol_range;   /*允许的误差*/
} button_adc_conf_t;

typedef struct _button_event_conf_ {
    int                 button_id;  /*物理按键ID*/
    int                 keymsg_id;  /*按键消息，对应具体的执行ID*/
    button_press_type_t press_type; /*按键类型，长按、短按*/
    int                 press_time; /*若是长按，填写持续的时间*/
} button_event_conf_t;

/*物理按键定义：请根据实际硬件修正参数*/
static button_adc_conf_t button_adc_confs[] = {
    {
        BUTTON_ID_VOL_UP,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_VOL_UP,
        BUTTON_ADC_VALUE_VOL_UP_RANGE,
    },
    {
        BUTTON_ID_VOL_DONW,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_VOL_DONW,
        BUTTON_ADC_VALUE_VOL_DONW_RANGE,
    },
    {
        BUTTON_ID_MUTE,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_MUTE,
        BUTTON_ADC_VALUE_MUTE_RANGE,
    },
    {
        BUTTON_ID_PLAY,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_PLAY,
        BUTTON_ADC_VALUE_PLAY_RANGE,
    },
#ifdef BUTTON_ID_USER
    {
        BUTTON_ID_USER,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_USER,
        BUTTON_ADC_VALUE_USER_RANGE,
    },
#endif
    /*ADC按键不存在逻辑上组合键，组合键也是对应不同的电压，和长按单键处理一样*/
#ifdef BUTTON_ID_VOL_UP_DOWN
    {
        BUTTON_ID_VOL_UP_DOWN,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_VOL_UP_DOWN,
        BUTTON_ADC_VALUE_VOL_UP_DOWN_RANGE,
    },
#endif
#ifdef BUTTON_ID_VOL_UP_MUTE
    {
        BUTTON_ID_VOL_UP_MUTE,
        "adc0",
        BUTTON_ADC_CHANNEL,
        BUTTON_ADC_VALUE_VOL_UP_MUTE,
        BUTTON_ADC_VALUE_VOL_UP_MUTE_RANGE,
    },
#endif
};

/*按键到事件对应表*/
static button_event_conf_t button_event_confs[] = {
    {
        BUTTON_ID_VOL_UP,
        KEY_MSG_VOL_UP,
        BUTTON_PRESS_UP /*短按键*/,
        0 /*ms*/,
    },
    {
        BUTTON_ID_VOL_UP,
        KEY_MSG_VOL_UP_LD,
        BUTTON_PRESS_LONG_DOWN /*长按键*/,
        2000 /*ms*/,
    },
    {
        BUTTON_ID_VOL_DONW,
        KEY_MSG_VOL_DONW,
        BUTTON_PRESS_UP /*短按键*/,
        0 /*ms*/,
    },
    {
        BUTTON_ID_VOL_DONW,
        KEY_MSG_VOL_DONW_LD,
        BUTTON_PRESS_LONG_DOWN /*长按键*/,
        2000 /*ms*/,
    },
    {
        BUTTON_ID_MUTE,
        KEY_MSG_MUTE,
        BUTTON_PRESS_UP /*短按键*/,
        0 /*ms*/,
    },
    {
        BUTTON_ID_PLAY,
        KEY_MSG_PLAY,
        BUTTON_PRESS_UP /*短按键*/,
        0 /*ms*/,
    },
#ifdef BUTTON_ID_USER
    {
        BUTTON_ID_USER,
        KEY_MSG_USER,
        BUTTON_PRESS_UP /*短按键*/,
        0 /*ms*/,
    },
#endif
    {
        BUTTON_ID_MUTE,
        KEY_MSG_UPLOG,
        BUTTON_PRESS_LONG_DOWN /*长按键*/,
        5000 /*ms*/,
    },
#ifdef BUTTON_ID_VOL_UP_DOWN
    {
        BUTTON_ID_VOL_UP_DOWN,
        KEY_MSG_FACTORY,
        BUTTON_PRESS_LONG_DOWN /*长按键*/,
        7000 /*ms*/,
    },
#endif
#ifdef BUTTON_ID_VOL_UP_MUTE
    {
        BUTTON_ID_VOL_UP_MUTE,
        KEY_MSG_BT,
        BUTTON_PRESS_LONG_DOWN /*长按键*/,
        5000 /*ms*/,
    },
#endif
};

void board_button_init(button_evt_cb_t keymsg_cb)
{
    button_init();

    /*添加物理按键*/
    for (int i = 0; i < sizeof(button_adc_confs) / sizeof(button_adc_conf_t); i++) {
        button_add_adc(button_adc_confs[i].button_id,
                       button_adc_confs[i].adc_name,
                       button_adc_confs[i].adc_channel,
                       button_adc_confs[i].vol_ref,
                       button_adc_confs[i].vol_range);
    }

    /*添加事件*/
    button_evt_t b_tbl[1];
    memset(b_tbl, 0, sizeof(b_tbl));
    for (int i = 0; i < sizeof(button_event_confs) / sizeof(button_event_conf_t); i++) {
        // button_evt_t b_tbl[] = { { .button_id = 0, .press_type = 0, .press_time = 0 } };
        b_tbl[0].button_id  = button_event_confs[i].button_id;
        b_tbl[0].press_type = button_event_confs[i].press_type;
        b_tbl[0].press_time = button_event_confs[i].press_time;
        button_add_event(button_event_confs[i].keymsg_id, b_tbl, sizeof(b_tbl) / sizeof(button_evt_t), keymsg_cb, "");
    }
}
