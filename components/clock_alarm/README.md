# 概述
本目录提供闹铃功能，用户可通过设置“仅响一次”或“每天/每周”或“工作日”等模式，来达到到时提醒功能，精度能达到秒级
提供闹铃设置修改、闹铃获取、闹铃使能和闹铃处理回调

初始化闹铃
alarm_cb：闹铃回调callback  typedef void (*clock_alarm_cb_t)(uint8_t clock_id);
void clock_alarm_init(clock_alarm_cb_t alarm_cb);

闹铃设置（新增/修改）
id：闹铃id，新增闹铃设置为0，修改闹铃设置为对应闹铃id
cfg_time：配置的闹铃绝对时间
返回值：-3为修改的闹铃id超过最大id值；-2为新增闹铃重复；-1为新增闹铃已满；>0为闹铃id
int clock_alarm_set(uint8_t id, clock_alarm_config_t *cfg_time);

获取指定id的闹铃信息
id：指定的闹铃
返回值：该id的闹铃信息
clock_alarm_ctx_t *clock_alarm_get(uint8_t id);

使能指定id的闹铃
id：指定的闹铃
enable：使能与否，0-不使能，1-使能
void clock_alarm_enable(uint8_t id, uint8_t enable);

获取闹铃的状态，供低功耗检查闹铃是否允许进入低功耗（提前唤醒模式）
返回值：0为不允许进入低功耗；1为允许
uint8_t clock_alarm_get_status(void);

# 示例

包含头文件：

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