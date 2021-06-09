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
* @file     kscan.h
* @brief    Contains all functions support for key scan driver
* @version  0.0
* @date     13. Nov. 2017
* @author   Ding
*
* Copyright(C) 2016, PhyPlus Semiconductor
* All rights reserved.
*
*******************************************************************************/
#ifndef __KSCAN__H__
#define __KSCAN__H__

#include "types.h"
#include "gpio.h"

#define     MULTI_KEY_NUM           6
#define     MULTI_KEY_READ_ADDR     (&(AP_KSCAN->mkc[0]))//0x400240CCUL
//#define       MULTI_KEY_READ_ADDR     0x4000d0CCUL//0x400240CCUL


const static uint8_t KSCAN_ROW_GPIO[11] = {
    P0,
    P2,
    P15,
    P25,
    P10,
    P18,
    P23,
    P32,
    P34,
    P27,
    P7,
};

const static uint8_t KSCAN_COL_GPIO[12] = {
    P1,
    P3,
    P14,
    P24,
    P9,
    P20,
    P33,
    P31,
    P26,
    P17,
    P16,
    P11,
};

#define NUM_KEY_ROWS   4
#define NUM_KEY_COLS   4
#define MAX_KEY_NUM    10
#define MAX_KEY_ROWS    (sizeof(KSCAN_ROW_GPIO)/sizeof(uint8_t))
#define MAX_KEY_COLS    (sizeof(KSCAN_COL_GPIO)/sizeof(uint8_t))

/*************************************************************
*   @brief      enum variable used for setting rows
*
*/
typedef enum {
    KEY_ROW_P00   =   0,
    KEY_ROW_P02   =   1,
    KEY_ROW_P15   =   2,
    KEY_ROW_P25   =   3,
    KEY_ROW_P10   =   4,
    KEY_ROW_P18   =   5,
    KEY_ROW_P23   =   6,
    KEY_ROW_P32   =   7,
    KEY_ROW_P34   =   8,
    KEY_ROW_P27   =   9,
    KEY_ROW_P07   =   10,

} KSCAN_ROWS_e;

/*************************************************************
*   @brief      enum variable used for setting cols
*
*/
typedef enum {
    KEY_COL_P01   =   0,
    KEY_COL_P03   =   1,
    KEY_COL_P14   =   2,
    KEY_COL_P24   =   3,
    KEY_COL_P09   =   4,
    KEY_COL_P20   =   5,
    KEY_COL_P33   =   6,
    KEY_COL_P31   =   7,
    KEY_COL_P26   =   8,
    KEY_COL_P17   =   9,
    KEY_COL_P16   =   10,
    KEY_COL_P11   =   11,

} KSCAN_COLS_e;

/*************************************************************
*   @brief      enum variable used for setting multiple key press
*
*/
typedef enum {

    NOT_IGNORE_MULTI_KEY = 0,
    IGNORE_MULTI_KEY    = 1

} KSCAN_MULTI_KEY_STATE_e;

/*************************************************************
*   @brief      enum variable used for setting whether ignore ghost key
*
*/
typedef enum {

    NOT_IGNORE_GHOST_KEY = 0,
    IGNORE_GHOST_KEY    = 1

} KSCAN_GHOST_KEY_STATE_e;

/*************************************************************
*   @brief      enum variable used for setting key press sense type
*
*/
typedef enum {

    SENCE_HIGH = 0,
    SENCE_LOW = 1

} KSCAN_POLARITY_e;

/*************************************************************
*   @brief      enum variable used for setting key press sense type
*
*/
typedef enum {

    NO_KEY_PRESS = 0x00,
    ONE_KEY_PRESS = 0x01,
    MULTI_KEY_PRESS = 0x02

} KSCAN_KEY_PRESS_STATE_e;

typedef enum {
    KEY_RELEASED = 0,
    KEY_PRESSED,
} kscan_Evt_Type_t;

typedef struct {
    uint8_t             row;
    uint8_t             col;
    kscan_Evt_Type_t    type;
} kscan_Key_t;

typedef struct kscan_Evt_t_ {
    uint8_t         num;
    kscan_Key_t    *keys;
} kscan_Evt_t;

typedef void (*kscan_Hdl_t)(kscan_Evt_t *pev);

typedef struct {
    KSCAN_GHOST_KEY_STATE_e  ghost_key_state;
    KSCAN_ROWS_e            *key_rows;
    KSCAN_COLS_e            *key_cols;
    kscan_Hdl_t              evt_handler;
    uint8_t                  interval;
} kscan_Cfg_t;



//PUBLIC FUNCTIONS
int  hal_kscan_init(kscan_Cfg_t cfg, uint8 task_id, uint16 event);
void hal_kscan_timeout_handler(void);
void __attribute__((weak)) hal_KSCAN_IRQHandler(void);

#endif
