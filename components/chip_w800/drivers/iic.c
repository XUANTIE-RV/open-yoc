/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     iic.c
 * @brief    CSI Source File for IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "wm_osal.h"
#include "wm_i2c.h"

#ifdef bool
#undef bool
#endif

#include <csi_config.h>
#include <drv/iic.h>
#include <drv/irq.h>
#include <soc.h>
#include <csi_core.h>
#include <string.h>
#include <sys_freq.h>

#define ERR_IIC(errno) (CSI_DRV_ERRNO_IIC_BASE | errno)

typedef struct {	
	__IO uint32_t PRER_LO;	
	__IO uint32_t PRER_HI;	
	__IO uint32_t CTRL;	
	__O  uint32_t TX_RX;	
	__O  uint32_t CR_SR;	
	__I  uint32_t TXR;	
	__I  uint32_t CR;
} dw_iic_reg_t;

typedef struct  {
    int32_t idx;
    uint32_t base;
    uint32_t irq;
    iic_event_cb_t cb_event;

    int32_t slave_addr;

    uint8_t bit10;
    uint8_t direction;

    uint8_t busy;
    uint32_t rx_cnt;
    uint32_t tx_cnt;
} dw_iic_priv_t;

extern int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static dw_iic_priv_t iic_instance[CONFIG_IIC_NUM];

static const iic_capabilities_t iic_capabilities = {
    .address_10_bit = 1  /* supports 10-bit addressing */
};

static int wm_i2c_send_address(uint16_t addr, bool bit10, bool read)
{
    u8 flags = 0;
    u8 addr_msb;
    u8 addr_lsb;
    int ret;

    if (bit10) {
        addr_msb = 0x7 & (addr >> 7);
        addr_lsb = addr & 0x7f;
#if 1
        if (read) {
            flags |= 0x01;
        }
#endif
        tls_i2c_write_byte((addr_lsb << 1) | flags, 1);
        ret = tls_i2c_wait_ack();

        if (!ret) {
            tls_i2c_write_byte(addr_msb, 0);
            ret = tls_i2c_wait_ack();
        }
    } else {
#if 1
        if (read) {
            flags |= 0x01;
        }
#endif
        tls_i2c_write_byte((addr << 1) | flags, 1);
        ret = tls_i2c_wait_ack();
    }

    return ret;
}

void iic_irqhandler(int32_t idx)
{
    dw_iic_priv_t *iic_priv = &iic_instance[idx];
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    uint32_t intr_stat = addr->CR_SR;

    addr->CR_SR = 1;

    if (intr_stat & 0x20)
	{
		if (iic_priv->cb_event)
		    iic_priv->cb_event(iic_priv->idx, IIC_EVENT_ARBITRATION_LOST);
	}
}

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

    iic_priv->bit10 = 0;
    iic_priv->direction = 0;

    iic_priv->busy = 0;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;

    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);
	addr->CTRL = I2C_CTRL_INT_DISABLE | I2C_CTRL_ENABLE;

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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->cb_event = NULL;

    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;

    drv_irq_disable(iic_priv->irq);
    drv_irq_unregister(iic_priv->irq);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx iic index.
  \return      \ref iic_capabilities_t
*/
iic_capabilities_t csi_iic_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_IIC_NUM) {
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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    uint32_t div;
    uint32_t freq;
    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (mode >= 0)
    {
        if (IIC_MODE_MASTER != mode)
            return ERR_IIC(DRV_ERROR_UNSUPPORTED);
    }

    if (speed >= 0)
    {
        if (IIC_BUS_SPEED_STANDARD == speed)
            freq = 100000;
        else if (IIC_BUS_SPEED_FAST == speed)
            freq = 400000;
        else
            return ERR_IIC(DRV_ERROR_PARAMETER);

        div = drv_get_apb_freq(0) / (5 * freq) - 1;
        addr->PRER_LO = div & 0xff;
    	addr->PRER_HI = (div >> 8) & 0xff;
    }

    if (addr_mode >= 0)
    {
        if (IIC_ADDRESS_7BIT == addr_mode)
            iic_priv->bit10 = 0;
        else if (IIC_ADDRESS_10BIT == addr_mode)
            iic_priv->bit10 = 1;
        else
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    iic_priv->slave_addr = slave_addr;

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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    if (IIC_MODE_MASTER != mode)
        return ERR_IIC(DRV_ERROR_UNSUPPORTED);

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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    uint32_t div;
    uint32_t freq;
    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    if (IIC_BUS_SPEED_STANDARD == speed)
        freq = 100000;
    else if (IIC_BUS_SPEED_FAST == speed)
        freq = 400000;
    else
        return ERR_IIC(DRV_ERROR_PARAMETER);

    div = drv_get_apb_freq(0) / (5 * freq) - 1;
    addr->PRER_LO = div & 0xff;
	addr->PRER_HI = (div >> 8) & 0xff;
	addr->CTRL = I2C_CTRL_INT_DISABLE | I2C_CTRL_ENABLE;

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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;

    if (IIC_ADDRESS_7BIT == addr_mode)
        iic_priv->bit10 = 0;
    else if (IIC_ADDRESS_10BIT == addr_mode)
        iic_priv->bit10 = 1;
    else
        return ERR_IIC(DRV_ERROR_PARAMETER);

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
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->slave_addr = slave_addr;

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
    if (!handle || !data || !num)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->busy = 1;
    iic_priv->tx_cnt = 0;
    iic_priv->direction = 0;

    int ret = wm_i2c_send_address(devaddr, iic_priv->bit10, 0);
    if (ret)
    {
        iic_priv->busy = 0;
        return ERR_IIC(DRV_ERROR_SPECIFIC);
    }

    uint8_t *buf = (uint8_t *)data;
    int txl = num;

    while (txl > 0) {
        tls_i2c_write_byte(*buf, 0);
        ret = tls_i2c_wait_ack();
        if (ret)
        {
            iic_priv->busy = 0;
            return ERR_IIC(DRV_ERROR_SPECIFIC);
        }
        txl--;
        buf++;
    }

    if (!xfer_pending)
    {
        tls_i2c_stop();
        tls_os_time_delay(1);
    }

    iic_priv->tx_cnt = num;
    iic_priv->busy = 0;

    if (iic_priv->cb_event)
        iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);

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
    if (!handle || !data || !num)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;

    iic_priv->busy = 1;
    iic_priv->rx_cnt = 0;
    iic_priv->direction = 1;

    int ret = wm_i2c_send_address(devaddr, iic_priv->bit10, 1);
    if (ret)
    {
        iic_priv->busy = 0;
        return ERR_IIC(DRV_ERROR_SPECIFIC);
    }

    uint8_t *dest = data;
    ret = num;
    while (ret > 1) {
        *dest++ = tls_i2c_read_byte(1, 0);
        ret--;
    }
    *dest = tls_i2c_read_byte(0, 1);

    if (!xfer_pending) {
        tls_i2c_stop();
    }

    iic_priv->rx_cnt = num;
    iic_priv->busy = 0;

    if (iic_priv->cb_event)
        iic_priv->cb_event(iic_priv->idx, IIC_EVENT_TRANSFER_DONE);

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
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
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
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_abort_transfer(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);//return csi_iic_send_stop(handle);
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
    //dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    //if (addr->CR_SR & I2C_SR_TIP)
    //    iic_status.busy = 1;
    iic_status.busy = iic_priv->busy;
    iic_status.mode = 1;
    iic_status.direction = iic_priv->direction;

    return iic_status;
}

/**
  \brief       Get IIC transferred data count.
  \param[in]   handle  iic handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_iic_get_data_count(iic_handle_t handle)
{
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    uint32_t cnt = 0;
    dw_iic_priv_t *iic_priv = handle;

    if (iic_priv->busy)
    {
        cnt = 0;
    }
    else if (1 == iic_priv->direction)
    {
        cnt = iic_priv->rx_cnt;
    }
    else if (0 == iic_priv->direction)
    {
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
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}
/**
  \brief       Send START command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_start(iic_handle_t handle)
{
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    addr->CR_SR = I2C_CR_STA;

    return 0;
}

/**
  \brief       Send STOP command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_stop(iic_handle_t handle)
{
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    dw_iic_priv_t *iic_priv = handle;
    dw_iic_reg_t *addr = (dw_iic_reg_t *)(iic_priv->base);

    addr->CR_SR = I2C_CR_STO;

    return 0;
}

/**
  \brief       Reset IIC peripheral.
  \param[in]   handle  iic handle to operate.
  \return      error code
  \note The action here. Most of the implementation sends stop.
*/
int32_t csi_iic_reset(iic_handle_t handle)
{
    if (!handle)
        return ERR_IIC(DRV_ERROR_PARAMETER);

    tls_i2c_stop();

    return 0;
}
