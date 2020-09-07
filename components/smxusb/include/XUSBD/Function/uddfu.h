/*
* uddfu.h                                                   Version 2.53
*
* smxUSBD DFU (Device Firmware Upgrade) Function Driver.
*
* Copyright (c) 2011 Micro Digital Inc.
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

#ifndef SUD_DFU_H
#define SUD_DFU_H

#define SUD_DFU_WRITE_BLOCK_DONE       1
#define SUD_DFU_MANIFESTATION_DONE     2

/* DFU Status */
#define SUD_DFU_STATUS_OK                0x00
#define SUD_DFU_STATUS_errTARGET         0x01
#define SUD_DFU_STATUS_errFILE           0x02
#define SUD_DFU_STATUS_errWRITE          0x03
#define SUD_DFU_STATUS_errERASE          0x04
#define SUD_DFU_STATUS_errCHECK_ERASED   0x05
#define SUD_DFU_STATUS_errPROG           0x06
#define SUD_DFU_STATUS_errVERIFY         0x07
#define SUD_DFU_STATUS_errADDRESS        0x08
#define SUD_DFU_STATUS_errNOTDONE        0x09
#define SUD_DFU_STATUS_errFIRMWARE       0x0A
#define SUD_DFU_STATUS_errVENDOR         0x0B
#define SUD_DFU_STATUS_errUSBR           0x0C
#define SUD_DFU_STATUS_errPOR            0x0D
#define SUD_DFU_STATUS_errUNKNOWN        0x0E
#define SUD_DFU_STATUS_errSTALLEDPKT     0x0F

/* File System Interface */
typedef struct {
    int (*DFUInit)(uint segment);
    int (*DFUWrite)(uint segment, u32 dwOffset, u8 *pData, uint iSize);
    int (*DFURead)(uint segment,  u32 dwOffset, u8 *pData, uint iSize);
    int (*DFUDone)(uint segment);
} SUD_DFU_IF;

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* used by smxUSBD internally */
int  sud_DFUInit(void);
void sud_DFURelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_DFUGetOps(void);
void *sud_DFUGetInterface(void);
#endif

int  sud_DFUIsConnected(void);

void sud_DFURegisterInterface(SUD_DFU_IF *pIF);

void sud_DFUWriteDone(uint result, uint condition);

int  sud_DFUIsRuntimeMode(void);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_DFU_H */

