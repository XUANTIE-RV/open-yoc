/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_msg_cmd.h>
#ifdef CONFIG_PLATFORM_HOBBIT1_2
#include <drv_pmu.h>

extern int32_t arch_do_cpu_save(void);
extern int32_t arch_resume_context(void);
extern int32_t non_cpu_saved_addr;

#define CONFIG_CPU_REGISTER_NUM_SAVE    28
uint32_t arch_tw_cpu_saved[CONFIG_CPU_REGISTER_NUM_SAVE];
#endif
#if (CONFIG_ALGO_LPM > 0)

typedef enum {
    LPM_MODE_WAIT     = 0,
    LPM_MODE_DOZE     = 1,
    LPM_MODE_STOP     = 2,
    LPM_MODE_STANDBY  = 3,
    LPM_MODE_MAX,
} lpm_mode_e;

__attribute__((weak)) void _tw_to_stop(void)
{
    return;
}

int tee_core_lpm(tee_param params[4])
{
#ifdef CONFIG_PLATFORM_HOBBIT1_2
    uint32_t gate   = params[0].value.a;
#endif
    uint32_t irqid  = params[0].value.b;
    lpm_mode_e  mode  = params[1].value.a;

//    (void)gate;
    (void)irqid;

    switch (mode) {
        case LPM_MODE_WAIT:

#ifdef CONFIG_PLATFORM_PHOBOS
            *(volatile uint32_t *)0x40000000 |= 0x04;
#else
#ifdef CONFIG_PLATFORM_HOBBIT1_2
            *(volatile uint32_t *)0x40002000 = 0x04;
#endif
#endif

            __asm volatile("wait");
            break;

        case LPM_MODE_DOZE:
#ifdef CONFIG_PLATFORM_HOBBIT1_2
            drv_pmu_power_control(0, DRV_POWER_OFF);
            *(volatile uint32_t *)0x40002000 |= 0x04;
            __asm volatile("doze");
            *(volatile uint32_t *)0x40002024 = 0x2000;
            drv_pmu_power_control(0, DRV_POWER_OFF);
#endif
            break;

        case LPM_MODE_STOP:
#ifdef CONFIG_PLATFORM_PHOBOS
                *(volatile uint32_t *)0x40000000 |= 0x04;
                __asm volatile("stop");
#endif
#ifdef CONFIG_PLATFORM_HOBBIT1_2
            non_cpu_saved_addr = gate;
            if (*(volatile uint32_t *)0x1003f7f0 != (uint32_t)arch_resume_context) {
                *(volatile uint32_t *)0x4003f004=0x1003f7f0;
                *(volatile uint32_t *)0x4003f010=0x1;
                *(volatile uint32_t *)0x4003f01c=(uint32_t)arch_resume_context;
                *(volatile uint32_t *)0x4003f018=0x1;
            }
            *(volatile uint32_t *)0x40002000 &= ~(3 << 3);
            drv_pmu_power_control(0, DRV_POWER_OFF);
            if (arch_do_cpu_save() == 0) {
                *(volatile uint32_t *)0x40002000 |= 0x2c;
                *(volatile uint32_t *)0x40002004 = 0x0;
                *(volatile uint32_t *)0x4000202c = 0x202;
                __asm volatile("stop");
            }
            drv_pmu_power_control(0, DRV_POWER_OFF);
#endif
#ifdef CONFIG_PLATFORM_ZX297100
            __asm volatile("stop");
#endif
            break;
        case LPM_MODE_STANDBY:
#ifdef CONFIG_PLATFORM_PHOBOS
            *(volatile uint32_t *)0x40000000 |= 0x04;
#else
#ifdef CONFIG_PLATFORM_HOBBIT1_2
            *(volatile uint32_t *)0x40002000 &= ~(3 << 3);
            *(volatile uint32_t *)0x40002000 |= 0x34;
            *(volatile uint32_t *)0x40002004 = 0x0;
            *(volatile uint32_t *)0x4000202c = 0x202;
#endif
#endif

#ifdef CONFIG_PLATFORM_ZX297100
            _tw_to_stop();
#else
            __asm volatile("stop");
#endif
            break;
        default:
            return TEE_ERROR_BAD_PARAMETERS;
            break;
    }

    return TEE_SUCCESS;
}
#endif
