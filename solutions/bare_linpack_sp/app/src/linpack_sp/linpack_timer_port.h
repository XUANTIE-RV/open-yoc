/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     linpack_timer_port.h
 * @brief    the head file for the linpack
 * @version  V1.0
 * @date     20. July 2016
 ******************************************************************************/
#ifndef CORE_SYSTIMER_H_
#define CORE_SYSTIMER_H_

void Timer_Open();
void Timer_Start();
void Timer_Stop();
void Timer_Close();
unsigned long long  clock_gettime();
long    time();

#endif /* CORE_SYSTIMER_H_ */
