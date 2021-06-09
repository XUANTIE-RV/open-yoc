## 概述
DHT11[数字温湿度传感器](https://baike.baidu.com/item/数字温湿度传感器)是一款含有已校准数字信号输出的温湿度[复合传感器](https://baike.baidu.com/item/复合传感器/5060200)，它应用专用的数字模块采集技术和温湿度传感技术，确保产品具有极高的可靠性和卓越的长期稳定性。传感器包括一个电阻式感湿元件和一个NTC测温元件，并与一个高性能8位单片机相连接。因此该产品具有品质卓越、超快响应、抗干扰能力强、性价比极高等优点。其精度湿度±5%RH， 温度±2℃，量程湿度5-95%RH， 温度-20~+60℃。

## 配置

无

## 接口列表

| 函数                       | 说明           |
| :------------------------- | :------------- |
| sensor_open/sensor_open_id | 打开sensor        |
| sensor_close               | 关闭sensor        |
| sensor_fetch               | 发现设备       |
| sensor_getvalue            | 获取传感器数据 |
| sensor_dht11_register      | 注册dht11设备  |

## 接口详细说明

### sensor_open

`aos_dev_t *sensor_open(const char *name);`

- 功能描述:
  - 打卡sensor设备。
- 参数:
  - `name`:  sensor驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### sensor_open_id

`aos_dev_t *sensor_open_id(const char *name);`

- 功能描述:
  - 打卡sensor设备。
- 参数:
  - `name`: sensor驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### sensor_close

`int sensor_close(aos_dev_t *dev);`

- 功能描述:
  - 关闭sensor设备。
- 参数:
  - `dev`: sensor设备句柄。
- 返回值:
  - 0: 关闭成功。
  - -1: 关闭失败

### sensor_fetch

`int sensor_close(aos_dev_t *dev);`

- 功能描述:
  - 发现sensor设备。
- 参数:
  - `dev`: sensor设备句柄。
- 返回值:
  - 0: 关闭成功。
  - -1: 关闭失败

### sensor_getvalue

`int sensor_close(aos_dev_t *dev, void *value, size_t size);`

- 功能描述:
  - 获取sensor设备的数据。
- 参数:
  - `dev`: sensor设备句柄。
  - `value`：数据指针
  - `size`：数据长度
- 返回值:
  - 0: 关闭成功。
  - -1: 关闭失败

### sensor_dht11_register

`void sensor_dht11_register(sensor_pin_config_t *config, int idx);`

- 功能描述:
  - sensor设备注册。
- 参数:
  - `config`: sensor设备配置
  - `idx`：sensor设备id
- 返回值:
  - 无

## 使用示例
```c
#include <devices/dht_sensor.h>

{
    dev_t *sensor_dev;
    sensor_dht11_t sval;

    /* 驱动初始化 */
    static sensor_pin_config_t dht11_config = {PA4};
    sensor_dht11_register(&dht11_config, 0);

    /* 打开设备 */
    sensor_dev = sensor_open_id(DHT11_DEV_NAME, 0);

    /* 读取数据 */
    sensor_fetch(sensor_dev);
    sensor_getvalue(sensor_dev, (void *)&sval, sizeof(sensor_dht11_t));

    /* 关闭驱动 */
    sensor_close(sensor_dev);

}
```
**注意**

受DHT11传感器的初始化时间限制，两次采集间隔必须大于2秒以上

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。