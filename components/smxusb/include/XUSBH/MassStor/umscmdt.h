/*
* umscmdt.h                                                 Version 3.00
*
* smxUSBH Mass Storage Class Driver Send Request for QIC, UFI and ATAPI protocol.
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

#ifndef SU_MS_CMDT_H
#define SU_MS_CMDT_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

int  su_MSSendCmdByContBulkInt(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
int  su_MSSendCmdByContBulk(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
void su_MSSendCmdIntReady(SU_REQ_INFO *);

#if defined(__cplusplus)
}
#endif

#endif /* SU_MS_CMDT_H */
