/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_iic.c
 * @brief    CSI Source File for USI Driver
 * @version  V1.0
 * @date     19. Feb 2020
 ******************************************************************************/
#include <drv/irq.h>
#include <drv/iic.h>
#include <wj_usi_ll.h>
#include <drv/usi_iic.h>
#include <drv/common.h>
#include <drv/tick.h>
#include "wj_usi_com.h"

#define RXFIFO_IRQ_TH  (2U)
#define IIC_MAX_FIFO   (8U)
#define VOID_P_DEC(p, val) do{ uint8_t *temp = (uint8_t *)p; temp -= val; p = (void *)temp; }while(0);

extern void wj_usi_irq_handler(void *arg);

/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usi usart private to operate.
*/
static void ck_usi_i2c_intr_transmit(csi_iic_t *usi)
{
    CSI_PARAM_CHK_NORETVAL(usi);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);
    volatile uint32_t  time_buff = 0U;
    uint32_t txdata_num;
    csi_iic_event_t event = IIC_EVENT_ERROR;
    txdata_num = (usi->size > (USI_TX_MAX_FIFO - 1U)) ? (USI_TX_MAX_FIFO - 1U) : usi->size;

    for (uint16_t i = 0U; i < txdata_num; i++) {
        if (usi->size > 0) {
            addr->USI_TX_RX_FIFO = *((uint8_t *)usi->data);
            usi->data++;
            usi->size--;
        } else {
            break;
        }
    }

    if (usi->size == 0U) {
        event = IIC_EVENT_SEND_COMPLETE;

        while ((0 == wj_usi_get_fifo_sta_tx_empty(addr)) || wj_usi_get_i2c_sta_i2cm_work(addr) || wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
            if (time_buff >= 6250000U) {            ///< 10s timeout
                event = IIC_EVENT_ERROR;
                break;
            }

            time_buff ++;
        }

        if (usi->callback) {
            wj_usi_dis_intr_tx_empty(addr);
            wj_usi_dis_intr_tx_thold(addr);
            wj_usi_en_tx_empty_mask(addr);
            usi->callback(usi, event, usi->arg);
        }
    }
}

/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usi usart private to operate.
*/
static void ck_usi_i2c_intr_receive(csi_iic_t *usi)
{
    CSI_PARAM_CHK_NORETVAL(usi);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);
    volatile uint32_t  time_buff = 0U;
    csi_iic_event_t event = IIC_EVENT_ERROR;
    uint32_t count, cmd_flag_read, iic_rx_fifo;

    iic_rx_fifo = wj_usi_get_fifo_sta_rx_num(addr);

    for (count = 0U; count < iic_rx_fifo; count++) {
        *usi->data = (uint8_t)wj_usi_get_tx_rx_fifo(addr);
        usi->size--;
        usi->data++;
    }

    if (usi->size == 0U) {

        event = IIC_EVENT_RECEIVE_COMPLETE;

        while ((0 == wj_usi_get_fifo_sta_rx_empty(addr)) || wj_usi_get_i2c_sta_i2cm_work(addr) || wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
            if (time_buff >= 6250000U) {            ///< 10s timeout
                event = IIC_EVENT_ERROR;
                break;
            }

            time_buff ++;
        }

        if (usi->callback) {
            wj_usi_en_rx_thold_mask(addr);
            wj_usi_dis_intr_rx_thold(addr);
            usi->callback(usi, event, usi->arg);
        }
    } else {
        if (wj_usi_get_i2c_mode(addr) == WJ_USI_I2C_MODE_MASTER) {
            if (usi->size > RXFIFO_IRQ_TH) {
                wj_usi_set_rx_fifo_th(addr, RXFIFO_IRQ_TH);
            } else {
                wj_usi_set_rx_fifo_th(addr, usi->size);
            }

            if ((unsigned long)usi->priv > iic_rx_fifo) {
                cmd_flag_read = iic_rx_fifo;
            } else {
                cmd_flag_read = (unsigned long)usi->priv;
            }

            for (count = 0U; count < cmd_flag_read; count++) {
                wj_usi_set_tx_rx_fifo(addr, 0x100U);
                VOID_P_DEC(usi->priv, 1U);
            }
        } else {
            wj_usi_set_rx_fifo_th(addr, WJ_USI_INTR_CTRL_RX_FIFO_TH_1BYTE);
        }

    }

}
/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usi usart private to operate.
*/
static void ck_usi_i2c_intr_error(csi_iic_t *usi)
{
    CSI_PARAM_CHK_NORETVAL(usi);


    if (usi->callback) {
        usi->callback(usi, IIC_EVENT_ERROR, usi->arg);
    }
}


void ck_usi_i2c_irqhandler(csi_iic_t  *iic)
{
    ck_usi_regs_t *addr = (ck_usi_regs_t *)(iic->dev.reg_base);

    uint32_t intr_state = wj_usi_get_intr_sta(addr);

    if ((intr_state & WJ_USI_INTR_STA_I2CM_LOSE_ARBI_Msk) || (intr_state & WJ_USI_INTR_STA_I2C_NACK_Msk)) {
        ck_usi_i2c_intr_error(iic);
    }

    if (intr_state & WJ_USI_INTR_STA_I2C_STOP_Msk) {

    }

    if (intr_state & WJ_USI_RAW_INTR_STA_TX_EMPTY_Msk) { //intrrupt send  mode
        ck_usi_i2c_intr_transmit(iic);
    }

    if (intr_state & WJ_USI_RAW_INTR_STA_TX_THOLD_Msk) { //intrrupt send  mode
        ck_usi_i2c_intr_transmit(iic);
    }

    if (intr_state & WJ_USI_RAW_INTR_STA_RX_THOLD_Msk) { //intrrupt send  mode
        ck_usi_i2c_intr_receive(iic);

    }

    wj_usi_set_intr_clr(addr, intr_state);
}

/**
  \brief       init iic ctrl block.
               initializes the resources needed for the iic instance.
  \param[in]   iic  handle of iic instance.
  \param[in]   idx  index of instance.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_init(csi_iic_t *iic, uint32_t idx)
{
    ck_usi_regs_t *addr;

    CSI_PARAM_CHK(iic, CSI_ERROR);

    target_get(DEV_WJ_USI_TAG, idx, &iic->dev);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    iic->state.writeable = 1U;
    iic->state.readable  = 1U;
    iic->state.error     = 0U;
    iic->send            = NULL;
    iic->receive         = NULL;
    iic->rx_dma          = NULL;
    iic->tx_dma          = NULL;
    iic->data            = NULL;
    iic->size            = 0U;
    iic->callback        = NULL;
    iic->arg             = NULL;

    wj_usi_set_ctrl(addr, 0U);
    wj_usi_set_mode_sel(addr, WJ_USI_MODE_SEL_I2C);
    wj_usi_set_intr_en(addr, 0U);
    wj_usi_set_intr_unmask(addr, 0U);
    wj_usi_set_ctrl(addr, WJ_USI_CTRL_USI_EN | WJ_USI_CTRL_FM_EN | WJ_USI_CTRL_TX_FIFO_EN | WJ_USI_CTRL_RX_FIFO_EN);
    return CSI_OK;
}

/**
  \brief       uninit iic ctrl block.
               stops operation and releases the software resources used by the instance.
  \param[in]   iic  handle of iic instance.
*/
void csi_usi_iic_uninit(csi_iic_t *iic)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK_NORETVAL(iic);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_dis_ctrl_usi_en(addr);
    iic->state.error     = 0U;
    iic->state.writeable = 0U;
    iic->state.readable  = 0U;
    iic->send            = NULL;
    iic->receive         = NULL;
    iic->rx_dma          = NULL;
    iic->tx_dma          = NULL;
    iic->data            = NULL;
    iic->size            = 0U;
    iic->callback        = NULL;
    iic->arg             = NULL;
}

int32_t wj_usi_iic_config_baudrate(ck_usi_regs_t *addr, uint32_t freq, uint32_t speed)
{
    uint32_t div;
    div = (freq / speed) - 2U;
    wj_usi_set_clk_div0(addr, div / 2U);
    wj_usi_set_clk_div1(addr, div / 2U);
    return 0;
}

/**
  \brief       config iic master or slave mode.
  \param[in]   iic  handle of iic instance.
  \param[in]   mode  iic mode \ref csi_iic_mode_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode)
{
    ck_usi_regs_t *addr;
    csi_error_t ret = CSI_OK;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_en_i2cm_ctrl_stop(addr);
    wj_usi_dis_i2cm_ctrl_addr_sbyte(addr);
    wj_usi_dis_i2cs_ctrl_addr_start(addr);

    if (mode == IIC_MODE_MASTER) {
        wj_usi_set_i2c_mode_master(addr);
        iic->mode = IIC_MODE_MASTER;
    } else if (mode == IIC_MODE_SLAVE) {
        wj_usi_set_i2c_mode_slave(addr);
        iic->mode = IIC_MODE_SLAVE;
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       config iic speed.
  \param[in]   iic  handle of iic instance.
  \param[in]   speed  iic speed mode \ref csi_iic_speed_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed)
{
    ck_usi_regs_t *addr;
    uint32_t freq;
    csi_error_t ret = CSI_OK ;
    CSI_PARAM_CHK(iic, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    freq = soc_get_usi_freq((uint32_t)iic->dev.idx);

    switch (speed) {
        case IIC_BUS_SPEED_STANDARD:
            wj_usi_iic_config_baudrate(addr, freq, 100000U);
            break;

        case IIC_BUS_SPEED_FAST    :
            wj_usi_iic_config_baudrate(addr, freq, 400000U) ;
            break;

        case IIC_BUS_SPEED_FAST_PLUS:
            wj_usi_iic_config_baudrate(addr, freq, 1000000U);
            break;

        case IIC_BUS_SPEED_HIGH    :
            wj_usi_iic_config_baudrate(addr, freq, 3400000U);
            break;

        default:
            ret = CSI_ERROR;
    }

    return ret;

}

/**
  \brief       config iic own addr.
  \param[in]   iic  handle of iic instance.
  \param[in]   own_addr  iic set own addr at slave mode.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_own_addr(csi_iic_t *iic, uint32_t own_addr)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(iic, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    wj_usi_set_i2c_addr(addr, own_addr);
    return CSI_OK;
}

/**
  \brief       config iic addr mode.
  \param[in]   iic  handle of iic instance.
  \param[in]   addr_mode  iic addr mode \ref csi_iic_addr_mode_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode)
{
    ck_usi_regs_t *addr;
    csi_error_t ret = CSI_OK ;
    CSI_PARAM_CHK(iic, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    if (iic->mode == IIC_MODE_MASTER) {
        if (addr_mode == IIC_ADDRESS_10BIT) {
            wj_usi_set_i2cm_ctrl_addr_mode_10bit(addr);
        } else if (addr_mode == IIC_ADDRESS_7BIT) {
            wj_usi_set_i2cm_ctrl_addr_mode_7bit(addr);
        } else {
            ret = CSI_ERROR; /* code */
        }

    } else {
        if (addr_mode == IIC_ADDRESS_7BIT) {
            wj_usi_set_i2cs_ctrl_addr_mode_7bit(addr);
        } else {
            wj_usi_set_i2cs_ctrl_addr_mode_auto(addr);
        }
    }


    return ret;
}

static int32_t drv_usi_common_send(ck_usi_regs_t *addr, const uint8_t *data, uint32_t  num, uint32_t  timeout, uint8_t write_flag)
{
    CSI_PARAM_CHK(addr, CSI_ERROR);
    uint32_t send_num = 0U;
    uint32_t timecount = csi_tick_get_ms();
    uint32_t  timeout_flag = 0U;
    int32_t ret = CSI_OK;

    timecount = (timecount + timeout) <= timeout ? timeout : (timecount + timeout);

    wj_usi_en_ctrl_usi_en(addr);

    if (write_flag == 1) {
        wj_usi_set_tx_rx_fifo(addr, (uint32_t)(*data++ | 0x200));
        send_num++;
    }

    while (send_num < num) {
        while (wj_usi_get_fifo_sta_tx_full(addr) > 0U) {
            if ((csi_tick_get_ms() >= timecount)) {
                timeout_flag = 1U;
                ret = CSI_TIMEOUT;
                break;
            }
        }

        if (timeout_flag == 1U) {
            break;
        } else {
            wj_usi_set_tx_rx_fifo(addr, (uint32_t)(*data++));
            send_num++;
            ret = (int32_t)send_num;
        }
    }

    while (!wj_usi_get_fifo_sta_tx_empty(addr)) {
        if ((csi_tick_get_ms() >= timecount)) {
            timeout_flag = 1U;
            ret = CSI_TIMEOUT;
            break;
        }
    }

    while (wj_usi_get_i2c_sta_i2cm_work(addr) || wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
        if ((csi_tick_get_ms() >= timecount)) {
            timeout_flag = 1U;
            ret = CSI_TIMEOUT;
            break;
        }
    }

    wj_usi_dis_ctrl_usi_en(addr);

    if (ret == CSI_OK) {
        ret = (int32_t)send_num;
    }

    return ret;
}

static int32_t drv_usi_common_receive(ck_usi_regs_t *addr, uint8_t *data, uint32_t  num, uint32_t  timeout)
{
    CSI_PARAM_CHK(addr, CSI_ERROR);

    uint32_t recev_num = 0U;
    uint32_t request_recev_num = 0U;
    uint32_t timecount = csi_tick_get_ms();
    uint32_t  timeout_flag = 0U;
    volatile uint32_t  time_buff = 0;
    int32_t ret = CSI_OK;

    timecount = (timecount + timeout) <= timeout ? timeout : (timecount + timeout);

    wj_usi_en_ctrl_usi_en(addr);
    wj_usi_set_tx_rx_fifo(addr, 0x300);
    request_recev_num ++;

    if (request_recev_num < num) {
        for (int32_t i = 0; i < 4; i++) {
            wj_usi_set_tx_rx_fifo(addr, 0x100U);
            request_recev_num ++;
        }
    }

    while (recev_num < num) {
        while (wj_usi_get_fifo_sta_rx_empty(addr) != 0U) {
            time_buff++;

            if (time_buff > 100000) {
                time_buff = 0;

                if ((csi_tick_get_ms() >= timecount)) {
                    timeout_flag = 1U;
                    ret = CSI_TIMEOUT;
                    break;
                }
            }
        }

        if (timeout_flag == 1U) {
            break;
        } else {
            *data++ = (uint8_t) wj_usi_get_tx_rx_fifo(addr);
            recev_num ++;

            if (request_recev_num < num) {
                wj_usi_set_tx_rx_fifo(addr, 0x100U);
                request_recev_num ++;
            }

            ret = (int32_t)recev_num;
        }
    }

    while (wj_usi_get_i2c_sta_i2cm_work(addr) || wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
        if ((csi_tick_get_ms() >= timecount)) {
            timeout_flag = 1U;
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

/**
  \brief       start sending data as iic master.
               This function is blocking.
  \param[in]   iic  handle of iic instance.
  \param[in]   devaddr  addrress of slave device.
  \param[in]   data  pointer to send data buffer.
  \param[in]   size  size of data items to send.
  \param[in]   timout  unit of time delay(ms).
  \return      the amount of real data sent.
*/
int32_t csi_usi_iic_master_send(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size, uint32_t timeout)
{
    ck_usi_regs_t *addr;
    uint32_t intr_en_status;
    int32_t ret;
    /* check data and uart */
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_set_i2c_addr(addr, devaddr);
    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_en(addr);
    wj_usi_set_intr_en(addr, 0U); //disable all interrupt
    ret = drv_usi_common_send(addr, (const uint8_t *) data, size, timeout, 1);
    wj_usi_set_intr_en(addr, intr_en_status);

    return ret;
}


/**
  \brief       Start receiving data as iic master.
               This function is blocking.
  \param[in]   iic  handle to operate.
  \param[in]   devaddr  iic addrress of slave device.
  \param[out]  data  pointer to buffer for data to receive from iic receiver.
  \param[in]   size  size of data items to receive.
  \param[in]   timeout  unit of time delay(ms).
  \return      the amount of real data received.
*/
int32_t csi_usi_iic_master_receive(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t intr_en_status;
    ck_usi_regs_t *addr;
    int32_t ret ;

    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    wj_usi_set_i2c_addr(addr, devaddr);
    intr_en_status =   wj_usi_get_intr_en(addr);
    wj_usi_set_intr_en(addr, 0U); //disable all interrupt
    ret = drv_usi_common_receive(addr, (uint8_t *)data, size, timeout);
    wj_usi_set_intr_en(addr, intr_en_status);
    return ret;
}

csi_error_t usi_iic_master_send_async(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_set_i2c_addr(addr, devaddr);

    iic->size = size;
    iic->data = (uint8_t *)data;
    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_en_intr_tx_empty(addr);
    iic->size--;
    wj_usi_set_tx_rx_fifo(addr, (uint32_t)(*iic->data++ | 0x200));
    wj_usi_dis_tx_empty_mask(addr);

    return  CSI_OK;
}

/**
  \brief       start sending data as iic master.
               This function is non-blocking,\ref csi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   iic  handle to operate.
  \param[in]   devaddr  iic addrress of slave device.
  \param[in]   data  pointer to send data buffer.
  \param[in]   size  size of data items to send.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_master_send_async(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_iic_master_send_async_t send_func;

    if (iic->send) {
        send_func = iic->send;
        send_func(iic, devaddr, data, size);
    } else {
        csi_irq_enable((uint32_t)iic->dev.irq_num);
        usi_iic_master_send_async(iic, devaddr, data, size);
    }

    return ret;
}

csi_error_t usi_iic_master_receive_async(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size)
{

    ck_usi_regs_t *addr ;

    iic->size = size;
    iic->data = data;
    unsigned long temp = size;
    iic->priv = (void *)temp;       ///< iic->priv used to record the transmission value

    uint32_t count = 0U, cmd_flag_read = 0U;

    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_set_i2c_addr(addr, devaddr);
    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_set_rx_edge_ge(addr);
    wj_usi_dis_rx_thold_mask(addr);

    if (iic->size > RXFIFO_IRQ_TH) {                                    ///< set receive data num and set receive FIFO threshold
        wj_usi_set_rx_fifo_th(addr, RXFIFO_IRQ_TH);
    } else {
        wj_usi_set_rx_fifo_th(addr, iic->size);
    }

    wj_usi_en_intr_rx_thold(addr);

    if (iic->size > IIC_MAX_FIFO) {
        cmd_flag_read = IIC_MAX_FIFO;
    } else {
        cmd_flag_read = iic->size;
    }

    wj_usi_set_tx_rx_fifo(addr, 0x300U);

    for (count = 0U; count < (cmd_flag_read - 1U); count++) {
        wj_usi_set_tx_rx_fifo(addr, 0x100U);
    }

    VOID_P_DEC(iic->priv, cmd_flag_read);
    return  CSI_OK;
}
/**
  \brief       Start receiving data as iic master.
               This function is non-blocking.\ref csi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   iic  handle to operate.
  \param[in]   devaddr  iic addrress of slave device.
  \param[out]  data  pointer to buffer for data to receive from iic receiver.
  \param[in]   size  size of data items to receive.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_master_receive_async(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size)
{

    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_iic_master_receive_async_t receive_func;

    if (iic->receive) {
        receive_func = iic->receive;
        receive_func(iic, devaddr, data, size);
    } else {
        csi_irq_enable((uint32_t)iic->dev.irq_num);
        usi_iic_master_receive_async(iic, devaddr, data, size);
    }

    return ret;
}

/**
  \brief       start sending data as iic master.
               this function is blocking.
  \param[in]   iic  handle of iic instance.
  \param[in]   devaddr  addrress of slave device.
  \param[in]   memaddr  internal addr of device.
  \param[in]   memaddr_size  internal addr mode of device.
  \param[in]   data  pointer to send data buffer.
  \param[in]   size  size of data items to send.
  \param[in]   timout  unit of time delay(ms).
  \return      the amount of real data sent.
*/
int32_t csi_usi_iic_mem_send(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, const void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    int32_t ret = 0;
    int32_t send_count = 0;
    uint8_t *send_data = (void *)data;
    uint8_t mem_buf[4];
    ck_usi_regs_t *addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    wj_usi_set_intr_en(addr, 0U); ///<disable all interrupt
    wj_usi_set_i2c_addr(addr, devaddr);

    if ((send_data == NULL) || (size == 0U)) {
        send_count = (int32_t)CSI_ERROR;
    } else {
        while (send_count != (int32_t)size) {
            if (memaddr_size == IIC_MEM_ADDR_SIZE_8BIT) {
                mem_buf[0] = (uint8_t)memaddr;
                mem_buf[1] = *(send_data++);
                ret = drv_usi_common_send(addr, (const uint8_t *)mem_buf, 2U, timeout, 1);

                if (ret != 2) {
                    break;
                }
            }

            if (memaddr_size == IIC_MEM_ADDR_SIZE_16BIT) {
                mem_buf[0] = (uint8_t)(memaddr >> 8U);
                mem_buf[1] = (uint8_t)memaddr;
                mem_buf[2] = *(send_data++);
                ret = drv_usi_common_send(addr, (const uint8_t *)mem_buf, 3U, timeout, 1);

                if (ret != 3) {
                    break;
                }
            }

            mdelay(5); ///< This delay parameter is due to the hardware characteristics of eeprom
            memaddr ++;
            send_count ++;
        }
    }

    return send_count;
}

/**
  \brief       start receiving data as iic master.
               This function is blocking.
  \param[in]   iic  handle to operate.
  \param[in]   devaddr  iic addrress of slave device.
  \param[in]   memaddr  internal addr of device.
  \param[in]   memaddr_mode  internal addr mode of device.
  \param[out]  data  pointer to buffer for data to receive from eeprom device.
  \param[in]   size  size of data items to receive.
  \param[in]   timeout  unit of time delay(ms).
  \return      the amount of real data received.
*/
int32_t csi_usi_iic_mem_receive(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t intr_en_status;
    ck_usi_regs_t *addr;
    int32_t ret = CSI_OK;
    int32_t read_count = 0;
    uint8_t mem_buf[2];
    uint8_t mem_addr_flag = 1U;

    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    intr_en_status =   wj_usi_get_intr_en(addr);
    wj_usi_set_intr_en(addr, 0U); ///<disable all interrupt

    wj_usi_set_i2c_addr(addr, devaddr);

    if (memaddr_size == IIC_MEM_ADDR_SIZE_8BIT) {
        ret = drv_usi_common_send(addr, (const uint8_t *)&memaddr, 1U, timeout, 1);

        if (ret != 1) {
            mem_addr_flag = 0U;
        }
    } else if (memaddr_size == IIC_MEM_ADDR_SIZE_16BIT) {
        mem_buf[0] = (uint8_t)(memaddr >> 8U);
        mem_buf[1] = (uint8_t)memaddr;
        ret = drv_usi_common_send(addr, (const uint8_t *)mem_buf, 2U, timeout, 1);

        if (ret != 2) {
            mem_addr_flag = 0U;
        }
    }

    mdelay(5);    ///< This delay parameter is due to the hardware characteristics of eeprom

    if (mem_addr_flag == 1U) {
        read_count = drv_usi_common_receive(addr, data, size, timeout);

    } else {
        read_count = (int32_t)CSI_ERROR;
    }

    wj_usi_set_intr_en(addr, intr_en_status);
    return read_count;

}

/**
  \brief       start sending data as iic slave.
               This function is blocking.
  \param[in]   iic  handle to operate.
  \param[in]   data  pointer to buffer with data to send to iic master.
  \param[in]   size  size of data items to send.
  \param[in]   timeout  unit of time delay(ms).
  \return      the amount of real data sent.
*/
int32_t csi_usi_iic_slave_send(csi_iic_t *iic, const void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    int32_t send_count = 0;
    uint8_t *send_data = (void *)data;
    uint32_t intr_en_status;
    csi_error_t ret = CSI_OK;
    uint32_t timecount = csi_tick_get_ms();
    ck_usi_regs_t *addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    timecount = (timecount + timeout) <= timeout ? timeout : (timecount + timeout);

    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_en(addr);

    wj_usi_set_intr_en(addr, 0U); ///<disable all interrupt

    while (!wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
        if ((csi_tick_get_ms() >= timecount)) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    if ((send_data == NULL) || (size == 0U) || (ret != CSI_OK)) {
        send_count = (int32_t)CSI_ERROR;
    } else {

        send_count = drv_usi_common_send(addr, (const uint8_t *)data, size,  timeout, 1);

    }

    wj_usi_set_intr_en(addr, intr_en_status);
    return send_count;
}

/**
  \brief       Start receiving data as iic slave.
               This function is blocking.
  \param[in]   iic  handle to operate.
  \param[out]  data  pointer to buffer for data to receive from iic master.
  \param[in]   size  size of data items to receive.
  \param[in]   timeout  unit of time delay(ms).
  \return      the amount of real data received.
*/
int32_t csi_usi_iic_slave_receive(csi_iic_t *iic, void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    int32_t send_count = 0;
    uint8_t *send_data = (void *)data;
    uint32_t intr_en_status;
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    uint32_t timecount = csi_tick_get_ms();

    timecount = (timecount + timeout) <= timeout ? timeout : (timecount + timeout);

    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_en(addr);

    wj_usi_set_intr_en(addr, 0U); //</disable all interrupt

    while (!wj_usi_get_usi_i2c_sta_i2cs_work(addr)) {
        if ((csi_tick_get_ms() >= timecount)) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    if ((send_data == NULL) || (size == 0U) || (ret != CSI_OK)) {
        send_count = (int32_t)CSI_ERROR;
    } else {

        send_count = drv_usi_common_receive(addr, data, size,  timeout);

    }

    wj_usi_set_intr_en(addr, intr_en_status);
    return send_count;
}

csi_error_t usi_iic_slave_send_async(csi_iic_t *iic, const void *data, uint32_t size)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);

    iic->size = size;
    iic->data = (uint8_t *)data;

    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_set_tx_fifo_th(addr, WJ_USI_INTR_CTRL_TX_FIFO_TH_0BYTE);
    wj_usi_set_rx_edge_le(addr);
    wj_usi_en_intr_tx_thold(addr);
    wj_usi_dis_tx_thold_mask(addr);

    return  CSI_OK;
}

/**
  \brief       Start sending data as iic slave.
               This function is non-blocking,\ref csi_usi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   iic  handle to operate.
  \param[in]   data  pointer to buffer with data to send to iic master.
  \param[in]   size  size of data items to send.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_slave_send_async(csi_iic_t *iic, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_iic_slave_send_async_t send_func;

    if (iic->send) {
        send_func = iic->send;
        send_func(iic, data, size);
    } else {
        usi_iic_slave_send_async(iic, data, size);
    }

    return ret;
}

csi_error_t usi_iic_slave_receive_async(csi_iic_t *iic, void *data, uint32_t size)
{
    ck_usi_regs_t *addr ;
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    iic->size = size;
    iic->data = data;

    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_en_i2cs_ctrl_addr_gcall_mode(addr);
    wj_usi_set_rx_fifo_th(addr, WJ_USI_INTR_CTRL_RX_FIFO_TH_1BYTE);
    wj_usi_set_rx_edge_ge(addr);
    wj_usi_dis_rx_thold_mask(addr);
    wj_usi_en_intr_rx_thold(addr);
    return  CSI_OK;
}

/**
  \brief       Start receiving data as iic slave.
               This function is non-blocking,\ref csi_usi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  pointer to buffer for data to receive from iic master.
  \param[in]   size  size of data items to receive.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_slave_receive_async(csi_iic_t *iic, void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_iic_slave_receive_async_t receive_func;

    if (iic->receive) {
        receive_func = iic->receive;
        receive_func(iic, data, size);
    } else {
        csi_irq_enable((uint32_t)iic->dev.irq_num);
        usi_iic_slave_receive_async(iic, data, size);
    }

    return ret ;
}

/**
  \brief       attach callback to the iic.
  \param[in]   iic  iic handle to operate.
  \param[in]   cb  event callback function \ref csi_usi_iic_callback_t.
  \param[in]   arg  user private param for event callback.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_attach_callback(csi_iic_t *iic, void *callback, void *arg)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    iic->callback  = callback;
    iic->arg = arg;
    csi_irq_attach((uint32_t)iic->dev.irq_num, &wj_usi_irq_handler, &iic->dev);
    csi_irq_enable((uint32_t)iic->dev.irq_num);
    return CSI_OK;
}

/**
  \brief       detach callback from the iic.
  \param[in]   iic  iic handle to operate.
  \return      \ref csi_error_t.
*/
void csi_usi_iic_detach_callback(csi_iic_t *iic)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK_NORETVAL(iic);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(iic);
    wj_usi_set_intr_en(addr, 0U);
    wj_usi_set_intr_unmask(addr, 0U);
    csi_irq_disable((uint32_t)iic->dev.irq_num);
    csi_irq_detach((uint32_t)iic->dev.irq_num);
    iic->callback   = NULL;
}

/**
  \brief       config iic stop to generate.
  \param[in]   iic  iic handle to operate.
  \param[in]   enable  transfer operation is pending - stop condition will not be generated.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_xfer_pending(csi_iic_t *iic, bool enable)
{
    return CSI_UNSUPPORTED;
}


static void dw_dma_iic_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    CSI_PARAM_CHK_NORETVAL(dma);
    csi_iic_t *iic = (csi_iic_t *)dma->parent;
    csi_iic_event_t  event_type = IIC_EVENT_ERROR;
    volatile uint32_t  time_buff = 0U;

    if (event == DMA_EVENT_TRANSFER_ERROR) {/* DMA transfer ERROR */
        if (iic->tx_dma->ch_id == dma->ch_id) {
            csi_dma_ch_stop(dma);

            if (iic->callback) {
                iic->callback(iic, IIC_EVENT_ERROR, iic->arg);
                iic->state.error = 1U;
            }
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) {
        /* DMA transfer complete */
        if ((iic->tx_dma != NULL) && (iic->tx_dma->ch_id == dma->ch_id)) {
            /* to do tx action */
            csi_dma_ch_stop(dma);
            event_type = IIC_EVENT_SEND_COMPLETE;
        } else if ((iic->rx_dma != NULL) && (iic->rx_dma->ch_id == dma->ch_id)) {
            /* to do tx action */
            csi_dma_ch_stop(dma);
            event_type = IIC_EVENT_RECEIVE_COMPLETE;
        } else {
            /* error code */
        }

        while ((0 == wj_usi_get_fifo_sta_tx_empty((ck_usi_regs_t *)iic->dev.reg_base)) || wj_usi_get_i2c_sta_i2cm_work((ck_usi_regs_t *)iic->dev.reg_base) || wj_usi_get_usi_i2c_sta_i2cs_work((ck_usi_regs_t *)iic->dev.reg_base)) {
            if (time_buff >= 6250000U) {            ///< 10s timeout
                event_type = IIC_EVENT_ERROR;
                break;
            }

            time_buff ++;
        }

        if (iic->callback) {
            iic->callback(iic, event_type, iic->arg);

            if (event_type == IIC_EVENT_SEND_COMPLETE) {
                iic->state.writeable = 1U;
            } else if (event_type == IIC_EVENT_RECEIVE_COMPLETE) {
                iic->state.readable = 1U;
            }
        }
    }

}

static csi_error_t usi_iic_slave_receive_dma(csi_iic_t *iic, void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    csi_dma_ch_config_t config;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)iic->dev.reg_base;
    csi_dma_ch_t *dma = (csi_dma_ch_t *)iic->rx_dma;

    iic->data = (uint8_t *)data;
    iic->size = size;
    iic->state.readable = 0U;
    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)iic->dev.irq_num);
    wj_usi_set_dma_threshold_rx(addr, 1U);
    wj_usi_en_dma_ctrl_rx(addr);

    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    config.group_len = 1U;
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = usi_rx_hs_num[iic->dev.idx];

    csi_dma_ch_config(dma, &config);
    soc_dcache_clean_invalid_range((unsigned long)iic->data, iic->size);
    csi_dma_ch_start(iic->rx_dma, (uint8_t *) & (addr->USI_TX_RX_FIFO), iic->data, iic->size);

    return ret;
}

static csi_error_t usi_iic_master_send_dma(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    csi_dma_ch_config_t config;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)iic->dev.reg_base;
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)iic->tx_dma;

    iic->data = (void *)data;
    iic->size = size;
    iic->state.writeable = 0U;
    csi_irq_disable((uint32_t)iic->dev.irq_num);
    wj_usi_set_i2c_addr(addr, devaddr);
    wj_usi_set_dma_threshold_tx(addr, (WJ_USI_FIFO_MAX - usi_find_max_prime_num(size)));
    wj_usi_en_dma_ctrl_tx(addr);

    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    /* config for wj_dma */
    config.group_len = usi_find_max_prime_num(size);
    config.trans_dir = DMA_MEM2PERH;
    /* config for etb */
    config.handshake = usi_tx_hs_num[iic->dev.idx];

    csi_dma_ch_config(dma_ch, &config);

    soc_dcache_clean_invalid_range((unsigned long)iic->data, iic->size);
    csi_dma_ch_start(iic->tx_dma, (void *)iic->data, (void *) & (addr->USI_TX_RX_FIFO), iic->size);
    return ret;
}

static csi_error_t usi_iic_slave_send_dma(csi_iic_t *iic, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    csi_dma_ch_config_t config;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)iic->dev.reg_base;
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)iic->tx_dma;

    iic->data = (void *)data;
    iic->size = size;
    iic->state.writeable = 0U;
    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)iic->dev.irq_num);
    wj_usi_set_dma_threshold_tx(addr, (WJ_USI_FIFO_MAX - usi_find_max_prime_num(size)));
    wj_usi_en_dma_ctrl_tx(addr);

    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    /* config for wj_dma */
    config.group_len = usi_find_max_prime_num(size);
    config.trans_dir = DMA_MEM2PERH;
    /* config for handshake */
    config.handshake = usi_tx_hs_num[iic->dev.idx];

    csi_dma_ch_config(dma_ch, &config);
    soc_dcache_clean_invalid_range((unsigned long)iic->data, iic->size);
    csi_dma_ch_start(iic->tx_dma, iic->data, (uint8_t *) & (addr->USI_TX_RX_FIFO),  iic->size);

    return ret;
}

/**
  \brief       link DMA channel to iic device.
  \param[in]   iic  handle to operate.
  \param[in]   tx_dma  the DMA channel handle for send, when it is NULL means to unlink the channel.
  \param[in]   rx_dma  the DMA channel handle for receive, when it is NULL means to unlink the channel.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_link_dma(csi_iic_t *iic, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    csi_error_t ret = CSI_ERROR;

    if (iic->callback != NULL) {
        if (iic->mode == IIC_MODE_MASTER) {
            /*the hardware only support master send with dma and master receive only use interrupt*/
            if (tx_dma != NULL) {
                tx_dma->parent = iic;
                ret = csi_dma_ch_alloc(tx_dma, -1, -1);

                if (ret == CSI_OK) {
                    csi_dma_ch_attach_callback(tx_dma, dw_dma_iic_dma_event_cb, NULL);
                    iic->tx_dma = tx_dma;
                    iic->send = usi_iic_master_send_dma;
                } else {
                    tx_dma->parent = NULL;
                }
            } else {
                if (iic->tx_dma) {
                    csi_dma_ch_detach_callback(iic->tx_dma);
                    csi_dma_ch_free(iic->tx_dma);
                    iic->send = NULL;
                    iic->tx_dma = NULL;
                }

                ret = CSI_OK;
            }
        } else if (iic->mode == IIC_MODE_SLAVE) {
            if (tx_dma != NULL) {
                tx_dma->parent = iic;
                ret = csi_dma_ch_alloc(tx_dma, -1, -1);

                if (ret == CSI_OK) {
                    csi_dma_ch_attach_callback(tx_dma, dw_dma_iic_dma_event_cb, NULL);
                    iic->tx_dma = tx_dma;
                    iic->send = usi_iic_slave_send_dma;
                } else {
                    tx_dma->parent = NULL;
                }
            } else {
                if (iic->tx_dma) {
                    csi_dma_ch_detach_callback(iic->tx_dma);
                    csi_dma_ch_free(iic->tx_dma);
                    iic->send = usi_iic_slave_send_async;
                    iic->tx_dma = NULL;
                }

                ret = CSI_OK;
            }

            /*the hardware only support slave receive with dma and slave send only use interrupt*/
            if (rx_dma != NULL) {
                rx_dma->parent = iic;
                ret = csi_dma_ch_alloc(rx_dma, -1, -1);

                if (ret == CSI_OK) {
                    iic->rx_dma = rx_dma;
                    csi_dma_ch_attach_callback(rx_dma, dw_dma_iic_dma_event_cb, NULL);
                    iic->receive = usi_iic_slave_receive_dma;
                } else {
                    rx_dma->parent = NULL;
                }
            } else {
                if (iic->rx_dma) {
                    csi_dma_ch_detach_callback(iic->rx_dma);
                    csi_dma_ch_free(iic->rx_dma);
                    iic->receive = usi_iic_slave_receive_async;
                    iic->rx_dma = NULL;
                }

                ret = CSI_OK;
            }
        }
    }

    return ret;
}

/**
  \brief       get iic state.
  \param[in]   iic  handle to operate.
  \param[out]  state  iic state \ref csi_state_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_get_state(csi_iic_t *iic, csi_state_t *state)
{
    *state = iic->state;
    return  CSI_OK;
}
#ifdef CONFIG_PM
/**
  \brief       enable iic power manage.
  \param[in]   iic  iic handle to operate.
  \return      \ref csi_error_t.
*/
csi_error_t csi_usi_iic_enable_pm(csi_iic_t *iic)
{
    return  usi_enable_pm(&iic->dev);
}

/**
  \brief       disable iic power manage.
  \param[in]   iic  iic handle to operate.
*/
void csi_usi_iic_disable_pm(csi_iic_t *iic)
{
    usi_disable_pm(&iic->dev);
}
#endif
