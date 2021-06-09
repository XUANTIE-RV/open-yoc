/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_wrap_iic.c
 * @brief    header file for wrap iic driver
 * @version  V2.0
 * @date     02. Mar 2020
 ******************************************************************************/
#include <drv/irq.h>
#include <drv/iic.h>
#include <wj_usi_ll.h>
#include <drv/usi_iic.h>

/**
  \brief       init iic ctrl block.
               initializes the resources needed for the iic instance.
  \param[in]   iic  handle of iic instance.
  \param[in]   idx  index of instance.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_init(csi_iic_t *iic, uint32_t idx)
{
    return csi_usi_iic_init(iic, idx);
}

/**
  \brief       uninit iic ctrl block.
               stops operation and releases the software resources used by the instance.
  \param[in]   iic  handle of iic instance.
*/
void csi_iic_uninit(csi_iic_t *iic)
{
    csi_usi_iic_uninit(iic);
}

/**
  \brief       config iic master or slave mode.
  \param[in]   iic  handle of iic instance.
  \param[in]   mode  iic mode \ref csi_iic_mode_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode)
{
    return csi_usi_iic_mode(iic, mode);
}
/**
  \brief       config iic speed.
  \param[in]   iic  handle of iic instance.
  \param[in]   speed  iic speed mode \ref csi_iic_speed_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed)
{
    return  csi_usi_iic_speed(iic, speed);
}

/**
  \brief       config iic own addr.
  \param[in]   iic  handle of iic instance.
  \param[in]   own_addr  iic set own addr at slave mode.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_own_addr(csi_iic_t *iic, uint32_t own_addr)
{
    return csi_usi_iic_own_addr(iic, own_addr);
}

/**
  \brief       config iic addr mode.
  \param[in]   iic  handle of iic instance.
  \param[in]   addr_mode  iic addr mode \ref csi_iic_addr_mode_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode)
{
    return csi_usi_iic_addr_mode(iic, addr_mode);
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
int32_t csi_iic_master_send(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_master_send(iic,  devaddr, data,  size,  timeout);
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
int32_t csi_iic_master_receive(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_master_receive(iic, devaddr, data,  size,  timeout);
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
csi_error_t csi_iic_master_send_async(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
    return csi_usi_iic_master_send_async(iic,  devaddr,  data,  size);
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
csi_error_t csi_iic_master_receive_async(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size)
{
    return csi_usi_iic_master_receive_async(iic,  devaddr, data,  size);
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
int32_t csi_iic_mem_send(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, const void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_mem_send(iic,  devaddr, memaddr,  memaddr_size, data,  size,  timeout);
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
int32_t csi_iic_mem_receive(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_mem_receive(iic,  devaddr,  memaddr,  memaddr_size, data,  size,  timeout);
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
int32_t csi_iic_slave_send(csi_iic_t *iic, const void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_slave_send(iic,  data,  size,  timeout);
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
int32_t    csi_iic_slave_receive(csi_iic_t *iic, void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_iic_slave_receive(iic, data,  size,  timeout);
}

/**
  \brief       Start sending data as iic slave.
               This function is non-blocking,\ref csi_usi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   iic  handle to operate.
  \param[in]   data  pointer to buffer with data to send to iic master.
  \param[in]   size  size of data items to send.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_slave_send_async(csi_iic_t *iic, const void *data, uint32_t size)
{
    return csi_usi_iic_slave_send_async(iic, data,  size);
}

/**
  \brief       Start receiving data as iic slave.
               This function is non-blocking,\ref csi_usi_iic_event_t is signaled when transfer completes or error happens.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  pointer to buffer for data to receive from iic master.
  \param[in]   size  size of data items to receive.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_slave_receive_async(csi_iic_t *iic, void *data, uint32_t size)
{
    return csi_usi_iic_slave_receive_async(iic, data,  size) ;
}

/**
  \brief       attach callback to the iic.
  \param[in]   iic  iic handle to operate.
  \param[in]   cb  event callback function \ref csi_usi_iic_callback_t.
  \param[in]   arg  user private param for event callback.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_attach_callback(csi_iic_t *iic, void *callback, void *arg)
{
    return csi_usi_iic_attach_callback(iic, callback, arg);
}

/**
  \brief       detach callback from the iic.
  \param[in]   iic  iic handle to operate.
  \return      \ref csi_error_t.
*/
void        csi_iic_detach_callback(csi_iic_t *iic)
{
    csi_usi_iic_detach_callback(iic);
}

/**
  \brief       config iic stop to generate.
  \param[in]   iic  iic handle to operate.
  \param[in]   enable  transfer operation is pending - stop condition will not be generated.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_xfer_pending(csi_iic_t *iic, bool enable)
{
    return CSI_UNSUPPORTED;
}

/**
  \brief       link DMA channel to iic device.
  \param[in]   iic  handle to operate.
  \param[in]   tx_dma  the DMA channel handle for send, when it is NULL means to unlink the channel.
  \param[in]   rx_dma  the DMA channel handle for receive, when it is NULL means to unlink the channel.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_link_dma(csi_iic_t *iic, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    return csi_usi_iic_link_dma(iic, tx_dma, rx_dma);
}

/**
  \brief       get iic state.
  \param[in]   iic  handle to operate.
  \param[out]  state  iic state \ref csi_state_t.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_get_state(csi_iic_t *iic, csi_state_t *state)
{
    return  csi_usi_iic_get_state(iic, state);

}

/**
  \brief       enable iic power manage.
  \param[in]   iic  iic handle to operate.
  \return      \ref csi_error_t.
*/
csi_error_t csi_iic_enable_pm(csi_iic_t *iic)
{
    return csi_usi_iic_enable_pm(iic);
}

/**
  \brief       disable iic power manage.
  \param[in]   iic  iic handle to operate.
*/
void csi_iic_disable_pm(csi_iic_t *iic)
{
    csi_usi_iic_disable_pm(iic);
}

