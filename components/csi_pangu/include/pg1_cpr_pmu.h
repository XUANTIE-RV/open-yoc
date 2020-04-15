/*
 * pg1_cpr_pmu.h
 *
 *  Created on: 2019-9-7
 *      Author: liuyg
 */

#ifndef PG1_CPR_PMU_H_
#define PG1_CPR_PMU_H_

#define BLOCK_SIZE  160
#define ADC_CHN_MAX 4

#define PANGU_PMU_BASE    0x30000000
#define PANGU_CPR0_BASE   0x31000000
#define PANGU_CPR1_BASE   0x8B000000
#define PANGU_GSK_BASE    0x90000000
#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

typedef struct {
    __IOM uint32_t PMU_CLKSRCSEL;		// 0x000
    __IOM uint32_t PMU_SYSCLKDIVEN;
    __IOM uint32_t PMU_CPU12SWRST;
    __IOM uint32_t PMU_PLLCTRL;
    __IOM uint32_t PMU_PLLCTRL_FRAC;	// 0x010
    __IOM uint32_t PMU_LPCR;
    __IOM uint32_t PMU_WKUPMASK;
    __IM  uint32_t PMU_RSTSTA;
    __IOM uint32_t PMU_RCCTRL;			// 0x020
    __IM  uint32_t PMU_LPSTA;
    __IM  uint32_t PMU_LVMD;
    __IOM uint32_t PMU_REQMDTRIG;
    __IM  uint32_t PMU_PERIERRSTA;		// 0x030
    __IOM uint32_t PMU_PLLINTRCTRL;
    __IOM uint32_t PMU_FRACPLLFRAC;
    uint32_t RESERVED0;
    __IOM uint32_t PMU_BOOTADDR_804_0;	// 0x040
    __IOM uint32_t PMU_BOOTADDR_805;
    __IOM uint32_t PMU_BOOTADDR_BYPASS;
    __IOM uint32_t PMU_LPTIMCTRL;
    uint32_t RESERVED1;			// 0x050
    __IOM uint32_t PMU_LPTIM0RST;
    uint32_t RESERVED2;
    __IOM uint32_t PMU_AONRTCRST;
    uint32_t RESERVED3;			// 0x060
    __IOM uint32_t PMU_AONGPIORST;
    __IOM uint32_t PMU_LPBOOTADDR_804_0;
    __IOM uint32_t PMU_LPBOOTADDR_805;
    __IOM uint32_t PMU_QSPI_CNT;		// 0x070
    __IOM uint32_t PMU_WAIT_12M;
    __IOM uint32_t PMU_RTCCLKCTRL;
    uint32_t RESERVED4;
    __IOM uint32_t PMU_BOOTADDR_804_1;	// 0x080
    __IOM uint32_t PMU_LP_BOOTADDR_804_1;
    uint32_t RESERVED5[95];
    __IOM uint32_t DLC_PCTRL;			// 0x200
    __IOM uint32_t DLC_PRDATA;
    __IOM uint32_t DLC_SR;
    __IOM uint32_t DLC_IMR;
    __IOM uint32_t DLC_IFR;				// 0x210
    __IOM uint32_t DLC_IOIFR;
    __IOM uint32_t DLC_IDIFR;
    __IOM uint32_t DLC_IMCIFR;
} pg1_pmu_reg_t;


typedef struct {
    __IOM uint32_t CPR0_CPU0CLK_SEL;		// 0x000
    __IOM uint32_t CPR0_SYSCLK0_DIV_CTL;
    __IOM uint32_t CPR0_RESERVED0;
    __IOM uint32_t CPR0_FFT_CTL;
    __IOM uint32_t CPR0_TIM1_CTL;			// 0x010
    __IOM uint32_t CPR0_FMC_AHB0_CTL;
    __IOM uint32_t CPR0_ROM_CTL;
    __IOM uint32_t CPR0_GPIO0_CTL;
    __IOM uint32_t CPR0_QSPI_CTL;			// 0x020
    __IOM uint32_t CPR0_USI0_CTL;
    __IOM uint32_t CPR0_USI1_CTL;
    __IOM uint32_t CPR0_UART0_CTL;
    __IOM uint32_t CPR0_I2S0_CLK_CTL;		// 0x030
    __IOM uint32_t CPR0_I2S1_CLK_CTL;
    __IOM uint32_t CPR0_EFUSE_CTL;
    __IOM uint32_t CPR0_SASC_CTL;
    __IOM uint32_t CPR0_TIPC_CTL;			// 0x040
    __IOM uint32_t CPR0_SDIO_CLK_CTL;
    __IOM uint32_t CPR0_SDMMC_CLK_CTL;
    __IOM uint32_t CPR0_AES_CLK_CTL;
    __IOM uint32_t CPR0_RSA_CLK_CTL;		// 0x050
    __IOM uint32_t CPR0_SHA_CLK_CTL;
    __IOM uint32_t CPR0_TRNG_CLK_CTL;
    __IOM uint32_t CPR0_DMA0_CLK_CTL;
    __IOM uint32_t CPR0_WDT_CLK_CTL;		// 0x060
    __IOM uint32_t CPR0_PWM0_CLK_CTL;
    __IOM uint32_t CPR0_MS0SWRST;
    __IOM uint32_t CPR0_AHB0SUB0SWRST;
    __IOM uint32_t CPR0_AHB0SUB1SWRST;		// 0x070
    __IOM uint32_t CPR0_AHB0SUB2SWRST;
    __IOM uint32_t CPR0_APB0SWRST;
    __IOM uint32_t CPR0_UART1_CTL;
    __IOM uint32_t CPR0_CK804_0_DAHBL_BASE;	// 0x080
    __IOM uint32_t CPR0_CK804_0_DAHBL_MASK;
    __IOM uint32_t CPR0_CK804_0_IAHBL_BASE;
    __IOM uint32_t CPR0_CK804_0_IAHBL_MASK;
    __IOM uint32_t CPR0_CK804_1_DAHBL_BASE;	// 0x090
    __IOM uint32_t CPR0_CK804_1_DAHBL_MASK;
    __IOM uint32_t CPR0_CK804_1_IAHBL_BASE;
    __IOM uint32_t CPR0_CK804_1_IAHBL_MASK;
    uint32_t RESERVED1[25];
    __IOM uint32_t CPR0_H2H0_REG0;			// 0x100
    __IOM uint32_t CPR0_H2H0_REG1;
    __IOM uint32_t CPR0_H2H0_REG2;
    __IOM uint32_t CPR0_H2H0_REG3;
} pg1_cpr0_reg_t;

typedef struct {
    __IOM uint32_t CPR1_CPU2CLK_SEL;		// 0x000
    __IOM uint32_t CPR1_SYSCLK1_DIV_CTL;
    __IOM uint32_t CPR1_TIM2_CTL;
    __IOM uint32_t CPR1_TIM3_CTL;
    __IOM uint32_t CPR1_GPIO1_CTL;			// 0x010
    __IOM uint32_t CPR1_USI2_CTL;
    __IOM uint32_t CPR1_USI3_CTL;
    __IOM uint32_t CPR1_UART2_CLK_CTL;
    __IOM uint32_t CPR1_UART3_CLK_CTL;		// 0x020
    __IOM uint32_t CPR1_I2S2_CTL;
    __IOM uint32_t CPR1_I2S3_CTL;
    __IOM uint32_t CPR1_SPDIF_CTL;
    __IOM uint32_t CPR1_FMC_AHB1_CTL;		// 0x030
    __IOM uint32_t CPR1_TDM_CTL;
    __IOM uint32_t CPR1_PDM_CTL;
    __IOM uint32_t CPR1_DMA1_CLK_CTL;
    __IOM uint32_t CPR1_PWM1_CLK_CTL;		// 0x040
    __IOM uint32_t CPR1_DMA_CH8_SEL;
    __IOM uint32_t CPR1_DMA_CH9_SEL;
    __IOM uint32_t CPR1_DMA_CH10_SEL;
    __IOM uint32_t CPR1_DMA_CH11_SEL;		// 0x050
    __IOM uint32_t CPR1_DMA_CH12_SEL;
    __IOM uint32_t CPR1_DMA_CH13_SEL;
    __IOM uint32_t CPR1_DMA_CH14_SEL;
    __IOM uint32_t CPR1_DMA_CH15_SEL;		// 0x060
    __IOM uint32_t CPR1_MS1SWRST;
    __IOM uint32_t CPR1_APB1SWRST;
    __IOM uint32_t CPR1_CODEC_CLK_CTL;
    __IOM uint32_t CPR1_H2H0_REG0;			// 0x070
    __IOM uint32_t CPR1_H2H0_REG1;
    __IOM uint32_t CPR1_H2H0_REG2;
    __IOM uint32_t CPR1_H2H0_REG3;
} pg1_cpr1_reg_t;

typedef struct {
    __IM  uint32_t SR;                             /* Offset: 0x00      (R)    SR                  Register */
    __IM  uint32_t SR2                           ; /* Offset: 0x04      (R)    SR2                 Register */
    __IM  uint32_t SR3                           ; /* Offset: 0x08      (R)    SR3                 Register */
    __IOM uint32_t ICR                           ; /* Offset: 0x0C      (R/W)  ICR                 Register */
    __IOM uint32_t IMR                           ; /* Offset: 0x10      (R/W)  IMR                 Register */
    __IOM uint32_t IFR                           ; /* Offset: 0x14      (R/W)  IFR                 Register */
    __IOM uint32_t CR_VIC                        ; /* Offset: 0x18      (R/W)  CR_VIC              Register */
    __IOM uint32_t CR_CK                         ; /* Offset: 0x1C      (R/W)  CR_CK               Register */
    __IOM uint32_t AICR_DAC                      ; /* Offset: 0x20      (R/W)  AICR_DAC            Register */
    __IOM uint32_t AICR_ADC                      ; /* Offset: 0x24      (R/W)  AICR_ADC            Register */
    __IOM uint32_t AICR_ADC_2                    ; /* Offset: 0x28      (R/W)  AICR_ADC_2          Register */
    __IOM uint32_t FCR_DAC                       ; /* Offset: 0x2C      (R/W)  FCR_DAC             Register */
    uint32_t RESERVED0[2]                  ; /* Offset: 0X30      (R)     RESERVED */
    __IOM uint32_t CR_WNF                        ; /* Offset: 0x38      (R/W)  CR_WNF              Register */
    __IOM uint32_t FCR_ADC                       ; /* Offset: 0x3C      (R/W)  FCR_ADC             Register */
    __IOM uint32_t CR_DMIC12                     ; /* Offset: 0x40      (R/W)  CR_DMIC12           Register */
    __IOM uint32_t CR_DMIC34                     ; /* Offset: 0x44      (R/W)  CR_DMIC34           Register */
    __IOM uint32_t CR_DMIC56                     ; /* Offset: 0x48      (R/W)  CR_DMIC56           Register */
    __IOM uint32_t CR_DMIC78                     ; /* Offset: 0x4C      (R/W)  CR_DMIC78           Register */
    __IOM uint32_t CR_HP                         ; /* Offset: 0x50      (R/W)  CR_HP               Register */
    __IOM uint32_t GCR_HPL                       ; /* Offset: 0x54      (R/W)  GCR_HPL             Register */
    __IOM uint32_t GCR_HPR                       ; /* Offset: 0x58      (R/W)  GCR_HPR             Register */
    __IOM uint32_t CR_MIC1                       ; /* Offset: 0x5C      (R/W)  CR_MIC1             Register */
    __IOM uint32_t CR_MIC2                       ; /* Offset: 0x60      (R/W)  CR_MIC2             Register */
    __IOM uint32_t CR_MIC3                       ; /* Offset: 0x64      (R/W)  CR_MIC3             Register */
    __IOM uint32_t CR_MIC4                       ; /* Offset: 0x68      (R/W)  CR_MIC4             Register */
    __IOM uint32_t CR_MIC5                       ; /* Offset: 0x6C      (R/W)  CR_MIC5             Register */
    __IOM uint32_t CR_MIC6                       ; /* Offset: 0x70      (R/W)  CR_MIC6             Register */
    __IOM uint32_t CR_MIC7                       ; /* Offset: 0x74      (R/W)  CR_MIC7             Register */
    __IOM uint32_t CR_MIC8                       ; /* Offset: 0x78      (R/W)  CR_MIC8             Register */
    __IOM uint32_t GCR_MIC12                     ; /* Offset: 0x7C      (R/W)  GCR_MIC12           Register */
    __IOM uint32_t GCR_MIC34                     ; /* Offset: 0x80      (R/W)  GCR_MIC34           Register */
    __IOM uint32_t GCR_MIC56                     ; /* Offset: 0x84      (R/W)  GCR_MIC56           Register */
    __IOM uint32_t GCR_MIC78                     ; /* Offset: 0x88      (R/W)  GCR_MIC78           Register */
    __IOM uint32_t CR_DAC                        ; /* Offset: 0x8C      (R/W)  CR_DAC              Register */
    __IOM uint32_t CR_ADC12                      ; /* Offset: 0x90      (R/W)  CR_ADC12            Register */
    __IOM uint32_t CR_ADC34                      ; /* Offset: 0x94      (R/W)  CR_ADC34            Register */
    __IOM uint32_t CR_ADC56                      ; /* Offset: 0x98      (R/W)  CR_ADC56            Register */
    __IOM uint32_t CR_ADC78                      ; /* Offset: 0x9C      (R/W)  CR_ADC78            Register */
    __IOM uint32_t CR_MIX                        ; /* Offset: 0xA0      (R/W)  CR_MIX              Register */
    __IOM uint32_t DR_MIX                        ; /* Offset: 0xA4      (R/W)  DR_MIX              Register */
    __IOM uint32_t GCR_DACL                      ; /* Offset: 0xA8      (R/W)  GCR_DACL            Register */
    __IOM uint32_t GCR_DACR                      ; /* Offset: 0xAC      (R/W)  GCR_DACR            Register */
    __IOM uint32_t GCR_ADC1                      ; /* Offset: 0xB0      (R/W)  GCR_ADC1            Register */
    __IOM uint32_t GCR_ADC2                      ; /* Offset: 0xB4      (R/W)  GCR_ADC2            Register */
    __IOM uint32_t GCR_ADC3                      ; /* Offset: 0xB8      (R/W)  GCR_ADC3            Register */
    __IOM uint32_t GCR_ADC4                      ; /* Offset: 0xBC      (R/W)  GCR_ADC4            Register */
    __IOM uint32_t GCR_ADC5                      ; /* Offset: 0xC0      (R/W)  GCR_ADC5            Register */
    __IOM uint32_t GCR_ADC6                      ; /* Offset: 0xC4      (R/W)  GCR_ADC6            Register */
    __IOM uint32_t GCR_ADC7                      ; /* Offset: 0xC8      (R/W)  GCR_ADC7            Register */
    __IOM uint32_t GCR_ADC8                      ; /* Offset: 0xCC      (R/W)  GCR_ADC8            Register */
    __IOM uint32_t GCR_MIXDACL                   ; /* Offset: 0xD0      (R/W)  GCR_MIXDACL         Register */
    __IOM uint32_t GCR_MIXDACR                   ; /* Offset: 0xD4      (R/W)  GCR_MIXDACR         Register */
    __IOM uint32_t GCR_MIXADCL                   ; /* Offset: 0xD8      (R/W)  GCR_MIXADCL         Register */
    __IOM uint32_t GCR_MIXADCR                   ; /* Offset: 0xDC      (R/W)  GCR_MIXADCR         Register */
    __IOM uint32_t CR_DAC_AGC                    ; /* Offset: 0xE0      (R/W)  CR_DAC_AGC          Register */
    __IOM uint32_t DR_DAC_AGC                    ; /* Offset: 0xE4      (R/W)  DR_DAC_AGC          Register */
    __IOM uint32_t CR_ADC_AGC                    ; /* Offset: 0xE8      (R/W)  CR_ADC_AGC          Register */
    __IOM uint32_t DR_ADC_AGC                    ; /* Offset: 0xEC      (R/W)  DR_ADC_AGC          Register */
    __IOM uint32_t CR_DAC_FI                     ; /* Offset: 0xF0      (R/W)  CR_DAC_FI           Register */
    __IOM uint32_t DR_DAC_FI                     ; /* Offset: 0xF4      (R/W)  DR_DAC_FI           Register */
    uint32_t RESERVED1[2]                  ; /* Offset: 0XF8      (R)     RESERVED */
    __IOM uint32_t SR_WT                         ; /* Offset: 0x100     (R/W)  SR_WT               Register */
    __IOM uint32_t ICR_WT                        ; /* Offset: 0x104     (R/W)  ICR_WT              Register */
    __IOM uint32_t IMR_WT                        ; /* Offset: 0x108     (R/W)  IMR_WT              Register */
    __IOM uint32_t IFR_WT                        ; /* Offset: 0x10C     (R/W)  IFR_WT              Register */
    __IOM uint32_t CR_WT                         ; /* Offset: 0x110     (R/W)  CR_WT               Register */
    __IOM uint32_t CR_WT_2                       ; /* Offset: 0x114     (R/W)  CR_WT_2             Register */
    __IOM uint32_t CR_WT_3                       ; /* Offset: 0x118     (R/W)  CR_WT_3             Register */
    __IOM uint32_t CR_WT_4                       ; /* Offset: 0x11C     (R/W)  CR_WT_4             Register */
    uint32_t RESERVED2[24]                 ; /* Offset: 0X120     (R)     RESERVED */
    __IOM uint32_t CR_TR                         ; /* Offset: 0x180     (R/W)  CR_TR               Register */
    __IOM uint32_t DR_TR                         ; /* Offset: 0x184     (R/W)  DR_TR               Register */
    __IOM uint32_t SR_TR1                        ; /* Offset: 0x188     (R/W)  SR_TR1              Register */
    __IOM uint32_t SR_TR_SRCDAC                  ; /* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
    uint32_t RESERVED3[28]                 ; /* Offset: 0X190     (R)     RESERVED */
    __IOM uint32_t TRAN_SEL                      ; /* Offset: 0x200     (R/W)  TRAN_SEL            Register */
    __OM  uint32_t PARA_TX_FIFO                  ; /* Offset: 0x204     (W)    PARA_TX_FIFO        Register */
    __IM  uint32_t RX_FIFO1                      ; /* Offset: 0x208     (R)    RX_FIFO1            Register */
    __IM  uint32_t RX_FIFO2                      ; /* Offset: 0x20C     (R)    RX_FIFO2            Register */
    __IM  uint32_t RX_FIFO3                      ; /* Offset: 0x210     (R)    RX_FIFO3            Register */
    __IM  uint32_t RX_FIFO4                      ; /* Offset: 0x214     (R)    RX_FIFO4            Register */
    __IM  uint32_t RX_FIFO5                      ; /* Offset: 0x218     (R)    RX_FIFO5            Register */
    __IM  uint32_t RX_FIFO6                      ; /* Offset: 0x21C     (R)    RX_FIFO6            Register */
    __IM  uint32_t RX_FIFO7                      ; /* Offset: 0x220     (R)    RX_FIFO7            Register */
    __IM  uint32_t RX_FIFO8                      ; /* Offset: 0x224     (R)    RX_FIFO8            Register */
    __IOM uint32_t FIFO_TH_CTRL                  ; /* Offset: 0x228     (R/W)  FIFO_TH_CTRL        Register */
    __IOM uint32_t INTR_ERR_CTRL                 ; /* Offset: 0x22C     (R/W)  INTR_ERR_CTRL       Register */
    __IM  uint32_t INTR_ERR_STA                  ; /* Offset: 0x230     (R)    INTR_ERR_STA        Register */
    __IOM uint32_t PATH_EN                       ; /* Offset: 0x234     (R/W)  PATH_EN             Register */
    __OM  uint32_t INTR_ERR_CLR                  ; /* Offset: 0x238     (W)    INTR_ERR_CLR        Register */
    __IM  uint32_t COD_MASTER                    ; /* Offset: 0x23C     (R)    COD_MASTER          Register */
    __IOM uint32_t I2C_ADDR                      ; /* Offset: 0x240     (R/W)  I2C_ADDR            Register */
    __IOM uint32_t TRAN_CTRL                     ; /* Offset: 0x244     (R/W)  TRAN_CTRL           Register */
    __IOM uint32_t SAMPLING_CTRL                 ; /* Offset: 0x248     (R/W)  SAMPLING_CTRL       Register */
    __IM  uint32_t TX_FIFO_CNT                   ; /* Offset: 0x24C     (R)    TX_FIFO_CNT         Register */
    __IM  uint32_t RX_FIFO_CNT_1                 ; /* Offset: 0x250     (R)    RX_FIFO_CNT_1       Register */
    __IM  uint32_t RX_FIFO_CNT_2                 ; /* Offset: 0x254     (R)    RX_FIFO_CNT_2       Register */
    __IM  uint32_t RX_FIFO_CNT_3                 ; /* Offset: 0x258     (R)    RX_FIFO_CNT_3       Register */
    __IM  uint32_t RX_FIFO_CNT_4                 ; /* Offset: 0x25C     (R)    RX_FIFO_CNT_4       Register */
    __IM  uint32_t RX_FIFO_CNT_5                 ; /* Offset: 0x260     (R)    RX_FIFO_CNT_5       Register */
    __IM  uint32_t RX_FIFO_CNT_6                 ; /* Offset: 0x264     (R)    RX_FIFO_CNT_6       Register */
    __IM  uint32_t RX_FIFO_CNT_7                 ; /* Offset: 0x268     (R)    RX_FIFO_CNT_7       Register */
    __IM  uint32_t RX_FIFO_CNT_8                 ; /* Offset: 0x26C     (R)    RX_FIFO_CNT_8       Register */
} pg1_codec_reg_t;

#define DMAC1_BASE              0x88000000

#define DMA_REG_SARx            0x0
#define DMA_REG_DARx            0x8
#define DMA_REG_CTRLax          0x18
#define DMA_REG_CTRLbx          0x1c
#define DMA_REG_CFGax           0x40
#define DMA_REG_CFGbx           0x44

#define DMA_REG_RawTfr          0x2c0
#define DMA_REG_RawBlock        0x2c8
#define DMA_REG_RawSrcTran      0x2d0
#define DMA_REG_RawDstTran      0x2d8
#define DMA_REG_RawErr          0x2e0

#define DMA_REG_StatusTfr       0x2e8
#define DMA_REG_StatusBlock     0x2f0
#define DMA_REG_StatusSrcTran   0x2f8
#define DMA_REG_StatusDstTran   0x300
#define DMA_REG_StatusErr       0x308

#define DMA_REG_MaskTfr         0x310
#define DMA_REG_MaskBlock       0x318
#define DMA_REG_MaskSrcTran     0x320
#define DMA_REG_MaskDstTran     0x328
#define DMA_REG_MaskErr         0x330

#define DMA_REG_ClearTfr        0x338
#define DMA_REG_ClearBlock      0x340
#define DMA_REG_ClearSrcTran    0x348
#define DMA_REG_ClearDstTran    0x350
#define DMA_REG_ClearErr        0x358
#define DMA_REG_StatusInt       0x360

#define DMA_REG_ReqSrc          0x368
#define DMA_REG_ReqDst          0x370
#define DMA_REG_SglReqSrc       0x378
#define DMA_REG_SglReqDst       0x380
#define DMA_REG_LstReqSrc       0x388
#define DMA_REG_LstReqDst       0x390

#define DMA_REG_Cfg             0x398
#define DMA_REG_ChEn            0x3a0

#define DMAC1_CFG(addr)  (*(volatile uint32_t *)((DMAC1_BASE + (addr))))



#endif /* PG1_CPR_PMU_H_ */
