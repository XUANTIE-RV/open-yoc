/*
* bcfg.h                                                    Version 4.4.0
*
* smxBase Configuration.
*
* Copyright (c) 2010-2018 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Authors: David Moore and Yingbo Hu
*
*****************************************************************************/

#ifndef SB_BCFG_H
#define SB_BCFG_H

/* OS Selection. Uncomment only ONE. */
#define SB_OS_SMX

/* Multitasking OS. 1 for MT. 0 for Non-MT. */
#if defined(SB_OS_SMX) || defined(SB_OS_CMX) || defined(SB_OS_ECOS) || \
    defined(SB_OS_FREERTOS) || defined(SB_OS_ITRON) || defined(SB_OS_MQX) || \
    defined(SB_OS_POWERPAC) || defined(SB_OS_NUCLEUS_PLUS) || defined(SB_OS_QUADROS) || \
    defined(SB_OS_RTX) || defined(SB_OS_THREADX) || defined(SB_OS_UCOS_II) || \
    defined(SB_OS_UCOS_III) || defined(SB_OS_VRTX) || defined(SB_OS_VXWORKS)
#define SB_MULTITASKING  1
#elif defined(SB_OS_NORTOS)
#define SB_MULTITASKING  0
#else
#error Configure SB_MULTITASKING for your OS in bcfg.h.
#define SB_MULTITASKING  0
#endif


/* CPU Architecture */
#define SB_CPU_ARM
#define SB_CPU_ARMM

#if defined(SB_OS_SMX)
/* Use what was passed on the command line or defined by the compiler. */
#if defined(_ARM) || defined(arm) || defined(__arm__) || defined(__ICCARM__)  /* compilers define these so not passed on command line */
#define SB_CPU_ARM
#else
/* Use what was passed on the command line. */
#endif

#else
#error Set CPU Family in bcfg.h. Uncomment ONE.
#endif
#define __TARGET_PROFILE_M
/***********************/
#if defined(SB_CPU_ARM) && \
    (defined(__TARGET_PROFILE_M))                  /* ARM RealView/Keil */ || \
    (defined(__ARM_ARCH_7M__))                     /* GNU */ || \
    (defined(__CORE__) && ((__CORE__ == __ARM7M__) || (__CORE__ == __ARM7EM__))) /* IAR */
#define SB_CPU_ARMM  /* Remember ARM is also defined since compiler defines it or variant */
#endif


/* CPU Operation Size */

#define SB_CPU_32BIT


/* CPU Memory Addressing Granularity */
/*
   Most processors can address 8-bit data, in which case this should be
   set to 1. However, some TI DSPs, for example, only allow accessing
   16-bit values. This is a big problem for protocol data structures
   that have 8-bit field and are packed. In this case, set this to 0
   so code is enabled to handle the problem.
*/
#define SB_CPU_MEM_ADDR_8BIT 1  /* 1 for most processors */


/* CPU I/O Type */
#define SB_CPU_MEM_MAPPED_IO 1


/* Interrupt Settings */
#define SB_CFG_IRQ_MAX_NUM    10  /* also change bcfg.inc, if present */
#define SB_CFG_IRQ_MAX_SHARED 2


/* Data Types */

/* Set SB_CFG_SHORT_ENUMS to 1 if the compiler supports short enums, and the
   switch is used (or default) that enables them.
*/
#define SB_CFG_SHORT_ENUMS      1

#if defined(__IAR_SYSTEMS_ICC__) || defined(__MWERKS__)  /* Add others as supported. */
#define SB_CFG_EXACT_WIDTH_TYPES 1
#else
#define SB_CFG_EXACT_WIDTH_TYPES 0
#endif

/*
   stdbool.h was added in C99 but IAR and ARM RealView/Keil for ARM define
   __STDC_VERSION__ >= 199409L and they have it, so we check for this.
   If stdbool.h is not found when supporting a new compiler, the conditional
   will have to be changed to something more complicated that also checks
   which compiler is being used (e.g. __ICCARM__ for IAR). For C99 the
   check is __STDC_VERSION__ >= 199901L. If the compiler does not have
   stdbool.h but does support _Bool, just add #define bool _Bool in place
   of #include <stdbool.h> in bdef.h.
*/
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L)
#define SB_CFG_STDBOOL 1
#else
#define SB_CFG_STDBOOL 0
#endif

/*
   64-bit integers are not supported by some compilers, and they are
   supported differently by some. (Most use long long.) If you get a
   syntax error in bdef.h where u64, etc. are defined, check if your
   compiler has an alternate syntax. Otherwise, add your compiler to
   the first check below to exclude those typedefs. You will not be
   able to use them or any of our code that uses them.
*/
#define SB_CFG_INT64_TYPE 1


/*
   Set to 1 to use interrupt-driven UART driver; 0 to use the polling
   vendor provided code. Only supported for the targets indicated
   in the conditional.
*/
#if defined(SMX_TSMX)
#define SB_CFG_UARTI 0  /* Keep 0 */
#elif defined(SB_CPU_STM32) || defined(SB_CPU_KXX) || defined(SB_CPU_AT91SAMV) // || defined(SB_CPU_IMX6) || defined(SB_CPU_AT91SAMA5)
#define SB_CFG_UARTI 1
#else
#define SB_CFG_UARTI 0
#endif


/* Console I/O */

/*
   SB_CON_IN
   Set to 1 if there is a keyboard or similar key input device on the target.
   When 1, opcon_main() and the keyboard pipe are enabled for smx.
   The BSP implements the keyboard driver.

   SB_CON_OUT
   Set to 1 if there is a console device on the target. When 1, calls to
   routines such as sb_ConWriteString() are enabled. The BSP can implement
   these to go to text video, graphics video, a serial port, a buffer
   in memory, etc.
*/
#if defined(SB_CPU_M9328MX1_MXL) || \
    defined(SB_BRD_STMICRO_STM32F3DISCOVERY) || defined(SB_BRD_STMICRO_STM32F4DISCOVERY) || defined(SB_BRD_STMICRO_STM32F429I_DISCOVERY) || \
    defined(SB_BRD_INFINEON_XMC4500_RK)
/* Remove this case when UART driver finished. */
#define SB_CON_IN  0
#define SB_CON_OUT 0
#elif defined(SB_BRD_IAR_TMS470R1A256DB)
#define SB_CON_IN  0
#define SB_CON_OUT 1
#else
#define SB_CON_IN  1
#define SB_CON_OUT 1
#endif

#if SB_CFG_UARTI
/* Buffer Sizes for Interrupt-Driven UART Driver for Terminal I/O */
#if SB_CON_IN
#define SB_CON_IN_BUFSZ    16
#else
#define SB_CON_IN_BUFSZ     0
#endif
#if SB_CON_OUT
#define SB_CON_OUT_BUFSZ  256
#else
#define SB_CON_OUT_BUFSZ    0
#endif
#endif

/* Message Output Configuration */

/*
   OMB is for messages built in RAM. OMQ is for constant messages in ROM.
   These settings are not used if SB_CFG_MSGOUT_DIRECT is 1.
*/
//#define SB_CFG_OMB_SIZE    20000  /* output message buffer size (bytes) */
//#define SB_CFG_OMQ_SIZE     1000  /* output message queue size (records) (40 for TSMX) */
#define SB_CFG_OMB_SIZE    512  /* output message buffer size (bytes) */
#define SB_CFG_OMQ_SIZE     40  /* output message queue size (records) (40 for TSMX) */

#define SB_CFG_MSGOUT_DIRECT 0  /* Normally 0. Set to 1 output messages directly to UART (no OMQ or OMB) for diagnostic purposes or if memory too limited for OMQ and OMB. */
#define SB_CFG_MSGOUT_VARMSG 1  /* If 0, can only output const messages; if 1 can also output variable messages. (Ignored if SB_CFG_MSGOUT_DIRECT == 1.) */

/*
   Set to the number of millisec for sb_MsgOutput__() to delay after writing
   the message. Useful when debugging and messages are written to a terminal
   via interrupt-driven serial driver. Ignored if SB_CON_OUT_PORT == -1
   which means direct screen writes (no serial port).
*/
#define SB_CFG_MSGOUT_DELAY 0


/* Other Config */

/*
   For some OS ports such as SMX, it is possible to use a mutex or a
   counting semaphore to protect critical sections. Mutexes may offer
   advanced capabilities such as avoidance of priority inversion, but
   they may add more complexity. A counting semaphore is adequate in
   many systems. Set to 1 to use mutexes; 0 to use semaphores.
*/
#define SB_CFG_OSPORT_USE_MUTEX 1

/* Enable time measurements */
#define SB_TM_EN 1

#if !defined(SB_OS_SMX)
/* This simplifies checks for MPU in BSP code. */
#define SMX_CFG_MPU 0
#endif

#endif /* SB_BCFG_H */

