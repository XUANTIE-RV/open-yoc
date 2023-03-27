# 概述

蓝牙驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
void bt_w800_register()
{
    LOGD(TAG,"%s", __FUNCTION__);
    w800_board_init();
    rvm_driver_register(&h4_driver.drv, NULL, 0);
}
```