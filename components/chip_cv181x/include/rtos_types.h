#ifndef _RTOS_TYPES_H
#define _RTOS_TYPES_H


/* move <uapi/asm-generic/int-ll64.h> here directly*/
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;


typedef	unsigned long	size_t;
typedef unsigned long	uintptr_t;
typedef long		intptr_t;
typedef unsigned short	umode_t;

/* uapi/asm-generic/posix_types.h */
typedef int          __kernel_pid_t;
typedef long         __kernel_long_t;;
typedef __kernel_long_t      __kernel_off_t;
typedef long long    __kernel_loff_t;

/*
 * The following typedefs are also protected by individual ifdefs for
 * historical reasons:
 */

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef __u16  __le16;
typedef __u16  __be16;
typedef __u32  __le32;
typedef __u32  __be32;
typedef __u64  __le64;
typedef __u64  __be64;


#define EOF      (-1)
#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#endif

