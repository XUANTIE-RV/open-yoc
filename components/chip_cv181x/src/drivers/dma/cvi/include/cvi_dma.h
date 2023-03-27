#ifndef __CVI_DMA_H__
#define __CVI_DMA_H__

#include <hal_dma.h>

typedef struct {
    uint8_t ctrl_idx;
    uint8_t ch_idx;
} cvi_dma_ch_desc_t;

int cvi_dma_init(dw_dma_t *dma, int8_t ctrl_id, unsigned long reg_base, uint32_t irq_num);
void cvi_dma_uninit(dw_dma_t *dma);
int cvi_dma_ch_alloc(uint8_t ctrl_idx, uint8_t ch_idx);
void cvi_dma_ch_stop(int ctrl_idx, int ch_idx);
void cvi_dma_ch_free(int ctrl_idx, int ch_idx);
void cvi_dma_ch_pause(int ctrl_idx, int ch_idx);
void cvi_dma_ch_resume(int ctrl_idx, int ch_idx);
int cvi_dma_ch_config(int ctrl_idx, int ch_idx, struct dw_dma_cfg *dw_cfg);
void cvi_dma_ch_start(int ctrl_idx, int ch_idx, void *srcaddr, void *dstaddr, uint32_t length);
uint32_t cvi_dma_get_inited_num();
uint32_t cvi_get_alloc_status_irqsave(uint8_t ctrl_idx, uint32_t *alloc_status);
void cvi_release_alloc_status_irqrestore(uint32_t irq_flags);
int cvi_dma_lock_ch(uint8_t ctrl_idx, uint8_t ch_idx);
void _cvi_dma_ch_alloc(uint8_t ctrl_idx, uint8_t ch_idx);
int cvi_set_ch_hw_param(dw_dma_channel_t *dwc, uint8_t master, uint8_t hs_polarity);
void dma_show_info();
void dma_show_ch_info_all();
void dma_show_ch_info(int ch_id);
void cvi_dma_ch_attach_callback(int ctrl_idx, int ch_idx, void *callback, void *args);
void cvi_dma_ch_detach_callback(int ctrl_idx, int ch_idx);
int cvi_get_ch_info_random(cvi_dma_ch_desc_t *ch_info);

#endif