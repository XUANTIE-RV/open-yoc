/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#if 0
#include <k_api.h>

#undef  PSR_SP
#define PSR_SP  (1UL << 29)
static ktask_t *tee_caller_task = NULL;

static inline uint32_t getcurrentpsr(void)
{
   uint32_t flags = 0;

    __asm__ __volatile__(
        "NOP\n" /* TODO */
    );

   return flags;
}

static inline void clear_psr_sp(void)
{
    __asm__ __volatile__ (
        "NOP\n" /* TODO */
    );
}

static inline void set_psr_sp(void)
{
    __asm__ __volatile__ (
        "NOP\n" /* TODO */
    );
}

void csky_get_tee_caller_task(void)
{
    uint32_t temp_psr;

    temp_psr = getcurrentpsr();
    if (temp_psr & PSR_SP) {
       tee_caller_task = (tee_caller_task == NULL) ? g_active_task[cpu_cur_get()] : tee_caller_task;
    }
}

void csky_deal_tee_caller_task(void)
{
    uint32_t temp_psr;

    temp_psr = getcurrentpsr();
    if (temp_psr & PSR_SP) {
        if (tee_caller_task != NULL) {
            if (tee_caller_task == g_active_task[cpu_cur_get()]) {
                tee_caller_task = NULL;
            } else {
                clear_psr_sp();
            }
        }
    } else {
        if (tee_caller_task != NULL) {
            if (tee_caller_task == g_active_task[cpu_cur_get()]) {
                tee_caller_task = NULL;
                set_psr_sp();
            }
        }
    }
}
#endif
