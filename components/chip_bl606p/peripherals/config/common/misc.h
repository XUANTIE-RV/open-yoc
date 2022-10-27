/**
 * @file misc.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef _MISC_H
#define _MISC_H

#include "./common/io.h"
#include "./common/riscv_types.h"
#include "./common/bl_misc.h"
/** @defgroup  COMMON_Public_Constants
 *  @{
 */

/**
 * @brief Memory access macro
 */
#ifndef readl
#define BL_RD_WORD(addr)       (*((volatile uint32_t *)(uintptr_t)(addr)))
#else
#define BL_RD_WORD(addr)       readl((void *)((addr_t)(addr)))
#endif

#ifndef writel
#define BL_WR_WORD(addr, val)  ((*(volatile uint32_t *)(uintptr_t)(addr)) = (val))
#else
#define BL_WR_WORD(addr, val)  writel(val, (void *)((addr_t)(addr)))
#endif

#ifndef readw
#define BL_RD_SHORT(addr)      (*((volatile uint16_t *)(uintptr_t)(addr)))
#else
#define BL_RD_SHORT(addr)      readw((void *)((addr_t)(addr)))
#endif

#ifndef writew
#define BL_WR_SHORT(addr, val) ((*(volatile uint16_t *)(uintptr_t)(addr)) = (val))
#else
#define BL_WR_SHORT(addr, val) writew(val, (void *)((addr_t)(addr)))
#endif

#ifndef readb
#define BL_RD_BYTE(addr)       (*((volatile uint8_t *)(uintptr_t)(addr)))
#else
#define BL_RD_BYTE(addr)       readb((void *)((addr_t)(addr)))
#endif

#ifndef writeb
#define BL_WR_BYTE(addr, val)  ((*(volatile uint8_t *)(uintptr_t)(addr)) = (val))
#else
#define BL_WR_BYTE(addr, val)   writeb(val, (void *)((addr_t)(addr)))
#endif

#define BL_RDWD_FRM_BYTEP(p)   ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))
#define BL_WRWD_TO_BYTEP(p, val)   \
    {                              \
        p[0] = val & 0xff;         \
        p[1] = (val >> 8) & 0xff;  \
        p[2] = (val >> 16) & 0xff; \
        p[3] = (val >> 24) & 0xff; \
    }
/**
 * @brief Register access macro
 */
#define BL_RD_REG16(addr, regname)                BL_RD_SHORT(addr + regname##_OFFSET)
#define BL_WR_REG16(addr, regname, val)           BL_WR_SHORT(addr + regname##_OFFSET, val)
#define BL_RD_REG(addr, regname)                  BL_RD_WORD(addr + regname##_OFFSET)
#define BL_WR_REG(addr, regname, val)             BL_WR_WORD(addr + regname##_OFFSET, val)
#define BL_SET_REG_BIT(val, bitname)              ((val) | (1U << bitname##_POS))
#define BL_CLR_REG_BIT(val, bitname)              ((val)&bitname##_UMSK)
#define BL_GET_REG_BITS_VAL(val, bitname)         (((val)&bitname##_MSK) >> bitname##_POS)
#define BL_SET_REG_BITS_VAL(val, bitname, bitval) (((val)&bitname##_UMSK) | ((uint32_t)(bitval) << bitname##_POS))
#define BL_IS_REG_BIT_SET(val, bitname)           (((val) & (1U << (bitname##_POS))) != 0)
#define BL_DRV_DUMMY \
    {                \
        __NOP();     \
        __NOP();     \
        __NOP();     \
        __NOP();     \
    }

/**
 * @brief Error type definition
 */
typedef enum {
    SUCCESS = 0,
    ERROR = 1,
    TIMEOUT = 2,
    INVALID = 3, /* invalid arguments */
    NORESC = 4   /* no resource or resource temperary unavailable */
} BL_Err_Type;

/**
 * @brief Functional type definition
 */
typedef enum {
    DISABLE = 0,
    ENABLE = 1,
} BL_Fun_Type;

/**
 * @brief Status type definition
 */
typedef enum {
    RESET = 0,
    SET = 1,
} BL_Sts_Type;

/**
 * @brief Mask type definition
 */
typedef enum {
    UNMASK = 0,
    MASK = 1
} BL_Mask_Type;

/**
 * @brief Logical status Type definition
 */
typedef enum {
    LOGIC_LO = 0,
    LOGIC_HI = !LOGIC_LO
} LogicalStatus;

/**
 * @brief Active status Type definition
 */
typedef enum {
    DEACTIVE = 0,
    ACTIVE = !DEACTIVE
} ActiveStatus;

/**
 *  @brief Interrupt callback function type
 */
typedef void(intCallback_Type)(void);
typedef void (*pFunc)(void);

/**
 * @brief Null Type definition
 */
#ifndef NULL
#define NULL 0
#endif

/*@} end of group COMMON_Public_Constants */


#endif /* __BL602_COMMON_H__ */
