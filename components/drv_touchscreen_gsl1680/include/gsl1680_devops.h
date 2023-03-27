#ifndef __GSL1680_DEVOPS_H__
#define __GSL1680_DEVOPS_H__



typedef struct gsl1680_gpio_pin_t {
    int iic_id;
    int reset_gpio_polarity;
    int reset_gpio_group;
    int reset_gpio_chn;
    int interrupt_gpio_group;
    int interrupt_gpio_chn;
} gsl1680_gpio_pin;


void drv_gsl1680_devops_register(gsl1680_gpio_pin * config);










#endif

