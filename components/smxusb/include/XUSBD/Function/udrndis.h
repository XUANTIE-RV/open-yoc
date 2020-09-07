/*
* udrndis.h                                                 Version 2.40
*
* smxUSBD Remote NDIS (IP over USB) Function Driver.
*
* Copyright (c) 2006-2010 Micro Digital Inc.
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

#ifndef SUD_RNDIS_H
#define SUD_RNDIS_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

typedef int (* PRNDISFUNC)(int port, u8 *pBuf, uint len);

/* used by smxUSB internally */
int  sud_RNDISInit(void);
void sud_RNDISRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_RNDISGetOps(void);
void *sud_RNDISGetInterface(void);
#endif

/* used by high level application */
int  sud_RNDISIsPortConnected(int port);
int  sud_RNDISWriteData(int port, u8 *pBuf, uint len);
void sud_RNDISRegisterPortNotify(int port, PRNDISFUNC handler);
void sud_RNDISSetEthernetAddr(int port, u8 *pMACAddr);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_RNDIS_H */

