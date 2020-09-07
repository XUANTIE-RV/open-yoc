/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _CLOCK_ALARM_H_
#define _CLOCK_ALARM_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CLOCK_ALARM_ONCE = 0,       /* 仅一次*/
    CLOCK_ALARM_PERIOD_DAY,     /* 每天 */
    CLOCK_ALARM_PERIOD_WEEK,    /* 每周 */
    CLOCK_ALARM_PERIOD_WORKDAY, /* 工作日 */
    CLOCK_ALARM_PERIOD_DISABLE   /* 去使能闹钟 */
} clock_alarm_period_e;

typedef struct {
    uint8_t id; /* clock id */
    time_t time; /* 记录该闹钟的绝对时间 */
    bool enable; /* 记录该闹钟的使能情况 */
    clock_alarm_period_e period; /* once/day/week */
    void *alarm_cb; /* 闹铃的回调函数 */
} clock_alarm_ctx_t;

typedef struct {
    clock_alarm_period_e period; //once/day/week
    int week;
    int hour;/*小时，0~23 */
    int min;/* 分钟，0~59 */
    int sec;/* 秒，0~59 */
} clock_alarm_config_t;
typedef void (*clock_alarm_cb_t)(uint8_t clock_id);

#define ONE_DAY_TIME    24 * 3600      /* sec */
#define ONE_WEEK_TIME   7 * 24 * 3600  /* sec */
#define CLOCK_ALARM_NUM   5


/**
 * 初始化闹铃
 *
 * @param[in]  alarm_cb  闹铃回调callback,类型void (*clock_alarm_cb_t)(uint8_t clock_id)
 *
 * @return  0为成功，-1为失败（入参为null）
 */
int clock_alarm_init(clock_alarm_cb_t alarm_cb);

/**
 * 闹铃设置（新增/修改）
 *
 * @param[in]  id：闹铃id，新增闹铃设置为0，修改闹铃设置为对应闹铃id
 * @param[in]  cfg_time：配置的闹铃绝对时间,该值为NUlL表示删除指定id的闹钟
 *
 * @return  -3为修改的闹铃id超过最大id值；-2为新增闹铃重复；-1为新增闹铃已满；>0为闹铃id.
 */
int clock_alarm_set(uint8_t id, clock_alarm_config_t *cfg_time);

/**
 * 绝对时间的闹铃设置（新增/修改）
 *
 * @param[in]  id：闹铃id，新增闹铃设置为0，修改闹铃设置为对应闹铃id
 * @param[in]  abs_time：配置闹铃的绝对时间
 * @param[in]  period： 配置闹铃的周期性设置
 *
 * @return  -3为修改的闹铃id超过最大id值；-2为新增闹铃重复；-1为新增闹铃已满；>0为闹铃id.
 */
int clock_alarm_abstime_set(uint8_t id, time_t abs_time, clock_alarm_period_e period);

/**
 * 获取指定id的闹铃信息
 *
 * @param[in]  id：指定的闹铃
 *
 * @return  该id的闹铃信息.
 * 
 */
clock_alarm_ctx_t *clock_alarm_get(uint8_t id);

/**
 * 使能指定id的闹铃
 *
 * @param[in]  id：指定的闹铃
 * @param[in]  enable使能与否，0-不使能，1-使能
 *
 * @return  .
 */
void clock_alarm_enable(uint8_t id, uint8_t enable);

/**
 * 获取闹铃状态
 *
 * @param[in]  无
 *
 * @return  0为无闹铃；1为有闹铃配置； 2为提前唤醒准备闹的状态
 */
uint8_t clock_alarm_get_status(void);

#endif