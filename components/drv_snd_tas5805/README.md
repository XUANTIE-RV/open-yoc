# 概述

`tas5805`数字PA驱动

# 示例

```c
#include <devices/drv_snd_tas5805.h>
#include <pin.h>

void board_yoc_init()
{
    sound_config_t config = {
        .i2s_id = 3,
        .i2c_id = 0,
        .pin    = PA11,
    };

    tas5805_snd_card_register(&config);
}
```
