/*
* uddcd.h                                                   Version 2.53
*
* smxUSBD Device Controller Driver.
*
* Copyright (c) 2005-2011 Micro Digital Inc.
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

#ifndef SUD_DCINTERFACE_H
#define SUD_DCINTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

int sud_DCDSendRequestInfo(SUD_REQUESTINFO_T *pRequestInfo);
int sud_DCDCancelRequestInfo(SUD_REQUESTINFO_T *pRequestInfo);
int sud_DCDIsEndpointHalted(SUD_DEVICE_HANDLE_T *pDevice, uint index);
int sud_DCDSetDeviceFeature(SUD_DEVICE_HANDLE_T *pDevice, u16 wFeatureSelector, u16 wIndex, uint flag);
int sud_DCDDoDeviceEvent(SUD_DEVICE_HANDLE_T *pDevice, uint event);

int sud_DCRegister(SUD_DC_OPERATION_T *pDCOperation, const char *pDCName);
void sud_DCDeregister(void);

SUD_DEVICE_HANDLE_T *sud_DCGetDevice(void);

#ifdef __cplusplus
}
#endif

#endif /* SUD_DCINTERFACE_H */

