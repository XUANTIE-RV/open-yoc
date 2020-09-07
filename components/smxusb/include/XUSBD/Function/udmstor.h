/*
* udmstor.h                                                 Version 2.50
*
* smxUSBD Mass Storage Function Driver.
*
* Copyright (c) 2005-2010 Micro Digital Inc.
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

#ifndef SUD_MSTORAGE_H
#define SUD_MSTORAGE_H

#include "bbd.h" /* Block Device Driver Interface */

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSB internally */
int  sud_MSInit(void);
void sud_MSRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_MSGetOps(void);
void *sud_MSGetInterface(void);
#endif

/* used by high level application */
void sud_MSRegisterDisk(const SB_BD_IF *pDiskOper, int iLUN);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_MSTORAGE_H */

