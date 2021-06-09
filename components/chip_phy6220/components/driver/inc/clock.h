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
#ifndef _HAL_CLOCK_H
#define _HAL_CLOCK_H

#include "types.h"
#include "bus_dev.h"

typedef enum {
    CLK_32K_XTAL        = 0,
    CLK_32K_RCOSC       = 1,

} CLK32K_e;

typedef enum {
    XTAL_16M  = 0,
    DBL_B_32M = 1,
    DBL_32    = 2,
    DLL_32M   = 3,

} ClkSrc_e;

typedef enum  _SYSCLK_SEL {
    SYS_CLK_RC_32M      = 0,
    SYS_CLK_DLL_32M     = 1,
    SYS_CLK_XTAL_16M    = 2,
    SYS_CLK_DLL_48M     = 3,
    SYS_CLK_DLL_64M     = 4,
    SYS_CLK_DLL_96M     = 5,
    SYS_CLK_8M          = 6,
    SYS_CLK_4M          = 7,
    SYS_CLK_NUM         = 8,

} sysclk_t;

typedef enum {
    HCLK_CHANGE = 0,
    AP_CLK_CHANGE = 1,
    CP_CLK_CHANGE = 2,

} clk_update_Type_t;

typedef struct _clk_Evt_t {
    uint8_t   flag;

} clk_Evt_t;

typedef void (*clk_Hdl_t)(clk_Evt_t *pev);

typedef struct _clk_Contex_t {
    bool      enable;
    clk_Hdl_t evt_handler;
} clk_Ctx_t;


#define   CLAER_RTC_COUNT   AP_AON->RTCCTL |= BIT(1)
#define   RUN_RTC           AP_AON->RTCCTL |= BIT(0)
#define   STOP_RTC          AP_AON->RTCCTL &= ~BIT(0)

#define hal_system_init clk_init
extern volatile uint32_t  g_hclk;
#define  clk_get_hclk()   g_hclk
uint32_t clk_get_pclk(void);

void hal_clk_gate_enable(MODULE_e module);
void hal_clk_gate_disable(MODULE_e module);
int hal_clk_gate_get(MODULE_e module);
void hal_clk_get_modules_state(uint32_t *buff);
void hal_clk_reset(MODULE_e module);
void hal_clk_rf_config(ClkSrc_e sel);
void hal_clk_rxadc_config(ClkSrc_e sel);

bool hal_clk_set_pclk(uint32_t div);
int hal_clk_init(sysclk_t hclk_sel, clk_Hdl_t evt_handler);
void hal_rtc_clock_config(CLK32K_e clk32Mode);

uint32_t hal_systick(void);
uint32_t hal_ms_intv(uint32_t tick);

extern uint32_t rtc_get_counter(void);
void WaitMs(uint32_t msecond);

/**************************************************************************************
 * @fn          hal_setMem
 *
 * @brief       This function process for set some memory addr with a value
 *
 * input parameters
 *
 * @param       uint8_t *buf: set memory buffer
 *              uint8_t value: memory value
 *              uint32_t length: set memory length
 *
 * output parameters
 *
 * @param       uint8_t *buf: set memory buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_setMem(uint8_t  *buf, uint8_t value, uint32_t length);

/**************************************************************************************
 * @fn          hal_cpyMem
 *
 * @brief       This function process for copying data from source addr to dest addr,once copy one byte
 *
 * input parameters
 *
 * @param       uint8_t *dst: copy destnation buffer
 *              uint8_t *src: copy source buffer
 *              uint32_t length: copy length
 *
 * output parameters
 *
 * @param       uint8_t *dst: copy destnation buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_cpyMem(uint8_t *dst, uint8_t *src, uint32_t length);

/**************************************************************************************
 * @fn          hal_cpyMem32
 *
 * @brief       This function process for copying data from source addr to dest addr,once copy 4 bytes
 *
 * input parameters
 *
 * @param       uint32_t *dst: copy destnation buffer
 *              uint32_t *src: copy source buffer
 *              uint32_t length: copy length
 *
 * output parameters
 *
 * @param       uint32_t *dst: copy destnation buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_cpyMem32(uint32_t *dst, uint32_t *src, uint32_t length);

/**************************************************************************************
 * @fn          hal_my_strcmp
 *
 * @brief       This function process for compare two strings, return  1 means same, 0 means different
 *
 * input parameters
 *
 * @param       const uint8_t *str: the first string
 *              const uint8_t *ptr: the second string
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      1:the same
 *              0:the different
 **************************************************************************************/
int hal_my_strcmp(const uint8_t *str,  const uint8_t *ptr);

/**************************************************************************************
 * @fn          hal_copy_bin_from_flash_to_sram
 *
 * @brief       This function process for copy bin from flash to sram
 *
 * input parameters
 *
 * @param       int toAddr: destnation address
 *              int fromAddr: source address
 *              int length: copy length
 *
 * output parameters
 *
 * @param       (uint8_t *) toAddr: destnation buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_copy_bin_from_flash_to_sram(int toAddr, int fromAddr, int length);

/**************************************************************************************
 * @fn          hal_my_sizeof
 *
 * @brief       This function process for calculate the string length
 *
 * input parameters
 *
 * @param       const uint8_t *str: the source string
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      the string length(int)
 **************************************************************************************/
int hal_my_sizeof(const uint8_t *str);

/**************************************************************************************
 * @fn          hal_my_strlen
 *
 * @brief       This function process for calculate the string length,PS:the char[] must give the '\0'
 *
 * input parameters
 *
 * @param       const uint8_t *str: the source string
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      the string length(int)
 **************************************************************************************/
int hal_my_strlen(const uint8_t *str);

/**************************************************************************************
 * @fn          hal_convert_hex_to_char
 *
 * @brief       This function process for convert a hex data to ASCII code type,include the 0x symbol
 *
 * input parameters
 *
 * @param       unsigned char *ch: the char string buffer
 *              unsigned int data: the source hex data
 *
 * output parameters
 *
 * @param       unsigned char *ch: the char string buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_convert_hex_to_char(unsigned char *ch,  unsigned int data);

/**************************************************************************************
 * @fn          hal_convert_hex_to_char_wo_x
 *
 * @brief       This function process for convert a hex data to ASCII code type,without the 0x symbol
 *
 * input parameters
 *
 * @param       unsigned char *ch: the char string buffer
 *              unsigned int data: the source hex data
 *
 * output parameters
 *
 * @param       unsigned char *ch: the char string buffer
 *
 * @return      None.
 **************************************************************************************/
void hal_convert_hex_to_char_wo_x(unsigned char *ch,  unsigned int data);

/**************************************************************************************
 * @fn          hal_convert_char_to_hex
 *
 * @brief       This function process for convert a data from ASCII code to hex type
 *
 * input parameters
 *
 * @param       const unsigned char *ch: the source char string
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hex data
 **************************************************************************************/
uint32_t hal_convert_char_to_hex(const unsigned char *ch);

/**************************************************************************************
 * @fn          hal_convert_char_to_dec
 *
 * @brief       This function process for convert a data from ASCII code to decimal type
 *
 * input parameters
 *
 * @param       const unsigned char *ch: the source char string
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      decimal data
 **************************************************************************************/
uint32_t hal_convert_char_to_dec(const unsigned char *ch);

/**************************************************************************************
 * @fn          hal_is_an_valid_number
 *
 * @brief       This function process for judge if a char is hex number or not, return  1 means yes, 0 means no
 *
 * input parameters
 *
 * @param       uint8_t ch: the source data
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      1: yes
 *              0: no
 **************************************************************************************/
int hal_is_an_valid_number(uint8_t ch);

/**************************************************************************************
 * @fn          clock_time_rtc
 *
 * @brief       This function process for return rtc count
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      RTC count
 **************************************************************************************/
static inline uint32_t clock_time_rtc(void)
{
    return (*(volatile unsigned int *)0x4000f028) & 0xffffff;
}

extern uint32_t clock_time_rtc(void);

/**************************************************************************************
 * @fn          WaitMs
 *
 * @brief       This function process for wait program msecond,use RTC
 *
 * input parameters
 *
 * @param       uint32_t msecond: the msecond value
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 **************************************************************************************/
void WaitMs(uint32_t msecond);

#define WR_REG(a,b)    *(volatile unsigned int *)a=b
#define RD_REG(a)      *(volatile unsigned int *)a
#define ADD_IDX(a,b)   (a==b)? a=0:a++;

//below is system initial defination
#define    ENABLE_XTAL_TRIGGER   *(volatile unsigned int *)0x4000f040 |= BIT(16)
#define    ENABLE_XTAL_OUTPUT    *(volatile unsigned int *)0x4000f040 |= BIT(18)
#define    ENABLE_DLL            *(volatile unsigned int *)0x4000f044 |= BIT(7)
#define    DLL32M_CLOCK_ENABLE   *(volatile unsigned int *)0x4000f044 |= BIT(13)
#define    DLLn_CLOCK_ENABLE(n)  *(volatile unsigned int *)0x4000f044 |= BIT(n+11)
#define    DBLE_CLOCK_DISABLE    *(volatile unsigned int *)0x4000f044 &= ~BIT(21)
#define    CLK_1P28M_ENABLE      *(volatile unsigned int *)0x4000f03c |= BIT(6)       //the source is 32M dll
#define    HCLK_DIV2_ENABLE      *(volatile unsigned int *)0x4000f03c |= BIT(15)      //use carefully
#define    PCLK_DIV_ENABLE       *(volatile unsigned int *)0x4000001c & 0x02



enum  LOWCLK_SEL {
    RC_32K,
    XTAL_32K
};

/**************************************************************************************
 * @fn          wdt_clk_sel
 *
 * @brief       This function process for low clock select; 0----RC 32k   1----XTAL 32k
 *
 * input parameters
 *
 * @param       enum LOWCLK_SEL clk:  0----RC 32k   1----XTAL 32k
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 **************************************************************************************/
//static inline void wdt_clk_sel(enum LOWCLK_SEL clk){
//  if(clk){
//      BM_SET((volatile unsigned int  *)0x4000f03c,BIT(16));  //use xtal 32k
//  }else{
//      BM_CLR((volatile unsigned int  *)0x4000f03c,BIT(16));  //use rc 32k
//  }
//}

/**************************************************************************************
 * @fn          hal_system_init
 *
 * @brief       This function process for system initial,you can select diff source,such as RC_32M XTAL_16M and so on
 *
 * input parameters
 *
 * @param       uint8_t h_system_clk_sel: system clock select  SYS_CLK_RC_32M rc32M
 *                                                                                                                                           SYS_CLK_DLL_32M dll32M
 *                                                                                                                                           SYS_CLK_XTAL_16M xtal16M
 *                                                                                                                                           SYS_CLK_DLL_48M dll48M
 *                                                                                                                                         SYS_CLK_DLL_64M dll64M
 *                                                                                                                                       SYS_CLK_DLL_96M dll96M
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 **************************************************************************************/
void hal_system_init(uint8_t h_system_clk_sel);

void WaitUs(uint32_t wtTime);

uint8 char_array_cmp(uint8 *s1, uint8 *s2, uint8 len);

void hal_system_soft_reset(void);

#endif

