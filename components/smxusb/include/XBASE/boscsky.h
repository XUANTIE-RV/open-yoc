/*
* boscsky.h                                                  Version 4.2.1
*
* OS Porting Macros for SMX and No RTOS. See bos.h for other RTOSes
*
* DO NOT USE THESE OS PORTING MACROS AND FUNCTIONS IN APPLICATION CODE.
* These are only for use by SMX middleware modules such as smxFS, smxNS,
* smxUSB, etc. Your code should call BSP and kernel services directly.
*
* Copyright (c) 2010-2017 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Authors: Yingbo Hu and David Moore
*
*****************************************************************************/

#ifndef SB_OSCSKY_H
#define SB_OSCSKY_H

#ifdef  __cplusplus
extern "C"
{
#endif
#include "syslog.h"
//#include <csi_kernel.h>
#include <k_api.h>
#include <drv/irq.h>
#include <soc.h>
#include <stdlib.h>
#define TASK_TIME_QUANTA    100
#define TEST_TASK_STACK_SIZE     1024
#define CONFIG_USB_NUM 1
#define DWC2_USB_DMA_ALIGN	32

typedef void *k_mutex_handle_t;
typedef void *k_sem_handle_t;
typedef void *k_task_handle_t;
typedef void (*k_task_entry_t)(void *arg);


#define SB_OS_MUTEX_HANDLE  k_mutex_handle_t
#define SB_OS_SEM_HANDLE    k_sem_handle_t
#define SB_OS_TASK_HANDLE   k_task_handle_t
#define SB_OS_TASK_ID       k_task_handle_t
typedef int (* SB_OS_PISRFUNC)(uint iParameter);
typedef void (* SB_OS_PLSRFUNC)(uint iParameter);
//char * _ultoa(unsigned long uval, char *str, int radix);
BOOLEAN sb_OS_IntVectorInstall(int irq, uint iParameter, SB_OS_PISRFUNC func, SB_OS_PLSRFUNC lsr, const char *name);
//BOOLEAN sb_OS_TaskCreate(k_task_handle_t *handle, k_task_entry_t mainfunc, void *par, k_priority_t pri, uint32_t stacksz, const char *name);
BOOLEAN sb_OS_TaskCreate(SB_OS_TASK_HANDLE *handle, k_task_entry_t mainfunc, void *par,  uint32_t pri, uint32_t stacksz, const char *name);

void *sb_OS_MemAlignedAlloc(int32_t size, int32_t dma_align);
void sb_OS_MemAlignedFree(void *ptr);
void *sb_OS_MemAlignedRealloc(void *ptr, int32_t size, int32_t dma_align);
SB_OS_MUTEX_HANDLE sb_OS_MUTEX_NEW(const char *name);
void sb_OS_MUTEX_DELETE(SB_OS_MUTEX_HANDLE *mutex);
void sb_OS_MUTEX_GET_INF(SB_OS_MUTEX_HANDLE *mutex);
void sb_OS_MUTEX_GET_TMO(SB_OS_MUTEX_HANDLE *mutex, uint32_t tmo);
void sb_OS_MUTEX_RELEASE(SB_OS_MUTEX_HANDLE *mutex);
void sb_OS_SEM_CLEAR(SB_OS_SEM_HANDLE sem);
void sb_OS_SEM_WAIT_INF(SB_OS_SEM_HANDLE *sem);
int32_t sb_OS_SEM_WAIT_TMO(SB_OS_SEM_HANDLE *sem, int32_t tmo);
SB_OS_SEM_HANDLE sb_OS_SEM_NEW(int32_t thres, const char *name);
void sb_OS_SEM_DELETE(SB_OS_SEM_HANDLE *sem);
void sb_OS_SEM_SIGNAL(SB_OS_SEM_HANDLE *sem);
void sb_OS_SEM_SIGNAL_ISR(SB_OS_SEM_HANDLE *sem);
void sb_OS_TASK_DELETE(SB_OS_TASK_HANDLE *task);
SB_OS_TASK_HANDLE sb_OS_TASK_GET_CURRENT();
void sb_OS_TASK_FINISHED(void);
void sb_OS_TASK_PREEMPT_ALLOW(void);
void sb_OS_TASK_PREEMPT_BLOCK(void);
void sb_OS_TASK_START_PREEMPTIBLE(void);
void sb_OS_TASK_YIELD(void);
uint32_t sb_OS_TICKS_GET();
uint32_t sb_OS_MSEC_GET();
uint32_t sb_OS_STIME_GET();
int32_t sb_OS_WAIT_MSEC_MT(uint32_t ms);
uint32_t sb_OS_TASK_GET_MAX_PRI();
uint32_t sb_OS_TASK_GET_HIGH_PRI();
//#define SB_OS_TASK_PRI_MAX KPRIO_HIGH0
//#define SB_OS_TASK_PRI_HIGH KPRIO_HIGH0
#define SB_OS_TASK_PRI_MAX  sb_OS_TASK_GET_MAX_PRI()
#define SB_OS_TASK_PRI_HIGH sb_OS_TASK_GET_HIGH_PRI()


#define sb_OS_MEM_ALLOC(size)             sb_OS_MemAlignedAlloc(size, DWC2_USB_DMA_ALIGN)
#define sb_OS_MEM_FREE(ptr)               sb_OS_MemAlignedFree(ptr)
#define sb_OS_MEM_REALLOC(ptr, sz)        sb_OS_MemAlignedAlloc(ptr, size, DWC2_USB_DMA_ALIGN)

/* Mutex */
#define sb_OS_MUTEX_CHECK(m)              (m != NULL)
#define sb_OS_MUTEX_CREATE(m, name)       m = sb_OS_MUTEX_NEW(name)
//#define sb_OS_MUTEX_DELETE(m)             csi_kernel_mutex_del(*(m))
//#define sb_OS_MUTEX_GET_INF(m)            csi_kernel_mutex_lock(*(m), -1)
//#define sb_OS_MUTEX_GET_TMO(m, tmo)       csi_kernel_mutex_lock(*(m), tmo)
//#define sb_OS_MUTEX_RELEASE(m)            csi_kernel_mutex_unlock(*(m))

/* Semaphore */
#define sb_OS_SEM_CHECK(s)                (s != NULL)
//#define sb_OS_SEM_CLEAR(s)                ((ksem_t*)(s))->count = 0
//#define sb_OS_SEM_SIGNAL_ISR(s)           csi_kernel_sem_post(*(s))
//#define sb_OS_SEM_WAIT_INF(s)		  csi_kernel_sem_wait(*(s), -1)
//#define sb_OS_SEM_WAIT_TMO(s, tmo)        ((csi_kernel_sem_wait(*(s), tmo) == 0) ? 1 : 0)
//
#define sb_OS_SEM_CREATE(s, thres, name)        s = sb_OS_SEM_NEW(thres, name)
//#define sb_OS_SEM_DELETE(s)              csi_kernel_sem_del(*(s))
//#define sb_OS_SEM_SIGNAL(s)              csi_kernel_sem_post(*(s))

/* Task */
#define sb_OS_TASK_HANDLE_TO_ID(t)        t
#define sb_OS_TASK_CREATE_PREEMPTIBLE(handle, mainfunc, par, pri, stacksz, name) sb_OS_TaskCreate(handle, (k_task_entry_t)mainfunc, (void *)par, pri, (uint32_t)stacksz, (const char *)name)
//#define sb_OS_TASK_DELETE(t)              csi_kernel_task_del(*(t))
//#define sb_OS_TASK_FINISHED()             csi_kernel_task_exit()
//#define sb_OS_TASK_GET_CURRENT()          csi_kernel_task_get_cur()
//#define sb_OS_TASK_PREEMPT_ALLOW()        csi_kernel_sched_resume(0)
//#define sb_OS_TASK_PREEMPT_BLOCK()        csi_kernel_sched_suspend()
//#define sb_OS_TASK_START_PREEMPTIBLE()    csi_kernel_sched_resume(0);
//#define sb_OS_TASK_YIELD()                csi_kernel_task_yield()
//#define sb_OS_TASK_CREATE_PREEMPTIBLE(handle, mainfunc, par, pri, stacksz, name) sb_OS_TaskCreate((k_task_handle_t *)handle, (k_task_entry_t)mainfunc, (void *)par, (k_priority_t)pri, (uint32_t)stacksz, (const char *)name)

/* Interrupt */
#define sb_OS_INT_DISABLE()               drv_irq_disable(SU_SYNOPSYS_IRQ)
#define sb_OS_INT_ENABLE()                drv_irq_enable(SU_SYNOPSYS_IRQ)
#define sb_OS_IRQ_CLEAR(irq)              csi_vic_clear_pending_irq(irq)
#define sb_OS_IRQ_MASK(irq)               drv_irq_disable(irq)
#define sb_OS_IRQ_UNMASK(irq)             drv_irq_enable(irq)

#define sb_OS_ISR_CFUN_INSTALL(irq, par, func, lsr, name) sb_OS_IntVectorInstall(irq, par, func, lsr, name)
#define sb_OS_LSR_INVOKE(lsr, par)        lsr(par)      /* ??? */
/* Time and Delays */
//#define sb_OS_TICKS_GET()                 csi_kernel_get_ticks()
//#define sb_OS_MSEC_GET()                  (csi_kernel_tick2ms(csi_kernel_get_ticks()))
//#define sb_OS_STIME_GET()                 csi_kernel_get_ticks()

extern void mdelay(uint32_t ms);
extern void udelay(uint32_t us);
//#define sb_OS_WAIT_MSEC_MT(ms)            csi_kernel_delay_ms(ms)
#define sb_OS_WAIT_MSEC_POLL(ms)          mdelay(ms)
#define sb_OS_WAIT_USEC_POLL(us)          udelay(us)

/* Misc */
#define sb_OS_LINEAR_TO_POINTER(lin)      ((u8 *)(lin))
#define sb_OS_POINTER_TO_LINEAR(ptr)      ((u32)(ptr))

#if defined(__cplusplus)
}
#endif /* SMX_VERSION */  /*-----------------------------------------------*/
#endif
/* SB_OSCSKY_H */

