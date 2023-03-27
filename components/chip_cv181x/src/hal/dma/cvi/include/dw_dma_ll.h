/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __DW_DMA_LL_H__
#define __DW_DMA_LL_H__

#define DW_DMAC_CH_ABORT_EN_OFFSET	40
#define DW_DMAC_CH_ABORT_OFFSET		32
#define DW_DMAC_CH_PAUSE_EN_OFFSET	24
#define DW_DMAC_CH_PAUSE_OFFSET		16
#define DW_DMAC_CH_EN_WE_OFFSET		8

#define DW_DMA_MAX_NR_MASTERS	2
#define DW_DMA_MAX_DATA_WIDTH	4
#define DW_DMA_MAX_NR_CHANNELS	8
#define DW_DMA_MAX_NR_REQUESTS	16
#define DW_DMA_CHAN_MASK	((1 << DW_DMA_MAX_NR_CHANNELS) - 1)
#define DW_DWC_MAX_BLOCK_TS	1024

#define DW_CFG_DMA_EN		(1 << 0)
#define DW_CFG_DMA_INT_EN	(1 << 1)

/*
 * Redefine this macro to handle differences between 32- and 64-bit
 * addressing, big vs. little endian, etc.
 */
#define DW_REG(name)	uint64_t name

/* Hardware register definitions. */
typedef struct dw_dma_chan_regs {
	DW_REG(SAR);		/* Source Address Register */
	DW_REG(DAR);		/* Destination Address Register */
	DW_REG(BLOCK_TS);		/* Block transfer size */
	DW_REG(CTL);	/* Contorl dma transer */
	DW_REG(CFG);	/* Configure dma transer */
	DW_REG(LLP);		/* Linked List Pointer */
	DW_REG(STATUS);		/* Status of dma transer */
	DW_REG(SWHSSRCREG);		/* SW handshake source register */
	DW_REG(SWHSDSTREG);	/* SW handshake Destination register */
	DW_REG(BLK_TFR_RESUMEREQREG);	/* Block transfer resume request */
	DW_REG(AXI_IDREG);	/* AXI ID register */
	DW_REG(AXI_QOSREG);	/* AXI QoS register */
	DW_REG(SSTAT);
	DW_REG(DSTAT);
	DW_REG(SSTATAR);
	DW_REG(DSTATAR);
	DW_REG(INTSTATUS_ENABLEREG);
	DW_REG(INTSTATUS);
	DW_REG(INTSIGNAL_ENABLEREG);
	DW_REG(INTCLEARREG);
	uint64_t __reserved1[12];
}dw_dma_chan_regs_t;

typedef struct dw_dma_regs {
	/* Common Registers */
	DW_REG(ID);
	DW_REG(COMPVER);
	DW_REG(CFG);
	DW_REG(CH_EN);
	uint64_t __reserved0[2];

	DW_REG(INTSTATUS);
	DW_REG(COMM_INTCLEAR);
	DW_REG(COMM_INTSTATUS_EN);
	DW_REG(COMM_INTSIGNAL_EN);
	DW_REG(COMM_INTSTATUS);
	DW_REG(RESET);

	uint64_t __reserved1[20];
	/* channel regs */
	struct dw_dma_chan_regs	CHAN[DW_DMA_MAX_NR_CHANNELS];
}dw_dma_regs_t;

#define DWC_CTL_SMS(n)		((n & 0x1)<<0)	/* src master select */
#define DWC_CTL_DMS(n)		((n & 0x1)<<2)	/* dst master select */
#define DWC_CTL_SRC_INC		(0<<4)	/* Source Address Increment update*/
#define DWC_CTL_SRC_FIX		(1<<4)	/* Source Address Increment not*/
#define DWC_CTL_DST_INC		(0<<6)	/* Destination Address Increment update*/
#define DWC_CTL_DST_FIX		(1<<6)	/* Destination Address Increment not*/
#define DWC_CTL_SRC_WIDTH(n)	((n & 0x7)<<8)	/* Source Transfer Width */
#define DWC_CTL_DST_WIDTH(n)	((n & 0x7)<<11)	/* Destination Transfer Width */
#define DWC_CTL_SRC_MSIZE(n)	((n & 0xf)<<14)	/* SRC Burst Transaction Length, data items */
#define DWC_CTL_DST_MSIZE(n)	((n & 0xf)<<18)	/* DST Burst Transaction Length, data items */
#define DWC_CTL_AR_CACHE(n)	((n & 0xf)<<22)
#define DWC_CTL_AW_CACHE(n)	((n & 0xf)<<26)
#define DWC_CTL_N_LAST_W_EN	(1<<30)	/* last write posted write enable/disable*/
#define DWC_CTL_N_LAST_W_DIS	(0<<30)	/* last write posted wrtie enable/disable*/
#define DWC_CTL_ARLEN_DIS	(0ULL<<38) /* Source Burst Length Disable */
#define DWC_CTL_ARLEN_EN	(1ULL<<38) /* Source Burst Length Enable */
#define DWC_CTL_ARLEN(n)	((n & 0xffULL)<<39)
#define DWC_CTL_AWLEN_DIS	(0ULL<<47) /* DST Burst Length Enable */
#define DWC_CTL_AWLEN_EN	(1ULL<<47)
#define DWC_CTL_AWLEN(n)	((n & 0xffULL)<<48)
#define DWC_CTL_SRC_STA_DIS	(0ULL<<56)
#define DWC_CTL_SRC_STA_EN	(1ULL<<56)
#define DWC_CTL_DST_STA_DIS	(0ULL<<57)
#define DWC_CTL_DST_STA_EN	(1ULL<<57)
#define DWC_CTL_IOC_BLT_DIS	(0ULL<<58)	/* Interrupt On completion of Block Transfer */
#define DWC_CTL_IOC_BLT_EN	(1ULL<<58)
#define DWC_CTL_SHADOWREG_OR_LLI_LAST	(1ULL<<62)	/* Last Shadow Register/Linked List Item */
#define DWC_CTL_SHADOWREG_OR_LLI_VALID	(1ULL<<63)	/* Shadow Register content/Linked List Item valid */

typedef uint64_t __dw64;
typedef uint32_t __dw32;

/* LLI == Linked List Item; a.k.a. DMA block descriptor */
struct dw_lli {
	/* values that are not changed by hardware */
	__dw64 sar;
	__dw64 dar;
	__dw64 block_ts;
	__dw64 llp;	/* chain to next lli */
	__dw64 ctl;

	/* sstat and dstat can snapshot peripheral register state.
	 * silicon config may discard either or both...
	 */
	__dw32 sstat;
	__dw32 dstat;
	__dw64 llp_status;
	__dw64 reserved;
};

/* Bitfields in CTL */
#define DWC_CTL_SMS(n)		((n & 0x1)<<0)	/* src master select */
#define DWC_CTL_DMS(n)		((n & 0x1)<<2)	/* dst master select */
#define DWC_CTL_SRC_INC		(0<<4)	/* Source Address Increment update*/
#define DWC_CTL_SRC_FIX		(1<<4)	/* Source Address Increment not*/
#define DWC_CTL_DST_INC		(0<<6)	/* Destination Address Increment update*/
#define DWC_CTL_DST_FIX		(1<<6)	/* Destination Address Increment not*/
#define DWC_CTL_SRC_WIDTH(n)	((n & 0x7)<<8)	/* Source Transfer Width */
#define DWC_CTL_DST_WIDTH(n)	((n & 0x7)<<11)	/* Destination Transfer Width */
#define DWC_CTL_SRC_MSIZE(n)	((n & 0xf)<<14)	/* SRC Burst Transaction Length, data items */
#define DWC_CTL_DST_MSIZE(n)	((n & 0xf)<<18)/* DST Burst Transaction Length, data items */
#define DWC_CTL_AR_CACHE(n)	((n & 0xf)<<22)
#define DWC_CTL_AW_CACHE(n)	((n & 0xf)<<26)
#define DWC_CTL_N_LAST_W_EN	(1<<30)	/* last write posted write enable/disable*/
#define DWC_CTL_N_LAST_W_DIS	(0<<30)	/* last write posted wrtie enable/disable*/
#define DWC_CTL_ARLEN_DIS	(0ULL<<38) /* Source Burst Length Disable */
#define DWC_CTL_ARLEN_EN	(1ULL<<38) /* Source Burst Length Enable */
#define DWC_CTL_ARLEN(n)	((n & 0xffULL)<<39)
#define DWC_CTL_AWLEN_DIS	(0ULL<<47) /* DST Burst Length Enable */
#define DWC_CTL_AWLEN_EN	(1ULL<<47)
#define DWC_CTL_AWLEN(n)	((n & 0xffULL)<<48)
#define DWC_CTL_SRC_STA_DIS	(0ULL<<56)
#define DWC_CTL_SRC_STA_EN	(1ULL<<56)
#define DWC_CTL_DST_STA_DIS	(0ULL<<57)
#define DWC_CTL_DST_STA_EN	(1ULL<<57)
#define DWC_CTL_IOC_BLT_DIS	(0ULL<<58)	/* Interrupt On completion of Block Transfer */
#define DWC_CTL_IOC_BLT_EN	(1ULL<<58)
#define DWC_CTL_SHADOWREG_OR_LLI_LAST	(1ULL<<62)	/* Last Shadow Register/Linked List Item */
#define DWC_CTL_SHADOWREG_OR_LLI_VALID	(1ULL<<63)	/* Shadow Register content/Linked List Item valid */

#define DWC_BLOCK_TS_MASK	0x3FFFFF

#define DWC_CFG_SRC_MULTBLK_TYPE(x)	((x & 0x7) << 0)
#define DWC_CFG_DST_MULTBLK_TYPE(x)	((x & 0x7) << 2)
#define DWC_CFG_TT_FC(x)		((x & 0x7ULL) << 32)
#define DWC_CFG_HS_SEL_SRC_HW	(0ULL<<35)
#define DWC_CFG_HS_SEL_SRC_SW	(1ULL<<35)
#define DWC_CFG_HS_SEL_DST_HW	(0ULL<<36)
#define DWC_CFG_HS_SEL_DST_SW	(1ULL<<36)
#define DWC_CFG_SRC_HWHS_POL_H	(0ULL << 37)
#define DWC_CFG_SRC_HWHS_POL_L	(1ULL << 37)
#define DWC_CFG_DST_HWHS_POL_H	(0ULL << 38)
#define DWC_CFG_DST_HWHS_POL_L	(1ULL << 38)
#define DWC_CFG_SRC_PER(x)	((x & 0xffULL) << 39)
#define DWC_CFG_DST_PER(x)	((x & 0xffULL) << 44)

#define DWC_CFG_CH_PRIOR_MASK	(0x7ULL << 49)	/* priority mask */
#define DWC_CFG_CH_PRIOR(x)	(((x) & 0x7ULL) << 49)	/* priority */
#define DWC_CFG_SRC_OSR_LMT(x)	(((x) & 0xfULL) << 55) /* max request x + 1 <= 16 */
#define DWC_CFG_DST_OSR_LMT(x)	(((x) & 0xfULL) << 59)

#define DWC_CFG_GET_TT_FC(x)		((x >> 32ULL) & 0x7)

enum dwc_multblk_type {
	CONTIGUOUS,
	RELOAD,
	SHADOW_REGISTER,
	LINK_LIST
};

/* flow controller */
enum dw_dma_fc {
	DW_DMA_FC_D_M2M, /* FlowControl is DMAC, mem to mem */
	DW_DMA_FC_D_M2P, /* FlowControl is DMAC, mem to perip */
	DW_DMA_FC_D_P2M,
	DW_DMA_FC_D_P2P,
	DW_DMA_FC_SP_P2M, /* FlowControl is Source periph, periph to mem */
	DW_DMA_FC_SP_P2P,
	DW_DMA_FC_DP_M2P, /* FlowControl is Dst periph, periph to mem */
	DW_DMA_FC_DP_P2P,
};

/* bursts size */
enum dw_dma_msize {
	DW_DMA_MSIZE_1,
	DW_DMA_MSIZE_4,
	DW_DMA_MSIZE_8,
	DW_DMA_MSIZE_16,
	DW_DMA_MSIZE_32,
	DW_DMA_MSIZE_64,
	DW_DMA_MSIZE_128,
	DW_DMA_MSIZE_256,
};

#define DWC_CH_INTSTA_DMA_TFR_DONE	(1<<1)
#define DWC_CH_INTSTA_BLOCK_TFR_DONE	(1<<0)
#define DWC_CH_INTSTA_SRC_TFR_COMP_EN	(1<<3)
#define DWC_CH_INTSTA_DST_TFR_COMP_EN	(1<<4)
#define DWC_CH_INTSTA_SRC_DEC_ERR_EN	(1<<5)
#define DWC_CH_INTSTA_DST_DEC_ERR_EN	(1<<6)
#define DWC_CH_INTSTA_SRC_SLV_ERR_EN	(1<<7)
#define DWC_CH_INTSTA_SRC_DST_ERR_EN	(1<<8)
#define DWC_CH_INTSTA_LLI_RD_DEV_ERR_EN	(1<<9)
#define DWC_CH_INTSTA_LLI_WD_DEV_ERR_EN	(1<<10)
#define DWC_CH_INTSTA_LLI_RD_SLV_ERR_EN	(1<<11)
#define DWC_CH_INTSTA_LLI_WD_SLV_ERR_EN	(1<<12)
#define DWC_CH_INTSTA_SHDW_LLI_INVALID_ERR_EN	(1<<13)
#define DWC_CH_INTSTA_SLVIF_MULTBLK_TYPE_ERR_EN	(1<<14)
#define DWC_CH_INTSTA_SLVIF_DEC_ERR_EN	(1<<16)
#define DWC_CH_INTSTA_CH_ABORTED_EN	(1<<31)
#define DWC_CH_SUSPENDED		(1 << 29)

#define DWC_CH_INTSIG_BLK_TRA_DONE	(1 << 0)
#define DWC_CH_INTSIG_DMA_TRA_DONE	(1 << 1)
#define DWC_CH_INTSIG_SRC_COMP_DONE	(1 << 3)
#define DWC_CH_INTSIG_DST_COMP_DONE	(1 << 4)

#endif