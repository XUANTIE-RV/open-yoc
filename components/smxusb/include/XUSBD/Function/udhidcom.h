/*
* udhidcom.h                                                Version 2.55
*
* smxUSBD HID Communication Function Driver.
*
* Copyright (c) 2012-2013 Micro Digital Inc.
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

#ifndef SUD_HIDCOM_H
#define SUD_HIDCOM_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSB internally */
int  sud_HIDInit(void);
void sud_HIDRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_HIDGetOps(void);
void *sud_HIDGetInterface(void);
#endif

typedef void (* PHIDFUNC)(uint iReportID, void *pDataBuf, uint size);

/* used by high level application */
int  sud_HIDIsConnected(void);
int  sud_HIDSendInput(uint iReportID, void *pDataBuf, uint size);
void sud_HIDRegisterOutputNotify(PHIDFUNC handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_HIDCOM_H */

