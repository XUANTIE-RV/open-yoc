/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     DCD_ISO7816.h
 * @brief    header file for ISO7816 driver
 * @version  V1.0
 * @date     02. Nov 2019
 ******************************************************************************/

#ifndef _DCD_ISO7816_H_
#define _DCD_ISO7816_H_

#include "soc.h"
#include "stdint.h"
#include "drv/iso7816.h"

#ifdef __cplusplus
extern "C" {
#endif

/*event*/
#define ISO_7816_CRD_EV_IRQ (1 << 7)    //If a card error or the rising edge of the cdet pin is detected this bit will be set HIGH.
#define ISO_7816_ERR_EV_IRQ (1 << 6)    //HIGH state indicates that one of the ERROR register status bit is active: PAR_ERR,CRC_ERR,REP_ERR,RX_OVER,CWT_TIM. Cleared by CLR_ERR command
#define ISO_7816_TX_EV_IRQ  (1 << 5)    //TX threshold is reached. Cleared by HIGH state of the ackt pin or reading the TX_FIFO
#define ISO_7816_RX_EV_IRQ  (1 << 4)    //RX threshold is reached. Cleared by HIGH state of the ackr pin or reading the RX_FIFO
#define ISO_7816_TX_EM_IRQ  (1 << 3)    //Indicates that TX_FIFO is empty and last character has been transmitted successfully. Cleared by writing anything to the TX_FIFO.
#define ISO_7816_BREAK_IRQ  (1 << 2)    //Asserted means that the IC Card has switched from receive mode to the transmit mode. It is cleared by asserting the RT or CLR_ERR bit in control register.
#define ISO_7816_BLK_FIN_IRQ  (1 << 1)  //HIGH state of this bit indicates that the block is finished. Cleared by CLR_ERR command or beginning of the new block.
#define ISO_7816_RX_AV_IRQ  (1 << 0)    //This bit indicates that there is at least one new character in the RX_FIFO. It is automatically reset when the RX_FIFO is empty.

/*card status*/
#define ISO_7816_ODDP_CARD_STA    (1 << 7)  //parity forced when this bit is HIGH. Default is EVEN parity.
#define ISO_7816_SES_END_CARD_STA (1 << 6)  //It informs that the card session has been closed.
#define ISO_7816_TIMOUT_CARD_STA  (1 << 5)  //The asserted state means that the card has not respond to reset or the block/character wait time has expired. Cleared by a clear error command.
#define ISO_7816_SYNC_ERR_CARD_STA (1 << 4) //When the neither direct nor inverse convention is detected this bit is set HIGH. Cleared by a clear error command.
#define ISO_7816_CONV_CARD_STA    (1 << 3)  //High value of this bit indicates the inverse convention (MSb first), the LOW value means that the direct convention (LSb first) is being used. The convention is detected automatically as the first byte of the ATR is received. Note that TS byte will not be stored in the RX FIFO.
#define ISO_7816_CRD_ERR_CARD_STA (1 << 2)  //If the activation sequence failed, the card was removed during the active card session or the synchronization error occurs this bit will be set HIGH. Cleared by a clear error command.
#define ISO_7816_CRD_ACT_CARD_STA (1 << 1)  //After successful activation sequence the CRD_ACT bit should be in the HIGH state, which means that card session is active and the CPU is able to transmit and receive data
#define ISO_7816_CRD_DET_CARD_STA (1 << 0)  //When the CRD_DET_EN bit in the CONFIG register is active, this bit will follow the state of the cdet pin. In other case it is always set HIGH.

/*err status*/
#define ISO_7816_RX_FULL (1 << 6)
#define ISO_7816_CWT_TIM (1 << 5)   //If this bit is in the HIGH state it means that character wait time exceed. Cleared by the CLR_ERR command. When HIGH sets also ERR_EV bit.
#define ISO_7816_TX_FULL (1 << 4)
#define ISO_7816_RX_OVER (1 << 3)
#define ISO_7816_REP_ERR (1 << 2)
#define ISO_7816_CRC_ERR (1 << 1)
#define ISO_7816_PAR_ERR (1 << 0)   //HIGH state of this bit indicates that parity error was detected during the reception of a character. When HIGH sets also ERR_EV bit. Cleared by a CLR_ERR command.

#define ISO7816_TX_FIFO_SIZE 16
#define ISO7816_RX_FIFO_SIZE 16

typedef struct {
    __IOM   uint32_t RW_FIFO;           //0x00
    __IOM   uint32_t CONTROL;           //0x04
    __IOM   uint32_t MSTATUS_MMASK;     //0x08
    __IOM    uint32_t CSTATUS;          //0x0c
    __IOM    uint32_t CONFIG;           //0x10
    __IOM    uint32_t MISC;             //0x14
    __IOM    uint32_t CCLK_DIV;         //0x18
    __IOM    uint32_t ETU_CRC0_LRC;     //0x1c
    __IOM    uint32_t EGT_CRC1;         //0x20
    __IOM    uint32_t WT0_BGT;          //0x24
    __IOM    uint32_t WT1_CWT;          //0x28
    __IOM    uint32_t WT2;              //0x2c
    __IOM    uint32_t WT3;              //0x30
    __IOM    uint32_t ERR_MERR;         //0x34
    __IOM    uint32_t LENGTH;           //0x38
} dcd_iso7816_reg_t;

#define ATR_MAX_DATA_SIZE 33
typedef enum {
    T0_S,
    TA1_S,
    TB1_S,
    TC1_S,
    TD1_S,
    TA2_S,
    TB2_S,
    TC2_S,
    TD2_S,
    TA3_S,
    TB3_S,
    TC3_S,
    TD3_S,
    TK_S,
    TCK_S,
    TEND_S
} chars_t;

typedef struct {
    int32_t is_direct_encode;
    int32_t wt;
    int32_t wwt;
    uint8_t buf_idx;
    uint8_t sections;
    uint8_t histbytes;
    uint8_t t1;
    chars_t atr_sta;
    uint8_t buf[ATR_MAX_DATA_SIZE];
    uint8_t pps[5];
    uint8_t rec_pps[5];
} atr_analysis_t;

typedef enum {
    RX_MODE,
    TX_MODE,
} tranfser_mode_t;

typedef struct {
    csi_dev_t dev;
    uint32_t base;
    int32_t idx;
    uint32_t gsk_irq;
    iso7816_event_cb_t cb;
    void *cb_arg;
    iso7816_card_sta_t card_sta;
    uint8_t *rx_buf;
    uint32_t rx_recv_cnt;
    uint8_t *tx_buf;
    uint32_t tx_trf_cnt;
    tranfser_mode_t rt;
    atr_analysis_t atr;
} dcd_iso7816_priv_t;

typedef enum {
    CLK_SOTP_NOT_SUPPORTED,
    CLK_STOP_STA_L,
    CLK_STOP_STA_H,
    CLK_STOP_NO_PREFERENCE,
} iso7816_clk_stop_type_t;

typedef enum {
    LRC,
    CRC,
} crc_type_t;

typedef struct {
    int32_t fi;
    int32_t di;
    int32_t f_max;
    iso7816_clk_stop_type_t clk_stop_type;
    iso7816_voltage_class_t support_voltage_class;
    int32_t protocol_negotiation_en;
    int32_t N;
    int32_t T; // T=0 char mode, T=1 block mode;
    int32_t TD2_T;
    int32_t wi;
    int32_t cwi;
    int32_t bwi;
    crc_type_t t1_check;
    int32_t history_byte_num;
    uint8_t history_data[15];
} atr_result_t;

chars_t atr_next_state(dcd_iso7816_priv_t *priv, uint8_t x);
void atr_parse(dcd_iso7816_priv_t *priv);
atr_result_t * atr_get_result(int idx);

#ifdef __cplusplus
}
#endif

#endif /* _DCD_ISO7816_H_ */

