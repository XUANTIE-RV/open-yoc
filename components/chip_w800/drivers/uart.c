/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     dw_usart.c
 * @brief    CSI Source File for usart Driver
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @name     dw-uart
 * @ip_id    0x111000010
 * @model    uart
 * @tag      DRV_DW_UART_TAG
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include <drv/irq.h>
#include <drv/usart.h>
#include <drv/pmu.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/dmac.h>

#include "wm_type_def.h"
#include "wm_uart.h"
#include "sys_freq.h"

#define ERR_UART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

#define UART_MAX_FIFO		   0x10

#define UART_TXEN_BIT			(0x40)
#define UART_RXEN_BIT			(0x80)
#define UART_PARITYEN_BIT		(0x08)
#define UART_PARITYODD_BIT		(0x10)
#define UART_BITSTOP_VAL		(0x03)                  /// 1 stop-bit; no crc; 8 data-bits

typedef struct {
    __IOM uint32_t UR_LC;
    __IOM uint32_t UR_FC;
    __IOM uint32_t UR_DMAC;
    __IOM uint32_t UR_FIFOC;
    __IOM uint32_t UR_BD;
    __IOM uint32_t UR_INTM;
    __IOM uint32_t UR_INTS;
    __IOM uint32_t UR_FIFOS;
    __IOM uint32_t UR_TXW;                    /**< tx windows register */
    __IOM uint32_t UR_RES0;
    __IOM uint32_t UR_RES1;
    __IOM uint32_t UR_RES2;
    __IOM uint32_t UR_RXW;   
} dw_usart_reg_t;


typedef struct {
    uint32_t base;
    uint32_t irq;
    usart_event_cb_t cb_event;           ///< Event callback
    int32_t idx;

    uint32_t rx_total_num;
    uint32_t tx_total_num;

    uint8_t *rx_buf;
    uint8_t *tx_buf;

    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;

    volatile uint32_t tx_busy;
    volatile uint32_t rx_busy;

    uint32_t last_tx_num;
    uint32_t last_rx_num;
} dw_usart_priv_t;

extern int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static dw_usart_priv_t usart_instance[CONFIG_USART_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};


#if 1
int tls_uart_output_char(int ch)
{
#if WM_CONFIG_DEBUG_UART1
	tls_reg_write32(HR_UART1_INT_MASK, 0x3);
	if(ch == '\n')	
	{
		while (tls_reg_read32(HR_UART1_FIFO_STATUS)&0x3F);
		tls_reg_write32(HR_UART1_TX_WIN, '\r');
	}
	while(tls_reg_read32(HR_UART1_FIFO_STATUS)&0x3F);
	tls_reg_write32(HR_UART1_TX_WIN, (char)ch);
	tls_reg_write32(HR_UART1_INT_MASK, 0x0);

#elif  WM_CONFIG_DEBUG_UART2
	tls_reg_write32(HR_UART2_INT_MASK, 0x3);
	if(ch == '\n')	
	{
		while (tls_reg_read32(HR_UART2_FIFO_STATUS)&0x3F);
		tls_reg_write32(HR_UART2_TX_WIN, '\r');
	}
	while(tls_reg_read32(HR_UART2_FIFO_STATUS)&0x3F);
	tls_reg_write32(HR_UART2_TX_WIN, (char)ch);
	tls_reg_write32(HR_UART2_INT_MASK, 0x0);
	
#else
	tls_reg_write32(HR_UART0_INT_MASK, 0x3);
    if(ch == '\n')  
	{
		while (tls_reg_read32(HR_UART0_FIFO_STATUS)&0x3F);
		tls_reg_write32(HR_UART0_TX_WIN, '\r');
    }
    while(tls_reg_read32(HR_UART0_FIFO_STATUS)&0x3F);
    tls_reg_write32(HR_UART0_TX_WIN, (char)ch);
    tls_reg_write32(HR_UART0_INT_MASK, 0x0);
#endif		
    return ch;
}

#if USE_UART0_PRINT
void uart0Init (int bandrate)
{
	unsigned int bd;

	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_ClearPendingIRQ(UART0_IRQn);

	bd = (APB_CLK/(16*bandrate) - 1)|(((APB_CLK%(bandrate*16))*16/(bandrate*16))<<16);
	tls_reg_write32(HR_UART0_BAUD_RATE_CTRL, bd);

	tls_reg_write32(HR_UART0_LINE_CTRL, UART_BITSTOP_VAL | UART_TXEN_BIT | UART_RXEN_BIT);
	tls_reg_write32(HR_UART0_FLOW_CTRL, 0x00);   			/* Disable afc */
	tls_reg_write32(HR_UART0_DMA_CTRL, 0x00);             		/* Disable DMA */
	tls_reg_write32(HR_UART0_FIFO_CTRL, 0x00);             		/* one byte TX/RX */
//	tls_reg_write32(HR_UART0_INT_MASK, 0x00);             		/* Disable INT */
}
#endif
#endif

static void wm_uart_reg_init(int32_t idx)
{
    int i;

    if (0 == idx)
    {
        /* disable auto flow control */
        tls_reg_write32(HR_UART0_FLOW_CTRL, 0);
        /* disable dma */
        tls_reg_write32(HR_UART0_DMA_CTRL, 0);
        /* one byte tx */
        tls_reg_write32(HR_UART0_FIFO_CTRL, 0);
        /* disable interrupt */
        tls_reg_write32(HR_UART0_INT_MASK, 0xFF);
        /* enable rx/timeout interrupt */
        tls_reg_write32(HR_UART0_INT_MASK, ~(3 << 2));
    }
    else
    {
    	for (i = TLS_UART_1; i < TLS_UART_MAX; i++)
    	{
            /* 4 byte tx, 8 bytes rx */
            tls_reg_write32(HR_UART0_FIFO_CTRL + i * (HR_UART1_BASE_ADDR - HR_UART0_BASE_ADDR), (0x01 << 2) | (0x02 << 4));
            /* enable rx timeout, disable rx dma, disable tx dma */
            tls_reg_write32(HR_UART0_DMA_CTRL  + i * (HR_UART1_BASE_ADDR - HR_UART0_BASE_ADDR), (8 << 3) | (1 << 2));
            /* enable rx/timeout interrupt */
            tls_reg_write32(HR_UART0_INT_MASK  + i * (HR_UART1_BASE_ADDR - HR_UART0_BASE_ADDR), ~(3 << 2));
    	}
    }
}

static void uart_int_send_data(dw_usart_priv_t *usart_priv)
{
    if (usart_priv->tx_total_num == 0) {
        return;
    }

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        usart_priv->last_tx_num = usart_priv->tx_total_num;

        usart_priv->tx_cnt = 0;
        usart_priv->tx_busy = 0;
        usart_priv->tx_buf = NULL;
        usart_priv->tx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(usart_priv->idx, USART_EVENT_SEND_COMPLETE);
        }
    } else {
        uint32_t remain_txdata  = usart_priv->tx_total_num - usart_priv->tx_cnt;
        uint32_t txdata_num = (remain_txdata > (UART_MAX_FIFO - 1)) ? (UART_MAX_FIFO - 1) : remain_txdata;
        uint32_t i = 0u;

        for (i = 0; i < txdata_num; i++) {
            addr->UR_TXW = *((uint8_t *)usart_priv->tx_buf);
            usart_priv->tx_cnt++;
            usart_priv->tx_buf++;
        }
    }
}

static void uart_int_recv_data(uint32_t intr_src, dw_usart_priv_t *usart_priv)
{
    uint8_t ch = 0;
    dw_usart_reg_t *addr;
    uint32_t rxfifo_num;
    uint32_t rxdata_num;

    ch = ch; /* for warnning */

    if ((usart_priv->rx_total_num == 0) || (usart_priv->rx_buf == NULL)) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVED);
        } else {
            uint8_t data;
            csi_usart_receive_query(usart_priv, &data, 1);
        }

        return;
    }

    addr = (dw_usart_reg_t *)(usart_priv->base);
    rxfifo_num = (addr->UR_FIFOS >> 6) & 0x3F;
    rxdata_num = (rxfifo_num > usart_priv->rx_total_num) ? usart_priv->rx_total_num : rxfifo_num;

    while (rxdata_num-- > 0)
    {
        ch = (uint8_t)addr->UR_RXW;
        if (intr_src & UART_RX_ERR_INT_FLAG)
        {
            //addr->UR_INTS |= UART_RX_ERR_INT_FLAG;
            continue;
        }
        *((uint8_t *)usart_priv->rx_buf) = ch;
        usart_priv->rx_cnt++;
        usart_priv->rx_buf++;
    }

    if (usart_priv->rx_cnt >= usart_priv->rx_total_num) {
        usart_priv->last_rx_num = usart_priv->rx_total_num;
        usart_priv->rx_cnt = 0;
        usart_priv->rx_buf = NULL;
        usart_priv->rx_busy = 0;
        usart_priv->rx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVE_COMPLETE);
        }
    }
}

static int wm_find_out_int_uart(void)
{
	int i;
	u32 value;
	
	for (i = TLS_UART_2; i < TLS_UART_MAX; i++)
	{
		value = tls_reg_read32(HR_UART0_INT_SRC + i * (HR_UART1_BASE_ADDR - HR_UART0_BASE_ADDR));
		value &= 0x1FF;
		if( value )
			break;
	}

	return i;
}

void uart_irqhandler(int32_t idx)
{
    uint32_t intr_src;
    dw_usart_priv_t *usart_priv;
    dw_usart_reg_t *addr;

    if (idx > 1)
    {
        idx = wm_find_out_int_uart();
    }

    usart_priv = &usart_instance[idx];
    addr = (dw_usart_reg_t *)(usart_priv->base);

    /* check interrupt status */
    intr_src = addr->UR_INTS;

    if ((intr_src & UART_RX_INT_FLAG) && (0 == (addr->UR_INTM & UIS_RX_FIFO)))
    {
        uart_int_recv_data(intr_src, usart_priv);
    }

    if (intr_src & UART_TX_INT_FLAG)
    {
        uart_int_send_data(usart_priv);
    }

    addr->UR_INTS = intr_src;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx usart index
  \return      \ref usart_capabilities_t
*/
usart_capabilities_t csi_usart_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USART_NUM) {
        usart_capabilities_t ret;
        memset(&ret, 0, sizeof(usart_capabilities_t));
        return ret;
    }

    return usart_capabilities;
}

/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   idx usart index
  \param[in]   cb_event  Pointer to \ref usart_event_cb_t
  \return      return usart handle if success
*/
usart_handle_t csi_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;
    int32_t ret = target_usart_init(idx, &base, &irq, &handler);

    if (ret < 0 || ret >= CONFIG_USART_NUM) {
        return NULL;
    }

    dw_usart_priv_t *usart_priv = &usart_instance[idx];
    usart_priv->base = base;
    usart_priv->irq = irq;
    usart_priv->cb_event = cb_event;
    usart_priv->idx = idx;
    usart_priv->tx_cnt = 0;
    usart_priv->rx_cnt = 0;

    wm_uart_reg_init(idx);

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    addr->UR_INTS = 0xFFFFFFFF;

    /* enable interupt */
    addr->UR_INTM = 0x0;
    addr->UR_DMAC = (4UL << UDMA_RX_FIFO_TIMEOUT_SHIFT) | UDMA_RX_FIFO_TIMEOUT;

    /* config FIFO control */
    addr->UR_FIFOC = UFC_TX_FIFO_LVL_16_BYTE | UFC_RX_FIFO_LVL_16_BYTE | UFC_TX_FIFO_RESET | UFC_RX_FIFO_RESET;
    addr->UR_LC &= ~(ULCON_TX_EN | ULCON_RX_EN);
    addr->UR_LC |= ULCON_TX_EN | ULCON_RX_EN;

    drv_irq_register(usart_priv->irq, handler);
    drv_irq_enable(usart_priv->irq);

    return usart_priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_uninitialize(usart_handle_t handle)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = handle;
    drv_irq_disable(usart_priv->irq);
    drv_irq_unregister(usart_priv->irq);
    usart_priv->cb_event   = NULL;

    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   baud      baud rate
  \param[in]   mode      \ref usart_mode_e
  \param[in]   parity    \ref usart_parity_e
  \param[in]   stopbits  \ref usart_stop_bits_e
  \param[in]   bits      \ref usart_data_bits_e
  \return      error code
*/
int32_t csi_usart_config(usart_handle_t handle,
                         uint32_t baud,
                         usart_mode_e mode,
                         usart_parity_e parity,
                         usart_stop_bits_e stopbits,
                         usart_data_bits_e bits)
{
    u32 value;
    u32 apbclk;
    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    apbclk = drv_get_apb_freq(0);
    value = (apbclk / (16 * baud) - 1) |
            (((apbclk % (baud * 16)) * 16 / (baud * 16)) << 16);
    addr->UR_BD = value;

    if (USART_MODE_ASYNCHRONOUS != mode)
        return ERR_UART(DRV_ERROR_PARAMETER);

    if (parity == USART_PARITY_NONE)
        addr->UR_LC &= ~ULCON_PMD_EN;
    else if (parity == USART_PARITY_EVEN)
    {
        addr->UR_LC &= ~ULCON_PMD_MASK;
        addr->UR_LC |= ULCON_PMD_EVEN;
    }
    else if (parity == USART_PARITY_ODD)
    {
        addr->UR_LC &= ~ULCON_PMD_MASK;
        addr->UR_LC |= ULCON_PMD_ODD;
    }
    else
        return ERR_UART(DRV_ERROR_PARAMETER);

    if (stopbits == USART_STOP_BITS_2)
        addr->UR_LC |= ULCON_STOP_2;
    else if (stopbits == USART_STOP_BITS_1)
        addr->UR_LC &= ~ULCON_STOP_2;
    else
        return ERR_UART(DRV_ERROR_PARAMETER);

    addr->UR_LC &= ~ULCON_WL_MASK;

    if (bits == USART_DATA_BITS_5)
        addr->UR_LC |= ULCON_WL5;
    else if (bits == USART_DATA_BITS_6)
        addr->UR_LC |= ULCON_WL6;
    else if (bits == USART_DATA_BITS_7)
        addr->UR_LC |= ULCON_WL7;
    else if (bits == USART_DATA_BITS_8)
        addr->UR_LC |= ULCON_WL8;
    else
        return ERR_UART(DRV_ERROR_PARAMETER);

    return 0;
}

/**
  \brief       Start sending data to UART transmitter,(received data is ignored).
               The function is non-blocking,UART_EVENT_TRANSFER_COMPLETE is signaled when transfer completes.
               csi_usart_get_status can indicates if transmission is still in progress or pending
  \param[in]   handle  usart handle to operate.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t csi_usart_send(usart_handle_t handle, const void *data, uint32_t num)
{
    if (!handle || !data)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

#if 1
    usart_priv->tx_buf = (uint8_t *)data;
    usart_priv->tx_total_num = num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 1;
    usart_priv->last_tx_num = 0;

    addr->UR_TXW = *((uint8_t *)usart_priv->tx_buf);
    usart_priv->tx_cnt++;
    usart_priv->tx_buf++;
#else
    int i;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    for (i = 0; i < num; i++)
    {
        while (addr->UR_FIFOS & UFS_TX_FIFO_CNT_MASK) __NOP();
        addr->UR_TXW = *((uint8_t *)data + i);
    }

    usart_priv->tx_cnt += num;

    if (usart_priv->cb_event)
    {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_SEND_COMPLETE);
    }
#endif

    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_send(usart_handle_t handle)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = handle;

    usart_priv->tx_cnt = usart_priv->tx_total_num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 0;
    usart_priv->tx_buf = NULL;
    usart_priv->tx_total_num = 0;

    return 0;
}


/**
  \brief       Start receiving data from UART receiver.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    if (!handle || !data)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;

#if 1
    usart_priv->rx_buf = (uint8_t *)data;   // Save receive buffer usart
    usart_priv->rx_total_num = num;         // Save number of data to be received
    usart_priv->rx_cnt = 0;
    usart_priv->rx_busy = 1;
    usart_priv->last_rx_num = 0;
#else
    int i;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    for (i = 0; i < num; i++)
    {
        while (!(addr->UR_FIFOS & UFS_RX_FIFO_CNT_MASK)) __NOP();
        *((uint8_t *)data + i) = addr->UR_RXW;
    }

    usart_priv->rx_cnt += num;

    if (usart_priv->cb_event)
    {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVE_COMPLETE);
    }
#endif

    return 0;
}

/**
  \brief       query data from UART receiver FIFO.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      receive fifo data num
*/
int32_t csi_usart_receive_query(usart_handle_t handle, void *data, uint32_t num)
{
    if (!handle || !data)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    int32_t recv_num = 0;
    uint8_t *dest = (uint8_t *)data;

    while (addr->UR_FIFOS & UFS_RX_FIFO_CNT_MASK)
    {
        *dest++ = addr->UR_RXW;
        recv_num++;

        if (recv_num >= num) {
            break;
        }
    }

    return recv_num;
}

int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    if (!handle || !ch)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    while (!(addr->UR_FIFOS & UFS_RX_FIFO_CNT_MASK)) __NOP();
    *ch = addr->UR_RXW;

    return 0;
}

int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    while (addr->UR_FIFOS & UFS_TX_FIFO_CNT_MASK) __NOP();
    addr->UR_TXW = (char)ch;

    return 0;
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_receive(usart_handle_t handle)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = handle;

    usart_priv->rx_cnt = usart_priv->rx_total_num;

    return 0;
}

/**
  \brief       Start sending/receiving data to/from UART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \return      error code
*/
int32_t csi_usart_transfer(usart_handle_t handle, const void *data_out, void *data_in, uint32_t num)
{
    return ERR_UART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_transfer(usart_handle_t handle)
{
    return ERR_UART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t csi_usart_get_status(usart_handle_t handle)
{
    usart_status_t status = {0};

    if (!handle)
        return status;

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    //if (addr->UR_FIFOS & UFS_TX_FIFO_CNT_MASK)
    //    status.tx_busy = 1;
    //if (addr->UR_FIFOS & UFS_RX_FIFO_CNT_MASK)
    //    status.rx_busy = 1;
    status.tx_busy = usart_priv->tx_busy;
    status.rx_busy = usart_priv->rx_busy;
    if (addr->UR_INTS & UIS_OVERRUN)
         status.rx_overflow = 1;
    if (addr->UR_INTS & UIS_BREAK)
         status.rx_break = 1;
    if (addr->UR_INTS & UIS_FRM_ERR)
         status.rx_framing_error = 1;
    if (addr->UR_INTS & UIS_PARITY_ERR)
         status.rx_parity_error = 1;
    if (addr->UR_LC & ULCON_TX_EN)
         status.tx_enable = 1;
    if (addr->UR_LC & ULCON_RX_EN)
         status.rx_enable = 1;

    return status;
}

/**
  \brief       control the transmit.
  \param[in]   handle  usart handle to operate.
  \param[in]   1 - enable the transmitter. 0 - disable the transmitter
  \return      error code
*/
int32_t csi_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (enable)
        addr->UR_LC |= ULCON_TX_EN;
    else
        addr->UR_LC &= ~ULCON_TX_EN;

    return 0;
}

/**
  \brief       control the receive.
  \param[in]   handle  usart handle to operate.
  \param[in]   1 - enable the receiver. 0 - disable the receiver
  \return      error code
*/
int32_t csi_usart_control_rx(usart_handle_t handle, uint32_t enable)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (enable)
        addr->UR_LC |= ULCON_RX_EN;
    else
        addr->UR_LC &= ~ULCON_RX_EN;

    return 0;
}

/**
  \brief       control the break.
  \param[in]   handle  usart handle to operate.
  \param[in]   1- Enable continuous Break transmission,0 - disable continuous Break transmission
  \return      error code
*/
int32_t csi_usart_control_break(usart_handle_t handle, uint32_t enable)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (enable)
        addr->UR_LC |= (1 << 5);
    else
        addr->UR_LC &= ~(1 << 5);

    return 0;
}

/**
  \brief       flush receive/send data.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_flush_type_e.
  \return      error code
*/
int32_t csi_usart_flush(usart_handle_t handle, usart_flush_type_e type)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    if (USART_FLUSH_WRITE == type)
    {

    }
    else if (USART_FLUSH_READ == type)
    {

    }
    else
        return ERR_UART(DRV_ERROR_PARAMETER);

    return 0;
}

/**
  \brief       set interrupt mode.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_intr_type_e.
  \param[in]   flag 0-OFF, 1-ON.
  \return      error code
*/
int32_t csi_usart_set_interrupt(usart_handle_t handle, usart_intr_type_e type, int32_t flag)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (USART_INTR_WRITE == type)
    {
        if (flag)
        {
            addr->UR_INTM &= ~(0x1 << 1);
        }
        else
        {
            addr->UR_INTM |= (0x1 << 1);
        }
    }
    else if (USART_INTR_READ == type)
    {
        if (flag)
        {
            addr->UR_INTM &= ~((0x1 << 2) | (0x01 << 8));
        }
        else
        {
            addr->UR_INTM |= ((0x1 << 2) | (0x01 << 8));
        }
    }
    else
        return ERR_UART(DRV_ERROR_PARAMETER);
 
    return 0;
}

/**
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t csi_usart_get_tx_count(usart_handle_t handle)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = handle;

    if (usart_priv->tx_busy)
    {
        return usart_priv->tx_cnt;
    }
    else
    {
        return usart_priv->last_tx_num;
    }
}

/**
  \brief       Get usart receive data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently received data bytes
*/
uint32_t csi_usart_get_rx_count(usart_handle_t handle)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = handle;

    if (usart_priv->rx_busy)
    {
        return usart_priv->rx_cnt;
    }
    else
    {
        return usart_priv->last_rx_num;
    }
}

/**
  \brief       control usart power.
  \param[in]   handle  usart handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_usart_power_control(usart_handle_t handle, csi_power_stat_e state)
{
    return ERR_UART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config usart flow control type.
  \param[in]   handle  usart handle to operate.
  \param[in]   flowctrl_type   flow control type.\ref usart_flowctrl_type_e.
  \return      error code
*/
int32_t csi_usart_config_flowctrl(usart_handle_t handle,
                                  usart_flowctrl_type_e flowctrl_type)
{
    if (!handle)
        return ERR_UART(DRV_ERROR_PARAMETER);

    dw_usart_priv_t *usart_priv = (dw_usart_priv_t *)handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (USART_FLOWCTRL_CTS_RTS == flowctrl_type)
    {
        addr->UR_FC = (1UL << 0) | (6UL << 2);
    }
    else if (USART_FLOWCTRL_NONE == flowctrl_type)
    {
        addr->UR_FC = 0;
    }
    else
        return ERR_UART(DRV_ERROR_PARAMETER);

    return 0;

}


/**
  \brief       config usart clock Polarity and Phase.
  \param[in]   handle  usart handle to operate.
  \param[in]   cpol    Clock Polarity.\ref usart_cpol_e.
  \param[in]   cpha    Clock Phase.\ref usart_cpha_e.
  \return      error code
*/
int32_t csi_usart_config_clock(usart_handle_t handle, usart_cpol_e cpol, usart_cpha_e cpha)
{
    return ERR_UART(DRV_ERROR_UNSUPPORTED);
}
