/*
 * silan_dmac.c
 */

#ifndef CONFIG_DMAC_DSP_ACQ

//#define __DEBUG__

#include "silan_dmac.h"
#include "silan_irq.h"
#include "silan_pic_regs.h"
#include "silan_types.h"
#include "silan_syscfg.h"
#include "silan_printf.h"
#include "silan_buffer.h"
#include "silan_errno.h"
#include "string.h"

DMAC_CORE_REGS *dmac_regs[2] = {(DMAC_CORE_REGS *)PCFG_ADMA_ADDR_BASE, \
							    (DMAC_CORE_REGS *)PCFG_SDMA_ADDR_BASE};

DMA_CH dma_chs_cb[2][DMAC_MAX_CHANNELS];

void silan_dmac_req_matrix_init(void)
{
	__REG32(SILAN_ADMAC_REQ0_BASE_ADDR) = \
	/* ADMAC REQ  0 */	(DMAC_REQ_SRCID_I1_I2S_RX0   <<  0) |  \
	/* ADMAC REQ  1 */	(DMAC_REQ_SRCID_I2_I2S_RX    <<  8) |  \
	/* ADMAC REQ  2 */	(DMAC_REQ_SRCID_I3_I2S_RX    << 16) |  \
	/* ADMAC REQ  3 */	(DMAC_REQ_SRCID_O1_I2S_TX0   << 24);
	__REG32(SILAN_ADMAC_REQ1_BASE_ADDR) = \
	/* ADMAC REQ  4 */	(DMAC_REQ_SRCID_O2_I2S_TX    <<  0) |  \
	/* ADMAC REQ  5 */	(DMAC_REQ_SRCID_O3_I2S_TX    <<  8) |  \
	/* ADMAC REQ  6 */	(DMAC_REQ_SRCID_NULL         << 16) |  \
	/* ADMAC REQ  7 */	(DMAC_REQ_SRCID_PCM_I2S_RX   << 24);
	__REG32(SILAN_ADMAC_REQ2_BASE_ADDR) = \
	/* ADMAC REQ  8 */	(DMAC_REQ_SRCID_PCM_I2S_TX   <<  0) |  \
	/* ADMAC REQ  9 */	(DMAC_REQ_SRCID_PDM          <<  8) |  \
	/* ADMAC REQ 10 */	(DMAC_REQ_SRCID_SPDIF_I      << 16) |  \
	/* ADMAC REQ 11 */	(DMAC_REQ_SRCID_SPDIF_O      << 24);
	__REG32(SILAN_ADMAC_REQ3_BASE_ADDR) = \
	/* ADMAC REQ 12 */	(DMAC_REQ_SRCID_IIR_BI       <<  0) |  \
	/* ADMAC REQ 13 */	(DMAC_REQ_SRCID_IIR_BO       <<  8) |  \
	/* ADMAC REQ 14 */	(DMAC_REQ_SRCID_IIR_AI       << 16) |  \
	/* ADMAC REQ 15 */	(DMAC_REQ_SRCID_IIR_AO       << 24);

	__REG32(SILAN_SDMAC_REQ0_BASE_ADDR) = \
	/* SDMAC REQ  0 */	(DMAC_REQ_SRCID_NULL         <<  0) |  \
	/* SDMAC REQ  1 */	(DMAC_REQ_SRCID_NULL         <<  8) |  \
	/* SDMAC REQ  2 */	(DMAC_REQ_SRCID_NULL         << 16) |  \
	/* SDMAC REQ  3 */	(DMAC_REQ_SRCID_NULL         << 24);
	__REG32(SILAN_SDMAC_REQ1_BASE_ADDR) = \
	/* SDMAC REQ  4 */	(DMAC_REQ_SRCID_UARTHS_RX    <<  0) |  \
	/* SDMAC REQ  5 */	(DMAC_REQ_SRCID_UARTHS_TX    <<  8) |  \
	/* SDMAC REQ  6 */	(DMAC_REQ_SRCID_SPI1_TX      << 16) |  \
	/* SDMAC REQ  7 */	(DMAC_REQ_SRCID_SPI1_RX      << 24);
	__REG32(SILAN_SDMAC_REQ2_BASE_ADDR) = \
	/* SDMAC REQ  8 */	(DMAC_REQ_SRCID_SPI2_TX      <<  0) |  \
	/* SDMAC REQ  9 */	(DMAC_REQ_SRCID_SPI2_RX      <<  8) |  \
	/* SDMAC REQ 10 */	(DMAC_REQ_SRCID_NULL         << 16) |  \
	/* SDMAC REQ 11 */	(DMAC_REQ_SRCID_NULL         << 24);
	__REG32(SILAN_SDMAC_REQ3_BASE_ADDR) = \
	/* SDMAC REQ 12 */	(DMAC_REQ_SRCID_UARTLS_RX    <<  0) |  \
	/* SDMAC REQ 13 */	(DMAC_REQ_SRCID_UARTLS_TX    <<  8) |  \
	/* SDMAC REQ 14 */	(DMAC_REQ_SRCID_PDP          << 16) |  \
	/* SDMAC REQ 15 */	(DMAC_REQ_SRCID_NULL         << 24);
}

int32_t silan_dmac_get_perid(int32_t id, uint8_t *perid)
{
	uint32_t addr, val;
	int i;

	if((id < 0) || (id > DMAC_REQ_SRCID_NULL))
		return -EINVAL;

	for(addr = SILAN_ADMAC_REQ0_BASE_ADDR; addr <= SILAN_SDMAC_REQ3_BASE_ADDR; addr += 4){
		val = *(volatile unsigned int *)addr;
		for(i = 0; i < 4; i++){
			if((val&0xff) == id){
				addr -= SILAN_ADMAC_REQ0_BASE_ADDR;
				i += addr;
				if(i < 16){
					*perid = i;
					return 0;
				}else{
					*perid = i - 16;
					return 1;
				}
			}
			val >>= 8;
		}
	}
	return -ENODEV;
}

int silan_dmac_get_ch(uint8_t which_dmac)
{
	int i;
	for(i=0;i<8;i++) {
		if(dma_chs_cb[which_dmac][i].active == 0) {
			dma_chs_cb[which_dmac][i].active = 1;
			return i;
		}
	}
	SL_LOG("DMAC-%d have no idle ch", which_dmac);
	return -ENOSPC;
}

int silan_dmac_release_ch(uint8_t which_dmac, uint8_t ch)
{
	// int i;

	if(dma_chs_cb[which_dmac][ch].active == 1) {
		dma_chs_cb[which_dmac][ch].active = 0;
		dma_chs_cb[which_dmac][ch].request = NULL;
		dma_chs_cb[which_dmac][ch].callback = NULL;
		return ENONE;
	}
	SL_LOG("DMAC-%d don't have the ch", which_dmac);
	return -EINVAL;
}

DMA_LLI *silan_dmac_get_next_lli(DMA_REQUEST *req)
{
	uint8_t which_dmac = req->which_dmac;
	uint8_t ch = req->ch;
	DMAC_CHNL_REGS *dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];
	// DMA Reg Addr -> CORE Handle Addr
	DMA_LLI *next_lli = (DMA_LLI *)ptr_dma_to_cpu((uint32_t *)(dma_ch->lli));
	return next_lli;
}

int32_t *silan_dmac_get_cur_addr(DMA_REQUEST *req, uint32_t **src, uint32_t **dst)
{
	uint8_t which_dmac = req->which_dmac;
	uint8_t ch = req->ch;
	DMAC_CHNL_REGS *dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];

	*src = ptr_dma_to_cpu((uint32_t *)dma_ch->srcaddr);
	*dst = ptr_dma_to_cpu((uint32_t *)dma_ch->destaddr);

	return ENONE;
}

static inline void dmac_irq_hdl(int id, int subid)
{
	DMAC_CORE_REGS *dmac;

	if (subid >=8 )
		return;

	dmac = dmac_regs[id];
	dmac->intTCClear = (1<<subid);
	dma_chs_cb[id][subid].request->result = 0;
	if (dma_chs_cb[id][subid].callback) {
		(*dma_chs_cb[id][subid].callback)(dma_chs_cb[id][subid].request);
	}
	dmac->intTCClear = (1<<subid);

	//sl_printf("*");
}

static inline void admac_irq_hdl(uint32_t subid)
{
	dmac_irq_hdl(ADMAC, subid);
}

static inline void sdmac_irq_hdl(uint32_t subid)
{
	dmac_irq_hdl(SDMAC, subid);
}

static uint32_t prepare_dma_cfg(DMA_REQUEST *req)
{
	uint8_t dma_mode, width, burst;
	uint32_t num_xfer, cfg;
	dma_mode = req->dma_dir;
	width = req->width;
	burst = req->burst;
	num_xfer = req->dma_lli->num_xfer;

	//SL_DBGLOG("width=%d, burst=%d, num_xfer=%d", width, burst, num_xfer);

	cfg = 0;
	switch(dma_mode){
		case DMAC_DIR_M2M:
			cfg = (1         << 31    )	| /* Enable count interrupt */ \
			      (1         << 27    )	| /* Dest Self-increase     */ \
			      (1         << 26    )	| /* Src Self-increase      */ \
			      (width     << 21    )	| \
			      (width     << 18    )	| \
			      (burst     << 12    ) | \
			      (burst     << 15    ) | \
			      (num_xfer  &  0xfff);
			break;
		case DMAC_DIR_M2P:
			cfg = (1         << 31    )	| /* Enable count interrupt */ \
			      (0         << 27    )	| /* Dest Self-increase     */ \
			      (1         << 26    )	| /* Src Self-increase      */ \
			      (DMAC_DBUS << 25    ) | \
			      (DMAC_SMEM << 24    ) | \
			      (width     << 21    ) | \
			      (width     << 18    ) | \
			      (burst     << 15    ) | \
			      (burst     << 12    ) | \
			      (num_xfer  &  0xfff);
			break;
		case DMAC_DIR_P2M:
			cfg = (1         << 31    )	| /* Enable count interrupt */ \
				  (1         << 27    )	| /* Dest Self-increase     */ \
				  (0         << 26    )	| /* Src Self-increase      */ \
				  (DMAC_DMEM << 25    ) | \
				  (DMAC_SBUS << 24    ) | \
				  (width     << 21    ) | \
				  (width     << 18    ) | \
				  (burst     << 15    ) | \
				  (burst     << 12    ) | \
				  (num_xfer  &  0xfff);
			break;
		default:
			break;
	}
	return cfg;
}

int silan_dmac_request_transfer(DMA_REQUEST *req)
{
	volatile DMAC_CHNL_REGS *dma_ch;
	DMA_LLI *lli;
	DMA_CH *cb;
	uint32_t lli_num, cfg;
	int which_dmac;
	uint8_t sreq, dreq;
	int ch, irqsid;
	int i, err;

	if ((!req) || (!req->dma_lli) || (req->ch >= 8)) {
		SL_LOG("DMA REQ Error");
		return -EINVAL;
	}

	SL_DBGLOG("DMA Request:");
	SL_DBGLOG("  REQ:   %08p", req);
	SL_DBGLOG("  LLI:   %08p", req->dma_lli);
	SL_DBGLOG("  CB:    %08p", req->callback);
	SL_DBGLOG("  WHICH: %d", req->which_dmac);
	SL_DBGLOG("  CH:    %d", req->ch);

	ch          = (int)req->ch;
	lli         = req->dma_lli;
	lli_num     = req->lli_num;
	for(i = 0; i < lli_num; i++){
		lli->raw.srcaddr = cpu_to_dma((uint32_t)lli->psrcaddr);
		lli->raw.dstaddr = cpu_to_dma((uint32_t)lli->pdstaddr);
		cfg = prepare_dma_cfg(req);
		lli->raw.TransferCtrl = cfg;
		lli->raw.nextlli = cpu_to_dma((uint32_t)lli->pnextlli);
		if(lli->pnextlli == NULL){
			lli->raw.TransferCtrl |= (1<<31);
			break;
		}
		dcache_writeback(lli, sizeof(*lli));
		lli = lli->pnextlli;
	}
	lli   = req->dma_lli;
	dreq = 0;
	sreq = 0;
	switch(req->dma_dir){
		case DMAC_DIR_M2M:
			SL_LOG("ERROR, M2M not supported.");
		case DMAC_DIR_M2P:
			which_dmac = silan_dmac_get_perid(req->srcid, &dreq);
			cfg = (0 << 22)   | /* Source Burst Enable */ \
			      (0 << 20)   | /* Destination Burst Enable */ \
			      (0 << 18)   | /* Flush DMA FIFO */ \
			      (0 << 16)   | /* Lock transmit */ \
			      (1 << 15)   | /* ITC */ \
			      (1 << 14)   | /* IE */ \
			      (DMAC_FLOWCTRL_M2P << 11)  | /* Flow control */ \
			      (dreq << 6) | /* Dest Peripheral request */ \
			      (sreq << 1) | /* Source Peripheral request */\
			      (0 << 0);     /* Channel Enable. (start dma) */
			break;
		case DMAC_DIR_P2M:
			which_dmac = silan_dmac_get_perid(req->srcid, &sreq);
			cfg = (0 << 22)   | /* Source Burst Enable */ \
			      (0 << 20)   | /* Destination Burst Enable */ \
			      (0 << 18)   | /* Flush DMA FIFO */ \
			      (0 << 16)   | /* Lock transmit */ \
			      (1 << 15)   | /* ITC */ \
			      (1 << 14)   | /* IE */ \
			      (DMAC_FLOWCTRL_P2M << 11)  | /* Flow control */ \
			      (dreq << 6) | /* Dest Peripheral request */ \
			      (sreq << 1) | /* Source Peripheral request */\
			      (0 << 0);     /* Channel Enable. (start dma) */
			break;
		default:
			which_dmac = 0;
			cfg = 0;
			SL_LOG("ERROR, use default config.");
			break;
	}
	if (dmac_regs[which_dmac]->activeChannels & (1<<ch)) {
		SL_LOG("dmac active busy");
		return -EBUSY;
	}

	cb = &dma_chs_cb[which_dmac][ch];
	if (cb->callback) {
		SL_LOG("dmac callback busy %p, %p", cb, cb->callback);
		return -EBUSY;
	} else {
		cb->request  = req;
		cb->callback = req->callback;
	}

	if (which_dmac == ADMAC) {
		irqsid  = PIC_SUBID_DMAC_CH0 + ch;
		err = silan_pic_request(PIC_IRQID_ADMAC, irqsid, (hdl_t)admac_irq_hdl);
	} else {
		irqsid  = PIC_SUBID_DMAC_CH0 + ch;
		err = silan_pic_request(PIC_IRQID_SDMAC, irqsid, (hdl_t)sdmac_irq_hdl);
	}

	if (err) {
		return -EACCES;
	}

	lli = req->dma_lli;
	dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];
	dma_ch->srcaddr  = lli->raw.srcaddr;
	dma_ch->destaddr = lli->raw.dstaddr;
	dma_ch->lli      = lli->raw.nextlli;
	dma_ch->control  = lli->raw.TransferCtrl;
	SL_DBGLOG("control(%p) = 0x%x, lli->raw.TransferCtrl = 0x%x", \
			dma_ch, dma_ch->control, lli->raw.TransferCtrl);
	dma_ch->config = cfg;    /* Channel Enable. (start dma) */
	SL_DBGLOG("Start DMA. (id=%d,ch=%d)", which_dmac, ch);
	SL_DBGLOG("control=0x%x,src/dst=%p/%p", \
			dma_ch->control, dma_ch->srcaddr, dma_ch->destaddr);
	return ENONE;
}

int silan_dmac_release_transfer(DMA_REQUEST *req)
{
	int which_dmac;
	int ch, irqsid;

	if ((!req) || (!req->dma_lli) || (req->ch >= 8)) {
		SL_LOG("DMA REQ Error");
		return -EINVAL;
	}

	SL_DBGLOG("DMA Release:");
	SL_DBGLOG("  REQ:   %08p", req);
	SL_DBGLOG("  LLI:   %08p", req->dma_lli);
	SL_DBGLOG("  CB:    %08p", req->callback);
	SL_DBGLOG("  WHICH: %d",   req->which_dmac);
	SL_DBGLOG("  CH:    %d",   req->ch);

	ch = (int)req->ch;
	which_dmac = req->which_dmac;

	silan_dmac_release_ch(which_dmac, ch);

	if (which_dmac == ADMAC) {
		irqsid  = PIC_SUBID_DMAC_CH0 + ch;
		silan_pic_free(PIC_IRQID_ADMAC, irqsid);
	} else {
		irqsid  = PIC_SUBID_DMAC_CH0 + ch;
		silan_pic_free(PIC_IRQID_SDMAC, irqsid);
	}

	return ENONE;
}


int silan_dmac_request_start(DMA_REQUEST *req)
{
	uint8_t ch, which_dmac;
	DMAC_CHNL_REGS *dma_ch;
	if(!req) {
		SL_LOG("REQ Start Error");
		return -EINVAL;
	}
	which_dmac = req->which_dmac;
	ch = req->ch;

	dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];

	dma_ch->config |= 1;
	return ENONE;
}

int silan_dmac_request_stop(DMA_REQUEST *req)
{
	uint8_t ch, which_dmac;
	DMAC_CHNL_REGS *dma_ch;
	if(!req) {
		SL_LOG("REQ Release Error");
		return -EINVAL;
	}
	which_dmac = req->which_dmac;
	ch = req->ch;

	dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];

	dma_ch->config &= ~(1);
	return ENONE;
}

void silan_dmac_init(void)
{
	uint32_t conf;
	// ?
	silan_soft_rst(SOFT_RST_ADMAC);
	silan_soft_rst(SOFT_RST_SDMAC);

	/* Enable ADMAC/SDMAC */
	conf = (0 << 2)  |  /* master 2 set to little endian. */
		   (0 << 1)  |  /* master 1 set to little endian. */
		   (1 << 0);    /* Enable DMAC. */
	dmac_regs[ADMAC]->DMACConfiguration = conf;
	dmac_regs[SDMAC]->DMACConfiguration = conf;

	memset(dma_chs_cb, 0, sizeof(DMA_CH)*2*DMAC_MAX_CHANNELS);
	//ADMAC have all 8ch, SDMAC only have 0,1,5,6
	dma_chs_cb[1][2].active = 1;
	dma_chs_cb[1][3].active = 1;
	dma_chs_cb[1][4].active = 1;
	dma_chs_cb[1][7].active = 1;
}
#endif
