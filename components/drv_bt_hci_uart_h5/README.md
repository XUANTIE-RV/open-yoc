## 概述

本组件是基于UART的H5协议蓝牙驱动组件。使用该组件，可以驱动支持H5协议的蓝牙Controller芯片，实现蓝牙通信功能。

## 配置

默认配置文件位于config/hci_uart_h5/config.h，通常情况下不需要要修改，如有修改的需求，可以定义在solution或者相应的芯片组件中。

```c
//H5驱动使用接收任务栈的大小
#ifndef CONFIG_HCI_UART_H5_TASK_STACK_SIZE
#define CONFIG_HCI_UART_H5_TASK_STACK_SIZE (2048)
#endif
//H5驱动使用接收任务的优先级
#ifndef CONFIG_HCI_UART_H5_TASK_PRIO
#define CONFIG_HCI_UART_H5_TASK_PRIO (6)
#endif

//配置为0, 默认蓝牙控制器需要支持广播模式和扫描模式自动切换。配置为1，BLE Mesh协议栈会完成切换动作，但效率会很低，不建议配置。
#define CONFIG_BT_ADV_SCAN_SCHEDULE_IN_HOST: 0

```

## 接口列表

| 函数                    | 说明           |
| :---------------------- | :------------- |
| bt_hci_uart_h5_register | H5驱动注册接口 |

## 接口详细说明

```C
/**
 * @brief  register bluetooth driver of h5 
 * @param  [in] config
 * @return  
 */
void bt_hci_uart_h5_register(int uart_idx, uart_config_t config);
```

H5驱动注册接口

- 参数
  - uart_idx：按键id
  - config：uart配置参数，定义同<devices/uart.h>

返回值：

- 无

## 示例

### 驱动注册

```c
#include <devices/uart.h>

void board_yoc_init()
{
    //配置Uart管脚复用
    drv_pinmux_config(PIN_XXX, FUNC_UART_TX_XXX)
    drv_pinmux_config(PIN_YYY, FUNC_UART_RX_YYY)

    //获取默认uart配置
    uart_config_t uart_config;
    uart_config_default(&uart_config);

    //根据设计，配置uart参数
    uart_config.baud_rate = 115200;
    
    //注册H5驱动
    extern void bt_hci_uart_h5_register(int uart_idx, uart_config_t config);
    bt_hci_uart_h5_register(0, uart_config);

    //H5驱动初始化
    extern int hci_h5_driver_init();
    hci_h5_driver_init();
}
```

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

无

## 组件参考

无。