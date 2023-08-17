//----------------------------------------------------------------------
//      FTSAFE INC COS Kernel: (Portable data prototype)
//----------------------------------------------------------------------
//      File: PTYPE.H
//----------------------------------------------------------------------
//      Update History: (mm/dd/yyyy)
//      10/08/2002 - V1.00 - LCS :  First official release
//      08/25/2004 - V1.01 - LCS :  为适应HIWARE C编译器进行修订。
//----------------------------------------------------------------------
//      Description:
//
//      可移植的数据类型。
//
//      08/25/2004 - V1.01 - LCS :
//      我们使用一个比较复杂的类型声明形式定义那些指向EEPROM的指针：
//      VOID EEPROM *BANKED pointer;
//      EEPROM前缀为了IAR编译器，BANKED后缀为HIWARE编译器而设。
//
//----------------------------------------------------------------------
#ifndef _PTYPE_H_
#define _PTYPE_H_
//---------------------------------------------------------------------------------------------
//                          Portable data type defined
//---------------------------------------------------------------------------------------------

#define CODESECT    const


typedef unsigned char  INT8U;                    // Unsigned  8 bit quantity
typedef signed   char  INT8S;                    // Signed    8 bit quantity
typedef unsigned short INT16U;                   // Unsigned 16 bit quantity
typedef signed   short INT16S;                   // Signed   16 bit quantity
typedef unsigned           char UINT8;
typedef unsigned short     int  UINT16;
typedef unsigned           int  UINT32;
typedef unsigned long long      UINT64;

typedef unsigned int  INT32U;                   // Unsigned 32 bit quantity
typedef signed int  INT32S;                     // Signed   32 bit quantity

typedef float          FP32;                     // Single precision floating point
typedef unsigned char  BOOL;
typedef void           VOID;
typedef struct _UINT_64
{
	INT32U uiHigh;
	INT32U uiLow;
}INT64U;




typedef unsigned char           T_U8;
typedef char                    T_S8;
typedef unsigned short          T_U16;
typedef short                   T_S16;
typedef unsigned int			T_U32;
typedef int						T_S32;
typedef void					T_VOID;
#endif

