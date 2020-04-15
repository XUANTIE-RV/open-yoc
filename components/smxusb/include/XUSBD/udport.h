/*
* udport.h                                                  Version 2.57
*
* smxUSBD Porting Layer.
*
* Copyright (c) 2005-2018 Micro Digital Inc.
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

#ifndef SUD_PORT_H
#define SUD_PORT_H

/*============================================================================
                            SOFTWARE/OS SETTINGS
============================================================================*/

#define SUD_VERSION 0x0257  /* version is XX.XX */

extern unsigned int sud_Version;

/* USER: Simplify this file by deleting conditionals that don't apply to you. */

/*============================================================================
                            HARDWARE SETTINGS
============================================================================*/

#define SUD_DC_TASK_STACK     (SUD_MTP*1000 + 900)

/* Set to 1 if you have a big endian CPU and you inverted the connection
   of the device controller to the data bus. (If you are designing your own
   hardware, we recommend you do this for better performance, so there
   is no need to invert data i/o to device controller registers in smxUSBD.)
*/
#if SB_CPU_BIG_ENDIAN
#define SUD_BUS_INVERTED 1
#else
#define SUD_BUS_INVERTED 0  /* do not change */
#endif

/* Byte Order Macros */

#if SB_CPU_BIG_ENDIAN
#define SUD_INVERT_U16(v16)    SB_INVERT_U16(v16)
#define SUD_INVERT_U32(v32)    SB_INVERT_U32(v32)
#else
#define SUD_INVERT_U16(v16)    (u16)(v16)
#define SUD_INVERT_U32(v32)    (u32)(v32)
#endif

#if SB_CPU_BIG_ENDIAN && SUD_BUS_INVERTED
#define SUD_INVERTIO_U16(v16)  (u16)(v16)
#define SUD_INVERTIO_U32(v32)  (u32)(v32)
#else
#define SUD_INVERTIO_U16(v16)  SUD_INVERT_U16(v16)
#define SUD_INVERTIO_U32(v32)  SUD_INVERT_U32(v32)
#endif

#define SUD_SYNOPSYS_BASE           CSKY_USB_BASE
#define SUD_SYNOPSYS_IRQ            USB_IRQn
//#define SUD_SYNOPSYS_BASE          0x40200000
//#define SUD_SYNOPSYS_IRQ           8
#define SUD_HIGH_SPEED             1
#define SUD_EP0_PACKETSIZE         0x40
#define SUD_SYNOPSYS_USE_ULPI      0
#define SUD_SYNOPSYS_USE_SLAVEMODE 0
#define SUD_MAX_ENDPOINTS          16

/*============================================================================
                           GLOBAL MACROS DECLARATION
============================================================================*/


#if SUD_USB_INT_DISPATCHER
#define sud_InstallIntVector(irq, param, func, lsr, name) /* dispatcher must be hooked by startup code */
#else
#define sud_InstallIntVector(irq, param, func, lsr, name) sb_OS_ISR_CFUN_INSTALL(irq, param, func, lsr, name)
#endif

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

int     sud_HdwInit(void);
void    sud_HdwRelease(void);
char   *sud_GetSerialNum(void);

/*************************
Debug Functions
*************************/

#if SUD_DEBUG_LEVEL
void    sud_DumpHexL(const char *pInfo, u32 value, int level);
void    sud_DebugL(const char *pszDbgInfo, int level);
#else
#define sud_DumpHexL(pInfo, value, level);
#define sud_DebugL(pszDbgInfo, level);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SUD_PORT_H */

