
#ifndef    __AP1508_DATATYPE_H__
#define    __AP1508_DATATYPE_H__

#ifndef NULL
#define    NULL  0x00
#endif

#ifndef true
#define true  0x01
#endif
#ifndef false
#define false 0x00
#endif

#ifndef SUCCESS
#define SUCCESS  0
#endif
#ifndef FAILURE
#define FAILURE  -1
#endif
#define TIMEOUT  0x1000

#define STATUS_ERR  1
#define STATUS_OK   0

#define u8  unsigned char
#define u16  unsigned short
#define u32  unsigned int

#define  IN
#define  OUT
#define INOUT

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

#define __IO                volatile

#define __REG32(addr)             (*(volatile uint32_t*)(addr))
#define __REG16(addr)             (*(volatile uint16_t*)(addr))
#define __REG8(addr)              (*(volatile uint8_t*)(addr))
#define __sREG32(addr, offset)    (*(volatile uint32_t*)((addr) + offset))
#define __sREG16(addr, offset)    (*(volatile uint16_t*)((addr) + offset))
#define __sREG8(addr,  offset)    (*(volatile uint8_t*)((addr) + offset))

#define bit0 0x01
#define bit1 0x02
#define bit2 0x04
#define bit3 0x08
#define bit4 0x10
#define bit5 0x20
#define bit6 0x40
#define bit7 0x80

typedef void (*hdl_t)(uint32_t);
typedef void (*tsk_t)(void);

#endif  // __DATATYPE_H__

