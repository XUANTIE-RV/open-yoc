/*
* uinit.h                                                   Version 3.00
*
* smxUSBH Initialization.
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

#ifndef SU_INIT_H
#define SU_INIT_H

/*============================================================================
                          STRUCTURES AND OTHER TYPEDEFS
============================================================================*/

/* Device insert/remove event callback function used by class driver */
typedef void (*SU_PDEVEVTCBFUNC)(uint iID, BOOLEAN bInserted);

/* USBH stack callback function */
typedef void (*SU_PSTACKEVTCBFUNC)(uint iFlag, u32 data);


/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_CB_DEVICE_INSERTED      0x0001
#define SU_CB_DEVICE_REMOVED       0x0002
#define SU_CB_OVERCURRENT          0x0004
#define SU_CB_HUB_INSERTED         0x0100
#define SU_CB_HUB_REMOVED          0x0200
#define SU_CB_UNKNOWN_DEV_REMOVED  0x0400
#define SU_CB_HUB_UNSUPPORTED      0x0800
#define SU_CB_DEVICE_UNRECOGNIZED  0x1000
#define SU_CB_DEVICE_UNSUPPORTED   0x2000
#define SU_CB_HC_FATAL_ERROR       0x4000
#define SU_CB_STACK_FATAL_ERROR    0x8000

#define SU_STACK_FATAL_ERROR_HEAP_BLOCK_OVERFLOW  0x1
#define SU_STACK_FATAL_ERROR_HEAP_INVALID_BLOCK   0x2


/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_USBInited(void);
int  su_Initialize(void);
void su_Release(void);
void su_RegStackCallback(SU_PSTACKEVTCBFUNC pCallback);
int  su_SetHubPortPower(uint iHCIndex, uint iHubAddr, uint port, BOOLEAN bEnable);
int  su_ResetHubPort(uint iHCIndex, uint iHubAddr, uint port, BOOLEAN bReset);
int  su_SetHubPortTest(uint iHCIndex, uint iHubAddr, uint port, uint test);


/* Function versions of ISRs. These assume the caller does the enter/exit
   ISR code. The ISR that calls one of these should do the exit isr right
   after the call, preferrably. These are defined here rather than in another 
   header file for those who purchase the USB disk driver in binary form.
*/
#if SU_EHCI
int su_EHCIHostInterruptHandler(uint iHCIndex);
int su_EHCIGetHCNum(void);
#endif
#if SU_OHCI
int su_OHCIHostInterruptHandler(uint iHCIndex);
int su_OHCIGetHCNum(void);
#endif
#if SU_UHCI
int su_UHCIHostInterruptHandler(uint iHCIndex);
int su_UHCIGetHCNum(void);
#endif
#if SU_ISP116X
int su_ISP116XHostInterruptHandler(uint iHCIndex);
#endif
#if SU_ISP1362
int su_ISP1362HostInterruptHandler(uint iHCIndex);
#endif
#if SU_ISP176X
int su_ISP176XHostInterruptHandler(uint iHCIndex);
#endif

#if !SB_MULTITASKING
/* Call this function periodically to check the hub status, if
   running in a non-multitasking environment such as DOS.
*/
void su_CheckRoothubStatus(void);
#endif

#if SU_OTG
int  su_EnableHNP();
#endif

#if defined(SMXAWARE)
int  su_GetDrvInfoSize(void);
int  su_GetDrvInfoConfDescOffset(void);
int  su_GetDrvInfoChildDevOffset(void);
int  su_GetDrvInfoDescOffset(void);
int  su_GetRootHubDeviceOffset(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_INIT_H */

