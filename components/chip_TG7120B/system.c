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
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_core.h>
#include <jump_function.h>
#include "clock.h"
#include "mcu.h"
#include "rom_sym_def.h"
#include "soc.h"
#include "global_config.h"
#include "gpio.h"
#include "pwrmgr.h"
#include "rf_phy_driver.h"
#include "k_api.h"
#include "sys_freq.h"
#include <drv/irq.h>
#include "pm.h"

extern volatile sysclk_t       g_system_clk;
int g_spif_ref_clk = SYS_CLK_DLL_64M;
extern void trap_c(uint32_t *regs);
extern uint32_t g_system_clk;

volatile uint8 g_clk32K_config;

extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void init_config(void);
extern void hal_rfphy_init(void);
extern int clk_spif_ref_clk(sysclk_t spif_ref_sel);

extern void Reset_Handler();
extern void Default_Handler();
extern void tspend_handler();
extern void Default_IRQHandler_TG7120B();
extern int spif_config(sysclk_t ref_clk, uint8_t div,  uint32_t rd_instr,  uint8_t mode_bit, uint8_t QE);

uint32_t TG7120B_Vectors[64] = {0};
/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */

__attribute__((section(".__sram.code")))  void hal_cache_init(void)
{
    volatile int dly = 100;
    //clock gate
    hal_clk_gate_enable(MOD_HCLK_CACHE);
    hal_clk_gate_enable(MOD_PCLK_CACHE);

    //cache rst
    AP_PCR->CACHE_RST = 0x00;

    while (dly--) {};


    AP_PCR->CACHE_RST = 0x03;

    //cache flush tag
    AP_CACHE->CTRL0 = 0x01;

    //cache enable
    AP_PCR->CACHE_BYPASS = 0;

    spif_config(g_spif_ref_clk, 0x1, 0x801003b, 0, 0);

    AP_SPIF->low_wr_protection = 0;

    AP_SPIF->up_wr_protection = 0x10;

    AP_SPIF->wr_protection = 0x2;
}

void hal_mpu_config(void)
{
    mpu_region_attr_t attr0 = {
        .nx = 0,
        .ap = AP_BOTH_RW,
        .s = 0,
    };
    mpu_region_attr_t attr1 = {
        .nx = 0,
        .ap = AP_BOTH_RW,
        .s = 1,
    };

    csi_mpu_config_region(0, 0,          REGION_SIZE_4GB, attr0, 1);
    csi_mpu_config_region(1, 0x11000000, REGION_SIZE_512KB, attr1, 1);
    csi_mpu_enable();
}

void hal_wakeup_irq_config(void)
{
    subWriteReg(0x40003030, 4, 4, 1); //ck802 WFI enable
    NVIC_SetWakeupIRQ(1);
    NVIC_SetWakeupIRQ(TIM1_IRQn);
    NVIC_SetWakeupIRQ(TIM2_IRQn);
    //NVIC_SetWakeupIRQ(TIM3_IRQn);
    NVIC_SetWakeupIRQ(TIM4_IRQn);
    NVIC_SetWakeupIRQ(BB_IRQn);
}

#ifdef CONFIG_KERNEL_NONE
static void _system_init_for_baremetal(void)
{
    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms

    __enable_excp_irq();

    mm_heap_initialize();
}
#endif

#ifndef CONFIG_KERNEL_NONE
static void _system_init_for_kernel(void)
{
#ifndef CONFIG_SUPPORT_TSPEND
    irq_vectors_init();
#endif
    //csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
    //drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#endif
}
#endif

void my_trap_c(void)
{
    while (1);
}

static void hal_low_power_io_init(void)
{
    //========= pull all io to gnd by default
    ioinit_cfg_t ioInit[GPIO_NUM] = {
        {GPIO_P00,   GPIO_PULL_DOWN  },
        {GPIO_P01,   GPIO_PULL_DOWN  },
        {GPIO_P02,   GPIO_FLOATING   },/*SWD*/
        {GPIO_P03,   GPIO_FLOATING   },/*SWD*/
        {GPIO_P07,   GPIO_PULL_DOWN  },
        {GPIO_P09,   GPIO_PULL_UP    },/*UART TX*/
        {GPIO_P10,   GPIO_PULL_UP    },/*UART RX*/
        {GPIO_P11,   GPIO_PULL_DOWN  },
        {GPIO_P14,   GPIO_PULL_DOWN  },
        {GPIO_P15,   GPIO_PULL_DOWN  },
        {GPIO_P16,   GPIO_FLOATING   },/*32k xtal*/
        {GPIO_P17,   GPIO_FLOATING   },/*32k xtal*/
        {GPIO_P18,   GPIO_PULL_DOWN  },
        {GPIO_P20,   GPIO_PULL_DOWN  },
        {GPIO_P23,   GPIO_PULL_DOWN  },
        {GPIO_P24,   GPIO_PULL_DOWN  },
        {GPIO_P25,   GPIO_PULL_DOWN  },
        {GPIO_P26,   GPIO_PULL_DOWN  },
        {GPIO_P27,   GPIO_PULL_DOWN  },
        {GPIO_P31,   GPIO_PULL_DOWN  },
        {GPIO_P32,   GPIO_PULL_DOWN  },
        {GPIO_P33,   GPIO_PULL_DOWN  },
        {GPIO_P34,   GPIO_PULL_DOWN  },
    };

    for (uint8_t i = 0; i < GPIO_NUM; i++) {
        phy_gpio_pull_set(ioInit[i].pin, ioInit[i].type);
    }

    DCDC_REF_CLK_SETTING(1);
    DCDC_CONFIG_SETTING(0x0a);
    DIG_LDO_CURRENT_SETTING(0x01);
    drv_pm_ram_retention(RET_SRAM0 | RET_SRAM1 | RET_SRAM2);
    //hal_pwrmgr_RAM_retention(RET_SRAM0);
    hal_pwrmgr_RAM_retention_set();
    hal_pwrmgr_LowCurrentLdo_enable();
    //========= low power module clk gate
#if(PHY_MCU_TYPE==MCU_BUMBEE_CK802)
    *(volatile uint32_t *)0x40000008 = 0x001961f1;  //
    *(volatile uint32_t *)0x40000014 = 0x01e00278;  //
#else

    *(volatile uint32_t *)0x40000008 = 0x001961f0;  //
    *(volatile uint32_t *)0x40000014 = 0x01e00279;  //
#endif

}

static void hal_init(void)
{
    hal_low_power_io_init();

    //hal_pwrmgr_init();
}

static void config_irq_priority(void)
{
    int i;

    for (i = 1; i <= 31; i++) {
        csi_vic_set_prio(i, 1);
    }
}

//int check_data __attribute__((section(".__data_copy_data_first__"))) = 1;

__attribute__((section(".__data_copy_first__")))  void check_before_startup()
{
    //check_data = -1;
}

void SystemInit(void)
{
    /* Clear active and pending IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;

    __disable_irq();

    g_system_clk = SYS_CLK_DLL_48M;

#ifdef CONFIG_USE_XTAL_CLK
    g_clk32K_config = CLK_32K_XTAL;//CLK_32K_XTAL,CLK_32K_RCOSC
#else
    g_clk32K_config = CLK_32K_RCOSC;//CLK_32K_XTAL,CLK_32K_RCOSC
#endif

    g_spif_ref_clk = SYS_CLK_DLL_64M;

    hal_rtc_clock_config(g_clk32K_config);

    JUMP_FUNCTION(CK802_TRAP_C) = (uint32_t)&trap_c;           //register trap irq handler

#ifdef CONFIG_KERNEL_NONE
    _system_init_for_baremetal();
#else
    _system_init_for_kernel();
#endif

    init_config();

    hal_rfphy_init();

    int i = 0;

    TG7120B_Vectors[0] = (uint32_t)Reset_Handler;

    for (i = 1; i < 32; i++) {
        TG7120B_Vectors[i] = (uint32_t)Default_Handler;
    }

    TG7120B_Vectors[32] = (uint32_t)tspend_handler;

    for (i = 33; i < 64; i++) {
        TG7120B_Vectors[i] = (uint32_t)Default_IRQHandler_TG7120B;
    }

    __set_VBR((uint32_t)(TG7120B_Vectors));

    config_irq_priority();

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
    drv_irq_enable(CORET_IRQn);

    //replace TS_PEND IRQ with IRQ0
    drv_irq_enable(0);
    csi_vic_set_prio(0, 3);
    hal_cache_init();

    hal_mpu_config();
    hal_wakeup_irq_config();

    hal_init();
}


