#ifndef __APP_LED_H__
#define __APP_LED_H__

#include <stdint.h>

void led_init(void);
void led_open(void);
void led_close(void);
void led_write_byte(uint8_t reg_addr, uint8_t reg_data);
void led_write_frame(uint8_t buffer[85]);
void led_write_frame2(uint8_t buffer[85]);
void led_pwm_write(uint8_t iColor, uint8_t iX, uint8_t iY, uint8_t idata);

#endif