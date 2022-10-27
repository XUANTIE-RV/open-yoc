#ifndef __RISCV_ARCH_TYPES_H
#define __RISCV_ARCH_TYPES_H

/*********************************************************************************************************
  C++ 下 void 不可以 typedef
*********************************************************************************************************/

#ifdef __cplusplus
#define VOID     void
#else
typedef          void               VOID;                               /*  void   类型                 */
#endif                                                                  /*  __cplusplus                 */

/*********************************************************************************************************
  编译器相关数据类型定义
*********************************************************************************************************/

typedef          int                BOOL;                               /*  布尔变量定义                */

typedef          void              *PVOID;                              /*  void * 类型                 */
typedef const    void              *CPVOID;                             /*  const void  *               */

typedef          char               CHAR;                               /*  8 位字符变量                */
typedef unsigned char               UCHAR;                              /*  8 位无符号字符变量          */
typedef unsigned char              *PUCHAR;                             /*  8 位无符号字符变量指针      */
typedef          char              *PCHAR;                              /*  8 位字符指针变量            */
typedef const    char              *CPCHAR;                             /*  const char  *               */

typedef unsigned char               BYTE;                               /*  8 位字节变量                */
typedef unsigned char              *PBYTE;                              /*  8 位字节变量指针            */

typedef          long               LONG;                               /*  32/64 位数定义              */
typedef unsigned long               ULONG;                              /*  32/64 位无符号数定义        */

typedef          int                INT;                                /*  编译器相关 int              */
typedef unsigned int                UINT;                               /*  编译器相关 unsigned int     */
typedef signed   int                SINT;                               /*  编译器相关 signed   int     */

/*********************************************************************************************************
  注意: INT8 必须是 signed 型. 这样 int8_t 才能保证是 signed 型.
        很多编译器默认 char 是 unsigned. 所以这里必须强制将 INT8 定义为 signed 型.
*********************************************************************************************************/

typedef signed   char               INT8;                               /*  char                        */
typedef unsigned char               UINT8;                              /*  8  位无符号数定义           */
typedef signed   char               SINT8;                              /*  8  位有符号数定义           */

typedef          short              INT16;                              /*  short                       */
typedef unsigned short              UINT16;                             /*  16 位无符号数定义           */
typedef signed   short              SINT16;                             /*  16 位有符号数定义           */

typedef          int                INT32;                              /*  long or int                 */
typedef unsigned int                UINT32;                             /*  32 位无符号数定义           */
typedef signed   int                SINT32;                             /*  32 位有符号数定义           */

typedef          long long          INT64;                              /*  long long                   */
typedef unsigned long long          UINT64;                             /*  64 位无符号数定义           */
typedef signed   long long          SINT64;                             /*  64 位有符号数定义           */

#if 0
#ifndef __addr_t_defined
typedef ULONG        addr_t; // unsigned long int
#define __addr_t_defined 1
#endif

#ifndef __ioaddr_t_defined
typedef ULONG        ioaddr_t;
#define __ioaddr_t_defined 1
#endif
#else
#ifndef __addr_t_defined
typedef  long unsigned int        addr_t; // unsigned long int
#define __addr_t_defined 1
#endif

#ifndef __ioaddr_t_defined
typedef  long unsigned int        ioaddr_t;
#define __ioaddr_t_defined 1
#endif
#endif
/*********************************************************************************************************
  此类型变量用来保存系统开关中断的上下文.
  注意: INTREG 仅用于记录开关中断状态, 而非整个标志寄存器, 所以 riscv-64 也使用 32 位 unsigned int 类型
*********************************************************************************************************/

typedef volatile unsigned int       INTREG;                             /*  定义处理器中断寄存器        */

#endif                                                                  /*  __RISCV_ARCH_TYPES_H        */
/*********************************************************************************************************
  END
*********************************************************************************************************/

