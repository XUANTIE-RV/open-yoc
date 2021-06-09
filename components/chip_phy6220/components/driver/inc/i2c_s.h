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


#ifndef _IIC_SLAVE_H
#define _IIC_SLAVE_H

#define I2CS_RX_MAX_SIZE  64
#define I2CS_TX_MAX_SIZE  64

//i2cs working state
enum {
    I2CSST_IDLE = 0,
    I2CSST_XMITING
};

typedef enum {
    I2CS_0 = 1,
    I2CS_1,
} i2cs_channel_t;

typedef enum {
    I2CS_MODE_REG_8BIT = 1,
    I2CS_MODE_REG_16BIT,
    I2CS_MODE_RAW
} i2cs_mode_t;

enum {
    I2CS_EVT_REG_REQ_READ = 1, //register mode read, master read request
    I2CS_EVT_REG_REQ_READ_CMPL, //register mode read, read completed
    I2CS_EVT_REG_RECV,          //register mode write, recieved data

    I2CS_ET_RAW_RECV,   //master write data to slave
    I2CS_ET_RAW_TX_CMPL,  //master read data from slave completed
};//event type


typedef struct {
    uint8_t   type;
    uint8_t   reg_u8;
    uint16_t  reg_u16;
    uint8_t  *dat;
} i2cs_evt_t;

typedef void (*i2cs_hdl_t)(i2cs_evt_t *pev);
void __attribute__((weak)) hal_I2C0_IRQHandler(void);
void __attribute__((weak)) hal_I2C1_IRQHandler(void);
int i2cs_init(
    i2cs_channel_t  ch_id,
    i2cs_mode_t     mode,
    uint8_t         saddr,  //slave address
    gpio_pin_e      cs,  //if need not cs, choose GPIO_DUMMY_PIN
    gpio_pin_e      sda,
    gpio_pin_e      scl,
    i2cs_hdl_t      evt_handler);

#endif

