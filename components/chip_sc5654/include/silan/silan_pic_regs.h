/*
 * silan_irq_regs.h
 */

#ifndef __SILAN_IRQ_REGISTERS_SPEC_H__
#define __SILAN_IRQ_REGISTERS_SPEC_H__

#include "ap1508.h"
#include "silan_types.h"

#define INTR_STS_RAW(id)        __REG32(SCFG_APB_INTR_ADDR_BASE+0x000+id*4)
#define INTR_STS_MSK_RISC(id)   __REG32(SCFG_APB_INTR_ADDR_BASE+0x100+id*4)
#define INTR_CTR_MSK_RISC(id)   __REG32(SCFG_APB_INTR_ADDR_BASE+0x200+id*4)
#define INTR_STS_MSK_DSP(id)    __REG32(SCFG_APB_INTR_ADDR_BASE+0x300+id*4)
#define INTR_CTR_MSK_DSP(id)    __REG32(SCFG_APB_INTR_ADDR_BASE+0x400+id*4)
#define INTR_STS_MSK_M0(id)     __REG32(SCFG_APB_INTR_ADDR_BASE+0x500+id*4)
#define INTR_CTR_MSK_M0(id)     __REG32(SCFG_APB_INTR_ADDR_BASE+0x600+id*4)

#if defined(__CC_ARM)
#define INTR_STS_MSK(id)        INTR_STS_MSK_M0(id)
#define INTR_CTR_MSK(id)        INTR_CTR_MSK_M0(id)
#endif

#if defined(__CSKY__)
#define INTR_STS_MSK(id)        INTR_STS_MSK_RISC(id)
#define INTR_CTR_MSK(id)        INTR_CTR_MSK_RISC(id)
#endif

#if defined(__XCC__)
#define INTR_STS_MSK(id)        INTR_STS_MSK_DSP(id)
#define INTR_CTR_MSK(id)        INTR_CTR_MSK_DSP(id)
#endif

#endif  //__SILAN_IRQ_REGISTERS_SPEC_H__
