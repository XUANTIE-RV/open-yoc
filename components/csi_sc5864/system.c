/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <soc.h>
#include <csi_core.h>
#include <silan_pic.h>
#include <silan_pmu.h>
#include <syscfg.h>
#include <dw_mmc_reg.h>

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

extern uint32_t __Vectors[];
extern int32_t g_top_irqstack;

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
int SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency      */

int32_t drv_get_cpu_id(void)
{
    return 0;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = SYSTEM_CLOCK;
}

void config_system_clock(void)
{
    drv_clk_disable(CLK_USBHS);
    drv_clk_disable(CLK_USBFS);
    drv_clk_disable(CLK_OSPDIF);
    drv_clk_disable(CLK_ISPDIF);
    // drv_clk_disable(CLK_PCM_I2S);
    drv_clk_enable(CLK_PCM_I2S);
    drv_clk_disable(CLK_UART1);
    drv_clk_disable(CLK_UART3);
    drv_clk_disable(CLK_UART4);
    drv_clk_disable(CLK_I2C1);
    drv_clk_disable(CLK_I2C2);
    drv_clk_disable(CLK_PDM);
    drv_clk_disable(CLK_IIR);
    drv_clk_disable(CLK_PDP);

    silan_mcu_cclk_onoff(CLK_OFF);
}

void disable_sdmmc(void)
{
    SDIF_TYPE *sdmmc = (SDIF_TYPE *)SILAN_SDMMC_BASE;
    SDIF_TYPE *sdio = (SDIF_TYPE *)SILAN_SDIO_BASE;

    sdmmc->CTRL = 0;
    sdmmc->PWREN = 0;
    sdio->CTRL = 0;
    sdio->PWREN = 0;
}

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    config_system_clock();
    silan_usb_pwd_deal();
    disable_sdmmc();

    /* Here we may setting exception vector, MGU, cache, and so on. */
    __set_VBR((uint32_t) & (__Vectors));

    /* Clear active and pending IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;

    csi_dcache_clean_invalid();
    csi_icache_invalid();
    csi_cache_set_range(0, 0x02000000, CACHE_CRCR_8M, 1);
    csi_icache_enable();
    csi_dcache_enable();

    csi_mpu_disable();

    silan_pic_init();

#ifdef CONFIG_KERNEL_NONE
    __enable_excp_irq();
#endif

    SystemCoreClockUpdate();

#ifndef CONFIG_KERNEL_NONE
    csi_coret_config(SystemCoreClock / CONFIG_SYSTICK_HZ, PIC_IRQID_CTIMER);    //1ms
    csi_vic_enable_irq(PIC_IRQID_CTIMER);
#else
    csi_coret_config(SystemCoreClock / CONFIG_SYSTICK_HZ, PIC_IRQID_CTIMER);    //for mdelay()
#endif

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    extern void mm_heap_initialize(void);
    mm_heap_initialize();
#endif
#endif
}
