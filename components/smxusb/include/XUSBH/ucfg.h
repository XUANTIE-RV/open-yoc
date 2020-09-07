/*
* ucfg.h                                                    Version 3.00
*
* smxUSBH Configuration Settings.
*
* Copyright (c) 2004-2018 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_CFG_H
#define SU_CFG_H

/* Host Controller Selection and Maximum Number. */
/*
   Multiple host controllers are supported for the first group.
   Set each to the number of host controllers of that type.
*/
#define SU_EHCI         0
#define SU_OHCI         0
#define SU_UHCI         0
#define SU_ISP116X      0
#define SU_ISP1362      0
#define SU_ISP176X      0
#define SU_LM3S         0        /* LM3S, LM4S, TM4C */
#define SU_SYNOPSYS     1        /* 0, 1, or 2 */

/* 0 or 1 only: */
#define SU_AT91OTG      0
#define SU_BLACKFIN     0
#define SU_BLACKFIN7    0
#define SU_CF522XX      0        /* non-EHCI 522xx (e.g. 52277 is EHCI) */
#define SU_MAX3421      0
#define SU_PD720150     0
#define SU_RENESAS      0
#define SU_HCXXXXX      0        /* Host Controller Driver Template */

/* Optional Class Driver Selection and Maximum Number. 0 to disable. */

#define SU_AUDIO        0
#define SU_CCID         0
#define SU_CDCACM       0
#define SU_FTDI232      0        /* Not a USB standard class driver */
#define SU_HID          0
#define SU_HUB          1
#define SU_K4510        0        /* Not a USB standard class driver */
#define SU_KBD          0
#define SU_MOUSE        0
#define SU_MSTOR        1
#define SU_MT7601       0        /* Not a USB standard class driver */
#define SU_NET          0        /* Not a USB standard class driver */
#define SU_NOVATEL      0        /* Not a USB standard class driver */
#define SU_OBID         0        /* Not a USB standard class driver */
#define SU_PL2303       0        /* Not a USB standard class driver */
#define SU_PRINTER      0
#define SU_RT250X       0        /* Not a USB standard class driver */
#define SU_RT2870       0        /* Not a USB standard class driver */
#define SU_RT3070       0        /* Not a USB standard class driver */
#define SU_RT3572       0        /* Not a USB standard class driver */
#define SU_RT5370       0        /* Not a USB standard class driver */
#define SU_RT5572       0        /* Not a USB standard class driver */
#define SU_RTL81XX      0        /* Not a USB standard class driver */
#define SU_SERIAL       0        /* Not a USB standard class driver */
#define SU_SIERRA       0        /* Not a USB standard class driver */
#define SU_UF8000       0        /* Not a USB standard class driver */
#define SU_V20K13       0        /* Not a USB standard class driver */
#define SU_VIDEO        0
#define SU_WCESERIAL    0        /* Not a USB standard class driver */
#define SU_CTEMPL       0        /* Class driver template */
#define SU_TUSBH        0        /* Regression Test program for smxUSBH */


/* USB Host Stack OTG support */
/* Set to 1 to enable OTG support of the USB host stack */
#define SU_OTG              0

/* Some devices may need to reset the hub port twice before they can be
   enumerated. You don't need to enable this feature in most cases.
   Contact MDI if you have any question about this.
*/
#define SU_HUB_RESET_TWICE  0

/* Set to 1 to always increase the device address. Set to 0 to re-use any 
   smaller available address first. */
#define SU_DEV_ADDR_INCREASE  0

/* Number of times to retry for the enumeration GetDescInfo request. */
#define SU_ENUMERATION_RETRY  3


#if SU_EHCI && (defined(SB_CPU_MCF532x7x) || defined(SB_CPU_MCF5251) \
                 || defined(SB_CPU_MCF5227x) \
                 || defined(SB_CPU_MCF5441x) || defined(SB_CPU_MCF5445x) \
                 || defined(SB_CPU_K60F120M) || defined(SB_CPU_K65) || defined(SB_CPU_K70) \
                 || defined(SB_CPU_IMX31)    || defined(SB_CPU_IMX6) \
                 || defined(SB_CPU_LPC18XX)  || defined(SB_CPU_LPC43XX) \
                 || defined(SB_CPU_LPC313X)  || defined(SB_CPU_LPC314X) || defined(SB_CPU_LPC315X)) \
                 || defined(SB_CPU_VFXX)
#define SU_FREESCALE_EHCI 1
#else
#define SU_FREESCALE_EHCI 0
#endif

#define SU_EHCI_PERIODIC_SIZE 256 /* increase it to 512 or 1024 if you have a lot of interrupt/isoc devices */

/* External Hub is not supported for the following host controller */
#if SU_CF522XX || SU_BLACKFIN
#undef SU_HUB
#define SU_HUB 0
#endif

/* Number of Host Controllers */
#define SU_HC_NUM       (SU_EHCI + SU_OHCI + SU_UHCI + SU_ISP116X + SU_ISP1362 \
                          + SU_ISP176X + SU_CF522XX + SU_MAX3421 + SU_LM3S \
                          + SU_SYNOPSYS + SU_BLACKFIN + SU_BLACKFIN7 + SU_RENESAS + SU_PD720150 \
                          + SU_AT91OTG + SU_HCXXXXX)

/* Debug Level */
/*   0  disables all debug output and debug statements are null macros */
/*   1  only output fatal error information */
/*   2  output additional warning information */
/*   3  output additional status information */
/*   4  output additional device change information */
/*   5  output additional data transfer information */
/*   6  output interrupt information */

//#define SU_DEBUG_LEVEL 0
#define SU_DEBUG_LEVEL 7

/* Set to 1 to output mass storage debug information. */
#define SU_MS_DEBUG 1

/* Run-Time Safety Checks */
/* Set to 1 enable extra code to do safety checks; 0 to disable. */
#define SU_SAFETY_CHECKS 1

/* Use pipe to send data from ISR to controller task.
   Set to 1 when you need to support Symmetric Multiprocessing (SMP) for
   EHCI/OHCI/UHCI.
*/
#define SU_USE_PIPE 0

/* Flag to copy data if OHCI or UHCI need a special memory space. */
/* Note: If you set to 1, please remember to increase the heap size above. */
#if 0
#if defined(SB_BRD_MOTOROLA_5475EVB) || defined(SB_BRD_MOTOROLA_5485EVB) \
 || defined(SB_CPU_MCF5251) || defined(SB_CPU_MCF5441x) || defined(SB_CPU_MCF5445x) \
 || (defined(SB_CPU_LPC178X_7X) && SU_OHCI) || (defined(SB_CPU_LPC408X_7X) && SU_OHCI) || defined(SB_CPU_LPC2388) \
 || defined(SB_CPU_LH7A404) \
 || defined(SB_CPU_VFXX)
#define SU_COPY_DATA 1
#else
#define SU_COPY_DATA 0
#endif
#endif
#define SU_COPY_DATA 0

/* Some host controllers have a limitation for the transferred data buffer
   for mass storage devices so we must split those requests into smaller
   ones. */
#if 0
#if ((SU_CF522XX && !defined(SB_CPU_MCF5225x)) \
 || defined(SB_CPU_MCF5251) || defined(SB_CPU_MCF5441x) || defined(SB_CPU_MCF5445x) \
 || (defined(SB_CPU_LPC178X_7X) && SU_OHCI) || (defined(SB_CPU_LPC408X_7X) && SU_OHCI) || defined(SB_CPU_LPC2388) \
 || defined(SB_CPU_VFXX) \
 || defined(SB_CPU_LH7A404) || SU_UHCI) && SU_MSTOR
#define SU_SPLIT_TRANSFER 1
#else
#define SU_SPLIT_TRANSFER 0
#endif
#endif
#define SU_SPLIT_TRANSFER 1

#if SU_SPLIT_TRANSFER
/* If we split the mass storage request, this specifies the maximum data
   size (KB) per transfer. */
//#if SU_FREESCALE_EHCI
//#if defined(SB_CPU_MCF5251) || defined(SB_CPU_MCF5445x)
//#define SU_SPLIT_TRANSFER_SIZE 6
//#elif defined(SB_CPU_MCF5441x) || defined(SB_CPU_VFXX)
//#define SU_SPLIT_TRANSFER_SIZE 8
//#endif
//#elif SU_CF522XX
//#define SU_SPLIT_TRANSFER_SIZE 1
//#elif defined(SB_CPU_LPC178X_7X) || defined(SB_CPU_LPC408X_7X) || defined(SB_CPU_LPC2388)
//#define SU_SPLIT_TRANSFER_SIZE 6
//#else
//#define SU_SPLIT_TRANSFER_SIZE 8
//#endif
//#else
#define SU_SPLIT_TRANSFER_SIZE 6
#endif

#if SU_OHCI || SU_UHCI || SU_EHCI
#define SU_NEED_NC_MEMORY     1
#else
#define SU_NEED_NC_MEMORY     0
#endif

#if SU_FREESCALE_EHCI
/* Non-cacheable memory is too small so we must use C heap to allocate other data structures. */
#define SU_USE_C_HEAP         1
#else
/* Set to 1 to use malloc() of C lib instead of internal heap function in uheap.c */
#define SU_USE_C_HEAP         0
#endif

/* smxUSB Heap Size, KB (not used for x86 real mode) */
/* Do not set lower than the default value. */

/* Core driver used heap size, mostly for data structure */
#if SU_USE_C_HEAP
#define SU_USBD_HEAP          0
#else
#define SU_USBD_HEAP          2
#endif

/* Class driver used heap size, mostly for data buffer */
#define SU_VIDEO_DRV_HEAP     (SU_VIDEO*80 + SU_V20K13*25)

#define SU_CDC_DRV_HEAP       (SU_CDCACM*4 + SU_FTDI232*2 + SU_NET*7 + SU_OBID + SU_PL2303*2 + SU_SERIAL + SU_SIERRA*2 + SU_WCESERIAL + SU_K4510*8 + SU_NOVATEL*4)

#define SU_HID_DRV_HEAP       (SU_HID*4 + SU_MOUSE + SU_KBD)

#if SU_RT5572 || SU_RT5370 || SU_RT3572 || SU_RT3070 || SU_RT2870 || SU_RTL81XX || SU_MT7601
#define SU_WIFI_DRV_HEAP      12
#elif SU_RT250X
#define SU_WIFI_DRV_HEAP      8
#else
#define SU_WIFI_DRV_HEAP      0
#endif

#define SU_CLASS_DRV_HEAP    (SU_AUDIO*10 + SU_CCID + SU_HUB*2 + SU_MSTOR*3 + \
                              SU_PRINTER*2 + SU_UF8000 + SU_CDC_DRV_HEAP + \
                              SU_HID_DRV_HEAP + SU_WIFI_DRV_HEAP + \
                              SU_VIDEO_DRV_HEAP + SU_CTEMPL*2 + SU_TUSBH*2)

/* Host controller driver used heap size, mostly for Transfer Descriptor */
#if SU_FREESCALE_EHCI
#define SU_HC_DRV_HEAP       (SU_EHCI*6)
#else
#define SU_HC_DRV_HEAP       (SU_EHCI*6 + SU_OHCI*3 + SU_UHCI*70 + SU_ISP116X*4 + SU_ISP1362*2 \
                              + SU_ISP176X*2 + SU_CF522XX + SU_MAX3421 + SU_LM3S + SU_SYNOPSYS*6 \
                              + SU_BLACKFIN + SU_BLACKFIN7 + SU_RENESAS + SU_PD720150 + SU_AT91OTG + SU_HCXXXXX)
#endif

#define SU_HEAPSIZE (SU_USBD_HEAP + SU_CLASS_DRV_HEAP + SU_HC_DRV_HEAP + SU_SPLIT_TRANSFER_SIZE)*1024

#if !SB_PACKED_STRUCT_SUPPORT
#error SB_PACKED_STRUCT_SUPPORT is required for smxUSBH. Unpacked support has been partially removed.
#endif

#endif /* SU_CFG_H */

