/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
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

#include <stdbool.h>
#include <string.h>
#include <drv/irq.h>
#include <drv/usart.h>
#include <drv/pmu.h>
#include <dw_usart.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/dmac.h>
#include "lsuart.h"
#include "field_manipulate.h"
#include "reg_rcc.h"
#include "platform.h"

struct csi_uart_inst
{
    reg_uart_t                    *UARTX;           /*!< UART registers base address        */
    usart_event_cb_t evt_cb;
}uart_inst[3];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};

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
  \brief       transmit character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   ch  the input character
  \return      error code
*/
int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    struct csi_uart_inst *huart = handle;
    huart->UARTX->TBR = ch;
    while (!(huart->UARTX->SR & UART_SR_TEMT));
    return 0;
}

void uart_isr(uint32_t idx)
{
    reg_uart_t *reg = uart_inst[idx].UARTX;
    uint32_t ifm = reg->IFM;
    if(ifm & UART_IT_RXRD)
    {
      if(uart_inst[idx].evt_cb)
      {
        uart_inst[idx].evt_cb(idx,USART_EVENT_RECEIVED);
      }else
      {
        while(reg->SR & UART_SR_RFNE)
        {
          reg->RBR;
        }
      }
    }
}

void UART1_Handler()
{
    uart_isr(0);
}

void UART2_Handler()
{
    uart_isr(1);
}

void UART3_Handler()
{
    uart_isr(2);
}

/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   idx usart index
  \param[in]   cb_event  Pointer to \ref usart_event_cb_t
  \return      return usart handle if success
*/

usart_handle_t csi_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    switch(idx)
    {
    case 0:
      REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 1);
      uart_inst[0].UARTX = UART1;
      arm_cm_set_int_isr(UART1_IRQn,UART1_Handler);
      __NVIC_EnableIRQ(UART1_IRQn);
    break;
    case 1:
      REG_FIELD_WR(RCC->APB1EN, RCC_UART2, 1);
      uart_inst[1].UARTX = UART2;
      arm_cm_set_int_isr(UART2_IRQn,UART2_Handler);
      __NVIC_EnableIRQ(UART2_IRQn);
    break;
    case 2:
      REG_FIELD_WR(RCC->APB1EN, RCC_UART3, 1);
      uart_inst[2].UARTX = UART3;
      arm_cm_set_int_isr(UART3_IRQn,UART3_Handler);
      __NVIC_EnableIRQ(UART3_IRQn);
    break;
    default:

    break;
    }
    uart_inst[idx].evt_cb = cb_event;
    return (void *)&uart_inst[idx];
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_uninitialize(usart_handle_t handle)
{
    struct csi_uart_inst *inst = handle;
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        __NVIC_DisableIRQ(UART1_IRQn);
        REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 0);
    break;
    case (uint32_t)UART2:
        __NVIC_DisableIRQ(UART2_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_UART2, 0);
    break;
    case (uint32_t)UART3:
        __NVIC_DisableIRQ(UART3_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_UART3, 0);
    break;
    }
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
    struct csi_uart_inst *inst = handle;
    REG_FIELD_WR(inst->UARTX->LCR,UART_LCR_BRWEN,1);
    inst->UARTX->BRR = (((UART_CLOCK<<4)/baud) +8)>>4;
    REG_FIELD_WR(inst->UARTX->LCR,UART_LCR_BRWEN,0);
    inst->UARTX->FCR = UART_FCR_TFRST_MASK | UART_FCR_RFRST_MASK | UART_FCR_FIFOEN_MASK;
    inst->UARTX->LCR = FIELD_BUILD(UART_LCR_DLS,bits)|FIELD_BUILD(UART_LCR_STOP,stopbits)
                                  |FIELD_BUILD(UART_LCR_PS,parity)|FIELD_BUILD(UART_LCR_MSB,0)
                                  |FIELD_BUILD(UART_LCR_RXEN,1);   
    REG_FIELD_WR(inst->UARTX->FCR,UART_FCR_RXTL,UART_FIFO_RL_1);
    inst->UARTX->IER = UART_RXRD_MASK;
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
    const uint8_t *ptr = data;
    const uint8_t *end = &ptr[num];
    while(ptr<end)
    {
      csi_usart_putchar(handle,*ptr++);
    }
    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_send(usart_handle_t handle)
{
    // TODO:

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
    // TODO:

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
    struct csi_uart_inst *huart = handle;
    if(huart->UARTX->SR & UART_SR_DR)
    {
      *(uint8_t *)data = huart->UARTX->RBR;
      return 1;
    }else
    {
      return 0;
    }
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_receive(usart_handle_t handle)
{
    // TODO:

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
    // TODO:

    return 0;
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_transfer(usart_handle_t handle)
{
    // TODO:

    return 0;
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t csi_usart_get_status(usart_handle_t handle)
{
    usart_status_t t;
    memset(&t, 0, sizeof(t));
    // TODO:

    return t;
}

/**
  \brief       control the transmit.
  \param[in]   handle  usart handle to operate.
  \param[in]   1 - enable the transmitter. 0 - disable the transmitter
  \return      error code
*/
int32_t csi_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    // TODO:

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
    // TODO:

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
    // TODO:

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
    // TODO:

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
    // TODO:

    return 0;
}

/**
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t csi_usart_get_tx_count(usart_handle_t handle)
{
    // TODO:

    return 0;
}

/**
  \brief       Get usart receive data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently received data bytes
*/
uint32_t csi_usart_get_rx_count(usart_handle_t handle)
{
    // TODO:

    return 0;
}

/**
  \brief       control usart power.
  \param[in]   handle  usart handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_usart_power_control(usart_handle_t handle, csi_power_stat_e state)
{
    // TODO:
    return 0;
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
    // TODO:
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
    // TODO:
    return 0;
}
