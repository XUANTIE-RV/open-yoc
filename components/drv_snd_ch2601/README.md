# 概述

`alkaid`声卡驱动

# 示例

```c
#include <devices/drv_snd_alkaid.h>

void board_yoc_init()
{
    snd_card_register(6300);
}
```
