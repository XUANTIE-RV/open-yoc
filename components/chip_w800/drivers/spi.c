/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spi.c
 * @brief    CSI Source File for SPI Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <drv/spi.h>
#include <drv/irq.h>

#include <soc.h>
#include <csi_core.h>

#include "wm_type_def.h"
#include "wm_hostspi.h"
#include "wm_spi_hal.h"

#define ERR_SPI(errno) (CSI_DRV_ERRNO_SPI_BASE | errno)

typedef struct {
    uint32_t base;
    uint32_t irq;
    spi_event_cb_t cb_event;

    uint8_t busy;
    int32_t idx;
    uint32_t tot_num;
} dw_spi_priv_t;

extern u32 spi_fill_txfifo(struct tls_spi_transfer *current_transfer, u32 current_remaining_bytes);
extern u32 spi_get_rxfifo(struct tls_spi_transfer *current_transfer, u32 current_remaining_bytes);
extern int32_t target_get_lspi(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
extern void __SPI_LS_IRQHandler(void);

static dw_spi_priv_t spi_instance[CONFIG_LSPI_NUM];

static const spi_capabilities_t spi_capabilities = {
    .simplex = 1,           /* Simplex Mode (Master and Slave) */
    .ti_ssi = 1,            /* TI Synchronous Serial Interface */
    .microwire = 1,         /* Microwire Interface */
    .event_mode_fault = 0   /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

/**
  \brief       handler the interrupt.
  \param[in]   spi      Pointer to \ref SPI_RESOURCES
*/
void lspi_irqhandler(int32_t idx)
{
    __SPI_LS_IRQHandler();
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
    void *handler;

    int32_t ret = target_get_lspi(idx, &base, &irq, &handler);

    if (ret < 0 || ret >= CONFIG_LSPI_NUM) {
        return NULL;
    }

    tls_spi_init();

    dw_spi_priv_t *spi_priv = &spi_instance[idx];

    spi_priv->base = base;
    spi_priv->irq = irq;

    spi_priv->cb_event = cb_event;
    spi_priv->idx = idx;

    spi_priv->busy = 0;
    spi_priv->tot_num = 0;

    drv_irq_register(spi_priv->irq, handler);
    drv_irq_enable(spi_priv->irq);

    return spi_priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_uninitialize(spi_handle_t handle)
{
    if (!handle)
        return ERR_SPI(DRV_ERROR_PARAMETER);

    dw_spi_priv_t *spi_priv = handle;

    drv_irq_disable(spi_priv->irq);
    drv_irq_unregister(spi_priv->irq);

    spi_priv->cb_event = NULL;

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
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx spi index.
  \return      \ref spi_capabilities_t
*/
spi_capabilities_t csi_spi_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_LSPI_NUM) {
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
    if (!handle)
        return ERR_SPI(DRV_ERROR_PARAMETER);

    if (baud >= 0)
    {
        if ((baud < TLS_SPI_FCLK_MIN) || (baud > TLS_SPI_FCLK_MAX))
            return ERR_SPI(DRV_ERROR_PARAMETER);

        //spi_set_sclk(baud);
    }

    if (mode >= 0)
    {
        if (SPI_MODE_MASTER != mode)
            return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (format >= 0)
    {
#if 0
        if (SPI_FORMAT_CPOL0_CPHA0 == format)
            spi_set_mode(TLS_SPI_MODE_0);
        else if (SPI_FORMAT_CPOL0_CPHA1 == format)
            spi_set_mode(TLS_SPI_MODE_1);
        else if (SPI_FORMAT_CPOL1_CPHA0 == format)
            spi_set_mode(TLS_SPI_MODE_2);
        else if (SPI_FORMAT_CPOL1_CPHA1 == format)
            spi_set_mode(TLS_SPI_MODE_3);
        else
            return ERR_SPI(DRV_ERROR_PARAMETER);
#else
        if (format > SPI_FORMAT_CPOL1_CPHA1)
            return ERR_SPI(DRV_ERROR_PARAMETER);
#endif 
    }

    tls_spi_setup(format, 0, baud);

    if (order >= 0)
    {
        if (SPI_ORDER_MSB2LSB != order)
            return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (ss_mode >= 0)
    {
        if (SPI_SS_MASTER_SW == ss_mode)
            spi_force_cs_out(1);
        else if (SPI_SS_MASTER_HW_OUTPUT == ss_mode)
            spi_force_cs_out(0);
        else
            return ERR_SPI(DRV_ERROR_PARAMETER);
    }

#if 0
    if (bit_width >= 0)
    {
        /* bit_width not use */

        tls_spi_trans_type(SPI_DMA_TRANSFER);
    }
#endif

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
    int ret;
    dw_spi_priv_t *spi_priv = handle;

    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    spi_priv->busy = 1;
    spi_priv->tot_num = num;

    ret = tls_spi_write(data, num);

    spi_priv->busy = 0;

    if (spi_priv->cb_event && (ret == TLS_SPI_STATUS_OK))
        spi_priv->cb_event(spi_priv->idx, SPI_EVENT_TX_COMPLETE);

    return (ret == TLS_SPI_STATUS_OK) ? 0 : ERR_SPI(DRV_ERROR_SPECIFIC);
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
    int ret;
    dw_spi_priv_t *spi_priv = handle;

    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    spi_priv->busy = 1;
    spi_priv->tot_num = num;

    ret = tls_spi_read(data, num);

    spi_priv->busy = 0;

    if (spi_priv->cb_event && (ret == TLS_SPI_STATUS_OK))
        spi_priv->cb_event(spi_priv->idx, SPI_EVENT_RX_COMPLETE);

    return (ret == TLS_SPI_STATUS_OK) ? 0 : ERR_SPI(DRV_ERROR_SPECIFIC);
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
    int ret;
    dw_spi_priv_t *spi_priv = handle;
	uint32_t length = 0;
    uint32_t remain_length ;
    uint32_t int_status;	
    struct tls_spi_transfer tls_transfer;



    if (handle == NULL || data_in == NULL || num_out == 0 || num_in == 0 || data_out == NULL) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }
	
    tls_transfer.tx_buf = data_out;
    tls_transfer.rx_buf = data_in;

    spi_priv->busy = 1;
    spi_priv->tot_num = (num_out > num_in) ? num_out : num_in;
	remain_length = spi_priv->tot_num;
    tls_transfer.len = spi_priv->tot_num;	
	drv_irq_disable(spi_priv->irq);
    //spi_set_rx_channel(1);
    //spi_set_tx_channel(1);	
	length = spi_fill_txfifo(&tls_transfer, remain_length);

	spi_start_transfer(length);
	while (remain_length > 0)
	{
		while (spi_i2s_get_busy_status() == 1)
			;
		length = spi_get_rxfifo(&tls_transfer, remain_length);
		remain_length -= length;

		if (remain_length == 0)
		{
			while (spi_i2s_get_busy_status() == 1)
				;
			break;
		}
		while (spi_i2s_get_busy_status() == 1)
			;
		length = spi_fill_txfifo(&tls_transfer, remain_length);
		if (length)
		{
			spi_set_sclk_length(length * 8, 0);
			spi_sclk_start();
		}
	}

	while (spi_i2s_get_busy_status() == 1)
		;
	int_status = spi_get_int_status();
	spi_clear_int_status(int_status);
	drv_irq_enable(spi_priv->irq);

    spi_priv->busy = 0;

    if (spi_priv->cb_event && (ret == TLS_SPI_STATUS_OK))
        spi_priv->cb_event(spi_priv->idx, SPI_EVENT_TRANSFER_COMPLETE);

    return (ret == TLS_SPI_STATUS_OK) ? 0 : ERR_SPI(DRV_ERROR_SPECIFIC);
}	

/**
  \brief       abort spi transfer.
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_abort_transfer(spi_handle_t handle)
{
    return ERR_SPI(DRV_ERROR_PARAMETER);
}

/**
  \brief       Get SPI status.
  \param[in]   handle spi handle to operate.
  \return      SPI status \ref ARM_SPI_STATUS
*/
spi_status_t csi_spi_get_status(spi_handle_t handle)
{
    spi_status_t spi_status = {0};

    if (!handle)
        return spi_status;

    //uint8_t busy;

    //spi_get_status(&busy, NULL, NULL);

    dw_spi_priv_t *spi_priv = handle;

    spi_status.busy = spi_priv->busy;//busy;

    return spi_status;
}

/**
  \brief       Get spi transferred data count.
  \param[in]   handle  spi handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_spi_get_data_count(spi_handle_t handle)
{
    if (!handle)
        return ERR_SPI(DRV_ERROR_PARAMETER);

    dw_spi_priv_t *spi_priv = handle;

    if (spi_priv->busy)
        return 0;
    else
        return spi_priv->tot_num;
}

/**
  \brief       Control the Slave Select signal (SS).
  \param[in]   handle  spi handle to operate.
  \param[in]   stat    SS state. \ref spi_ss_stat_e.
  \return      error code
*/
int32_t csi_spi_ss_control(spi_handle_t handle, spi_ss_stat_e stat)
{
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}
