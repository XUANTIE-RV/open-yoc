/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

/*
This file provides support for cpu tickless-idle.

Tickless-idle eliminates unnecessary timer interrupts when the processor is
otherwise idle.  When the processor exits the idle state, it advances time by
the number of ticks that elapsed during the idle period. For SMP, the tickless
idle is entered when all CPUs are ready.
*/

#include "k_api.h"
#include "cpu_pwr_config.h"

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)
#if (RHINO_CONFIG_CPU_TICKLESS > 0) 

#include "stdlib.h"
#include "cpu_pwr_api.h"
#include "cpu_pwr_lib.h"
#include "cpu_pwr_hal_lib.h"
#include "k_critical.h"
#include "pwr_debug.h"
#include "k_ffs.h"
#include "cpu_tickless.h"

//#define CPU_TICKLESS_DBG

/* define */

/* 100 * 365 * 24 * 3600 * 1000 * 1000 = 0xB342EB7C38000 */
#define TIME_100_YEARS_IN_US    0xB342EB7C38000ULL

typedef struct
    {
    DL_NODE    node;
    klist_t  * pTickNode;
    } TICKLESS_OBJ;

static DL_LIST      g_ignore_list;
static uint32_t     tickless_ctate_mask;	/* C-states support set */
static cpu_cstate_t c_state_entered;
static int          is_current_tickless = FALSE;

static uint32_t          cStateConfig[CPUS_NUM_MAX];
static uint32_t          cStateLatency[CPUS_NUM_MAX][CPU_CSTATE_MAX + 1];
static one_shot_timer_t* cStateOneShotTimer[CPU_CSTATE_MAX];

tick_t       g_tick_sleep;

/*
 * tickless_live_cpu_set, after tickless module is initialized,
 * all the cores will set bit in tickless_live_cpu_set, each core
 * will clear the relative bit in tickless_live_cpu_set before
 * it is going to enter tickless idle mode, when all the cores
 * clears the bit in tickless_live_cpu_set which means all cores
 * want to enter tickless idle mode, the last core will close
 * the system tick interrupt and start a one-shot plan, and
 * then enter tickless idle mode.
 *
 * On a big-little system, there are some big cores(higher
 * frequency) and some little cores(lower frequency), normally
 * all big cores belong same cluster and all little cores belong
 * same cluster, and these two clusters could has different
 * Cx state, say big cluster could in C2 while little cluster
 * could in C0. For tickless mode, only when all cores want
 * to enter tickless idle mode, the system could enter tickless
 * mode, all cores here means include both big cores and little
 * cores.
 *
 * In the struct cpu_pwr_t, it also has a value named childLiveCpuSet,
 * that is not same thing as tickless_live_cpu_set, different cluster
 * could has different Cx state in one CPU package, childLiveCpuSet
 * in cpu_pwr_t is used to track idle state of each cores under same
 * cluster in this example.
 */

static cpuset_t      tickless_live_cpu_set = 0;
static kspinlock_t   ticklessSpin;

#ifdef CPU_TICKLESS_DBG
static uint32_t tickless_enter_count[CPU_CSTATE_MAX + 1];
#endif

/* import */

extern void     systick_suspend(void);
extern void     systick_resume(void);

/* forward declare */

static kstat_t  tickless_timer_init  (void);
static void     tickless_enter     (void);
static void     tickless_exit      (void);
static void     tickless_enter_check(uint32_t cpuIdx, uint32_t cStatesCfg,
                            uint64_t * pSleepTime,cpu_cstate_t * pCStateToSet);
static kstat_t  tickless_one_shot_start     (uint64_t sleepTime,cpu_cstate_t c_state_to_enter);
static tick_t   tickless_one_shot_stop     (cpu_cstate_t c_state_current);
static tick_t   tickless_sleep_ticks_get(void);
static kstat_t  tickless_ignore_list_add (klist_t * pTickNode);
static void     tickless_announce_n(tick_t nTicks);
static kstat_t  tickless_c_state_latency_init(uint32_t cpuIdx, uint32_t * pLatency);

#ifdef CPU_TICKLESS_DBG
static void     tickless_info_show (void);
static void     tickless_ignore_list_show(void);
static void     tickless_gticklist_show(void);
#endif /* CPU_TICKLESS_DBG */

/*
* This routine is supplied by this module for board/platform,
* it is responsibility of board/platform to provide information
* that which C state is supported with tickless mode.
*/

void tickless_c_states_add(uint32_t c_state_set)
{
    tickless_ctate_mask |= c_state_set;
}

/*
* This routine installs the static routines tickless_enter(),
* tickless_exit(), into cpu power management facility.
*/

kstat_t tickless_init(void)
    {
    uint32_t cpuIdx;

    if (tickless_ctate_mask == 0)
        {
#ifdef CPU_TICKLESS_DBG
        printf("tickless_ctate_mask is zero\n");
#endif /* CPU_TICKLESS_DBG */        
        return RHINO_PWR_MGMT_ERR; 
        }

    /* initialize the spinlock */

    krhino_spin_init(&ticklessSpin);

    DLL_INIT(&g_ignore_list);
   
    /* initialize the one-shot timers */

    if (tickless_timer_init() != RHINO_SUCCESS)
        {
        return RHINO_PWR_MGMT_ERR;
        }

#ifdef CPU_TICKLESS_DBG
    printf("input tickless_ctate_mask = 0x%x\n",tickless_ctate_mask);
#endif /* CPU_TICKLESS_DBG */

    /* set cpu idle mode to run until initialization completes */

    cpu_pwr_idle_mode_set (CPU_IDLE_MODE_RUN);

    for (cpuIdx = 0; cpuIdx < CPUS_NUM_MAX; cpuIdx++)
        {
        if (cpu_pwr_c_state_capability_get (cpuIdx, 
            &cStateConfig[cpuIdx]) != RHINO_SUCCESS)
            {
#ifdef CPU_TICKLESS_DBG            
            printf("cpu_pwr_c_state_capability_get() failed\n");
#endif /* CPU_TICKLESS_DBG */

            return RHINO_PWR_MGMT_ERR;
            }

        cStateConfig[cpuIdx] &= tickless_ctate_mask;

        tickless_c_state_latency_init (cpuIdx, cStateLatency[cpuIdx]);

        /* set live flag for all cores */

        CPUSET_SET (tickless_live_cpu_set, cpuIdx);
        }

    _func_cpu_tickless_down         = tickless_enter;
    _func_cpu_tickless_up           = tickless_exit;
    
    _func_tickless_ignore_list_add  = tickless_ignore_list_add;

    /*set cpu idle mode to tickless mode now */

    cpu_pwr_idle_mode_set (CPU_IDLE_MODE_TICKLESS);

#ifdef RHINO_CONFIG_SMP

    /* 
     * Under SMP environment, it is possible that when current core
     * comes here, the other cores has already enter C1 idle mode,
     * when tickless_enter() is called on other cores,
     * thus tickless_live_cpu_set will never be equal to 0 because
     * those cores in C1 idle kstat_t has no chance to clear bit
     * in tickless_live_cpu_set, and it will lead system never enter
     * tickless mode. 
     *
     * To fix this issue, we send all cores(exclude self) a IPI
     * interrupt to make sure all cores wake up and clear bit in
     * tickless_live_cpu_set when it call tickless_enter().
     */

    cpu_pwr_send_ipi_to_all ();

#endif

    return (RHINO_SUCCESS);
    }

static kstat_t tickless_timer_init (void)
    {
    uint32_t idx;

    for(idx = 0; idx < CPU_CSTATE_MAX; idx++)
        {
        if (cStateOneShotTimer[idx] != NULL)
            {
            if (cStateOneShotTimer[idx]->one_shot_init() != RHINO_SUCCESS)
                {
#ifdef CPU_TICKLESS_DBG
                printf("cStateOneShotTimer[%d].one_shot_init() failed\n",idx);
#endif /* CPU_TICKLESS_DBG */

                return RHINO_PWR_MGMT_ERR;
                }
#ifdef CPU_TICKLESS_DBG            
            else
                {
                printf("cStateOneShotTimer[%d].one_shot_init() ok\n",idx);
                }
#endif                
            }
        }

    return RHINO_SUCCESS;
    }

static kstat_t tickless_one_shot_start
    (
    uint64_t     sleepTime,
    cpu_cstate_t c_state_to_enter
    )
    {
    if (cStateOneShotTimer[c_state_to_enter]->one_shot_start(sleepTime)
        != RHINO_SUCCESS)
        {
        PWR_DBG (DBG_INFO, "start one shot(%lld ms) fail\n",sleepTime);

        return (RHINO_PWR_MGMT_ERR);
        }    

    return (RHINO_SUCCESS);
    }

static tick_t tickless_one_shot_stop 
    (
    cpu_cstate_t c_state_current
    )
    {
    tick_t     nTicks;
    uint64_t   passedMicrosec;
    uint32_t   howmanyUsPerTick = 1000000/RHINO_CONFIG_TICKS_PER_SECOND;
    static uint64_t usRemain = 0;

    if(cStateOneShotTimer[c_state_current]->one_shot_stop(&passedMicrosec)
        != RHINO_SUCCESS)
        {
        PWR_DBG (DBG_ERR, "timerOneShotCancel fail\n");
       
        return 0;
        }
 
    nTicks = (tick_t) ((passedMicrosec * RHINO_CONFIG_TICKS_PER_SECOND) / (1000000));  

    /* fixs ticks drift issue */

    usRemain += passedMicrosec % howmanyUsPerTick;
    if (usRemain >= howmanyUsPerTick)
        {
        nTicks++;
        usRemain -= howmanyUsPerTick;
        }

    return nTicks;
    }

/*
* tickless_c_state_latency_init() sets the latency times for entering and
* exiting the various C states for idle power management. Latency table
* is only used when CPU Power Manager is present.
*
* RETURNS: RHINO_SUCCESS or RHINO_PWR_MGMT_ERR when failed.
*/

static kstat_t tickless_c_state_latency_init
    (
    uint32_t         cpuIdx,    /* CPU index */
    uint32_t         pLatency[] /* Latency array */
    )
    {
    cpu_cstate_t     cstate;
    uint32_t         cstate_all;
    uint32_t         latency;
    
    if (cpu_pwr_c_state_capability_get(cpuIdx, &cstate_all) != RHINO_SUCCESS)
        {
        return (RHINO_PWR_MGMT_ERR);
        }

    for (cstate = CPU_CSTATE_C0; cstate <= CPU_CSTATE_MAX; cstate++)
        {
        pLatency[cstate] = 0;
        
        if (cstate_all & (1 << cstate))
            {
            latency = cpu_pwr_c_state_latency_get(cpuIdx, cstate);

            if (latency == (uint32_t) CPU_LATENCY_UNKNOW)
                {
                pLatency[cstate] = 0;
                }
            else
                {
                pLatency[cstate] = latency;
                }
            }
        }

    return (RHINO_SUCCESS);
    }

/*
* tickless_enter_check() calculates the amount of time until the next kernel
* time event, adjusts it based on configuration and CPU capabilities.
* It also deteremines the CPU idle state (C-state) to enter.
*
* RETURNS: N/A
*/

static void tickless_enter_check
    (
    uint32_t          cpuIdx,       /* IN, current CPU */
    uint32_t          cStateCfg,    /* IN, enabled C states */
    uint64_t *        pSleepTime,   /* OUT, time to next event in us */
    cpu_cstate_t *    pCStateToSet  /* OUT, C state to set */
    )
    {
    uint32_t          cState;
    uint64_t          sleep_time_us;            /* sleep time in microseconds */
    uint64_t          one_shot_max_us_support;  /* max one shot time support */
    tick_t            nTicks;

#ifdef CPU_TICKLESS_DBG
    tickless_ignore_list_show ();
    tickless_gticklist_show ();
#endif /* CPU_TICKLESS_DBG */

    nTicks = tickless_sleep_ticks_get();

    if (nTicks == RHINO_WAIT_FOREVER)
        {
        sleep_time_us = TIME_100_YEARS_IN_US;
        }
    else
        {
        sleep_time_us = 1000000ull * nTicks / RHINO_CONFIG_TICKS_PER_SECOND;     
        }

    /* get max valid Cx from cStateCfg */
    
    cState = FFS_MSB (cStateCfg) - 1;

    while (cState > 0)
        {
        /* check if latency is less than idle time */

        if (cStateLatency[cpuIdx][cState] < sleep_time_us)
            {
            /* hit! save cState as the target C state */
            
            * pCStateToSet = (cpu_cstate_t)cState;

            /* find the target, break here */

            break;
            }

        /* clear the current Cx from cStateCfg */

        cStateCfg &= ~(1 << cState);

        /* get max valid Cx from cStateCfg */

        cState = FFS_MSB (cStateCfg) - 1;
        }    

    one_shot_max_us_support = 0;
    
    if ((cStateOneShotTimer[cState] != NULL )&& 
        (cStateOneShotTimer[cState]->one_shot_seconds_max != NULL))
        {
        one_shot_max_us_support = 
            1000000 * cStateOneShotTimer[cState]->one_shot_seconds_max();
        }

    /*
     * if request sleeptime is longer than support, cut it down
     * to max support value.
     */

    if (sleep_time_us > one_shot_max_us_support)
        {
        sleep_time_us = one_shot_max_us_support;
        }

    /* real sleep time should be reduced by latency time. */

    sleep_time_us -= cStateLatency[cpuIdx][cState];

    /* save the real sleep time into pSleepTime and return. */

    * pSleepTime = sleep_time_us;

    }

/*
* tickless_enter() is called when a CPU is going to enter idle state, a one shot
* interrupt is planned at sametime which is used to wake up CPU.
*
* RETURNS: N/A
*/

static void tickless_enter (void)
    {
    uint64_t        sleepTime;
    cpu_cstate_t    cStateToEnter = CPU_CSTATE_C1;
    uint32_t        cpuIdx        = 0;

#ifdef RHINO_CONFIG_SMP
    cpuIdx = cpu_index_get();
#endif

    krhino_spin_lock_irq_save(&ticklessSpin);

    /* Check if tickless can be entered now */

    tickless_enter_check (cpuIdx, cStateConfig[cpuIdx],
                              &sleepTime, &cStateToEnter);

#ifdef RHINO_CONFIG_SMP

    /* 
     * if sleepTime > 0, which means that CPU should go to
     * tick less mode. Under SMP environment, the system clock
     * heartbeat will be closed and start one shot plan only
     * when all CPU decide go to tick less mode, and the last
     * CPU which decide enter tickless mode will take the action
     * to close system heartbeat and start one shot plan.
     */

    /* clear the live bit for current CPU */

    CPUSET_CLR (tickless_live_cpu_set, cpuIdx);

    if ((sleepTime > 0) &&
        CPUSET_IS_ZERO (tickless_live_cpu_set) &&
        (is_current_tickless == FALSE))
        {
     
        /*
         * Enable a one shot timer to wake up the system from an idle power
         * management state. This one shot timer will wakeup the system 
         * unless another asynchronous event has woken up the CPU already.
         */

        if (tickless_one_shot_start (sleepTime, cStateToEnter) == RHINO_SUCCESS)
            {
            is_current_tickless = TRUE;

#ifdef CPU_TICKLESS_DBG
            tickless_enter_count[cStateToEnter]++;
#endif
            }
#ifdef CPU_TICKLESS_DBG   
        else
            {
            printf("tickless_one_shot_start (%lld, C%d) failed\n",
                   sleepTime, cStateToEnter);
            }
#endif      
        }

#else /* RHINO_CONFIG_SMP */

    if ((sleepTime > 0) &&
        (is_current_tickless == FALSE))
        {
        /*
         * Enable a one shot timer to wake up the system from an idle power
         * management state. This one shot timer will wakeup the system 
         * unless another asynchronous event has woken up the CPU already.
         */

        if (tickless_one_shot_start (sleepTime, cStateToEnter) == RHINO_SUCCESS)
            {
            is_current_tickless = TRUE;
#ifdef CPU_TICKLESS_DBG
            tickless_enter_count[cStateToEnter]++;
#endif
            }
#ifdef CPU_TICKLESS_DBG   
        else
            {
            printf("tickless_one_shot_start (%lld, C%d) failed\n",
                   sleepTime, cStateToEnter);
            }
#endif
        }

#endif /* RHINO_CONFIG_SMP */

    c_state_entered= cStateToEnter;


    if (is_current_tickless == TRUE)
        {
        /* suspend system tick interrupt first */

        systick_suspend();

        /* 
         * take CPU into relative C idle state which is decided by
         * tickless_enter_check().
         *
         * NOTE, the CPU interrupt is still locked here, so CPU will be put
         * into Cx state with cpu interrupt locked, but it is safe to wake
         * up CPU throught interrupt.
         *
         * For ARM cortex-M, the processor ignores the value of PRIMASK in
         * determining whether an asynchronous exception is a WFI wakeup event.
         *
         * For ARM cortex-A, when processor is in WFI state, it will be waked
         * up by:
         * a physical IRQ interrupt regardless of the value of the CPSR.I bit,
         * a physical FIQ interrupt regardless of the value of the CPSR.F bit.
         *
         * For Xtensa, WAITI sets the interrupt level in PS.INTLEVEL to imm4
         * and then, on some Xtensa ISA implementations, suspends processor
         * operation until an interrupt occurs. The combination of setting
         * the interrupt level and suspending operation avoids a race condition
         * where an interrupt between the interrupt level setting and the
         * suspension of operation would be ignored until a second interrupt
         * occurred.
         */

        (void) cpu_pwr_c_state_set(cStateToEnter);
        }

    krhino_spin_unlock_irq_restore (&ticklessSpin);
    }

/*
* tickless_exit() is called when a CPU gets interrupted. If it determeines 
* that the system is waking up from tickless idle, it re-enables tick 
* interrupts and stop the one shot interrupt plan, also announces elapsed
* ticks to the kernel.
*
* RETURNS: N/A
*/

static void tickless_exit (void)
    {
    tick_t   nTicks = 0;
#ifdef RHINO_CONFIG_SMP
    int           cpuIdx = cpu_index_get();
    int          firstWakeUp = FALSE;
#endif

    krhino_spin_lock_irq_save (&ticklessSpin);

#ifdef RHINO_CONFIG_SMP

    if (CPUSET_IS_ZERO (tickless_live_cpu_set))
        {
        firstWakeUp = TRUE;
        }
    
    CPUSET_SET (tickless_live_cpu_set, cpuIdx);

    if ((firstWakeUp == TRUE) && (is_current_tickless == TRUE))
        {

        /* first CPU to wake up gets to disable one-shot mode */

        nTicks = tickless_one_shot_stop();

        is_current_tickless = FALSE;
        }
    else
        {
        nTicks = 0;
        }
#else

    if (!is_current_tickless)
        {
        krhino_spin_unlock_irq_restore (&ticklessSpin);
        return;
        }

    nTicks = tickless_one_shot_stop(c_state_entered);
    g_tick_sleep += nTicks;
    
    /* set is_current_tickless to FALSE */

    is_current_tickless = FALSE;

#endif

    krhino_spin_unlock_irq_restore (&ticklessSpin);

    if (nTicks > 0)
        {

        /* announces elapsed ticks to the kernel */

        tickless_announce_n (nTicks);
        }

    /* resume system tick interrupt */

    systick_resume();
    }

/*
* tickless_sleep_ticks_get() checks the kernel tick list for the time that
* the next task or event is scheduled to run while ignoring tick nodes
* that are on the ignore list.  It is used by power management to enable
* tickless-idle for the proper duration.
*
* RETURNS: The number of ticks until the next scheduled task or RHINO_WAIT_FOREVER
*          if the tick list is empty.
*/

static tick_t tickless_sleep_ticks_get (void)
    {

    klist_t * tick_head_ptr = &g_tick_head;
    ktask_t * p_tcb = NULL;
    klist_t * iter;
    tick_t    ticks = 0xFFFFFFFF;
    TICKLESS_OBJ * pIgnore = NULL;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    if (tick_head_ptr->next == &g_tick_head)
        {
        RHINO_CRITICAL_EXIT();
        return RHINO_WAIT_FOREVER;
        }

    /* the first element in g_tick_head list must has the minimum wait time */
 
    iter  = tick_head_ptr->next;
    while(iter != &g_tick_head)
        {
        pIgnore = (TICKLESS_OBJ *)DLL_FIRST(&g_ignore_list);

        /*
         * Do a linear search to find whether node should be ignored.
         * It is assumed that ignore list is very short.
         */

        while (pIgnore != NULL)
            {
            if (iter == pIgnore->pTickNode)
                {
                /* 
                 * hit! ignore this node on g_tick_head and inspect next
                 * node on g_tick_head list.
                 */

                break;
                }

            pIgnore = (TICKLESS_OBJ *)DLL_NEXT (pIgnore);
            }

       if (pIgnore == NULL)
            {
            /* 
             * Not found on ignore list, use the tick information of current 
             * node as return value.
             */

            p_tcb    = krhino_list_entry(iter, ktask_t, tick_list);
            ticks = p_tcb->tick_match - g_tick_count;
            break;
            }

        iter = iter->next;
        }
    
    RHINO_CRITICAL_EXIT();

#ifdef CPU_TICKLESS_DBG
    printf("tickless_sleep_ticks_get() return %lld(ticks), task name -> %s\n",
            ticks, p_tcb==NULL?"NULL":(char*)p_tcb->task_name); 

    tickless_info_show();
#endif

    return ticks;
    }

/* tickless_announce_n() is called to announces elapsed ticks to the kernel.*/

static void tickless_announce_n 
    (
    tick_t nTicks
    )
    {
    tick_list_update((tick_i_t)nTicks);
    }

/* 
* tickless_one_shot_timer_save() is used to build connection between
* one shot timer and c state, different c state could has different
* wake up timer.
*/

void tickless_one_shot_timer_save
    (
    cpu_cstate_t     cstate,
    one_shot_timer_t * ptimer
    )
    {
    cStateOneShotTimer[cstate] = ptimer;
    }

static kstat_t tickless_ignore_list_add
    (
    klist_t * pTickNode
    )
    {
    TICKLESS_OBJ * pNode;
    CPSR_ALLOC();

    pNode = (TICKLESS_OBJ *) malloc(sizeof(TICKLESS_OBJ));

    if (pNode == NULL)
        {
        return RHINO_PWR_MGMT_ERR;
        }

    RHINO_CRITICAL_ENTER();

    pNode->pTickNode = pTickNode;
    
    DLL_ADD(&g_ignore_list, pNode);
    
    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
    }


/*
* API: tickless_task_ignore() makes a task be ignored when
* tickless_sleep_ticks_get() is computing.
*/

kstat_t tickless_task_ignore
    (
    ktask_t * p_tcb
    )
    {
    if (p_tcb == NULL)
        {
        p_tcb = krhino_cur_task_get();
        }

    if (_func_tickless_ignore_list_add != NULL)
        return _func_tickless_ignore_list_add(&p_tcb->tick_list);
    else
        return RHINO_SUCCESS;
    }

#ifdef CPU_TICKLESS_DBG

/*
* tickless_info_show() prints the current CPU tickless statistics information
* such as howmany times that CPU enter C1 tickless(if support), C2 tickless(if
* support), C3 tickless(if support), and so on.
*/

static void tickless_info_show (void)
    {
    int             cpuIdx;
    cpu_cstate_t    cState;

    for (cpuIdx = 0; cpuIdx < CPUS_NUM_MAX; cpuIdx++)
        {

        for (cState = CPU_CSTATE_C1; cState <= CPU_CSTATE_MAX; cState++)
            {
            if ((tickless_ctate_mask & (1 << cState)))
                {
                printf ("cpu %d - number of times to enter C%d tickless: %d\n",
                    cpuIdx, cState, tickless_enter_count[cState]);
                }
            }
        }
    }

/*
* tickless_ignore_list_show() prints the information of g_ignore_list
* list, to show which task existed in g_ignore_list list. 
*/

static void tickless_ignore_list_show (void)
    {
    DL_NODE     * pNode;
    ktask_t     * pTcb;
    TICKLESS_OBJ * pIgnore;    

    printf ("g_ignore_list -> ");

    pNode = DLL_FIRST(&g_ignore_list);

    while (pNode != NULL)
        {
        pIgnore = (TICKLESS_OBJ *)pNode;

        pTcb = container_of(pIgnore->pTickNode, ktask_t, tick_list);

        printf ("task(%s) -> ", pTcb->task_name);

        pNode = DLL_NEXT (pNode);
        }

    printf ("NULL\n");
    }

/*
* tickless_gticklist_show() prints the information of g_tick_head list, 
* to show which task existed in g_tick_head list and it's delay ticks.
*/

static void tickless_gticklist_show (void)
    {
    ktask_t     * pTcb;
    tick_t        nTicks;
    klist_t     * iter;
    klist_t     * tick_head_ptr = &g_tick_head;

    printf ("g_tick_head -> ");

    iter  = tick_head_ptr->next;
    while(iter != &g_tick_head)
        {
        pTcb = container_of(iter, ktask_t, tick_list);

        nTicks = pTcb->tick_match - g_tick_count;

        printf ("task(%s)+(%lld)nTicks -> ", pTcb->task_name, nTicks);        

        iter = iter->next;
        }

    printf ("NULL\n");  
    }

#endif /* CPU_TICKLESS_DBG */

#endif /* RHINO_CONFIG_CPU_TICKLESS */
#endif /* RHINO_CONFIG_CPU_PWR_MGMT */

