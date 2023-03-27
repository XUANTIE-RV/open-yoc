#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"

typedef enum
{
	APP_TEST_IIC_SLAVE_RECV_SEND,  //设备做iic通信中的从, 测试数据的接收与发送
	APP_TEST_IIC_MASTER_SEND_RECV, //设备做iic通信中的主, 测试数据的发送和接收
	APP_TEST_IIC_MEM_WRITE_READ,   //设备做iic通信中的主, 测试mem模式数据的读写
} APP_TEST_IIC_FUNCTION;

void board_yoc_init(void);
int hal_adc_demo(uint8_t gpio_pin);
int hal_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_watchdog_demo(void);
int hal_gpio_out_demo(int port);
int hal_gpio_int_demo(int port, int trigger_method);
int hal_uart_demo(uint32_t *gpio_pins);
int hal_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins);
int hal_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_flash_demo(void);
int hal_rtc_demo(void);
int hal_clk_demo(void);
int hal_mmc_demo(void);

#define PORT_WDG_0 0

#endif