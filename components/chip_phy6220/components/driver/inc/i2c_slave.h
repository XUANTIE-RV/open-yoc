/**************************************************************************************************

  Phyplus Microelectronics Limited confidential and proprietary.
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics
  Limited ("Phyplus"). Your use of this Software is limited to those
  specific rights granted under  the terms of the business contract, the
  confidential agreement, the non-disclosure agreement and any other forms
  of agreements as a customer or a partner of Phyplus. You may not use this
  Software unless you agree to abide by the terms of these agreements.
  You acknowledge that the Software may not be modified, copied,
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
  (BLE) integrated circuit, either as a product or is integrated into your
  products.  Other than for the aforementioned purposes, you may not use,
  reproduce, copy, prepare derivative works of, modify, distribute, perform,
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/

/*******************************************************************************
* @file     i2c_slave.h
* @brief    i2c slave Configuration,API...
* @version  1.0
* Copyright(C) 2016, PhyPlus Semiconductor
* All rights reserved.
*******************************************************************************/

#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

/*******************************************************************************
*@ Module           :  Includes
*@ Description    :  NULL
*******************************************************************************/
#include "types.h"
#include "i2c_common.h"

/*******************************************************************************
*@ Module               :   I2C Slave Paramter structure
*@ Description    :   None
*******************************************************************************/
typedef struct {
    // General
    uint8_t                     id;                                         // set
    I2C_WorkMode            workmode;                               // init some Para ,according to workmode
    I2C_ADDRESS_e       AddressMode;
    uint8_t                     RX_FIFO_Len;                        // RX , TX FIFO SET
    uint8_t                     Tx_FIFO_Len;
    gpio_pin_e              SDA_PIN;                                // I2C Pin
    gpio_pin_e              SCL_PIN;
    uint32_t                    IRQ_Source;
    uint8_t                     Slave_Address;                  // SAR,when as Slave
    I2C_Hdl_t               evt_handler;
} I2C_Slave_Parameter;


/*******************************************************************************
*@ Module           :  Function Statement
*@ Description    :  NULL
*******************************************************************************/
uint8_t Hal_I2C_Slave_Init(I2C_Slave_Parameter *para, uint8_t *handle);
uint8_t Hal_I2c_Slave_Open(uint8_t handle);
void Hal_I2c_Slave_Close(uint8_t handle);
uint8_t Hal_I2C_Slave_Deinit(uint8_t *handle);
uint8_t Hal_Check_I2C_Slave_Closed(uint8_t handle);
void Hal_I2C_Slave_ReadRX_FIFO(uint8_t handle, uint8_t *p, uint8_t len);
void Hal_I2C_Slave_CLR_IRQs(uint8_t handle, uint32_t irqs);
void Hal_I2C_Slave_WriteTX_FIFO(uint8_t handle, uint8_t *p, uint8_t len);

#endif
