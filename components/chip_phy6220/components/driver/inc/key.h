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
/**************************************************************
 *
 *
 * Module Name: key
 * File name:   key.h
 * Brief description:
 *    key driver module
 * Author:  Eagle.Lao
 * Data:    2017-07-01
 * Revision:V0.01
****************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#include "types.h"
#include "gpio.h"

#define HAL_KEY_NUM                               2      //config key's number
#define HAL_KEY_EVENT                             0x0100 //assign short key event in your app event process

#define HAL_KEY_SUPPORT_LONG_PRESS           //if use long key,please enable it
#ifdef HAL_KEY_SUPPORT_LONG_PRESS
#define HAL_KEY_LONG_PRESS_TIME       2000    //2s
#endif

#define HAL_KEY_DEBOUNCD              20      //20ms

typedef enum {
    HAL_STATE_KEY_IDLE = 0x00,
    HAL_STATE_KEY_PRESS_DEBOUNCE = 0x01,
    HAL_STATE_KEY_PRESS = 0x02,
    HAL_STATE_KEY_RELEASE_DEBOUNCE = 0x03,
} key_state_e;

typedef enum {
    HAL_KEY_EVT_IDLE = 0x0000,
    HAL_KEY_EVT_PRESS   = 0x0002,
    HAL_KEY_EVT_RELEASE = 0x0004,
    HAL_KEY_EVT_LONG_PRESS = 0x0010,
    HAL_KEY_EVT_LONG_RELEASE = 0x0020,
} key_evt_t;

typedef enum {
    HAL_LOW_IDLE = 0x00,
    HAL_HIGH_IDLE = 0x01,
} idle_level_e;

typedef void (* key_callbank_hdl_t)(uint8_t, key_evt_t);

typedef struct gpio_key_t {
    gpio_pin_e          pin;
    key_state_e     state;
    idle_level_e        idle_level;

} gpio_key;

typedef struct gpio_internal_t {
    uint32_t    timer_tick;
    bool        in_enable;

} gpio_internal;

typedef struct key_state {
    gpio_key                        key[HAL_KEY_NUM];
    gpio_internal           temp[HAL_KEY_NUM];
    uint8_t                         task_id;
    key_callbank_hdl_t  key_callbank;

} key_contex_t;


void key_init(void);
void gpio_key_timer_handler(uint8 index);
extern key_contex_t key_state;
#endif

