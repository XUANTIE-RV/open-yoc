/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <pinmux.h>
#include <soc.h>
#include <drv/rtc.h>
#include <drv/pmu.h>

#include <aos/aos.h>
#include <devices/driver.h>
#include <devices/device.h>
#include <devices/flash.h>
#include <yoc/partition.h>

#include <yoc/lpm.h>

#define PART_ADDR_FLASH_BEGIN           (0x10000000)

/* Constants identifying power state categories */
#define SYS_PM_ACTIVE_STATE     0 /* SOC and CPU are in active state */
#define SYS_PM_LOW_POWER_STATE      1 /* CPU low power state */
#define SYS_PM_DEEP_SLEEP       2 /* SOC low power state */

#define SYS_PM_NOT_HANDLED      SYS_PM_ACTIVE_STATE

#define MIN_TIME_TO_SLEEP       50      //ms
#define MIN_TIME_TO_SUSPEND     10000   //ms

enum power_states {
    SYS_POWER_STATE_RUN,       /* C1 state */
    SYS_POWER_STATE_WAIT,       /* C1 state */
    SYS_POWER_STATE_DOZE,     /* C2 state */
    SYS_POWER_STATE_STOP,     /* C2LP state */
    SYS_POWER_STATE_STANDBY,    /* SLEEP state */

    SYS_POWER_STATE_MAX
};

static pmu_handle_t pmu_handle = NULL;
static rtc_handle_t g_rtc_handle;

static const char *TAG = "soc_pm";

static void rtc_cb(int32_t idx, rtc_event_e event)
{
    LOGI("lpm", "rtc event");
}

static int rtc_init(void)
{
    struct tm current_time;
    int ret = 0;

    if (g_rtc_handle)
        return 0;

    current_time.tm_sec  = 10;
    current_time.tm_min  = 26;
    current_time.tm_hour = 17;
    current_time.tm_mday = 4;
    current_time.tm_mon  = 7;
    current_time.tm_year = 117;

    g_rtc_handle = csi_rtc_initialize(0, rtc_cb);
    if (g_rtc_handle == NULL) {
        LOGE("lpm", "rtc init error");
        return -1;
    }

    //run about 10 second
    ret = csi_rtc_set_time(g_rtc_handle, &current_time);
    if (ret < 0) {
        csi_rtc_uninitialize(g_rtc_handle);
        LOGE("lpm", "csi rtc timer set error");
        return -1;
    }

    ret = csi_rtc_start(g_rtc_handle);
    if (ret < 0) {
        csi_rtc_uninitialize(g_rtc_handle);
        LOGE("lpm", "csi rtc start error");
        return -1;
    }

    return 0;
}

static int rtc_timer_start(uint32_t ms)
{
    int32_t ret = 0;
    time_t  sec_time = 0;
    struct tm tm_time;

    ret =  csi_rtc_get_time(g_rtc_handle, &tm_time);

    if (ret < 0) {
        LOGE("lpm", "rtc get time error");
        return -1;
    }

    sec_time = mktime(&tm_time);

    sec_time += (ms / 1000);

    gmtime_r(&sec_time, &tm_time);

    ret = csi_rtc_set_alarm(g_rtc_handle, &tm_time);

    if (ret < 0) {
        LOGE("lpm", "rtc timer config error");
        return -1;
    }

    ret = csi_rtc_enable_alarm(g_rtc_handle, 1);

    if (ret < 0) {
        LOGE("lpm", "rtc timer enable error");
        return -1;
    }

    return ret;
}

/*
        RUN WAIT    DOZE    STOP        STANDBY
EHS     X1  X1      OFF     SHUT DOWN   SHUT DOWN
IHS     X1  X1      OFF     SHUT DOWN   SHUT DOWN
PLL     X1  OFF     OFF     SHUT DOWN   SHUT DOWN
ELS     X1  X1      X1      X1          X1
ILS     X1  X1      X1      X1          X1
CPU     ON  OFF     OFF     SHUT DOWN   SHUT DOWN
FLASH   ON  X       OFF     SHUT DOWN   SHUT DOWN
RAM     ON  X       OFF     OFF         SHUT DOWN
RTC     X   X       X       X           X
PMU     ON  ON      ON      ON          ON
PERIPH  X   X       OFF     SHUT DOWN   SHUT DOWN

X: clock programmable on; power on
ON: clock on; power on
OFF: clock off; power on
*/
static int check_pm_policy(pm_ctx_t *pm_ctx)
{
    int pm_state = 0;

    if (pm_ctx->suspend_tick == 0xFFFFFFFFU) {
        return pm_state;
    }

    uint32_t ticks = pm_ctx->suspend_tick;
    uint64_t ms = aos_kernel_tick2ms(ticks);
    int policy = pm_ctx->policy;

    /* policy:
    * 0 = no power saving operation
    * 1 = low power state
    * 2 = deep sleep
    */
    //LOGI(TAG, "suspend %dms", ms);
    if (policy == LPM_POLICY_NO_POWER_SAVE) {
        pm_state = SYS_POWER_STATE_RUN;
    } else if (policy == LPM_POLICY_LOW_POWER) {
        pm_state = SYS_POWER_STATE_STOP;
    } else if (policy == LPM_POLICY_DEEP_SLEEP) {
        if (pm_ctx->agree_halt) {
            //assist application to halt system
            pm_state = SYS_POWER_STATE_STANDBY;
        } else {
            if (ms <= pm_ctx->ls_ms) {
                pm_state = SYS_POWER_STATE_RUN;
            } else if ((ms > pm_ctx->ls_ms) && (ms <= pm_ctx->ds_ms)) {
                pm_state = SYS_POWER_STATE_WAIT;
            } else if (ms > pm_ctx->ds_ms) {
                // check device if busy first
                // ensure data transfer was completed before device is suspended
                if (device_is_busy()) {
                    pm_state = SYS_POWER_STATE_WAIT;
                } else {
                    pm_state = SYS_POWER_STATE_DOZE;
                }
            }
        }
    }

    //LOGI(TAG, "decide to %d", pm_state);

    return pm_state;
}

/*
Mode    Ext Pad GPIO Interrupt  Other IP Interrupt  RTC Reset
Wait    Yes     Yes             Yes   Yes           Yes(reset)
Doze    Yes     Yes             No    Yes           Yes(reset)
Stop    Yes     No              No    Yes           Yes(wakeup)
Standby Yes     No              No    Yes           Yes(wakeup)
*/
static int setup_wake_event_and_sleep(pm_ctx_t *pm_ctx, int pm_state)
{
    int ret = 0;

    if (pm_state >= SYS_POWER_STATE_DOZE) {
        rtc_init();
    }

    csi_vic_set_wakeup_irq(PWR_IRQn);
    csi_vic_set_wakeup_irq(MAILBOX_IRQn);
    csi_vic_set_wakeup_irq(UART0_IRQn);
    csi_vic_set_wakeup_irq(UART1_IRQn);
    csi_vic_set_wakeup_irq(RTC_IRQn);

    switch (pm_state) {
        case SYS_POWER_STATE_WAIT:
            //!was managed by rhino tickless
            //disable rhino tickless, no need to compensate tick
            break;

        case SYS_POWER_STATE_DOZE:
            csi_pmu_enter_sleep(pmu_handle, PMU_MODE_DOZE);
            break;

        case SYS_POWER_STATE_STOP:
            LOGI(TAG, "go to stop mode %d\n", pm_ctx->suspend_tick);
            csi_pmu_enter_sleep(pmu_handle, PMU_MODE_DORMANT);
            break;

        case SYS_POWER_STATE_STANDBY:
            if (pm_ctx->alarm_ms > 0) {
                if (pm_ctx->alarm_ms < 100)
                    pm_ctx->alarm_ms = 100;
                rtc_timer_start(pm_ctx->alarm_ms);  // run about 4 second
            }

            LOGI(TAG, "go to standby mode with %ds\n", pm_ctx->alarm_ms/1000);
            csi_pmu_enter_sleep(pmu_handle, PMU_MODE_STANDBY);
            break;

        default:
            break;
    }

    return ret;
}

/******************************************
// SOC interfaces
******************************************/
int sys_soc_init(pm_ctx_t *pm_ctx)
{
    pmu_handle = csi_pmu_initialize(0, NULL);

    pm_ctx->policy = 0;
    pm_ctx->ls_ms = MIN_TIME_TO_SLEEP;
    pm_ctx->ds_ms = MIN_TIME_TO_SUSPEND;

    return 0;
}

int sys_soc_suspend(pm_ctx_t *pm_ctx)
{
    int pm_state = SYS_POWER_STATE_RUN;
    //uint32_t ticks = pm_ctx->suspend_tick;

    pm_state = check_pm_policy(pm_ctx);

    if (pm_state < SYS_POWER_STATE_WAIT) {
        return pm_state;
    }

    if ((pm_state > SYS_POWER_STATE_WAIT) && (pm_state < SYS_POWER_STATE_STANDBY)) {
        device_manage_power(DEVICE_PM_SUSPEND_STATE);
    } else if (pm_state == SYS_POWER_STATE_STANDBY) {
        device_manage_power(DEVICE_PM_OFF_STATE);
    }

    setup_wake_event_and_sleep(pm_ctx, pm_state);

    return pm_state;
}

int sys_soc_resume(pm_ctx_t *pm_ctx, int pm_state)
{
    uint32_t actual_suspend_ticks = 0;  //

    if (pm_state <= SYS_POWER_STATE_WAIT) {
        //!managed by rhino
        //support systick, no need to compensate tick
        actual_suspend_ticks = 0;
    } else if ((pm_state > SYS_POWER_STATE_WAIT) && (pm_state < SYS_POWER_STATE_STANDBY)) {
        device_manage_power(DEVICE_PM_ACTIVE_STATE);
        //get rtc value, may be waked up by network card
        //set actual_suspend_ticks;
        actual_suspend_ticks = pm_ctx->suspend_tick;//dummy

        LOGI(TAG, "Wake from Deep Sleep");
    }

    return actual_suspend_ticks;
}

