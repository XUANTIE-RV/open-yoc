/*
* umscmd.h                                                  Version 3.00
*
* smxUSBH Mass Storage Class Driver Build Request for QIC, UFI and ATAPI protocol.
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

#ifndef SU_MS_CMD_H
#define SU_MS_CMD_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

void su_MSSendATAPIReq(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
void su_MSSendQICReq(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
void su_MSSendUFIReq(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);

#if defined(__cplusplus)
}
#endif

#endif /* SU_MS_CMD_H */
