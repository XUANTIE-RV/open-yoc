/*
 * silan_cxc_regs.h
 *
 * Modify Date: 2016-3-21 10:52
 * MaintainedBy: qinling <qinling@silan.com.cn>
 */

#ifndef __SILAN_CXC_REGISTERS_H__
#define __SILAN_CXC_REGISTERS_H__

#include "silan_types.h"
#include "ap1508.h"

#define CXC_MUTEX(x)                __sREG32(SCFG_APB_CXC_ADDR_BASE, x*sizeof(uint32_t))
#define CXC_MBOX_BASEADDR           (SCFG_APB_CXC_ADDR_BASE+0x400)
#define CXC_MBOX_LMTADDR            (SCFG_APB_CXC_ADDR_BASE+0x47C)
#define CXC_MBOX(x)                 __REG32(CXC_MBOX_BASEADDR + x*sizeof(uint32_t))
#define CXC_RAW_INT_STATUS          __REG32(SCFG_APB_CXC_ADDR_BASE+0x800)
#define CXC_RAW_INT_SET             __REG32(SCFG_APB_CXC_ADDR_BASE+0x804)
#define CXC_RAW_INT_CLR             __REG32(SCFG_APB_CXC_ADDR_BASE+0x808)
#define CXC_RISC_INT_MASK           __REG32(SCFG_APB_CXC_ADDR_BASE+0x80c)
#define CXC_DSP_INT_MASK            __REG32(SCFG_APB_CXC_ADDR_BASE+0x810)
#define CXC_MCU_INT_MASK            __REG32(SCFG_APB_CXC_ADDR_BASE+0x814)
#define CXC_RISC_INT_STATUS         __REG32(SCFG_APB_CXC_ADDR_BASE+0x818)
#define CXC_DSP_INT_STATUS          __REG32(SCFG_APB_CXC_ADDR_BASE+0x81c)
#define CXC_MCU_INT_STATUS          __REG32(SCFG_APB_CXC_ADDR_BASE+0x820)

#if defined(__CC_ARM)
#define CXC_CORE_HOST               SILAN_CXC_CORE_M0
#define CXC_INT_MASK                CXC_MCU_INT_MASK
#define CXC_INT_STATUS              CXC_MCU_INT_STATUS
#endif

#if defined(__CSKY__)
#define CXC_CORE_HOST               SILAN_CXC_CORE_RISC
#define CXC_INT_MASK                CXC_RISC_INT_MASK
#define CXC_INT_STATUS              CXC_RISC_INT_STATUS
#endif

#if defined(__XCC__)
#define CXC_CORE_HOST               SILAN_CXC_CORE_DSP
#define CXC_INT_MASK                CXC_DSP_INT_MASK
#define CXC_INT_STATUS              CXC_DSP_INT_STATUS
#endif

#endif  //__SILAN_CXC_REGISTERS_H__

