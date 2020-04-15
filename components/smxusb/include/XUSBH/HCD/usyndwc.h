/*
* usyndwc.h                                                 Version 2.55
*
* smxUSBH Synopsys DWC OTG Host Controller Driver.
*
* Copyright (c) 2008-2018 Micro Digital Inc.
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

#ifndef SU_SYNOPSYS_H
#define SU_SYNOPSYS_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_SynopsysInit(void);
void su_SynopsysRelease(void);
int  su_SynopsysHostInterruptHandler(uint iHCIndex);
#if !SB_MULTITASKING
void su_SynopsysCheckRoothubStatus(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_SYNOPSYS_H */

