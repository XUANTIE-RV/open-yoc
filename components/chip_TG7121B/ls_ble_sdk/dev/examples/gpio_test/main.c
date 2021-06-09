#include "lsgpio.h"
#include "le501x.h"
#include "sys_stat.h"
#include "io_config.h"
#include <string.h>

#define GPIO_TEST 0
#define EXTI_TEST 1

#if (GPIO_TEST)
void gpio_test(void)
{
    io_cfg_input(PB00);
    io_pull_write(PB00, IO_PULL_DISABLE);
    io_cfg_input(PB07);
    io_pull_write(PB00, IO_PULL_DISABLE);

    io_cfg_output(PB08);
    io_write_pin(PB08,0);
    io_cfg_output(PB09);
    io_write_pin(PB09,0);

    while (1)
    {
        io_write_pin(PB08,io_read_pin(PB000));
        io_write_pin(PB09,io_read_pinPB07));
    }
}
#endif //(GPIO_TEST)

#if (EXTI_TEST)
void exti_test(void)
{
    io_cfg_output(PB08);
    io_write_pin(PB08,0);

    io_cfg_output(PB09);
    io_write_pin(PB09,0);

    io_cfg_input(PB15);
    io_pull_write(PB15,IO_PULL_UP);
    io_exti_config(PB15,INT_EDGE_RISING);
    io_exti_enable(PB15,true);

    NVIC_EnableIRQ(EXTI_IRQn); //exti_IRQHandler

    NVIC_SetPriority(EXTI_IRQn,1);
}
/*
void EXTI_Handler(void)
{
    //clear Exti
    WRITE_REG(EXTI->EICR, READ_REG(EXTI->ERIF)); 

    HAL_GPIO_TogglePin(LSGPIOB, GPIO_PIN_8);
}
*/

#endif //(EXTI_TEST)
int main(void)
{
#if (GPIO_TEST)
    gpio_test();
#endif

#if (EXTI_TEST)
    exti_test();
#endif

    while (1)
    {
        ;
    }
}

void io_exti_callback(uint8_t pin) // override io_exti_callback
{
    switch (pin)
    {
    case PA07:
        // do something
        break;
    case PB15:
        // do something
        break;
    default:
        break;
    }
}
