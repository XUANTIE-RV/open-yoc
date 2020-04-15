/*
* bcc.h                                                     Version 4.3.2
*
* C Compiler Definitions and Run-Time Library Replacements include file.
* The main purposes of this file are to:
*
* 1. define macros to map onto some compiler keywords, for portability.
* 2. include C run-time library header files and definitions that MUST
*    precede smx header files when there is an issue such as the heap
*    functions vs. smx's translation macros.
* 3. supply prototypes for functions missing in some compilers' RTLs.
* 4. include common RTL headers so it's not necessary to add includes
*    to many files.
*
* Note: bdef.h must be included before this file since it defines the
*       target-related constants. smx headers should be included after
*       this file.
*
* Copyright (c) 2003-2016 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: David Moore
*
*****************************************************************************/

#ifndef SB_BCC_H
#define SB_BCC_H

/*===========================================================================*
*          C RTL Headers and Defines that Must Precede smx Headers           *
*===========================================================================*/

/*
*  Note: Including stdlib.h ahead of xapi.h, since otherwise, translation
*        macros in xapi.h would cause heap call names in stdlib.h to be
*        translated to smx names, causing build errors.
*/
#include <stdlib.h>

#if defined(__MWERKS__)
#if defined(SB_CPU_POWERPC) && (__MWERKS__ >= 0x2300)
/* extras.h was not included in the CW PPC 5.0 release (0x2300 to 0x23FF) or in CW PPC 8.0 (0x3000 to 0x30ff) */
#else
#include <extras.h>     /* _itoa() and _ltoa(). Defines malloc() so has to be here like stdlib.h (see comment above) */
#endif
#endif


/*===========================================================================*
*                             Other C RTL Headers                            *
*===========================================================================*/
/*
   These do not need to precede smx headers, but are here for convenience.
*/

#include <string.h>     /* memcpy, memset, etc. */
#include <stdarg.h>

#if defined(SB_CPU_ARMM) && defined(__GNUC__)
#endif


/*===========================================================================*
*                             Compiler Defines                               *
*===========================================================================*/

#if defined(__IAR_SYSTEMS_ICC__) || defined(__MWERKS__)
#include "ctype.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* packed is to make sure structure members are not 32 bit aligned for some compilers */

#if defined(__CC_ARM)                   /* ARM RealView / Keil */
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu __attribute__((packed))
#define __packed_pragma 0
#define __short_enum_attr
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
char *_strupr(char *s);
char *_ultoa(unsigned long v, char *str, int r);
int toupper(int c);
#define __interdecl __irq
#define __interrupt

#elif defined(__DCC__)                    /* Diab Data C++ */
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __packed_pragma 1
#define __short_enum_attr
int _stricmp(const char *__s1, const char *__s2);
#if  defined(SB_CPU_POWERPC)
int strncasecmp(const char *__s1, const char *__s2, size_t __n);
#define _strnicmp strncasecmp
#else
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
#endif
char *_strupr(char *s);
//void _ultoa(unsigned long v, char * str, int r);
int toupper(int c);

#elif defined(__GNUC__)                   /* GNU C/C++ */
#define __inline__      __inline__
#define __interdecl
#define __interrupt __attribute__((interrupt))
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#if !defined(__packed)  /* see note <1> */
#define __packed
#endif
#define __packed_gnu    __attribute__((packed))
#if defined(SB_CPU_RX)
#define __packed_pragma 1
#else
#define __packed_pragma 0
#endif
#define __short_enum_attr __attribute__((packed))
#define __unaligned
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
char *_strupr(char *s);
#if !defined(__CROSSWORKS_ARM)
char *_ultoa(unsigned long v, char *str, int r);
int toupper(int c);
#endif

#elif defined(__HIGHC__)                  /* MetaWare High C/C++ */
#define __inline__      inline
#define __interdecl
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#undef  __packed
#define __packed        _Packed
#define __packed_gnu
#define __packed_pragma 0
#define __short_enum_attr
#define __unaligned     _Unaligned
#if defined(SB_CPU_POWERPC)
#define _asm            asm
#define __asm           _ASM
#endif

#elif defined(__IAR_SYSTEMS_ICC__)        /* IAR EWARM */
#define __inline__      inline
#if defined(SB_CPU_ARMM) || defined(SB_OS_SMX) || defined(SB_OS_UCOS_II)
#define __interdecl
#else
#define __interdecl     __irq
#endif
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __packed_pragma 1
#define __short_enum_attr
#define __unaligned
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
char *_strupr(char *s);
char *_ultoa(unsigned long v, char *str, int r);

#elif defined(__MWERKS__)                 /* CodeWarrior */
#define __inline__
#define __interdecl     __declspec(interrupt)
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __unaligned
#if defined(SB_CPU_POWERPC)
#define __packed_pragma 0
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
#else /* not SB_CPU_POWERPC */
#define __packed_pragma 1
#define _stricmp stricmp
#define _strnicmp strnicmp
int toupper(int c);
#endif /* SB_CPU_POWERPC */
#define __short_enum_attr

#elif defined(__RENESAS__)                /* Renesas */
#define __inline__      inline
#define __interdecl
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __packed_pragma 1
#define __short_enum_attr
#define __unaligned
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
char *_strupr(char *s);

#elif defined(__VISUALDSPVERSION__) || defined(_ADI_COMPILER)     /* Analog Devices */
#define __inline__      inline
#define __interdecl
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __packed_pragma 1
#define __short_enum_attr
#define __unaligned
int _stricmp(const char *__s1, const char *__s2);
int _strnicmp(const char *__s1, const char *__s2, size_t __n);
char *_strupr(char *s);
char *_ultoa(unsigned long v, char *str, int r);
int toupper(int c);

#else
#error Define __packed etc. macros for your compiler in bdef.h.
#define __inline__
#define __interdecl
#define __interrupt
#if !defined(SB_OS_SMX3)
typedef void (* ISR_PTR)(void);
#endif
#define __packed
#define __packed_gnu
#define __packed_pragma 1
#define __short_enum_attr
#define __unaligned
#endif

#ifdef __cplusplus
}
#endif

#if (SB_CPU_MEM_ADDR_8BIT)  /* see comment where defined in bcfg.h */
#define SB_PACKED_STRUCT_SUPPORT 1
#else
#define SB_PACKED_STRUCT_SUPPORT 0
/* Disable packed keywords/pragmas above. */
#undef  __packed
#define __packed
#undef  __packed_gnu
#define __packed_gnu
#undef  __packed_pragma
#define __packed_pragma 0
#endif

#if (SB_CPU_MEM_ADDR_8BIT)
#define SB_BYTES_TO_CHARS(size)  (size)
#else
/* Convert the number of bytes (8-bit) into the number of characters
   for use in pointer manipulations and calls to malloc, memset, memcpy.
   For most processors, bytes and chars are the same, but on some such
   as some TI DSPs, the minimum access size is 16-bit, so a char is defined
   as 16-bit. In this case, sizeof(u16) returns 1. When data is read from
   a disk on these processors, it is packed with 2 bytes per char, so if
   we want to advance a pointer 100 bytes, we add 50 to it, for example.
*/
#define SB_BYTES_TO_CHARS(size)  (sizeof(u16)*(size)/2)
#endif

#if defined(__CC_ARM) || defined(__DCC__)

/* Most compilers treat an undefined symbol as having been defined as 0,
   if tested in a preprocessor conditional. Picky compilers give a warning
   in this case, which is a problem if the symbol is used in a header file
   because then the warning appears for every file that is compiled. The
   ARM compiler is one example. Here we explicitly define undefined symbols,
   to avoid the warnings.
*/
#if !defined(__BACKENDVERSION__)
#define __BACKENDVERSION__ 0
#endif
#if !defined(__embedded_cplusplus)
#define __embedded_cplusplus 0
#endif
#if !defined(__HIGHC__)
#define __HIGHC__ 0
#endif
#if !defined(__MSL__)
#define __MSL__ 0
#endif
#if !defined(_MSL_OS_TIME_SUPPORT)
#define _MSL_OS_TIME_SUPPORT 0
#endif

#endif /* __CC_ARM || __DCC__ */

#if defined(__GNUC__)
#include "ctype.h"

/* Stringification Macros. See "3.4 Stringification" in GNU C Preprocessor
   manual. These allow using #defined constants/macros in inline assembly,
   such as:
      __asm("mov  r0, #" stringify_expanded(SOME_CONSTANT) "\n\t");
   stringify() makes the name of the constant/macro into a string.
   stringify_expanded() makes the value of the constant/macro into a string.
*/
#define stringify_expanded(x) stringify(x)  /* Expand argument then stringify */
#define stringify(x)          #x            /* Stringify argument */
#endif /* __GNUC__ */


/*===========================================================================*
*                               Other RTL Stuff                              *
*===========================================================================*/

/* itoa(), ltoa(), etc. */

/* These functions are not supplied with some compilers so we define our
   own versions. Here we define prototypes in case the compiler doesn't.
*/

#if defined(__MWERKS__)  /* CodeWarrior */
#if defined(SB_CPU_POWERPC) && (__MWERKS__ >= 0x2300)
#ifdef __cplusplus
extern "C" {
#endif
/* CW PPC no longer has an include file for these (extras.h). */
char *_itoa(int val, char *str, int radix);
char   *itoa(int val, char *str, int radix);
#if (__MWERKS__ >= 0x3000)
/* It is necessary to provide our own functions to replace the ones in extras.h
   because the current version of ansi_prefix......h has _MSL_NEEDS_EXTRAS defined as 0
   so the extras functions will not be part of the MSL_C library.
*/
char *_ltoa(int val, char *str, int radix);
char   *ltoa(int val, char *str, int radix);

#else
char *_ltoa(long val, char *str, int radix);
char   *ltoa(long val, char *str, int radix);
#endif
char *_ultoa(unsigned long val, char *str, int radix);
char   *ultoa(unsigned long val, char *str, int radix);
char *_strupr(char *str);
char   *strupr(char *str);
#ifdef __cplusplus
}
#endif
#else
/* CW CF defines _ltoa() first par wrong (as int), so just use
   its defs rather than those below to avoid compiler error.
*/
#endif
#else  /* other compilers */
#ifdef __cplusplus
extern "C" {
#endif
char *_itoa(int val, char *str, int radix);
char *_ltoa(long val, char *str, int radix);
char *_ultoa(unsigned long val, char *str, int radix);
char *_strupr(char *str);
#ifdef __cplusplus
}
#endif
#endif


/* Macros: CodeWarrior used to define it only with the underscore, then
   both ways, and now in "Pro 9" (e.g. CWCF v5) without the underscore.
*/
#if (defined(__MWERKS__) && (__MSL__ >= 0x9000))
#define  _itoa   itoa
#define  _ltoa   ltoa
#define  _ultoa  ultoa
#define  _strupr strupr
#else
#define  itoa   _itoa
#define  ltoa   _ltoa
#define  ultoa  _ultoa
#define  strupr _strupr
#endif


/* Configuration: Specify whether to use C library functions (1) or ours (0). */

/* CC_XTOA is for itoa() and ltoa() */
#if (defined(SB_CPU_COLDFIRE) && defined(__MWERKS__) && (__MWERKS__ >= 0x4305))  /* 0x4305 is CWCF v7.2.2 */
#define SB_CC_XTOA 1
#else
/* Use our functions for all other cases. Simpler than figuring out which
   compilers support them, which are buggy, etc.
*/
#define SB_CC_XTOA 0
#endif

/* Use our functions for all cases. Simpler than figuring out which
   compilers support them, which are buggy, etc.
*/
#define SB_CC_ULTOA 0

#if defined(__CC_ARM) || defined(__CROSSWORKS_ARM) || defined(__GNUC__) || \
    defined(__IAR_SYSTEMS_ICC__) || \
    (defined(SB_CPU_COLDFIRE) && (__MWERKS__ >= 0x4300)) || \
    (defined(SB_CPU_POWERPC) && (__MWERKS__ >= 0x3000))  || \
    (defined(SB_CPU_POWERPC) && defined(__DCC__)) || \
    (defined(_ADI_COMPILER))
/* ARM RealView / Keil, CrossWorks, GNU C/C++, IAR, CodeWarrior, and Diab
   do not supply _stricmp().
*/
#define SB_CC_STRICMP 0
#else
#define SB_CC_STRICMP 1
#endif

#if defined(__CROSSWORKS_ARM) || (defined(__DCC__) && defined(SB_CPU_POWERPC) || \
    (defined(SB_CPU_COLDFIRE) && defined(__MWERKS__) && (__MWERKS__ >= 0x4305))) || \
    (defined(_ADI_COMPILER))

/* CrossWorks, Diab for PowerPC, and some CodeWarrior ColdFire do not supply _strnicmp().
*/
#define SB_CC_STRNICMP 0
#else
#define SB_CC_STRNICMP 1
#endif

#if defined(__CC_ARM) || defined(__CROSSWORKS_ARM) || defined(__DCC__) || defined(__IAR_SYSTEMS_ICC__) || \
    defined(__MWERKS__) || (defined(SB_CPU_POWERPC) && defined(__HIGHC__)) || (defined(_ADI_COMPILER))
/* ARM/Keil, CrossWorks, Diab Data C/C++, IAR, CodeWarrior, and
   MetaWare High C/C++ PowerPC do not supply _strupr().
*/
#define SB_CC_STRUPR 0
#else
#define SB_CC_STRUPR 0
#endif

#if defined(__GNUC__) && defined(SB_CPU_RX)
/* GNU C/C++ do not supply toupper().
*/
#define SB_CC_TOUPPER 0
#else
#define SB_CC_TOUPPER 1
#endif


/* Time Functions */
/* Add #elif cases for other compilers that don't support them. */

#if defined(__MWERKS__) && defined(_MSL_OS_TIME_SUPPORT) && !_MSL_OS_TIME_SUPPORT
/* Before CodeWarrior added _MSL_OS_TIME_SUPPORT they were always supported.
   Note that stdlib.h and other headers included above include the header
   file that defines this symbol.
*/
#define SB_CC_TIME_FUNCS 0
#elif defined (__CROSSWORKS_ARM)
#define SB_CC_TIME_FUNCS 0
#else
#define SB_CC_TIME_FUNCS 1
#endif

/*
   Notes:
   1. Newer versions of gcc define __packed, but it still must be used at
      the end of a struct definition, not the start as for other compilers.
      We added this check to avoid a warning about redefining it, but the
      compiler will generate warning: '__packed__' attribute ignored for
      all structs that use __packed at the start. (Note that the warning
      is reported for the last line of the struct, making it look like it
      applies to the __packed_gnu macro, which isn't the case.) You could
      turn off the warning or rename our __packed macro and change all uses.
*/

#endif /* SB_BCC_H */

