## 概述

一个c语言实现的按键服务组件，支持单击、双击、长按、松开、自动消抖，可以自由设置组合按键，可用于GPIO及ADC类型按键。

## 配置

无。

## 接口列表

| 函数             | 说明             |
| :--------------- | :--------------- |
| button_init      | button初始化     |
| button_deinit    | button去初始化   |
| button_add_gpio  | 添加gpio类型按键 |
| button_add_adc   | 添加adc类型按键  |
| button_add_event | 添加按键事件     |

## 接口详细说明

```C
int button_init(void)
```

按键服务初始化

- 返回值：
  - 成功返回0，失败返回-1



```C
int button_deinit(void)
```

按键服务去初始化

- 返回值：
  - 成功返回0，失败返回-1



```C
int button_add_gpio(int button_id, int gpio_pin, button_gpio_level_t active_level)
```

添加gpio类型按键

- 参数
  - button_id：按键id
  - gpio_pin：按键对应的管脚
  - active_level：按键按下时的电平
- 返回值：
  - 成功返回0，失败返回-1



```C
int button_add_adc(int button_id, char *adc_name, int adc_channel, int vol_ref, int vol_range)
```

添加adc类型按键

- 参数
  - button_id：按键id
  - adc_name: ADC设备名
  - adc_channel: ADC通道
  - vol_ref: 参考电压
  - vol_range: 电压范围,即vol_ref±vol_range
- 返回值：
  - 成功返回0，失败返回-1



```c
int button_add_event(int evt_id, button_evt_t *buttons, int button_count, button_evt_cb_t evt_cb, void *priv)
```

组合按键列表初始化

- 参数
  - evt_id：用户事件id
  - button: 物理按键参数表
  - button_count: 按键数目
  - cb: 事件回调
  -  priv: 用户参数
- 返回值：
  - 成功返回0，失败返回-1


```c

typedef struct {
    int             button_id;  //按键id
    button_evt_id_t event_id;   //按键事件
    int             press_time; //当按键事件为长按时，为长按事件
} button_evt_t;

typedef enum {
    BUTTON_PRESS_DOWN = 0,
    BUTTON_PRESS_UP,
    BUTTON_PRESS_LONG_DOWN,
    BUTTON_PRESS_DOUBLE,
    BUTTON_PRESS_TRIPLE
} button_evt_id_t;
```

```c
typedef void (*button_evt_cb_t)(int evt_id, void *priv)
```

- 参数
	- event_id ：用户事件id
	- priv:  用户自定义参数



## 示例代码

```c
void button_event(int evt_id, void *priv)
{
    LOGI(TAG, "-------------button(%s)", (char *)priv);
}

int main(void)
{
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());

    button_init();
    button_add_gpio(0, PA29, LOW_LEVEL);
    button_add_gpio(1, PB20, LOW_LEVEL);
    button_add_gpio(2, PA24, LOW_LEVEL); //添加GPIO类型按键，pin为PA29，PA20，PA24

    button_evt_t buttons[] = {
        {
            .event_id = BUTTON_PRESS_DOWN,
            .button_id = 0,
            .press_time = 0,
        },
    };
    button_add_event(0, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_DOWN"); //用户事件号为0，事件为按键（0）按下
    
    buttons[0].event_id = BUTTON_PRESS_UP;
    button_add_event(1, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_UP");//用户事件号为1，事件为按键（0）松开

    buttons[0].event_id = BUTTON_PRESS_LONG_DOWN;
    buttons[0].press_time = 3000;
    button_add_event(2, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_LONG_DOWN(3000)");//用户事件号为2，事件为按键（0）长按3000ms

    buttons[0].event_id = BUTTON_PRESS_DOUBLE;
    button_add_event(3, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_DOUBLE");//用户事件号为3，button id为0， button事件号为双击

    buttons[0].event_id = BUTTON_PRESS_LONG_DOWN;
    buttons[0].press_time = 5000;
    button_add_event(4, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_LONG_DOWN(5000)");//用户事件号为4，事件为按键（0）长按5000ms

    buttons[0].event_id = BUTTON_PRESS_LONG_DOWN;
    buttons[0].press_time = 1000;
    button_add_event(5, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_LONG_DOWN(1000)");//用户事件号为5，事件为按键（0）长按1000ms

    buttons[0].event_id = BUTTON_PRESS_TRIPLE;
    button_add_event(6, buttons, sizeof(buttons)/sizeof(button_evt_t), button_event, "BUTTON_PRESS_TRIPLE");//用户事件号为6，事件为按键（0）三击

    button_evt_t buttons_com[] = {
        {
            .event_id = BUTTON_PRESS_DOWN,
            .button_id = 1,
            .press_time = 0,
        },
        {
            .event_id = BUTTON_PRESS_DOWN,
            .button_id = 2,
            .press_time = 0,
        },
    };
    button_add_event(7, buttons_com, sizeof(buttons_com)/sizeof(button_evt_t), button_event, "BC(1+2)");//用户事件号为7，由按键（1）按下和按键（2）按下组合的事件

    button_evt_t buttons_com1[] = {
        {
            .event_id = BUTTON_PRESS_DOWN,
            .button_id = 1,
            .press_time = 0,
        },
        {
            .event_id = BUTTON_PRESS_DOWN,
            .button_id = 0,
            .press_time = 0,
        },
    };
    button_add_event(8, buttons_com1, sizeof(buttons_com1)/sizeof(button_evt_t), button_event, "BC(1+0)");//用户事件号为7，由按键（1）按下和按键（0）按下组合的事件

    button_evt_t buttons_com2[] = {
        {
            .event_id = BUTTON_PRESS_LONG_DOWN,
            .button_id = 0,
            .press_time = 1000,
        },
        {
            .event_id = BUTTON_PRESS_LONG_DOWN,
            .button_id = 1,
            .press_time = 1000,
        },
        {
            .event_id = BUTTON_PRESS_LONG_DOWN,
            .button_id = 2,
            .press_time = 1000,
        },
    };
    button_add_event(9, buttons_com2, sizeof(buttons_com2)/sizeof(button_evt_t), button_event, "BC(0+1+2)");//用户事件号为7，由按键（0）长按1000ms、按键（1）长按1000ms和按键（2）长按1000ms组合的事件
    
    return 0;
}

```

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

csi: v7.2.0及以上。
aos: v7.2.0及以上。
rhino：v7.2.0及以上。

## 组件参考

无。