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
#include <drv/spi.h>
#include <soc.h>
#include <csi_core.h>
#include <silan_pic.h>
#include <drv/errno.h>
#include <string.h>
#include "silan_spi.h"
#include "silan_syscfg.h"

#define ERR_SPI(errno) (CSI_DRV_ERRNO_SPI_BASE | errno)
#define SPI_BUSY_TIMEOUT    0x1000000
#define SPI_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_SPI(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    uint32_t irq;
    int32_t ssel;
    spi_event_cb_t cb_event;
    int32_t send_num;
    int32_t recv_num;
    uint8_t *send_buf;
    uint8_t *recv_buf;
    uint8_t enable_slave;
    int32_t transfer_num;
    int32_t clk_num;            //clock number with a process of communication
    uint8_t state;               //Current SPI state
    uint32_t mode;               //Current SPI mode
    uint8_t ss_mode;
    spi_status_t status;
    int32_t block_mode;
    int32_t idx;
#define TRANSFER_STAT_IDLE      0
#define TRANSFER_STAT_SEND      1
#define TRANSFER_STAT_RCV       2
#define TRANSFER_STAT_TRAN      3
    uint8_t  transfer_stat;     //TRANSFER_STAT_* : 0 - idle, 1 - send , 2 -receive , 3 - transceive
    uint32_t tot_num;
} silan_spi_priv_t;
extern int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, uint32_t *ssel);
static int32_t silan_spi_set_mode(spi_handle_t handle, DWENUM_SPI_MODE mode);
extern int32_t target_get_addr_space(uint32_t addr);

static silan_spi_priv_t spi_instance[CONFIG_SPI_NUM];

static const spi_capabilities_t spi_capabilities = {
    .simplex = 1,           /* Simplex Mode (Master and Slave) */
    .ti_ssi = 1,            /* TI Synchronous Serial Interface */
    .microwire = 1,         /* Microwire Interface */
    .event_mode_fault = 0   /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

static void silan_spi_cs_sel(SPI_CORE_REGS *regs, uint8_t cs_sel)
{
    SPI_SPICR_REGS spicr;

    spicr.d32 = regs->SPICR;
    spicr.b.cs_sel = (~(1 << cs_sel)) & 0x7;
    regs->SPICR = spicr.d32;
}

static int32_t silan_spi_fifo_transfer(SPI_CORE_REGS *regs, SPI_XFER *xfer)
{
    SPI_SPICR_REGS spicr;
    SPI_SPISR_REGS spisr;
    uint8_t *tx_buf, *rx_buf;
    uint32_t xfer_size, rcv_size;

    tx_buf = xfer->tx_buf;
    rx_buf = xfer->rx_buf;
    xfer_size = xfer->xfer_size;
    rcv_size = xfer_size;

    if (xfer_size <= 0) {
        return -1;
    }

    spicr.d32 = regs->SPICR;
    spicr.b.pure_tx_en = 0;
    spicr.b.pure_rx_en = 0;
    spicr.b.dma_tx_en = 0;
    spicr.b.dma_rx_en = 0;
    regs->SPICR = spicr.d32;

    spisr.d32 = regs->SPISR;

    while ((spisr.b.txfifo_full == 0) && (xfer_size != 0)) {
        regs->SPIFIFOTR = *tx_buf;
        tx_buf++;
        xfer_size--;
        spisr.d32 = regs->SPISR;
    }

    spicr.b.start = 1;
    regs->SPICR = spicr.d32;

    while (rcv_size) {
        if (spisr.b.rxfifo_empty == 0) {
            *rx_buf = regs->SPIFIFORR & 0xFF;
            rx_buf++;
            rcv_size--;
        }

        if ((spisr.b.txfifo_full == 0) && (xfer_size != 0)) {
            regs->SPIFIFOTR = *tx_buf;
            tx_buf++;
            xfer_size--;
        }

        spisr.d32 = regs->SPISR;
    }

    spicr.b.start = 0;
    regs->SPICR = spicr.d32;
    return 0;
}

/**
  \brief       Set the SPI datawidth.
  \param[in]   handle     spi handle
  \param[in]   datawidth  date frame size in bits
  \return      error code
*/
int32_t csi_spi_config_datawidth(spi_handle_t handle, uint32_t datawidth)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (datawidth > 16 || datawidth < 2) {
        return ERR_SPI(SPI_ERROR_DATA_BITS);
    }

    SPI_SPICR_REGS spicr;

    spicr.d32 = addr->SPICR;
    spicr.b.bitnum = datawidth;
    addr->SPICR = spicr.d32;
    silan_spi_cs_sel(addr, 0);
    return 0;
}

/**
  \brief       Set the SPI clock divider.
  \param[in]   handle   spi handle
  \param[in]   baud     spi baud rate
  \return      error code
*/
int32_t csi_spi_config_baudrate(spi_handle_t handle, uint32_t baud)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (baud <= 10000) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    uint32_t bus_clk, tmp_div, div;

    bus_clk = silan_get_bus_cclk();
    tmp_div = bus_clk / baud;

    SPI_SPICR_REGS spicr;

    spicr.d32 = addr->SPICR;

    if (tmp_div > 32) {
        div = SPI_CLKDIV_32;
    } else if (tmp_div > 16) {
        div = SPI_CLKDIV_32;
    } else if (tmp_div > 12) {
        div = SPI_CLKDIV_16;
    } else if (tmp_div > 8) {
        div = SPI_CLKDIV_12;
    } else if (tmp_div > 6) {
        div = SPI_CLKDIV_8;
    } else if (tmp_div > 4) {
        div = SPI_CLKDIV_6;
    } else if (tmp_div > 2) {
        div = SPI_CLKDIV_4;
    } else {
        div = SPI_CLKDIV_2;
    }

    spicr.b.clkdiv1 = div & 0x3;
    spicr.b.clkdiv2 = (div >> 2) & 0x1;

    spicr.b.rx_bigendian = 1;
    spicr.b.sclk_en = 1;
    addr->SPICR = spicr.d32;
    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       Set the SPI polarity.
  \param[in]   addr  pointer to register address
  \param[in]   polarity spi polarity
  \return      error code
*/
static int32_t silan_spi_set_polarity(PSPI_CORE_REGS addr, DWENUM_SPI_POLARITY polarity)
{
    /* To config the polarity, we can set the CPOL as below:
     *     0 - inactive state of serial clock is low
     *     1 - inactive state of serial clock is high
     */
    SPI_SPICR_REGS spicr;

    spicr.d32 = addr->SPICR;

    switch (polarity) {
        case DWENUM_SPI_CLOCK_POLARITY_LOW:
            spicr.b.cpol = 0;
            break;

        case DWENUM_SPI_CLOCK_POLARITY_HIGH:
            spicr.b.cpol = 0;
            break;

        default:
            return -1;
    }

    addr->SPICR = spicr.d32;

    return 0;
}

/**
  \brief       Set the SPI Phase.
  \param[in]   addr  pointer to register address
  \param[in]   phase    Serial clock phase
  \return      error code
*/
static int32_t silan_spi_set_phase(PSPI_CORE_REGS addr, DWENUM_SPI_PHASE phase)
{
    SPI_SPICR_REGS spicr;

    spicr.d32 = addr->SPICR;

    switch (phase) {
        case DWENUM_SPI_CLOCK_PHASE_MIDDLE:
            spicr.b.cpha = 0;
            break;

        case DWENUM_SPI_CLOCK_PHASE_START:
            spicr.b.cpha = 0;
            break;

        default:
            return -1;
    }

    addr->SPICR = spicr.d32;

    return 0;
}

/**
  \brief       config the SPI format.
  \param[in]   handle   spi handle
  \param[in]   format   spi format. \ref spi_format_e
  \return      error code
*/
int32_t csi_spi_config_format(spi_handle_t handle, spi_format_e format)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    switch (format) {
        case SPI_FORMAT_CPOL0_CPHA0:
            silan_spi_set_polarity(addr, DWENUM_SPI_CLOCK_POLARITY_LOW);
            silan_spi_set_phase(addr, DWENUM_SPI_CLOCK_PHASE_MIDDLE);
            break;

        case SPI_FORMAT_CPOL0_CPHA1:
            silan_spi_set_polarity(addr, DWENUM_SPI_CLOCK_POLARITY_LOW);
            silan_spi_set_phase(addr, DWENUM_SPI_CLOCK_PHASE_START);
            break;

        case SPI_FORMAT_CPOL1_CPHA0:
            silan_spi_set_polarity(addr, DWENUM_SPI_CLOCK_POLARITY_HIGH);
            silan_spi_set_phase(addr, DWENUM_SPI_CLOCK_PHASE_MIDDLE);
            break;

        case SPI_FORMAT_CPOL1_CPHA1:
            silan_spi_set_polarity(addr, DWENUM_SPI_CLOCK_POLARITY_HIGH);
            silan_spi_set_phase(addr, DWENUM_SPI_CLOCK_PHASE_START);
            break;

        default:
            return ERR_SPI(SPI_ERROR_FRAME_FORMAT);
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   mode     spi mode. \ref spi_mode_e
  \return      error code
*/
int32_t csi_spi_config_mode(spi_handle_t handle, spi_mode_e  mode)
{
    SPI_NULL_PARAM_CHK(handle);

    if (mode > SPI_MODE_SLAVE_SIMPLEX) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (mode == SPI_MODE_SLAVE_SIMPLEX || mode == SPI_MODE_MASTER_SIMPLEX || mode == SPI_MODE_SLAVE) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    return 0;
}
/**
  \brief       config the SPI block mode.
  \param[in]   handle   spi handle
  \param[in]   flag 1 - enbale the block mode. 0 - disable the block mode
  \return      error code
*/
int32_t csi_spi_config_block_mode(spi_handle_t handle, int32_t flag)
{
    SPI_NULL_PARAM_CHK(handle);
    silan_spi_priv_t *spi_priv = handle;

    if (flag == 1) {
        spi_priv->block_mode = 1;
    } else if (flag == 0) {
        spi_priv->block_mode = 0;
    } else {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   order    spi bit order.\ref spi_bit_order_e
  \return      error code
*/
int32_t csi_spi_config_bit_order(spi_handle_t handle, spi_bit_order_e order)
{
    SPI_NULL_PARAM_CHK(handle);

    if (order > SPI_ORDER_LSB2MSB) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    return 0;
}

/**
  \brief       config the SPI slave select mode.
  \param[in]   handle   spi handle
  \param[in]   ss_mode  spi slave select mode. \ref spi_ss_mode_e
  \return      error code
*/
int32_t csi_spi_config_ss_mode(spi_handle_t handle, spi_ss_mode_e ss_mode)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    switch (ss_mode) {
        case SPI_SS_MASTER_SW:
            spi_priv->ss_mode = SPI_SS_MASTER_SW;
            break;

        case SPI_SS_MASTER_HW_OUTPUT:
            spi_priv->ss_mode = SPI_SS_MASTER_HW_OUTPUT;
            break;

        case SPI_SS_MASTER_HW_INPUT:
            spi_priv->ss_mode = SPI_SS_MASTER_HW_INPUT;
            break;

        case SPI_SS_SLAVE_HW:
            spi_priv->ss_mode = SPI_SS_SLAVE_HW;
            break;

        case SPI_SS_SLAVE_SW:
            spi_priv->ss_mode = SPI_SS_SLAVE_SW;
            break;

        default:
            return ERR_SPI(SPI_ERROR_SS_MODE);
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       Set the SPI mode.
  \param[in]   addr  pointer to register address
  \param[in]   mode     SPI_Mode
  \return      error code
*/
static int32_t silan_spi_set_mode(spi_handle_t handle, DWENUM_SPI_MODE mode)
{
    silan_spi_priv_t *spi_priv = handle;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    SPI_SPICR_REGS spicr;

    spicr.d32 = addr->SPICR;

    switch (mode) {
        case DWENUM_SPI_TXRX:
            spicr.b.rd_only = 1;
            spicr.b.wr_only = 1;
            break;

        case DWENUM_SPI_TX:
            spicr.b.rd_only = 0;
            spicr.b.wr_only = 1;
            break;

        case DWENUM_SPI_RX:
            spicr.b.rd_only = 1;
            spicr.b.wr_only = 0;
            break;

        default:
            break;
    }

    if (spi_priv->block_mode == 1) {
        spicr.b.rd_only = 1;
        spicr.b.wr_only = 1;

    }

    addr->SPICR = spicr.d32;

    spi_priv->mode = mode;
    return 0;
}

/**
  \brief       interrupt service function for receive FIFO full interrupt .
  \param[in]   spi_priv pointer to spi private.
*/
void silan_spi_intr_rx_full(int32_t idx, silan_spi_priv_t *spi_priv)
{
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    uint8_t *pbuffer = spi_priv->recv_buf;
    uint32_t length = spi_priv->recv_num;
    uint8_t rxnum = SILAN_SPI_FIFO_MAX_LEN;
    uint32_t i = 0u;

    if (pbuffer == NULL) {
        for (i = 0; i < rxnum; i++) {
            addr->SPIFIFORR;
        }

        return;
    }

    if (spi_priv->mode == DWENUM_SPI_TXRX) {
        SPI_SPISR_REGS spisr;
        spisr.d32 = addr->SPISR;
        spi_priv->transfer_num = 0;

        while (spisr.b.rxfifo_empty == 0) {
            *spi_priv->recv_buf = addr->SPIFIFORR & 0xff;
            spi_priv->recv_buf++;
            spi_priv->transfer_num++;
            spisr.d32 = addr->SPISR;
        }

        spi_priv->clk_num -= spi_priv->transfer_num;
        spi_priv->recv_num -= spi_priv->transfer_num;
        spi_priv->transfer_num = 0;
        if (spi_priv->clk_num <= 0) {
            SPI_SPICR_REGS spicr;
            spicr.d32 = addr->SPICR;
            spicr.b.inten = 0;
            spicr.b.start = 0;
            addr->SPICR = spicr.d32;

            spi_priv->status.busy = 0U;
            spi_priv->recv_num = 0;

            if (spi_priv->cb_event) {
                spi_priv->cb_event(idx, SPI_EVENT_TRANSFER_COMPLETE);
                return;
            }
        }
    } else {
        if (rxnum > length) {
            rxnum = length;
        }

        for (i = 0; i < rxnum; i++) {
            *pbuffer = addr->SPIFIFORR;
            pbuffer++;
        }

        length -= rxnum;

        if (length <= 0) {
            SPI_SPICR_REGS spicr;
            SPI_SPISR_REGS spisr;
            spisr.d32 = addr->SPISR;

            spicr.d32 = addr->SPICR;
            spicr.b.inten = 0;
            spicr.b.start = 0;
            addr->SPICR = spicr.d32;
            spisr.d32 = addr->SPISR;
            uint32_t timeout = 0;

            while (spisr.b.rxfifo_empty == 0) {
                addr->SPIFIFORR;
                spisr.d32 = addr->SPISR;
                timeout++;

                if (timeout > SPI_BUSY_TIMEOUT) {
                    if (spi_priv->cb_event) {
                        spi_priv->cb_event(idx, SPI_EVENT_DATA_LOST);
                        return;
                    }
                }
            }

            spi_priv->status.busy = 0U;

            spi_priv->recv_num = 0;

            if (spi_priv->cb_event) {
                spi_priv->cb_event(idx, SPI_EVENT_RX_COMPLETE);
                return;
            }
        } else {
            spi_priv->recv_buf = pbuffer;
            spi_priv->recv_num = length;
        }

    }

}
static void spi_delay(int count)
{
    volatile int i;

    for (i = 0; i < count ; i++);
}
/**
  \brief       interrupt service function for transmit FIFO empty interrupt.
  \param[in]   spi_priv pointer to spi private.
*/
void silan_spi_intr_tx_empty(int32_t idx, silan_spi_priv_t *spi_priv)
{
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    /* transfer mode:transmit & receive */
    uint32_t i = 0u;
    SPI_SPICR_REGS spicr;
    SPI_SPISR_REGS spisr;

    if (spi_priv->mode == DWENUM_SPI_TXRX) {
        spi_delay(100);

        /* read data out from rx FIFO */
        spisr.d32 = addr->SPISR;
        spi_priv->transfer_num = 0;

        while (spisr.b.rxfifo_empty == 0) {
            *spi_priv->recv_buf = addr->SPIFIFORR & 0xff;
            spi_priv->recv_buf++;
            spi_priv->transfer_num++;
            spisr.d32 = addr->SPISR;
        }

        uint32_t send_num;

        if (spi_priv->send_num >= SILAN_SPI_FIFO_MAX_LEN) {
            send_num = SILAN_SPI_FIFO_MAX_LEN;
        } else {
            send_num = spi_priv->send_num;
        }

        for (i = 0; i < send_num; i++) {
            addr->SPIFIFOTR = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->send_num--;
        }

        spi_priv->clk_num -= spi_priv->transfer_num;
        spi_priv->recv_num -= spi_priv->transfer_num;
        spi_priv->transfer_num = 0;

    } else if (spi_priv->mode == DWENUM_SPI_RX) {
        return;
    } else if (spi_priv->mode == DWENUM_SPI_TX) {
        spisr.d32 = addr->SPISR;
        spi_priv->transfer_num = 0;
        uint32_t timeout = 0;

        while ((spisr.b.txfifo_full == 0) && (spi_priv->clk_num - spi_priv->transfer_num != 0)) {
            addr->SPIFIFOTR = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->transfer_num++;
            spisr.d32 = addr->SPISR;
            timeout++;

            if (timeout > SPI_BUSY_TIMEOUT) {
                if (spi_priv->cb_event) {
                    spi_priv->cb_event(idx, SPI_EVENT_DATA_LOST);
                    return;
                }
            }
        }
    }

    if (spi_priv->clk_num == 0) {
        spicr.d32 = addr->SPICR;
        spicr.b.inten = 0;
        spicr.b.start = 0;
        addr->SPICR = spicr.d32;

        spi_priv->status.busy = 0U;

        spi_priv->send_num = 0;

        if (spi_priv->mode == DWENUM_SPI_TXRX) {
            if (spi_priv->cb_event) {
                spi_priv->cb_event(idx, SPI_EVENT_TRANSFER_COMPLETE);
                return;
            }
        } else {
            if (spi_priv->cb_event) {
                spi_priv->cb_event(idx, SPI_EVENT_TX_COMPLETE);
                return;
            }
        }
    }

    spi_priv->clk_num -= spi_priv->transfer_num;
}
/**
  \brief       handler the interrupt.
  \param[in]   spi      Pointer to \ref SPI_RESOURCES
*/
void silan_spi_irqhandler(int32_t idx)
{
    idx -= 4;
    silan_spi_priv_t *spi_priv = &spi_instance[idx];
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    SPI_SPISR_REGS spisr;
    spisr.d32 = addr->SPISR;

    /* deal with receive FIFO full interrupt */
    if (spisr.b.txfifo_empty) {
        silan_spi_intr_tx_empty(idx, spi_priv);
    }

    /* deal with transmit FIFO empty interrupt */
    if (spisr.b.rxfifo_full) {
        silan_spi_intr_rx_full(idx, spi_priv);
    }
}

/**
  \brief       Initialize SPI Interface. 1. Initializes the resources needed for the SPI interface 2.registers event callback function
  \param[in]   idx spi index
  \param[in]   cb_event  event call back function \ref spi_event_cb_t
  \return      return spi handle if success
*/
spi_handle_t csi_spi_initialize(int32_t idx, spi_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    uint32_t ssel = 0u;

    int32_t ret = target_spi_init(idx, &base, &irq, &ssel);

    if (ret < 0 || ret >= CONFIG_SPI_NUM) {
        return NULL;
    }

    silan_spi_priv_t *spi_priv = &spi_instance[idx];

    spi_priv->base = base;
    spi_priv->irq  = irq;
    spi_priv->ssel = ssel;

    spi_priv->cb_event          = cb_event;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->enable_slave      = 1U;
    spi_priv->state             = SPI_INITIALIZED;

    spi_priv->idx  = idx;

    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);
    silan_pic_request(PIC_IRQID_MISC, irq, (hdl_t)silan_spi_irqhandler);
    silan_spi_cs_sel(addr, 0);
    spi_priv->block_mode = 0;

    return (spi_handle_t)spi_priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_uninitialize(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;
    silan_pic_free(PIC_IRQID_MISC, spi_priv->irq);

    spi_priv->cb_event          = NULL;
    spi_priv->state             = 0U;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    return 0;
}

/**
  \brief       control spi power.
  \param[in]   idx     spi index.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_spi_power_control(spi_handle_t handle, csi_power_stat_e state)
{
    SPI_NULL_PARAM_CHK(handle);
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx spi index.
  \return      \ref spi_capabilities_t
*/
spi_capabilities_t csi_spi_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPI_NUM) {
        spi_capabilities_t ret;
        memset(&ret, 0, sizeof(spi_capabilities_t));
        return ret;
    }

    return spi_capabilities;
}

/**
  \brief       config spi mode.
  \param[in]   handle spi handle to operate.
  \param[in]   baud      spi baud rate. if negative, then this attribute not changed
  \param[in]   mode      \ref spi_mode_e . if negative, then this attribute not changed
  \param[in]   format    \ref spi_format_e . if negative, then this attribute not changed
  \param[in]   order     \ref spi_bit_order_e . if negative, then this attribute not changed
  \param[in]   ss_mode   \ref spi_ss_mode_e . if negative, then this attribute not changed
  \param[in]   bit_width spi data bitwidth: (1 ~ SPI_DATAWIDTH_MAX) . if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_spi_config(spi_handle_t handle,
                       int32_t          baud,
                       spi_mode_e       mode,
                       spi_format_e     format,
                       spi_bit_order_e  order,
                       spi_ss_mode_e    ss_mode,
                       int32_t          bit_width)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_INITIALIZED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    int32_t ret = 0;

    if (baud >= 0) {
        ret = csi_spi_config_baudrate(handle, baud);

        if (ret < 0) {
            return ret;
        }
    }

    if (mode >= 0) {
        ret = csi_spi_config_mode(handle, mode);

        if (ret < 0) {
            return ret;
        }
    }

    if (format >= 0) {
        ret = csi_spi_config_format(handle, format);

        if (ret < 0) {
            return ret;
        }
    }

    if (order >= 0) {
        ret = csi_spi_config_bit_order(handle, order);

        if (ret < 0) {
            return ret;
        }
    }

    if (ss_mode >= 0) {
        ret = csi_spi_config_ss_mode(handle, ss_mode);

        if (ret < 0) {
            return ret;
        }
    }

    if (bit_width > 0) {
        ret = csi_spi_config_datawidth(handle, bit_width);

        if (ret < 0) {
            return ret;
        }
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}


/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               if non-blocking mode, this function only start the sending,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref csi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send.
  \return      error code
*/
int32_t csi_spi_send(spi_handle_t handle, const void *data, uint32_t num)

{
    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy       = 1U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->tot_num           = num;
    spi_priv->transfer_stat     = TRANSFER_STAT_SEND;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);
    silan_spi_set_mode(spi_priv, DWENUM_SPI_TX);
    uint8_t *ptxbuffer = (uint8_t *)data;

    if (spi_priv->block_mode) {
        SPI_XFER xfer;
        xfer.rx_buf = ptxbuffer;
        xfer.tx_buf = ptxbuffer;
        xfer.xfer_size = spi_priv->tot_num;
        silan_spi_fifo_transfer(addr, &xfer);

        spi_priv->status.busy   = 0U;
    } else {
        spi_priv->send_buf = ptxbuffer;
        spi_priv->clk_num  = num;
        spi_priv->recv_buf = NULL;
        SPI_SPICR_REGS spicr;
        SPI_SPISR_REGS spisr;

        spicr.d32 = addr->SPICR;
        spicr.b.pure_tx_en = 0;
        spicr.b.pure_rx_en = 0;
        spicr.b.dma_tx_en = 0;
        spicr.b.dma_rx_en = 0;
        addr->SPICR = spicr.d32;

        spisr.d32 = addr->SPISR;

        while ((spisr.b.txfifo_full == 0) && (spi_priv->clk_num != 0)) {
            addr->SPIFIFOTR = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->clk_num--;
            spisr.d32 = addr->SPISR;
        }

        spicr.b.inten = 1;
        spicr.b.start = 1;
        addr->SPICR = spicr.d32;
    }

    return 0;
}

/**
\brief      receiving data from SPI receiver. if non-blocking mode, this function only start the receiving,
            \ref spi_event_e is signaled when operation completes or error happens.
            \ref csi_spi_get_status can indicates operation status.
            if blocking mode, this function return after operation completes or error happens.
\param[in]  handle spi handle to operate.
\param[out] data  Pointer to buffer for data to receive from SPI receiver
\param[in]  num   Number of data items to receive
\return     error code
*/
int32_t csi_spi_receive(spi_handle_t handle, void *data, uint32_t num)
{
    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy = 1U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->tot_num           = num;
    spi_priv->transfer_stat     = TRANSFER_STAT_RCV;

    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);
    uint8_t *prx_buffer = data;

    if (spi_priv->block_mode) {
        silan_spi_set_mode(spi_priv, DWENUM_SPI_RX);
        spi_priv->recv_buf = prx_buffer;
        spi_priv->recv_num = num;

        SPI_XFER xfer;

        xfer.rx_buf = spi_priv->recv_buf;
        xfer.xfer_size = spi_priv->recv_num;
        xfer.tx_buf = spi_priv->recv_buf;

        silan_spi_fifo_transfer(addr, &xfer);

        spi_priv->status.busy   = 0U;
    } else {
        spi_priv->recv_buf = prx_buffer;
        spi_priv->recv_num = num;
        spi_priv->clk_num = num;
        silan_spi_set_mode(spi_priv, DWENUM_SPI_RX);
        SPI_SPICR_REGS spicr;
        SPI_SPISR_REGS spisr;
        spicr.d32 = addr->SPICR;
        spicr.b.pure_tx_en = 0;
        spicr.b.pure_rx_en = 0;
        spicr.b.dma_tx_en = 0;
        spicr.b.dma_rx_en = 0;
        addr->SPICR = spicr.d32;

        spisr.d32 = addr->SPISR;

        while (spisr.b.rxfifo_empty == 0) {
            addr->SPIFIFORR;
            spisr.d32 = addr->SPISR;
        }

        spicr.b.inten = 1;
        spicr.b.start = 1;
        addr->SPICR = spicr.d32;
    }

    return 0;
}

/**
  \brief       sending/receiving data to/from SPI transmitter/receiver.
               if non-blocking mode, this function only start the transfer,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref csi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num_out      Number of data items to send
  \param[in]   num_in       Number of data items to receive
  \param[in]   block_mode   blocking and non_blocking to selcect
  \return      error code
*/
int32_t csi_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    if (handle == NULL || data_in == NULL || num_out == 0 || num_in == 0 || data_out == NULL) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy = 1U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;

    spi_priv->tot_num = (num_out > num_in) ? num_out : num_in;
    spi_priv->transfer_stat     = TRANSFER_STAT_TRAN;

    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    silan_spi_set_mode(spi_priv, DWENUM_SPI_TXRX);
    uint8_t *ptx_buffer = (uint8_t *)data_out;
    uint8_t *prx_buffer = (uint8_t *)data_in;

    if (spi_priv->block_mode) {
        SPI_XFER xfer;

        xfer.rx_buf = prx_buffer;
        xfer.xfer_size = spi_priv->tot_num;
        xfer.tx_buf = ptx_buffer;

        silan_spi_fifo_transfer(addr, &xfer);
        spi_priv->status.busy = 0U;
    } else {
        spi_priv->send_buf      = ptx_buffer;
        spi_priv->recv_buf      = prx_buffer;
        spi_priv->send_num      = spi_priv->tot_num;
        spi_priv->recv_num      = spi_priv->tot_num;
        spi_priv->clk_num       = spi_priv->tot_num;
        spi_priv->transfer_num  = 0;
        SPI_SPICR_REGS spicr;
        SPI_SPISR_REGS spisr;
        spicr.d32 = addr->SPICR;
        spicr.b.pure_tx_en = 0;
        spicr.b.pure_rx_en = 0;
        spicr.b.dma_tx_en = 0;
        spicr.b.dma_rx_en = 0;
        spicr.b.inten = 0;
        spicr.b.start = 0;
        addr->SPICR = spicr.d32;

        spisr.d32 = addr->SPISR;

        while (spisr.b.rxfifo_empty == 0) {
            addr->SPIFIFORR;
            spisr.d32 = addr->SPISR;
        }

        spisr.d32 = addr->SPISR;

        while ((spisr.b.txfifo_full == 0) && (spi_priv->send_num != 0)) {
            addr->SPIFIFOTR      = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->send_num--;
            spisr.d32 = addr->SPISR;
        }

        spicr.b.inten = 1;
        spicr.b.start = 1;
        addr->SPICR = spicr.d32;
    }

    return 0;
}

/**
  \brief       abort spi transfer.
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_abort_transfer(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    silan_spi_priv_t *spi_priv = handle;
    PSPI_CORE_REGS addr = (PSPI_CORE_REGS)(spi_priv->base);

    SPI_SPICR_REGS spicr;
    spicr.d32 = addr->SPICR;
    spicr.b.start = 0;
    addr->SPICR = spicr.d32;

    spi_priv->status.busy = 0U;
    spi_priv->recv_buf = NULL;
    spi_priv->recv_num = 0;

    return 0;
}

/**
  \brief       Get SPI status.
  \param[in]   handle spi handle to operate.
  \return      SPI status \ref ARM_SPI_STATUS
*/
spi_status_t csi_spi_get_status(spi_handle_t handle)
{
    spi_status_t spi_status = {0, 0, 0};

    if (handle == NULL) {
        return spi_status;
    }

    silan_spi_priv_t *spi_priv = handle;

    return spi_priv->status;
}

/**
  \brief       Get spi transferred data count.
  \param[in]   handle  spi handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_spi_get_data_count(spi_handle_t handle)
{
    uint32_t cnt = 0;

    if (handle == NULL) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    silan_spi_priv_t *spi_priv = handle;

    if (spi_priv->transfer_stat == TRANSFER_STAT_SEND) {
        cnt = spi_priv->tot_num - spi_priv->send_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_RCV) {
        cnt = spi_priv->tot_num - spi_priv->recv_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_TRAN) {
        cnt = spi_priv->tot_num - (spi_priv->recv_num > spi_priv->send_num ? spi_priv->recv_num : spi_priv->send_num);
    }

    return cnt;
}

/**
  \brief       Control the Slave Select signal (SS).
  \param[in]   handle  spi handle to operate.
  \param[in]   stat    SS state. \ref spi_ss_stat_e.
  \return      error code
*/
int32_t csi_spi_ss_control(spi_handle_t handle, spi_ss_stat_e stat)
{
    SPI_NULL_PARAM_CHK(handle);
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}
