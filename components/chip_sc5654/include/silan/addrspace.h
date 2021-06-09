/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _ADDRSPACE_H_
#define _ADDRSPACE_H_

#include <stdint.h>

#define __REG32(addr)             (*(volatile uint32_t*)(addr))
#define __REG16(addr)             (*(volatile uint16_t*)(addr))
#define __REG8(addr)              (*(volatile uint8_t*)(addr))
#define __sREG32(addr, offset)    (*(volatile uint32_t*)((addr) + offset))
#define __sREG16(addr, offset)    (*(volatile uint16_t*)((addr) + offset))
#define __sREG8(addr,  offset)    (*(volatile uint8_t*)((addr) + offset))

/*
 * Map an address to a certain kernel segment
 */
#define KSEG0(addr)     (addr)
#define KSEG1(addr)     (addr)
#define PHYS(addr)      (addr)

typedef void (*hdl_t)(uint32_t );
typedef void (*tsk_t)(void);

#endif /* ADDRSPACE_H */

