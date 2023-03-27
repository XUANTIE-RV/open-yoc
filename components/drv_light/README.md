# 概述

为灯控场景提供的灯驱动组件，可配置三种点灯模式，也可配置高电平或低电平驱动。

# 实例代码

## 两路冷暖光色温调节示例

```c
//  onoff : 0 or 1，开关
uint8_t onoff = 1;
//  actual : 0 - 0xffff，亮度
uint16_t actual = 0x8000;
//  temperature : 800 - 20000，色温
uint16_t temperature = 10000;

// PWM channel
static pwm_port_func_t pwm_channel[] = {
    {0},   //COLD
    {1},  //WARM
};

static pwm_dev_t pmw_light[ARRAY_SIZE(pwm_channel)];

static led_light_cfg_t led_config[] = {
    LED_LIGHT_MODEL(GENIE_COLD_WARM_LIGHT, &pwm_channel[0], LOW_LIGHT, &pmw_light[0], ARRAY_SIZE(pwm_channel)), 
};

int main()
{
    struct genie_cold_warm_op  led_config;
    LOGI("BTIME:%s\n", __DATE__ ","__TIME__);
    board_yoc_init();

    led_light_init(led_config);
    led_config.power_switch = 1;
    led_config.actual = actual;
    led_config.temperature = temperature;
    led_light_control(&led_config);
    return 0;
}
```

## 三路RGB灯示例

```c

// IO 配置 : 管脚，管脚功能配置，PWM channel
static pwm_port_func_t pwm_channel[] = {
    {P23, FMUX_PWM0，0}, // R
    {P31, FMUX_PWM1, 1}, // G
    {P32, FMUX_PWM2, 2}, // B
};

static pwm_dev_t pmw_light[ARRAY_SIZE(pwm_channel)];
//RGB_LIGHT : RGB灯模式  LOW_LIGHT : 低电平点亮
static led_light_cfg_t led_config[] = {
    LED_LIGHT_MODEL(RGB_LIGHT, &pwm_channel[0], LOW_LIGHT, &pmw_light[0], ARRAY_SIZE(pwm_channel)), 
};

static uint8_t _led_set(bool on)
{
    //power_switch : 开关  led_actual : 亮度，取值0-255
    struct genie_rgb_op rgb_config;
    if (on) {
        rgb_config.rgb_config[0].power_switch = 1;
        rgb_config.rgb_config[1].power_switch = 1;
        rgb_config.rgb_config[2].power_switch = 1;
        rgb_config.rgb_config[0].led_actual = 200;
        rgb_config.rgb_config[1].led_actual = 200;
        rgb_config.rgb_config[2].led_actual = 200;
    }else {
        rgb_config.rgb_config[0].power_switch = 0;
        rgb_config.rgb_config[1].power_switch = 0;
        rgb_config.rgb_config[2].power_switch = 0;
        rgb_config.rgb_config[0].led_actual  = 0;
        rgb_config.rgb_config[1].led_actual  = 0;
        rgb_config.rgb_config[2].led_actual  = 0;
    }
    
    return led_light_control(&rgb_config);
}
int main()
{
    LOGI("BTIME:%s\n", __DATE__ ","__TIME__);
    board_yoc_init();

    led_light_init(led_config);
    _led_set(1);
    mdelay(1000);
    _led_set(0);
    mdelay(1000);
    return 0;
}
```

## 一路开关灯示例

```c
// IO 配置 : 管脚，管脚功能配置，PWM channel
static pwm_port_func_t pwm_channel[] = {
    {P31, FMUX_PWM0, 0},
};

static pwm_dev_t pmw_light[ARRAY_SIZE(pwm_channel)];
//ON_OFF_LIGHT : 一路开关灯模式  LOW_LIGHT : 低电平点亮
static led_light_cfg_t led_config[] = {
    LED_LIGHT_MODEL(ON_OFF_LIGHT, &pwm_channel[0], LOW_LIGHT, &pmw_light[0], ARRAY_SIZE(pwm_channel)), 
};

static uint8_t _led_ctrl(bool on)
{
    struct genie_on_off_op on_off_config;

    on_off_config.power_switch = on;
    led_light_control((void *)(&on_off_config));
}

int main()
{
    LOGI("BTIME:%s\n", __DATE__ ","__TIME__);
    board_yoc_init();

    led_light_init(led_config);
    _led_ctrl(1);
    mdelay(1000);
    _led_ctrl(0);
    mdelay(1000);
    return 0;
}
```

