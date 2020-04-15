/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_iic.c
 * @brief    CSI Source File for IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <drv/iic.h>
#include <drv/irq.h>
#include <dw_iic.h>
#include <soc.h>
#include <csi_core.h>
#include <string.h>
#include <drv/dmac.h>
#if defined(CONFIG_DW_DMAC)
#include <dw_dmac.h>
#endif
#if defined(CONFIG_CK_DMAC)
#include <drv/etb.h>
#include <ck_dmac_v2.h>
#include <ck_etb.h>
#endif

#define ERR_IIC(errno) (CSI_DRV_ERRNO_IIC_BASE | errno)

#define IIC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_IIC(DRV_ERROR_PARAMETER))

typedef struct  {
#ifdef CONFIG_LPM
    uint8_t iic_power_status;
    uint32_t iic_regs_saved[25];
#endif
    int32_t idx;
    uint32_t base;
    uint32_t irq;
    iic_event_cb_t cb_event;
    uint32_t rx_total_num;
    uint32_t tx_total_num;
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    uint32_t cmd_read_num;
    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;
    uint32_t status;             ///< status of iic transfer
    iic_mode_e mode;
#if defined  CONFIG_DW_DMAC || defined CONFIG_CK_DMAC
    uint8_t dma_use;
    int32_t dma_tx_ch;
    int32_t dma_rx_ch;
    uint32_t send_num;
    uint32_t recv_num;
#endif
#if defined  CONFIG_CK_DMAC
    int32_t etb_tx_ch;
    int32_t etb_rx_ch;
#endif
} dw_iic_priv_t;

#define IIC_DMA_BLOCK_SIZE  2048
extern int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
extern int32_t target_get_addr_space(uint32_t addr);

static dw_iic_priv_t iic_instance[CONFIG_IIC_NUM];

static const iic_capabilities_t iic_capabilities = {
    .address_10_bit = 0  /* supports 10-bit addressing */
};

static inline void dw_iic_disable(dw_iic_reg_t *addr)
{
    /* First clear ACTIVITY, then Disable IIC */
    addr->IC_CLR_ACTIVITY;
    addr->IC_ENABLE = DW_IIC_DISABLE;

}

static inline void dw_iic_enable(dw_iic_reg_t *addr)
{
    addr->IC_ENABLE = DW_IIC_ENABLE;
}

static inline void dw_iic_set_transfer_speed(dw_iic_reg_t *addr, DWENUM_IIC_SPEED speed)
{
    uint16_t temp = addr->IC_CON;
    temp &= ~((1 << 1) + (1 << 2));
    temp |= speed << 1;
    addr->IC_CON = temp;
}

static inline void dw_iic_set_target_address(dw_iic_reg_t *addr, uint32_t address)
{
    /* disable i2c */
    dw_iic_disable(addr);
    uint32_t temp = addr->IC_TAR;
    temp &= 0xfc00;
    temp |= address;
    addr->IC_TAR = temp;
}

static inline void dw_iic_set_slave_address(dw_iic_reg_t *addr, uint32_t address)
{
    /* write to IC_SAR register to set the slave address */
    addr->IC_SAR = address & 0x3ff;
}

static inline void dw_iic_set_addr_mode(dw_iic_priv_t *iic_priv, iic_address_mode_e addr_mode)
{
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (iic_priv->mode == IIC_MODE_MASTER) {
        uint16_t temp = addr->IC_TAR;
        temp &= 0xefff;
        temp |= addr_mode << 12;
        addr->IC_TAR = temp;
    } else {
        if (addr_mode == IIC_ADDRESS_10BIT) {
            addr->IC_CON |= DW_IIC_CON_10BITADDR_SLAVE;
        } else {
            addr->IC_CON &= ~DW_IIC_CON_10BITADDR_SLAVE;
        }
    }
}

static uint32_t dw_iic_read_clear_intrbits(dw_iic_reg_t *addr)
{
    uint32_t  stat = 0;

    stat = addr->IC_INTR_STAT;

    if (stat & DW_IIC_INTR_RX_UNDER) {
        addr->IC_CLR_RX_UNDER;
    }

    if (stat & DW_IIC_INTR_RX_OVER) {
        addr->IC_CLR_RX_OVER;
    }

    if (stat & DW_IIC_INTR_TX_OVER) {
        addr->IC_CLR_TX_OVER;
    }

    if (stat & DW_IIC_INTR_RD_REQ) {
        addr->IC_CLR_RD_REQ;
    }

    if (stat & DW_IIC_INTR_TX_ABRT) {
        addr->IC_TX_ABRT_SOURCE;
    }

    if (stat & DW_IIC_INTR_RX_DONE) {
        addr->IC_CLR_RX_DONE;
    }

    if (stat & DW_IIC_INTR_ACTIVITY) {
        addr->IC_CLR_ACTIVITY;
    }

    if (stat & DW_IIC_INTR_STOP_DET) {
        addr->IC_CLR_STOP_DET;
    }

    if (stat & DW_IIC_INTR_START_DET) {
        addr->IC_CLR_START_DET;
    }

    if (stat & DW_IIC_INTR_GEN_CALL) {
        addr->IC_CLR_GEN_CALL;
    }

    return stat;
}

/**
  \brief       interrupt service function for transmit FIFO empty interrupt.
  \param[in]   iic_priv pointer to iic private.
*/
static void dw_iic_intr_tx_empty(int32_t idx, dw_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    uint32_t remain_txfifo;
    uint8_t emptyfifo;
    uint32_t i;

    if (intr_stat & DW_IIC_INTR_TX_EMPTY) {

        remain_txfifo = iic_priv->tx_total_num - iic_priv->tx_cnt;
        emptyfifo = (remain_txfifo > (DW_IIC_FIFO_MAX_LV - addr->IC_TXFLR)) ? DW_IIC_FIFO_MAX_LV - addr->IC_TXFLR : remain_txfifo;

        for (i = 0; i < emptyfifo; i++) {
            addr->IC_DATA_CMD = *iic_priv->tx_buf++;
        }


        iic_priv->tx_cnt += emptyfifo;

        if (iic_priv->tx_cnt == iic_priv->tx_total_num) {
            addr->IC_INTR_MASK &= ~DW_IIC_INTR_TX_EMPTY;
        }
    }

    if (intr_stat & DW_IIC_INTR_RD_REQ) {
        /* slave-transfer operation for bulk transfers */
        remain_txfifo = iic_priv->tx_total_num - iic_priv->tx_cnt;
        emptyfifo = (remain_txfifo > (DW_IIC_FIFO_MAX_LV - addr->IC_TXFLR)) ? DW_IIC_FIFO_MAX_LV - addr->IC_TXFLR : remain_txfifo;

        for (i = 0; i < emptyfifo; i++) {
            addr->IC_DATA_CMD = *iic_priv->tx_buf++;
        }

        iic_priv->tx_cnt += emptyfifo;

        if (iic_priv->tx_cnt == iic_priv->tx_total_num) {
            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, IIC_EVENT_TRANSFER_DONE);
            }
        }
    }

    if (intr_stat & DW_IIC_INTR_TX_OVER) {
        iic_priv->status = IIC_STATE_ERROR;
        dw_iic_disable(addr);
        addr->IC_FIFO_RST_EN = DW_IIC_FIFO_RST_EN;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
        }
    }

    if (intr_stat & DW_IIC_INTR_STOP_DET) {
        iic_priv->status  = IIC_STATE_SEND_DONE;

        if (iic_priv->tx_cnt != iic_priv->tx_total_num) {
            addr->IC_INTR_MASK &= ~DW_IIC_INTR_TX_EMPTY;
        }

        dw_iic_disable(addr);
        addr->IC_FIFO_RST_EN = DW_IIC_FIFO_RST_EN;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_TRANSFER_DONE);
        }
    }

}
/**
  \brief       interrupt service function for receive FIFO full interrupt .
  \param[in]   iic_priv pointer to iic private.
*/
static void dw_iic_intr_rx_full(int32_t idx, dw_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    uint32_t count;
    uint32_t iic_rxflr  = addr->IC_RXFLR;

    if (intr_stat & DW_IIC_INTR_RX_FULL) {

        for (count = 0; count < iic_rxflr; count++) {
            *iic_priv->rx_buf++ = ((addr->IC_DATA_CMD) & 0xff);
            iic_priv->rx_cnt++;
            iic_priv->rx_total_num--;;
        }

        /* change receive FIFO threshold */
        if (iic_priv->rx_total_num >  4) {
            addr->IC_RX_TL = 4 - 1;
        } else {
            addr->IC_RX_TL  = iic_priv->rx_total_num - 1;
        }

        uint32_t cmd_flag_read;

        if (iic_priv->cmd_read_num > iic_rxflr) {
            cmd_flag_read = iic_rxflr;
        } else {
            cmd_flag_read = iic_priv->cmd_read_num;
        }

        for (count = 0; count < cmd_flag_read; count++) {
            addr->IC_DATA_CMD = 1 << 8;
        }

        iic_priv->cmd_read_num -= cmd_flag_read;
    }

    if (intr_stat & DW_IIC_INTR_RX_OVER) {
        iic_priv->status = IIC_STATE_ERROR;
        addr->IC_DATA_CMD = 0;
        dw_iic_disable(addr);

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
        }
    }

    if (intr_stat & DW_IIC_INTR_STOP_DET) {
        if (iic_priv->rx_total_num == 0) {
            iic_priv->status  = IIC_STATE_RECV_DONE;
            addr->IC_INTR_MASK = 0;
            dw_iic_disable(addr);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, IIC_EVENT_TRANSFER_DONE);
            }
        }
    }

}
void dw_iic_irqhandler(int32_t idx)
{
    dw_iic_priv_t *iic_priv = &iic_instance[idx];
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    uint32_t  intr_stat = dw_iic_read_clear_intrbits(addr);

    if (intr_stat & DW_IIC_INTR_TX_ABRT) {
        /* If arbitration fault, it indicates either a slave device not
        * responding as expected, or other master which is not supported
        * by this SW.
        */
        iic_priv->status    = IIC_STATE_DONE;
        dw_iic_disable(addr);
        addr->IC_INTR_MASK = 0;
        addr->IC_FIFO_RST_EN = DW_IIC_FIFO_RST_EN;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
            return;
        }
    }

    switch (iic_priv->status) {
        /* send data to slave */
        case IIC_STATE_DATASEND: {
            dw_iic_intr_tx_empty(idx, iic_priv, intr_stat);
            break;
        }

        /* wait for data from slave */
        case IIC_STATE_WFDATA: {
            dw_iic_intr_rx_full(idx, iic_priv, intr_stat);
            break;
        }

        /* unexpected state,SW fault */
        default: {
            addr->IC_FIFO_RST_EN = DW_IIC_FIFO_RST_EN;
            addr->IC_INTR_MASK = 0;
            dw_iic_disable(addr);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, IIC_EVENT_ARBITRATION_LOST);
            }
        }
    }
}

#if defined CONFIG_DW_DMAC
void dw_iic_dma_event_cb(int32_t ch, dma_event_e event, void *cb_arg)
{
    dw_iic_priv_t *iic_priv = NULL;
    int i = 0;
    uint32_t timeout = 0;

    for (i = 0; i < CONFIG_IIC_NUM; i++) {
        iic_priv = &iic_instance[i];

        if (((iic_priv->dma_tx_ch == ch) && (iic_priv->dma_use == 1)) || ((iic_priv->dma_rx_ch == ch) && (iic_priv->dma_use == 1))) {
            break;
        }
    }

    if (i >= CONFIG_IIC_NUM) {
        if (iic_priv->cb_event) {
            iic_priv->cb_event(iic_priv->idx, IIC_EVENT_BUS_ERROR);
        }
    }

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /* sending date done with DMA  */
        if (iic_priv->dma_tx_ch == ch) {
            iic_priv->tx_total_num -= iic_priv->send_num;
            iic_priv->tx_buf += iic_priv->send_num;

            if (iic_priv->tx_total_num) {

                if (iic_priv->tx_total_num > IIC_DMA_BLOCK_SIZE) {
                    iic_priv->send_num = IIC_DMA_BLOCK_SIZE;
                } else {
                    iic_priv->send_num = iic_priv->tx_total_num;
                }

                /*num divided by 1, 4, 8 with no remainder.*/
                for (i = 2; i > 0; i--) {
                    if (!(iic_priv->send_num % (2 << i))) {
                        break;
                    }
                }

                if (i == 0) {
                    addr->IC_DMA_TDLR = 0;
                } else {
                    addr->IC_DMA_TDLR = (2 << i);
                }

                csi_dma_start(iic_priv->dma_tx_ch, iic_priv->tx_buf, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->send_num);
                return;
            }

            csi_dma_stop(iic_priv->dma_tx_ch);
            csi_dma_release_channel(iic_priv->dma_tx_ch);
            iic_priv->dma_use = 0;
            iic_priv->tx_total_num = 0;
            iic_priv->tx_buf = NULL;
            iic_priv->dma_tx_ch = -1;

            drv_irq_enable(iic_priv->irq);

            while (!(addr->IC_RAW_INTR_STAT & DW_IIC_INTR_STOP_DET)) {
                timeout ++;

                if (timeout > DW_IIC_TIMEOUT_DEF_VAL) {
                    if (iic_priv->cb_event) {
                        iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_INCOMPLETE);
                    }
                }
            }

            addr->IC_DMA_CR &= ~DW_IIC_DMACR_TDMAE_Msk;
            dw_iic_disable(addr);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);
            }

            return;
        } else {
            iic_priv->rx_total_num -= iic_priv->recv_num;
            iic_priv->rx_buf += iic_priv->recv_num;

            if (iic_priv->rx_total_num) {

                if (iic_priv->rx_total_num > IIC_DMA_BLOCK_SIZE) {
                    iic_priv->recv_num = IIC_DMA_BLOCK_SIZE;
                } else {
                    iic_priv->recv_num = iic_priv->rx_total_num;
                }

                /*num divided by 1, 4, 8 with no remainder.*/
                for (i = 2; i > 0; i--) {
                    if (!(iic_priv->recv_num % (2 << i))) {
                        break;
                    }
                }

                if (i == 0) {
                    addr->IC_DMA_RDLR = 0;
                } else {
                    addr->IC_DMA_RDLR = (2 << i) - 1;
                }

                csi_dma_start(iic_priv->dma_rx_ch, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->rx_buf, iic_priv->recv_num);
                return;
            }

            csi_dma_stop(iic_priv->dma_rx_ch);
            csi_dma_release_channel(iic_priv->dma_rx_ch);
            iic_priv->dma_use = 0;
            iic_priv->rx_total_num = 0;
            iic_priv->dma_rx_ch = -1;
            iic_priv->rx_buf = NULL;
            addr->IC_DMA_CR &= ~DW_IIC_DMACR_RDMAE_Msk;
            drv_irq_enable(iic_priv->irq);

            dw_iic_disable(addr);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);
            }

            return;
        }
    } else if (event == DMA_EVENT_TRANSFER_ERROR) {
        if (iic_priv->cb_event) {
            iic_priv->cb_event(iic_priv->idx, IIC_EVENT_BUS_ERROR);
        }
    }


    return;
}

/**
  \brief sending data to IIC transmitter with DMA,(received data is ignored).
*/
static int32_t dw_iic_send_dma(dw_iic_priv_t *iic_priv, const void *data, uint32_t num)
{
    int32_t ret = 0;
    uint8_t i;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (target_get_addr_space((uint32_t)iic_priv->tx_buf) != ADDR_SPACE_SRAM) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));
    config.src_inc  = DMA_ADDR_INC;
    config.dst_inc  = DMA_ADDR_CONSTANT;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    if (iic_priv->idx == 0) {
        config.hs_if    = CKENUM_DMA_IIC0_TX;
    } else if (iic_priv->idx == 1) {
        config.hs_if    = CKENUM_DMA_IIC1_TX;
    } else {
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        iic_priv->dma_tx_ch = -1;
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    config.type     = DMA_MEM2PERH;

    ret = csi_dma_config_channel(iic_priv->dma_tx_ch, &config, dw_iic_dma_event_cb, NULL);

    if (ret < 0) {
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        iic_priv->dma_tx_ch = -1;
        return ERR_IIC(DRV_ERROR_BUSY);
    }

    dw_iic_disable(addr);

    if (iic_priv->tx_total_num > IIC_DMA_BLOCK_SIZE) {
        iic_priv->send_num = IIC_DMA_BLOCK_SIZE;
    } else {
        iic_priv->send_num = iic_priv->tx_total_num;
    }

    /*num divided by 1, 4, 8 with no remainder.*/
    for (i = 2; i > 0; i--) {
        if (!(iic_priv->send_num % (2 << i))) {
            break;
        }
    }

    if (i == 0) {
        addr->IC_DMA_TDLR = 0;
    } else {
        addr->IC_DMA_TDLR = (2 << i);
    }

    addr->IC_DMA_CR = DW_IIC_DMACR_TDMAE_Msk;

    dw_iic_enable(addr);

    csi_dma_start(iic_priv->dma_tx_ch, iic_priv->tx_buf, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->send_num);

    return 0;
}

/**
  \brief receiving data to IIC receiver with DMA.
*/
static int32_t dw_iic_receive_dma(dw_iic_priv_t *iic_priv, void *data, uint32_t num)
{
    int32_t ret = 0;
    uint8_t i;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));
    config.src_inc  = DMA_ADDR_CONSTANT;
    config.dst_inc  = DMA_ADDR_INC;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    if (iic_priv->idx == 0) {
        config.hs_if    = CKENUM_DMA_IIC0_RX;
    } else if (iic_priv->idx == 1) {
        config.hs_if    = CKENUM_DMA_IIC1_RX;
    } else {
        csi_dma_release_channel(iic_priv->dma_rx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        iic_priv->dma_rx_ch = -1;
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    config.type     = DMA_PERH2MEM;

    ret = csi_dma_config_channel(iic_priv->dma_rx_ch, &config, dw_iic_dma_event_cb, NULL);

    if (ret < 0) {
        csi_dma_release_channel(iic_priv->dma_rx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        iic_priv->dma_rx_ch = -1;
        return ERR_IIC(DRV_ERROR_BUSY);
    }

    dw_iic_disable(addr);

    if (iic_priv->rx_total_num > IIC_DMA_BLOCK_SIZE) {
        iic_priv->recv_num = IIC_DMA_BLOCK_SIZE;
    } else {
        iic_priv->recv_num = iic_priv->rx_total_num;
    }

    /*num divided by 1, 4, 8 with no remainder.*/
    for (i = 2; i > 0; i--) {
        if (!(iic_priv->recv_num % (2 << i))) {
            break;
        }
    }

    if (i == 0) {
        addr->IC_DMA_RDLR = 0;
    } else {
        addr->IC_DMA_RDLR = (2 << i) - 1;
    }

    addr->IC_DMA_CR |= DW_IIC_DMACR_RDMAE_Msk;

    dw_iic_enable(addr);

    csi_dma_start(iic_priv->dma_rx_ch, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->rx_buf, iic_priv->recv_num);
    return 0;
}
#endif

#if defined CONFIG_CK_DMAC
static uint32_t find_max_prime_num(uint32_t num)
{
    if (!num) {
        return 0;
    }

    if (!(num % 8)) {
        return 8;
    }

    if (!(num % 7)) {
        return 7;
    }

    if (!(num % 6)) {
        return 6;
    }

    if (!(num % 5)) {
        return 5;
    }

    if (!(num % 4)) {
        return 4;
    }

    if (!(num % 3)) {
        return 3;
    }

    if (!(num % 2)) {
        return 2;
    }

    return 1;
}

void dw_iic_dma_event_cb(int32_t ch, dma_event_e event, void *cb_arg)
{
    dw_iic_priv_t *iic_priv = NULL;
    int i = 0;
    uint32_t timeout = 0;

    for (i = 0; i < CONFIG_IIC_NUM; i++) {
        iic_priv = &iic_instance[i];

        if (((iic_priv->dma_tx_ch == ch) && (iic_priv->dma_use == 1)) || ((iic_priv->dma_rx_ch == ch) && (iic_priv->dma_use == 1))) {
            break;
        }
    }

    if (i >= CONFIG_IIC_NUM) {
        if (iic_priv->cb_event) {
            iic_priv->cb_event(iic_priv->idx, IIC_EVENT_BUS_ERROR);
        }
    }

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /* sending date done with DMA  */
        if (iic_priv->dma_tx_ch == ch) {

            csi_dma_stop(iic_priv->dma_tx_ch);
            drv_etb_stop(iic_priv->etb_tx_ch);
            csi_dma_release_channel(iic_priv->dma_tx_ch);
            iic_priv->dma_use = 0;
            iic_priv->dma_tx_ch = -1;
            iic_priv->etb_tx_ch = -1;

            while (!(addr->IC_RAW_INTR_STAT & DW_IIC_INTR_STOP_DET)) {
                timeout ++;

                if (timeout > DW_IIC_TIMEOUT_DEF_VAL) {
                    if (iic_priv->cb_event) {
                        iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_INCOMPLETE);
                    }
                }
            }

            dw_iic_disable((dw_iic_reg_t *)iic_priv->base);
            drv_irq_enable(iic_priv->irq);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);
            }

            return;
        } else if (iic_priv->dma_rx_ch == ch) {
            /* receiving date done with DMA*/
            csi_dma_stop(iic_priv->dma_rx_ch);
            drv_etb_stop(iic_priv->etb_rx_ch);
            csi_dma_release_channel(iic_priv->dma_rx_ch);
            iic_priv->dma_use = 0;
            iic_priv->dma_rx_ch = -1;
            iic_priv->etb_rx_ch = -1;

            dw_iic_disable((dw_iic_reg_t *)iic_priv->base);
            drv_irq_enable(iic_priv->irq);

            if (iic_priv->cb_event) {
                iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);
            }

            return;
        }
    } else if (event == DMA_EVENT_TRANSFER_ERROR) {
        if (iic_priv->cb_event) {
            iic_priv->cb_event(iic_priv->idx, IIC_EVENT_BUS_ERROR);
        }
    }

    if (event != DMA_EVENT_TRANSFER_MODE_DONE) {
        csi_dma_stop(iic_priv->dma_tx_ch);
        drv_etb_stop(iic_priv->etb_tx_ch);
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        iic_priv->dma_use = 0;
        iic_priv->dma_tx_ch = -1;
        iic_priv->etb_tx_ch = -1;
        drv_irq_enable(iic_priv->irq);
    }

    return;
}

/**
  \brief sending data to IIC transmitter with DMA,(received data is ignored).
*/
static int32_t dw_iic_send_dma(dw_iic_priv_t *iic_priv, const void *data, uint32_t num)
{
    int32_t ret = 0;
    uint32_t etb_source_ip = 0;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));
    config.src_inc  = DMA_ADDR_INC;
    config.dst_inc  = DMA_ADDR_CONSTANT;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    config.mode      = DMA_GROUP_TRIGGER;
    config.group_len = find_max_prime_num(num);
    config.ch_mode   = DMA_MODE_HARDWARE;

    if (iic_priv->idx == 0) {
        etb_source_ip   = ETB_IIC0_TX;
    } else if (iic_priv->idx == 1) {
        etb_source_ip   = ETB_IIC1_TX;
    } else {
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    ret = csi_dma_config_channel(iic_priv->dma_tx_ch, &config, dw_iic_dma_event_cb, NULL);

    if (ret < 0) {
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        return ERR_IIC(DRV_ERROR_BUSY);
    }

    dw_iic_disable(addr);

    addr->IC_DMA_TDLR = find_max_prime_num(iic_priv->tx_total_num);

    addr->IC_DMA_CR |= DW_IIC_DMACR_TDMAE_Msk;

    csi_dma_start(iic_priv->dma_tx_ch, iic_priv->tx_buf, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->tx_total_num);

    iic_priv->etb_tx_ch = drv_etb_channel_config(etb_source_ip, iic_priv->dma_tx_ch, ETB_HARDWARE, ETB_ONE_TRIGGER_ONE);

    drv_etb_start(iic_priv->etb_tx_ch);

    dw_iic_enable(addr);

    return 0;
}

/**
  \brief sending data to IIC transmitter with DMA,(received data is ignored).
*/
static int32_t dw_iic_receive_dma(dw_iic_priv_t *iic_priv, void *data, uint32_t num)
{
    int32_t ret = 0;
    uint32_t etb_source_ip = 0;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));
    config.src_inc  = DMA_ADDR_CONSTANT;
    config.dst_inc  = DMA_ADDR_INC;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    config.mode      = DMA_GROUP_TRIGGER;
    config.group_len = find_max_prime_num(num);
    config.ch_mode   = DMA_MODE_HARDWARE;

    if (iic_priv->idx == 0) {
        etb_source_ip   = ETB_IIC0_RX;
    } else if (iic_priv->idx == 1) {
        etb_source_ip   = ETB_IIC1_RX;
    } else {
        csi_dma_release_channel(iic_priv->dma_rx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    ret = csi_dma_config_channel(iic_priv->dma_rx_ch, &config, dw_iic_dma_event_cb, NULL);

    if (ret < 0) {
        csi_dma_release_channel(iic_priv->dma_rx_ch);
        drv_irq_enable(iic_priv->irq);
        iic_priv->dma_use = 0;
        return ERR_IIC(DRV_ERROR_BUSY);
    }

    dw_iic_disable(addr);

    addr->IC_DMA_RDLR = find_max_prime_num(iic_priv->rx_total_num) - 1;

    addr->IC_DMA_CR = DW_IIC_DMACR_RDMAE_Msk;

    csi_dma_start(iic_priv->dma_rx_ch, (uint8_t *) & (addr->IC_DATA_CMD), iic_priv->rx_buf, iic_priv->rx_total_num);

    iic_priv->etb_rx_ch = drv_etb_channel_config(etb_source_ip, iic_priv->dma_rx_ch, ETB_HARDWARE, ETB_ONE_TRIGGER_ONE);

    drv_etb_start(iic_priv->etb_rx_ch);

    dw_iic_enable(addr);

    return 0;
}
#endif

#ifdef CONFIG_LPM
static void manage_clock(iic_handle_t handle, uint8_t enable)
{
    if (handle == &iic_instance[0]) {
        drv_clock_manager_config(CLOCK_MANAGER_I2C0, enable);
    } else if (handle == &iic_instance[1]) {
        drv_clock_manager_config(CLOCK_MANAGER_I2C1, enable);
    }
}

static void do_prepare_sleep_action(iic_handle_t handle)
{
    dw_iic_priv_t *iic_priv = handle;
    uint32_t *ibase = (uint32_t *)(iic_priv->base);
    registers_save(iic_priv->iic_regs_saved, ibase, 4);
    registers_save(&iic_priv->iic_regs_saved[4], ibase + 5, 6);
    registers_save(&iic_priv->iic_regs_saved[10], ibase + 12, 1);
    registers_save(&iic_priv->iic_regs_saved[11], ibase + 14, 2);
    registers_save(&iic_priv->iic_regs_saved[13], ibase + 32, 12);
}

static void do_wakeup_sleep_action(iic_handle_t handle)
{
    dw_iic_priv_t *iic_priv = handle;
    uint32_t *ibase = (uint32_t *)(iic_priv->base);
    registers_restore(ibase, iic_priv->iic_regs_saved, 4);
    registers_restore(ibase + 5, &iic_priv->iic_regs_saved[4], 6);
    registers_restore(ibase + 12, &iic_priv->iic_regs_saved[10], 1);
    registers_restore(ibase + 14, &iic_priv->iic_regs_saved[11], 2);
    registers_restore(ibase + 32, &iic_priv->iic_regs_saved[13], 12);
}
#endif

/**
  \brief       Initialize IIC Interface specified by pins. \n
               1. Initializes the resources needed for the IIC interface 2.registers event callback function
  \param[in]   idx iic index
  \param[in]   cb_event  Pointer to \ref iic_event_cb_t
  \return      0 for success, negative for error code
*/
iic_handle_t csi_iic_initialize(int32_t idx, iic_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    int32_t ret = target_iic_init(idx, &base, &irq, &handler);

    if (ret < 0 || ret >= CONFIG_IIC_NUM) {
        return NULL;
    }

    dw_iic_priv_t *iic_priv = &iic_instance[idx];
    iic_priv->base = base;
    iic_priv->irq  = irq;
    iic_priv->idx = idx;

    iic_priv->cb_event = cb_event;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;
#if defined  CONFIG_DW_DMAC || defined CONFIG_CK_DMAC
    iic_priv->dma_use = 0;
    iic_priv->dma_tx_ch = -1;
    iic_priv->dma_rx_ch = -1;
#endif
#if defined  CONFIG_CK_DMAC
    iic_priv->etb_tx_ch = -1;
    iic_priv->etb_rx_ch = -1;
#endif
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

#ifdef CONFIG_LPM
    csi_iic_power_control(iic_priv, DRV_POWER_FULL);
#endif
    /* mask all interrupts */
    addr->IC_INTR_MASK  = 0x00;
    addr->IC_CON        = DW_IIC_CON_DEFAUL;

    drv_irq_register(iic_priv->irq, handler);
    drv_irq_enable(iic_priv->irq);

    return iic_priv;
}

/**
  \brief       De-initialize IIC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_uninitialize(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    /* First clear ACTIVITY, then Disable IIC */
    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    addr->IC_CLR_ACTIVITY;
    addr->IC_INTR_MASK  = 0x00;
    addr->IC_ENABLE = DW_IIC_DISABLE;

    iic_priv->cb_event = NULL;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;
#if defined CONFIG_DW_DMAC || defined CONFIG_CK_DMAC

    if (iic_priv->dma_tx_ch == -1) {
        csi_dma_stop(iic_priv->dma_tx_ch);
        csi_dma_release_channel(iic_priv->dma_tx_ch);
        iic_priv->dma_tx_ch = -1;
    }

    if (iic_priv->dma_rx_ch == -1) {
        csi_dma_stop(iic_priv->dma_rx_ch);
        csi_dma_release_channel(iic_priv->dma_rx_ch);
        iic_priv->dma_rx_ch = -1;
    }

#endif
#if defined CONFIG_CK_DMAC

    if (iic_priv->etb_tx_ch == -1) {
        drv_etb_stop(iic_priv->etb_tx_ch);
        iic_priv->etb_tx_ch = -1;
    }

    if (iic_priv->etb_rx_ch == -1) {
        drv_etb_stop(iic_priv->etb_rx_ch);
        iic_priv->etb_rx_ch = -1;
    }

#endif
    drv_irq_disable(iic_priv->irq);
    drv_irq_unregister(iic_priv->irq);

#ifdef CONFIG_LPM
    csi_iic_power_control(iic_priv, DRV_POWER_OFF);
#endif
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx iic index.
  \return      \ref iic_capabilities_t
*/
iic_capabilities_t csi_iic_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPI_NUM) {
        iic_capabilities_t ret;
        memset(&ret, 0, sizeof(iic_capabilities_t));
        return ret;
    }

    return iic_capabilities;
}

/**
  \brief       config iic.
  \param[in]   handle  iic handle to operate.
  \param[in]   mode      \ref iic_mode_e.if negative, then this attribute not changed
  \param[in]   speed     \ref iic_speed_e.if negative, then this attribute not changed
  \param[in]   addr_mode \ref iic_address_mode_e.if negative, then this attribute not changed
  \param[in]   slave_addr slave address.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config(iic_handle_t handle,
                       iic_mode_e mode,
                       iic_speed_e speed,
                       iic_address_mode_e addr_mode,
                       int32_t slave_addr)
{
    int32_t ret;

    ret = csi_iic_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_speed(handle, speed);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_addr_mode(handle, addr_mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_slave_addr(handle, slave_addr);

    if (ret < 0) {
        return ret;
    }

    return 0;
}


/**
  \brief       config iic mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   mode      \ref iic_mode_e.
  \return      error code
*/
int32_t csi_iic_config_mode(iic_handle_t handle, iic_mode_e mode)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)mode < 0) {
        return 0;
    }

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    dw_iic_disable(addr);

    switch (mode) {
        case IIC_MODE_MASTER:
            iic_priv->mode = IIC_MODE_MASTER;
            addr->IC_CON |= DW_IIC_CON_MASTER_ENABLE;  /* master enbaled */
            break;

        case IIC_MODE_SLAVE:
            iic_priv->mode = IIC_MODE_SLAVE;
            addr->IC_CON &= ~DW_IIC_CON_SLAVE_ENABLE;  /* slave enabled */
            addr->IC_CON &= ~DW_IIC_CON_MASTER_ENABLE;  /* master enbaled */

            break;

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic speed.
  \param[in]   handle  iic handle to operate.
  \param[in]   speed     \ref iic_speed_e.
  \return      error code
*/
int32_t csi_iic_config_speed(iic_handle_t handle, iic_speed_e speed)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)speed < 0) {
        return 0;
    }

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(((dw_iic_priv_t *)handle)->base);
    dw_iic_disable(addr);

    switch (speed) {
        case IIC_BUS_SPEED_STANDARD:
            dw_iic_set_transfer_speed(addr, DW_IIC_STANDARDSPEED);
            break;

        case IIC_BUS_SPEED_FAST:
            dw_iic_set_transfer_speed(addr, DW_IIC_FASTSPEED);
            break;

        case IIC_BUS_SPEED_FAST_PLUS:
            return ERR_IIC(DRV_ERROR_UNSUPPORTED);

        case IIC_BUS_SPEED_HIGH:
            dw_iic_set_transfer_speed(addr, DW_IIC_HIGHSPEED);
            break;

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic address mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   addr_mode \ref iic_address_mode_e.
  \return      error code
*/
int32_t csi_iic_config_addr_mode(iic_handle_t handle, iic_address_mode_e addr_mode)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)addr_mode < 0) {
        return 0;
    }

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    dw_iic_disable(addr);

    switch (addr_mode) {
        case IIC_ADDRESS_10BIT:
        case IIC_ADDRESS_7BIT:
            dw_iic_set_addr_mode(iic_priv, addr_mode);
            break;

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic slave address.
  \param[in]   handle  iic handle to operate.
  \param[in]   slave_addr slave address.
  \return      error code
*/
int32_t csi_iic_config_slave_addr(iic_handle_t handle, int32_t slave_addr)
{
    IIC_NULL_PARAM_CHK(handle);
    dw_iic_priv_t *iic_priv = handle;

    if (slave_addr < 0 || iic_priv->mode == IIC_MODE_MASTER) {
        return 0;
    }

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(((dw_iic_priv_t *)handle)->base);
    dw_iic_disable(addr);

    dw_iic_set_slave_address(addr, slave_addr);
    return 0;
}

/**
  \brief       Start transmitting data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref csi_iic_get_status can indicates transmission status.
  \param[in]   handle         iic handle to operate.
  \param[in]   devaddr        iic addrress of slave device.
  \param[in]   data           data to send to IIC Slave
  \param[in]   num            Number of data items to send
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
int32_t csi_iic_master_send(iic_handle_t handle, uint32_t devaddr, const void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    dw_iic_set_target_address(addr, devaddr);

    iic_priv->tx_buf          = (uint8_t *)data;
    iic_priv->tx_total_num    = num;
    iic_priv->tx_cnt          = 0;
    iic_priv->status          = IIC_STATE_DATASEND;

    dw_iic_disable(addr);

#if defined CONFIG_DW_DMAC || defined CONFIG_CK_DMAC

    if (iic_priv->tx_total_num >= DW_IIC_FIFO_MAX_LV) {
        iic_priv->dma_tx_ch = csi_dma_alloc_channel();

        if (iic_priv->dma_tx_ch != -1) {
            iic_priv->dma_use = 1;
            drv_irq_disable(iic_priv->irq);
            return dw_iic_send_dma(iic_priv, data, num);
        }
    }

#endif
    /* open corresponding interrupts */
    addr->IC_INTR_MASK = DW_IIC_INTR_DEFAULT_MASK;

    addr->IC_CLR_INTR;
    addr->IC_TX_TL = DW_IIC_TXFIFO_LV;

    dw_iic_enable(addr);

    return 0;

}

/**
  \brief       Start receiving data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref csi_iic_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[in]   devaddr        iic addrress of slave device.
  \param[out]  data    Pointer to buffer for data to receive from IIC receiver
  \param[in]   num     Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
int32_t csi_iic_master_receive(iic_handle_t handle, uint32_t devaddr, void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->rx_buf          = (uint8_t *)data;
    iic_priv->rx_total_num    = num;
    iic_priv->rx_cnt          = 0;
    iic_priv->status          = IIC_STATE_WFDATA;
    iic_priv->cmd_read_num    = num;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    dw_iic_set_target_address(addr, devaddr);
    dw_iic_disable(addr);

    addr->IC_INTR_MASK = DW_IIC_INTR_STOP_DET | DW_IIC_INTR_RX_FULL;
    addr->IC_CLR_INTR;

    if (iic_priv->rx_total_num >  4) {
        addr->IC_RX_TL        = 4 - 1;    /* Sets receive FIFO threshold */
    } else {
        addr->IC_RX_TL        = iic_priv->rx_total_num - 1;    /* Sets receive FIFO threshold */
    }

    uint32_t count, cmd_flag_read;

    if (iic_priv->cmd_read_num > DW_IIC_FIFO_MAX_LV) {
        cmd_flag_read = 8;
    } else {
        cmd_flag_read = iic_priv->rx_total_num;
    }

    iic_priv->cmd_read_num -= cmd_flag_read;

    dw_iic_enable(addr);

    for (count = 0; count < cmd_flag_read; count++) {
        addr->IC_DATA_CMD = 1 << 8;
    }

    return 0;
}

/**
  \brief       Start transmitting data as IIC Slave.
  \param[in]   handle  iic handle to operate.
  \param[in]   data  Pointer to buffer with data to transmit to IIC Master
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t csi_iic_slave_send(iic_handle_t handle, const void *data, uint32_t num)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    iic_priv->tx_buf          = (uint8_t *)data;
    iic_priv->tx_total_num    = num;
    iic_priv->tx_cnt          = 0;
    iic_priv->status          = IIC_STATE_DATASEND;

    dw_iic_disable(addr);
    /* open corresponding interrupts */
    addr->IC_INTR_MASK =  DW_IIC_INTR_RD_REQ;

    addr->IC_CLR_INTR;

    dw_iic_enable(addr);

    return 0;
}

/**
  \fn          int32_t  csi_iic_slave_receive (iic_handle_t handle, const void *data, uint32_t num)
  \brief       Start receiving data as IIC Slave.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from IIC Master
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_iic_slave_receive(iic_handle_t handle, void *data, uint32_t num)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->rx_buf          = (uint8_t *)data;
    iic_priv->rx_total_num      = num;
    iic_priv->rx_cnt          = 0;
    iic_priv->status          = IIC_STATE_WFDATA;

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
    dw_iic_disable(addr);

#if defined CONFIG_DW_DMAC || defined CONFIG_CK_DMAC

    if (iic_priv->rx_total_num >= DW_IIC_FIFO_MAX_LV) {
        iic_priv->dma_rx_ch = csi_dma_alloc_channel();

        if (iic_priv->dma_rx_ch != -1) {
            iic_priv->dma_use = 1;
            drv_irq_disable(iic_priv->irq);
            return dw_iic_receive_dma(iic_priv, data, num);
        }
    }

#endif
    addr->IC_INTR_MASK = DW_IIC_INTR_STOP_DET | DW_IIC_INTR_RX_FULL | DW_IIC_INTR_RD_REQ;
    addr->IC_CLR_INTR;

    /* Sets receive FIFO threshold */
    if (iic_priv->rx_total_num >  4) {
        addr->IC_RX_TL        = 4 - 1;
    } else {
        addr->IC_RX_TL        = iic_priv->rx_total_num - 1;
    }

    dw_iic_enable(addr);

    return 0;
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_abort_transfer(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    dw_iic_disable(addr);

    if (iic_priv->status == IIC_STATE_DATASEND) {
        addr->IC_FIFO_RST_EN = DW_IIC_FIFO_RST_EN;
    }

    iic_priv->rx_cnt          = 0;
    iic_priv->tx_cnt          = 0;
    iic_priv->rx_buf          = NULL;
    iic_priv->tx_buf          = NULL;
    return 0;
}


/**
  \brief       Get IIC status.
  \param[in]   handle  iic handle to operate.
  \return      IIC status \ref iic_status_t
*/
iic_status_t csi_iic_get_status(iic_handle_t handle)
{
    iic_status_t iic_status = {0};

    if (handle == NULL) {
        return iic_status;
    }

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);


    if ((iic_priv->status == IIC_STATE_DATASEND) || (iic_priv->status == IIC_STATE_WFDATA)) {
        iic_status.busy = 1;
    }

    if (iic_priv->status == IIC_STATE_WFDATA) {
        iic_status.direction = 1;
    }

    if (addr->IC_RAW_INTR_STAT & 0x800) {
        iic_status.general_call = 1;
    }

    if (iic_priv->status == IIC_STATE_ERROR) {
        iic_status.bus_error = 1;
    }

    return iic_status;
}

/**
  \brief       Get IIC transferred data count.
  \param[in]   handle  iic handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_iic_get_data_count(iic_handle_t handle)
{
    uint32_t cnt = 0;

    if (handle == NULL) {
        return 0;
    }

    dw_iic_priv_t *iic_priv = handle;

    if ((iic_priv->status == IIC_STATE_WFDATA) || (iic_priv->status == IIC_STATE_RECV_DONE)) {
        cnt = iic_priv->rx_cnt;
    } else if ((iic_priv->status == IIC_STATE_DATASEND) || (iic_priv->status == IIC_STATE_SEND_DONE)) {
        cnt = iic_priv->tx_cnt;
    }

    return cnt;
}

/**
  \brief       control IIC power.
  \param[in]   handle  iic handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_iic_power_control(iic_handle_t handle, csi_power_stat_e state)
{
    IIC_NULL_PARAM_CHK(handle);

#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
#endif
}
/**
  \brief       Send START command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_start(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Send STOP command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_stop(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Reset IIC peripheral.
  \param[in]   handle  iic handle to operate.
  \return      error code
  \note The action here. Most of the implementation sends stop.
*/
int32_t csi_iic_reset(iic_handle_t handle)
{
    return csi_iic_send_stop(handle);
}
