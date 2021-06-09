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
#include "rom_sym_def.h"
#include "clock.h"
#include "gpio.h"
#include "global_config.h"
#include "error.h"

extern uint32_t hclk, pclk;
extern uint32_t osal_sys_tick;

__attribute__((section(".__sram.code"))) void hal_clk_gate_enable(MODULE_e module)
{
    if (module < MOD_CP_CPU) {
        AP_PCR->SW_CLK |= BIT(module);
    } else if (module < MOD_PCLK_CACHE) {
        AP_PCR->SW_CLK1 |= BIT(module - MOD_CP_CPU);
    } else if (module < MOD_USR0) {
        AP_PCR->CACHE_CLOCK_GATE |= BIT(module - MOD_PCLK_CACHE);
    }
}

void hal_clk_gate_disable(MODULE_e module)
{
    if (module < MOD_CP_CPU) {
        AP_PCR->SW_CLK &= ~(BIT(module));
    } else if (module < MOD_PCLK_CACHE) {
        AP_PCR->SW_CLK1 &= ~(BIT(module - MOD_CP_CPU));
    } else if (module < MOD_USR0) {
        AP_PCR->CACHE_CLOCK_GATE &= ~(BIT(module - MOD_PCLK_CACHE));
    }
}

int hal_clk_gate_get(MODULE_e module)
{
    if (module < MOD_CP_CPU) {
        return (AP_PCR->SW_CLK & BIT(module));
    } else if (module < MOD_PCLK_CACHE) {
        return (AP_PCR->SW_CLK1 & BIT(module - MOD_CP_CPU));
    }
    //else if(module < MOD_USR0)
    else {
        return (AP_PCR->CACHE_CLOCK_GATE & BIT(module - MOD_PCLK_CACHE));
    }
}

void hal_clk_get_modules_state(uint32_t *buff)
{
    *buff     = AP_PCR->SW_CLK;
    *(buff + 1) = AP_PCR->SW_CLK1;
    *(buff + 2) = AP_PCR->CACHE_CLOCK_GATE;
}

__attribute__((section(".__sram.code"))) void hal_clk_reset(MODULE_e module)
{
    if (module < MOD_CP_CPU) {
        if ((module >= MOD_TIMER5) && (module <= MOD_TIMER6)) {
            AP_PCR->SW_RESET0 &= ~BIT(5);
            AP_PCR->SW_RESET0 |= BIT(5);
        } else {
            AP_PCR->SW_RESET0 &= ~BIT(module);
            AP_PCR->SW_RESET0 |= BIT(module);
        }
    } else if (module < MOD_PCLK_CACHE) {
        if ((module >= MOD_TIMER1) && (module <= MOD_TIMER4)) {
            AP_PCR->SW_RESET2 &= ~BIT(4);
            AP_PCR->SW_RESET2 |= BIT(4);
        } else {
            AP_PCR->SW_RESET2 &= ~BIT(module - MOD_CP_CPU);
            AP_PCR->SW_RESET2 |= BIT(module - MOD_CP_CPU);
        }
    } else if (module < MOD_USR0) {
        AP_PCR->CACHE_RST &= ~BIT(1 - (module - MOD_PCLK_CACHE));
        AP_PCR->CACHE_RST |= BIT(1 - (module - MOD_PCLK_CACHE));
    }
}

//clock config and div
void hal_clk_rf_config(ClkSrc_e sel)
{
    AP_PCRM->CLKHF_CTL1 &= ~BIT(18);

    if (sel != XTAL_16M) {
        subWriteReg(&(AP_PCRM->CLKHF_CTL1), 21, 20, sel);
    }

    subWriteReg(&(AP_PCRM->CLKHF_CTL1), 23, 22, sel);
    AP_PCRM->CLKHF_CTL1 |= BIT(18);
}

void hal_clk_rxadc_config(ClkSrc_e sel)
{
    AP_PCRM->CLKHF_CTL1 &= ~BIT(19);

    if (sel != XTAL_16M) {
        subWriteReg(&(AP_PCRM->CLKHF_CTL1), 21, 20, sel);
    }

    subWriteReg(&(AP_PCRM->CLKHF_CTL1), 25, 24, sel);
    AP_PCRM->CLKHF_CTL1 |= BIT(19);
}

static clk_Ctx_t mClk_Ctx = {
    .enable = FALSE,
    .evt_handler = NULL,
};

void hal_clk_cb(clk_Evt_t *pev)
{
    switch (pev->flag) {
        case 0x07:
            //LOG("hclk,apclk,cpclk change\n");
            break;

        case AP_CLK_CHANGE:
            //LOG("apclk change\n");
            break;

        case CP_CLK_CHANGE:
            //LOG("cpclk change\n");
            break;

        default:
            //LOG("unexpected\n");
            break;
    }
}

static void hal_clk_update(void)
{
//  switch(AP_PCRM->CLKSEL & 0x07)
//  {
//      case SYS_CLK_RC_32M:
//      case SYS_CLK_DLL_32M:
//          mClk_Ctx.hclk = 32000000;
//          break;
//      case SYS_CLK_XTAL_16M:
//          mClk_Ctx.hclk = 16000000;
//          break;
//      case SYS_CLK_4M:
//          mClk_Ctx.hclk = 4000000;
//          break;
//      case SYS_CLK_8M:
//          mClk_Ctx.hclk = 8000000;
//          break;
//      case SYS_CLK_DLL_48M:
//          mClk_Ctx.hclk = 48000000;
//          break;
//      case SYS_CLK_DLL_64M:
//          mClk_Ctx.hclk = 64000000;
//          break;
//      case SYS_CLK_DLL_96M:
//          mClk_Ctx.hclk = 96000000;
//          break;
//      default:
//          break;
//  }
//  mClk_Ctx.pclk = mClk_Ctx.hclk / (((AP_PCR->APB_CLK & 0xF0) >> 4) + 1);
}

void hal_clk_set_hclk(sysclk_t hclk_sel)
{
    //
    hal_clk_update();
//  if((mClk_Ctx.evt_handler != NULL) && ((hclk_sel + temp) != 1))
//  {
//      evt.flag = 0x07;//(BIT(HCLK_CHANGE) | BIT(AP_CLK_CHANGE) | BIT(CP_CLK_CHANGE));
//      mClk_Ctx.evt_handler(&evt);
//  }
}

bool hal_clk_set_pclk(uint32_t div)
{
    uint8_t divider;
    volatile int i = 100;
    clk_Evt_t evt;

    if ((div == 0) || (div > 16)) {
        return FALSE;
    }

    evt.flag = 0;
    divider = div - 1;

    if (((AP_PCR->APB_CLK & 0xF0) >> 4) != divider) {
        subWriteReg(&(AP_PCR->APB_CLK), 7, 4, divider);
        AP_PCR->APB_CLK_UPDATE = 3;

        while (i--);
    }

    evt.flag |= BIT(CP_CLK_CHANGE);
    hal_clk_update();

    if (mClk_Ctx.evt_handler != NULL) {
        mClk_Ctx.evt_handler(&evt);
    }

    return TRUE;
}

int hal_clk_init(sysclk_t hclk_sel, clk_Hdl_t evt_handler)
{
    mClk_Ctx.enable = TRUE;
    mClk_Ctx.evt_handler = evt_handler;
    hal_clk_set_hclk(hclk_sel);
    return PPlus_SUCCESS;
}

void hal_rtc_clock_config(CLK32K_e clk32Mode)
{
    if (clk32Mode == CLK_32K_RCOSC) {
        subWriteReg(&(AP_AON->PMCTL0), 31, 27, 0x05);
        subWriteReg(&(AP_AON->PMCTL2_0), 16, 7, 0x3fb);
        subWriteReg(&(AP_AON->PMCTL2_0), 6, 6, 0x01);
        //pGlobal_config[LL_SWITCH]|=RC32_TRACKINK_ALLOW|LL_RC32K_SEL;
    } else if (clk32Mode == CLK_32K_XTAL) {
        // P16 P17 for 32K XTAL input
        phy_gpio_pull_set(P16, FLOATING);
        phy_gpio_pull_set(P17, FLOATING);

        subWriteReg(&(AP_AON->PMCTL2_0), 9, 8, 0x03); //software control 32k_clk
        subWriteReg(&(AP_AON->PMCTL2_0), 6, 6, 0x00); //disable software control

        subWriteReg(&(AP_AON->PMCTL0), 31, 27, 0x16);
        //pGlobal_config[LL_SWITCH]&=0xffffffee;
    }
    //ZQ 20200812 for rc32k wakeup
    subWriteReg(&(AP_AON->PMCTL0),28,28,0x1);//turn on 32kxtal
    subWriteReg(&(AP_AON->PMCTL1),18,17,0x0);// reduce 32kxtal bias current
}

uint32_t hal_systick(void)
{
    return osal_sys_tick;
}

uint32_t hal_ms_intv(uint32_t tick)
{
    uint32_t diff = 0;

    if (osal_sys_tick < tick) {
        diff = 0xffffffff - tick;
        diff = osal_sys_tick + diff;
    } else {
        diff = osal_sys_tick - tick;
    }

    return diff * 625 / 1000;
}


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
void WaitMs(uint32_t msecond)
{
    uint32_t now_clock_tick = 0;

    AP_AON->RTCCTL |= BIT(0);//RUN_RTC;
    now_clock_tick = rtc_get_counter();

    while ((rtc_get_counter() - now_clock_tick) < (32 * msecond)) {
        ;
    }
}

void hal_system_soft_reset(void)
{
    *(volatile uint32_t *) 0x40000010 = 0x00;
}


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
void hal_setMem(uint8_t *buf, uint8_t value, uint32_t length)
{
    while (length--) {
        *buf++ = value;
    }
}

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
void hal_cpyMem(uint8_t *dst, uint8_t *src, uint32_t length)
{
    uint8_t *p_dst = (uint8_t *) dst;
    uint8_t *p_src = (uint8_t *) src;

    while (length--) {
        *p_dst++ = *p_src++;
    }
}

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
void hal_cpyMem32(uint32_t *dst, uint32_t *src, uint32_t length)
{
    uint32_t *p1 = (uint32_t *)dst;
    uint32_t *p2 = (uint32_t *)src;

    while (length) {
        *p1++ = *p2++;

        if (length >= 4) {
            length -= 4;
        } else {
            length = 0;
        }
    }
}


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
int hal_my_strcmp(const uint8_t *str,  const uint8_t *ptr)
{
    while (*ptr != '\0') {
        if (*str != *ptr) {
            return 0;
        }

        ++str;
        ++ptr;
    }

    if (*str == '\0') {
        return 1;
    } else {
        return 0;
    }
}

uint8 char_array_cmp(uint8 *s1, uint8 *s2, uint8 len)
{
    uint8 i;

    for (i = 0; i < len; i ++)
        if (s1[i] > s2[i]) {
            return 1;
        } else if (s1[i] < s2[i]) {
            return 2;
        }

    return 0;//equal
}


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
void hal_copy_bin_from_flash_to_sram(int toAddr, int fromAddr, int length)
{
    hal_cpyMem((uint8_t *) toAddr, (uint8_t *) fromAddr, length);
}

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
int hal_my_sizeof(const uint8_t *str)
{
    int size = 0;

    while (*str != '\0') {
        size++;
        str++;
    }

    return size;
}

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
int hal_my_strlen(const uint8_t *str)
{
    int len = 0;

    while (str[len] != '\0') {
        ++len;
    }

    return len;
}


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
int hal_is_an_valid_number(uint8_t ch)
{
    if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
        return 1;
    } else {
        return 0;
    }
}



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
void hal_convert_hex_to_char(unsigned char *ch,  unsigned int data)
{
    int i;

    ch[0] = '0';
    ch[1] = 'x';

    for (i = 7; i >= 0; i--) {
        switch ((data & (0xF << i * 4)) >> i * 4) {
            case 0:
                ch[2 + 7 - i] = '0';
                break;

            case 1:
                ch[2 + 7 - i] = '1';
                break;

            case 2:
                ch[2 + 7 - i] = '2';
                break;

            case 3:
                ch[2 + 7 - i] = '3';
                break;

            case 4:
                ch[2 + 7 - i] = '4';
                break;

            case 5:
                ch[2 + 7 - i] = '5';
                break;

            case 6:
                ch[2 + 7 - i] = '6';
                break;

            case 7:
                ch[2 + 7 - i] = '7';
                break;

            case 8:
                ch[2 + 7 - i] = '8';
                break;

            case 9:
                ch[2 + 7 - i] = '9';
                break;

            case 10:
                ch[2 + 7 - i] = 'a';
                break;

            case 11:
                ch[2 + 7 - i] = 'b';
                break;

            case 12:
                ch[2 + 7 - i] = 'c';
                break;

            case 13:
                ch[2 + 7 - i] = 'd';
                break;

            case 14:
                ch[2 + 7 - i] = 'e';
                break;

            case 15:
                ch[2 + 7 - i] = 'f';
                break;

            default:
                break;
        }
    }

    ch[10] = '\0';
}


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
void hal_convert_hex_to_char_wo_x(unsigned char *ch,  unsigned int data)
{
    int i;

    for (i = 7; i >= 0; i--) {
        switch ((data & (0xF << i * 4)) >> i * 4) {
            case 0:
                ch[0 + 7 - i] = '0';
                break;

            case 1:
                ch[0 + 7 - i] = '1';
                break;

            case 2:
                ch[0 + 7 - i] = '2';
                break;

            case 3:
                ch[0 + 7 - i] = '3';
                break;

            case 4:
                ch[0 + 7 - i] = '4';
                break;

            case 5:
                ch[0 + 7 - i] = '5';
                break;

            case 6:
                ch[0 + 7 - i] = '6';
                break;

            case 7:
                ch[0 + 7 - i] = '7';
                break;

            case 8:
                ch[0 + 7 - i] = '8';
                break;

            case 9:
                ch[0 + 7 - i] = '9';
                break;

            case 10:
                ch[0 + 7 - i] = 'a';
                break;

            case 11:
                ch[0 + 7 - i] = 'b';
                break;

            case 12:
                ch[0 + 7 - i] = 'c';
                break;

            case 13:
                ch[0 + 7 - i] = 'd';
                break;

            case 14:
                ch[0 + 7 - i] = 'e';
                break;

            case 15:
                ch[0 + 7 - i] = 'f';
                break;

            default:
                break;
        }
    }

    ch[8] = '\0';
}



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
uint32_t hal_convert_char_to_hex(const unsigned char *ch)
{
    uint32_t  HexData = 0;
    const uint8_t   *p = ch;
    int       i = 0, size;

    size = hal_my_sizeof(ch);

    if (size > 8) {
        return 0;
    }

    while (i < size && hal_is_an_valid_number(*p)) {
        switch (*p++) {
            case 'f':
            case 'F':
                HexData += 15 << (size - i - 1) * 4;
                break;

            case 'e':
            case 'E':
                HexData += 14 << (size - i - 1) * 4;
                break;

            case 'd':
            case 'D':
                HexData += 13 << (size - i - 1) * 4;
                break;

            case 'c':
            case 'C':
                HexData += 12 << (size - i - 1) * 4;
                break;

            case 'b':
            case 'B':
                HexData += 11 << (size - i - 1) * 4;
                break;

            case 'a':
            case 'A':
                HexData += 10 << (size - i - 1) * 4;
                break;

            case '9':
                HexData += 9 << (size - i - 1) * 4;
                break;

            case '8':
                HexData += 8 << (size - i - 1) * 4;
                break;

            case '7':
                HexData += 7 << (size - i - 1) * 4;
                break;

            case '6':
                HexData += 6 << (size - i - 1) * 4;
                break;

            case '5':
                HexData += 5 << (size - i - 1) * 4;
                break;

            case '4':
                HexData += 4 << (size - i - 1) * 4;
                break;

            case '3':
                HexData += 3 << (size - i - 1) * 4;
                break;

            case '2':
                HexData += 2 << (size - i - 1) * 4;
                break;

            case '1':
                HexData += 1 << (size - i - 1) * 4;
                break;

            case '0':
                HexData += 0 << (size - i - 1) * 4;
                break;
        }

        i++;
    }

    return HexData;
}


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
uint32_t hal_convert_char_to_dec(const unsigned char *ch)
{
    unsigned int num = 0;

    while (*ch != '\0') {
        if (*ch >= '0' && *ch <= '9') {
            num = num * 10 + (*ch - '0');
            ch++;
        } else {
            num = 0;
            break;
        }
    }

    return num;
}

