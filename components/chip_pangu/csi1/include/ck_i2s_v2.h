/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_i2s_v2.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.2
 * @date     8.15. August 2019
 ******************************************************************************/

#ifndef _CK_I2S_V2_H_
#define _CK_I2S_V2_H_

#include "soc.h"
#include "dev_ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_CHIP_PANGU) || defined (CONFIG_CHIP_TX216)
#define I2S_TRANFSER_DMAC
#else
#define I2S_TRANFSER_INTERRUPT
//#define I2S_TRANFSER_POLLING
#endif

#define I2S_TIME_OUT 0xffffffff

#define I2S_MODULE_ENABLE_MASK 0x01
#define I2S_RX_MODE_WRITE_EN_MASK (0x01 << 5)
#define I2S_RX_MODE_EN_MASK (0x01 << 4)
#define I2S_TX_MODE_WRITE_EN_MASK (0x01 << 1)
#define I2S_TX_MODE_EN_MASK (0x01)
#define I2S_RECEIVE_DELAY_MASK (0x03 << 12)     //I2S receiver s_sclk and s_ws delay level
#define I2S_RX_FIFO_THRESHOLD_MASK (0x1f)
#define I2S_RX_CLK_SELECT_MASK (0X01 << 9)
#define I2S_TX_MODE_MASK (0x01 << 4)
#define I2S_TX_FIFO_THRESHOLD_MASK (0x1f)
#define I2S_TX_MONO_MODE_MASK (0x01 << 3)
#define I2S_TX_STANDARD_MASK (0x03)
#define I2S_RX_MODE_MASK (0X01 << 8)
#define I2S_RX_MONO_MODE_CHANNEL_SEL_MASK (0x01 << 5)
#define I2S_RX_MONO_MODE_ENABLE_MASK (0x01 << 4)
#define I2S_RX_LEFT_CHANNEL_POLARITY_MASK (0x01 << 2)
#define I2S_RX_STANDARD_MASK (0x03)
#define I2S_MCLK_FREQ_MASK (0x01 << 16)
#define I2S_DATA_WIDTH_MODE_MASK (0x0f << 8)
#define I2S_RX_AUDIO_RATE_SCALE_FACTOR_MASK (0x03 << 6)
#define I2S_RX_AUDIO_INPUT_RATE_MASK (0x03 << 4)
#define I2S_RX_AUDIO_RATE_AUTO_DETECTED_EN_MAXK 0x01
#define I2S_TX_SCLK_SELECT_MASK (0x01 << 5)
#define I2S_TX_LEFT_CHANNEL_POLARITY_MASK (0x01 << 2)
#define I2S_FIFO_DATA_NUM_MASK (0x3f)

/*IRQ mask*/
#define I2S_RX_INPUT_SAMPLE_FREQ_CHANGE_IRQ_MASK (0x01 << 9)
#define I2S_TX_BUSY_FLAG_REVERSE_IRQ_MASK (0x01 << 8)
#define I2S_RX_BUSY_FLAG_REVERSE_IRQ_MASK (0x01 << 7)
#define I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK (0x01 << 6)
#define I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK (0x01 << 5)
#define I2S_RX_OVERFLOW_ERROR_IRQ_MASK (0x01 << 4)
#define I2S_RX_UNDERFLOW_ERROR_IRQ_MASK (0x01 << 3)
#define I2S_TX_OVERFLOW_ERROR_IRQ_MASK (0x01 << 2)
#define I2S_TX_UNDERFLOW_ERROR_IRQ_MASK (0x01 << 1)
#define I2S_WRONG_ADDRS_ERROR_IRQ_MASK (0x01)

#define I2S_B16_B16 0X00
#define I2S_B16_B24 0X01
#define I2S_B16_B32 0X02
#define I2S_B24_B16 0X04
#define I2S_B24_B24 0X05
#define I2S_B24_B32 0X06
#define I2S_B32_B16 0X08
#define I2S_B32_B24 0X09
#define I2S_B32_B32 0X0A
#define I2S_B8_B16  0X0C
#define I2S_B8_B24  0X0D
#define I2S_B8_B32  0X0E
#define I2S_B8_B8   0X0F

#define CK_I2S_DMA_TX_EN_MASK (0x01 << 1)
#define CK_I2S_DMA_RX_EN_MASK 0x01

/*I2S Status Code*/
#define I2S_RX_FIFO_FULL   (0x01 << 5)
#define I2S_RX_FIFO_NOT_EMPTY (0x01 << 4)
#define I2S_TX_FIFO_EMPTY (0x01 << 3)
#define I2S_TX_FIFO_NOT_FULL (0x01 << 2)
#define I2S_TX_BUSY (0x01 << 1)
#define I2S_RX_BUSY (0x01)

#define ERR_I2S(errno) (CSI_DRV_ERRNO_I2S_BASE | errno)
#define I2S_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_I2S(DRV_ERROR_PARAMETER))

#define I2S_BUSY_TIMEOUT 0x1000000
#define I2S_SRC_CLK_FREQ 49152000
#define I2S_DMA_MAX_MV_SIZE (4095 * 4)

/*I2S priv status*/
#define I2S_READY        (0x01)
#define I2S_TX_RUNING    (0x01 << 1)
#define I2S_RX_RUNING    (0x01 << 2)
#define I2S_TX_PAUSE     (0x01 << 3)
#define I2S_RX_PAUSE     (0x01 << 4)

#define min(a, b)  (((a) < (b)) ? (a) : (b))

typedef struct {
    dev_ringbuf_t fifo;
    uint32_t rewind_len;
    uint32_t backward_len;
    uint8_t *backward_point;
    uint8_t *rewind_point;
    uint32_t mv_block_size;
} ringbuffer_dma_point_t;

typedef struct  {
    i2s_config_type_t record_cfg;
    uint32_t base;
    uint32_t tx_en;
    uint32_t rx_en;
    int32_t idx;
    uint32_t irq_num;
    void *irq_handle;
    i2s_event_cb_t cb;
    void *cb_arg;
    uint32_t priv_sta;
    ringbuffer_dma_point_t rx_ring_buf;
    ringbuffer_dma_point_t tx_ring_buf;
    int32_t tx_dma_ch;
    int32_t rx_dma_ch;
    uint32_t tx_period;
    uint32_t rx_period;
} ck_i2s_priv_v2_t;

typedef enum {
    I2S_SCLK_SRC_MCLK_I,
    I2S_SCLK_SRCCLK,
} i2s_sclk_clock_src_e;

typedef struct {
    __IOM uint32_t I2S_IISEN;            /* Offset: 0x000h (R/W)  AUDIO_IO Enable Register */
    __IOM uint32_t I2S_FUNCMODE;         /* Offset: 0x004h (R/W)  AUDIO_IO function mode */
    __IOM uint32_t I2S_IISCNF_IN;        /* Offset: 0x008h (R/W)  IIS interface configuration in (on ARX side) */
    __IOM uint32_t I2S_FSSTA;            /* Offset: 0x00ch (R/W)  IIS ATX audio input control/state register */
    __IOM uint32_t I2S_IISCNF_OUT;       /* Offset: 0x010h (R/W)  IIS interface configuration in (on ATX side) */
    __IOM uint32_t I2S_FADTLR;           /* Offset: 0x014h (R/W)  IIS Fs auto detected Threshold level register */
    __IOM uint32_t I2S_SCCR;             /* Offset: 0x018h (R/W)  Sample compress control register */
    __IOM uint32_t I2S_TXFTLR;           /* Offset: 0x01ch (R/W)  Transmit FIFO Threshold Level */
    __IOM uint32_t I2S_RXFTLR;           /* Offset: 0x020h (R/W)  Receive FIFO Threshold Level */
    __IOM uint32_t I2S_TXFLR;            /* Offset: 0x024h (R/W)  Transmit FIFO Level Register */
    __IOM uint32_t I2S_RXFLR;            /* Offset: 0x028h (R/W)  Receive FIFO Level Register */
    __IOM uint32_t I2S_SR;               /* Offset: 0x02ch (R/W)  Status Register */
    __IOM uint32_t I2S_IMR;              /* Offset: 0x030h (R/W)  Interrupt Mask Register */
    __IOM uint32_t I2S_ISR;              /* Offset: 0x034h (R/W)  Interrupt Status Register */
    __IOM uint32_t I2S_RISR;             /* Offset: 0x038h (R/W)  Raw Interrupt Status Register */
    __IOM uint32_t I2S_ICR;              /* Offset: 0x03ch (R/W)  FIFO Interrupt Clear Register */
    __IOM uint32_t I2S_DMACR;            /* Offset: 0x040h (R/W)  DMA Control Register */
    __IOM uint32_t I2S_DMATDLR;          /* Offset: 0x044h (R/W)  DMA Transmit Data Level */
    __IOM uint32_t I2S_DMARDLR;          /* Offset: 0x048h (R/W)  DMA Receive Data Level */
    __IOM uint32_t I2S_DR;               /* Offset: 0x04Ch (R/W)  Data Register */
    __IOM uint32_t I2S_DIV0_LEVEL;       /* Offset: 0x050h (R/W)  Divide i2s_clkgen source clock, get mclk or sclk */
    __IOM uint32_t I2S_DIV3_LEVEL;       /* Offset: 0x054h (R/W)  Divide i2s_clkgen source clock, get reference clock */
} ck_i2s_v2_reg_t;

typedef enum {
    I2S_LEFT_CHANNEL,
    I2S_RIGHT_CHANNEL,
    I2S_BOTH_CHANNEL,
} i2s_channel_e;

typedef struct {
    void (*stream_uninit)(ck_i2s_priv_v2_t *priv);
    uint32_t (*stream_send)(i2s_handle_t handle, const uint8_t *data, uint32_t length);
    uint32_t (*stream_recv)(i2s_handle_t handle, uint8_t *buf, uint32_t length);
    void (*stream_tx_start)(ck_i2s_priv_v2_t *priv);
    void (*stream_rx_start)(ck_i2s_priv_v2_t *priv);
    void (*stream_tx_stop)(ck_i2s_priv_v2_t *priv);
    void (*stream_rx_stop)(ck_i2s_priv_v2_t *priv);
    void (*stream_irq_handle)(int idx);
} ck_i2s_stream_obj;


extern ck_i2s_stream_obj priv_stream_obj;

#ifdef __cplusplus
}
#endif

#endif /* _CK_I2S_V2_H_ */
