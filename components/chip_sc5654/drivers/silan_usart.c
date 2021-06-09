/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_usart.c
 * @brief    CSI Source File for usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <addrspace.h>
#include <drv/usart.h>
#include <silan_usart.h>
#include <csi_core.h>
#include <pinmux.h>
#include <silan_pic.h>
#include <syscfg.h>

#define ERR_USART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

#define UART1_CLKON_BIT 6
#define UART2_CLKON_BIT 5
#define UART3_CLKON_BIT 31
#define UART4_CLKON_BIT 0


#define USART_NULL_PARAM_CHK(para)              \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_USART(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

#define WAIT_USART_IDLE(addr)\
    do {                       \
        int32_t timecount = 0;  \
        while ((addr->UARTFR & SILAN_UART_FR_BUSY) && (timecount < UART_BUSY_TIMEOUT)) {\
            timecount++;\
        }\
        if (timecount >= UART_BUSY_TIMEOUT) {\
            return ERR_USART(DRV_ERROR_TIMEOUT);\
        }                                   \
    } while(0)

typedef struct {
    uint32_t base;
    uint32_t irq;
    usart_event_cb_t cb_event;           ///< Event callback
    void *cb_arg;
    uint32_t rx_total_num;
    uint32_t tx_total_num;
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;
    volatile uint32_t tx_busy;
    volatile uint32_t rx_busy;
    //for get data count
    uint32_t last_tx_num;
    uint32_t last_rx_num;
    int32_t idx;
    uint32_t fcr_reg;
} silan_usart_priv_t;

extern void mdelay(uint32_t ms);
extern int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t target_usart_tx_flowctrl_init(int32_t idx, uint32_t flag);
extern int32_t target_usart_rx_flowctrl_init(int32_t idx, uint32_t flag);

#ifdef CONFIG_CHIP_SC6138
static const uint8_t  g_uart_clkon[] = {UART1_CLKON_BIT, UART2_CLKON_BIT, UART3_CLKON_BIT, UART4_CLKON_BIT};
#endif

static silan_usart_priv_t usart_instance[CONFIG_USART_NUM];

static usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};

/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usart_priv usart private to operate.
*/
static void silan_usart_intr_threshold_empty(int32_t idx, silan_usart_priv_t *usart_priv)
{
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    addr->UARTICR |= SILAN_UART_INT_TX;

    if (usart_priv->tx_total_num == 0) {
        return;
    }

    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        addr->UARTIMSC &= ~SILAN_UART_INT_TX;
        usart_priv->last_tx_num = usart_priv->tx_total_num;

        usart_priv->tx_cnt = 0;
        usart_priv->tx_busy = 0;
        usart_priv->tx_buf = NULL;
        usart_priv->tx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_SEND_COMPLETE);
        }
    } else {
        uint32_t remain_txdata  = usart_priv->tx_total_num - usart_priv->tx_cnt;
        uint32_t txdata_num = (remain_txdata > (SILAN_UART_MAX_FIFO - 1)) ? (SILAN_UART_MAX_FIFO - 1) : remain_txdata;
        uint32_t i = 0u;

        for (i = 0; i < txdata_num; i++) {
            addr->UARTDR = *((uint8_t *)usart_priv->tx_buf);
            usart_priv->tx_cnt++;
            usart_priv->tx_buf++;
        }
    }

    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        addr->UARTIMSC &= ~SILAN_UART_INT_TX;
        usart_priv->last_tx_num = usart_priv->tx_total_num;
        usart_priv->tx_cnt = 0;
        usart_priv->tx_busy = 0;
        usart_priv->tx_buf = NULL;
        usart_priv->tx_total_num = 0;
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_SEND_COMPLETE);
        }
    }
}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   usart_priv usart private to operate.
*/
static void silan_usart_intr_recv_data(int32_t idx, silan_usart_priv_t *usart_priv)
{
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    addr->UARTICR |= SILAN_UART_INT_RX;

    if ((usart_priv->rx_total_num == 0) || (usart_priv->rx_buf == NULL)) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
        } else {
            uint8_t data;
            csi_usart_receive_query(usart_priv, &data, 1);
        }
        return;
    }

    uint32_t rxfifo_num = SILAN_UART_MAX_FIFO / 2;
    uint32_t rxdata_num = (rxfifo_num > usart_priv->rx_total_num) ? usart_priv->rx_total_num : rxfifo_num;
    uint32_t i;

    for (i = 0; i < rxdata_num; i++) {
        *((uint8_t *)usart_priv->rx_buf) = addr->UARTDR & 0xff;
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
            usart_priv->cb_event(idx, USART_EVENT_RECEIVE_COMPLETE);
        }
    }
}

/**
  \brief        interrupt service function for character timeout.
  \param[in]   usart_priv usart private to operate.
*/
static void silan_usart_intr_char_timeout(int32_t idx, silan_usart_priv_t *usart_priv)
{
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    addr->UARTICR |= SILAN_UART_INT_RT;

    if ((usart_priv->rx_total_num != 0) && (usart_priv->rx_buf != NULL)) {
        silan_usart_intr_recv_data(idx, usart_priv);
        return;
    }

    if (usart_priv->cb_event) {
        usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
    } else {
        uint32_t timecount = 0;
        while (!(addr->UARTFR & SILAN_UART_FR_RXFE)) {
            addr->UARTDR;
            timecount++;
            if (timecount >= UART_BUSY_TIMEOUT) {
                return;
            }
        }
    }
}

/**
  \brief       the interrupt service function.
  \param[in]   index of usart instance.
*/
void silan_usart_irqhandler(int32_t idx)
{
    silan_usart_priv_t *usart_priv = &usart_instance[idx];
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    uint8_t intr_state = addr->UARTFR & 0x7f0;
    uint32_t mask_intr_state = addr->UARTMIS;

    if (mask_intr_state & SILAN_UART_INT_OE) {
        addr->UARTICR |= SILAN_UART_INT_OE;
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_OVERFLOW);
        }
    }

    if (mask_intr_state & SILAN_UART_INT_PE) {
        addr->UARTICR |= SILAN_UART_INT_PE;
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_PARITY_ERROR);
        }
    }

    if (mask_intr_state & SILAN_UART_INT_FE) {
        addr->UARTICR |= SILAN_UART_INT_FE;
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_FRAMING_ERROR);
        }
    }

    if ((mask_intr_state & SILAN_UART_INT_TX) && (intr_state & SILAN_UART_FR_TXFE)) {
        silan_usart_intr_threshold_empty(idx, usart_priv);
        return;
    }

    if (mask_intr_state & SILAN_UART_INT_RT) {
        silan_usart_intr_char_timeout(idx, usart_priv);
        return;
    }

    if ((mask_intr_state & SILAN_UART_INT_RX)) {
        silan_usart_intr_recv_data(idx, usart_priv);
        return;
    }
}

/**
  \brief       set the bautrate of usart.
  \param[in]   addr  usart base to operate.
  \param[in]   baudrate.
  \param[in]   apbfreq the frequence of the apb.
  \return      error code
*/
static int32_t silan_usart_set_baudrate(silan_usart_reg_t *addr, uint32_t baudrate, uint32_t apbfreq)
{
    if ((apbfreq == 0) || (baudrate == 0)) {
        return  ERR_USART(USART_ERROR_BAUDRATE);
    }
    /* baudrate=(seriak clock freq)/(16*divisor) */

    int32_t temp = baudrate << 4;

    uint32_t divisor = apbfreq / temp;

    if (divisor < 1) {
        divisor = 1;
    }

    uint32_t remainder = apbfreq % temp;
    temp = (remainder << 3) / baudrate;
    uint32_t fraction = (temp >> 1) + (temp & 0x1);

    addr->UARTIBRD = divisor;
    addr->UARTFBRD = fraction;

    return 0;
}

/**
  \brief       enable or disable parity.
  \param[in]   addr  usart base to operate.
  \param[in]   parity ODD=8, EVEN=16, or NONE=0.
  \return      error code
*/
static int32_t silan_usart_set_parity(silan_usart_reg_t *addr, usart_parity_e parity)
{

    switch (parity) {
        case USART_PARITY_NONE:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_PEN;
            break;

        case USART_PARITY_ODD:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_EPS;
            addr->UARTLCR_H |= SILAN_UARTLCR_H_PEN;
            break;

        case USART_PARITY_EVEN:
            addr->UARTLCR_H |= SILAN_UARTLCR_H_PEN | SILAN_UARTLCR_H_EPS;
            break;

        default:
            return ERR_USART(USART_ERROR_PARITY);
    }

    return 0;
}

/**
  \brief       set the stop bit.
  \param[in]   addr  usart base to operate.
  \param[in]   stopbit two possible value: USART_STOP_BITS_1 and USART_STOP_BITS_2.
  \return      error code
*/
static int32_t silan_usart_set_stopbit(silan_usart_reg_t *addr, usart_stop_bits_e stopbit)
{

    switch (stopbit) {
        case USART_STOP_BITS_1:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_STP2;
            break;

        case USART_STOP_BITS_2:
            addr->UARTLCR_H |= SILAN_UARTLCR_H_STP2;
            break;

        default:
            return ERR_USART(USART_ERROR_STOP_BITS);
    }

    return 0;
}

/**
  \brief       the transmit data length,and we have four choices:5, 6, 7, and 8 bits.
  \param[in]   addr  usart base to operate.
  \param[in]   databits the data length that user decides.
  \return      error code
*/
static int32_t silan_usart_set_databit(silan_usart_reg_t *addr, usart_data_bits_e databits)
{
   /* The word size decides by the UARTLCR_H bits(UARTLCR_H[6:5]), and the
    * corresponding relationship between them is:
    *UARTLCR_H[6:5]   word size
    *           00 -- 5 bits
    *           01 -- 6 bits
    *           10 -- 7 bits
    *           11 -- 8 bits
    */

    switch (databits) {
        case USART_DATA_BITS_5:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_WLEN_8;
            break;

        case USART_DATA_BITS_6:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_WLEN_8;
            addr->UARTLCR_H |= SILAN_UARTLCR_H_WLEN_6;
            break;

        case USART_DATA_BITS_7:
            addr->UARTLCR_H &= ~SILAN_UARTLCR_H_WLEN_8;
            addr->UARTLCR_H |= SILAN_UARTLCR_H_WLEN_7;
            break;

        case USART_DATA_BITS_8:
            addr->UARTLCR_H |= SILAN_UARTLCR_H_WLEN_8;
            break;

        default:
            return ERR_USART(USART_ERROR_DATA_BITS);
    }

    return 0;
}

int32_t silan_usart_clockenable(int32_t idx)
{
#ifdef CONFIG_CHIP_SC6138
    volatile unsigned int clock_reg;
     //enable uart clock
    if(idx == 2)
    {
        clock_reg = SILAN_SYSTEM_CTL_REG14;
    }
    else
    {
        clock_reg = SILAN_SYSTEM_CTL_REG12;
    }

    uint32_t value = REG32(KSEG1(clock_reg));
    value |= (1<<g_uart_clkon[idx]);
    REG32(KSEG1(clock_reg)) = value;

#endif
    return 0;

}

/**
  \brief       Get driver capabilities.
  \param[in]   handle  usart handle to operate.
  \return      \ref usart_capabilities_t
*/
usart_capabilities_t csi_usart_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USART_NUM) {
        usart_capabilities_t ret;
        memset(&ret, 0, sizeof(usart_capabilities_t));
        return ret;
    }

    if (idx == 0) {
        usart_capabilities.rts = 1;
        usart_capabilities.cts = 1;
    } else {
        usart_capabilities.rts = 0;
        usart_capabilities.cts = 0;
    }

    return usart_capabilities;
}

/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   usart pin of tx
  \param[in]   usart pin of rx
  \param[in]   cb_event  Pointer to \ref usart_event_cb_t
  \return      return usart handle if success
*/

usart_handle_t csi_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_usart_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_USART_NUM) {
        return NULL;
    }

    drv_clk_enable(CLK_UART1 + idx);

    silan_usart_priv_t *usart_priv = &usart_instance[idx];
    usart_priv->base = base;
    usart_priv->irq = irq;
    usart_priv->cb_event = cb_event;

    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    usart_priv->idx = idx;

    addr->UARTRSR |= 0xff;
    addr->UARTIMSC |= 0xfffff800;
    addr->UARTICR |= 0x7ff;
    /* disable uart */
    addr->UARTCR &= ~SILAN_UARTCR_RXE;
    addr->UARTCR &= ~SILAN_UARTCR_TXE;
    addr->UARTCR &= ~SILAN_UARTCR_UARTEN;
    silan_usart_clockenable(idx);

    /* FIFO enable */
    addr->UARTLCR_H |= SILAN_UARTLCR_H_FEN;
    addr->UARTIFLS &= ~SILAN_UART_FLS_TX_RX_SEL;
    addr->UARTIFLS |= SILAN_UART_FLS_TXIFLSEL_1 | SILAN_UART_FLS_RXIFLSEL_4;
    addr->UARTIMSC |= SILAN_UART_INT_RX | SILAN_UART_INT_RT | SILAN_UART_INT_OE | SILAN_UART_INT_PE |SILAN_UART_INT_FE;
    addr->UARTIMSC &= ~SILAN_UART_INT_TX;
    addr->UARTCR = SILAN_UARTCR_UARTEN | SILAN_UARTCR_RXE | SILAN_UARTCR_TXE;

    silan_pic_request(PIC_IRQID_MISC, irq, (hdl_t)silan_usart_irqhandler);
    return usart_priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_uninitialize(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    addr->UARTRSR |= 0xff;
    addr->UARTIMSC |= 0xfffff800;
    addr->UARTICR |= 0x7ff;
    /* disable uart */
    addr->UARTCR &= ~SILAN_UARTCR_RXE;
    addr->UARTCR &= ~SILAN_UARTCR_TXE;
    addr->UARTCR &= ~SILAN_UARTCR_UARTEN;

    /* silan_pic_free(PIC_IRQID_MISC, usart_priv->irq); */
    usart_priv->cb_event   = NULL;

    drv_clk_disable(CLK_UART1 + usart_priv->idx);

    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   sysclk    configured system clock.
  \param[in]   mode      \ref usart_mode_e
  \param[in]   parity    \ref usart_parity_e
  \param[in]   stopbits  \ref usart_stop_bits_e
  \param[in]   bits      \ref usart_data_bits_e
  \param[in]   baud      configured baud
  \return      error code
*/
int32_t csi_usart_config(usart_handle_t handle,
                         uint32_t baud,
                         usart_mode_e mode,
                         usart_parity_e parity,
                         usart_stop_bits_e stopbits,
                         usart_data_bits_e bits)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    mdelay(100);

    /* clear Rx FIFO */
    while(!(addr->UARTFR & SILAN_UARTFR_RXFE)) {
        addr->UARTDR;
    }

    /* disable everything */
    uint32_t control_value = addr->UARTCR;
    addr->UARTCR = 0;

    /* control the data_bit of the usart*/
    int32_t ret = silan_usart_set_baudrate(addr, baud, SYSTEM_CLOCK);

    if (ret < 0) {
        goto exit;
    }

    /* control the parity of the usart*/
    ret = silan_usart_set_parity(addr, parity);

    if (ret < 0) {
        goto exit;
    }

    /* control the stopbit of the usart*/
    ret = silan_usart_set_stopbit(addr, stopbits);

    if (ret < 0) {
        goto exit;
    }

    ret = silan_usart_set_databit(addr, bits);

    if (ret < 0) {
        goto exit;
    }

exit:
    addr->UARTCR = control_value;

    return ret;
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
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    if (num == 0) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    silan_usart_priv_t *usart_priv = handle;

    usart_priv->tx_buf = (uint8_t *)data;
    usart_priv->tx_total_num = num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 1;
    usart_priv->last_tx_num = 0;

    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
#if 0
    addr->UARTIMSC |= SILAN_UART_INT_TX | SILAN_UART_INT_FE;
#else       /* blocking mode */
    uint32_t timecount = 0;

    while(usart_priv->tx_total_num--) {
        /* Wait until there is space in the FIFO */
        timecount = 0;
        while (addr->UARTFR & SILAN_UARTFR_TXFF) {
            timecount++;

            if (timecount >= UART_BUSY_TIMEOUT) {
                usart_priv->tx_busy = 0;
                return ERR_USART(DRV_ERROR_TIMEOUT);
            }
        }

        /* Send the character */
        addr->UARTDR = *((uint8_t *)usart_priv->tx_buf);
        usart_priv->tx_cnt++;
        usart_priv->tx_buf++;
        usart_priv->last_tx_num++;
    }
    timecount = 0;
    while (!(addr->UARTFR & SILAN_UARTFR_TXFE)) {
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            usart_priv->tx_busy = 0;
            return ERR_USART(DRV_ERROR_TIMEOUT);
        }
    }

    usart_priv->tx_busy = 0;

    if (usart_priv->cb_event) {
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
    USART_NULL_PARAM_CHK(handle);
    silan_usart_priv_t *usart_priv = handle;

    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    addr->UARTCR &= ~SILAN_UARTCR_TXE;

    usart_priv->tx_cnt = usart_priv->tx_total_num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 0;
    usart_priv->tx_buf = NULL;
    usart_priv->tx_total_num = 0;
    return 0;
}

/**
  \brief       Start receiving data from UART receiver.transmits the default value as specified by csi_usart_set_default_tx_value
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    silan_usart_priv_t *usart_priv = handle;

    usart_priv->rx_buf = (uint8_t *)data;   // Save receive buffer usart
    usart_priv->rx_total_num = num;         // Save number of data to be received
    usart_priv->rx_cnt = 0;
    usart_priv->rx_busy = 1;
    usart_priv->last_rx_num = 0;

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
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    int32_t recv_num = 0;
    uint8_t *dest = (uint8_t *)data;

    while (!(addr->UARTFR & SILAN_UART_FR_RXFE)) {
        *dest++ = addr->UARTDR & 0xff;
        recv_num++;

        if (recv_num >= num) {
            break;
        }
    }

    return recv_num;
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_receive(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    silan_usart_priv_t *usart_priv = handle;

    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    addr->UARTCR &= ~SILAN_UARTCR_RXE;
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
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_transfer(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t csi_usart_get_status(usart_handle_t handle)
{
    if (handle == NULL) {
        usart_status_t ret;
        memset(&ret, 0, sizeof(usart_status_t));
        return ret;
    }

    usart_status_t usart_status;
    memset(&usart_status, 0, sizeof(usart_status_t));
    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    uint32_t line_status_reg    = addr->UARTRSR;

    usart_status.tx_busy = usart_priv->tx_busy;
    usart_status.rx_busy = usart_priv->rx_busy;

    if (line_status_reg & SILAN_UART_RSR_BE) {
        usart_status.rx_break = 1;
    }

    if (line_status_reg & SILAN_UART_RSR_FE) {
        usart_status.rx_framing_error = 1;
    }

    if (line_status_reg & SILAN_UART_RSR_PE) {
        usart_status.rx_parity_error = 1;
    }

    usart_status.tx_enable  = 1;
    usart_status.rx_enable  = 1;

    return usart_status;
}

/**
  \brief       control the transmit.
  \param[in]   handle  usart handle to operate.
  \param[in]   1 - enable the transmitter. 0 - disable the transmitter
  \return      error code
*/
int32_t csi_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    if (enable == 1) {
        addr->UARTCR |= SILAN_UARTCR_TXE;
        return 0;
    } else if (enable == 0) {
        addr->UARTCR &= ~SILAN_UARTCR_TXE;
        return 0;
    }

    return ERR_USART(DRV_ERROR_PARAMETER);
}

/**
  \brief       control the receive.
  \param[in]   handle  usart handle to operate.
  \param[in]   1 - enable the receiver. 0 - disable the receiver
  \return      error code
*/
int32_t csi_usart_control_rx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    if (enable == 1) {
        addr->UARTCR |= SILAN_UARTCR_RXE;
        return 0;
    } else if (enable == 0) {
        addr->UARTCR &= ~SILAN_UARTCR_RXE;
        return 0;
    }

    return ERR_USART(DRV_ERROR_PARAMETER);
}

/**
  \brief       control the break.
  \param[in]   handle  usart handle to operate.
  \param[in]   1- Enable continuous Break transmission,0 - disable continuous Break transmission
  \return      error code
*/
int32_t csi_usart_control_break(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       flush receive/send data.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_flush_type_e.
  \return      error code
*/
int32_t csi_usart_flush(usart_handle_t handle, usart_flush_type_e type)
{
    USART_NULL_PARAM_CHK(handle);

    return ERR_USART(DRV_ERROR_UNSUPPORTED);
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
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    switch (type) {
        case USART_INTR_WRITE:
            if (flag == 0) {
                addr->UARTIMSC &= ~SILAN_UART_INT_TX;
            } else if (flag == 1) {
                addr->UARTIMSC |= SILAN_UART_INT_TX;
            } else {
                return ERR_USART(DRV_ERROR_PARAMETER);
            }

            break;

        case USART_INTR_READ:
            if (flag == 0) {
                addr->UARTIMSC &= ~SILAN_UART_INT_RX;
            } else if (flag == 1) {
                addr->UARTIMSC |= SILAN_UART_INT_RX;
            } else {
                return ERR_USART(DRV_ERROR_PARAMETER);
            }

            break;

        default:
            return ERR_USART(DRV_ERROR_PARAMETER);

    }

    return 0;
}

/**
  \brief       set the baud rate of usart.
  \param[in]   baud  usart base to operate.
  \param[in]   baudrate baud rate
  \return      error code
*/
int32_t csi_usart_config_baudrate(usart_handle_t handle, uint32_t baud)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    return silan_usart_set_baudrate(addr, baud, SYSTEM_CLOCK);
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   mode    \ref usart_mode_e
  \return      error code
*/
int32_t csi_usart_config_mode(usart_handle_t handle, usart_mode_e mode)
{
    USART_NULL_PARAM_CHK(handle);

    if (mode == USART_MODE_ASYNCHRONOUS) {
        return 0;
    }

    return ERR_USART(USART_ERROR_MODE);
}

/**
  \brief       config usart parity.
  \param[in]   handle  usart handle to operate.
  \param[in]   parity    \ref usart_parity_e
  \return      error code
*/
int32_t csi_usart_config_parity(usart_handle_t handle, usart_parity_e parity)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    return silan_usart_set_parity(addr, parity);
}

/**
  \brief       config usart stop bit number.
  \param[in]   handle  usart handle to operate.
  \param[in]   stopbits  \ref usart_stop_bits_e
  \return      error code
*/
int32_t csi_usart_config_stopbits(usart_handle_t handle, usart_stop_bits_e stopbit)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    return silan_usart_set_stopbit(addr, stopbit);
}

/**
  \brief       config usart data length.
  \param[in]   handle  usart handle to operate.
  \param[in]   databits      \ref usart_data_bits_e
  \return      error code
*/
int32_t csi_usart_config_databits(usart_handle_t handle, usart_data_bits_e databits)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    return silan_usart_set_databit(addr, databits);
}

/**
  \brief       get character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[out]  ch the pointer to the received character.
  \return      error code
*/
int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(ch);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    while(addr->UARTFR & SILAN_UARTFR_RXFE);

    *ch = addr->UARTDR & 0xFF;

    return 0;
}

/**
  \brief       transmit character in query mode.
  \param[in]   instance  usart instance to operate.
  \param[in]   ch  the input charater
  \return      error code
*/
int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);
    uint32_t timecount = 0;

    /* Wait until there is space in the FIFO */
    while (addr->UARTFR & SILAN_UARTFR_TXFF) {
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            return ERR_USART(DRV_ERROR_TIMEOUT);
        }
    }

    /* Send the character */
    addr->UARTDR = ch;

    timecount = 0;
    while (!(addr->UARTFR & SILAN_UARTFR_TXFE)) {
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            return ERR_USART(DRV_ERROR_TIMEOUT);
        }
    }

    return 0;
}

/**
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t csi_usart_get_tx_count(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;

    if (usart_priv->tx_busy) {
        return usart_priv->tx_cnt;
    } else {
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
    USART_NULL_PARAM_CHK(handle);

    silan_usart_priv_t *usart_priv = handle;

    if (usart_priv->rx_busy) {
        return usart_priv->rx_cnt;
    } else {
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
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
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
    USART_NULL_PARAM_CHK(handle);
    silan_usart_priv_t *usart_priv = handle;
    silan_usart_reg_t *addr = (silan_usart_reg_t *)(usart_priv->base);

    switch (flowctrl_type) {
        case USART_FLOWCTRL_CTS:
            target_usart_tx_flowctrl_init(usart_priv->idx, 1);
            WAIT_USART_IDLE(addr);
            addr->UARTCR |= SILAN_UART_CR_CTSEN;
            break;

        case USART_FLOWCTRL_RTS:
            target_usart_rx_flowctrl_init(usart_priv->idx, 1);
            WAIT_USART_IDLE(addr);
            addr->UARTCR |= SILAN_UART_CR_RTSEN;
            break;

        case USART_FLOWCTRL_CTS_RTS:
            target_usart_tx_flowctrl_init(usart_priv->idx, 1);
            target_usart_rx_flowctrl_init(usart_priv->idx, 1);
            WAIT_USART_IDLE(addr);
            addr->UARTCR |= SILAN_UART_CR_CTSEN;
            addr->UARTCR |= SILAN_UART_CR_RTSEN;
            break;

        case USART_FLOWCTRL_NONE:
            target_usart_tx_flowctrl_init(usart_priv->idx, 0);
            target_usart_rx_flowctrl_init(usart_priv->idx, 0);
            WAIT_USART_IDLE(addr);
            addr->UARTCR &= ~SILAN_UART_CR_CTSEN;
            addr->UARTCR &= ~SILAN_UART_CR_RTSEN;
            break;

        default:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);
    }

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
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

