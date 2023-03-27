/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __ASM_IO_H
#define __ASM_IO_H

#include "asm/barrier.h"
#include "rtos_types.h"
/*
 * Generic IO read/write.  These perform native-endian accesses.
 */
#ifdef __CHECKER__
#define __iomem __attribute__((noderef, address_space(2)))
#else
#define __iomem
#endif


/* description:Generic IO read/write.  These perform native-endian accesses. */
#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 cvi_val, volatile void __iomem *cvi_addr)
{
	asm volatile("sb %0, 0(%1)" : : "r" (cvi_val), "r" (cvi_addr));
}

#define __raw_writew __raw_writew
static inline void __raw_writew(u16 cvi_val, volatile void __iomem *cvi_addr)
{
	asm volatile("sh %0, 0(%1)" : : "r" (cvi_val), "r" (cvi_addr));
}

#define __raw_writel __raw_writel
static inline void __raw_writel(u32 cvi_val, volatile void __iomem *cvi_addr)
{
	asm volatile("sw %0, 0(%1)" : : "r" (cvi_val), "r" (cvi_addr));
}

#define __raw_writeq __raw_writeq
static inline void __raw_writeq(u64 cvi_val, volatile void __iomem *cvi_addr)
{
	asm volatile("sd %0, 0(%1)" : : "r" (cvi_val), "r" (cvi_addr));
}

#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *cvi_addr)
{
	u8 cvi_val;

	asm volatile("lb %0, 0(%1)" : "=r" (cvi_val) : "r" (cvi_addr));
	return cvi_val;
}

#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *cvi_addr)
{
	u16 cvi_val;

	asm volatile("lh %0, 0(%1)" : "=r" (cvi_val) : "r" (cvi_addr));
	return cvi_val;
}

#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *cvi_addr)
{
	u32 cvi_val;

	asm volatile("lw %0, 0(%1)" : "=r" (cvi_val) : "r" (cvi_addr));
	return cvi_val;
}

#define __raw_readq __raw_readq
static inline u64 __raw_readq(const volatile void __iomem *cvi_addr)
{
	u64 cvi_val;

	asm volatile("ld %0, 0(%1)" : "=r" (cvi_val) : "r" (cvi_addr));
	return cvi_val;
}

/* IO barriers */
#define __iormb()		rmb()
#define __iowmb()		wmb()

#define mmiowb()		do { } while (0)


#define cpu_to_le64(cvi_x) ((__le64)(__u64)(cvi_x))
#define le64_to_cpu(cvi_x) ((__u64)(__le64)(cvi_x))
#define cpu_to_le32(cvi_x) ((__le32)(__u32)(cvi_x))
#define le32_to_cpu(cvi_x) ((__u32)(__le32)(cvi_x))
#define cpu_to_le16(cvi_x) ((__le16)(__u16)(cvi_x))
#define le16_to_cpu(cvi_x) ((__u16)(__le16)(cvi_x))

/*
 * Relaxed I/O memory access primitives. These follow the Device memory
 * ordering rules but do not guarantee any ordering relative to Normal memory
 * accesses.
 */

#define readb_relaxed(cvi_c)    ({ u8  __r = __raw_readb(cvi_c); __r; })
#define readw_relaxed(cvi_c)    ({ u16 __r = le16_to_cpu((__le16)__raw_readw(cvi_c)); __r; })
#define readl_relaxed(cvi_c)    ({ u32 __r = le32_to_cpu((__le32)__raw_readl(cvi_c)); __r; })
#define readq_relaxed(cvi_c)    ({ u64 __r = le64_to_cpu((__le64)__raw_readq(cvi_c)); __r; })

#define writeb_relaxed(cvi_v, cvi_c) ((void)__raw_writeb((cvi_v), (cvi_c)))
#define writew_relaxed(cvi_v, cvi_c) ((void)__raw_writew((u16)cpu_to_le16(cvi_v), (cvi_c)))
#define writel_relaxed(cvi_v, cvi_c) ((void)__raw_writel((u32)cpu_to_le32(cvi_v), (cvi_c)))
#define writeq_relaxed(cvi_v, cvi_c) ((void)__raw_writeq((u64)cpu_to_le64(cvi_v), (cvi_c)))

/*
 * I/O memory access primitives. Reads are ordered relative to any
 * following Normal memory access. Writes are ordered relative to any prior
 * Normal memory access.
 */
#define readb(cvi_c)		({ u8  __v = readb_relaxed(cvi_c); __iormb(); __v; })
#define readw(cvi_c)		({ u16 __v = readw_relaxed(cvi_c); __iormb(); __v; })
#define readl(cvi_c)		({ u32 __v = readl_relaxed(cvi_c); __iormb(); __v; })
#define readq(cvi_c)		({ u64 __v = readq_relaxed(cvi_c); __iormb(); __v; })

#define writeb(cvi_v, cvi_c)		({ __iowmb(); writeb_relaxed((cvi_v), (cvi_c)); })
#define writew(cvi_v, cvi_c)		({ __iowmb(); writew_relaxed((cvi_v), (cvi_c)); })
#define writel(cvi_v, cvi_c)		({ __iowmb(); writel_relaxed((cvi_v), (cvi_c)); })
#define writeq(cvi_v, cvi_c)		({ __iowmb(); writeq_relaxed((cvi_v), (cvi_c)); })

#define iowrite8 writeb
#define iowrite16 writew
#define iowrite32 writel
#define iowrite64 writeq

#define ioread8 readb
#define ioread16 readw
#define ioread32 readl
#define ioread64 readq

#endif	/* __ASM_IO_H */
