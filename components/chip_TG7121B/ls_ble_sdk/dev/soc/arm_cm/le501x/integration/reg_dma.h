#ifndef REG_DMA_H_
#define REG_DMA_H_
#include "reg_dma_type.h"

#define DMA1 ((reg_dma_t *)(0x40011000))            // DMA controller configuration
#define DMA1INT ((reg_dma_int_t *)(0x40012000))     // DMA interrupt configuration
#define DMA1CHSEL ((reg_dma_chsel_t *)(0x40012100)) // DMA channel selection configuration

#endif //(REG_DMA_H_)
