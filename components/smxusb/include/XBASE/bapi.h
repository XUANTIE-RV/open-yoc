/*
* bapi.h                                                    Version 4.4.0
*
* smxBase API.
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
* Authors: David Moore, Yingbo Hu, and Ralph Moore
*
*****************************************************************************/

#ifndef SB_BAPI_H
#define SB_BAPI_H

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(SB_OS_SMX3)
/* Base Block Pool Functions */
BOOLEAN  sb_BlockPoolCreate(u8 *p, PCB_PTR pool, u16 num, u16 size,
                            const char *name);
BOOLEAN  sb_BlockPoolCreateDAR(SB_DCB *dar, PCB_PTR pool, u16 num, u16 size,
                               u16 align, const char *name);
u8      *sb_BlockPoolDelete(PCB_PTR pool);
u32      sb_BlockPoolPeek(PCB_PTR pool, SB_PK_PARM par);
u8      *sb_BlockGet(PCB_PTR pool, u16 clrsz);
BOOLEAN  sb_BlockRel(PCB_PTR pool, u8 *bp, u16 clrsz);
#endif

/* DAR Functions */
u8      *sb_DARAlloc(SB_DCB_PTR dar, u32 sz, u32 align);
BOOLEAN  sb_DARFreeLast(SB_DCB_PTR dar);
BOOLEAN  sb_DARInit(SB_DCB_PTR dar, u8 *pi, u32 sz, BOOLEAN fill, u32 fillval);

/* Error Manager Functions */
BOOLEAN  sb_EMInit(void);
BOOLEAN  sb_EMClear(void);
void     sb_EM(SB_ERRNO errnum);

/* Message Display Functions */
#define  sb_MsgDisplay()
#define  sb_MsgOutConst(mtype, mp)
#define  sb_MsgOutVar(mtype, mp)

#define  sb_MsgConstDisplay(mtype, mp) \
    { \
        sb_MsgOutConst(mtype, mp); \
        sb_MsgDisplay(); \
    }

#define  sb_MsgVarDisplay(mtype, mp) \
    { \
        sb_MsgOutVar(mtype, mp); \
        sb_MsgDisplay(); \
    }

/* MPU Functions */
void     sb_MPUInit(void);
void     sb_MPULoad(void);

/* Power Management Functions */
u32      sb_PowerDown(u32 power_mode);

/* System Stack Functions */
void     sb_SSFill(void);
void     sb_SSFillMost(void);
void     sb_SSFillResv(u32 resv);
void     sb_SSPInit(void);
u32      sb_SSScan(void);

/* Time Measurement Functions */
void     sb_TMInit(void);
void     sb_TMStart(u32 *pts);
void     sb_TMEnd(u32 ts, u32 *ptm);

/* Time Functions */
void     sb_GetLocalTime(DATETIME *pDateTime);

/* Utility Functions */
u32      sb_read32_unaligned(u8 *addr);
void     sb_write32_unaligned(u8 *addr, u32 val);

#ifdef __cplusplus
}
#endif


/* Macros */

#define  sb_ERROR_RET(errnum, x)\
    { \
        sb_EM(errnum); \
        return x; \
    }

/* Utility Macros */
#define  sb_BCD_BYTE_TO_DECIMAL(num) (((num) & 0x0F) + ((((num) & 0xF0)>>4) * 10))
#define  sb_DECIMAL_TO_BCD_BYTE(num) (((((num)%100)/10)<<4) | ((num)%10))   /* max 99; higher values truncated (e.g. 100 --> 00) */
#define  sb_INVERT_U16(v16)          (u16)(((uint)((v16) & 0x00FF) << 8) | ((uint)((v16) & 0xFF00) >> 8))
#define  sb_INVERT_U32(v32)          (u32)(((u32)(v32) << 24) | (((u32)(v32) << 8) & 0x00FF0000) | (((u32)(v32) >> 8) & 0x0000FF00L) | ((u32)(v32) >> 24))
#define  sb_MIN(a, b)                (((a) > (b)) ? (b) : (a))
#define  sb_MAX(a, b)                (((a) > (b)) ? (a) : (b))
#define  sb_LOU16(l)                 ((u16)(l))
#define  sb_HIU16(l)                 ((u16)(((l) & 0xFFFF0000L)>>16))
#define  sb_MAKEU32(h,l)             ((u32)(((u32)(h)<<16) | (l)))
#if (SB_DATA_ALIGN == 1)
#define  sb_READ32_UNALIGNED(a)      (*(u32*)(a))
#define  sb_WRITE32_UNALIGNED(a, v)  (*(u32*)(a) = (v))
#else
#define  sb_READ32_UNALIGNED(a)      sb_read32_unaligned((u8 *)(a))
#define  sb_WRITE32_UNALIGNED(a, v)  sb_write32_unaligned((u8 *)(a), (v))
#endif

/* Time Measurement Macros */
/* The first call to TMInit() primes the caches. */
#if SB_TM_EN
#define  sb_TM_INIT()                {sb_TMInit(); \
        sb_TMInit();}
#define  sb_TM_START(p)              sb_TMStart(p);
#define  sb_TM_END(p, q)             sb_TMEnd(p, q);
#else
#define  sb_TM_INIT()
#define  sb_TM_START(p)
#define  sb_TM_END(p, q)
#endif

/* Old names to support existing code. Will be deleted in a future release. */
#define SB_INVERT_U16(v16)           sb_INVERT_U16(v16)
#define SB_INVERT_U32(v32)           sb_INVERT_U32(v32)
#define SB_MIN(a, b)                 sb_MIN(a, b)
#define SB_MAX(a, b)                 sb_MAX(a, b)
#define SB_LOU16(l)                  sb_LOU16(l)
#define SB_HIU16(l)                  sb_HIU16(l)
#define SB_MAKEU32(h,l)              sb_MAKEU32(h,l)

#endif /* SB_BAPI_H */
