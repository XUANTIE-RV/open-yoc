/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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
/******************************************************************************
 * @file     ck_pmu.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdint.h>
#include <drv/pmu.h>
#include <soc.h>

#include <jump_function.h>
#include <ll_sleep.h>

#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)
#define PMU_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_PMU(DRV_ERROR_PARAMETER))

typedef struct {
    uint8_t idx;
    pmu_event_cb_t cb;
} phy_pmu_priv_t;

extern int32_t arch_do_cpu_save(void);
extern int32_t arch_do_cpu_resume(void);

static uint8_t pmu_power_status;
static phy_pmu_priv_t pmu_handle[CONFIG_PMU_NUM];

#define CONFIG_CORETIM_REGISTER_NUM_SAVE    2
static uint32_t cortim_regs_saved[CONFIG_CORETIM_REGISTER_NUM_SAVE];

#define CONFIG_VIC_REGISTER_NUM_SAVE    11
static uint32_t vic_regs_saved[CONFIG_VIC_REGISTER_NUM_SAVE];

#define CONFIG_CPU_REGISTER_NUM_SAVE    28
uint32_t g_arch_cpu_saved[CONFIG_CPU_REGISTER_NUM_SAVE];

/* Driver Capabilities */
//
// Functions
//
static void do_prepare_sleep_action(int32_t idx)
{
    int i = 0;
    /* save vic register */
    vic_regs_saved[0] = VIC->ISER[0U];
    vic_regs_saved[1] = VIC->ISPR[0U];
    vic_regs_saved[2] = VIC->IPTR;
    for (i = 0;i < 8; i++)
    {
        vic_regs_saved[3 + i] = VIC->IPR[i];
    }

    /* save the coretim register */
    cortim_regs_saved[0] = CORET->LOAD;
    cortim_regs_saved[1] = CORET->CTRL;
}

static void do_wakeup_sleep_action(int32_t idx)
{
    int i = 0;
    /* resume vic register */
    VIC->ISER[0U] = vic_regs_saved[0];
    VIC->ISPR[0U] = vic_regs_saved[1];
    VIC->IPTR = vic_regs_saved[2];
    for (i = 0;i < 8; i++)
    {
        VIC->IPR[i] = vic_regs_saved[3 + i];
    }

    /* resume the coretim register */
    CORET->LOAD = cortim_regs_saved[0];
    CORET->VAL = 0UL;
    CORET->CTRL = cortim_regs_saved[1];
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

    phy_pmu_priv_t *pmu_priv = &pmu_handle[idx];

    /* initialize the pmu context */
    pmu_priv->idx = idx;
    pmu_priv->cb = cb_event;

    JUMP_FUNCTION(WAKEUP_PROCESS) = (uint32_t)arch_do_cpu_resume;

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

    phy_pmu_priv_t *pmu_priv = handle;
    pmu_priv->cb = NULL;

    return 0;
}

int32_t csi_pmu_power_control(pmu_handle_t handle, csi_power_stat_e state)
{
    PMU_NULL_PARAM_CHK(handle);

    phy_pmu_priv_t *pmu_priv = handle;

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

    phy_pmu_priv_t *pmu_priv = handle;

    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */

    switch (mode) {
        case PMU_MODE_RUN:
            break;

        case PMU_MODE_SLEEP:
        case PMU_MODE_DOZE:
        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case PMU_MODE_DORMANT:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            do_prepare_sleep_action(0);

            if (arch_do_cpu_save() == 0) {
                enter_sleep_off_mode(SYSTEM_SLEEP_MODE);
            }

            do_wakeup_sleep_action(0);

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_STANDBY:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }
            set_sleep_flag(0);
            *((unsigned int *)0x4000f0c0) = 0x2;
            subWriteReg(0x4000f01c, 6, 6, 0x00); //disable software control
            enter_sleep_off_mode(SYSTEM_OFF_MODE);
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
