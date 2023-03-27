/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __ASM_BARRIER_H
#define __ASM_BARRIER_H

#ifndef __ASSEMBLY__

#define nop()		__asm__ __volatile__ ("nop")

#define RISCV_FENCE(p, s) \
	__asm__ __volatile__ ("fence " #p "," #s : : : "memory")

/* These barriers need to enforce ordering on both devices or memory. */
#define mb()		RISCV_FENCE(iorw,iorw)
#define rmb()		RISCV_FENCE(ir,ir)
#define wmb()		RISCV_FENCE(ow,ow)

#define dma_rmb()	dmb(oshld)
#define dma_wmb()	dmb(oshst)

/* These barriers do not need to enforce ordering on devices, just memory. */
#define __smp_mb()	RISCV_FENCE(rw,rw)
#define __smp_rmb()	RISCV_FENCE(r,r)
#define __smp_wmb()	RISCV_FENCE(w,w)

#if 0
#define __smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	RISCV_FENCE(r,rw);						\
	___p1;								\
})

#define __smp_store_release(p, v)					\
do {									\
	compiletime_assert_atomic_type(*p);				\
	RISCV_FENCE(rw,w);						\
	WRITE_ONCE(*p, v);						\
} while (0)

#define smp_mb__after_spinlock()	RISCV_FENCE(iorw,iorw)

#include <asm-generic/barrier.h>
#endif
#endif	/* __ASSEMBLY__ */

#endif	/* __ASM_BARRIER_H */
