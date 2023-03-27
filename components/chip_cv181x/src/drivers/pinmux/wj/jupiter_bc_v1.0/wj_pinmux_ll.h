/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pinmux_ll.h
 * @brief    head file for jupiter lc pinmux
 * @version  V1.0
 * @date     29. Dec. 2020
 ******************************************************************************/
#ifndef _WJ_PINMUX_LL_
#define _WJ_PINMUX_LL_

#define HOBBIT_GIPO0_PORTCTL_REG 0xD6030000U
#define HOBBIT_IOMUX0L_REG       0xD6030008U
#define HOBBIT_IOMUX0H_REG       0xD603000CU
#define HOBBIT_WAKEUP0_REG       0xD6030018U
#define HOBBIT_WAKEUP_POLAR0_REG 0xD6030078U
#define HOBBIT_IOPU0_REG         0xD6030020U
#define HOBBIT_IOPD0_REG         0xD6030028U
#define HOBBIT_IODS0L_REG        0xD6030038U
#define HOBBIT_IODS0H_REG        0xD603003CU
#define HOBBIT_IO_GPIO_SEL2_REG  0xD60300A4U
#define HOBBIT_IO_PU2_REG        0xD60300B0U

#define CSKY_ANA_IO_EN_REG       (WJ_IOC_BASE + 0x70U)
#define QSPI_POINT_POS           (0x0U)
#define QSPI_POINT_MASK          (0x3FU<<QSPI_POINT_POS)

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))


static inline void wj_set_digitalio_to_qspi(void)
{
     writel(QSPI_POINT_MASK, HOBBIT_IO_GPIO_SEL2_REG);
}

static inline void wj_set_digitalio_to_gpio(void)
{
     writel(0, HOBBIT_IO_GPIO_SEL2_REG);
}

static inline void wj_set_digitalio_pullup(void)
{
     writel(QSPI_POINT_MASK, HOBBIT_IO_PU2_REG);
}

static inline void wj_reset_digitalio_pullup(void)
{
     writel(0, HOBBIT_IO_PU2_REG);
}

static inline void wj_set_gpio_func_select(uint32_t val)
{
     writel(val, HOBBIT_GIPO0_PORTCTL_REG);
}

static inline uint32_t wj_get_gpio_func_select(void)
{
    return readl(HOBBIT_GIPO0_PORTCTL_REG);
}

static inline void wj_set_gpio_pulldown(uint32_t val)
{
     writel(val, HOBBIT_IOPU0_REG);
}

static inline void wj_set_gpio_pullup(uint32_t val)
{
     writel(val, HOBBIT_IOPD0_REG);
}

static inline uint32_t wj_get_gpio_pulldown(void)
{
    return readl(HOBBIT_IOPU0_REG);
}

static inline uint32_t wj_get_gpio_pullup(void)
{
    return readl(HOBBIT_IOPD0_REG);
}
#endif