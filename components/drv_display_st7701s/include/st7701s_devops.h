#ifndef __ST7701S_DEVOPS_H__
#define __ST7701S_DEVOPS_H__

typedef struct st7701s_gpio_pin_t {
    int blank_pwm_bank;
    int blank_pwm_chn;
} st7701s_gpio_pin;

void drv_st7701s_display_register(st7701s_gpio_pin *config);
void drv_st7701s_display_unregister();

#endif