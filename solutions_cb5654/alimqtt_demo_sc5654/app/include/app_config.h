/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/* 控制APP中是否输出LOGD的打印内容 */
#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG 1
//#undef CONFIG_DEBUG
#endif
#include <aos/log.h>

/* You can add user configuration items here. */
#define APP_FOTA_EN 1
#define CONFIG_TEST_LPMVAD 1
#define APP_LPM_CHECK_TIME 5  /* 进入低功耗的检查时间 */

#endif
