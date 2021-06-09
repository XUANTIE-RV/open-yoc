## 概述

提供闹铃功能，用户可通过设置“仅响一次”或“每天/每周”或“工作日”等模式，来达到到时提醒功能，精度能达到秒级 提供闹铃设置修改、闹铃获取、闹铃使能和闹铃处理回调。

## 组件安装
```bash
yoc init
yoc install clock_alarm
```

## 配置
无。

## 接口列表

| 函数 | 说明 |
| :--- | :--- |
| clock_alarm_init | 初始化闹铃 |
| clock_alarm_set | 闹铃设置（新增/修改） |
| clock_alarm_get | 获取指定id的闹铃信息 |
| clock_alarm_enable | 使能指定id的闹铃  |
| clock_alarm_get_status | 获取闹铃的状态 |

## 接口详细说明

### clock_alarm_init
`void clock_alarm_init(clock_alarm_cb_t alarm_cb);`

- 功能描述:
   - 初始化闹铃 。

- 参数:
   - `alarm_cb`: 闹铃回调callback。typedef void (*clock_alarm_cb_t)(uint8_t clock_id);。

- 返回值:
   - 无

### clock_alarm_set
` int clock_alarm_set(uint8_t id, clock_alarm_config_t *cfg_time);`

- 功能描述:
   - 闹铃设置（新增/修改）。

- 参数:
   - `id`: 闹铃id，新增闹铃设置为0，修改闹铃设置为对应闹铃id。
   - `cfg_time`：配置的闹铃绝对时间。

- 返回值:
   - -3: 修改的闹铃id超过最大id值。
   - -2: 新增闹铃重复。
   - -1: 新增闹铃已满。
   - >0: 闹铃id。

### clock_alarm_get
`clock_alarm_ctx_t *clock_alarm_get(uint8_t id);`

- 功能描述:
   - 获取指定id的闹铃信息。

- 参数:
  - `id`: 指定的闹铃。

- 返回值:
   - clock_alarm_ctx_t 该id的闹铃信息。

### clock_alarm_enable
`void clock_alarm_enable(uint8_t id, uint8_t enable);`

- 功能描述:
   - 使能指定id的闹铃。

- 参数:
   - `id`: 指定的闹铃。
   - `enable`: 使能与否，0-不使能，1-使能。

- 返回值:
   - 无

### clock_alarm_get_status
`uint8_t clock_alarm_get_status(void);`

- 功能描述:
   - 获取闹铃的状态，供低功耗检查闹铃是否允许进入低功耗（提前唤醒模式）。

- 参数:
   - 无

- 返回值:
   - 0: 不允许进入低功耗。
   - 1: 允许。

## 示例

```c
#include <clock_alarm.h>


static void app_clock_alarm_cb(uint8_t clock_id)
{
    clock_alarm_ctx_t *clock_ctx;

    LOGI(TAG, "clock_id %d alarm cb handle", clock_id);
    clock_ctx = clock_alarm_get(clock_id);

    if (clock_ctx->period == CLOCK_ALARM_ONCE) {
        local_audio_play(LOCAL_AUDIO_HELLO);
    } else if (clock_ctx->period == CLOCK_ALARM_PERIOD_WORKDAY) {
        local_audio_play(LOCAL_AUDIO_OK);
    }
}


void clock_alarm_test(int hour, int min, int sec , int period)
{
    uint8_t clock_id = 0;
    clock_alarm_config_t cfg_time;

    cfg_time.hour = hour;
    cfg_time.min = min;
    cfg_time.sec = sec;
    cfg_time.period = period;

    clock_id = clock_alarm_set(0, cfg_time);
    if (clock_id > 0) {
        clock_alarm_enable(clock_id，1);
    }

    clock_alarm_config_t clock_info;
    clock_info = clock_alarm_get(clock_id);

    cfg_time.hour += 8;
    clock_id = clock_alarm_set(clock_id, cfg_time);
}
```

## 诊断错误码
无。

## 运行资源

无。

## 依赖资源
  - csi
  - aos
  - kv

## 组件参考
无。