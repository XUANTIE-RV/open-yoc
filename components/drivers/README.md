## 概述

驱动组件，以csi接口为基础实现对应HAL层的设备结构体功能，目前支持uart、spiflash、eflash、iic等，并提供设备注册接口。本文以iic组件为例做介绍
**非csi接口直接实现，建议单独在componets下创建对应组件**

## 配置

无

## 接口列表

| 函数                 | 说明        |
| :------------------- | :---------- |
| rvm_hal_iic_open/iic_open_id | 打开iic     |
| rvm_hal_iic_close            | 关闭iic     |
| iic_control          | 控制iic     |
| rvm_iic_drv_register    | 注册iic设备 |
| rvm_hal_iic_config           | 配置iic |
| rvm_hal_iic_master_send           | master发送数据 |
| rvm_hal_iic_master_recv           | master接收数据 |
| rvm_hal_iic_slave_send           | slave发送数据 |
| rvm_hal_iic_slave_recv           | slave接收数据 |
| rvm_hal_iic_mem_write           | 向固定地址写入数据 |
| rvm_hal_iic_mem_read           | 从固定地址读取数据 |
| rvm_hal_iic_config_default           | iic默认配置 |

## 接口详细说明

### rvm_hal_iic_open

`rvm_dev_t *rvm_hal_iic_open(const char *name);`

- 功能描述:
  - 打卡iic设备。
- 参数:
  - `name`:  iic驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### iic_open_id

`rvm_dev_t *iic_open_id(const char *name);`

- 功能描述:
  - 打卡iic设备。
- 参数:
  - `name`: iic驱动名。
- 返回值:
  - 0: 打开成功。
  - -1: 打开失败。

### rvm_hal_iic_close

`int rvm_hal_iic_close(rvm_dev_t *dev);`

- 功能描述:
  - 关闭iic设备。
- 参数:
  - `dev`: iic设备句柄。
- 返回值:
  - 0: 关闭成功。
  - -1: 关闭失败

### iic_control

`int rvm_hal_iic_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config)`

- 功能描述:
  - iic配置。
- 参数:
  - `dev`: iic设备句柄。
  - `config`：iic的配置
- 返回值:
  - 0: 成功。
  - -1: 失败



### rvm_hal_iic_config_default

`void rvm_hal_iic_config_default(rvm_hal_iic_config_t *config)`

- 功能描述:
  - iic 默认配置。
- 参数:
  - `config`：iic的配置
- 返回值:
  - 无



### rvm_iic_drv_register

`void rvm_iic_drv_register(int idx)`

- 功能描述:
  - iic设备注册。
- 参数:
  - `idx`：iic设备id
- 返回值:
  - 无

### rvm_hal_iic_master_send

`int rvm_hal_iic_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
  - iic master发送数据。
- 参数:
  - `dev`: iic设备句柄。
  - `dev_addr`：iic的配置
  - `data`：数据指针
  - `size`：数据长度
  - `timeout`：超时时长
- 返回值:
  - 0: 成功。
  - -1: 失败



### rvm_hal_iic_master_recv

`int rvm_hal_iic_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
  - iic master接收数据。
- 参数:
  - `dev`: iic设备句柄。
  - `dev_addr`：iic的配置
  - `data`：数据指针
  - `size`：数据长度
  - `timeout`：超时时长
- 返回值:
  - 0: 成功。
  - -1: 失败



### rvm_hal_iic_slave_send

`int rvm_hal_iic_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
  - iic slave发送数据。
- 参数:
  - `dev`: iic设备句柄。
  - `dev_addr`：iic的配置
  - `data`：数据指针
  - `size`：数据长度
  - `timeout`：超时时长
- 返回值:
  - 0: 成功。
  - -1: 失败



### rvm_hal_iic_slave_recv

`int rvm_hal_iic_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
  - iic slave接收数据。
- 参数:
  - `dev`: iic设备句柄。
  - `dev_addr`：iic的配置
  - `data`：数据指针
  - `size`：数据长度
  - `timeout`：超时时长
- 返回值:
  - 0: 成功。
  - -1: 失败

## 示例

### 注册iic设备

```c
#include <devices/devicelist.h>

void board_yoc_init()
{
    rvm_iic_drv_register(0);
}
```

### 打开iic设备

```c
iic_dev = iic_open_id("iic", 0);
rvm_hal_iic_config_t config = {
    MODE_MASTER,
    BUS_SPEED_STANDARD,
    ADDR_7BIT,
    Addr_GND
};
rvm_hal_iic_config(iic_dev, &config);
```

### 发送数据

```c
void iic_write_byte(uint8_t reg_addr,uint8_t reg_data)
{
    uint8_t data[2] = {0};

    data[0] = reg_addr;
    data[1] = reg_data;

    rvm_hal_iic_master_send(iic_dev, Addr_GND, data, 2, -1);
}
```

### 读取数据

```c
int iic_readnreg(rvm_dev_t *dev, uint8_t write_cmd, uint8_t *rxbuf, int nbyte)
{
    rvm_hal_iic_config_t *config = (rvm_hal_iic_config_t *)dev->config;

    int ret = rvm_hal_iic_master_send(dev, config->slave_addr, &write_cmd, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    ret = rvm_hal_iic_master_recv(dev, config->slave_addr, rxbuf, nbyte, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    return 0;
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