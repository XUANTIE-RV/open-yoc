/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_pmu.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <drv/pmu.h>
#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
#endif
#include <drv/eflash.h>
#include <ck_pmu.h>
#include <soc.h>

#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)
#define PMU_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_PMU(DRV_ERROR_PARAMETER))


typedef struct {
    uint8_t idx;
    uint32_t base;
    uint32_t irq;
    pmu_event_cb_t cb;
} ck_pmu_priv_t;

typedef enum {
    WAIT_MODE = 0,
    DOZE_MODE,
    STOP_MODE,
    STANDBY_MODE,
    SLEEP_MODE
} lpm_mode_e;

extern int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t arch_do_cpu_save(void);
extern int32_t arch_do_cpu_resume(void);
extern int32_t arch_resume_context(void);

static uint8_t pmu_power_status;
static ck_pmu_priv_t pmu_handle[CONFIG_PMU_NUM];
#ifndef CONFIG_TEE_CA
#define CONFIG_PMU_REGISTER_NUM_SAVE  19
static uint32_t pmu_regs_saved[CONFIG_PMU_REGISTER_NUM_SAVE];
#endif
#define CONFIG_CORETIM_REGISTER_NUM_SAVE    2
static uint32_t cortim_regs_saved[CONFIG_CORETIM_REGISTER_NUM_SAVE];
#define CONFIG_VIC_REGISTER_NUM_SAVE    3
static uint32_t vic_regs_saved[CONFIG_VIC_REGISTER_NUM_SAVE];

#define CONFIG_CPU_REGISTER_NUM_SAVE    28
uint32_t g_arch_cpu_saved[CONFIG_CPU_REGISTER_NUM_SAVE];

#define CONFIG_IOCTRL_REGISTER_NUM_SAVE 19
uint32_t io_regs_saved[CONFIG_IOCTRL_REGISTER_NUM_SAVE];
/* Driver Capabilities */
//
// Functions
//

static void do_prepare_sleep_action(int32_t idx)
{
    uint8_t i;
#ifndef CONFIG_TEE_CA
    volatile ck_pmu_reg_t *pbase = (ck_pmu_reg_t *)pmu_handle[idx].base;

    for (i = 0; i < sizeof(pmu_regs_saved) / 4; i++) {
        pmu_regs_saved[i] = *((volatile uint32_t *)pbase + i);
    }

#endif

    /* save vic register */
    volatile uint32_t *ibase = (uint32_t *)(CSKY_GPIO0_BASE + 0x100);
    vic_regs_saved[0] = VIC->ISER[0U];
    vic_regs_saved[1] = VIC->ISPR[0U];
    vic_regs_saved[2] = VIC->IPTR;

    /* save the coretim register */
    cortim_regs_saved[0] = CORET->LOAD;
    cortim_regs_saved[1] = CORET->CTRL;

    /* save the ioctrl register */
    for (i = 0; i < sizeof(io_regs_saved) / 4; i++) {
        if (i < 17) {
            io_regs_saved[i] = *((volatile uint32_t *)ibase + i);
        } else if (i == 17) {
            io_regs_saved[i] = *((volatile uint32_t *)(CSKY_GPIO0_BASE + 0x8));
        } else if (i == 18) {
            io_regs_saved[i] = *((volatile uint32_t *)(CSKY_GPIO1_BASE + 0x8));
        }
    }
}

static void do_wakeup_sleep_action(int32_t idx)
{
    uint8_t i;
#ifndef CONFIG_TEE_CA
    uint32_t timeout = 0;
    volatile ck_pmu_reg_t *pbase = (ck_pmu_reg_t *)pmu_handle[idx].base;
    *((volatile uint32_t *)pbase + 5) = pmu_regs_saved[5];

    while ((*((volatile uint32_t *)pbase + 6) & 0xf) != 0xf && timeout < 0x10000000) {
        timeout++;
    }

    timeout = 0;
    *((volatile uint32_t *)pbase + 11) = pmu_regs_saved[11];

    while ((*((volatile uint32_t *)pbase + 6) & 0x1f) != 0x1f && timeout < 0x10000000) {
        timeout++;
    }

    for (i = 0; i < sizeof(pmu_regs_saved) / 4; i++) {
        if (i != 5 && i != 11) {
            *((volatile uint32_t *)pbase + i) = pmu_regs_saved[i];
        }
    }

#endif

    /* resume vic register */
    volatile uint32_t *ibase = (uint32_t *)(CSKY_GPIO0_BASE + 0x100);
    VIC->ISER[0U] = vic_regs_saved[0];
    VIC->ISPR[0U] = vic_regs_saved[1];
    VIC->IPTR = vic_regs_saved[2];

    /* resume the ioctrl register */
    for (i = 0; i < sizeof(io_regs_saved) / 4; i++) {
        if (i < 17) {
            *((volatile uint32_t *)ibase + i) = io_regs_saved[i];
        } else if (i == 17) {
            *((volatile uint32_t *)(CSKY_GPIO0_BASE + 0x8)) = io_regs_saved[i];
        } else if (i == 18) {
            *((volatile uint32_t *)(CSKY_GPIO1_BASE + 0x8)) = io_regs_saved[i];
        }
    }

    /* resume the coretim register */
    CORET->LOAD = cortim_regs_saved[0];
    CORET->CTRL = cortim_regs_saved[1];

}

void resume_context_from_stop_mode(void)
{
    arch_do_cpu_resume();
}

#ifndef CONFIG_TEE_CA
static void set_resume_func(uint32_t *func)
{
    eflash_handle_t eflash = csi_eflash_initialize(0, NULL);
    csi_eflash_erase_sector(eflash, CONFIG_LPM_RESUME_ADDR);
    csi_eflash_program(eflash, CONFIG_LPM_RESUME_ADDR, &func, 4);
}
#endif

void soc_sleep(pmu_handle_t handle, lpm_mode_e mode)
{
#ifdef CONFIG_TEE_CA
    tee_lpm_mode_e lpm_mode = 0;

    if (mode == WAIT_MODE) {
        lpm_mode = TEE_LPM_MODE_WAIT;
    } else if (mode == DOZE_MODE) {
        lpm_mode = TEE_LPM_MODE_DOZE;
    } else if (mode == STOP_MODE) {
        lpm_mode = TEE_LPM_MODE_STOP;
    } else if (mode == STANDBY_MODE) {
        lpm_mode = TEE_LPM_MODE_STANDBY;
    } else {
        lpm_mode = TEE_LPM_MODE_WAIT;
    }

    csi_tee_enter_lpm((uint32_t)g_arch_cpu_saved, 0, lpm_mode);

    if (mode == STOP_MODE) {
        resume_context_from_stop_mode();
    }

#else

    ck_pmu_priv_t *pmu_priv = handle;
    ck_pmu_reg_t *pmu_reg = (ck_pmu_reg_t *)pmu_priv->base;

    set_resume_func((uint32_t *)&arch_resume_context);

    if (mode == WAIT_MODE) {
        pmu_reg->LPCR |= CONFIG_PMU_ENTER_WAIT_MODE;
        __WFI();
    } else if (mode == DOZE_MODE) {
        pmu_reg->LPCR |= CONFIG_PMU_ENTER_DOZE_MODE;
        __DOZE();
        /* large the stable time only for ch2201 */
        pmu_reg->CSSCR = EHS_CLOCK_STABLE_TIME;
    } else if (mode == STOP_MODE) {
        pmu_reg->LPCR &= ~(3 << 3);
        pmu_reg->LPCR |= CONFIG_PMU_ENTER_STOP_MODE;
        __STOP();
    } else if (mode == STANDBY_MODE) {
        pmu_reg->LPCR |= CONFIG_PMU_ENTER_STANDBY_MODE;
        __STOP();
    } else {
        pmu_reg->LPCR |= CONFIG_PMU_ENTER_WAIT_MODE;
        __WFI();
    }

#endif
}

/**
  \brief       Initialize PMU Interface. 1. Initializes the resources needed for the PMU interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref pmu_event_cb_t
  \return      pointer to pmu handle
*/
pmu_handle_t csi_pmu_initialize(int32_t idx, pmu_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_PMU_NUM) {
        return NULL;
    }

    /* obtain the pmu information */
    uint32_t base = 0u;
    uint32_t irq = 0u;
    int32_t real_idx = target_get_pmu(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    ck_pmu_priv_t *pmu_priv = &pmu_handle[idx];

    /* initialize the pmu context */
    pmu_priv->idx = idx;
    pmu_priv->base = base;
    pmu_priv->irq = irq;
    pmu_priv->cb = cb_event;

    return (pmu_handle_t)pmu_priv;
}

/**
  \brief       De-initialize PMU Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  pmu handle to operate.
  \return      error code
*/
int32_t csi_pmu_uninitialize(pmu_handle_t handle)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;
    pmu_priv->cb = NULL;

    return 0;
}

int32_t csi_pmu_power_control(pmu_handle_t handle, csi_power_stat_e state)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;

    switch (state) {
        case DRV_POWER_OFF:
        case DRV_POWER_LOW:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case DRV_POWER_FULL:
            if (pmu_power_status == DRV_POWER_SUSPEND) {
                do_wakeup_sleep_action(pmu_priv->idx);
            }

            break;

        case DRV_POWER_SUSPEND:
            do_prepare_sleep_action(pmu_priv->idx);
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    pmu_power_status = state;

    return 0;
}

/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
int32_t csi_pmu_enter_sleep(pmu_handle_t handle, pmu_mode_e mode)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;

    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */

    switch (mode) {
        case PMU_MODE_RUN:
            break;

        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case PMU_MODE_SLEEP:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            soc_sleep(handle, WAIT_MODE);

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_DOZE:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            do_prepare_sleep_action(0);
            soc_sleep(handle, DOZE_MODE);
            do_wakeup_sleep_action(0);

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_DORMANT:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            if (arch_do_cpu_save() == 0) {
                soc_sleep(handle, STOP_MODE);
            }

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_STANDBY:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            soc_sleep(handle, STANDBY_MODE);
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   type    \ref pmu_wakeup_type
  \param[in]   pol     \ref pmu_wakeup_pol
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
int32_t csi_pmu_config_wakeup_source(pmu_handle_t handle, uint32_t wakeup_num, pmu_wakeup_type_e type, pmu_wakeup_pol_e pol, uint8_t enable)
{
    PMU_NULL_PARAM_CHK(handle);

    if (wakeup_num >= 32 || (type != PMU_WAKEUP_TYPE_LEVEL) || (pol != PMU_WAKEUP_POL_HIGH)) {
        return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    if (enable) {
        csi_vic_set_wakeup_irq(wakeup_num);
    } else {
        csi_vic_clear_wakeup_irq(wakeup_num);
    }

    return 0;
}
