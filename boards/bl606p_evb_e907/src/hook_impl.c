/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <k_api.h>
#include <drv/timer.h>
#include <csi_core.h>
/* auto define heap size */
extern size_t __heap_start;
extern size_t __heap_end;
extern size_t __heap2_start;
extern size_t __heap2_end;
#if defined(CONFIG_CPU_E907)
extern size_t __heap3_start;
extern size_t __heap3_end;
extern size_t __heap4_start;
extern size_t __heap4_end;
extern size_t __heap5_start;
extern size_t __heap5_end;
#endif
extern k_mm_region_t   g_mm_region[];

extern void soc_hw_timer_init(void);

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif

    size_t h_end = (size_t)(&__heap_end);
    size_t h_start = (size_t)(&__heap_start);

    if (h_start > h_end) {
        k_err_proc(RHINO_MM_POOL_SIZE_ERR);
    }
    /* auto define heap size */
    g_mm_region[0].len = (size_t)(&__heap_end) - (size_t)(&__heap_start);

    return;

    /* heap2 */
#if defined(CONFIG_CPU_E907) // E907 heap cache able
    h_end = (size_t)(&__heap2_end) | 0x40000000;
    h_start = (size_t)(&__heap2_start) | 0x40000000;
    g_mm_region[1].start = (uint8_t *)h_start;
#else
    h_end = (size_t)(&__heap2_end);
    h_start = (size_t)(&__heap2_start);
#endif

    if (h_start > h_end) {
        k_err_proc(RHINO_MM_POOL_SIZE_ERR);
    }
    /* auto define heap size */
    g_mm_region[1].len = (size_t)(&__heap2_end) - (size_t)(&__heap2_start);

#if defined(CONFIG_CPU_E907)
    /* heap3 */
    h_end = (size_t)(&__heap3_end);
    h_start = (size_t)(&__heap3_start);
    if (h_start > h_end) {
        k_err_proc(RHINO_MM_POOL_SIZE_ERR);
    }
    g_mm_region[2].len = (size_t)(&__heap3_end) - (size_t)(&__heap3_start);

    /* heap4 */
    h_end = (size_t)(&__heap4_end);
    h_start = (size_t)(&__heap4_start);
    if (h_start > h_end) {
        k_err_proc(RHINO_MM_POOL_SIZE_ERR);
    }
    g_mm_region[3].len = (size_t)(&__heap4_end) - (size_t)(&__heap4_start);

    /* heap5 */
    h_end = (size_t)(&__heap5_end);
    h_start = (size_t)(&__heap5_start);
    if (h_start > h_end) {
        k_err_proc(RHINO_MM_POOL_SIZE_ERR);
    }
    g_mm_region[4].len = (size_t)(&__heap5_end) - (size_t)(&__heap5_start);
#endif
}

__attribute__((weak)) void krhino_start_hook(void)
{
#if (RHINO_CONFIG_SYS_STATS > 0)
    krhino_task_sched_stats_reset();
#endif /* (RHINO_CONFIG_SYS_STATS > 0) */
}

__attribute__((weak)) void krhino_task_create_hook(ktask_t *task)
{

}

__attribute__((weak)) void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
}

__attribute__((weak)) void krhino_task_create_hook_lwip_thread_sem(ktask_t *task)
{
}

__attribute__((weak)) void krhino_task_abort_hook(ktask_t *task)
{

}

__attribute__((weak)) void  krhino_task_del_hook_lwip_thread_sem(ktask_t *task, res_free_t *arg)
{
}

__attribute__((weak)) void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
//#if (RHINO_CONFIG_SYS_STATS > 0)
//    krhino_task_sched_stats_get();
//#endif /* (RHINO_CONFIG_SYS_STATS > 0) */
}


__attribute__((weak)) void krhino_tick_hook(void)
{

}

__attribute__((weak)) void krhino_idle_pre_hook(void)
{
    extern void lpm_idle_pre_hook(void);
    lpm_idle_pre_hook();
}

__attribute__((weak)) int32_t _sleep_tick_get()
{
    if (is_klist_empty(&g_tick_head)) {
        return -1;
    }

    ktask_t * p_tcb  = krhino_list_entry(g_tick_head.next, ktask_t, tick_list);
    return  p_tcb->tick_match > g_tick_count ?  p_tcb->tick_match - g_tick_count : 0;
}

__attribute__((weak)) void krhino_idle_hook(void)
{
    extern void lpm_idle_hook(void);
    lpm_idle_hook();
}

__attribute__((weak)) void krhino_intrpt_hook(int irq)
{

}

__attribute__((weak)) void krhino_mm_alloc_hook(void *mem, size_t size)
{

}

#endif

__attribute__((weak)) void krhino_intrpt_enter_hook(int irq)
{

}

__attribute__((weak)) void krhino_intrpt_exit_hook(int irq)
{

}


extern size_t __heap2_start;
extern size_t __heap3_start;
extern size_t __heap4_start;
extern size_t __heap5_start;
/* auto define heap size */
k_mm_region_t g_mm_region[] = {
    {(uint8_t *)&__heap_start, (size_t)0},
//     {(uint8_t *)&__heap2_start, (size_t)0},
// #if defined(CONFIG_CPU_E907)
//     {(uint8_t *)&__heap3_start, (size_t)0},
//     {(uint8_t *)&__heap4_start, (size_t)0},
//     {(uint8_t *)&__heap5_start, (size_t)0},
// #endif
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);
