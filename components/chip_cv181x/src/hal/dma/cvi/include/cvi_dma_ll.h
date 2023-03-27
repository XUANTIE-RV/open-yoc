/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __CVI_DMA_LL_H__
#define __CVI_DMA_LL_H__

#include <stdint.h>
#include <drv/common.h>
#include <dw_dma_ll.h>

void dma_hs_remap_init(void);

#define dma_log printf
#define dma_err printf
//#define DEBUG
#ifdef DEBUG
#define dma_dbg printf
#else
#define dma_dbg(...)
#endif

#define DW_DMA_BASE 0x4330000U
#define DW_DMA_IRQn 29U

#define DMA_CLK_EN_REG		0x03002004
#define CLK_SDMA_AXI_BIT	1

typedef uint64_t dma_addr_t;

typedef enum {
    CVI_DMA_ADDR_INC    = 0,
    CVI_DMA_ADDR_DEC,
    CVI_DMA_ADDR_CONSTANT
} cvi_dma_addr_inc_t;

typedef enum {
    CVI_DMA_DATA_WIDTH_8_BITS  = 0,
    CVI_DMA_DATA_WIDTH_16_BITS,
    CVI_DMA_DATA_WIDTH_32_BITS,
    CVI_DMA_DATA_WIDTH_64_BITS,
    CVI_DMA_DATA_WIDTH_128_BITS,
    CVI_DMA_DATA_WIDTH_512_BITS
} cvi_dma_data_width_t;

typedef enum {
    CVI_DMA_MEM2MEM     = 0,
    CVI_DMA_MEM2PERH,
    CVI_DMA_PERH2MEM,
} cvi_dma_trans_dir_t;

struct dw_dma_cfg{
	uint8_t master;
	bool hs_polarity;
	void *src_addr;
	void *dst_addr;
	uint32_t length;
	cvi_dma_addr_inc_t		  src_inc;		///< source address increment
	cvi_dma_addr_inc_t		  dst_inc;		///< destination address increment
	cvi_dma_data_width_t		src_tw;		 ///< source transfer width in byte
	cvi_dma_data_width_t		dst_tw;		 ///< destination transfer width in byte
	cvi_dma_trans_dir_t		 trans_dir;	  ///< transfer direction
	uint16_t					handshake;	  ///< handshake id
	uint16_t					group_len;	  ///< group transaction length (unit: bytes)
};

typedef struct dw_dma_channel {
	void (*callback)(uint8_t ch_id, void *args);
	void *args;
	uint8_t ctrl_id;
	uint8_t ch_id;
	uint8_t ch_mask;
	void *regs;
	struct dw_dma_cfg cfg;
	dlist_t queue_list;
	dlist_t active_list;
}dw_dma_channel_t;

typedef struct dw_dma {
	uint8_t idx;	//current only have one dma controller
	dw_dma_regs_t *regs;
	uint32_t alloc_status;
	uint32_t nr_masters;
	uint32_t nr_channels;
	uint32_t block_ts;
	uint32_t irq_num;
	int clk_enable_count;	//When clk_enable_count==0, turn off dma/clk.
	uint8_t data_width[DW_DMA_MAX_NR_MASTERS];
	dw_dma_channel_t chans[DW_DMA_MAX_NR_CHANNELS];
}dw_dma_t;

#define dma_writeq_native(__value,__reg)	\
	(*(volatile uint64_t *)(__reg) = (__value));

#define dma_writeq(dw, name, val) \
		dma_writeq_native((val), &(_dw_regs(dw)->name))

#define channel_readq(dwc, name) \
		dma_readq_native(&(_dwc_regs(dwc)->name))

#define channel_writeq(dwc, name, val) \
		dma_writeq_native((val), &(_dwc_regs(dwc)->name))

#ifdef readq
#define dma_readq_native	readq
#else
#define dma_readq_native(__reg)	(*(volatile uint64_t *)(__reg))
#endif

#define dma_readq(dw, name) \
		dma_readq_native(&(_dw_regs(dw)->name))

#define dma_set_bit(dw, name, mask) \
		dma_writeq_native((mask) | dma_readq_native(&(_dw_regs(dw)->name)), \
			 &(_dw_regs(dw)->name))

#define  dma_clear_bit(dw, name, mask) \
		dma_writeq_native((~mask & dma_readq_native(&(_dw_regs(dw)->name))) \
			, &(_dw_regs(dw)->name))

#define CH_REG_BASE(dw, i) &(_dw_regs(dw)->CHAN[i])

struct dw_desc {
	/* FIRST values the hardware uses */
	struct dw_lli lli;
	void * raw_addr;
	struct dw_desc *next;

#define lli_set(d, reg, v)		((d)->lli.reg |= (v))
#define lli_clear(d, reg, v)	((d)->lli.reg &= ~(v))
#define lli_read(d, reg)		((d)->lli.reg)
#define lli_write(d, reg, v)	((d)->lli.reg = (v))

	dlist_t list;
};

#if 1
static inline uint32_t mmio_read_32(uintptr_t addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	*(volatile uint32_t *)addr = value;
}
#endif
#ifdef __riscv
#define SDMA_DMA_INT_MUX 0x03000298
#define SDMA_DMA_INT_MUX_C906B (0xff << 10)

static inline uint32_t sdma_dma_int_mux_get(void)
{
	return *(volatile uint32_t *)SDMA_DMA_INT_MUX;
}

#endif

static inline void barrier(void)
{
	asm volatile("": : :"memory");
}

static inline unsigned long __ffs(uint64_t word)
{
	return __builtin_ctzl(word);
}

static inline int __fls(int x)
{
	return x ? sizeof(x) * 8 - __builtin_clz(x) : 0;
}

static inline struct dw_dma_regs * _dw_regs(dw_dma_t *dw)
{
	return dw->regs;
}

static inline struct dw_dma_chan_regs * _dwc_regs(dw_dma_channel_t *dwc)
{
	return dwc->regs;
}

static inline int is_slave(struct dw_dma_cfg *cfg)
{
	return (cfg->trans_dir == CVI_DMA_MEM2PERH || cfg->trans_dir == CVI_DMA_PERH2MEM);
}

static inline struct dw_desc *get_first_desc(dlist_t *list)
{
	return dlist_first_entry(list, struct dw_desc, list);
}

static inline void __cvi_list_splice(const dlist_t *list,
				 dlist_t *prev,
				 dlist_t *next)
{
	dlist_t *first = list->next;
	dlist_t *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * cvi_list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void cvi_list_splice(const dlist_t *list,
				dlist_t *head)
{
	if (!dlist_empty(list))
		__cvi_list_splice(list, head, head->next);
}

/**
 * cvi_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void cvi_list_splice_init(dlist_t *list,
					dlist_t *head)
{
	if (!dlist_empty(list)) {
		__cvi_list_splice(list, head, head->next);
		INIT_AOS_DLIST_HEAD(list);
	}
}

static inline void dlist_move(dlist_t *src, dlist_t *dst)
{
	dlist_t *tmp = src->next;
	dlist_del(tmp);
	dlist_add_tail(tmp, dst);
}

#endif
