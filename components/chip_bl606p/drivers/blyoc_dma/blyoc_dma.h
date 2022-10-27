#ifndef COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_DMA_H_
#define COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_DMA_H_

#include <bl606p_dma.h>

typedef struct _dma_callbak_func_type {
    void (*dma_callbak_func)(void *arg);
    uint32_t            argv;   //
} bl_dma_callbak_func_t;

void blyoc_dma_callback_install(DMA_ID_Type dmaId, DMA_Chan_Type dmaChan, DMA_INT_Type intType, void *cbFun, void *agrv);
void blyoc_dma0_irqhandlert(void *arg);
void blyoc_dma1_irqhandlert(void *arg);


void blyoc_dma_set_clk(uint8_t dmaId, uint8_t ch);

void blyoc_dma_enable(uint8_t dmaId);
void blyoc_dma_disable(uint8_t dmaId);
void blyoc_dma_channel_enable(uint8_t dmaId, uint8_t ch);
void blyoc_dma_channel_disable(uint8_t dmaId, uint8_t ch);
void blyoc_dma_irq_function(DMA_ID_Type dmaId, DMA_Chan_Type dmaChan, void *irq_handler, void *dev);

#endif /* COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_DMA_H_ */

