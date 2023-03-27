/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name:platform.h
 * Description:
 *   ....
 */
#ifndef __PLATORM_H__
#define __PLATORM_H__

void PLATFORM_IoInit(void);
void PLATFORM_PowerOff(void);
int PLATFORM_PanelInit(void);
void PLATFORM_PanelBacklightCtl(int level);
void PLATFORM_SpkMute(int value);
void PLATFORM_SpkUnMute(int value);
int PLATFORM_IrCutCtl(int duty);
#endif