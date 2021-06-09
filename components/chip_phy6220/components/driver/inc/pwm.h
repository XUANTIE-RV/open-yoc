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
* @file     pwm.h
* @brief    Contains all functions support for pwm driver
* @version  0.0
* @date     30. Oct. 2017
* @author   Ding
*
* Copyright(C) 2016, PhyPlus Semiconductor
* All rights reserved.
*
*******************************************************************************/
#ifndef __PWM__H__
#define __PWM__H__

#include "types.h"
#include "gpio.h"
#include <drv/common.h>

#define    PWM_ENABLE_ALL           do{\
        AP_PWM->pwmen |= BIT(0);\
        AP_PWM->pwmen |= BIT(4);\
    }while(0)
#define    PWM_DISABLE_ALL          do{\
        AP_PWM->pwmen &= ~BIT(0);\
        AP_PWM->pwmen &= ~BIT(4);\
    }while(0)
#define    PWM_ENABLE_CH_012        do{\
        AP_PWM->pwmen |= BIT(8);\
        AP_PWM->pwmen |= BIT(9);\
    }while(0)
#define    PWM_DISABLE_CH_012       do{\
        AP_PWM->pwmen &= ~BIT(8);\
        AP_PWM->pwmen &= ~BIT(9);\
    }while(0)
#define    PWM_ENABLE_CH_345        do{\
        AP_PWM->pwmen |= BIT(10);\
        AP_PWM->pwmen |= BIT(11);\
    }while(0)
#define    PWM_DISABLE_CH_345       do{\
        AP_PWM->pwmen &= ~BIT(10);\
        AP_PWM->pwmen &= ~BIT(11);\
    }while(0)
#define    PWM_ENABLE_CH_01         do{\
        AP_PWM->pwmen |= BIT(12);\
        AP_PWM->pwmen |= BIT(13);\
    }while(0)
#define    PWM_DISABLE_CH_01        do{\
        AP_PWM->pwmen &= ~BIT(12);\
        AP_PWM->pwmen &= ~BIT(13);\
    }while(0)
#define    PWM_ENABLE_CH_23         do{\
        AP_PWM->pwmen |= BIT(14);\
        AP_PWM->pwmen |= BIT(15);\
    }while(0)
#define    PWM_DISABLE_CH_23        do{\
        AP_PWM->pwmen &= ~BIT(14);\
        AP_PWM->pwmen &= ~BIT(15);\
    }while(0)
#define    PWM_ENABLE_CH_45         do{\
        AP_PWM->pwmen |= BIT(16);\
        AP_PWM->pwmen |= BIT(17);\
    }while(0)
#define    PWM_DISABLE_CH_45        do{\
        AP_PWM->pwmen &= ~BIT(16);\
        AP_PWM->pwmen &= ~BIT(17);\
    }while(0)


#define    PWM_INSTANT_LOAD_CH(n)       subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),31,31,1)
#define    PWM_NO_INSTANT_LOAD_CH(n)    subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),31,31,0)
#define    PWM_LOAD_CH(n)               subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),16,16,1)
#define    PWM_NO_LOAD_CH(n)            subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),16,16,0)
#define    PWM_SET_DIV(n,v)             subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),14,12,v)
#define    PWM_SET_MODE(n,v)            subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),8,8,v)
#define    PWM_SET_POL(n,v)             subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),4,4,v)
#define    PWM_ENABLE_CH(n)             subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),0,0,1)
#define    PWM_DISABLE_CH(n)            subWriteReg(&(AP_PWM_CTRL(n)->ctrl0),0,0,0)

#define    PWM_SET_CMP_VAL(n,v)         subWriteReg(&(AP_PWM_CTRL(n)->ctrl1),31,16,v)
#define    PWM_SET_TOP_VAL(n,v)         subWriteReg(&(AP_PWM_CTRL(n)->ctrl1),15,0,v)
#define    PWM_GET_CMP_VAL(n)           ((AP_PWM_CTRL(n)->ctrl1 & 0xFFFF0000) >> 8)
#define    PWM_GET_TOP_VAL(n)           (AP_PWM_CTRL(n)->ctrl1 & 0x0000FFFF)


/*************************************************************
*   @brief      enum variable, the number of PWM channels supported
*
*/
typedef enum {

    PWM_CH0 = 0,
    PWM_CH1 = 1,
    PWM_CH2 = 2,
    PWM_CH3 = 3,
    PWM_CH4 = 4,
    PWM_CH5 = 5

} PWMN_e;

/*************************************************************
*   @brief      enum variable used for PWM clock prescaler
*
*/
typedef enum {

    PWM_CLK_NO_DIV = 0,
    PWM_CLK_DIV_2 = 1,
    PWM_CLK_DIV_4 = 2,
    PWM_CLK_DIV_8 = 3,
    PWM_CLK_DIV_16 = 4,
    PWM_CLK_DIV_32 = 5,
    PWM_CLK_DIV_64 = 6,
    PWM_CLK_DIV_128 = 7

} PWM_CLK_DIV_e;

/*************************************************************
*   @brief      enum variable used for PWM work mode setting
*
*/
typedef enum {

    PWM_CNT_UP = 0,
    PWM_CNT_UP_AND_DOWN = 1

} PWM_CNT_MODE_e;

/*************************************************************
*   @brief      enum variable used for PWM output polarity setting
*
*/
typedef enum {

    PWM_POLARITY_RISING = 0,
    PWM_POLARITY_FALLING = 1

} PWM_POLARITY_e;


/******************************************************/
typedef void *pwm_handle_t;

pwm_handle_t csi_pwm_initialize(uint32_t idx);

void csi_pwm_uninitialize(pwm_handle_t handle);

int32_t csi_pwm_config(pwm_handle_t handle,
                       uint8_t channel,
                       uint32_t period_us,
                       uint32_t pulse_width_us);

void csi_pwm_start(pwm_handle_t handle, uint8_t channel);

void csi_pwm_stop(pwm_handle_t handle, uint8_t channel);
#endif
