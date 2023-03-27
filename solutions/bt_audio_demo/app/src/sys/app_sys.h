/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _APP_SYS_H_
#define _APP_SYS_H_

void app_sys_init();

#define APPEXP_MODE_DEV 1
#define APPEXP_MODE_FCT 2
#define APPEXP_MODE_RLS 3
/**
 * @brief 系统异常处理
 * @param debug
 *        1:开发模式,启动软狗,异常后不复位
 *        2:开发模式,关闭软狗,产测模式
 *        其他:release模式，开启软狗及硬狗
 */
void app_sys_except_init(int debug);
#endif
