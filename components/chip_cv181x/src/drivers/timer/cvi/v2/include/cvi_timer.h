/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __CVI_TIMER_H__
#define __CVI_TIMER_H__

#include <hal_timer.h>
#include <stdint.h>

int cvi_timer_init(cvi_timer_t *timer, uint32_t idx, dw_timer_regs_t *timer_base, uint8_t irq_num);
void cvi_timer_uninit(cvi_timer_t *timer);
int cvi_timer_start(cvi_timer_t *timer, uint32_t timeout_us);
void cvi_timer_stop(cvi_timer_t *timer);
uint32_t cvi_timer_get_remaining_value(cvi_timer_t *timer);
uint32_t cvi_timer_get_load_value(cvi_timer_t *timer);
bool cvi_timer_is_running(cvi_timer_t *timer);
void cvi_timer_irq_handler(unsigned int irqn, void *arg);
int cvi_timer_attach_callback(cvi_timer_t *timer, void *callback, void *arg);
void cvi_timer_detach_callback(cvi_timer_t *timer);

#endif