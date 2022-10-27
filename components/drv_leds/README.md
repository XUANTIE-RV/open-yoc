## 概述

三色灯驱动，可以分开单独控制，也可以同时控制，产生混色效果。

## 配置

无

## 接口列表

| 函数                 | 说明        |
| :------------------- | :---------- |
| led_open/led_open_id | 打开led     |
| led_close            | 关闭led     |
| led_control          | 控制led     |
| led_rgb_register     | 注册led设备 |

## 接口详细说明

### led_open

`aos_dev_t *led_open(const char *name);`

- 功能描述:
  - 打卡led设备。
- 参数:
  - `name`:  led驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### led_open_id

`aos_dev_t *led_open_id(const char *name);`

- 功能描述:
  - 打卡led设备。
- 参数:
  - `name`: led驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### led_close

`int led_close(aos_dev_t *dev);`

- 功能描述:
  - 关闭led设备。
- 参数:
  - `dev`: led设备句柄。
- 返回值:
  - 0: 关闭成功。
  - -1: 关闭失败

### led_control

`int led_control(aos_dev_t *dev, int color, int on_time, int off_time)`

- 功能描述:
  - led控制。
- 参数:
  - `dev`: led设备句柄。
  - `color`：待控制的颜色
  - `on_time`: 打开的时长
  - `off_time`: 关闭的时长
- 返回值:
  - 0: 控制成功。
  - -1: 控制失败

### led_rgb_register

`void led_rgb_register(led_pin_config_t *config, int idx)`

- 功能描述:
  - led设备注册。
- 参数:
  - `config`: led设备配置
  - `idx`：led设备id
- 返回值:
  - 无

## 示例代码

### 注册led设备

```c
static led_pin_config_t led_config = {EXAMPLE_LED_R_PIN, EXAMPLE_LED_G_PIN, EXAMPLE_LED_B_PIN, 1};

void board_init(void)
{
    ......
    led_rgb_register(&led_config, 1);
    ......
}
```

### led控制

```c
void app_set_led_state(int state)
{
    switch (state)
    {
        case LED_LOADING:
            led_control(led_dev, COLOR_WHITE, 0, 0);
            break;
        case LED_NET_DOWN:
            led_control(led_dev, COLOR_WHITE, 50, 500);
            break;
        case LED_NET_READY:
            led_control(led_dev, COLOR_WHITE, 50, 3000);
            break;
        case LED_PLAYING:
            led_control(led_dev, COLOR_WHITE, 50, 1000);
            break;
        case LED_NET_CONFIG:
            led_control(led_dev, COLOR_WHITE, 50, 100);
            break;
        case LED_TALKING:
            led_control(led_dev, COLOR_WHITE, 0, 0);
            break;
        default:
            led_control(led_dev, COLOR_BLACK, 0, 0);
            break;
    }
}
```




## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。