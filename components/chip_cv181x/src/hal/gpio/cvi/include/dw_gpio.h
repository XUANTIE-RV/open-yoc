/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/
#ifndef __DW_GPIO_H__
#define __DW_GPIO_H__

#include <drv/common.h>
#include "aos/cli.h"
#include <ulog/ulog.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define CSI_DRV_DEBUG

#ifndef pr_err
#define pr_err(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#ifdef CSI_DRV_DEBUG
#ifndef pr_debug
#define pr_debug(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#ifndef pr_warn
#define pr_warn(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#else

#ifndef pr_debug
#define pr_debug(x, args...)
#endif

#ifndef pr_warn
#define pr_warn(x, args...)
#endif

#endif

#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

// enum cvi_gpio_reg_type {
// 	CVI_182X,
// 	CVI_183X,
// };

// static enum cvi_gpio_reg_type cvi_gpio_reg_type;

struct cvi_gpio_regs_t {
    uint32_t SWPORTA_DR;                    /* Offset: 0x000 (W/R)  PortA data register */
    uint32_t SWPORTA_DDR;                   /* Offset: 0x004 (W/R)  PortA data direction register */
    uint32_t INTEN;                         /* Offset: 0x030 (W/R)  Interrupt enable register */
    uint32_t INTMASK;                       /* Offset: 0x034 (W/R)  Interrupt mask register */
    uint32_t INTTYPE_LEVEL;                 /* Offset: 0x038 (W/R)  Interrupt level register */
    uint32_t INT_POLARITY;                  /* Offset: 0x03c (W/R)  Interrupt polarity register */
    uint32_t INTSTATUS;                     /* Offset: 0x040 (R)    Interrupt status of Port */
    uint32_t RAW_INTSTATUS;                  /* Offset: 0x044 (W/R)  Raw interrupt status of Port */
    uint32_t DEBOUNCE;                      /* Offset: 0x048 (W/R)  Debounce enable register */
    uint32_t PORTA_EOI;                     /* Offset: 0x04c (W/R)  Port clear interrupt register */
    uint32_t EXT_PORTA;                     /* Offset: 0x050 (W/R)  PortA external port register */
    uint32_t LS_SYNC;                       /* Offset: 0x060 (W/R)  Level-sensitive synchronization enable register */
};

static struct cvi_gpio_regs_t cv182x_gpio_reg = {
    .SWPORTA_DR = 0x0,
    .SWPORTA_DDR = 0x4,
    .INTEN = 0x30,
    .INTMASK = 0x34,
    .INTTYPE_LEVEL = 0x38,
    .INT_POLARITY = 0x3c,
    .INTSTATUS = 0x40,
    .RAW_INTSTATUS = 0x44,
    .DEBOUNCE = 0x48,
    .PORTA_EOI = 0x4c,
    .EXT_PORTA = 0x50,
    .LS_SYNC = 0x60,
};

static struct cvi_gpio_regs_t *cvi_gpio_reg = &cv182x_gpio_reg;

#define GPIO_SWPORTA_DR(reg_base)       *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->SWPORTA_DR))
#define GPIO_SWPORTA_DDR(reg_base)      *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->SWPORTA_DDR))
#define GPIO_INTEN(reg_base)        	*((__IOM uint32_t *)(reg_base + cvi_gpio_reg->INTEN))
#define GPIO_INTMASK(reg_base)      	*((__IOM uint32_t *)(reg_base + cvi_gpio_reg->INTMASK))
#define GPIO_INTTYPE_LEVEL(reg_base)    *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->INTTYPE_LEVEL))
#define GPIO_INT_POLARITY(reg_base)     *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->INT_POLARITY))
#define GPIO_INTSTATUS(reg_base)    *((__IM uint32_t *)(reg_base + cvi_gpio_reg->INTSTATUS))
#define GPIO_RAW_INTSTATUS(reg_base) *((__IM uint32_t *)(reg_base + cvi_gpio_reg->RAW_INTSTATUS))
#define GPIO_DEBOUNCE(reg_base)     *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->DEBOUNCE))
#define GPIO_PORTA_EOI(reg_base)    *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->PORTA_EOI))
#define GPIO_EXT_PORTA(reg_base)    *((__IM uint32_t *)(reg_base + cvi_gpio_reg->EXT_PORTA))
#define GPIO_LS_SYNC(reg_base)      *((__IOM uint32_t *)(reg_base + cvi_gpio_reg->LS_SYNC))

static inline void dw_gpio_write_output_port(unsigned long reg_base, uint32_t value)
{
    pr_debug("write reg %08p value 0x%x\n", &GPIO_SWPORTA_DR(reg_base), value);
    GPIO_SWPORTA_DR(reg_base) = value;
    pr_debug("reg %08p value 0x%x\n", &GPIO_SWPORTA_DR(reg_base), GPIO_SWPORTA_DR(reg_base));
}

static inline uint32_t dw_gpio_read_output_port(unsigned long reg_base)
{
    return GPIO_SWPORTA_DR(reg_base);
}

static inline void dw_gpio_set_port_direction(unsigned long reg_base, uint32_t value)
{
    pr_debug("write reg %08p value 0x%x\n", &GPIO_SWPORTA_DDR(reg_base), value);
    GPIO_SWPORTA_DDR(reg_base) = value;
    pr_debug("reg %08p value 0x%x\n", &GPIO_SWPORTA_DDR(reg_base), GPIO_SWPORTA_DDR(reg_base));
}

static inline uint32_t dw_gpio_read_port_direction(unsigned long reg_base)
{
    return GPIO_SWPORTA_DDR(reg_base);
}

static inline void dw_gpio_set_port_irq(unsigned long reg_base, uint32_t value)
{
    GPIO_INTEN(reg_base) = value;
}

static inline uint32_t dw_gpio_read_port_irq(unsigned long reg_base)
{
    return GPIO_INTEN(reg_base);
}

static inline void dw_gpio_set_port_msk(unsigned long reg_base, uint32_t value)
{
    GPIO_INTMASK(reg_base) = value;
}

static inline uint32_t dw_gpio_read_port_msk(unsigned long reg_base)
{
    return GPIO_INTMASK(reg_base);
}

static inline void dw_gpio_set_port_irq_sensitive(unsigned long reg_base, uint32_t value)
{
    GPIO_INTTYPE_LEVEL(reg_base) = value;
}

static inline uint32_t dw_gpio_read_port_irq_sensitive(unsigned long reg_base)
{
    return GPIO_INTTYPE_LEVEL(reg_base);
}

static inline void dw_gpio_set_port_irq_polarity(unsigned long reg_base, uint32_t value)
{
    GPIO_INT_POLARITY(reg_base) = value;
}

static inline uint32_t dw_gpio_read_port_irq_polarity(unsigned long reg_base)
{
    return GPIO_INT_POLARITY(reg_base);
}

static inline uint32_t dw_gpio_read_port_int_status(unsigned long reg_base)
{
    pr_debug("read reg %08p value 0x%x\n", &GPIO_INTSTATUS(reg_base), GPIO_INTSTATUS(reg_base));
    return GPIO_INTSTATUS(reg_base);
}

static inline uint32_t dw_gpio_read_raw_port_int_status(unsigned long reg_base)
{
    return GPIO_RAW_INTSTATUS(reg_base);
}

static inline void dw_gpio_clr_port_irq(unsigned long reg_base, uint32_t value)
{
    GPIO_PORTA_EOI(reg_base) = value;
}

static inline uint32_t dw_gpio_read_input_port(unsigned long reg_base)
{
    return GPIO_EXT_PORTA(reg_base);
}

static inline void dw_gpio_set_sync_level(unsigned long reg_base, bool enable)
{
    GPIO_LS_SYNC(reg_base) = enable;
}

static inline uint32_t dw_gpio_read_sync_level(unsigned long reg_base)
{
    return GPIO_LS_SYNC(reg_base);
}

static inline void dw_gpio_set_debounce(unsigned long reg_base, uint32_t value)
{
    GPIO_DEBOUNCE(reg_base) = value;
}

static inline uint32_t dw_gpio_get_debounce(unsigned long reg_base)
{
    return GPIO_DEBOUNCE(reg_base);
}

#ifdef __cplusplus
}
#endif

#endif

