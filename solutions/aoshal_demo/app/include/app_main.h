#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"

void board_yoc_init(void);
void aoshal_test(void);
int hal_adc_out_signal_demo(uint8_t gpio_pin);
int hal_adc_out_multiple_demo(uint8_t gpio_pin);
int hal_task_adc_multiple_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_task_adc_signal_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle);
int hal_watchdog_demo(void);
int hal_gpio_out_demo(int port);
int hal_gpio_int_demo(int port, int trigger_method);
int hal_uart_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_spi_demo(uint32_t *gpio_pin);
int hal_iic_master_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_iic_slave_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_flash_demo(void);

#define PORT_WDG_0 0

#endif