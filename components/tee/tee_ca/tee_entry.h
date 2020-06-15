/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_ENTRY_H
#define TEE_ENTRY_H

__attribute__((weak)) void csi_kernel_sched_suspend(void)
{
}

__attribute__((weak)) void csi_kernel_sched_resume(uint32_t sleep_ticks)
{
}

/**
  \brief       Lock TEE entry to prevent agaist host application call TEE again
  \note        The MICRO is a porting interface
*/
#define TEE_ENTRY_LOCK()   do { csi_kernel_sched_suspend();} while(0)

/**
  \brief       Unlock TEE entry to prevent agaist host application call TEE again
  \note        The MICRO is a porting interface
*/
#define TEE_ENTRY_UNLOCK() do { csi_kernel_sched_resume((uint32_t)0);} while(0)

#endif

