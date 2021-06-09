/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File for YUNVOICE
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdint.h>
#include <io.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/irq.h>

#include <sys_freq.h>

#include "wm_type_def.h"
#include "wm_cpu.h"
#include "wm_osal.h"
#include "wm_pmu.h"
#include "wm_io.h"
#include "wm_gpio_afsel.h"
#include "wm_crypto_hard.h"
#include "wm_include.h"
#include "wm_internal_flash.h"

extern uint32_t irq_vectors[];

extern void irq_vectors_init(void);
extern void uart0Init (int bandrate);
extern ATTRIBUTE_ISR void CORET_IRQHandler(void);
extern ATTRIBUTE_ISR void GPSEC_IRQHandler(void);
extern ATTRIBUTE_ISR void RSA_IRQHandler(void);

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 500
#endif

const unsigned int HZ = CONFIG_SYSTICK_HZ;

static void wm_gpio_config()
{
	/* must call first */
	wm_gpio_af_disable();	

	wm_uart0_tx_config(WM_IO_PB_19);
	wm_uart0_rx_config(WM_IO_PB_20);

	wm_uart1_rx_config(WM_IO_PB_07);
	wm_uart1_tx_config(WM_IO_PB_06);
}

void w800_board_init(void)
{
    static int w800_board_is_init = 0;

    if (!w800_board_is_init)
    {
        tls_fls_init();
        tls_fls_sys_param_postion_init();

    	tls_ft_param_init();
        tls_param_load_factory_default();
        tls_param_init();

        w800_board_is_init = 1;
    }
}

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    __set_VBR((uint32_t) & (irq_vectors));

    /* Clear active and pending IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;

#ifdef CONFIG_KERNEL_NONE
    __enable_excp_irq();
#endif

#ifndef CONFIG_SUPPORT_TSPEND
    irq_vectors_init();
#endif

    tls_pmu_clk_select(1);

    u32 value = 0;

    /*Switch to DBG*/
	value = tls_reg_read32(HR_PMU_BK_REG);
	value &=~(BIT(19));
	tls_reg_write32(HR_PMU_BK_REG, value);
	value = tls_reg_read32(HR_PMU_PS_CR);
	value &= ~(BIT(5));
	tls_reg_write32(HR_PMU_PS_CR, value);	
	
	/*Close those not used clk*/
	tls_reg_write32(HR_CLK_BASE_ADDR,tls_reg_read32(HR_CLK_BASE_ADDR)&
		(~(BIT(6)|BIT(14)|BIT(18)|BIT(19)|BIT(21))));	
	
	tls_sys_clk_set(CPU_CLK_80M);

	drv_irq_register(SYS_TICK_IRQn, CORET_IRQHandler);
    drv_irq_enable(SYS_TICK_IRQn);

#if TLS_CONFIG_CRYSTAL_24M
	tls_wl_hw_using_24m_crystal();
#endif

    /* must call first to configure gpio Alternate functions according the hardware design */
	wm_gpio_config();

#if USE_UART0_PRINT
	uart0Init(115200);
#endif

#if TLS_CONFIG_HARD_CRYPTO
	drv_irq_register(RSA_IRQn, RSA_IRQHandler);
    drv_irq_register(CRYPTION_IRQn, GPSEC_IRQHandler);
    tls_crypto_init();
#endif

#if 0
    unsigned int v0 = *(volatile unsigned int*)0xE000EC10;
    *(volatile unsigned int*)0xE000EC10 = 0xFF;
    unsigned int v1 = *(volatile unsigned int*)0xE000EC10;
    wm_printf("v0=%x, v1=%x, %s\n", v0, v1, (v0 != v1) ? "TSPEND" : "NOTSPEND");
#endif
}

