/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

#include "k_api.h"
#include "cpu_pwr_config.h"

//#if RHINO_CONFIG_CPU_PWR_MGMT

#include "cpu_pwr_hal_lib.h"
#include "cpu_pwr_lib.h"
#include "cpu_pwr_api.h"
#include "k_atomic.h"
#if RHINO_CONFIG_CPU_TICKLESS
#include "cpu_tickless.h"
#endif

static CPU_IDLE_MODE cpu_pwr_idle_mode = CPU_IDLE_MODE_RUN;
static int           cpu_pwr_init_flag = 0;

#if RHINO_CONFIG_PWR_DEBUG
int pwr_debug_level = 0x8;
#endif /* RHINO_CONFIG_PWR_DEBUG */

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
void (*_func_cpu_pwr_idle_enter_hook) (void);
void (*_func_cpu_pwr_int_enter_hook)  (void);
void (*_func_cpu_pwr_int_exit_hook)  (void);
#endif /* RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT */

kstat_t (*_func_tickless_ignore_list_add) (klist_t *pTickNode);
void (* _func_cpu_tickless_down) (void);
void (* _func_cpu_tickless_up) (void);

/*
* cpu_pwrmgmt_init() is the entry of whole cpu power manamgement system.
*/

void cpu_pwrmgmt_init(void)
    {
    /* avoid reentry */

    if (cpu_pwr_init_flag)
        return;

    /* create cpu power management framework and cpu topology structure */

    cpu_pwr_hal_lib_init();

#if RHINO_CONFIG_CPU_TICKLESS

    /* init cpu tickless framework */

    tickless_init();

#endif /* RHINO_CONFIG_CPU_TICKLESS */

    /* save the job complete flag */

    cpu_pwr_init_flag = 1;
    }


/*
* cpu_pwr_enabled() is enbaled tickless in runtime.
*/
void cpu_pwr_enabled (int enabled)
{
    cpu_pwr_init_flag = enabled;
}

/*
* cpu_pwr_down() is invoked from idle task when it detects that the kernel
* can go idle.  cpu_pwr_down() checks the current cpu idle mode and put
* cpu into lower power mode according different idle mode.
*/
static int g_cpu_pwr_down = 0;
void cpu_pwr_down (void)
    {
    if (cpu_pwr_init_flag == 0)
        return;

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
    if (_func_cpu_pwr_idle_enter_hook != NULL)
        {
        _func_cpu_pwr_idle_enter_hook ();
        }
#endif

    if ((cpu_pwr_idle_mode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_down != NULL))
        {
        _func_cpu_tickless_down ();
        }
    else if (cpu_pwr_idle_mode == CPU_IDLE_MODE_SLEEP)
        {
        (void)cpu_pwr_c_state_set(CPU_CSTATE_C1);
        }
    else /* CPU_IDLE_MODE_RUN */
        {
        return;
        }

        g_cpu_pwr_down = 1;
    }

/*
* cpu_pwr_up() is invoked from all interrupt and exception stubs to wakeup
* the CPU in case it was powered off.  This is necessary so that the kernel
* can adjust its tick count to reflect the length of time that it was
* powered off. 
*/

void cpu_pwr_up (void)
    {
    if (cpu_pwr_init_flag == 0 && !g_cpu_pwr_down)
        return;
    
    (void)cpu_pwr_c_state_set(CPU_CSTATE_C0);

    /* execute tickless up hook if existed */

    if ((cpu_pwr_idle_mode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_up != NULL))
        {              
        _func_cpu_tickless_up();
        }

#if RHINO_CONFIG_CPU_PWR_P_STATE_SUPPORT
    if (_func_cpu_pwr_int_enter_hook != NULL)
        _func_cpu_pwr_int_enter_hook();
#endif
    g_cpu_pwr_down = 0;
    }

/*
* cpu_pwr_idle_mode_set() sets the power mode used when CPUs are idle. The following 
* <mode> settings are supported:
* CPU_IDLE_MODE_RUN     - CPUs are continuously running when idle
* CPU_IDLE_MODE_SLEEP   - system clock interupts occur at every tick.
* CPU_IDLE_MODE_TICKLESS- CPUs sleep for multiple tick durations, system tick
*                         interupts occur only when kernel event expirations
*                         occur (timout, delays, etc). 
*
* RETURNS: RHINO_SUCCESS, or RHINO_PWR_MGMT_ERR in case of failure
*/

kstat_t cpu_pwr_idle_mode_set 
    (
    CPU_IDLE_MODE curMode
    )
    {
#ifdef RHINO_CONFIG_SMP    
    CPU_IDLE_MODE preMode;
#endif /* RHINO_CONFIG_SMP */

    if (curMode > CPU_IDLE_MODE_TICKLESS)
        {
        return (RHINO_PWR_MGMT_ERR);
        }

    if ((curMode == CPU_IDLE_MODE_TICKLESS) &&
        (_func_cpu_tickless_down == NULL))
        {
        return (RHINO_PWR_MGMT_ERR);
        }

    if (curMode == cpu_pwr_idle_mode)
        {
        return (RHINO_SUCCESS);
        }

#ifdef RHINO_CONFIG_SMP
    preMode = cpu_pwr_idle_mode;
#endif /* RHINO_CONFIG_SMP */

    cpu_pwr_idle_mode = curMode;

#ifdef RHINO_CONFIG_SMP

    if ((preMode == CPU_IDLE_MODE_RUN && curMode == CPU_IDLE_MODE_SLEEP) ||
        (preMode == CPU_IDLE_MODE_RUN && curMode == CPU_IDLE_MODE_TICKLESS))
        {
        /* do not send IPI */
        }
    else
        {
        cpu_pwr_send_ipi_to_all ();
        }

#endif /* RHINO_CONFIG_SMP */

    return RHINO_SUCCESS;
    }

/*
* cpu_pwr_idle_mode_get() returns the current idle CPU power mode
*
* RETURNS: current CPU idle mode
*/

kstat_t cpu_pwr_idle_mode_get (CPU_IDLE_MODE * pMode)
    {
    * pMode = cpu_pwr_idle_mode;

    return RHINO_SUCCESS;
    }

//#endif /* RHINO_CONFIG_CPU_PWR_MGMT */
