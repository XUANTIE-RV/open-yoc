/*
* uport.h                                                   Version 3.00
*
* smxUSBH Porting Layer.
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

#ifndef SU_PORT_H
#define SU_PORT_H

/* USER: Simplify this file by deleting conditionals that don't apply to you. */

/*============================================================================
                            SOFTWARE/OS SETTINGS
============================================================================*/

#define SU_VERSION 0x0300  // version is XX.XX

extern unsigned int su_Version;

/*============================================================================
                            HARDWARE SETTINGS
============================================================================*/
#define SU_HC_TASK_STACK     900
#define SU_HUB_TASK_STACK    (1200 + 400*SU_VIDEO)

/* Set to 1 if you have a big endian CPU and you inverted the connection
   of the host controller to the data bus. (If you are designing your own
   hardware, we recommend you do this for better performance, so there
   is no need to invert data i/o to host controller registers in smxUSB.)
*/
#define SU_BUS_INVERTED 0  /* do not change */

/* Some built-in OHCI/EHCI have the address register endianness inverted,
   so if you are using this kind OHCI/EHCI controller, please set this to 1.
   The default setting is 0 which means the Host Controller address register
   is using the same endianess as the other control registers.
*/
#define SU_ADDR_REG_INVERTED 0

/* Base and IRQ Settings for Non-PCI Systems */

#define SU_SYNOPSYS_USE_HS_PHY                  1
#define SU_SYNOPSYS_BASE                        CSKY_USB_BASE
#define SU_SYNOPSYS_IRQ                         USB_IRQn
//#define SU_SYNOPSYS_BASE                        0x40200000
//#define SU_SYNOPSYS_IRQ                         8
#define SU_SYNOPSYS_PHY_CLK_48                  1
#define SU_SYNOPSYS_PHY_CLK_30_60               0
#define SU_SYNOPSYS_FORCE_SLAVE_MODE            0
#define SU_SYNOPSYS_DEDICATED_RX_FIFO_SIZE      384
#define SU_SYNOPSYS_DEDICATED_HNP_FIFO_SIZE     384
#define SU_SYNOPSYS_DEDICATED_HP_FIFO_SIZE      256

/* Use USB interrupt dispatcher since interrupt shared by host and device.
   Normally shared interrupts are handled by the smxBase interrupt handling
   mechanism, but this setting is used for special cases when there is a
   good reason not to do it that way.
*/
//#if defined(SB_CPU_LPC17XX) || defined(SB_CPU_LPC24XX) || defined(SB_CPU_LPC40XX)
//#define SU_USB_INT_DISPATCHER 1
//#else
#define SU_USB_INT_DISPATCHER 0
//#endif

/*============================================================================
                           GLOBAL MACROS DECLARATION
============================================================================*/

/* Byte Order Macros */
#define SU_INVERT_U16(v16)  (u16)(v16)
#define SU_INVERT_U32(v32)  (u32)(v32)

#define SU_INVERTIO_U16(v16)  (u16)(v16)
#define SU_INVERTIO_U32(v32)  (u32)(v32)

//#if SU_BIG_ENDIAN_HC == SB_CPU_BIG_ENDIAN
#define SU_INVERTHC_U16(v16)  (u16)(v16)
#define SU_INVERTHC_U32(v32)  (u32)(v32)
//#else
//#define SU_INVERTHC_U16(v16)  SU_INVERT_U16(v16)
//#define SU_INVERTHC_U32(v32)  SU_INVERT_U32(v32)
//#endif

//#if SU_ADDR_REG_INVERTED
//#define SU_INVERTHC_ADDR_U32(v32)  SB_INVERT_U32(v32)
//#else
#define SU_INVERTHC_ADDR_U32(v32)  (u32)(v32)
//#endif

#if SU_USB_INT_DISPATCHER
#define su_InstallIntVector(irq, iHCIndex, func, lsr, name) /* dispatcher must be hooked by startup code */
#else
#define su_InstallIntVector(irq, iHCIndex, func, lsr, name) sb_OS_ISR_CFUN_INSTALL(irq, iHCIndex, func, lsr, name)
#endif

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int     su_HdwInit(void);
void    su_HdwRelease(void);

/*************************
MMU Functions
*************************/

void    su_FlushData(void);
void *  su_PhysicalToVirtualAddr(void * pAddr);
void *  su_VirtualToPhysicalAddr(void * pAddr);

/*************************
Host Controller Functions
*************************/

#if SU_EHCI
/* EHCI controller uses memory address */
u32     su_EHCIRead32(u32 addr);
void    su_EHCIWrite32(u32 x, u32 addr);
u32     su_GetEHCIBase(uint iHCIndex);
int     su_GetEHCIInterrupt(uint iHCIndex);
#if defined(SB_CPU_X86)
void    su_PCIReadConfigByte(uint iHCIndex, u8 reg, u8  *val);
void    su_PCIWriteConfigByte(uint iHCIndex, u8 reg, u8  val);
void    su_PCIReadConfigDWord(uint iHCIndex, u8 reg, u32  *val);
void    su_PCIWriteConfigDWord(uint iHCIndex, u8 reg, u32  val);
#endif
#endif

#if SU_OHCI
/* OHCI controller uses memory address */
u32     su_OHCIRead32(u32 addr);
void    su_OHCIWrite32(u32 x, u32 addr);
u32     su_GetOHCIBase(uint iID);
int     su_GetOHCIInterrupt(uint iID);
#endif

#if SU_UHCI
/* UHCI uses I/O address */
u16     su_UHCIRead16(u32 addr);
void    su_UHCIWrite16(u16 val, u32 addr);
void    su_UHCIWrite32(u32 val, u32 addr);
u32     su_GetUHCIBase(uint iHCIndex);
int     su_GetUHCIInterrupt(uint iHCIndex);
void    su_PCIReadConfigWord(uint iHCIndex, u8 reg, u16  *val);
void    su_PCIWriteConfigWord(uint iHCIndex, u8 reg, u16  val);
#endif

#if SU_ISP116X
/*
   USER: You may need to tune the code for the READ/WRITE functions to meet
   the ISP116x register read/write timing requirements.
*/
u32     su_ISP116XRead32(uint iHCIndex, u16 regindex);
u16     su_ISP116XRead16(uint iHCIndex, u16 regindex);
void    su_ISP116XReadBuf(uint iHCIndex, u16 regindex, u16 length, u8 * buffer);
void    su_ISP116XWrite32(uint iHCIndex, u32 value, u16 regindex);
void    su_ISP116XWrite16(uint iHCIndex, u16 value, u16 regindex);
void    su_ISP116XWriteBuf(uint iHCIndex, u16 regindex, u16 length, u8 * buffer);
u16     su_GetISP116XIntSetting(uint iHCIndex);  /* USER: modify implementation for your system */
u32     su_GetISP116XBase(uint iHCIndex);
int     su_GetISP116XInterrupt(uint iHCIndex);
#endif

#if SU_ISP1362
/*
   USER: You may need to tune the code for the READ/WRITE functions to meet
   the ISP1362 register read/write timing requirements.
*/
u32     su_ISP1362Read32(uint iHCIndex, u8 regindex);
u16     su_ISP1362Read16(uint iHCIndex, u8 regindex);
void    su_ISP1362ReadBuf(uint iHCIndex, u8 regindex, u16 length, u8 * buffer);
void    su_ISP1362Write32(uint iHCIndex, u32 value, u8 regindex);
void    su_ISP1362Write16(uint iHCIndex, u16 value, u8 regindex);
void    su_ISP1362WriteBuf(uint iHCIndex, u8 regindex, u16 length, u8 * buffer);
u16     su_GetISP1362IntSetting(uint iHCIndex);  /* USER: modify implementation for your system */
u32     su_GetISP1362Base(uint iHCIndex);
int     su_GetISP1362Interrupt(uint iHCIndex);
#endif

#if SU_ISP176X
/*
   USER: You may need to tune the code for the READ/WRITE functions to meet
   the ISP1760/1 register read/write timing requirements.
*/
u16     su_ISP176XRead16(uint iHCIndex, u16 regindex);
u32     su_ISP176XRead32(uint iHCIndex, u16 regindex);
void    su_ISP176XReadBuf(uint iHCIndex, u32 start, u8 * buffer, u16 len);
void    su_ISP176XWrite16(uint iHCIndex, u16 value, u16 regindex);
void    su_ISP176XWrite32(uint iHCIndex, u32 value, u16 regindex);
void    su_ISP176XWriteBuf(uint iHCIndex, u32 start, u8 * buffer, u16 len);
u16     su_GetISP176XIntSetting(uint iHCIndex);  /* USER: modify implementation for your system */
u32     su_GetISP176XBase(uint iHCIndex);
int     su_GetISP176XInterrupt(uint iHCIndex);
#endif

#if SU_LM3S
u32     su_GetLM3SBase(uint iHCIndex);
int     su_GetLM3SInterrupt(uint iHCIndex);
#endif

#if SU_MAX3421
#if defined(SB_BRD_KEIL_MCB2130_LPC2138)
#include "lpc21xx.h"
#endif
u8      su_MAX3421Read(u8 regindex);
void    su_MAX3421ReadBuf(u8 regindex, u8 * buffer, u16 len);
void    su_MAX3421Write(u8 regindex, u8 value);
void    su_MAX3421WriteBuf(u8 regindex, u8 * buffer, u16 len);
void    su_MAX3421VBusOn(uint iOn);
#endif

#if SU_SYNOPSYS
extern u32  su_SynopsysBase[SU_SYNOPSYS];
extern int  su_SynopsysIRQ[SU_SYNOPSYS];
extern uint su_SynopsysForceSlaveMode[SU_SYNOPSYS];
extern uint su_SynopsysDedicatedRXFIFOSize[SU_SYNOPSYS];
extern uint su_SynopsysDedicatedHNPFIFOSize[SU_SYNOPSYS];
extern uint su_SynopsysDedicatedHPFIFOSize[SU_SYNOPSYS];
extern uint su_SynopsysUseHSPhy[SU_SYNOPSYS];
extern uint su_SynopsysPhyClk48[SU_SYNOPSYS];
extern uint su_SynopsysPhyClk3060[SU_SYNOPSYS];
u32     su_SynopsysRead(uint iHCIndex, u32 regindex);
void    su_SynopsysWrite(uint iHCIndex, u32 value, u32 regindex);
#endif

#if SU_BLACKFIN
void    su_BlackfinVBusOn(uint iOn);
#endif

#if SU_RENESAS
u32     su_GetRenesasBase(void);
int     su_GetRenesasInterrupt(void);
u16     su_RenesasRead16(uint regindex);
void    su_RenesasWrite16(u16 value, uint regindex);
u32     su_RenesasRead32(uint regindex);
void    su_RenesasWrite32(u32 value, uint regindex);
#if defined(SB_CPU_RZA1H)
void    su_RenesasCh0Write16(u16 value, uint regindex);
u16     su_RenesasCh0Read16(uint regindex);
#endif
#endif

/*************************
Debug Functions
*************************/

#if SU_DEBUG_LEVEL
void    su_DumpHexL(const char * pInfo, u32 value, int level);
void    su_DebugL(const char * pszDbgInfo, int level);
#else
#define su_DumpHexL(pInfo, value, level);
#define su_DebugL(pszDbgInfo, level);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_PORT_H */

