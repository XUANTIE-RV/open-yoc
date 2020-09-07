/*
* bdef.h                                                    Version 4.4.0
*
* smxBase Definitions.
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

#ifndef SB_BDEF_H
#define SB_BDEF_H

#define SB_VERSION 0x0440
/*
   Version numbers are of the form XX.X.X. Using the hex scheme above,
   digits up to 15 (0xF) can be represented.
*/

/*===========================================================================*
*                               BASIC DEFINES                                *
*===========================================================================*/

/* Use NULL for pointers; use explicit 0 for numeric values.
   This is mostly for good style/readability. In the past this was
   more needed to avoid compiler warnings. Some C compilers required
   defining this as ((void *)0), but for C++ 0 is ok.
*/
#ifdef  NULL
#undef  NULL
#endif
#define NULL      0

#define OFF       0
#define ON        1

#define SB_FAIL   0
#define SB_PASS   1

#define SB_TMO_INF  0xFFFFFFFF  /* infinite timeout */

#if defined(SMX_BT_DEBUG)  /* NOTE: delete if not used for middleware */
#define STATIC
#else
#define STATIC    static
#endif


/*===========================================================================*
*                             BASIC DATA TYPES                               *
*===========================================================================*/

#define SB_BASIC_TYPES  /* used in SMX v3 xtypes32.h to avoid duplicate defs */

typedef  const char             cch;

#if (SB_CFG_EXACT_WIDTH_TYPES)
#include <stdint.h>
typedef  int8_t                 s8;
typedef  int16_t                s16;
typedef  int32_t                s32;
typedef  uint8_t                u8;
typedef  uint16_t               u16;
typedef  uint32_t               u32;
#else
typedef  signed   char          s8;
typedef  signed   short int     s16;
typedef  signed   long int      s32;
typedef  unsigned char          u8;
typedef  unsigned short int     u16;
typedef  unsigned long int      u32;
#endif

#if defined(SB_CPU_16BIT)
typedef  u16                    uint;
#elif defined(__DCC__) || defined(__GNUC__)  /* Diab and GNU have trouble if defined as u32 */
typedef  unsigned int           uint;
#else
typedef  u32                    uint;
#endif

typedef  volatile s8            vs8;
typedef  volatile s16           vs16;
typedef  volatile s32           vs32;
typedef  volatile u8            vu8;
typedef  volatile u16           vu16;
typedef  volatile u32           vu32;

#if (SB_CFG_INT64_TYPE)
/* 64-bit integers are not supported by some compilers or use different
   syntax. See the comment for SB_CFG_INT64_TYPE in bcfg.h.
*/
#if (SB_CFG_EXACT_WIDTH_TYPES)
typedef  int64_t                s64;
typedef  uint64_t               u64;
#else
typedef  signed   long long int s64;
typedef  unsigned long long int u64;
#endif /* SB_CFG_EXACT_WIDTH_TYPES */
typedef  volatile s64           vs64;
typedef  volatile u64           vu64;
#endif /* SB_CFG_INT64_TYPE */

typedef  float                  f32;
typedef  double                 f64;

#if defined(SB_OS_SMX) || defined(SB_OS_NORTOS)
typedef  u32   BOOLEAN;
#elif !defined(SB_OS_UCOS_II)
typedef  int   BOOLEAN;    /* int is probably most common way to define */
#endif

typedef  volatile BOOLEAN vBOOLEAN;
#define  FALSE 0
#define  TRUE  1

#ifndef __cplusplus
#if (SB_CFG_STDBOOL)
#include <stdbool.h>       /* Maps bool onto _Bool (C99) with #define bool _Bool. */
#else
typedef  int   bool;       /* Change type if needed/desired. <1> */
#endif
typedef  int   booli;      /* Keep int (fixed size) for use in structs. <1> */
#define  false 0
#define  true  1
#endif /* __cplusplus */

typedef  void (* FUN_PTR)(void);
typedef  void (* LSR_PTR)(u32);
/*
   ISR_PTR definition varies by compiler, processor, and OS, so it is
   defined below. For many cases for smx an isr is a normal C function
   since often the interrupt keyword is not sufficient and an assembly
   shell is required. Conditionals should be added below to handle any
   variance needed in the typedef as other environments are tested.
*/

#if defined(SB_OS_SMX3)
/* Do not use these data types. Here only for building middleware on SMX v3
   since we exclude the basic types in xtypes32.h on SB_BASIC_TYPES.
*/
#if defined(SB_CPU_ARM) || defined(SB_CPU_COLDFIRE) || defined(SB_CPU_POWERPC)
typedef  u32  u8_32;
#else
typedef  u8   u8_32;
#endif
typedef  void (* CODE_PTR)(void);
typedef  void (* CODE_PTRF)(void);
#endif

typedef struct {
    u16 wYear;
    u16 wMonth;
    u16 wDay;
    u16 wHour;
    u16 wMinute;
    u16 wSecond;
    u16 wMilliseconds;
} DATETIME;

/* Control Block Types */
#if SB_CFG_SHORT_ENUMS
typedef enum {
    SB_CB_NULL,       /* 0 */
    SB_CB_PCB,        /* 1 Must be same as SMX_CB_PCB */
    SB_CB_DCB         /* 2 */
} __short_enum_attr SB_CBTYPE;
#else
#define  SB_CB_NULL          0
#define  SB_CB_PCB           1 /* Must be same as SMX_CB_PCB */
#define  SB_CB_DCB           2
typedef u8 SB_CBTYPE;
#endif

/* Error Types */
typedef enum {
    SBE_OK,
    SBE_INSUFF_DAR,
    SBE_INV_ALIGN,
    SBE_INV_BP,
    SBE_INV_DAR,
    SBE_INV_LSR,
    SBE_INV_OFFSET,
    SBE_INV_POOL,
    SBE_INV_PRI,
    SBE_INV_SIZE,
    SBE_LQ_OVERFLOW,
    SBE_OUT_OF_LCBS,
    SBE_OUT_OF_PCBS,
    SBE_DAR_INIT_FAIL,
    SBE_INV_PARM,
    SB_NUM_ERRORS
} SB_ERRNO;

/* Peek Parameters */
typedef enum {
    SB_PK_BP,
    SB_PK_COUNT,
    SB_PK_ERROR,
    SB_PK_FIRST,
    SB_PK_FLAGS,
    SB_PK_FREE,
    SB_PK_INDEX,
    SB_PK_LAST,
    SB_PK_LIMIT,
    SB_PK_MAX,
    SB_PK_MIN,
    SB_PK_MODE,
    SB_PK_MSG,
    SB_PK_NAME,
    SB_PK_NEXT,
    SB_PK_NUM,
    SB_PK_POOL,
    SB_PK_SIZE,
    SB_PK_STATE,
} SB_PK_PARM;


/*===========================================================================*
*                    CONTROL BLOCKS AND OTHER STRUCTURES                     *
*===========================================================================*/

typedef struct SB_DCB {    /* DYNAMICALLY ALLOCATED REGION CONTROL BLOCK */
    u8         *pi;            /* pointer to first word of DAR */
    u8         *pl;            /* pointer to last block allocated */
    u8         *pn;            /* pointer to next free block */
    u8         *px;            /* pointer to last word of DAR */
} SB_DCB, *SB_DCB_PTR;

#if !defined(SB_OS_SMX3)
/* Note: shared with smx */
typedef struct PCB {       /* POOL CONTROL BLOCK */
    SB_CBTYPE   cbtype;        /* control block type = SB_CB_PCB */
    u8          pad1;
    u16         num;           /* number of blocks in pool */
    u16         pad2;
    u16         size;          /* block size */
    u8         *pi;            /* pointer to first block */
    u8         *pn;            /* pointer to next free block */
    u8         *px;            /* pointer to last block */
    const char *name;          /* pool name */
} PCB, *PCB_PTR;
#endif


/*===========================================================================*
*                            TARGET ENVIRONMENT                              *
*===========================================================================*/

/* Processor Architecture */
#define SB_UNKNOWN   0
#define SB_X86       1
#define SB_POWERPC   2
#define SB_COLDFIRE  3
#define SB_SUPERH    4
#define SB_ARM       5
#define SB_68K       6
#define SB_ARMM      7
#define SB_BLACKFIN  8
#define SB_RX        9

#if defined(SB_CPU_PORTABLE)   /* Must precede other SB_CPU_ checks. */
#define SB_CPU_ARCH SB_UNKNOWN /* Change to one of the settings above to build a reference port. */

#elif defined(SB_CPU_ARM)
#if defined(SB_CPU_ARMM)
#define SB_CPU_ARCH SB_ARMM
#else
#define SB_CPU_ARCH SB_ARM
#endif

#elif defined(SB_CPU_BLACKFIN)
#define SB_CPU_ARCH SB_BLACKFIN

#elif defined(SB_CPU_COLDFIRE)
#define SB_CPU_ARCH SB_COLDFIRE

#elif defined(SB_CPU_M68K)
#define SB_CPU_ARCH SB_68K

#elif defined(SB_CPU_POWERPC)
#define SB_CPU_ARCH SB_POWERPC

#elif defined(SB_CPU_RX)
#define SB_CPU_ARCH SB_RX

#elif defined(SB_CPU_SUPERH)
#define SB_CPU_ARCH SB_SUPERH

#elif defined(SB_CPU_X86)
#define SB_CPU_ARCH SB_X86
#error x86 is no longer supported. Use smx and smxBase v4.1.

#else
#error Must define target environment on compiler command line. Fix makefile.
#endif

/* Processor Type Encoding */
/*
   This is very basic for now and may change in the future if we decide
   to encode more detail about the processor being used. Currently it is
   needed only to initialize smxProcessor, which is used by smxAware.

   Current Encoding (binary):  PPPP000000000000

                     P = processor family
*/

#define SB_PROCESSOR  (SB_CPU_ARCH << 12)


/*===========================================================================*
*                                 CONSTANTS                                  *
*===========================================================================*/

/* Message Output Types */
#define SB_MSG_ERR         0  /* Errors. Possibly catastrophic. */
#define SB_MSG_WARN        2  /* Warnings. May still run properly. */
#define SB_MSG_INFO        4  /* Informational message (status/debug). */

#if (SB_CON_OUT)
/* Message Display Constants */
#define SB_DISP_XMIN      40
#define SB_DISP_XMAX      (SB_CON_COLS_MAX)
#define SB_DISP_LL        (SB_DISP_XMAX - SB_DISP_XMIN - 1)
#if defined(SMX_ESMX) || defined(SMX_TSMX)
#define SB_DISP_YMIN       0
#else
#define SB_DISP_YMIN       1
#endif
#define SB_DISP_YMAX      (SB_CON_ROWS_MAX-1)
#endif


/*===========================================================================*
*                             GLOBAL VARIABLES                               *
*===========================================================================*/

extern SB_DCB      sb_ADAR;
extern SB_DCB_PTR  sb_adar;
extern SB_DCB      sb_SDAR;
extern SB_DCB_PTR  sb_sdar;
extern u8         *sb_isp;    /* interrupt stack pointer */
extern u8         *sb_ssp;    /* system stack pointer */

#if SB_TM_EN  /* bcfg.h */
extern u32         sb_te1;    /* ending times for time measurements */
extern u32         sb_te2;
extern u32         sb_te3;
extern u32         sb_te4;
extern u32         sb_ts1;    /* starting times for time measurements */
extern u32         sb_ts2;
extern u32         sb_ts3;
extern u32         sb_ts4;
extern u32         sb_TMCal;
#endif

extern SB_ERRNO    sb_errno;
extern u32         sb_errctr;

extern const BOOLEAN sb_cfg_msgout_direct;

/*
Notes:

1. bool is a pre-defined type for C++ and its size is controlled by the
   compiler. For C we have to define it. For C99 we use stdbool.h to
   map it to _Bool. It is defined as int for efficiency, but you can
   change it if this causes a problem or if you prefer to use another type.

   booli is always int-sized, for use in structs where alignment matters.
   Since bool may be a smaller type, a (bool) typecast is needed to assign
   a booli to a bool.
*/

#endif /* SB_BDEF_H */
