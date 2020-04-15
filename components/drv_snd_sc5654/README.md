# 概述

`SC5654`声卡驱动

# 示例

```c
#include <devices/drv_snd_sc5654.h>

void board_yoc_init()
{
    sound_drv_sc5654_register(90, -1, 90);
}
```
