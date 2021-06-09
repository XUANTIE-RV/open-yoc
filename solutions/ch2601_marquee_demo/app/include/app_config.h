/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/* 控制APP中是否输出LOGD的打印内容 */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 0
//#undef CONFIG_DEBUG
#endif

/* You can add user configuration items here. */

#define CONFIG_PWM_MODE
//#define CONFIG_GPIO_MODE
#define CONSOLE_UART_IDX 0

#endif
