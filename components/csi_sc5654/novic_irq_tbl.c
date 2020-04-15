/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <csi_config.h>

extern void NOVIC_IRQ_Default_Handler(void);
extern void CORET_IRQHandler(void);
extern void SARADC_IRQHandler(void);
extern void SPIFLASH_IRQHandler(void);
extern void USART_IRQHandler(void);
extern void PDM_IRQHandler(void);
extern void GPIO1_IRQHandler(void);
extern void GPIO2_IRQHandler(void);
extern void TIMER_IRQHandler(void);
extern void SDMAC_IRQHandler(void);
extern void SDMMC_IRQHandler(void);
extern void IIR_IRQHandler(void);
extern void SDIO_IRQHandler(void);
extern void USBFS_IRQHandler(void);
extern void USBHS_IRQHandler(void);
extern void CXC_IRQHandler(void);
extern void ADMAC_IRQHandler(void);
extern void SPDIF_IRQHandler(void);
extern void PCM_IRQHandler(void);
extern void IIS_IRQHandler(void);
extern void RTC_IRQHandler(void);
extern void PMU_IRQHandler(void);
extern void DMAC_IRQHandler(void);

void (*g_irqvector[])(void) = {
    NOVIC_IRQ_Default_Handler,        /*  0:   */
    NOVIC_IRQ_Default_Handler,        /*  1:   */
    SARADC_IRQHandler,                /*  2:   */
    SPIFLASH_IRQHandler,              /*  3:   */
    USART_IRQHandler,                 /*  4:   */
    PDM_IRQHandler,                   /*  5:   */
    GPIO1_IRQHandler,                 /*  6:   */
    GPIO2_IRQHandler,                 /*  7:   */
    TIMER_IRQHandler,                 /*  8:   */
    SDMAC_IRQHandler,                 /*  9:   */
    SDMMC_IRQHandler,                 /*  10:  */
    IIR_IRQHandler,                   /*  11:  */
    SDIO_IRQHandler,                  /*  12:  */
    USBFS_IRQHandler,                 /*  13:  */
    USBHS_IRQHandler,                 /*  14:  */
    CXC_IRQHandler,                   /*  15:  */
    ADMAC_IRQHandler,                 /*  16:  */
    SPDIF_IRQHandler,                 /*  17:  */
    PCM_IRQHandler,                   /*  18:  */
    IIS_IRQHandler,                   /*  19:  */
    RTC_IRQHandler,                   /*  20:  */
    PMU_IRQHandler,                   /*  21:  */
    DMAC_IRQHandler,                  /*  22:  */
    NOVIC_IRQ_Default_Handler,        /*  23:  */
    NOVIC_IRQ_Default_Handler,        /*  24:  */
    NOVIC_IRQ_Default_Handler,        /*  25:  */
    NOVIC_IRQ_Default_Handler,        /*  26:  */
    NOVIC_IRQ_Default_Handler,        /*  27:  */
    NOVIC_IRQ_Default_Handler,        /*  28:  */
    NOVIC_IRQ_Default_Handler,        /*  29:  */
    NOVIC_IRQ_Default_Handler,        /*  30:  */
    CORET_IRQHandler,                 /*  31:  */
};
