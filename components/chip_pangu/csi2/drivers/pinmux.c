/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include "soc.h"

#define YUNVOIC_PADIO_SEL0         (WJ_PADREG_BASE + 0x00U)
#define YUNVOIC_PADIO_SEL1         (WJ_PADREG_BASE + 0x04U)
#define YUNVOIC_PADIO_AFSEL_0L     (WJ_PADREG_BASE + 0x08U)
#define YUNVOIC_PADIO_AFSEL_0H     (WJ_PADREG_BASE + 0x0CU)
#define YUNVOIC_PADIO_AFSEL_1L     (WJ_PADREG_BASE + 0x10U)
#define YUNVOIC_PADIO_AFSEL_1H     (WJ_PADREG_BASE + 0x14U)
#define YUNVOIC_PADIO_IO_PA_PE     (WJ_PADREG_BASE + 0x18U)
#define YUNVOIC_PADIO_IO_PB_PE     (WJ_PADREG_BASE + 0x1CU)
#define YUNVOIC_PADIO_IO_MOD_SEL_PE     (WJ_PADREG_BASE + 0x20U)
#define YUNVOIC_PADIO_IO_FMC_DATA_PE    (WJ_PADREG_BASE + 0x24U)
#define YUNVOIC_PADIO_IO_MCURST_PE      (WJ_PADREG_BASE + 0x28U)
#define YUNVOIC_PADIO_IO_PA_DS     (WJ_PADREG_BASE + 0x2cU)
#define YUNVOIC_PADIO_IO_PB_DS     (WJ_PADREG_BASE + 0x30U)
#define YUNVOIC_PADIO_IO_FMC_ADDR_DS (WJ_PADREG_BASE + 0x34U)
#define YUNVOIC_PADIO_IO_FMC_DATA_DS (WJ_PADREG_BASE + 0x38U)
#define YUNVOIC_PADIO_IO_MEM_DS4     (WJ_PADREG_BASE + 0x3cU)

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))

/*******************************************************************************
 * function: ioreuse_inital
 *
 * description:
 *   initial hobbit_pinmux
 *******************************************************************************/

void yunvoice_initial(void)
{
    ;
}

csi_error_t csi_pin_set_mux(pin_name_t pin_name, pin_func_t pin_func)
{
    uint32_t val = 0U;
    uint32_t reg_val = 0U;
    uint8_t offset;
    csi_error_t ret = CSI_OK;

    do{
        if ((uint8_t)pin_func > 3U) {      /* select gpio mode */
            if (pin_name < PB0) {
                offset = (uint8_t)pin_name;
                val = readl(YUNVOIC_PADIO_SEL0);
                val &= ~((uint32_t)1U << ( uint32_t)offset);
                writel(val, YUNVOIC_PADIO_SEL0);
                ret =  CSI_OK;
                break;
            } else if (pin_name >= PB0) {
                offset = (uint8_t)pin_name - 32U;
                /* gpio data source select */
                val = readl(YUNVOIC_PADIO_SEL1);
                val &= ~((uint32_t)1U << (uint32_t)offset);
                writel(val, YUNVOIC_PADIO_SEL1);
                ret = CSI_OK;
                break;
            }
        }

        if ((uint8_t)pin_name >= (uint8_t)PB0) {
            offset = (uint8_t)pin_name - 32U;

            /* gpio data source select */
            val = readl(YUNVOIC_PADIO_SEL1);
            val |= ((uint32_t)1U << (uint32_t)offset);
            writel(val, YUNVOIC_PADIO_SEL1);

            if (offset < 16U) {
                reg_val = ((uint32_t)0x3U << (offset * 2U));
                /* reuse function select */
                val = readl(YUNVOIC_PADIO_AFSEL_1L);
                val &= ~(reg_val);
                val |= ((uint32_t)pin_func << (2U * offset));
                writel(val, YUNVOIC_PADIO_AFSEL_1L);
            } else {
                offset = offset - 16U;
                reg_val = ((uint32_t)0x3U << (offset * 2U));
                /* reuse function select */
                val = readl(YUNVOIC_PADIO_AFSEL_1H);
                val &= ~(reg_val);
                val |= ((uint32_t)pin_func << (2U * offset));
                writel(val, YUNVOIC_PADIO_AFSEL_1H);
            }
            ret = CSI_OK;
            break;
        }
        if ((uint32_t)pin_name >= (uint32_t)PA16) {
            offset = (uint8_t)pin_name - 16U;

            reg_val = ((uint32_t)0x3U << ((uint32_t)offset * 2U));
            /* reuse function select */
            val = readl(YUNVOIC_PADIO_AFSEL_0H);
            val &= ~(reg_val);
            val |= ((uint32_t)pin_func << (2U * offset));
            writel(val, YUNVOIC_PADIO_AFSEL_0H);
            val = readl(YUNVOIC_PADIO_SEL0);
            val |= (uint32_t)1U << (uint32_t)pin_name;
            writel(val, YUNVOIC_PADIO_SEL0);
            ret = CSI_OK;
            break;
        }

        offset = (uint8_t)pin_name;
        reg_val = ((uint32_t)0x3U << (offset * 2U));
        /* reuse function select */
        val = readl(YUNVOIC_PADIO_AFSEL_0L);
        val &= ~(reg_val);
        val |= ((uint32_t)pin_func << (2U * offset));
        writel(val, YUNVOIC_PADIO_AFSEL_0L);
        val = readl(YUNVOIC_PADIO_SEL0);
        val |= (uint32_t)1U << (uint32_t)offset;
        writel(val, YUNVOIC_PADIO_SEL0);
    }while(0);
    return ret;
}

static csi_gpio_mode_t pin_support_pull_mode(pin_name_t pin_name)
{
    csi_gpio_mode_t ret = GPIO_MODE_PULLNONE;

    if (pin_name <= PA31) {
        if (((pin_name >= PA22) && (pin_name <= PA28)) || ((pin_name >= PA3) && (pin_name <= PA20)) || (pin_name == PA1) ) {
            ret = GPIO_MODE_PULLUP;
        } else {
            ret = GPIO_MODE_PULLDOWN;
        }
    } else if (pin_name <= PB31) {
        if ((pin_name == PB25) || (pin_name == PB1) ) {
            ret = GPIO_MODE_PULLUP;
        } else {
            ret = GPIO_MODE_PULLDOWN;
        }
    }

    return ret;
}


/**
  \brief       set pin mode
  \param[in]   pin_name pin name, defined in soc.h.
  \param[in]   mode     push/pull mode
  \return      error code
*/
csi_error_t csi_pin_mode(pin_name_t pin_name, csi_gpio_mode_t mode)
{
    uint32_t pull_reg = 0U;
    uint32_t offset = 0U;
    csi_error_t ret = CSI_OK; 

    do{
        if (pin_name <= PA31) {
            pull_reg = YUNVOIC_PADIO_IO_PA_PE;
            offset = (uint32_t)pin_name;
        } else if (pin_name <= PB31) {
            pull_reg = YUNVOIC_PADIO_IO_PB_PE;
            offset = (uint32_t)pin_name - (uint32_t)PB0;
        } else {
            ret = CSI_ERROR;
            break;
        }

        if (mode != GPIO_MODE_PULLNONE) {
            if (mode != pin_support_pull_mode(pin_name)) {
                ret = CSI_ERROR;
                break;
            }
        }

        uint32_t reg_val = readl(pull_reg);

        if (mode == GPIO_MODE_PULLNONE) {
            reg_val &= ~((uint32_t)1U << (uint32_t)offset);
        } else {
            reg_val |= (uint32_t)1U << (uint32_t)offset;
        }

        writel(reg_val, pull_reg);

        ret = CSI_OK;
    }while(0);
    return ret;
}




/**
  \brief       get the pin function.
  \param[in]   pin       refs to pin_name_e.
  \return      pin function count
*/
pin_func_t csi_pin_get_mux(pin_name_t  pin_name)
{
    uint32_t val;
    uint8_t offset;
    uint32_t reg_val;
    uint32_t reg_mux0;
    uint32_t  ret = 0U;

    if(pin_name > PB31){
        ret = (uint32_t)PIN_FUNC_GPIO;
    }
    else{
    
        /* gpio data source select */
        if (pin_name <= PA31) {
            offset = (uint8_t)pin_name;
            reg_val = YUNVOIC_PADIO_SEL0;
        } else{
            offset = (uint8_t)pin_name - (uint8_t)PB0;
            reg_val = YUNVOIC_PADIO_SEL1;
        } 
        val = readl(reg_val);
        val &= ((uint32_t)1U << (uint32_t)offset);

        if (val != ((uint32_t)1U << (uint32_t)offset )) {
             ret = (uint32_t)PIN_FUNC_GPIO;
        }else{
            if (pin_name <= PA15) {
                offset = (uint8_t)pin_name;
                reg_mux0 = YUNVOIC_PADIO_AFSEL_0L;
            } else if (pin_name <= PA31) {
                offset = (uint8_t)pin_name - (uint8_t)PA16;
                reg_mux0 = YUNVOIC_PADIO_AFSEL_0H;
            } else if (pin_name <= PB15) {
                offset = (uint8_t)pin_name - (uint8_t)PB0;
                reg_mux0 = YUNVOIC_PADIO_AFSEL_1L;
            } else{
                offset = (uint8_t)pin_name - (uint8_t)PB16;
                reg_mux0 = YUNVOIC_PADIO_AFSEL_1H;
            } 
            reg_val = 0U;
            reg_val = ((uint32_t)0x3U << (offset * 2U));
            /* reuse function select */
            val = readl(reg_mux0);
            val &= (reg_val);
            ret = val >> (2U * offset);
        }
    }
    return (pin_func_t)ret;
}

csi_error_t csi_pin_wakeup(pin_name_t pin_name, bool enable)
{
    return CSI_OK;
}

/**
  \brief       set pin drive
  \param[in]   pin_name pin name, defined in soc.h.
  \param[in]   drive    io drive
  \return      error code
*/
csi_error_t csi_pin_drive(pin_name_t pin_name, csi_pin_drive_t drive)
{
    uint32_t drive_reg = 0U;
    uint8_t offset;
    uint32_t reg_val;
    csi_error_t ret = CSI_OK;

    do{
        if (drive > PIN_DRIVE_LV1) {
            ret = CSI_UNSUPPORTED;
            break;
        }

        if (pin_name <= PA31) {
            drive_reg = YUNVOIC_PADIO_IO_PA_DS;
            offset = (uint8_t)pin_name;
        } else if (pin_name <= PB31) {
            drive_reg = YUNVOIC_PADIO_IO_PB_DS;
            offset = (uint8_t)pin_name - (uint8_t)PB0;
        } else {
             ret = CSI_ERROR;
             break;
        }

        reg_val = readl(drive_reg);

        if (drive == PIN_DRIVE_LV0) {
            reg_val &= ~((uint32_t)1U << (uint32_t)offset);
        } else {
            reg_val |= (uint32_t)1U << (uint32_t)offset;
        }
        writel(reg_val, drive_reg);
    }while(0);
    return ret;
}
/**
  \brief       set pin speed
  \param[in]   pin_name pin name, defined in soc.h.
  \param[in]   speed    io speed
  \return      error code
*/
csi_error_t csi_pin_speed(pin_name_t pin_name, csi_pin_speed_t speed)
{
    return CSI_OK;
}
