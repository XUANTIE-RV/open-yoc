/*
* udcompos.h                                                Version 2.12
*
* smxUSBD Composite Device Driver Framework.
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

#ifndef SUD_COMPOSITE_H
#define SUD_COMPOSITE_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSBD internally */
int  sud_CompositeInit(void);
void sud_CompositeRelease(void);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_COMPOSITE_H */

