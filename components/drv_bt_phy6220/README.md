# 概述

蓝牙驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
void board_init(void)
{
    extern int hci_driver_phy6220_register(int idx);
    extern int hci_h4_driver_init();
    hci_driver_phy6220_register(0);
    hci_h4_driver_init();
}
```
