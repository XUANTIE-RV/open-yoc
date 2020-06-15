# 概述

IPCFlash驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/ipcflash.h>

void board_yoc_init()
{
    ipcflash_csky_register(0);
}
```
