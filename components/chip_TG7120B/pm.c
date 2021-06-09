
#include <stdint.h>
#include <string.h>
#include <soc.h>
#include <yoc/lpm.h>

#include <drv/pmu.h>
#include <drv/wdt.h>
#include <drv/gpio.h>

#include <ll_sleep.h>
#include <ll_debug.h>
#include <ll_hw_drv.h>

#include <global_config.h>
//#include <OSAL_PwrMgr.h>
//#include <OSAL_Timers.h>

#include <k_api.h>
#include <gpio.h>
#include <pinmux.h>
#include "pwrmgr.h"
#include "pin.h"
#include "pm.h"

#define DBGIO_LL_TRIG       P14
#define DBGIO_LL_IRQ        P15
#define DBGIO_APP_WAKEUP    P20
#define DBGIO_APP_SLEEP     P11
#define DBGIO_DIS_IRQ       P11
#define DBGIO_EN_IRQ        P11
#define DBG_GPIO_WRITE(a,b)

#define MIN_TIME_TO_SLEEP       50      //ms
#define MIN_TIME_TO_SUSPEND     10000   //ms
extern int g_spif_ref_clk;
//pwrmgr_attribute_t pwrmgr_attribute;
extern uint32_t ll_remain_time;
extern uint32_t  g_wakeup_rtc_tick;// = 0;
extern uint32_t sleep_flag;// = 0;                   // when sleep, set this value to SLEEP_MAGIC. when wakeup, set this value to 0
extern uint32_t osal_sys_tick;

extern uint32_t  g_counter_traking_avg ;//      = 3906;
extern volatile uint32_t llWaitingIrq;

//used for sleep timer sync
extern uint32_t  g_TIM2_IRQ_TIM3_CurrCount;//  =   0;
extern uint32_t  g_TIM2_IRQ_to_Sleep_DeltTick;//=0;
extern uint32_t  g_TIM2_IRQ_PendingTick;//=0;
extern uint32_t  g_osal_tick_trim;//=0;
extern uint32_t  g_osalTickTrim_mod;//=0;
extern uint32_t  g_TIM2_wakeup_delay;//=0;
extern uint32_t  rtc_mod_value;// = 0;
extern uint32_t  g_counter_traking_cnt;// = 0;
extern pwrmgr_attribute_t pwrmgr_attribute;
extern uint8 isTimer4Running(void);
extern uint32_t counter_tracking;                // 24bit tracking counter, read from 0x4000f064
extern void hal_wakeup_irq_config(void);
extern void wakeup_init(void);
extern void set_timer(AP_TIM_TypeDef *TIMx, int time);
extern void ll_hw_tx2rx_timing_config(uint8_t pkt);
extern void WaitRTCCount(uint32_t rtcDelyCnt);
extern void hal_cache_init(void);
extern uint32  read_LL_remainder_time(void);
extern uint32_t read_current_fine_time(void);
extern int clk_spif_ref_clk(sysclk_t spif_ref_sel);
extern void hal_mpu_config(void);

static uint32_t irq_flags;

extern void osalTimeUpdate(void);
extern void csi_usart_prepare_sleep_action();
extern void csi_usart_wakeup_sleep_action();
extern void csi_pinmux_prepare_sleep_action();
extern void csi_pinmux_wakeup_sleep_action();
extern void csi_gpio_prepare_sleep_action();
extern void csi_gpio_wakeup_sleep_action();

extern uint32_t sleep_tick;

enum power_states {
    SYS_POWER_STATE_RUN,       /* C1 state */
    SYS_POWER_STATE_WAIT,       /* C1 state */
    SYS_POWER_STATE_DOZE,     /* C2 state */
    SYS_POWER_STATE_STOP,     /* C2LP state */
    SYS_POWER_STATE_STANDBY,    /* SLEEP state */

    SYS_POWER_STATE_MAX
};

static pmu_handle_t pmu_handle = NULL;
static Sleep_Mode sleepMode = SYSTEM_SLEEP_MODE;// MCU_SLEEP_MODE;
static uint8_t bSleepDisable = 0;
pm_wakeup_by_io_cb g_wakeup_cb = NULL;


#define LPM_FLASH_NAME   "lpm"
#define BOOT_FLASH_NAME  "boot"
#define PMU_ADDROFF      (0x1F0)

#ifndef CONFIG_WDT
#define CONFIG_WDT     0
#endif

#if (CONFIG_WDT > 0)
#define WDT_TIMEOUT             2000
static wdt_handle_t wdt_handle = NULL;
void wdt_cb_handle(int32_t idx, wdt_event_e event)
{
    if (wdt_handle == NULL) {
        return;
    }

    csi_wdt_restart(wdt_handle);
}
#endif

int32_t boot_wdt_close(void)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;
    extern int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
    int32_t real_idx = target_get_wdt(0, &base, &irq, &handler);

    if (real_idx != 0) {
        return 0;
    }

    extern void drv_irq_disable(uint32_t irq_num);
    drv_irq_disable(irq);
    extern void drv_irq_unregister(uint32_t irq_num);
    drv_irq_unregister(irq);

    return 0;
}

void wdt_init(void)
{
#if (CONFIG_WDT > 0)
    wdt_handle = csi_wdt_initialize(0, NULL);//wdt_cb_handle);

    if (wdt_handle == NULL) {
        printf("WDT init Err!\n");
        return;
    }

    if (csi_wdt_set_timeout(wdt_handle, WDT_TIMEOUT) < 0) {
        printf("WDT set timeout Err!\n");
        return;
    }

    csi_wdt_start(wdt_handle);
#else
    boot_wdt_close();
#endif
}

void feed_wdt(void)
{
#if (CONFIG_WDT > 0)
    if (wdt_handle == NULL) {
        printf("WDT init Err!\n");
        return;
    }
    csi_wdt_restart(wdt_handle);
#endif
}

void wdt_wakeup_action()
{
#if (CONFIG_WDT > 0)
    if (wdt_handle == NULL) {
        return;
    }

    uint32_t delay = 0;

    if ((AP_PCR->SW_RESET0 & 0x04) == 0) {
        AP_PCR->SW_RESET0 |= 0x04;
        delay = 20;

        while (delay-- > 0);
    }

    if ((AP_PCR->SW_RESET2 & 0x04) == 0) {
        AP_PCR->SW_RESET2 |= 0x04;
        delay = 20;

        while (delay-- > 0);
    }

    AP_PCR->SW_RESET2 &= ~0x20;
    delay = 20;

    while (delay-- > 0);

    AP_PCR->SW_RESET2 |= 0x20;
    delay = 20;

    while (delay-- > 0);

    AP_WDT->CR = 0x1F;

    csi_wdt_set_timeout(wdt_handle, WDT_TIMEOUT);
#endif
}

__attribute__((weak)) __attribute__((section(".__sram.code"))) int pm_prepare_sleep_action()
{
    csi_pinmux_prepare_sleep_action();
    csi_usart_prepare_sleep_action();
    csi_gpio_prepare_sleep_action();
    return 0;
}

__attribute__((weak)) __attribute__((section(".__sram.code"))) int pm_after_sleep_action()
{
    csi_pinmux_wakeup_sleep_action();
    csi_usart_wakeup_sleep_action();
    csi_gpio_wakeup_sleep_action();
    if (g_wakeup_cb) {
        g_wakeup_cb(NULL);
    }
    return 0;
}

__attribute__((section(".__sram.code"))) void registers_save(uint32_t *mem, uint32_t *addr, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        mem[i] = *((volatile uint32_t *)addr + i);
    }
}

__attribute__((section(".__sram.code"))) void registers_restore(uint32_t *addr, uint32_t *mem, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        *((volatile uint32_t *)addr + i) = mem[i];
    }

}
#if 0
static uint32_t usart_regs_saved[5];
__attribute__((section(".__sram.code"))) static void usart_prepare_sleep_action(void)
{
    uint32_t addr = 0x40004000;
    //uint32_t read_num = 0;

    // while (*(volatile uint32_t *)(addr + 0x14) & 0x1) {
    //     *(volatile uint32_t *)addr;
//
    //      if (read_num++ >= 16) {
    //        break;
    //   }
    //}

//    while (*(volatile uint32_t *)(addr + 0x7c) & 0x1);

    *(volatile uint32_t *)(addr + 0xc) |= 0x80;
    registers_save((uint32_t *)usart_regs_saved, (uint32_t *)addr, 2);
    *(volatile uint32_t *)(addr + 0xc) &= ~0x80;
    registers_save(&usart_regs_saved[2], (uint32_t *)addr + 1, 1);
    registers_save(&usart_regs_saved[3], (uint32_t *)addr + 3, 2);
}

__attribute__((section(".__sram.code"))) static void usart_wakeup_action(void)
{
    uint32_t addr = 0x40004000;
    drv_pinmux_config(P9, FMUX_UART0_TX);
    drv_pinmux_config(P10, FMUX_UART0_RX);

    //while (*(volatile uint32_t *)(addr + 0x7c) & 0x1);

    *(volatile uint32_t *)(addr + 0xc) |= 0x80;
    registers_save((uint32_t *)addr, usart_regs_saved, 2);
    *(volatile uint32_t *)(addr + 0xc) &= ~0x80;
    registers_save((uint32_t *)addr + 1, &usart_regs_saved[2], 1);
    registers_save((uint32_t *)addr + 3, &usart_regs_saved[3], 2);
}
#endif
#define   CRY32_2_CYCLE_16MHZ_CYCLE_MAX    (976 + 196)     // tracking value range std +/- 20%
#define   CRY32_2_CYCLE_16MHZ_CYCLE_MIN    (976 - 196)
#define   CRY32_2_CYCLE_DELTA_LMT          (97)

uint32_t g_xtal16M_waitCnt=0;
uint32_t g_xtal16M_tmp=0;
volatile uint8_t g_rc32kCalRes;

extern uint8 rc32k_calibration(void);

void rf_calibrate2(void)
{
    //========== do rf tp cal for tx and rx dcoffset cal
    rf_phy_ana_cfg();
    rf_tpCal_gen_cap_arrary();                                  //generate the tpCal cap arrary
    //rf_tpCal_cfg(/*rfChn*/2);
    rf_rxDcoc_cfg(/*rfChn*/88,/*bwSet*/1,&g_rfPhyRxDcIQ);       //set the rfChn as 2488 for BW=1MHz
    g_rc32kCalRes = rc32k_calibration();
}

static void check_16MXtal_by_rcTracking(void)
{
    /*
        for fiset wakeupini, not do rcCal, just skip the rcTacking
    */
    if(g_rc32kCalRes==0xFF)
    {
        WaitRTCCount(30);
        return;
    }

    uint32_t temp,temp1,waitCnt;
    // ======== enable tracking 32KHz RC timer with 16MHz crystal clock
    temp = *(volatile uint32_t *)0x4000f040;
    *(volatile uint32_t *)0x4000f040 = temp | BIT(18);

    waitCnt=0;

    temp = *(volatile uint32_t *)0x4000f05C;
    *(volatile uint32_t *)0x4000f05C = (temp & 0xfffefe00) | 0x0028;
    WaitRTCCount(2);
    temp = (*(volatile uint32_t *)0x4000f064 & 0x1ffff);

    while(1) //
    {
        // 1. read RC 32KHz tracking counter, calculate 16MHz ticks number per RC32KHz cycle
        waitCnt++;
        WaitRTCCount(2);
        temp1 = (*(volatile uint32_t *)0x4000f064 & 0x1ffff);

        if( temp1>CRY32_2_CYCLE_16MHZ_CYCLE_MIN  &&
            temp1<CRY32_2_CYCLE_16MHZ_CYCLE_MAX  &&
            temp >CRY32_2_CYCLE_16MHZ_CYCLE_MIN  &&
            temp <CRY32_2_CYCLE_16MHZ_CYCLE_MAX  &&
            ((temp >temp1 ? (temp -temp1):(temp1>temp)))<CRY32_2_CYCLE_DELTA_LMT
            )
        {
            break;
        }
        temp=temp1;
    }

    g_xtal16M_tmp = temp1;
    g_xtal16M_waitCnt = (g_xtal16M_waitCnt>waitCnt) ? g_xtal16M_waitCnt: waitCnt;
}

uint8_t isSleepAllowInPM(void)
{
    return (bSleepDisable == 0) ? TRUE : FALSE;
}

// enable sleep
void enableSleepInPM(uint8_t flag)
{
    bSleepDisable &= ~ flag;
}

// disable sleep
void disableSleepInPM(uint8_t flag)
{
    bSleepDisable |= flag;
}

// set sleep mode
void setSleepMode(Sleep_Mode mode)
{
    sleepMode = mode;
}

// get sleep mode configuration
Sleep_Mode getSleepMode(void)
{
    return sleepMode;
}

void set_sleep_flag(int flag)
{
    if (flag) {
        *(volatile uint32_t *) 0x4000f0a8 |= 1 ;
        sleep_flag = SLEEP_MAGIC ;
    } else {
        *(volatile uint32_t *) 0x4000f0a8 &= ~1;
        sleep_flag = 0 ;
    }
}

//    *((volatile uint32_t *)(0xe000e100)) |= INT_BIT_RTC;   // remove, we don't use RTC interrupt

//ZQ: cannot comment, need update the sleep_tick
// comparator configuration

//align to rtc clock edge

//  *(volatile uint32_t *) 0x4000f024 |= 1 << 20;           //enable comparator0 envent
//  *(volatile uint32_t *) 0x4000f024 |= 1 << 18;           //counter overflow interrupt
//  *(volatile uint32_t *) 0x4000f024 |= 1 << 15;           //enable comparator0 inerrupt

//*(volatile uint32_t *) 0x4000f024 |= 0x148000;          // combine above 3 statement to save MCU time

void enterSleepProcess1(uint32_t time)
{

    uint32 delta, total, step, temp;

    // if allow RC 32KHz tracking, adjust the time according to the bias
    if (pGlobal_config[LL_SWITCH] & RC32_TRACKINK_ALLOW) {
        // 1. read RC 32KHz tracking counter, calculate 16MHz ticks number per RC32KHz cycle

        temp = *(volatile uint32_t *)0x4000f064 & 0x1ffff;
        //====== assume the error cnt is (n+1/2) cycle,for this case, it should be 9 or 10

//         error_delt = (temp>STD_CRY32_8_CYCLE_16MHZ_CYCLE)
//                     ?   (temp- STD_CRY32_8_CYCLE_16MHZ_CYCLE) : (STD_CRY32_8_CYCLE_16MHZ_CYCLE-temp);


//         if(error_delt<ERR_THD_RC32_CYCLE)
//            temp = temp;
//         else if(error_delt<((STD_CRY32_8_CYCLE_16MHZ_CYCLE>>3)+ERR_THD_RC32_CYCLE))
        //temp = ((temp<<3)*455+2048)>>12;//*455/4096~=1/9
//             temp = temp<<3;
//             temp = ((temp<<9)-(temp<<6)+(temp<<3)-temp+2048)>>12;
//         else
        //temp = ((temp<<3)*410+2048)>>12;//*410/4096~=1/10
//             temp = temp<<3;
//             temp = ((temp<<9)-(temp<<6)-(temp<<5)-(temp<<3)+(temp<<1)+2048)>>12;
//         }

        //check for the abnormal temp value
        counter_tracking = (temp > CRY32_16_CYCLE_16MHZ_CYCLE_MAX) ? counter_tracking : temp;
        //20181204 filter the counter_tracking spur, due to the N+1 issue


        if (g_counter_traking_cnt < 1000) {
            //before traking converage use hard limitation
            counter_tracking = (counter_tracking > CRY32_16_CYCLE_16MHZ_CYCLE_MAX || counter_tracking < CRY32_16_CYCLE_16MHZ_CYCLE_MIN)
                               ? g_counter_traking_avg : counter_tracking;

            g_counter_traking_cnt++;
        } else {

            //after tracking converage use soft limitation
            counter_tracking = (counter_tracking > g_counter_traking_avg + (g_counter_traking_avg >> 8)
                                ||  counter_tracking < g_counter_traking_avg - (g_counter_traking_avg >> 8))
                               ? g_counter_traking_avg : counter_tracking;

        }



        //one order filer to tracking the average counter_tracking
        g_counter_traking_avg = (7 * g_counter_traking_avg + counter_tracking) >> 3 ;

        // 2.  adjust the time according to the bias

        step = (counter_tracking) >> 3;           // accurate step = 500 for 32768Hz timer


        if (counter_tracking > STD_CRY32_16_CYCLE_16MHZ_CYCLE) {         // RTC is slower, should sleep less RTC tick
            delta = counter_tracking - STD_CRY32_16_CYCLE_16MHZ_CYCLE;   // delta 16MHz tick in 8 32KHz ticks

            total = (time * delta) >> 3;                               // total timer bias in 16MHz tick

            while (total > step) {
                total -= step;
                time --;
            }
        } else { // RTC is faster, should sleep more RTC tick
            delta = STD_CRY32_16_CYCLE_16MHZ_CYCLE - counter_tracking;   // delta 16MHz tick in 8 32KHz ticks
            total = (time * delta) >> 3;                               // total timer bias in 16MHz tick

            while (total > step) {
                total -= step;
                time ++;
            }
        }
    }

    // backup registers         ------   none now

    // backup timers            ------   none now

    //===20180417 added by ZQ
    //   for P16,P17
    subWriteReg(&(AP_AON->PMCTL2_0), 6, 6, 0x00); //disable software control
    //printf("config rtc %d\n", time);
    // 3. config wakeup timer
    config_RTC(time);

    //usart_prepare_sleep_action();
    pm_prepare_sleep_action();
    // 4. app could add operation before sleep
    //app_sleep_process();
    //====== set sram retention
    //    hal_pwrmgr_RAM_retention_set();           // IMPORTANT: application should set retention in app_sleep_process

    ll_debug_output(DEBUG_ENTER_SYSTEM_SLEEP);

    // 5. set sleep flag(AON reg & retention SRAM variable)
    set_sleep_flag(1);


    // 6. trigger system sleep
    csi_pmu_enter_sleep(pmu_handle, PMU_MODE_DORMANT);
}

void osal_pwrmgr_powerconserve_dummy(void)
{
    //do nothing
    return;
}

/******************************************
// SOC interfaces
******************************************/
int sys_soc_init()
{
    pmu_handle = csi_pmu_initialize(0, NULL);

    JUMP_FUNCTION(OSAL_POWER_CONSERVE) = (uint32_t)&osal_pwrmgr_powerconserve_dummy;

    return 0;
}


int sys_soc_suspend(uint32_t suspend_tick)
{
    uint32_t        next;
    uint32_t        temp1, temp2;
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    irq_flags = psr;

    if (!isSleepAllowInPM()) {
        RHINO_CPU_INTRPT_ENABLE();
        __WFI();
        return SYS_POWER_STATE_RUN;
    }

    if (llWaitingIrq) {     // bug correct 2017-7-5, osal sleep function may be interrupted by LL irq and LL timer could be changed
        // Don't triggered System sleep in such case
        RHINO_CPU_INTRPT_ENABLE();
        __WFI();
        return SYS_POWER_STATE_RUN;
    }

    //===============================================================================================================
    // ZQ: add 20180712. For 32K RC Tracking
    // Ensure wakup enough time before enter sleep
    // RTC counter_tracking is fatched at enter_sleep_process,
    // Need to reserve enough time for 16M Xtal settling, then fatch the RTC counter_traking.
    // When wakeup time is not enough, turn on the IRQ and _WFI,waiting for the timer_irq or peripheral_irq
    //
    if (pGlobal_config[LL_SWITCH] & RC32_TRACKINK_ALLOW) {
        uint32_t cnt1 = rtc_get_counter();

        uint32_t delt = (cnt1 >= g_wakeup_rtc_tick) ? cnt1 - g_wakeup_rtc_tick : (0xffffffff - g_wakeup_rtc_tick + cnt1);

        if (delt < (pGlobal_config[ALLOW_TO_SLEEP_TICK_RC32K])) {
            RHINO_CPU_INTRPT_ENABLE();
            __WFI();
            return SYS_POWER_STATE_RUN;
        }
    }

    //===============================================================================================================
    // Hold off interrupts.
    //        HAL_ENTER_CRITICAL_SECTION( intState );

    // the comparator of RTC not consider loop case, it will interrupt immediately if comparator value smaller then threshold
    // walkaround: sleep time will be decreased to avoid RTC counter overflow when sleep

#if 0
    uint32_t        rtc_counter;
    //rtc_counter = *(volatile uint32_t *)0x4000f028 & 0x00ffffff;     // read current RTC counter
    rtc_counter = rtc_get_counter();
    rtc_counter = 0xffffffff - rtc_counter;

    if (pGlobal_config[LL_SWITCH] & RC32_TRACKINK_ALLOW) {
        //when next>rtc_counter, use rc32k_rtc_counter 20180324
        //becase this time will be convert to rc32k in enterSleepProcess once more
        //            rc32k_rtc_counter = (((rtc_counter << 7) - (rtc_counter << 2) - (rtc_counter << 1)) >>2 )   /* rtc_counter * (128-4-2)/4 */
        //                    +(((rtc_counter << 3)+ rtc_counter ) >> 9 ) ; /* rtc_counter *9/512 */
        //
        //            //check for the abnormal counter_tracking value
        counter_tracking = (counter_tracking > CRY32_8_CYCLE_16MHZ_CYCLE_MAX || counter_tracking < CRY32_8_CYCLE_16MHZ_CYCLE_MIN)
                           ? g_counter_traking_avg : counter_tracking;

        rtc_counter = ((((rtc_counter & 0xffff0000) >> 16) * counter_tracking) << 9)
                      + (((rtc_counter & 0xffff) * counter_tracking) >> 7);

        //            //rtc_counter = (rtc_counter << 5) - (rtc_counter << 1) + (rtc_counter >> 1);     // convert RTC tick to us, multiply 30.5
        //            rtc_counter = (((rtc_counter << 7) - (rtc_counter << 2) - (rtc_counter << 1)) >>2 )   /* rtc_counter * (128-4-2)/4 */
        //                    +(((rtc_counter << 3)+ rtc_counter ) >> 9 ) ; /* rtc_counter *9/512 */
    } else {

        //rtc_counter = (rtc_counter << 5) - (rtc_counter << 1) + (rtc_counter >> 1);     // convert RTC tick to us, multiply 30.5
        rtc_counter = (((rtc_counter << 7) - (rtc_counter << 2) - (rtc_counter << 1)) >> 2)   /* rtc_counter * (128-4-2)/4 */
                      + (((rtc_counter << 3) + rtc_counter) >> 9) ; /* rtc_counter *9/512 */
    }

    // Get next time-out
    next = osal_next_timeout();         // convert from ms to us
    next = (next << 10) - (next >> 4) - (next >> 3); ////next * 1000

    // printf("next %d rtc_counter %d\n", next, rtc_counter);

    if (next == 0) {                   // no OSAL timer running, only consider rtc_counter & ll timer
        next = rtc_counter;
    } else {
        next = (next > rtc_counter) ? rtc_counter : next;
#endif
        // Get next time-out
        next = osal_next_timeout();         // convert from ms to us
        next = (next << 10) - (next >> 4) - (next >> 3); ////next * 1000
        // if LL timer is not kick off, set remain time as max sleep time
        ll_remain_time = read_LL_remainder_time();
        // printf("read_LL_remainder_time %d\n", ll_remain_time);

        // ZQ: add 20180514. When timer1 overrun happend, turn on the IRQ and return.
        // TIM1_IRQ_Handle will process the TIM1_IRQ evernt

        if ((AP_TIM1->status & 0x1) == 1) {
            RHINO_CPU_INTRPT_ENABLE();
            return SYS_POWER_STATE_RUN;
        }

        if (llState == LL_STATE_IDLE) { // (ll_remain_time == 0 || llState == LL_STATE_IDLE)
            ll_remain_time = pGlobal_config[MAX_SLEEP_TIME];
            //  printf("ll_remain_time %d\n", ll_remain_time);
        }

        // remove below decision 2018-04-04. In LL_STATE_IDLE, below statement will fail the sleep process
        // correct 17-09-11, timer IRQ during sleep process will cause read fault ll_remain_time value
        // walkaround here, it is better to modify timer driver to clear all register in ISR
        //        if (AP_TIM1->CurrentCount > AP_TIM1->LoadCount)// && (AP_TIM1->ControlReg & 0x1) != 0)
        //        {
        //            HAL_EXIT_CRITICAL_SECTION();
        //            return;
        //        }

        // ===============  BBB ROM code add, timer4 save/recover process
        if (isTimer4Running()) {
            g_llSleepContext.isTimer4RecoverRequired = TRUE;
            g_llSleepContext.timer4Remainder         = read_ll_adv_remainder_time();
        } else {
            g_llSleepContext.isTimer4RecoverRequired = FALSE;
        }

        //printf("n %d l %d\n", next, ll_remain_time);

        next = (next == 0) ? ll_remain_time : ((next > ll_remain_time) ? ll_remain_time : next);

        if (g_llSleepContext.isTimer4RecoverRequired) {
            next = (next > g_llSleepContext.timer4Remainder) ? g_llSleepContext.timer4Remainder : next;
        }

        uint32_t kernel_us = (suspend_tick << 10) - (suspend_tick >> 4) - (suspend_tick >> 3);//suspend_tick * 1000
        //printf("suspend tick is %d %d\n", kernel_us, next);
        next = (next > kernel_us) ? kernel_us : next;

        if (getSleepMode() == MCU_SLEEP_MODE
            || llWaitingIrq       // system sleep will not trigger when ll HW processing
            || next < (pGlobal_config[MIN_SLEEP_TIME] + pGlobal_config[WAKEUP_ADVANCE])   // update 2018-09-06
            || pwrmgr_attribute.pwrmgr_task_state != 0) {  // Are all tasks in agreement to conserve
            // MCU sleep mode
            //LOG("[E]%d %d %d %d\n",getSleepMode(),llWaitingIrq,next,pwrmgr_attribute.pwrmgr_task_state);
            RHINO_CPU_INTRPT_ENABLE();
            __WFI();

            return SYS_POWER_STATE_RUN;

        } else {
            next = next - pGlobal_config[WAKEUP_ADVANCE];    // wakeup advance: consider HW delay, SW re-init time, ..., etc.

            if (next > pGlobal_config[MAX_SLEEP_TIME]) { // consider slave latency, we may sleep up to 16s between 2 connection events
                next = pGlobal_config[MAX_SLEEP_TIME];
            }

            // covert time to RTC ticks
            // RTC timer clock is 32768HZ, about 30.5us per tick
            // time / 30.5 = time * ( 1/32 + 1 / 512 - 1 / 2048 + 1/16384 - 1/65536 + 1/128K - 1/512K...)
            //             = time * ( 1/32 + 1 / 512 - 1 / 2048 + 1/16384 - 1/128K - 1/512K...) = 32766.3 * 10e-6
            temp1 = next >> 9;
            temp2 = next >> 11;
            next = next >> 5;
            next = next + temp1 - temp2 + (temp2 >> 3) - (temp2 >> 6) - (temp2 >> 8);
            //next = next + temp1 - temp2 + (temp2 >> 4);
            enterSleepProcess1(next);
            return SYS_POWER_STATE_STOP;
        }

        // Re-enable interrupts.
        RHINO_CPU_INTRPT_ENABLE();
        return SYS_POWER_STATE_RUN;
    }

#ifndef BASE_TIME_UINTS
#define BASE_TIME_UNITS   (0x3fffff)
#endif

void wakeup_init1() {
    uint8_t pktFmt = 1;    // packet format 1: BLE 1M
    uint32_t  temp;
    //int int_state;

    *(volatile uint32_t *)0x40000008 = 0x001961f1;  //
    *(volatile uint32_t *)0x40000014 = 0x01e00278;  //

    //each rtc count is about 30.5us
    //after 15count , xtal will be feedout to dll and doubler
    //WaitRTCCount(pGlobal_config[WAKEUP_DELAY]);
    check_16MXtal_by_rcTracking();

    // ============ config BB Top
    *(volatile uint32_t *) 0x40030000 = 0x3d068001; // set tx pkt =2
    *(volatile uint32_t *) 0x400300bc = 0x834;      //[7:0] pll_tm [11:8] rxafe settle
    *(volatile uint32_t *) 0x400300a4 = 0x140;      //[6] for tpm_en

    clk_init(g_system_clk);

    hal_wakeup_irq_config();

    // ========== init timers
   // set_timer(AP_TIM2, 625);      // OSAL 625us tick
    set_timer(AP_TIM3, BASE_TIME_UNITS);   // 1s timer

    NVIC_EnableIRQ(BB_IRQn);
    NVIC_EnableIRQ(TIM1_IRQn);
    NVIC_DisableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);

    ll_hw_set_empty_head(0x0001);

    //time related setting
    ll_hw_set_rx_timeout_1st(500);
    ll_hw_set_rx_timeout(88);               //ZQ 20180606, reduce rx timeout for power saving
    ll_hw_set_loop_timeout(30000);

    ll_hw_set_timing(pktFmt);

    ll_hw_ign_rfifo(LL_HW_IGN_SSN | LL_HW_IGN_CRC | LL_HW_IGN_EMP);

    // ======== enable tracking 32KHz RC timer with 16MHz crystal clock
    temp = *(volatile uint32_t *)0x4000f05C;
    *(volatile uint32_t *)0x4000f05C = (temp & 0xfffefe00) | 0x0108; //[16] 16M [8:4] cnt [3] track_en_rc32k
}

static void hw_spif_cache_config(void) {
    NVIC_DisableIRQ(SPIF_IRQn);
    NVIC_SetPriority((IRQn_Type)SPIF_IRQn, IRQ_PRIO_HAL);
    hal_cache_init();
}

int sys_soc_resume(int pm_state) {
    uint32_t current_RTC_tick;
    uint32_t wakeup_time, wakeup_time0, next_time;
    uint32_t sleep_total;
    uint32_t dlt_tick;

    if (pm_state == SYS_POWER_STATE_RUN) {
        return 0;
    }

    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 1);
    // restore HW registers
    wakeup_init1();
    //__ck802_cpu_wakeup_init();
    //restore initial_sp according to the app_initial_sp : 20180706 ZQ
    //__set_MSP(pGlobal_config[INITIAL_STACK_PTR]);

    //drv_irq_init();

    // NVIC_EnableIRQ(UART0_IRQn);
    // NVIC_EnableIRQ(BB_IRQn);
    // NVIC_EnableIRQ(TIM1_IRQn);
    // NVIC_EnableIRQ(TIM2_IRQn);
    // NVIC_EnableIRQ(TIM3_IRQn);
    // NVIC_EnableIRQ(TIM4_IRQn);

    //===20180417 added by ZQ
    //  could be move into wakeup_init
    //  add the patch entry for tx2rx/rx2tx interval config
    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 0);
    //2018-11-10 by ZQ
    //config the tx2rx timing according to the g_rfPhyPktFmt
    ll_hw_tx2rx_timing_config(g_rfPhyPktFmt);

    if (pGlobal_config[LL_SWITCH] & LL_RC32K_SEL) {
        subWriteReg(0x4000f01c, 16, 7, 0x3fb); //software control 32k_clk
        subWriteReg(0x4000f01c, 6, 6 , 0x01); //enable software control

    } else {
        subWriteReg(0x4000f01c, 9, 8, 0x03); //software control 32k_clk
        subWriteReg(0x4000f01c, 6, 6, 0x00); //disable software control
    }

    //20181201 by ZQ
    //restart the TIM2 to align the RTC
    //----------------------------------------------------------
    //stop the 625 timer
  //  AP_TIM2->ControlReg = 0x0;
  //  AP_TIM2->ControlReg = 0x2;
  //  AP_TIM2->LoadCount = 2500;
    //----------------------------------------------------------
    //wait rtc cnt change
    // gpio_write(DBGIO_APP_WAKEUP, 1);
    WaitRTCCount(1);
    // gpio_write(DBGIO_APP_WAKEUP, 0);
    //----------------------------------------------------------
    //restart the 625 timer
   // AP_TIM2->ControlReg = 0x3;
    current_RTC_tick = rtc_get_counter();
    //g_TIM2_wakeup_delay= (AP_TIM2->CurrentCount)+12; //12 is used to align the rtc_tick

    wakeup_time0 = read_current_fine_time();

    g_wakeup_rtc_tick = rtc_get_counter();
    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 1);
    // rf initial entry, will be set in app
    extern void rf_phy_ini();
    rf_phy_ini();

    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 0);

    if (current_RTC_tick > sleep_tick) {
        dlt_tick = current_RTC_tick - sleep_tick;
    } else {
        dlt_tick = current_RTC_tick + (0xffffffff - sleep_tick) ;
    }

    //dlt_tick should not over 24bit
    //otherwise, sleep_total will overflow !!!
    if (dlt_tick > 0x3fffff) {
        dlt_tick &= 0x3fffff;
    }

    if (pGlobal_config[LL_SWITCH] & RC32_TRACKINK_ALLOW) {

        //sleep_total = ((current_RTC_tick - sleep_tick) * counter_tracking) >> 7; // shift 4 for 16MHz -> 1MHz, shift 3 for we count 8 RTC tick

        sleep_total = ((((dlt_tick & 0xffff0000) >> 16) * counter_tracking) << 8)
                      + (((dlt_tick & 0xffff) * counter_tracking) >> 8);
    } else {
        // time = tick * 1000 0000 / f (us). f = 32000Hz for RC, f = 32768Hz for crystal. We also calibrate 32KHz RC to 32768Hz
        //sleep_total =  ((current_RTC_tick - sleep_tick) * TIMER_TO_32K_CRYSTAL) >> 2;

        //fix sleep timing error
        sleep_total = (((dlt_tick << 7) - (dlt_tick << 2) - (dlt_tick << 1) + 2)       >> 2) /* dlt_tick * (128-4-2)/4 */
                      + (((dlt_tick << 3) + dlt_tick + 128)                       >> 9) ; /* dlt_tick *9/512 */
        //+2,+128 for zero-mean quanization noise

    }

    // restore systick
    g_osal_tick_trim = (pGlobal_config[OSAL_SYS_TICK_WAKEUP_TRIM] + g_TIM2_IRQ_to_Sleep_DeltTick + 2500 - g_TIM2_IRQ_PendingTick) >> 2; //16 is used to compensate the cal delay
    g_osalTickTrim_mod += (pGlobal_config[OSAL_SYS_TICK_WAKEUP_TRIM] + g_TIM2_IRQ_to_Sleep_DeltTick + 2500 - g_TIM2_IRQ_PendingTick) & 0x03; //16 is used to compensate the cal delay

    if (g_osalTickTrim_mod > 4) {
        g_osal_tick_trim += 1;
        g_osalTickTrim_mod = g_osalTickTrim_mod % 4;
    }

    // restore systick
//    osal_sys_tick += (sleep_total + g_osal_tick_trim) / 625;    // convert to 625us systick
    //  rtc_mod_value += ((sleep_total + g_osal_tick_trim) % 625);

    if (rtc_mod_value > 625) {
        osal_sys_tick += 1;
        rtc_mod_value = rtc_mod_value % 625;
    }

    //  gpio_write(DBGIO_APP_WAKEUP, 1);
    osalTimeUpdate();

//   gpio_write(DBGIO_APP_WAKEUP, 0);
    // osal time update, not required. It will be updated when osal_run_system() is called after wakeup

    // TODO: should we consider widen the time drift window  ????

    //20190117 ZQ
    if (llState != LL_STATE_IDLE) {
        // SW delay
        wakeup_time = read_current_fine_time() - wakeup_time0;

        next_time = 0;

        if (ll_remain_time > sleep_total + wakeup_time) {
            next_time = ll_remain_time - sleep_total - wakeup_time;
            // restore LL timer
            set_timer(AP_TIM1, next_time);
        } else {
            // should not be here
            set_timer(AP_TIM1, 1000);
        }
    }

    if (g_llSleepContext.isTimer4RecoverRequired) {
        // SW delay
        wakeup_time = read_current_fine_time() - wakeup_time0;

        next_time = 0;

        if (g_llSleepContext.timer4Remainder > sleep_total + wakeup_time) {
            next_time = g_llSleepContext.timer4Remainder - sleep_total - wakeup_time;
            // restore LL timer
            set_timer(AP_TIM4, next_time);
        } else {
            // should not be here
            set_timer(AP_TIM4, 1500);
            //  next_time = 0xffff;
        }

        g_llSleepContext.isTimer4RecoverRequired = FALSE;
    }

    //app could add operation after wakeup
    //app_wakeup_process();
    //uart_tx0(" 111 ");
    //printf("wakeup\n");

    ll_debug_output(DEBUG_WAKEUP);

    set_sleep_flag(0);

    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 1);

    hal_cache_init();
    hal_mpu_config();

    //clk_spif_ref_clk(g_spif_ref_clk);

    CPSR_ALLOC();
    psr = irq_flags;
    RHINO_CPU_INTRPT_ENABLE();

    wdt_wakeup_action();
    //usart_wakeup_action();
    pm_after_sleep_action();

    DBG_GPIO_WRITE(DBGIO_APP_WAKEUP, 0);

    //gpio_write(DBGIO_APP_WAKEUP, 1);
    //gpio_write(DBGIO_APP_WAKEUP, 0);

    // ==== measure value, from RTC counter meet comparator 0 -> here : 260us ~ 270us
    // start task loop
    // osal_start_system();
    uint32_t sleep_total_us = sleep_total + g_osal_tick_trim;
    sleep_total_us = ((sleep_total_us << 7) + (sleep_total_us << 1) + sleep_total_us) >> 17; //sleep_total_us/1000
    return sleep_total_us;
}

void drv_pm_sleep_enable() {
    enableSleepInPM(0xFF);
}

void drv_pm_sleep_disable() {
    disableSleepInPM(0xFF);
}

int  drv_pm_enter_standby() {
    return csi_pmu_enter_sleep(pmu_handle, PMU_MODE_STANDBY);
}

int drv_pm_ram_retention(uint32_t sram) {
    return hal_pwrmgr_RAM_retention(sram);
}



void drv_pm_io_wakeup_handler_unregister() {
    g_wakeup_cb = NULL;
}

void drv_pm_io_wakeup_handler_register(pm_wakeup_by_io_cb wakeup_cb) {
    g_wakeup_cb = wakeup_cb;
}


