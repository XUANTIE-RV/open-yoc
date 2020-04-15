/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <drv/common.h>
#include <csi_config.h>
#include <soc.h>
#include <silan_pic.h>
#ifndef CONFIG_KERNEL_NONE
#include <csi_kernel.h>
#endif
#include <string.h>

extern void systick_handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);
extern void silan_usart_irqhandler(int32_t idx);
extern void silan_gpio_irqhandler(int32_t idx);
extern void silan_rtc_irqhandler(int32_t idx);

#if defined(CONFIG_SUPPORT_TSPEND) || defined(CONFIG_KERNEL_NONE)
#define  ATTRIBUTE_ISR __attribute__((isr))
#else
#define  ATTRIBUTE_ISR
#endif

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#ifndef CONFIG_KERNEL_NONE
#define  CSI_INTRPT_ENTER() csi_kernel_intrpt_enter()
#define  CSI_INTRPT_EXIT()  csi_kernel_intrpt_exit()
#else
#define  CSI_INTRPT_ENTER()
#define  CSI_INTRPT_EXIT()
#endif

IRQ_HANDLER irq[32];

int pic_port_request(int id, hdl_t hdl)
{
    IRQ_HANDLER *irq_p;
    int err = 0;

    irq_p = &irq[id];
    memset((void *)irq_p, 0, sizeof(IRQ_HANDLER));
    irq_p->irqid    = (uint32_t)id;
    irq_p->handler  = hdl;

    csi_vic_clear_pending_irq(irq_p->irqid);
    csi_vic_set_prio(irq_p->irqid, irq_p->priority);
    csi_vic_enable_irq(irq_p->irqid);
    return err;
}

void pic_port_free(int id)
{
    IRQ_HANDLER *irq_p;

    irq_p = &irq[id];
    csi_vic_clear_pending_irq(irq_p->irqid);
    csi_vic_disable_irq(irq_p->irqid);

    memset((void *)irq_p, 0, sizeof(IRQ_HANDLER));
}

int pic_port_init()
{
    memset((void *)irq, 0, sizeof(irq));
    return 0;
}

static inline void do_handler(IRQ_HANDLER *irq_p)
{
    csi_vic_clear_pending_irq(irq_p->irqid);

    if (irq_p->handler != NULL) {
        irq_p->handler(irq_p->irqid);
    }
}

ATTRIBUTE_ISR void CORET_IRQHandler(void)
{
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif
}

ATTRIBUTE_ISR void SARADC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SAR];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SPIFLASH_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SPIFLASH];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USART_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_MISC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}
ATTRIBUTE_ISR void PDM_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_PDM];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO1_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_GPIO1];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO2_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_GPIO2];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIMER_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_TIMER];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SDMAC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SDMAC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SDMMC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SDMMC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SDIO_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SDIO];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void IIR_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_IIR];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USBFS_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_USBFS];

    CSI_INTRPT_ENTER();
#ifdef _SUPPORT_STM_USBH_DRIVER_
    USBH_OTG_ISR_IRQHandler(&USB_OTG_Core);
#else
    do_handler(irq_p);
#endif
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USBHS_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_USBHS];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void CXC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_CXC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void ADMAC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_ADMAC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SPDIF_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_SPDIF];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PCM_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_PCM];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void IIS_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_IIR];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RTC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    silan_rtc_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PMU_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_PMU];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMAC_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_DMAC];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void WDT_IRQHandler(void)
{
    IRQ_HANDLER *irq_p = &irq[PIC_IRQID_WDOG];

    CSI_INTRPT_ENTER();
    do_handler(irq_p);
    CSI_INTRPT_EXIT();
}
