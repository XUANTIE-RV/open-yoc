/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include <drv_irq.h>
#include <drv_usart.h>
#include <drv_pmu.h>
#include <dw_usart.h>
#include <soc.h>
#include <csi_core.h>
#include <drv_dmac.h>
#if defined  CONFIG_DW_DMAC
#include <dw_dmac.h>
#endif
#if defined  CONFIG_CK_DMAC
#include <drv_etb.h>
#include <ck_dmac_v2.h>
#include <ck_etb.h>
#endif
#ifdef CONFIG_CHIP_CH2201
#include <pinmux.h>
#include <pin_name.h>
#include <drv_gpio.h>
#endif

#define ERR_USART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

/*
 * setting config may be accessed when the USART is not
 * busy(USR[0]=0) and the DLAB bit(LCR[7]) is set.
 */

#define WAIT_USART_IDLE(addr)\
    do {                       \
        int32_t timecount = 0;  \
        while ((addr->USR & USR_UART_BUSY) && (timecount < UART_BUSY_TIMEOUT)) {\
            timecount++;\
        }\
        if (timecount >= UART_BUSY_TIMEOUT) {\
            return ERR_USART(DRV_ERROR_TIMEOUT);\
        }                                   \
    } while(0)

#define WAIT_USART_IDLE_VOID(addr)\
    do {                       \
        int32_t timecount = 0;  \
        while ((addr->USR & USR_UART_BUSY) && (timecount < UART_BUSY_TIMEOUT)) {\
            timecount++;\
        }\
        if (timecount >= UART_BUSY_TIMEOUT) {\
            return;\
        }                                   \
    } while(0)

#define USART_NULL_PARAM_CHK(para)    HANDLE_PARAM_CHK(para, ERR_USART(DRV_ERROR_PARAMETER))

typedef struct {
#ifdef CONFIG_LPM
    uint8_t usart_power_status;
    uint32_t usart_regs_saved[5];
#endif
    uint32_t base;
    uint32_t irq;
    usart_event_cb_t cb_event;           ///< Event callback
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
#if defined  CONFIG_DW_DMAC || defined CONFIG_CK_DMAC
    int32_t dma_tx_ch;
    int32_t dma_rx_ch;
    uint8_t dma_tx_mode;     // 0-not sending  1-sending
    uint8_t dma_rx_mode;     // 0-not receiving  1-receiving
#endif
#if defined  CONFIG_CK_DMAC
    int32_t etb_tx_ch;
    int32_t etb_rx_ch;
#endif
} dw_usart_priv_t;

extern int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
extern int32_t target_get_addr_space(uint32_t addr);

static dw_usart_priv_t usart_instance[CONFIG_USART_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};
#ifdef CONFIG_CHIP_CH2201
extern int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode);
static void pin_uart_to_gpio(usart_handle_t handle)
{
    dw_usart_priv_t *usart_priv = handle;
    switch (usart_priv->idx)
    {
        case 0:
            //console
            drv_pinmux_config(PA10, 4);
            drv_pinmux_config(PA11, 4);
            break;
        case 1:
            //uart1
            drv_pinmux_config(PA17, 4);
            drv_pinmux_config(PA16, 4);
            break;
        case 2:
            //uart2
            drv_pinmux_config(PA24, 4);
            drv_pinmux_config(PA25, 4);
            break;
        default:
            return;

    }
}


static void pin_gpio_to_uart(usart_handle_t handle)
{
    dw_usart_priv_t *usart_priv = handle;
    switch (usart_priv->idx)
    {
        case 0:
            //console
            drv_pinmux_config(PA10, PA10_UART0_TX);
            drv_pinmux_config(PA11, PA11_UART0_RX);
            drv_pin_config_mode(PORTA, PA10,1);
            drv_pin_config_mode(PORTA, PA11,1);
            break;
        case 1:
            //uart1
            drv_pinmux_config(PA17, PA17_UART1_TX);
            drv_pinmux_config(PA16, PA16_UART1_RX);
            drv_pin_config_mode(PORTA, PA17,1);
            drv_pin_config_mode(PORTA, PA16,1);
            break;
        case 2:
            //uart2
            drv_pinmux_config(PA24, PA24_UART2_TX);
            drv_pinmux_config(PA25, PA25_UART2_RX);
            drv_pin_config_mode(PORTA, PA24,1);
            drv_pin_config_mode(PORTA, PA25,1);
            break;
        default:
            return;

    }

}
#endif

#if defined  CONFIG_DW_DMAC
static void dw_usart_dma_event_cb(int32_t ch, dma_event_e event)
{
    dw_usart_priv_t *usart_priv = NULL;
    int8_t priv_num  = 0;

    for (priv_num = 0; priv_num < CONFIG_USART_NUM; priv_num++) {
        usart_priv = &usart_instance[priv_num];

        if ((usart_priv->dma_tx_ch == ch && usart_priv->dma_tx_mode == 1) || (usart_priv->dma_rx_ch == ch && usart_priv->dma_rx_mode == 1)) {
            break;
        }
    }

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (event == DMA_EVENT_TRANSFER_ERROR) {           /* DMA transfer ERROR */
        if (usart_priv->dma_tx_ch == ch && usart_priv->dma_tx_mode == 1) {
            csi_dma_stop(usart_priv->dma_tx_ch);
            csi_dma_release_channel(usart_priv->dma_tx_ch);
            usart_priv->tx_busy = 0;
            usart_priv->dma_tx_mode = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->fcr_reg &= ~DW_FCR_TET_FIFO_HALF;
            addr->FCR = usart_priv->fcr_reg;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_TX_UNDERFLOW);
            }
        } else {
            csi_dma_stop(usart_priv->dma_rx_ch);
            csi_dma_release_channel(usart_priv->dma_rx_ch);
            usart_priv->rx_busy = 0;
            usart_priv->dma_rx_mode = 0;
            usart_priv->dma_rx_ch = -1;

            /* FIFO enable */
            usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);
            addr->FCR |= DW_FCR_FIFOE | DW_FCR_RT_FIFO_HALF;
            usart_priv->fcr_reg |= DW_FCR_RT_FIFO_HALF;
            /* enable received data available */
            addr->IER |= IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_RX_FRAMING_ERROR);
            }
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) {  /* DMA transfer complete */
        if (usart_priv->dma_tx_ch == ch && usart_priv->dma_tx_mode == 1) {
            csi_dma_stop(usart_priv->dma_tx_ch);
            csi_dma_release_channel(usart_priv->dma_tx_ch);
            usart_priv->tx_busy = 0;
            usart_priv->dma_tx_mode = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->fcr_reg &= ~(DW_FCR_TET_FIFO_HALF);
            addr->FCR = usart_priv->fcr_reg;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_SEND_COMPLETE);
            }
        } else {
            csi_dma_stop(usart_priv->dma_rx_ch);
            csi_dma_release_channel(usart_priv->dma_rx_ch);
            usart_priv->rx_busy = 0;
            usart_priv->dma_rx_mode = 0;
            usart_priv->dma_rx_ch = -1;

            /* FIFO enable */
            usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);
            usart_priv->fcr_reg |= DW_FCR_RT_FIFO_HALF;
            addr->FCR =  usart_priv->fcr_reg;
            /* enable received data available */
            addr->IER |= IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_RECEIVE_COMPLETE);
            }
        }
    }
}
#endif

#if defined  CONFIG_CK_DMAC
static void dw_usart_dma_event_cb(int32_t ch, dma_event_e event)
{
    dw_usart_priv_t *usart_priv = NULL;
    int8_t priv_num  = 0;

    for (priv_num = 0; priv_num < CONFIG_USART_NUM; priv_num++) {
        usart_priv = &usart_instance[priv_num];

        if ((usart_priv->dma_tx_ch == ch) || (usart_priv->dma_rx_ch == ch)) {
            break;
        }
    }

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (event == DMA_EVENT_TRANSFER_ERROR) {           /* DMA transfer ERROR */
        if (usart_priv->dma_tx_ch == ch) {
            csi_dma_stop(usart_priv->dma_tx_ch);
            drv_etb_stop(usart_priv->etb_tx_ch);
            csi_dma_release_channel(usart_priv->dma_tx_ch);
            usart_priv->tx_busy = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->etb_tx_ch = -1;
            addr->HTX &= ~(DW_USART_HTX_TX_ETB_FUNC_Msk);
            usart_priv->fcr_reg &= ~DW_FCR_TET_FIFO_HALF;
            addr->FCR = usart_priv->fcr_reg;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_TX_UNDERFLOW);
            }
        } else {
            csi_dma_stop(usart_priv->dma_rx_ch);
            drv_etb_stop(usart_priv->etb_rx_ch);
            csi_dma_release_channel(usart_priv->dma_rx_ch);
            usart_priv->rx_busy = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->etb_tx_ch = -1;
            addr->HTX &= ~(DW_USART_HTX_RX_ETB_FUNC_Msk);
            /* FIFO enable */
            usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);
            addr->FCR |= DW_FCR_FIFOE | DW_FCR_RT_FIFO_HALF;
            usart_priv->fcr_reg |= DW_FCR_RT_FIFO_HALF;
            /* enable received data available */
            addr->IER |= IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_RX_FRAMING_ERROR);
            }
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) {  /* DMA transfer complete */
        if (usart_priv->dma_tx_ch == ch) {
            addr->HTX &= ~(DW_USART_HTX_TX_ETB_FUNC_Msk);
            csi_dma_stop(usart_priv->dma_tx_ch);
            drv_etb_stop(usart_priv->etb_tx_ch);
            csi_dma_release_channel(usart_priv->dma_tx_ch);
            usart_priv->tx_busy = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->etb_tx_ch = -1;
            usart_priv->fcr_reg &= ~(DW_FCR_TET_FIFO_HALF);
            addr->FCR = usart_priv->fcr_reg;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_SEND_COMPLETE);
            }
        } else {
            addr->HTX &= ~(DW_USART_HTX_RX_ETB_FUNC_Msk);
            csi_dma_stop(usart_priv->dma_rx_ch);
            drv_etb_stop(usart_priv->etb_rx_ch);
            csi_dma_release_channel(usart_priv->dma_rx_ch);
            usart_priv->rx_busy = 0;
            usart_priv->dma_tx_ch = -1;
            usart_priv->etb_tx_ch = -1;
            /* FIFO enable */
            usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);
            usart_priv->fcr_reg |= DW_FCR_RT_FIFO_HALF;
            addr->FCR =  usart_priv->fcr_reg;
            /* enable received data available */
            addr->IER |= IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

            if (usart_priv->cb_event) {
                usart_priv->cb_event(priv_num, USART_EVENT_RECEIVE_COMPLETE);
            }
        }
    }
}
#endif

static int32_t usart_wait_timeout(usart_handle_t handle, dw_usart_reg_t *addr)
{
#ifdef CONFIG_CHIP_CH2201
    pin_uart_to_gpio(handle);
#endif
    uint32_t timecount = 0;
    while (addr->LSR & 0x1 || addr->USR & 0x1) {
        addr->RBR;
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            return -1;
        }
    }
    WAIT_USART_IDLE(addr);

    return 0;
}

/**
  \brief       set the baut drate of usart.
  \param[in]   addr  usart base to operate.
  \return      error code
*/
int32_t csi_usart_config_baudrate(usart_handle_t handle, uint32_t baud)
{
    USART_NULL_PARAM_CHK(handle);
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_wait_timeout(handle, addr) != 0)
        return ERR_USART(USART_ERROR_BAUDRATE);

    /* baudrate=(seriak clock freq)/(16*divisor); algorithm :rounding*/
    uint32_t divisor = ((drv_get_usart_freq(usart_priv->idx)  * 10) / baud) >> 4;

    if ((divisor % 10) >= 5) {
        divisor = (divisor / 10) + 1;
    } else {
        divisor = divisor / 10;
    }

    uint32_t flag = csi_irq_save();
    addr->LCR |= LCR_SET_DLAB;
    /* DLL and DLH is lower 8-bits and higher 8-bits of divisor.*/
    addr->DLL = divisor & 0xff;
    addr->DLH = (divisor >> 8) & 0xff;
    /*
     * The DLAB must be cleared after the baudrate is setted
     * to access other registers.
     */
    addr->LCR &= (~LCR_SET_DLAB);
    csi_irq_restore(flag);

#ifdef CONFIG_CHIP_CH2201
    pin_gpio_to_uart(handle);
#endif

    return 0;
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
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_wait_timeout(handle, addr) != 0)
        return ERR_USART(USART_ERROR_PARITY);

    switch (parity) {
        case USART_PARITY_NONE:
            /*CLear the PEN bit(LCR[3]) to disable parity.*/
            addr->LCR &= (~LCR_PARITY_ENABLE);
            break;

        case USART_PARITY_ODD:
            /* Set PEN and clear EPS(LCR[4]) to set the ODD parity. */
            addr->LCR |= LCR_PARITY_ENABLE;
            addr->LCR &= LCR_PARITY_ODD;
            break;

        case USART_PARITY_EVEN:
            /* Set PEN and EPS(LCR[4]) to set the EVEN parity.*/
            addr->LCR |= LCR_PARITY_ENABLE;
            addr->LCR |= LCR_PARITY_EVEN;
            break;

        default:
#ifdef CONFIG_CHIP_CH2201
            pin_gpio_to_uart(handle);
#endif
            return ERR_USART(USART_ERROR_PARITY);
    }
#ifdef CONFIG_CHIP_CH2201
    pin_gpio_to_uart(handle);
#endif

    return 0;
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
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_wait_timeout(handle, addr) != 0)
        return ERR_USART(USART_ERROR_STOP_BITS);

    switch (stopbit) {
        case USART_STOP_BITS_1:
            /* Clear the STOP bit to set 1 stop bit*/
            addr->LCR &= LCR_STOP_BIT1;
            break;

        case USART_STOP_BITS_2:
            /*
            * If the STOP bit is set "1",we'd gotten 1.5 stop
            * bits when DLS(LCR[1:0]) is zero, else 2 stop bits.
            */
            addr->LCR |= LCR_STOP_BIT2;
            break;

        default:
#ifdef CONFIG_CHIP_CH2201
            pin_gpio_to_uart(handle);
#endif

            return ERR_USART(USART_ERROR_STOP_BITS);
    }
#ifdef CONFIG_CHIP_CH2201
    pin_gpio_to_uart(handle);
#endif

    return 0;
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
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_wait_timeout(handle, addr) != 0)
        return ERR_USART(USART_ERROR_DATA_BITS);

    /* The word size decides by the DLS bits(LCR[1:0]), and the
     * corresponding relationship between them is:
     *   DLS   word size
     *       00 -- 5 bits
     *       01 -- 6 bits
     *       10 -- 7 bits
     *       11 -- 8 bits
     */

    switch (databits) {
        case USART_DATA_BITS_5:
            addr->LCR &= LCR_WORD_SIZE_5;
            break;

        case USART_DATA_BITS_6:
            addr->LCR &= 0xfd;
            addr->LCR |= LCR_WORD_SIZE_6;
            break;

        case USART_DATA_BITS_7:
            addr->LCR &= 0xfe;
            addr->LCR |= LCR_WORD_SIZE_7;
            break;

        case USART_DATA_BITS_8:
            addr->LCR |= LCR_WORD_SIZE_8;
            break;

        default:
#ifdef CONFIG_CHIP_CH2201
            pin_gpio_to_uart(handle);
#endif
            return ERR_USART(USART_ERROR_DATA_BITS);
    }

#ifdef CONFIG_CHIP_CH2201
    pin_gpio_to_uart(handle);
#endif

    return 0;
}

/**
  \brief       get character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   the pointer to the received character if return 0.
  \return      error code
*/
int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(ch);
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    while (!(addr->LSR & LSR_DATA_READY));

    *ch = addr->RBR;

    return 0;
}

/**
  \brief       transmit character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   ch  the input character
  \return      error code
*/
int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    USART_NULL_PARAM_CHK(handle);
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    uint32_t timecount = 0;

    while ((!(addr->LSR & DW_LSR_TRANS_EMPTY))) {
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            return ERR_USART(DRV_ERROR_TIMEOUT);
        }
    }

    addr->THR = ch;

    return 0;
}

/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usart_priv usart private to operate.
*/
static void dw_usart_intr_threshold_empty(int32_t idx, dw_usart_priv_t *usart_priv)
{
    if (usart_priv->tx_total_num == 0) {
        return;
    }

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        addr->IER &= (~IER_THRE_INT_ENABLE);
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
        uint32_t txdata_num = (remain_txdata > (UART_MAX_FIFO - 1)) ? (UART_MAX_FIFO - 1) : remain_txdata;
        uint32_t i = 0u;

        for (i = 0; i < txdata_num; i++) {
            addr->THR = *((uint8_t *)usart_priv->tx_buf);
            usart_priv->tx_cnt++;
            usart_priv->tx_buf++;
        }
    }
}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   usart_priv usart private to operate.
*/
static void dw_usart_intr_recv_data(int32_t idx, dw_usart_priv_t *usart_priv)
{
    if ((usart_priv->rx_total_num == 0) || (usart_priv->rx_buf == NULL)) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
        } else {
            uint8_t data;
            csi_usart_receive_query(usart_priv, &data, 1);
        }

        return;
    }

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    uint32_t rxfifo_num = addr->RFL;
    uint32_t rxdata_num = (rxfifo_num > usart_priv->rx_total_num) ? usart_priv->rx_total_num : rxfifo_num;
    uint32_t i;

    for (i = 0; i < rxdata_num; i++) {
        *((uint8_t *)usart_priv->rx_buf) = addr->RBR;;
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
  \brief        interrupt service function for receiver line.
  \param[in]   usart_priv usart private to operate.
*/
static void dw_usart_intr_recv_line(int32_t idx, dw_usart_priv_t *usart_priv)
{
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    uint32_t lsr_stat = addr->LSR;

    uint32_t timecount = 0;

    while (addr->LSR & 0x1) {
        addr->RBR;
        timecount++;

        if (timecount >= UART_BUSY_TIMEOUT) {
            if (usart_priv->cb_event) {
                usart_priv->cb_event(idx, USART_EVENT_RX_TIMEOUT);
            }

            return;
        }
    }

    /** Break Interrupt bit. This is used to indicate the detection of a
      * break sequence on the serial input data.
      */
    if (lsr_stat & DW_LSR_BI) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_BREAK);
        }

        return;
    }

    /** Framing Error bit. This is used to indicate the occurrence of a
      * framing error in the receiver. A framing error occurs when the receiver
      * does not detect a valid STOP bit in the received data.
      */
    if (lsr_stat & DW_LSR_FE) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_FRAMING_ERROR);
        }

        return;

    }

    /** Framing Error bit. This is used to indicate the occurrence of a
      * framing error in the receiver. A framing error occurs when the
      * receiver does not detect a valid STOP bit in the received data.
      */
    if (lsr_stat & DW_LSR_PE) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_PARITY_ERROR);
        }

        return;

    }

    /** Overrun error bit. This is used to indicate the occurrence of an overrun error.
      * This occurs if a new data character was received before the previous data was read.
      */
    if (lsr_stat & DW_LSR_OE) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_OVERFLOW);
        }

        return;

    }

}
/**
  \brief        interrupt service function for character timeout.
  \param[in]   usart_priv usart private to operate.
*/
static void dw_usart_intr_char_timeout(int32_t idx, dw_usart_priv_t *usart_priv)
{
    if ((usart_priv->rx_total_num != 0) && (usart_priv->rx_buf != NULL)) {
        dw_usart_intr_recv_data(idx, usart_priv);
        return;
    }

    if (usart_priv->cb_event) {
        usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
    } else {
        dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

        uint32_t timecount = 0;

        while (addr->LSR & 0x1) {
            addr->RBR;
            timecount++;

            if (timecount >= UART_BUSY_TIMEOUT) {
                if (usart_priv->cb_event) {
                    usart_priv->cb_event(idx, USART_EVENT_RX_TIMEOUT);
                }

                return;
            }
        }
    }
}

/**
  \brief       the interrupt service function.
  \param[in]   index of usart instance.
*/
void dw_usart_irqhandler(int32_t idx)
{
    dw_usart_priv_t *usart_priv = &usart_instance[idx];
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    uint8_t intr_state = addr->IIR & 0xf;

    switch (intr_state) {
        case DW_IIR_THR_EMPTY:       /* interrupt source:transmitter holding register empty */
            dw_usart_intr_threshold_empty(idx, usart_priv);
            break;

        case DW_IIR_RECV_DATA:       /* interrupt source:receiver data available or receiver fifo trigger level reached */
            dw_usart_intr_recv_data(idx, usart_priv);
            break;

        case DW_IIR_RECV_LINE:
            dw_usart_intr_recv_line(idx, usart_priv);
            break;

        case DW_IIR_CHAR_TIMEOUT:
            dw_usart_intr_char_timeout(idx, usart_priv);
            break;

        default:
            break;
    }
}

#ifdef CONFIG_LPM
static void manage_clock(usart_handle_t handle, uint8_t enable)
{
    dw_usart_priv_t *usart_priv = handle;
    uint8_t device[] = {CLOCK_MANAGER_UART0, CLOCK_MANAGER_UART1, CLOCK_MANAGER_UART2};
    drv_clock_manager_config(device[usart_priv->idx], enable);
}

static void do_prepare_sleep_action(usart_handle_t handle)
{
    dw_usart_priv_t *usart_priv = handle;
    volatile dw_usart_reg_t *ubase = (dw_usart_reg_t *)(usart_priv->base);
    uint8_t data[16];
    csi_usart_receive_query(handle, data, 16);
    WAIT_USART_IDLE_VOID(ubase);
    ubase->LCR |= LCR_SET_DLAB;
    registers_save(usart_priv->usart_regs_saved, (uint32_t *)ubase, 2);
    ubase->LCR &= ~LCR_SET_DLAB;
    registers_save(&usart_priv->usart_regs_saved[2], (uint32_t *)ubase + 1, 1);
    registers_save(&usart_priv->usart_regs_saved[3], (uint32_t *)ubase + 3, 2);
}

static void do_wakeup_sleep_action(usart_handle_t handle)
{
    dw_usart_priv_t *usart_priv = handle;
    volatile dw_usart_reg_t *ubase = (dw_usart_reg_t *)(usart_priv->base);
    WAIT_USART_IDLE_VOID(ubase);
    ubase->LCR |= LCR_SET_DLAB;
    registers_restore((uint32_t *)ubase, usart_priv->usart_regs_saved, 2);
    ubase->LCR &= ~LCR_SET_DLAB;
    registers_restore((uint32_t *)ubase + 1, &usart_priv->usart_regs_saved[2], 1);
    registers_restore((uint32_t *)ubase + 3, &usart_priv->usart_regs_saved[3], 2);
}
#endif

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
#if defined CONFIG_DW_DMAC || defined CONFIG_CK_DMAC
    usart_priv->dma_tx_ch = -1;
    usart_priv->dma_rx_ch = -1;
    usart_priv->dma_tx_mode = 0;
    usart_priv->dma_rx_mode = 0;
#endif
#if defined CONFIG_CK_DMAC
    usart_priv->etb_tx_ch = -1;
    usart_priv->etb_rx_ch = -1;
#endif

#ifdef CONFIG_LPM
    csi_usart_power_control(usart_priv, DRV_POWER_FULL);
#endif

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    /* FIFO enable */
    addr->FCR = DW_FCR_FIFOE | DW_FCR_RT_FIFO_HALF;
    usart_priv->fcr_reg = DW_FCR_FIFOE | DW_FCR_RT_FIFO_HALF;
    /* enable received data available */
    addr->IER = IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE;

#ifndef CONFIG_DISABLE_IRQ
    drv_irq_register(usart_priv->irq, handler);
    drv_irq_enable(usart_priv->irq);
#endif

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

    dw_usart_priv_t *usart_priv = handle;
    drv_irq_disable(usart_priv->irq);
    drv_irq_unregister(usart_priv->irq);
    usart_priv->cb_event   = NULL;

#if defined  CONFIG_DW_DMAC

    if (usart_priv->dma_tx_ch != -1) {
        csi_dma_stop(usart_priv->dma_tx_ch);
        csi_dma_release_channel(usart_priv->dma_tx_ch);
        usart_priv->dma_tx_ch = -1;
    }

    if (usart_priv->dma_rx_ch != -1) {
        csi_dma_stop(usart_priv->dma_rx_ch);
        csi_dma_release_channel(usart_priv->dma_rx_ch);
        usart_priv->dma_rx_ch = -1;
    }

#endif

#ifdef CONFIG_LPM
    csi_usart_power_control(usart_priv, DRV_POWER_OFF);
#endif

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
    int32_t ret;

    /* control the data_bit of the usart*/
    ret = csi_usart_config_baudrate(handle, baud);

    if (ret < 0) {
        return ret;
    }

    /* control mode of the usart*/
    ret = csi_usart_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    /* control the parity of the usart*/
    ret = csi_usart_config_parity(handle, parity);

    if (ret < 0) {
        return ret;
    }

    /* control the stopbit of the usart*/
    ret = csi_usart_config_stopbits(handle, stopbits);

    if (ret < 0) {
        return ret;
    }

    ret = csi_usart_config_databits(handle, bits);

    if (ret < 0) {
        return ret;
    }

    return 0;
}


#if defined  CONFIG_DW_DMAC
static int32_t dw_usart_send_dma(dw_usart_priv_t *usart_priv)
{
    dma_config_t config;

    if (target_get_addr_space((uint32_t)usart_priv->tx_buf) != ADDR_SPACE_SRAM) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    config.src_inc  = DMA_ADDR_INC;
    config.dst_inc  = DMA_ADDR_CONSTANT;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    if (usart_priv->idx == 0) {
        config.hs_if    = CKENUM_DMA_UART0_TX;
    } else if (usart_priv->idx == 1) {
        config.hs_if    = CKENUM_DMA_UART1_TX;
    } else {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    config.type     = DMA_MEM2PERH;

    if (usart_priv->dma_tx_ch  == -1) {
        usart_priv->dma_tx_ch  = csi_dma_alloc_channel();

        if (usart_priv->dma_tx_ch == -1) {
            return ERR_USART(DRV_ERROR_PARAMETER);
        }
    }

    int32_t ret = csi_dma_config_channel(usart_priv->dma_tx_ch, &config, dw_usart_dma_event_cb);

    if (ret < 0) {
        csi_dma_release_channel(usart_priv->dma_tx_ch);
        return ret;
    }

    usart_priv->dma_tx_mode = 1;

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    addr->FCR = usart_priv->fcr_reg | (DW_FCR_TET_FIFO_HALF);
    usart_priv->fcr_reg |= (DW_FCR_TET_FIFO_HALF);

    csi_dma_start(usart_priv->dma_tx_ch, usart_priv->tx_buf, (uint8_t *) & (addr->THR), usart_priv->tx_total_num);


    return 0;
}
#endif

#if defined CONFIG_CK_DMAC
static uint32_t find_max_prime_num(uint32_t num)
{
    if (!(num % 8)) {
        return 8;
    }

    if (!(num % 4)) {
        return 4;
    }

    return 1;
}

static int32_t dw_usart_send_dma(dw_usart_priv_t *usart_priv)
{
    uint32_t etb_source_ip = 0;
    int32_t ret;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    addr->IER &= ~IER_THRE_INT_ENABLE;
    usart_priv->dma_tx_ch  = csi_dma_alloc_channel();

    if (usart_priv->dma_tx_ch == -1) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));

    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_inc    = DMA_ADDR_INC;
    config.dst_inc    = DMA_ADDR_CONSTANT;
    config.src_tw     = 1;
    config.dst_tw     = 1;

    if (usart_priv->idx == 0) {
        etb_source_ip = ETB_UART0_TX;
    } else if (usart_priv->idx == 1) {
        etb_source_ip = ETB_UART1_TX;
    } else {
        csi_dma_release_channel(usart_priv->dma_tx_ch);
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    config.mode      = DMA_GROUP_TRIGGER;
    config.group_len = find_max_prime_num(usart_priv->tx_total_num);
    config.ch_mode   = DMA_MODE_HARDWARE;

    ret = csi_dma_config_channel(usart_priv->dma_tx_ch, &config, dw_usart_dma_event_cb);

    if (ret < 0) {
        csi_dma_release_channel(usart_priv->dma_tx_ch);
        return ret;
    }

    usart_priv->fcr_reg &= ~(DW_FCR_TET_FIFO_HALF);

    if (config.group_len >= (UART_MAX_FIFO / 2)) {
        usart_priv->fcr_reg |= DW_FCR_TET_FIFO_HALF;
    } else if (config.group_len >= (UART_MAX_FIFO / 4)) {
        usart_priv->fcr_reg |= DW_FCR_TET_FIFO_QUARTER;
    } else if (config.group_len >= (UART_MAX_FIFO / 8)) {
        usart_priv->fcr_reg |= DW_FCR_TET_FIFO_TWO;
    } else {
        usart_priv->fcr_reg |= DW_FCR_TET_FIFO_EMPTY;
    }

    addr->FCR = usart_priv->fcr_reg;

    csi_dma_start(usart_priv->dma_tx_ch, usart_priv->tx_buf, (uint8_t *) & (addr->THR), usart_priv->tx_total_num);

    usart_priv->etb_tx_ch = drv_etb_channel_config(etb_source_ip, usart_priv->dma_tx_ch, ETB_HARDWARE, ETB_ONE_TRIGGER_ONE);

    drv_etb_start(usart_priv->etb_tx_ch);

    addr->HTX |= DW_USART_HTX_TX_ETB_FUNC_Msk;

    return 0;
}
#endif

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

    dw_usart_priv_t *usart_priv = handle;

    usart_priv->tx_buf = (uint8_t *)data;
    usart_priv->tx_total_num = num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 1;
    usart_priv->last_tx_num = 0;
#if defined  CONFIG_DW_DMAC || defined CONFIG_CK_DMAC

    /* using dma when send count greater than UART_MAX_FIFO */
    if (usart_priv->tx_total_num >= UART_MAX_FIFO) {

        if (dw_usart_send_dma(usart_priv) == 0) {
            return 0;
        }
    }

#endif

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    dw_usart_intr_threshold_empty(usart_priv->idx, usart_priv);
    /* enable the interrupt*/
    addr->IER |= IER_THRE_INT_ENABLE;
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
    dw_usart_priv_t *usart_priv = handle;

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    addr->IER &= (~IER_THRE_INT_ENABLE);

    usart_priv->tx_cnt = usart_priv->tx_total_num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 0;
    usart_priv->tx_buf = NULL;
    usart_priv->tx_total_num = 0;
    return 0;
}

#if defined  CONFIG_DW_DMAC
static int32_t dw_usart_receive_dma(dw_usart_priv_t *usart_priv)
{
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    /* disable received data available */

    dma_config_t config;
    config.src_inc  = DMA_ADDR_CONSTANT;
    config.dst_inc  = DMA_ADDR_INC;
    config.src_tw   = 1;
    config.dst_tw   = 1;

    if (usart_priv->idx == 0) {
        config.hs_if    = CKENUM_DMA_UART0_RX;
    } else if (usart_priv->idx == 1) {
        config.hs_if    = CKENUM_DMA_UART1_RX;
    } else {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    addr->IER &= ~(IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE);
    config.type     = DMA_PERH2MEM;

    if (usart_priv->dma_rx_ch == -1) {
        usart_priv->dma_rx_ch = csi_dma_alloc_channel();

        if (usart_priv->dma_rx_ch == -1) {
            return ERR_USART(DRV_ERROR_PARAMETER);
        }
    }

    int32_t ret = csi_dma_config_channel(usart_priv->dma_rx_ch, &config, dw_usart_dma_event_cb);

    if (ret < 0) {
        csi_dma_release_channel(usart_priv->dma_rx_ch);
        return ret;
    }

    uint8_t i;

    for (i = 2; i > 0; i--) {
        if (!(usart_priv->rx_total_num % (2 << i))) {
            break;
        }
    }

    usart_priv->dma_rx_mode = 1;

    usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);
    addr->FCR = 1 << DW_USART_FCR_RFIFOR_Pos | (i << DW_USART_FCR_RT_Pos) | usart_priv->fcr_reg;
    usart_priv->fcr_reg |= (i << DW_USART_FCR_RT_Pos);
    csi_dma_start(usart_priv->dma_rx_ch, (uint8_t *) & (addr->THR), usart_priv->rx_buf,  usart_priv->rx_total_num);
    return 0;
}
#endif

#if defined CONFIG_CK_DMAC
static int32_t dw_usart_receive_dma(dw_usart_priv_t *usart_priv)
{
    uint32_t etb_dest_ip = 0;
    int32_t ret;

    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    /* disable received data available */
    addr->IER &= ~(IER_RDA_INT_ENABLE | IIR_RECV_LINE_ENABLE);

    usart_priv->dma_rx_ch = csi_dma_alloc_channel();

    if (usart_priv->dma_rx_ch == -1) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));

    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_inc    = DMA_ADDR_CONSTANT;
    config.dst_inc    = DMA_ADDR_INC;
    config.src_tw     = 1;
    config.dst_tw     = 1;

    if (usart_priv->idx == 0) {
        etb_dest_ip = ETB_UART0_RX;
    } else if (usart_priv->idx == 1) {
        etb_dest_ip = ETB_UART1_RX;
    } else {
        csi_dma_release_channel(usart_priv->dma_rx_ch);
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    config.mode = DMA_GROUP_TRIGGER;
    config.group_len = find_max_prime_num(usart_priv->rx_total_num);
    config.ch_mode   = DMA_MODE_HARDWARE;

    ret = csi_dma_config_channel(usart_priv->dma_rx_ch, &config, dw_usart_dma_event_cb);

    if (ret < 0) {
        csi_dma_release_channel(usart_priv->dma_rx_ch);
        return ret;
    }

    usart_priv->fcr_reg &= ~(DW_FCR_RT_FIFO_LESSTWO);

    if (config.group_len >= (UART_MAX_FIFO / 2)) {
        usart_priv->fcr_reg |= DW_FCR_RT_FIFO_HALF;
    } else if (config.group_len >= (UART_MAX_FIFO / 4)) {
        usart_priv->fcr_reg |= DW_FCR_RT_FIFO_QUARTER;
    } else {
        usart_priv->fcr_reg |= DW_FCR_RT_FIFO_SINGLE;
    }

    addr->FCR = 1 << DW_USART_FCR_RFIFOR_Pos | usart_priv->fcr_reg;

    csi_dma_start(usart_priv->dma_rx_ch, (uint8_t *) & (addr->RBR), usart_priv->rx_buf,  usart_priv->rx_total_num);

    usart_priv->etb_rx_ch = drv_etb_channel_config(etb_dest_ip, usart_priv->dma_rx_ch, ETB_HARDWARE, ETB_ONE_TRIGGER_ONE);

    drv_etb_start(usart_priv->etb_rx_ch);

    addr->HTX |= DW_USART_HTX_RX_ETB_FUNC_Msk;
    return 0;
}
#endif

/**
  \brief       Start receiving data from UART receiver.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    dw_usart_priv_t *usart_priv = handle;

    usart_priv->rx_buf = (uint8_t *)data;   // Save receive buffer usart
    usart_priv->rx_total_num = num;         // Save number of data to be received
    usart_priv->rx_cnt = 0;
    usart_priv->rx_busy = 1;
    usart_priv->last_rx_num = 0;
#if defined  CONFIG_DW_DMAC || defined  CONFIG_CK_DMAC

    /* using dma when receive count greater than UART_MAX_FIFO */
    if (usart_priv->rx_total_num > UART_MAX_FIFO) {
        if (dw_usart_receive_dma(usart_priv) == 0) {
            return 0;
        }
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
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    int32_t recv_num = 0;
    uint8_t *dest = (uint8_t *)data;

    while (addr->LSR & 0x1) {
        *dest++ = addr->RBR;
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
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);
    uint32_t line_status_reg    = addr->LSR;

    usart_status.tx_busy = usart_priv->tx_busy;
    usart_status.rx_busy = usart_priv->rx_busy;

    if (line_status_reg & DW_LSR_BI) {
        usart_status.rx_break = 1;
    }

    if (line_status_reg & DW_LSR_FE) {
        usart_status.rx_framing_error = 1;
    }

    if (line_status_reg & DW_LSR_PE) {
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
    USART_NULL_PARAM_CHK(handle);
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

    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    uint32_t timecount = 0;

    if (type == USART_FLUSH_WRITE) {
        usart_priv->fcr_reg  |= DW_FCR_XFIFOR;
        addr->FCR = usart_priv->fcr_reg;
        usart_priv->fcr_reg  &= ~DW_FCR_XFIFOR;

        while (addr->FCR & DW_FCR_XFIFOR) {
            timecount++;

            if (timecount >= UART_BUSY_TIMEOUT) {
                return ERR_USART(DRV_ERROR_TIMEOUT);
            }
        }
    } else if (type == USART_FLUSH_READ) {
        usart_priv->fcr_reg |= DW_FCR_RFIFOR;
        addr->FCR = usart_priv->fcr_reg;
        usart_priv->fcr_reg  &= DW_FCR_RFIFOR;

        while (addr->FCR & DW_FCR_RFIFOR) {
            timecount++;

            if (timecount >= UART_BUSY_TIMEOUT) {
                return ERR_USART(DRV_ERROR_TIMEOUT);
            }
        }
    } else {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

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
    USART_NULL_PARAM_CHK(handle);

    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    switch (type) {
        case USART_INTR_WRITE:
            if (flag == 0) {
                addr->IER &= ~IER_THRE_INT_ENABLE;
            } else if (flag == 1) {
                addr->IER |= IER_THRE_INT_ENABLE;
            } else {
                return ERR_USART(DRV_ERROR_PARAMETER);
            }

            break;

        case USART_INTR_READ:
            if (flag == 0) {
                addr->IER &= ~IER_RDA_INT_ENABLE;
            } else if (flag == 1) {
                addr->IER |= IER_RDA_INT_ENABLE;
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
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t csi_usart_get_tx_count(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    dw_usart_priv_t *usart_priv = handle;

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

    dw_usart_priv_t *usart_priv = handle;

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
#ifdef CONFIG_LPM
    power_cb_t callback;
    callback.wakeup = do_wakeup_sleep_action;
    callback.sleep = do_prepare_sleep_action;
    callback.manage_clock = manage_clock;
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
#endif
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
    dw_usart_priv_t *usart_priv = handle;
    dw_usart_reg_t *addr = (dw_usart_reg_t *)(usart_priv->base);

    switch (flowctrl_type) {
        case USART_FLOWCTRL_CTS:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        case USART_FLOWCTRL_RTS:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        case USART_FLOWCTRL_CTS_RTS:
            WAIT_USART_IDLE(addr);
            addr->MCR |= DW_MCR_AFCE | DW_MCR_RTS;
            break;

        case USART_FLOWCTRL_NONE:
            WAIT_USART_IDLE(addr);
            addr->MCR = 0;
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
