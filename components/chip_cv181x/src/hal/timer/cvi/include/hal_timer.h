/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#include <stdint.h>
#include <dw_timer_ll.h>

typedef struct cvi_timer {
    uint8_t timer_id;
    dw_timer_regs_t *timer_base;
    uint8_t irq_num;
    uint8_t load_value;
    void (*callback)(struct cvi_timer *timer, void *args);
    void            *arg;
} cvi_timer_t;

#define CVI_PARAM_CHK(para, err)                        \
    do                                                  \
    {                                                   \
        if ((unsigned long)para == (unsigned long)NULL) \
        {                                               \
            return (err);                               \
        }                                               \
    } while (0)

#define CVI_PARAM_CHK_NORETVAL(para)                    \
    do                                                  \
    {                                                   \
        if ((unsigned long)para == (unsigned long)NULL) \
        {                                               \
            return;                                     \
        }                                               \
    } while (0)

uint32_t hal_timer_read_load(dw_timer_regs_t *timer_base);
void hal_timer_write_load(dw_timer_regs_t *timer_base, uint32_t value);
uint32_t hal_timer_get_current(dw_timer_regs_t *timer_base);
void hal_timer_set_enable(dw_timer_regs_t *timer_base);
void hal_timer_set_disable(dw_timer_regs_t *timer_base);
uint32_t hal_timer_get_enable(dw_timer_regs_t *timer_base);
void hal_timer_set_mode_free(dw_timer_regs_t *timer_base);
void hal_timer_set_mode_load(dw_timer_regs_t *timer_base);
uint32_t hal_timer_get_model(dw_timer_regs_t *timer_base);
void hal_timer_set_mask(dw_timer_regs_t *timer_base);
void hal_timer_set_unmask(dw_timer_regs_t *timer_base);
uint32_t hal_timer_get_mask(dw_timer_regs_t *timer_base);
void hal_timer_set_hardtrigger_en(dw_timer_regs_t *timer_base);
void hal_timer_set_hardtrigger_dis(dw_timer_regs_t *timer_base);
uint32_t hal_timer_get_hardtrigger(dw_timer_regs_t *timer_base);
uint32_t hal_timer_clear_irq(dw_timer_regs_t *timer_base);
uint32_t hal_timer_get_int_status(dw_timer_regs_t *timer_base);
void hal_timer_reset_register(dw_timer_regs_t *timer_base);
uint32_t hal_timer_general_active_after_mask(dw_timer_general_regs_t *timer_base);
uint32_t hal_timer_general_clear_irq(dw_timer_general_regs_t *timer_base);
uint32_t hal_timer_general_active_prior_mask(dw_timer_general_regs_t *timer_base);

#endif