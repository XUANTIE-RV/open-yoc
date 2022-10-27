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
* @file   gpio.c
* @brief  Contains all functions support for gpio and iomux driver
* @version  0.0
* @date   19. Oct. 2017
* @author qing.han
*

*
*******************************************************************************/
#include "types.h"
#include "string.h"
#include "mcu.h"
#include "clock.h"
#include "gpio.h"
#include "pwrmgr.h"
#include "error.h"
#include "jump_function.h"
//#include "log.h"


extern uint32_t s_gpio_wakeup_src_group1, s_gpio_wakeup_src_group2;


enum {
    GPIO_PIN_ASSI_NONE = 0,
    GPIO_PIN_ASSI_OUT,
    GPIO_PIN_ASSI_IN,
};

typedef struct {
    bool          enable;
    uint8_t       pin_state;
    gpioin_Hdl_t  posedgeHdl;
    gpioin_Hdl_t  negedgeHdl;

} gpioin_Ctx_t;

typedef struct {
    bool          state;
    uint8_t       pin_assignments[NUMBER_OF_PINS];
    gpioin_Ctx_t  irq_ctx[NUMBER_OF_PINS];

} gpio_Ctx_t;

typedef struct {
    uint8_t    reg_i;
    uint8_t    bit_h;
    uint8_t    bit_l;

} PULL_TypeDef;

static gpio_Ctx_t m_gpioCtx = {
    .state = FALSE,
    .pin_assignments = {0,},
};

const uint8_t c_gpio_index[GPIO_NUM] = {0, 1, 2, 3, 7, 9, 10, 11, 14, 15, 16, 17, 18, 20, 23, 24, 25, 26, 27, 31, 32, 33, 34};

const PULL_TypeDef c_gpio_pull[GPIO_NUM] = {
    {0, 2, 1}, //p0
    {0, 5, 4}, //p1
    {0, 8, 7}, //p2
    {0, 11, 10}, //p3
    {0, 23, 22}, //p7
    {0, 29, 28}, //p9
    {1, 2, 1}, //p10
    {1, 5, 4}, //p11
    {1, 14, 13}, //p14
    {1, 17, 16}, //p15
    {1, 20, 19}, //p16
    {1, 23, 22}, //p17
    {1, 26, 25}, //p18
    {2, 2, 1}, //p20
    {2, 11, 10}, //p23
    {2, 14, 13}, //p24
    {2, 17, 16}, //p25
    {2, 20, 19}, //p26
    {2, 23, 22}, //p27
    {3, 5, 4}, //p31
    {3, 8, 7}, //p32
    {3, 11, 10}, //p33
    {3, 14, 13}, //p34
};

const signed char retention_reg[GPIO_NUM][2] = {
    {0, 13}, //p0
    {0, 14}, //p1
    {0, 16}, //p2
    {0, 17}, //p3
    {0, 19}, //p7
    {0, 20}, //p9
    {1, 7}, //p10
    {1, 8}, //p11
    {1, 10}, //p14
    {1, 11}, //p15
    {1, 28}, //p16
    {1, 29}, //p17
    {2, 4}, //p18
    {2, 5}, //p20
    {2, 7}, //p23
    {2, 8}, //p24
    {2, 25}, //p25
    {2, 26}, //p26
    {2, 28}, //p27
    {2, 29}, //p31
    {3, 1}, //p32
    {3, 2}, //p33
    {3, 23}, //p34
};

static int phy_gpio_interrupt_disable(gpio_pin_e pin)
{
    subWriteReg(&(AP_GPIO->intmask), pin, pin, 1);
    subWriteReg(&(AP_GPIO->inten), pin, pin, 0);

    return PPlus_SUCCESS;
}

void phy_gpio_write(gpio_pin_e pin, uint8_t en)
{
    if (en) {
        AP_GPIO->swporta_dr |= BIT(pin);
    } else {
        AP_GPIO->swporta_dr &= ~BIT(pin);
    }

    phy_gpio_pin_init(pin, GPIO_OUTPUT_1);
}

void phy_gpio_fast_write(gpio_pin_e pin, uint8_t en)
{
    if (en) {
        AP_GPIO->swporta_dr |= BIT(pin);
    } else {
        AP_GPIO->swporta_dr &= ~BIT(pin);
    }
}

bool phy_gpio_read(gpio_pin_e pin)
{
    uint32_t r;

    if (AP_GPIO->swporta_ddr & BIT(pin)) {
        r = AP_GPIO->swporta_dr;
    } else {
        r = AP_GPIO->ext_porta;
    }

    return (int)((r >> pin) & 1);
}

void phy_gpio_fmux(gpio_pin_e pin, bit_action_e value)
{
    if (value) {
//        if((pin == P2) || (pin == P3))
//            hal_gpio_pin2pin3_control(pin,1);
        AP_IOMUX->full_mux0_en |= BIT(pin);
    }

    else {
        AP_IOMUX->full_mux0_en &= ~BIT(pin);
    }
}

void phy_gpio_fmux_set(gpio_pin_e pin, gpio_fmux_e type)
{
    uint8_t h = 0, l = 0;
    uint32_t reg_index;
    uint32_t bit_index;

    reg_index = pin / 4;
    bit_index = pin % 4;
    l = 8 * bit_index;
    h = l + 5;
    subWriteReg(&(AP_IOMUX->gpio_sel[reg_index]), h, l, type);
    phy_gpio_fmux(pin, Bit_ENABLE);
}

void phy_gpio_pin_init(gpio_pin_e pin, gpio_dir_t type)
{
    phy_gpio_fmux(pin, Bit_DISABLE);

    if ((pin == P2) || (pin == P3)) {
        phy_gpio_pin2pin3_control(pin, 1);
    }

    if (type == GPIO_OUTPUT_1) {
        AP_GPIO->swporta_ddr |= BIT(pin);
    } else {
        AP_GPIO->swporta_ddr &= ~BIT(pin);
    }
}

static void phy_gpio_wakeup_control(gpio_pin_e pin, bit_action_e value)
{
    if (pin < P32) {
        if (value) {
            AP_AON->REG_S9 |= BIT(c_gpio_index[pin]);
        } else {
            AP_AON->REG_S9 &= ~BIT(c_gpio_index[pin]);
        }
    } else {
        if (value) {
            AP_AON->REG_S10 |= BIT(c_gpio_index[pin] - 32);
        } else {
            AP_AON->REG_S10 &= ~BIT(c_gpio_index[pin] - 32);
        }
    }
}

void phy_gpio_ds_control(gpio_pin_e pin, bit_action_e value)
{
    if (value) {
        AP_IOMUX->pad_ps0 |= BIT(pin);
    } else {
        AP_IOMUX->pad_ps0 &= ~BIT(pin);
    }
}

static uint32_t c_gpio_retention = 0x00000000;
int phy_gpioretention_unregister(gpio_pin_e pin)
{
    if (AP_GPIO->swporta_ddr & BIT(pin)) {
        c_gpio_retention &= ~BIT(pin);
        return true;
    }
    else {
        return false;
    }
}

int phy_gpioin_unregister(gpio_pin_e pin)
{
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);

    if (pin > (NUMBER_OF_PINS - 1)) {
        return PPlus_ERR_NOT_SUPPORTED;
    }

    phy_gpioin_disable(pin);
    p_irq_ctx[pin].negedgeHdl = NULL;
    p_irq_ctx[pin].posedgeHdl = NULL;

    return PPlus_SUCCESS;
}

int phy_gpio_cfg_analog_io(gpio_pin_e pin, bit_action_e value)
{
    if ((pin < P11) || (pin > P25)) {
        return PPlus_ERR_INVALID_PARAM;
    }

    if (value) {
        phy_gpio_pull_set(pin, GPIO_FLOATING);
        AP_IOMUX->Analog_IO_en |= BIT(pin - P11);
    } else {
        AP_IOMUX->Analog_IO_en &= ~BIT(pin - P11);
    }

    return PPlus_SUCCESS;
}


void phy_gpio_pull_set(gpio_pin_e pin, gpio_pupd_e type)
{
    uint8_t i = c_gpio_pull[pin].reg_i;
    uint8_t h = c_gpio_pull[pin].bit_h;
    uint8_t l = c_gpio_pull[pin].bit_l;

    if (pin < P31) {
        subWriteReg(&(AP_AON->IOCTL[i]), h, l, type);
    } else {
        subWriteReg(&(AP_AON->PMCTL0), h, l, type);
    }
}

void phy_gpio_wakeup_set(gpio_pin_e pin, gpio_polarity_e type)
{
    uint8_t i = c_gpio_pull[pin].reg_i;
    uint8_t p = c_gpio_pull[pin].bit_l - 1;
#if 0
    if (m_gpioCtx.pin_assignments[pin] != GPIO_PIN_ASSI_IN) {
        return;
    }
#endif
    AP_GPIO->inttype_level |= BIT(pin);//edge sensitive

    if (pin < P31) {
        if (POL_FALLING == type) {
            AP_AON->IOCTL[i] |= BIT(p);
        } else {
            AP_AON->IOCTL[i] &= ~BIT(p);
        }
    } else {
        if (POL_FALLING == type) {
            AP_AON->PMCTL0 |= BIT(p);
        } else {
            AP_AON->PMCTL0 &= ~BIT(p);
        }
    }

    phy_gpio_wakeup_control(pin, Bit_ENABLE); //enable wakeup function
}

void phy_gpio_pin2pin3_control(gpio_pin_e pin, uint8_t en)//0:sw,1:other func
{
    if (en) {
        AP_IOMUX->gpio_pad_en |= BIT(pin - 2);
    } else {
        AP_IOMUX->gpio_pad_en &= ~BIT(pin - 2);
    }
}


void phy_gpio_retention_enable(gpio_pin_e pin, uint8_t en)
{
    if (en) {
        if ((pin == P32) || (pin == P33) || (pin == P34)) {
            AP_AON->PMCTL0 |= BIT(retention_reg[pin][1]);
        } else {
            AP_AON->IOCTL[retention_reg[pin][0]] |= BIT(retention_reg[pin][1]);
        }
    } else {
        if ((pin == P32) || (pin == P33) || (pin == P34)) {
            AP_AON->PMCTL0 &= ~BIT(retention_reg[pin][1]);
        } else {
            AP_AON->IOCTL[retention_reg[pin][0]] &= ~BIT(retention_reg[pin][1]);
        }
    }
}

int phy_gpioin_disable(gpio_pin_e pin)
{
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);

    if (pin > (NUMBER_OF_PINS - 1)) {
        return PPlus_ERR_NOT_SUPPORTED;
    }

    p_irq_ctx[pin].enable = FALSE;
    m_gpioCtx.pin_assignments[pin] = GPIO_PIN_ASSI_NONE;
    phy_gpio_pin_init(pin, GPIO_INPUT_1);

    return phy_gpio_interrupt_disable(pin);
}

static int phy_gpio_interrupt_enable(gpio_pin_e pin, gpio_polarity_e type)
{
    uint32_t gpio_tmp;

    if (pin >= NUMBER_OF_PINS) {
        return PPlus_ERR_NOT_SUPPORTED;
    }

    gpio_tmp = AP_GPIO->inttype_level;
    gpio_tmp |= (1 << pin); //edge sensitive
    AP_GPIO->inttype_level = gpio_tmp;

    gpio_tmp = AP_GPIO->intmask;
    gpio_tmp &= ~(1 << pin); //unmask interrupt
    AP_GPIO->intmask = gpio_tmp;

    gpio_tmp = AP_GPIO->int_polarity;

    if (type == POL_RISING) {
        gpio_tmp |= (1 << pin);
    } else {
        gpio_tmp &= ~(1 << pin);
    }

    AP_GPIO->int_polarity = gpio_tmp;

    gpio_tmp = AP_GPIO->inten;
    gpio_tmp |= (1 << pin); //enable interrupt
    AP_GPIO->inten = gpio_tmp;

    return PPlus_SUCCESS;
}

static void phy_gpioin_event_pin(gpio_pin_e pin, gpio_polarity_e type)
{
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);

    if (p_irq_ctx[pin].posedgeHdl && (type == POL_RISING)) {
        p_irq_ctx[pin].posedgeHdl(pin, POL_RISING);//LOG("POS\n");
    } else if (p_irq_ctx[pin].negedgeHdl && (type == POL_FALLING)) {
        p_irq_ctx[pin].negedgeHdl(pin, POL_FALLING); //LOG("NEG\n");
    }
}

static void phy_gpioin_wakeup_trigger(gpio_pin_e pin)
{
    uint8_t pin_state = (uint8_t)phy_gpio_read(pin);
    gpio_polarity_e type = pin_state ? POL_RISING  : POL_FALLING;

    if (m_gpioCtx.irq_ctx[pin].pin_state != pin_state) {
        phy_gpioin_event_pin(pin, type);
    }
}

static void phy_gpioin_event(uint32 int_status, uint32 polarity)
{
    int i;
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);
//    LOG("GI:%x,%x\n",int_status,polarity);

    for (i = 0; i < NUMBER_OF_PINS; i++) {
        if (int_status & (1ul << i)) {
            gpio_polarity_e type = (polarity & BIT(i)) ? POL_RISING  : POL_FALLING;
            phy_gpioin_event_pin((gpio_pin_e)i, type);

            //reconfig interrupt
            if (p_irq_ctx[i].posedgeHdl && p_irq_ctx[i].negedgeHdl) { //both raise and fall
                type = (type == POL_RISING) ? POL_FALLING : POL_RISING ;
                phy_gpio_interrupt_enable((gpio_pin_e)i, type);
            } else if (p_irq_ctx[i].posedgeHdl) { //raise
                phy_gpio_interrupt_enable((gpio_pin_e)i, POL_RISING);
            } else if (p_irq_ctx[i].negedgeHdl) { //fall
                phy_gpio_interrupt_enable((gpio_pin_e)i, POL_FALLING);
            }
        }
    }
}

static void phy_gpio_sleep_handler(void)
{
    int i;
    gpio_polarity_e pol;

    for (i = 0; i < NUMBER_OF_PINS; i++) {
        //config wakeup
        if (m_gpioCtx.pin_assignments[i] == GPIO_PIN_ASSI_IN) {
            pol = phy_gpio_read((gpio_pin_e)i) ? POL_FALLING : POL_RISING ;
            phy_gpio_wakeup_set((gpio_pin_e)i, pol);
            m_gpioCtx.irq_ctx[i].pin_state = phy_gpio_read((gpio_pin_e)i);
        }
    }
}

static void phy_gpio_wakeup_handler(void)
{
    int i;
    NVIC_SetPriority(GPIO_IRQn, IRQ_PRIO_HAL);
    NVIC_EnableIRQ(GPIO_IRQn);

    for (i = 0; i < NUMBER_OF_PINS; i++) {
        if ((i == 2) || (i == 3)) {
            phy_gpio_pin2pin3_control((gpio_pin_e)i, 1);
        }

        if (m_gpioCtx.irq_ctx[i].enable) {
            phy_gpioin_enable((gpio_pin_e)i); //resume gpio irq
            phy_gpioin_wakeup_trigger((gpio_pin_e)i);//trigger gpio irq manually
        }
    }
}

void __attribute__((used)) phy_GPIO_IRQHandler(void)
{
    uint32 polarity = AP_GPIO->int_polarity;
    uint32 st = AP_GPIO->int_status;

    AP_GPIO->porta_eoi = st;//clear interrupt
    phy_gpioin_event(st, polarity);
}

int phy_gpioin_enable(gpio_pin_e pin)
{
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);
    gpio_polarity_e type = POL_FALLING;
    uint32 pinVal = 0;

    if (p_irq_ctx[pin].posedgeHdl == NULL && p_irq_ctx[pin].negedgeHdl == NULL) {
        return PPlus_ERR_NOT_REGISTED;
    }

    m_gpioCtx.pin_assignments[pin] = GPIO_PIN_ASSI_IN;

    p_irq_ctx[pin].enable = TRUE;
    phy_gpio_pin_init(pin, GPIO_INPUT_1);

    //hal_gpio_pull_set(pin, PULL_DOWN); //??need disccuss
    if (p_irq_ctx[pin].posedgeHdl && p_irq_ctx[pin].negedgeHdl) { //both raise and fall
        pinVal = phy_gpio_read(pin);
        type = pinVal ? POL_FALLING : POL_RISING ;
    } else if (p_irq_ctx[pin].posedgeHdl) { //raise
        type = POL_RISING ;
    } else if (p_irq_ctx[pin].negedgeHdl) { //fall
        type = POL_FALLING;
    }

    phy_gpio_interrupt_enable(pin, type);

    return PPlus_SUCCESS;
}

int phy_gpioretention_register(gpio_pin_e pin)
{
    if (AP_GPIO->swporta_ddr & BIT(pin)) {
        c_gpio_retention |= BIT(pin);
        return true;
    }
    else {
        return false;
    }
}

__attribute__((section(".__sram.code.phy_gpioretention_prepare_sleep_action"))) void phy_gpioretention_prepare_sleep_action(void)
{
    gpio_pin_e pin=0;
    if(c_gpio_retention == 0x00000000)
        return;

    for(pin=0;pin<GPIO_NUM;pin++)
    {
        if(c_gpio_retention & BIT(pin))
        {
            phy_gpio_retention_enable(pin,true);
        }
    }
}

__attribute__((section(".__sram.code.phy_gpioretention_disable"))) void phy_gpioretention_disable(void)
{
    gpio_pin_e pin=0;

    for(pin=0;pin<GPIO_NUM;pin++)
    {
        if ((pin == P32) || (pin == P33) || (pin == P34)) {
            AP_AON->PMCTL0 &= ~BIT(retention_reg[pin][1]);
        } else {
            AP_AON->IOCTL[retention_reg[pin][0]] &= ~BIT(retention_reg[pin][1]);
        }
    }
}

__attribute__((section(".__sram.code.phy_gpioretention_prepare_wakeup_action"))) void phy_gpioretention_prepare_wakeup_action(void)
{
    gpio_pin_e pin=0;

    if(c_gpio_retention == 0x00000000)
        return;

    for(pin=0;pin<GPIO_NUM;pin++)
    {
        if(c_gpio_retention & BIT(pin))
        {
            bool pol = phy_gpio_read((gpio_pin_e)pin);
            phy_gpio_write((gpio_pin_e)pin,pol);
            phy_gpio_retention_enable(pin,false);
        }
    }
}

int phy_gpioin_register(gpio_pin_e pin, gpioin_Hdl_t posedgeHdl, gpioin_Hdl_t negedgeHdl)
{
    int ret;
    gpioin_Ctx_t *p_irq_ctx = &(m_gpioCtx.irq_ctx[0]);

    phy_gpioin_disable(pin);
    p_irq_ctx[pin].posedgeHdl = posedgeHdl;
    p_irq_ctx[pin].negedgeHdl = negedgeHdl;
    ret = phy_gpioin_enable(pin);

    JUMP_FUNCTION(V16_IRQ_HANDLER)   = (uint32_t)&phy_GPIO_IRQHandler;

    if (ret != PPlus_SUCCESS) {
        phy_gpioin_disable(pin);
    }

    return ret;
}

int phy_gpio_init(void)
{
    if (m_gpioCtx.state) {
        return PPlus_ERR_INVALID_STATE;
    }

    memset(&m_gpioCtx, 0, sizeof(m_gpioCtx));
    m_gpioCtx.state = TRUE;

    //disable all channel irq,unmask all channel
    AP_GPIO->inten = 0;
    AP_GPIO->intmask = 0;

    //disable all wakeup pin
    AP_WAKEUP->io_wu_mask_31_0 = 0;
    AP_WAKEUP->io_wu_mask_34_32 = 0;

    NVIC_SetPriority(GPIO_IRQn, IRQ_PRIO_HAL);
    NVIC_EnableIRQ(GPIO_IRQn);

    hal_pwrmgr_register(MOD_GPIO, phy_gpio_sleep_handler, phy_gpio_wakeup_handler);

    return PPlus_SUCCESS;
}

void phy_gpio_debug_mux(Freq_Type_e fre, bool en)
{
    if (en) {
        AP_IOMUX->debug_mux_en |= BIT(fre);
    } else {
        AP_IOMUX->debug_mux_en &= ~BIT(fre);
    }
}
