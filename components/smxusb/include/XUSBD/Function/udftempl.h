/*
* udftempl.h                                                Version 2.12
*
* smxUSBD Function Driver Template.
*
* Copyright (c) 2006 Micro Digital Inc.
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

#ifndef SUD_FTEMPL_H
#define SUD_FTEMPL_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSBD internally */
int  sud_FTemplInit(void);
void sud_FTemplRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_FTemplGetOps(void);
void *sud_FTemplGetInterface(void);
#endif

/* TODO: Add your API here */
#define SUD_FTEMPL_NOTIFY_INTDATAREADY  1 /* Received INT data */
#define SUD_FTEMPL_NOTIFY_BULKDATAREADY 2 /* Received BULK data */
typedef void (* PFTEMPLFUNC)(int port, int notification);

int  sud_FTemplIsConnected(int port);
/* send INT data */
int  sud_FTemplSendInt(int port, u8 *pData, int iLen);
/* send BULK data */
int  sud_FTemplSendBulk(int port, u8 *pData, int iLen);

/* Get the received INT data */
int  sud_FTemplGetIntData(int port, u8 *pData, int iLen);
/* Get the received BULK data */
int  sud_FTemplGetBulkData(int port, u8 *pData, int iLen);

/* register callback function for received INT and BULK data */
void sud_FTemplRegisterNotify(int port, PFTEMPLFUNC handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_FTEMPL_H */

