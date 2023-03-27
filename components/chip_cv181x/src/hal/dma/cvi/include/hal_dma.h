/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/
#ifndef __HAL_DMA_H__
#define __HAL_DMA_H__

#include <stdint.h>
#include <cvi_dma_ll.h>

void hal_dma_dwc_clk_set(int enable);
void hal_dma_off(dw_dma_t *dma);
void hal_dma_on(dw_dma_t *dma);
void hal_dma_write_cfg(dw_dma_channel_t *dwc, uint64_t cfg);
void hal_dma_enable_irq(dw_dma_channel_t *dwc, uint64_t int_status_reg);
void hal_dma_write_llp(dw_dma_channel_t *dwc, uint64_t llp);
uint64_t hal_dma_get_ch_en_status(struct dw_dma *dma);
void hal_dma_ch_on(struct dw_dma *dma, uint8_t ch_mask);
void hal_dma_ch_off(struct dw_dma *dma, uint8_t ch_mask);
void hal_dma_ch_pause(struct dw_dma *dma, uint8_t ch_mask);
void hal_dma_ch_resume(struct dw_dma *dma, uint8_t ch_mask);
uint64_t hal_dma_get_intstatus(dw_dma_t *dma);
void hal_dma_clear_comm_intstatus(dw_dma_t *dma);
uint64_t hal_dma_dwc_read_clear_intstatus(dw_dma_channel_t *dwc);
void hal_dma_dwc_clear_intstatus(dw_dma_channel_t *dwc);
void hal_dma_turn_off_chans(dw_dma_t *dma);
void hal_dma_reset(dw_dma_t *dma);
void hal_sdma_dma_int_mux_set_c906b(void);
void hal_print_dma_reg(struct dw_dma *dw);
void hal_print_ch_info(struct dw_dma *dw, int ch_id);

#endif