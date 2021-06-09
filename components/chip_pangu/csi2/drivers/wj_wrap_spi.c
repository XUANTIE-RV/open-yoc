/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_wrap_spi.c
 * @brief    header file for wrap spi ll driver
 * @version  V2.0
 * @date     02. Mar 2020
 ******************************************************************************/
#include <drv/irq.h>
#include <drv/spi.h>
#include <wj_usi_ll.h>
#include <drv/usi_spi.h>
#include <drv/common.h>
#include <drv/tick.h>

/**
  \brief       Initialize SPI Interface.
               Initializes the resources needed for the SPI instance
  \param[in]   spi      spi handle
  \param[in]   idx      spi instance index
  \return      error code
*/
csi_error_t csi_spi_init(csi_spi_t *spi, uint32_t idx)
{
    return  csi_usi_spi_init(spi, idx);
}

/**
  \brief       De-initialize SPI Interface
               stops operation and releases the software resources used by the spi instance
  \param[in]   spi spi handle
  \return      none
*/
void csi_spi_uninit(csi_spi_t *spi)
{
    csi_usi_spi_uninit(spi);
}


/**
  \brief       Attach the callback handler to SPI
  \param[in]   spi  operate handle.
  \param[in]   callback    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_spi_attach_callback(csi_spi_t *spi, void *callback, void *arg)
{
    return csi_usi_spi_attach_callback(spi, callback, arg);
}

/**
  \brief       Detach the callback handler
  \param[in]   spi  operate handle.
  \return      none
*/
void csi_spi_detach_callback(csi_spi_t *spi)
{
    csi_usi_spi_detach_callback(spi);
}

/**
  \brief       Config spi mode (master or slave).
  \param[in]   spi    spi handle
  \param[in]   mode   the mode of spi (master or slave)
  \return      error code
*/
csi_error_t csi_spi_mode(csi_spi_t *spi, csi_spi_mode_t mode)
{
    return csi_usi_spi_mode(spi, mode);
}


/**
  \brief       Config spi cp format
  \param[in]   spi       SPI handle
  \param[in]   format    SPI cp format
  \return      Error code
*/
csi_error_t csi_spi_cp_format(csi_spi_t *spi, csi_spi_cp_format_t format)
{
    return csi_usi_spi_cp_format(spi,format);
}
/**
  \brief       Config spi frame len.
  \param[in]   spi       spi handle
  \param[in]   length    spi frame len
  \return      error code
*/
csi_error_t csi_spi_frame_len(csi_spi_t *spi, csi_spi_frame_len_t length)
{
    return csi_usi_spi_frame_len(spi, length);
}

/**
  \brief       Config spi work frequence.
  \param[in]   spi     spi handle
  \param[in]   baud    spi work baud
  \return      the actual config frequency
*/
uint32_t csi_spi_baud(csi_spi_t *spi, uint32_t baud)
{
    return csi_usi_spi_baud(spi,  baud);
}

/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               blocking mode ,return unti all data has been sent or err happened
  \param[in]   spi   handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter.
  \param[in]   size  Number of data to send(byte)
  \param[in]   timeout  unit in mini-second
  \return      if send success, this function shall return the num of data witch is sent successful
               otherwise, the function shall return error code
*/
int32_t csi_spi_send(csi_spi_t *spi, const void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_spi_send(spi, data, size, timeout);
}


/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               non-blocking mode,transfer done event will be signaled by driver
  \param[in]   spi   handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter.
  \param[in]   size  Number of data items to send(byte)
  \return      error code
*/

csi_error_t csi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
    return csi_usi_spi_send_async(spi, data, size);
}


/**
  \brief       receiving data from SPI receiver.
               blocking mode, return untill curtain data items are readed
  \param[in]   spi   handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   size  Number of data items to receive(byte)
  \param[in]   timeout  unit in mini-second
  \return      if receive success, this function shall return the num of data witch is received successful
               otherwise, the function shall return error code
*/

int32_t csi_spi_receive(csi_spi_t *spi, void *data, uint32_t size, uint32_t timeout)
{
    return csi_usi_spi_receive(spi, data, size, timeout);

}

/**
  \brief       receiving data from SPI receiver.
               not-blocking mode, event will be signaled when receive done or err happend
  \param[in]   spi   handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   size  Number of data items to receive(byte)
  \return      error code
*/

csi_error_t csi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{
    return  csi_usi_spi_receive_async(spi, data, size);
}


/**
  \brief       dulplex,sending and receiving data at the same time
               \ref csi_spi_event_t is signaled when operation completes or error happens.
               \ref csi_spi_get_state can get operation status.
               blocking mode, this function returns after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   size      data size(byte)
  \return      if transfer success, this function shall return the num of data witch is transfer successful
               otherwise, the function shall return error code
*/
int32_t csi_spi_send_receive(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size, uint32_t timeout)
{
    return csi_usi_spi_send_receive(spi, data_out, data_in, size, timeout);
}

/**
  \brief       transmit first then receive ,receive will begin after transmit is done
               if non-blocking mode, this function only starts the transfer,
               \ref csi_spi_event_t is signaled when operation completes or error happens.
               \ref csi_spi_get_state can get operation status.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   size      data size(byte)
  \return      error code
*/
csi_error_t csi_spi_send_receive_async(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    return  csi_usi_spi_send_receive_async(spi, data_out, data_in, size);

}

/*
  \brief       set slave select num. Only valid for master
  \param[in]   handle  spi handle to operate.
  \param[in]   slave_num  spi slave num.
  \return      none
 */
void csi_spi_select_slave(csi_spi_t *spi, uint32_t slave_num)
{
    //csi_usi_spi_select_slave(spi, slave_num);
}

/**
  \brief       link DMA channel to spi device
  \param[in]   spi  spi handle to operate.
  \param[in]   tx_dma the DMA channel handle for send, when it is NULL means to unlink the channel
  \param[in]   rx_dma the DMA channel handle for receive, when it is NULL means to unlink the channel
  \return      error code
*/
csi_error_t csi_spi_link_dma(csi_spi_t *spi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    return csi_usi_spi_link_dma(spi, tx_dma, rx_dma);
}
/**
  \brief       get the state of spi device
  \param[in]   spi  spi handle to operate.
  \param[out]  state the state of spi device
  \return      error code
*/
csi_error_t csi_spi_get_state(csi_spi_t *spi, csi_state_t *state)
{
    return csi_usi_spi_get_state(spi, state);
}

/**
  \brief       enable spi power manage
  \param[in]   spi  spi handle to operate.
  \return      error code
*/
csi_error_t csi_spi_enable_pm(csi_spi_t *spi)
{
    return CSI_UNSUPPORTED;//csi_usi_spi_enable_pm(spi);
}

/**FFF
  \brief       disable spi power manage
  \param[in]   spi  spi handle to operate.
  \return      error code
*/
void csi_spi_disable_pm(csi_spi_t *spi)
{
  //  csi_usi_spi_disable_pm(spi);
}
