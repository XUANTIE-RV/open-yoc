/*
 * The Clear BSD License
 * Copyright (c) 2018, Hangzhou Silan Microelectronics CO.,LTD.
 * Copyright 2018-2020 SILAN
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SILAN_SPI_H_
#define _SILAN_SPI_H_

#include "assert.h"
#include "silan_types.h"
#include "ap1508.h"
#include "silan_dmac.h"

#define SPI_CP_MODE0            0
#define SPI_CP_MODE1            1
#define SPI_CP_MODE2            2
#define SPI_CP_MODE3            3


#define SILAN_SPI1_ID           0
#define SILAN_SPI2_ID           1

#define SPI_REG_OFFSET_START    0x80
#define SILAN_SPI_FIFO_MAX_LEN   8

typedef enum {
    DWENUM_SPI_TXRX = 0,
    DWENUM_SPI_TX   = 1,
    DWENUM_SPI_RX   = 2,
    DWENUM_SPI_EERX = 3
} DWENUM_SPI_MODE;

typedef enum {
    DWENUM_SPI_CLOCK_POLARITY_LOW   = 0,
    DWENUM_SPI_CLOCK_POLARITY_HIGH  = 1
} DWENUM_SPI_POLARITY;

typedef enum {
    DWENUM_SPI_CLOCK_PHASE_MIDDLE   = 0,
    DWENUM_SPI_CLOCK_PHASE_START    = 1
} DWENUM_SPI_PHASE;

#define SPI_INITIALIZED         ((uint8_t)(1U))             // SPI initalized
#define SPI_POWERED             ((uint8_t)(1U<< 1))         // SPI powered on
#define SPI_CONFIGURED          ((uint8_t)(1U << 2))        // SPI configured
#define SPI_DATA_LOST           ((uint8_t)(1U << 3))        // SPI data lost occurred
#define SPI_MODE_FAULT          ((uint8_t)(1U << 4))        // SPI mode fault occurred

typedef struct SPI_core_regs
{
    __IO uint32_t        RESERVED0[SPI_REG_OFFSET_START >> 2];
    __IO uint32_t        SPISR;
    __IO uint32_t        SPICR;
    __IO uint32_t        SPICNT;
    __IO uint32_t        RESERVED1;
    __IO uint32_t        SPIFIFOTR;
    __IO uint32_t        SPIFIFORR;
    __IO uint32_t        SPIREGTR;
    __IO uint32_t        SPIREGRR;
}
SPI_CORE_REGS, *PSPI_CORE_REGS;

typedef union SPI_spisr_regs
{
    uint32_t d32;

    struct
    {
        uint32_t reserved0_1 : 2;
        uint32_t rcv_full : 1;
        uint32_t xmit_empty : 1;
        uint32_t int_n : 1;
        uint32_t busy : 1;
        uint32_t reserved6 : 1;
        uint32_t done : 1;
        uint32_t reserved8_14 : 7;
        uint32_t cnt_done : 1;
        uint32_t rxfifo_hf : 1;
        uint32_t rxfifo_empty : 1;
        uint32_t rxfifo_full : 1;
        uint32_t reserved19_23 : 5;
        uint32_t txfifo_he : 1;
        uint32_t txfifo_empty : 1;
        uint32_t txfifo_full : 1;
        uint32_t reserved27_31 : 5;
    }
    b;
} SPI_SPISR_REGS ;

typedef union SPI_spicr_regs
{
    uint32_t d32;

    struct
    {
        uint32_t rcv_cpol : 1;
        uint32_t cpol : 1;
        uint32_t cpha : 1;
        uint32_t clkdiv1 : 2;
        uint32_t start : 1;
        uint32_t inten : 1;
        uint32_t spien : 1;
        uint32_t clkdiv2 : 1;
        uint32_t reserved9_10 : 2;
        uint32_t sclk_en : 1;
        uint32_t cs_clken : 1;
        uint32_t rd_only : 1;
        uint32_t wr_only : 1;
        uint32_t bitnum : 6;
        uint32_t cnt_done_en : 1;
        uint32_t rx_bigendian : 1;
        uint32_t tx_bigendian : 1;
        uint32_t cs_sel : 3;
        uint32_t fifo_flush : 1;
        uint32_t pure_rx_en : 1;
        uint32_t pure_tx_en : 1;
        uint32_t dma_rx_en : 1;
        uint32_t dma_tx_en : 1;
    }
    b;
} SPI_SPICR_REGS ;

typedef union SPI_spicnt_regs
{
    uint32_t d32;

    struct
    {
        uint32_t cur_cnt : 16;
        uint32_t xfer_cnt : 16;
    }
    b;
} SPI_SPICNT_REGS ;

typedef enum SPI_clkdiv
{
    SPI_CLKDIV_HALF        = 0,
    SPI_CLKDIV_2,
    SPI_CLKDIV_4,
    SPI_CLKDIV_8,
    SPI_CLKDIV_16,
    SPI_CLKDIV_32,
    SPI_CLKDIV_6,
    SPI_CLKDIV_12,
}
SPI_CLKDIV, *PSPI_CLKDIV;

typedef struct SPI_cfg
{
    uint8_t            id;
    uint8_t         mode;
    uint8_t         cpha;
    uint8_t         cpol;
    uint8_t            rcv_cpol;
    uint8_t         div;
    uint32_t        freq;
    uint8_t            tx_srcid;
    uint8_t            rx_srcid;
}
SPI_CFG, *PSPI_CFG;

typedef struct SPI_port_cfg
{
    DMA_REQUEST        *tx_dma_req;
    DMA_REQUEST        *rx_dma_req;
    int8_t            tx_num;
    int8_t            rx_num;
}
SPI_PORT_CFG, *PSPI_PORT_CFG;

typedef struct SPI_port
{
    SPI_CFG            spi_cfg;
    SPI_PORT_CFG    port_cfg;
    SPI_CORE_REGS    *regs;
}
SPI_PORT, *PSPI_PORT;

typedef struct SPI_xfer
{
    uint32_t        xfer_size;      // Transfer size, Bytes
    uint8_t         *rx_buf;
    uint8_t         *tx_buf;

    uint8_t         tx_only;
    uint8_t         cs_sel;
}
SPI_XFER, *PSPI_XFER;

int32_t silan_spi_open(SPI_PORT *port, uint8_t id, uint8_t mode);
int32_t silan_spi_xfer(SPI_PORT *port, SPI_XFER *xfer);

#endif

