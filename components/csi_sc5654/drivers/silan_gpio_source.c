/*
 * silan_gpio.c
 *
 * Modify Date:
*/
#define MODULE_NAME          "SILAN GPIO"

#include "ap1508.h"
#include "silan_gpio_source.h"
#include "silan_printf.h"
#include "silan_irq.h"
#include "silan_syscfg.h"
#include "silan_syscfg_regs.h"
#include "silan_iomux.h"
#include "silan_gpio.h"

#define GPIO_CONFIG_OFFSET(x)    (x / (GPIO1_MAX_NUM+1))
#define GPIO_CONFIG_PIN(x)       (x % (GPIO1_MAX_NUM+1))
#define IO_TO_GPIO(x)            ((x>=8) ? (x-8) : (34+x))

void silan_gpio_filsel_sel(uint32_t addr,uint32_t pin,int if_open)
{
    if(if_open){
        silan_gpio_filsel_open(addr,pin);
        silan_gpio_filsel_div(addr,0x3);
    }else{
        silan_gpio_filsel_close(addr,pin);
    }
}

void silan_io_input(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    silan_gpio_input(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_output(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    silan_gpio_output(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_direction(int io, int dir)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    if (dir)
        silan_gpio_output(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    else
        silan_gpio_input(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_set_value(int io, int val)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    if(val)
        silan_gpio_set_high(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    else
        silan_gpio_set_low(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_set_high(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    silan_gpio_set_high(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_set_low(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    silan_gpio_set_low(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

int silan_io_get(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    return(silan_gpio_get(SILAN_GPIO_BASE(offset), GPIO_PIN(pin)));
}

void silan_io_toggle(int io)
{
    int offset, pin, gpio;

    gpio = IO_TO_GPIO(io);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);
    if (silan_gpio_get(SILAN_GPIO_BASE(offset), GPIO_PIN(pin)))
        silan_gpio_set_low(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
    else
        silan_gpio_set_high(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}


void silan_io_irq_enable(int io)
{
    int offset, pin;

    offset = GPIO_CONFIG_OFFSET(io);
    pin = GPIO_CONFIG_PIN(io);

    silan_gpio_irq_enable(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_irq_clear(int io)
{
    int offset, pin;

    offset = GPIO_CONFIG_OFFSET(io);
    pin = GPIO_CONFIG_PIN(io);

    silan_gpio_irq_clear(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

void silan_io_irq_mode(int io, int mode)
{
    int offset, pin;

    offset = GPIO_CONFIG_OFFSET(io);
    pin = GPIO_CONFIG_PIN(io);

    silan_gpio_irq_mode(SILAN_GPIO_BASE(offset), GPIO_PIN(pin), mode);
}

void silan_io_filter_config(int gpio, int div)
{
    silan_gpio_filsel_div(SILAN_GPIO_BASE(gpio), div);
}

void silan_io_filter_open(int io)
{
    int offset, pin;

    offset = GPIO_CONFIG_OFFSET(io);
    pin = GPIO_CONFIG_PIN(io);

    silan_gpio_filsel_open(SILAN_GPIO_BASE(pin), offset);
}

void silan_io_filter_close(int io)
{
    int offset, pin;

    offset = GPIO_CONFIG_OFFSET(io);
    pin = GPIO_CONFIG_PIN(io);

    silan_gpio_filsel_open(SILAN_GPIO_BASE(pin), offset);
}

