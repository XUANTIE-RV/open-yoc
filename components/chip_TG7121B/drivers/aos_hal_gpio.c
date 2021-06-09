#include "aos/hal/gpio.h"
#include "reg_lsgpio.h"
#include "field_manipulate.h"
#include "reg_rcc.h"
#include "io_config.h"
#include <stddef.h>
#include "sdk_default_config.h"
#include "le501x.h"
#include "lsgpio.h"
#include <aos/kernel.h>
#define  GPIO_IRQ_SLOP_MAX  (16)

void ana_func1_io_init(uint8_t ain);

typedef  struct gpio_irq_slop {
    uint8_t pin_num;
    gpio_irq_handler_t handler;
    void * arg;
} gpio_irq_slop_t;

static gpio_irq_slop_t gGpioSlop[GPIO_IRQ_SLOP_MAX];

int8_t  gpio_slop_irq(int8_t irq_num)
{
        gGpioSlop[irq_num].handler(gGpioSlop[irq_num].arg);
        return(0);
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    switch(gpio->port)
    {
        case 0:
            __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
        case 1:
            __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
        case 2:
            __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
    }
    switch (gpio->config)
    {
        case ANALOG_MODE:
            ana_func1_io_init(gpio->port);
        break;
        case IRQ_MODE:
            //TODO
            io_cfg_input(gpio->port);
        break;
        case INPUT_PULL_UP:
            io_pull_write(gpio->port,IO_PULL_UP);
            io_cfg_input(gpio->port);
        break;
        case INPUT_PULL_DOWN:
            io_pull_write(gpio->port,IO_PULL_DOWN);
            io_cfg_input(gpio->port);
        break;
        case INPUT_HIGH_IMPEDANCE:
            io_pull_write(gpio->port,IO_PULL_DISABLE);
        break;
        case OUTPUT_PUSH_PULL:
            io_cfg_output(gpio->port);
        break;
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            io_cfg_output(gpio->port);
            io_pull_write(gpio->port,IO_PULL_DISABLE);
            io_cfg_opendrain(gpio->port);
        break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            io_cfg_output(gpio->port);
            io_pull_write(gpio->port,IO_PULL_UP);
            io_cfg_opendrain(gpio->port);
        break;
        case OUTPUT_OPEN_DRAIN_AF:

        break;
        case OUTPUT_PUSH_PULL_AF:

        break;

    }
    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    io_write_pin(gpio->port,1);
    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    io_write_pin(gpio->port,0);
    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t* gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    io_toggle_pin(gpio->port);
    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if(gpio == NULL)
    {
        return -1;
    }
    *value = (uint32_t)io_read_pin(gpio->port);
    return 0;
}

#define PINS_IN_GROUP 16
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{
    if(gpio == NULL)
    {
        return -1;
    }
    uint8_t num = gpio->port % PINS_IN_GROUP;
    gGpioSlop[num].pin_num = gpio->port;
    gGpioSlop[num].handler = handler;
    gGpioSlop[num].arg = arg;
    io_exti_config(gpio->port,trigger);
    io_exti_enable(gpio->port,1);
    return (0);

}

void io_exti_callback(uint8_t pin)
{
    uint8_t num = pin % PINS_IN_GROUP;
    aos_kernel_intrpt_enter();
    gpio_slop_irq(num);
    aos_kernel_intrpt_exit();
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    io_exti_enable(gpio->port,0);
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    if(gpio == NULL)
    {
        return -1;
    }
    return 0;
}
