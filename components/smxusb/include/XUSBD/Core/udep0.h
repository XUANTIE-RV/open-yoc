/*
* udep0.h                                                   Version 2.57
*
* smxUSBD Default Control Pipe.
*
* Copyright (c) 2005-2016 Micro Digital Inc.
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

#ifndef SUD_EP0_H
#define SUD_EP0_H

#ifdef __cplusplus
extern "C" {
#endif

int  sud_EP0ProcessSetup(SUD_REQUESTINFO_T *pRequestInfo);
#if SUD_CF5272 || SUD_CF5275 || SUD_CF548X
int  sud_BuildWholeDescriptor(SUD_DEVICE_HANDLE_T *pDevice, SUD_REQUESTINFO_T *pRequestInfo);
#endif
int  sud_BuildSingleDescriptor(SUD_DEVICE_HANDLE_T *pDevice, SUD_REQUESTINFO_T *pRequestInfo);

#ifdef __cplusplus
}
#endif

#endif /* SUD_EP0_H */

