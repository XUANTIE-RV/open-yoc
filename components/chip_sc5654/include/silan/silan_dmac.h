/*
 * silan_dmac.h
 */

#ifndef __SILAN_DMAC_H__
#define __SILAN_DMAC_H__

#include "silan_types.h"
#include "silan_dmac_regs.h"

#define ADMAC					0
#define SDMAC					1

#define DMAC_MAX_CHANNELS		8

#define ADMAC_MAX_CHANNELS		8
#define SDMAC_MAX_CHANNELS		4

#define ALL_DMAC_CHANNELS		(ADMAC_MAX_CHANNELS+SDMAC_MAX_CHANNELS)

typedef struct DMA_raw_lli
{
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint32_t nextlli;
	uint32_t TransferCtrl;
}
DMA_RAW_LLI, *PDMA_RAW_LLI;

typedef struct DMA_lli
{
	DMA_RAW_LLI raw;
	uint32_t			num_xfer;
	void		*psrcaddr;
	void 		*pdstaddr;
	struct DMA_lli		*pnextlli;
}
DMA_LLI, *PDMA_LLI;

typedef struct DMA_request
{
	DMA_LLI		*dma_lli;
	DMA_LLI		*dma_lli_origin;
	void		(*callback)(struct DMA_request *);
	int			result;
	uint32_t			lli_num;
	uint8_t			ch;
	uint8_t			dma_dir;
	uint8_t			width;
	uint8_t			burst;
	void		*pdev;
	void		*buf_t;
	DMA_LLI		*done_lli;
	int			buf_pre_io_bytes;
	uint8_t			srcid;
	uint8_t			which_dmac;
	uint32_t			lli_size;
}
DMA_REQUEST, *PDMA_REQUEST;

typedef struct DMA_ch
{
	uint8_t			active;
	DMA_REQUEST *request;
	void		(*callback)(DMA_REQUEST *);
	//void		*pdev;
}
DMA_CH, *PDMA_CH;

void silan_dmac_init(void);

int silan_request_dma_transfer(DMA_REQUEST *req);
DMA_LLI *silan_dmac_get_next_lli(DMA_REQUEST *req);
int32_t silan_dmac_get_perid(int32_t id, uint8_t *perid);
int silan_dmac_request_transfer(DMA_REQUEST *req);
int silan_dmac_request_start(DMA_REQUEST *req);
void silan_dmac_req_matrix_init(void);
int silan_dmac_get_ch(uint8_t which_dmac);
int silan_dmac_release_ch(uint8_t which_dmac, uint8_t ch);
int silan_dmac_request_stop(DMA_REQUEST *req);
int silan_dmac_release_transfer(DMA_REQUEST *req);
int32_t *silan_dmac_get_cur_addr(DMA_REQUEST *req, uint32_t **src, uint32_t **dst);

int *silan_dmac_remap(int *addr);

#endif
