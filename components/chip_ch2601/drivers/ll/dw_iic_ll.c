/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <dw_iic_ll.h>

void dw_iic_set_target_address(dw_iic_regs_t *iic_base, uint32_t address)
{
    uint32_t iic_status;
    iic_status = dw_iic_get_iic_status(iic_base);
    dw_iic_disable(iic_base);
    uint32_t target_address = iic_base->IC_TAR;
    target_address &= (DW_IIC_TAR_MASTER_ADDR_MODE | DW_IIC_TAR_SPECIAL | DW_IIC_TAR_GC_OR_START);
    target_address |= address;
    iic_base->IC_TAR = target_address; /* this register can be written only when the I2C is disabled*/

    if (iic_status == DW_IIC_EN) {
        dw_iic_enable(iic_base);
    }
}

void dw_iic_set_transfer_speed_high(dw_iic_regs_t *iic_base)
{
    uint32_t speed_config = iic_base->IC_CON;
    speed_config &= ~(DW_IIC_CON_SPEEDL_EN | DW_IIC_CON_SPEEDH_EN);
    speed_config |= DW_IIC_CON_SPEEDL_EN | DW_IIC_CON_SPEEDH_EN;
    iic_base->IC_CON = speed_config;
}

void dw_iic_set_transfer_speed_fast(dw_iic_regs_t *iic_base)
{
    uint32_t speed_config = iic_base->IC_CON;
    speed_config &= ~(DW_IIC_CON_SPEEDL_EN | DW_IIC_CON_SPEEDH_EN);
    speed_config |= DW_IIC_CON_SPEEDH_EN;
    iic_base->IC_CON = speed_config;
}

void dw_iic_set_transfer_speed_standard(dw_iic_regs_t *iic_base)
{
    uint32_t speed_config = iic_base->IC_CON;
    speed_config &= ~(DW_IIC_CON_SPEEDL_EN | DW_IIC_CON_SPEEDH_EN);
    speed_config |= DW_IIC_CON_SPEEDL_EN;
    iic_base->IC_CON = speed_config;
}

void dw_iic_set_slave_mode(dw_iic_regs_t *iic_base)
{
    uint32_t iic_status;
    iic_status = dw_iic_get_iic_status(iic_base);
    dw_iic_disable(iic_base);
    uint32_t val = DW_IIC_CON_MASTER_EN | DW_IIC_CON_SLAVE_EN;
    iic_base->IC_CON &= ~val;  ///< set 0 to disabled master mode; set 0 to enabled slave mode

    if (iic_status == DW_IIC_EN) {
        dw_iic_enable(iic_base);
    }
}

void dw_iic_set_master_mode(dw_iic_regs_t *iic_base)
{
    uint32_t iic_status;
    iic_status = dw_iic_get_iic_status(iic_base);
    dw_iic_disable(iic_base);
    uint32_t val = DW_IIC_CON_MASTER_EN | DW_IIC_CON_SLAVE_EN; ///< set 1 to enabled master mode; set 1 to disabled slave mode
    iic_base->IC_CON |= val;

    if (iic_status == DW_IIC_EN) {
        dw_iic_enable(iic_base);
    }
}

uint32_t dw_iic_find_max_prime_num(uint32_t num)
{
    uint32_t i = 0U;

    for (i = 8U; i > 0U; i --) {
        if (!num) {
            i = 1U;
            break;
        }

        if (!(num % i)) {
            break;
        }

    }

    return i;
}
