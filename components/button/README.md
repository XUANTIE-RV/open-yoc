# 概述

按键服务组件，通过判断GPIO状态来实现长按、短按等事件响应，并支持组合按键。

# 示例代码

```c
const static char *log_buf[] = {
    "down",
    "up",
    "down_l",
    "d_down",
    "bc",
};

static void button_evt(button_evt_id_t event_id, char *name, void *priv)
{
    LOGE(TAG, "key[%s]:%s", name, log_buf[event_id]);
}

const static button_config_t button_table[] = {
    {APP_KEY_MUTE, EVT_ALL_FLAG, button_evt, NULL, "mute"},
    {APP_KEY_VOL_INC, EVT_ALL_FLAG, button_evt, NULL, "inc"},
    {APP_KEY_VOL_DEC, EVT_ALL_FLAG, button_evt, NULL, "dec"},
    {0, 0, NULL, NULL},
};

const static button_combinations_t bc_table[] = {
    {
        .pin_id[0] = APP_KEY_MUTE,
        .pin_id[1] = APP_KEY_VOL_INC,
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = button_evt,
        .priv = NULL,
        .name = "mute&inc"
    },
    {
        .pin_id[0] = APP_KEY_MUTE,
        .pin_id[1] = APP_KEY_VOL_DEC,
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = button_evt,
        .priv = NULL,
        .name = "mute&dec"
    },
    {
        .pin_sum = 0,
        .cb = NULL,
    },
};

void app_main()
{
    button_srv_init();
    button_init(button_table);
    button_combination_init(bc_table);
}

```