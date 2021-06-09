/**
 * @file    wm_hostspi.c
 *
 * @brief   host spi Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_gpio.h"
#include "wm_hostspi.h"
#include "wm_dma.h"
#include "wm_dbg.h"
#include "wm_mem.h"
#include "wm_cpu.h"
#include "wm_spi_hal.h"
#include "wm_wl_task.h"
#include "tls_common.h"
#include "core_804.h"

//#define  ATTRIBUTE_ISR __attribute__((isr))

static struct tls_spi_port *spi_port = NULL;

#ifdef SPI_USE_DMA
static void spi_dma_init(u8 mode, u32 fclk)
{
    tls_sys_clk sysclk;

    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS)
        ;

    tls_sys_clk_get(&sysclk);

    SPIM_CLKCFG_REG = sysclk.apbclk * UNIT_MHZ / (fclk * 2) - 1;
    SPIM_SPICFG_REG = 0;
    SPIM_SPICFG_REG = SPI_FRAME_FORMAT_MOTO | SPI_SET_MASTER_SLAVE(SPI_MASTER) | mode;

    /* Disable all interrupt request */
    SPIM_INTEN_REG = 0xff;

    tls_dma_init();
}

static int spi_wait_idle(void)
{
    unsigned long regVal;
    unsigned long timeout = 0;

    do
    {
        timeout++;
        if (timeout > 0x4FFFFF) // 5s
            return TLS_SPI_STATUS_EBUSY;
        regVal = SPIM_SPISTATUS_REG;
    } while (regVal & (1 << 12));

    return TLS_SPI_STATUS_OK;
}

static int spi_block_write_dma(u8 *data, u32 len, u8 ifusecmd, u32 cmd)
{
    unsigned char dmaCh = 0;
    struct tls_dma_descriptor DmaDesc;
    u32 txlen, txlenbk;
    u32 i, blocknum, blocksize = 0;
    int ret = TLS_SPI_STATUS_OK;
    int txcmdover = 0;

    if (NULL == data)
    {
        return TLS_SPI_STATUS_EINVAL;
    }
    if (spi_wait_idle())
        return TLS_SPI_STATUS_EBUSY;
    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS)
        ;

    if (ifusecmd)
        SPIM_TXDATA_REG = cmd;

    if (len % 4)
    {
        txlen = len & 0xfffffffc; // 4-byte alignment
    }
    else
    {
        txlen = len;
    }

    txlenbk = txlen;
    if (txlen > 0)
    {
        blocknum = txlen / SPI_DMA_MAX_TRANS_SIZE;

        /* Request DMA Channel */
        dmaCh = tls_dma_request(1, TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_LSSPI_TX) |
                                TLS_DMA_FLAGS_HARD_MODE);
        for (i = 0; i <= blocknum; i++)
        {
            DmaDesc.src_addr = (int)(data + i * SPI_DMA_MAX_TRANS_SIZE);
            DmaDesc.dest_addr = HR_SPI_TXDATA_REG;
            blocksize = (txlen > SPI_DMA_MAX_TRANS_SIZE) ? SPI_DMA_MAX_TRANS_SIZE : txlen;

            if (0 == blocksize)
                break;
            DmaDesc.dma_ctrl = TLS_DMA_DESC_CTRL_SRC_ADD_INC | TLS_DMA_DESC_CTRL_DATA_SIZE_WORD |
                               TLS_DMA_DESC_CTRL_TOTAL_BYTES(blocksize);
            DmaDesc.valid = TLS_DMA_DESC_VALID;
            DmaDesc.next = NULL;
            tls_dma_start(dmaCh, &DmaDesc, 0);
            /* Enable SPI TX DMA */
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0) | SPI_TX_DMA_ON;
            SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32)0xffff);

            if (ifusecmd && 0 == i)
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                                 SPI_VALID_CLKS_NUM(((blocksize + 4) * 8)); // add 4 byte cmd
                txcmdover = 1;
            }
            else
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                                 SPI_VALID_CLKS_NUM((blocksize * 8));
            }
            if (spi_wait_idle())
            {
                ret = TLS_SPI_STATUS_EBUSY;
                break;
            }
            /* Wait Dma Channel Complete and Free Dma channel */
            if (tls_dma_wait_complt(dmaCh))
            {
                ret = TLS_SPI_STATUS_EBUSY;
                break;
            }
            txlen -= blocksize;
        }
        tls_dma_free(dmaCh);
    }
    /*The remaining bytes*/
    if (len > txlenbk)
    {
        u32 word32 = 0;
        int temp = 0;
        for (i = 0; i < (len - txlenbk); i++)
        {
            word32 |= (data[txlenbk + i] << (i * 8));
        }
        SPIM_TXDATA_REG = word32;
        SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0);
        SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32)0xffff);
        if (ifusecmd && 0 == txcmdover) //// add 4 byte cmd
            temp = 4;
        SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE |
                         SPI_START | SPI_VALID_CLKS_NUM(((temp + len - txlenbk) * 8));
        if (spi_wait_idle())
        {
            ret = TLS_SPI_STATUS_EBUSY;
        }
    }
    SPIM_CHCFG_REG = 0x00000000;
    SPIM_MODECFG_REG = 0x00000000;
    SPIM_SPITIMEOUT_REG = 0x00000000;

    return ret;
}

static int spi_block_read_dma(u8 *data, u32 len, u8 *txdata, u8 txlen)
{
    unsigned char dmaCh = 0;
    struct tls_dma_descriptor DmaDesc;
    u32 word32 = 0;
    u32 i;
    u32 rxlen, rxlenbk;
    u8 blocknum;
    u32 blocksize = 0;
    int ret = TLS_SPI_STATUS_OK;

    if (NULL == data)
    {
        return TLS_SPI_STATUS_EINVAL;
    }
    if (spi_wait_idle())
    {
        return TLS_SPI_STATUS_EBUSY;
    }

    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS)
        ;

    if (len % 4)
    {
        rxlen = len & 0xfffffffc; // 4-byte alignment
    }
    else
    {
        rxlen = len;
    }
    rxlenbk = rxlen;
    blocknum = rxlen / SPI_DMA_MAX_TRANS_SIZE;

    if (txlen > 0 && txlen <= 32)
    {
        for (i = 0; i < txlen; i++)
        {
            if ((i > 0) && (0 == i % 4))
            {
                SPIM_TXDATA_REG = word32;
                word32 = 0;
            }
            word32 |= (txdata[i] << ((i % 4) * 8));
        }
        SPIM_TXDATA_REG = word32;
    }

    /* Request DMA Channel */
    dmaCh = tls_dma_request(1, TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_LSSPI_RX) | TLS_DMA_FLAGS_HARD_MODE);
    DmaDesc.src_addr = HR_SPI_RXDATA_REG;
    for (i = 0; i <= blocknum; i++)
    {
        /*receive data more than 4 bytes*/
        if (rxlenbk > 0)
        {
            DmaDesc.dest_addr = (int)(data + i * SPI_DMA_MAX_TRANS_SIZE);
            blocksize = (rxlen > SPI_DMA_MAX_TRANS_SIZE) ? SPI_DMA_MAX_TRANS_SIZE : rxlen;
            if (0 == blocksize)
                break;
            DmaDesc.dma_ctrl = TLS_DMA_DESC_CTRL_DEST_ADD_INC | TLS_DMA_DESC_CTRL_BURST_SIZE1 |
                               TLS_DMA_DESC_CTRL_DATA_SIZE_WORD | TLS_DMA_DESC_CTRL_TOTAL_BYTES(blocksize);
            DmaDesc.valid = TLS_DMA_DESC_VALID;
            DmaDesc.next = NULL;
            tls_dma_start(dmaCh, &DmaDesc, 0);
            /* Enable SPI RX DMA */
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0) | SPI_RX_DMA_ON;
        }
        else
        {
            /*less than 4 bytes no need DMA*/
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0);
        }
        SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32)0xffff);
        if (0 == blocknum)
        {
            SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE |
                             SPI_START | SPI_VALID_CLKS_NUM(((len + txlen) * 8)) | SPI_RX_INVALID_BITS(txlen * 8);
        }
        else
        {
            if (0 == i)
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE |
                                 SPI_START | SPI_VALID_CLKS_NUM(((blocksize + txlen) * 8)) | SPI_RX_INVALID_BITS(txlen * 8);
            }
            else if (i == blocknum)
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                                 SPI_VALID_CLKS_NUM((blocksize + len - rxlenbk) * 8);
            }
            else
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                                 SPI_VALID_CLKS_NUM(blocksize * 8);
            }
        }
        if (spi_wait_idle())
        {
            ret = TLS_SPI_STATUS_EBUSY;
            break;
        }

        /* Wait Dma Channel Complete and Free Dma channel */
        if (tls_dma_wait_complt(dmaCh))
        {
            ret = TLS_SPI_STATUS_EBUSY;
            break;
        }
        rxlen -= blocksize;
    }
    tls_dma_free(dmaCh);

    /*The remaining bytes*/
    if (len > rxlenbk)
    {
        word32 = SPIM_RXDATA_REG;
        *((int *)data + rxlenbk / 4) = word32;
    }
    SPIM_CHCFG_REG = 0x00000000;
    SPIM_MODECFG_REG = 0x00000000;
    SPIM_SPITIMEOUT_REG = 0x00000000;
    return ret;
}
#endif /* SPI_USE_DMA */

static void spi_message_init(struct tls_spi_message *m)
{
    memset(m, 0, sizeof(*m));
    dl_list_init(&m->transfers);
}

u32 spi_fill_txfifo(struct tls_spi_transfer *current_transfer, u32 current_remaining_bytes)
{
    u8 fifo_level;
    u16 rw_words;
    u16 rw_bytes;
    u8 data8;
    u8 i;
    u32 data32 = 0;
    u32 tx_remaining_bytes;
    if ((current_transfer == NULL) || (current_remaining_bytes == 0))
        return 0;

    tx_remaining_bytes = current_remaining_bytes;

    spi_get_status(NULL, NULL, &fifo_level);

    rw_words = ((fifo_level > tx_remaining_bytes) ? tx_remaining_bytes : fifo_level) / 4;
    rw_bytes = ((fifo_level > tx_remaining_bytes) ? tx_remaining_bytes : fifo_level) % 4;

    for (i = 0; i < rw_words; i++)
    {
        if (current_transfer->tx_buf)
        {
            if (SPI_BYTE_TRANSFER == spi_port->transtype)
            {
                data32 = 0;
                data32 |= ((u8 *)current_transfer->tx_buf + (current_transfer->len - tx_remaining_bytes))[0] << 24;
                data32 |= ((u8 *)current_transfer->tx_buf + (current_transfer->len - tx_remaining_bytes))[1] << 16;
                data32 |= ((u8 *)current_transfer->tx_buf + (current_transfer->len - tx_remaining_bytes))[2] << 8;
                data32 |= ((u8 *)current_transfer->tx_buf + (current_transfer->len - tx_remaining_bytes))[3] << 0;
            }
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                data32 = *((u32 *)((u8 *)current_transfer->tx_buf + current_transfer->len - tx_remaining_bytes));
            }
        }
        else
        {
            data32 = 0xffffffff;
        }
        spi_data_put(data32);
        tx_remaining_bytes -= 4;
    }

    if (rw_bytes)
    {
        data32 = 0;
        for (i = 0; i < rw_bytes; i++)
        {
            if (current_transfer->tx_buf)
            {
                data8 = ((u8 *)current_transfer->tx_buf)[current_transfer->len - tx_remaining_bytes];
            }
            else
            {
                data8 = 0xff;
            }
            if (SPI_BYTE_TRANSFER == spi_port->transtype)
            {
                data32 |= data8 << ((3 - i) * 8);
            }
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                data32 |= data8 << (i * 8);
            }
            tx_remaining_bytes -= 1;
        }

        spi_data_put(data32);
    }

    return (current_remaining_bytes - tx_remaining_bytes);
}

u32 spi_get_rxfifo(struct tls_spi_transfer *current_transfer, u32 current_remaining_bytes)
{
    u8 fifo_level;
    u8 rw_words;
    u8 rw_bytes;
    u8 data8 = 0;
    u8 i;
    u32 data32;
    u32 rx_remaining_bytes;

    if ((current_transfer == NULL) || (current_remaining_bytes == 0))
        return 0;

    rx_remaining_bytes = current_remaining_bytes;
    spi_get_status(NULL, &fifo_level, NULL);

    rw_words = fifo_level / 4;
    rw_bytes = fifo_level % 4;

    for (i = 0; i < rw_words; i++)
    {

        data32 = spi_data_get();
        if (current_transfer->rx_buf)
        {
            if (SPI_BYTE_TRANSFER == spi_port->transtype)
            {
                data32 = swap_32(data32);
                (((u8 *)current_transfer->rx_buf + (current_transfer->rx_len - rx_remaining_bytes)))[0] = (u8)data32;
                (((u8 *)current_transfer->rx_buf + (current_transfer->rx_len - rx_remaining_bytes)))[1] = (u8)(data32 >> 8);
                (((u8 *)current_transfer->rx_buf + (current_transfer->rx_len - rx_remaining_bytes)))[2] = (u8)(data32 >> 16);
                (((u8 *)current_transfer->rx_buf + (current_transfer->rx_len - rx_remaining_bytes)))[3] = (u8)(data32 >> 24);
            }
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                *((u32 *)((u8 *)current_transfer->rx_buf + current_transfer->rx_len - rx_remaining_bytes)) = data32;
            }
        }
        rx_remaining_bytes -= 4;
    }

    if (rw_bytes)
    {
        data32 = spi_data_get();
        if (current_transfer->rx_buf)
        {
            for (i = 0; i < rw_bytes; i++)
            {
                if (SPI_BYTE_TRANSFER == spi_port->transtype)
                {
                    data8 = (u8)(data32 >> ((3 - i) * 8));
                }
                else if (SPI_WORD_TRANSFER == spi_port->transtype)
                {
                    data8 = (u8)(data32 >> (i * 8));
                }
                ((u8 *)current_transfer->rx_buf)[current_transfer->rx_len - rx_remaining_bytes] = data8;
                rx_remaining_bytes -= 1;
            }
        }
        else
        {
            rx_remaining_bytes -= rw_bytes;
        }
    }

    return (current_remaining_bytes - rx_remaining_bytes);
}

static struct tls_spi_transfer *spi_next_transfer(struct tls_spi_message *current_message)
{
    if (current_message == NULL)
    {
        return NULL;
    }

    return dl_list_first(&current_message->transfers, struct tls_spi_transfer, transfer_list);
}

static struct tls_spi_message *spi_next_message(void)
{
    struct tls_spi_message *current_message;

    current_message = dl_list_first(&spi_port->wait_queue, struct tls_spi_message, queue);
    if (current_message == NULL)
    {
        return NULL;
    }

    spi_port->current_transfer = spi_next_transfer(current_message);
    current_message->status = SPI_MESSAGE_STATUS_INPROGRESS;

    return current_message;
}

void spi_start_transfer(u32 transfer_bytes)
{
    if (spi_port->reconfig)
    {
        TLS_DBGPRT_SPI_INFO("reconfig the spi master controller.\n");
        spi_set_mode(spi_port->mode);
        spi_set_chipselect_mode(spi_port->cs_active);
        spi_set_sclk(spi_port->speed_hz);

        spi_port->reconfig = 0;
    }

    spi_set_sclk_length(transfer_bytes * 8, 0);
    spi_set_chipselect_mode(SPI_CS_ACTIVE_MODE);
    spi_sclk_start();
}

void spi_stop_transfer(void)
{
    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
}


static void spi_continue_transfer(void)
{
    struct tls_spi_message *current_message;
    struct tls_spi_transfer *current_transfer;
    u32 transfer_bytes;

    current_message = spi_port->current_message;
    current_transfer = spi_port->current_transfer;

    if ((current_message == NULL) || (current_transfer == NULL))
    {
        return;
    }
    transfer_bytes = spi_get_rxfifo(current_transfer, spi_port->current_remaining_bytes);

    spi_port->current_remaining_bytes -= transfer_bytes;
    if (spi_port->current_remaining_bytes == 0)
    {
        dl_list_del(&current_transfer->transfer_list);
        spi_port->current_transfer = spi_next_transfer(spi_port->current_message);
        if (spi_port->current_transfer == NULL)
        {
            spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
            current_message->status = SPI_MESSAGE_STATUS_DONE;
            dl_list_del(&current_message->queue);
            spi_port->current_message = spi_next_message();
        }

        current_transfer = spi_port->current_transfer;
        if (current_transfer != NULL)
        {
            spi_port->current_remaining_bytes = current_transfer->len;
        }
    }

    transfer_bytes = spi_fill_txfifo(current_transfer, spi_port->current_remaining_bytes);

    if (transfer_bytes)
    {
        spi_start_transfer(transfer_bytes);
    }

    if (current_message->status == SPI_MESSAGE_STATUS_DONE)
    {
        current_message->complete(current_message->context);
    }
}

static int add_message_to_wait_list(struct tls_spi_message *message)
{
    u8 first_transfer = 0;
    u32 transfer_bytes;
    struct tls_spi_transfer *current_transfer;
    struct tls_spi_transfer *transfer;

    if (spi_port == NULL)
    {
        TLS_DBGPRT_ERR("spi master driver module not beed installed!\n");
        return TLS_SPI_STATUS_ESHUTDOWN;
    }

    if ((message == NULL) || (dl_list_empty(&message->transfers)))
    {
        TLS_DBGPRT_ERR("@message is NULL or @message->transfers is empty!\n");
        return TLS_SPI_STATUS_EINVAL;
    }

    dl_list_for_each(transfer, &message->transfers, struct tls_spi_transfer,
                     transfer_list)
    {
        if (transfer->len == 0)
        {
            TLS_DBGPRT_ERR("\"@transfer->len\" belong to @message is 0!\n");
            return TLS_SPI_STATUS_EINVAL;
        }
    }

    if (dl_list_empty(&spi_port->wait_queue))
    {
        first_transfer = 1;
    }

    message->status = SPI_MESSAGE_STATUS_IDLE;
    dl_list_add_tail(&spi_port->wait_queue, &message->queue);


    if (first_transfer == 1)
    {
        if (spi_port->current_message)
        {
            TLS_DBGPRT_WARNING("spi transaction scheduler is running now!\n");
            return TLS_SPI_STATUS_EBUSY;
        }

        TLS_DBGPRT_SPI_INFO("acquire the first transaction message in waiting queue.\n");

        spi_port->current_message = spi_next_message();


        current_transfer = spi_port->current_transfer;
        if (current_transfer == NULL)
        {
            return TLS_SPI_STATUS_EINVAL;
        }
        spi_port->current_remaining_bytes = current_transfer->len;

        transfer_bytes = spi_fill_txfifo(current_transfer, spi_port->current_remaining_bytes);

        spi_start_transfer(transfer_bytes);
    }

    return TLS_SPI_STATUS_OK;
}

static int tls_spi_async(struct tls_spi_transfer *x, uint32_t num)
{
	struct tls_spi_message message;
	int i = 0;
	int status;
	spi_message_init(&message);
	for (i = 0; i< num; i++)
	{
        dl_list_add_tail(&message.transfers, &x[i].transfer_list);		
	}
    status = add_message_to_wait_list(&message);
    if (status == TLS_SPI_STATUS_OK)
    {
        TLS_DBGPRT_SPI_INFO("waiting spi transaction finishing!\n");
    }
	
    return status;
}

/**
 * @brief          This function is used to set SPI transfer mode.
 *
 * @param[in]      type     is the transfer type.
 *                 type == SPI_BYTE_TRANSFER    byte transfer
 *                 type == SPI_WORD_TRANSFER    word transfer
 *                 type == SPI_DMA_TRANSFER     DMA transfer
 *
 * @return         None
 *
 * @note           None
 */
void tls_spi_trans_type(u8 type)
{

    spi_port->transtype = type;
    if (SPI_WORD_TRANSFER == type)
    {
        spi_set_endian(0);
    }
    else if (SPI_BYTE_TRANSFER == type)
    {
        spi_set_endian(1);
    }
    else if (SPI_DMA_TRANSFER == type)
    {
#ifdef SPI_USE_DMA
        spi_dma_init(spi_port->mode, spi_port->speed_hz);
#endif
    }
}

/**
 * @brief          This function is used to initialize the SPI master driver.
 *
 * @param[in]      None
 *
 * @retval         TLS_SPI_STATUS_OK        if initialize success
 * @retval         TLS_SPI_STATUS_EBUSY     if SPI is already initialized
 * @retval         TLS_SPI_STATUS_ENOMEM    if malloc SPI memory fail
 *
 * @note           None
 */
int tls_spi_init(void)
{
    u8 err;
    struct tls_spi_port *port;

    if (spi_port != NULL)
    {
        TLS_DBGPRT_ERR("spi driver module has been installed!\n");
        return TLS_SPI_STATUS_EBUSY;
    }

    TLS_DBGPRT_SPI_INFO("initialize spi master driver module.\n");

    port = (struct tls_spi_port *)tls_mem_alloc(sizeof(struct tls_spi_port));
    if (port == NULL)
    {
        TLS_DBGPRT_ERR("allocate \"struct tls_spi_port\" fail!\n");
        return TLS_SPI_STATUS_ENOMEM;
    }

    port->speed_hz  = SPI_DEFAULT_SPEED;
    port->cs_active = SPI_CS_ACTIVE_MODE;
    port->mode      = SPI_DEFAULT_MODE;
    port->reconfig  = 0;

    dl_list_init(&port->wait_queue);

    port->current_message = NULL;
    port->current_remaining_transfer = 0;
    port->current_transfer = NULL;
    port->current_remaining_bytes = 0;

    spi_port = port;

    TLS_DBGPRT_SPI_INFO("initialize spi master controller.\n");

    spi_clear_fifo();
    tls_spi_trans_type(SPI_BYTE_TRANSFER);
    spi_set_mode(spi_port->mode);
    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);

    /* cs pin control by software*/
    spi_force_cs_out(1);
    spi_set_sclk(spi_port->speed_hz);

    spi_set_tx_trigger_level(0);
    spi_set_rx_trigger_level(7);

    spi_set_rx_channel(1);
    spi_set_tx_channel(1);
    spi_unmask_int(SPI_INT_TRANSFER_DONE);

    TLS_DBGPRT_SPI_INFO("register spi master interrupt handler.\n");

    tls_irq_enable(SPI_LS_IRQn);

    TLS_DBGPRT_SPI_INFO("spi master driver module initialization finish.\n");

    return TLS_SPI_STATUS_OK;
}

/**
 * @brief          This function is used to setup the spi CPOL,CPHA,cs signal and clock.
 *
 * @param[in]      mode         is CPOL and CPHA type defined in TLS_SPI_MODE_0 to TLS_SPI_MODE_3
 * @param[in]      cs_active    is cs mode, defined as TLS_SPI_CS_LOW or TLS_SPI_CS_HIGH
 * @param[in]      fclk            is spi clock,the unit is HZ.
 *
 * @retval         TLS_SPI_STATUS_OK                if setup success
 * @retval         TLS_SPI_STATUS_EMODENOSUPPORT    if mode is not support
 * @retval         TLS_SPI_STATUS_EINVAL            if cs_active is not support
 * @retval         TLS_SPI_STATUS_ECLKNOSUPPORT     if fclk is not support
 *
 * @note           None
 */
int tls_spi_setup(u8 mode, u8 cs_active, u32 fclk)
{
    tls_sys_clk sysclk;

    if ((spi_port->mode == mode) && (spi_port->cs_active == cs_active) && (spi_port->speed_hz == fclk))
    {
        TLS_DBGPRT_WARNING("@mode, @cs_activer, @fclk is the same as settings of the current spi master driver!\n");
        return TLS_SPI_STATUS_OK;
    }

    switch (mode)
    {
    case TLS_SPI_MODE_0:
    case TLS_SPI_MODE_1:
    case TLS_SPI_MODE_2:
    case TLS_SPI_MODE_3:
        spi_port->mode = mode;
        break;

    default:
        TLS_DBGPRT_ERR("@mode is invalid!\n");
        return TLS_SPI_STATUS_EMODENOSUPPORT;
    }

    if ((cs_active != TLS_SPI_CS_HIGH) && (cs_active != TLS_SPI_CS_LOW))
    {
        TLS_DBGPRT_ERR("@cs_active  is invalid!\n");
        return TLS_SPI_STATUS_EINVAL;
    }
    else
    {
        spi_port->cs_active = cs_active;
    }

    tls_sys_clk_get(&sysclk);

    if ((fclk < TLS_SPI_FCLK_MIN) || (fclk > sysclk.apbclk * UNIT_MHZ / 2)) //TLS_SPI_FCLK_MAX
    {
        TLS_DBGPRT_ERR("@fclk is invalid!\n");
        return TLS_SPI_STATUS_ECLKNOSUPPORT;
    }
    else
    {
        spi_port->speed_hz = fclk;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        spi_dma_init(mode, fclk);
        return TLS_SPI_STATUS_OK;
    }
#endif

    spi_port->reconfig = 1;

    return TLS_SPI_STATUS_OK;
}

static int32_t tls_spi_xfer(const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    int ret = TLS_SPI_STATUS_OK;
	uint32_t tx_length = 0;
    uint32_t int_status;	
    struct tls_spi_transfer tls_transfer;
	uint32_t tot_num = 0;
    uint32_t total_tx_length ;	
	uint32_t total_rx_length = 0;
	uint32_t total_discard_rx_byte = 0;
	uint32_t rx_length;

	uint8_t *tx_buf = NULL;

    if (spi_port == NULL || data_in == NULL || data_out == NULL || num_out == 0 || num_in == 0) {
        return -1;
    }

   	tls_transfer.tx_buf = data_out;

	tls_transfer.rx_buf = data_in;
	tls_transfer.rx_len = num_in;
	total_rx_length = num_in;

    tot_num = (num_out > num_in) ? num_out : num_in+num_out;
	total_tx_length = tot_num;
    tls_transfer.len = tot_num;	
	tls_irq_disable(SPI_LS_IRQn);
    if (spi_port->reconfig)
    {
        spi_set_mode(spi_port->mode);
        spi_set_chipselect_mode(spi_port->cs_active);
        spi_set_sclk(spi_port->speed_hz);
        spi_port->reconfig = 0;
    }

	tx_length = spi_fill_txfifo(&tls_transfer, total_tx_length);
	if (num_out < tx_length)
	{
		spi_set_sclk_length(tx_length * 8, num_out*8);
		total_discard_rx_byte = 0;
	}
	else
	{
		spi_set_sclk_length(tx_length * 8, tx_length*8);		
		total_discard_rx_byte = num_out - tx_length;
	}
	
    spi_set_chipselect_mode(SPI_CS_ACTIVE_MODE);
    spi_sclk_start();

	do{
		if (total_rx_length)
		{
			while (spi_i2s_get_busy_status() == 1);
			rx_length = spi_get_rxfifo(&tls_transfer, total_rx_length);
			total_rx_length = (total_rx_length > rx_length) ? (total_rx_length - rx_length):0;
			if ((total_rx_length == 0) && (num_in == num_out))
			{
				break;
			}
		}
		total_tx_length -= tx_length;
		if (total_tx_length == 0)
		{
			break;
		}
		
		while (spi_i2s_get_busy_status() == 1);
		tx_length = spi_fill_txfifo(&tls_transfer, total_tx_length);
		if (tx_length)
		{
			
			if (total_discard_rx_byte > tx_length)
			{
				spi_set_sclk_length(tx_length * 8, tx_length*8);			
				total_discard_rx_byte -= tx_length;
			}
			else
			{
				spi_set_sclk_length(tx_length * 8, total_discard_rx_byte*8);	
				total_discard_rx_byte = 0;
			}

			spi_sclk_start();
		}
	}while(total_tx_length > 0);

	while (spi_i2s_get_busy_status() == 1);
	int_status = spi_get_int_status();
	spi_clear_int_status(int_status);
    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
	tls_irq_enable(SPI_LS_IRQn);

    return (ret == TLS_SPI_STATUS_OK) ? 0 : -1;
}	


static int32_t tls_spi_tx(const void *data_out, uint32_t num_out,uint32_t conitueflag)
{
    int ret = TLS_SPI_STATUS_OK;
	uint32_t tx_length = 0;
	uint32_t rx_length = 0;
    uint32_t int_status;	
    struct tls_spi_transfer tls_transfer;
	uint32_t tot_num = 0;
    uint32_t total_tx_length ;

	uint32_t total_discard_rx_byte = 0;

	uint8_t *tx_buf = NULL;

    if (spi_port == NULL || data_out == NULL || num_out == 0) {
        return -1;
    }

   	tls_transfer.tx_buf = data_out;

	tls_transfer.rx_buf = NULL;
	tls_transfer.rx_len = 0;

    tot_num = num_out;
	total_tx_length = tot_num;
    tls_transfer.len = tot_num;	
	tls_irq_disable(SPI_LS_IRQn);
    if (spi_port->reconfig)
    {
        spi_set_mode(spi_port->mode);
        spi_set_chipselect_mode(spi_port->cs_active);
        spi_set_sclk(spi_port->speed_hz);
        spi_port->reconfig = 0;
    }
	
	while(total_tx_length)
	{
		tx_length = spi_fill_txfifo(&tls_transfer, total_tx_length);
		if (tx_length)
		{		
			spi_set_sclk_length(tx_length * 8, tx_length * 8);
			spi_set_chipselect_mode(SPI_CS_ACTIVE_MODE);
			spi_sclk_start();
		}

		while (spi_i2s_get_busy_status() == 1);

		rx_length = spi_get_rxfifo(&tls_transfer,total_tx_length);
		while (spi_i2s_get_busy_status() == 1);

		total_tx_length -= tx_length;
	}
	if (conitueflag == 0)
	{
		int_status = spi_get_int_status();
		spi_clear_int_status(int_status);
	    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
		tls_irq_enable(SPI_LS_IRQn);
	}

    return (ret == TLS_SPI_STATUS_OK) ? 0 : -1;
}	

static int32_t tls_spi_rx(void *data_in, uint32_t num_in, uint32_t conitueflag)
{
    int ret = TLS_SPI_STATUS_OK;
	uint32_t tx_length = 0;
    uint32_t int_status;	
    struct tls_spi_transfer tls_transfer;
    uint32_t total_tx_length ;	
	uint32_t total_rx_length = 0;
	uint32_t total_discard_rx_byte = 0;
	uint32_t rx_length;

	uint8_t *tx_buf = NULL;

    if (spi_port == NULL || data_in == NULL ||  num_in == 0) {
        return -1;
    }
	tx_buf = tls_mem_alloc(num_in);
	if (tx_buf == NULL)
	{
		return -1;
	}
	memset(tx_buf, 0xFF, num_in);
   	tls_transfer.tx_buf = tx_buf;

	tls_transfer.rx_buf = data_in;
	tls_transfer.rx_len = num_in;
	total_rx_length = num_in;

	total_tx_length = num_in;
    tls_transfer.len = num_in;	
	tls_irq_disable(SPI_LS_IRQn);
    if (spi_port->reconfig)
    {
        spi_set_mode(spi_port->mode);
        spi_set_chipselect_mode(spi_port->cs_active);
        spi_set_sclk(spi_port->speed_hz);
        spi_port->reconfig = 0;
    }

	tx_length = spi_fill_txfifo(&tls_transfer, total_tx_length);
	spi_set_sclk_length(tx_length * 8, 0);

    spi_set_chipselect_mode(SPI_CS_ACTIVE_MODE);
    spi_sclk_start();

	do{
		if (total_rx_length)
		{
			while (spi_i2s_get_busy_status() == 1);
			rx_length = spi_get_rxfifo(&tls_transfer, total_rx_length);
			total_rx_length = (total_rx_length > rx_length) ? (total_rx_length - rx_length):0;
			if (total_rx_length == 0)
			{
				while (spi_i2s_get_busy_status() == 1);
				break;
			}
		}
		total_tx_length -= tx_length;
		if (total_tx_length == 0)
		{
			while (spi_i2s_get_busy_status() == 1);
			break;
		}
		
		while (spi_i2s_get_busy_status() == 1);
		tx_length = spi_fill_txfifo(&tls_transfer, total_tx_length);
		if (tx_length)
		{
			spi_set_sclk_length(tx_length * 8, 0);			
			spi_sclk_start();
		}
	}while(total_tx_length > 0);

	if (conitueflag == 0)
	{
		while (spi_i2s_get_busy_status() == 1);
		int_status = spi_get_int_status();
		spi_clear_int_status(int_status);
	    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
		tls_irq_enable(SPI_LS_IRQn);
	}
	tls_mem_free(tx_buf);
    return (ret == TLS_SPI_STATUS_OK) ? 0 : -1;
}


/**
 * @brief          This function is used to synchronous read data by SPI.
 *
 * @param[in]      buf          is the buffer for saving SPI data.
 * @param[in]      len          is the data length.
 *
 * @retval         TLS_SPI_STATUS_OK            if write success.
 * @retval         TLS_SPI_STATUS_EINVAL        if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM        if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN     if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_read(u8 *buf, u32 len)
{

    if ((buf == NULL) || (len == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (spi_port && SPI_DMA_TRANSFER == spi_port->transtype)
    {
        u32 data32 = 0;
        u16 rxBitLen;
        u32 rdval1 = 0;
        u32 i;

        if (len <= 4)
        {
            SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
            while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS)
                ;

            rxBitLen = 8 * len;
            rdval1 = SPI_FORCE_SPI_CS_OUT | SPI_CS_LOW | SPI_TX_CHANNEL_ON | SPI_RX_CHANNEL_ON |
                     SPI_CONTINUE_MODE | SPI_START | SPI_VALID_CLKS_NUM(rxBitLen);
            SPIM_CHCFG_REG = rdval1;
            spi_wait_idle();
            SPIM_CHCFG_REG |= SPI_CS_HIGH;

            data32 = SPIM_RXDATA_REG;

            for (i = 0; i < len; i++)
            {
                *(buf + i) = (u8)(data32 >> i * 8);
            }
            SPIM_CHCFG_REG = 0x00000000;
            SPIM_MODECFG_REG = 0x00000000;
        }
        else
        {
            if (len > SPI_DMA_BUF_MAX_SIZE)
            {
                TLS_DBGPRT_ERR("\nread len too long\n");

                return TLS_SPI_STATUS_EINVAL;
            }
            spi_block_read_dma((u8 *)SPI_DMA_BUF_ADDR, len, NULL, 0);
            MEMCPY(buf, (u8 *)SPI_DMA_BUF_ADDR, len);
        }

        return TLS_SPI_STATUS_OK;
    }
#endif

	return tls_spi_rx(buf,len,0);
}

/**
 * @brief          This function is used to synchronous write data by SPI.
 *
 * @param[in]      buf          is the user data.
 * @param[in]      len          is the data length.
 *
 * @retval         TLS_SPI_STATUS_OK            if write success.
 * @retval         TLS_SPI_STATUS_EINVAL        if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM        if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN     if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_write(const u8 *buf, u32 len)
{

    if ((buf == NULL) || (len == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (spi_port && SPI_DMA_TRANSFER == spi_port->transtype)
    {
        u32 data32 = 0;
        u16 txBitLen;
        u32 rdval1 = 0;
        u32 i;
        if (len <= 4)
        {
            SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
            while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS)
                ;
            for (i = 0; i < len; i++)
            {
                data32 |= (((u8)(buf[i])) << (i * 8));
            }
            SPIM_TXDATA_REG = data32;
            txBitLen = 8 * len;
            rdval1 = SPI_FORCE_SPI_CS_OUT | SPI_CS_LOW | SPI_TX_CHANNEL_ON | SPI_RX_CHANNEL_ON |
                     SPI_CONTINUE_MODE | SPI_START | SPI_VALID_CLKS_NUM(txBitLen);
            SPIM_CHCFG_REG = rdval1;
            spi_wait_idle();
            SPIM_CHCFG_REG |= SPI_CS_HIGH;

            SPIM_CHCFG_REG = 0x00000000;
            SPIM_MODECFG_REG = 0x00000000;
        }
        else
        {
            if (len > SPI_DMA_BUF_MAX_SIZE)
            {
                TLS_DBGPRT_ERR("\nwrite len too long\n");
                return TLS_SPI_STATUS_EINVAL;
            }
            MEMCPY((u8 *)SPI_DMA_BUF_ADDR, buf, len);
            spi_block_write_dma((u8 *)SPI_DMA_BUF_ADDR, len, 0, 0);
        }
        return TLS_SPI_STATUS_OK;
    }

#endif

	return tls_spi_tx(buf, len, 0);
}

/**
 * @brief          This function is used to synchronous write 32bit command then write data by SPI.
 *
 * @param[in]      cmd                     is the command data.
 * @param[in]      n_cmd                   is the command len,can not bigger than four
 * @param[in]      txbuf                   is the write data buffer.
 * @param[in]      n_tx                    is the write data length.
 *
 * @retval         TLS_SPI_STATUS_OK            if write success.
 * @retval         TLS_SPI_STATUS_EINVAL        if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM        if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN     if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_write_with_cmd(const u8 *cmd, u32 n_cmd, const u8 *txbuf,
                           u32 n_tx)
{
    int status;

    if ((cmd == NULL) || (n_cmd == 0) || (txbuf == NULL) || (n_tx == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (spi_port && SPI_DMA_TRANSFER == spi_port->transtype)
    {
        if (n_tx > SPI_DMA_BUF_MAX_SIZE)
        {
            TLS_DBGPRT_ERR("\nwriten len too long\n");
            return TLS_SPI_STATUS_EINVAL;
        }

        MEMCPY((u8 *)SPI_DMA_BUF_ADDR, (u8 *)cmd, n_cmd);
        MEMCPY((u8 *)(SPI_DMA_BUF_ADDR + n_cmd), txbuf, n_tx);
        spi_block_write_dma((u8 *)SPI_DMA_BUF_ADDR, (n_cmd + n_tx), 0, 0);

        return TLS_SPI_STATUS_OK;
    }
#endif

	status = tls_spi_tx(cmd, n_cmd, 1);
	status |= tls_spi_tx(txbuf, n_tx, 0);

    return status;
}

/**
 * @brief          This function is used to synchronous write command then read data by SPI.
 *
 * @param[in]      txbuf        is the write data buffer.
 * @param[in]      n_tx         is the write data length.
 * @param[in]      rxbuf        is the read data buffer.
 * @param[in]      n_rx         is the read data length.
 *
 * @retval         TLS_SPI_STATUS_OK            if write success.
 * @retval         TLS_SPI_STATUS_EINVAL        if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM        if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN     if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_read_with_cmd(const u8 *txbuf, u32 n_tx, u8 *rxbuf, u32 n_rx)
{
    int status;

    if ((txbuf == NULL) || (n_tx == 0) || (rxbuf == NULL) || (n_rx == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (spi_port && SPI_DMA_TRANSFER == spi_port->transtype)
    {
        if (n_rx > SPI_DMA_BUF_MAX_SIZE || n_tx > SPI_DMA_CMD_MAX_SIZE)
        {
            TLS_DBGPRT_ERR("\nread length too long\n");
            return TLS_SPI_STATUS_EINVAL;
        }
        MEMCPY((u8 *)SPI_DMA_CMD_ADDR, txbuf, n_tx);
        spi_block_read_dma((u8 *)SPI_DMA_BUF_ADDR, n_rx, (u8 *)SPI_DMA_CMD_ADDR,
                           n_tx);
        MEMCPY(rxbuf, (u8 *)SPI_DMA_BUF_ADDR, n_rx);
        return TLS_SPI_STATUS_OK;
    }
#endif

	tls_spi_xfer(txbuf, rxbuf, n_tx, n_rx);

    return status;
}

void __SPI_LS_IRQHandler(void)
{

    u32 int_status;
    u32 int_mask;

    int_status = spi_get_int_status();
    spi_clear_int_status(int_status);

    int_mask = spi_int_mask();
    int_status &= ~int_mask;
#if 0
    if (int_status & SPI_INT_TRANSFER_DONE)
    {
        spi_continue_transfer();
    }
#endif	
}
