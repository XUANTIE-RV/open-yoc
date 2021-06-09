/**
  ******************************************************************************
  * @file    reg_pdm.c
  * @author  Linkedsemi Application Team
  * @brief   PDM register Header file
  ******************************************************************************
  */

#ifndef REG_PDM_TYPE_H_
#define REG_PDM_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t CR; //0x0
    volatile uint32_t CFG; //0x4
    volatile uint32_t DATA0; //0x8
    volatile uint32_t DATA1; //0xc
    volatile uint32_t IER; //0x10
    volatile uint32_t IDR; //0x14
    volatile uint32_t IVS; //0x18
    volatile uint32_t RIF; //0x1c
    volatile uint32_t IFM; //0x20
    volatile uint32_t ICR; //0x24
    volatile uint32_t RESERVED0[6];
    volatile uint32_t COEF0; //0x40
} reg_pdm_t;

typedef struct
{
    uint32_t pdm_fir_coef[64];
} PDM_COEF_TypeDef;

enum PDM_REG_CR_FIELD
{
    PDM_CR_EN_MASK = (int)0x1,
    PDM_CR_EN_POS = 0,
    PDM_CR_EN = PDM_CR_EN_MASK,
    PDM_CR_DMAEN_MASK = (int)0x6,
    PDM_CR_DMAEN_POS = 1,
    PDM_CR_DMAEN = PDM_CR_DMAEN_MASK,
    PDM_CR_DMAEN_0 = 1 << PDM_CR_DMAEN_POS,
    PDM_CR_DMAEN_1 = 2 << PDM_CR_DMAEN_POS,
    PDM_CR_CHN_MASK = (int)0x8,
    PDM_CR_CHN_POS = 3,
    PDM_CR_CHN = PDM_CR_CHN_MASK
};

enum PDM_REG_CFG_FIELD
{
    PDM_CFG_CLKRATIO_MASK = (int)0xff,
    PDM_CFG_CLKRATIO_POS = 0,
    PDM_CFG_CLKRATIO = PDM_CFG_CLKRATIO_MASK,
    PDM_CFG_CAPDELAY_MASK = (int)0xff00,
    PDM_CFG_CAPDELAY_POS = 8,
    PDM_CFG_CAPDELAY = PDM_CFG_CAPDELAY_MASK,
    PDM_CFG_SAMPLERATE_MASK = (int)0xff0000,
    PDM_CFG_SAMPLERATE_POS = 16,
    PDM_CFG_SAMPLERATE = PDM_CFG_SAMPLERATE_MASK,
    PDM_CFG_DATAGAIN_MASK = (int)0xf000000,
    PDM_CFG_DATAGAIN_POS = 24,
    PDM_CFG_DATAGAIN = PDM_CFG_DATAGAIN_MASK
};

enum PDM_REG_IER_FIELD
{
    PDM_IER_INT_MASK = (int)0x1,
    PDM_IER_INT_POS = 0,
    PDM_IER_INT = PDM_IER_INT_MASK
};

enum PDM_REG_IDR_FIELD
{
    PDM_IDR_INT_MASK = (int)0x1,
    PDM_IDR_INT_POS = 0,
    PDM_IDR_INT = PDM_IDR_INT_MASK
};

enum PDM_REG_IVS_FIELD
{
    PDM_IVS_INT_MASK = (int)0x1,
    PDM_IVS_INT_POS = 0,
    PDM_IVS_INT = PDM_IVS_INT_MASK
};

enum PDM_REG_RIF_FIELD
{
    PDM_RIF_INT_MASK = (int)0x1,
    PDM_RIF_INT_POS = 0,
    PDM_RIF_INT = PDM_RIF_INT_MASK
};

enum PDM_REG_IFM_FIELD
{
    PDM_IFM_INT_MASK = (int)0x1,
    PDM_IFM_INT_POS = 0,
    PDM_IFM_INT = PDM_IFM_INT_MASK
};

enum PDM_REG_ICR_FIELD
{
    PDM_ICR_INT_MASK = (int)0x1,
    PDM_ICR_INT_POS = 0,
    PDM_ICR_INT = PDM_ICR_INT_MASK
};

#endif

/************************ (C) COPYRIGHT Linkedsemi *****END OF FILE****/
