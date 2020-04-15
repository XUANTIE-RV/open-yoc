/*
* funicode.h                                                Version 1.40
*
* smxFS Unicode Functions.
*
* Copyright (c) 2006-2007 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Shuduo Sang
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SFS_FUNICODE_H
#define SFS_FUNICODE_H

#ifdef __cplusplus
extern "C" {
#endif 

uint Unicode2String(u8 *string, u16 unicode);
uint String2Unicode(u8 *string, u16 *unicode);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FUNICODE_H */
