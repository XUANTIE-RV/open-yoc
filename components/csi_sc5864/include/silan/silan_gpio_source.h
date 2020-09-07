/*
 * silan_gpio.h
 *
 * Modify Date:
 */
#ifndef __SILAN_GPIO_H__
#define __SILAN_GPIO_H__

#include "silan_gpio_regs.h"

#define GPIO1			 0
#define GPIO2            1

#define GPIO1_MAX_NUM    22
#define GPIO2_MAX_NUM    10

#define IO_MAX_NUM       41

#define IO_INPUT		 0
#define IO_OUTPUT 		 1

static inline void silan_gpio_input(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_DIR) &= ~pin;
}

static inline void silan_gpio_output(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_DIR) |= pin;
}

static inline void silan_gpio_set_high(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_DATA) |= pin;
}

static inline void silan_gpio_set_low(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_DATA) &= ~pin;
}

static inline int silan_gpio_get(uint32_t addr, uint32_t pin)
{
	if (__sREG32(addr, GPIO_DATA) & pin)
	{
        return 1;
    }
    return 0;
}

static inline void silan_gpio_irq_enable(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_IE) |= pin;
}

static inline void silan_gpio_irq_clear(uint32_t addr, uint32_t pin)
{
	__sREG32(addr, GPIO_IC) |= pin;
}

static inline void silan_gpio_irq_mode(uint32_t addr, uint32_t pin, int mode)
{
    uint32_t base = addr;

    if ((mode == GPIO_High_Level) || (mode == GPIO_Low_Level))
    {
        __sREG32(base,GPIO_IS) |= pin;
    }
    else
    {
        __sREG32(base,GPIO_IS) &= ~pin;
        if (mode == GPIO_Edge)
        {
            __sREG32(base,GPIO_IBE) |= pin;
        }
    }
    if ((mode == GPIO_High_Level) || (mode == GPIO_High_Edge))
    {
        __sREG32(base,GPIO_IEV) |= pin;
    }
    else
    {
        __sREG32(base,GPIO_IEV) &= ~pin;
    }
}

static inline void silan_gpio_filsel_open(uint32_t addr,uint32_t pin)
{
	__sREG32(addr, GPIO_FILSEL) |= pin;
}

static inline void silan_gpio_filsel_close(uint32_t addr,uint32_t pin)
{
	__sREG32(addr, GPIO_FILSEL) &= ~pin;
}

static inline void silan_gpio_filsel_div(uint32_t addr,int div)
{
	__sREG32(addr, GPIO_FILDIV) |= ((div&0xf)<<1)|0x1;
}

void silan_io_input(int io);
void silan_io_output(int io);
void silan_io_direction(int io, int dir);
void silan_io_set_high(int io);
void silan_io_set_low(int io);
void silan_io_set_value(int io, int val);
int silan_io_get(int io);
void silan_io_toggle(int io);
void silan_io_irq_enable(int io);
void silan_io_irq_clear(int io);
void silan_io_irq_mode(int io, int mode);

#endif // __SILAN_GPIO_H__
