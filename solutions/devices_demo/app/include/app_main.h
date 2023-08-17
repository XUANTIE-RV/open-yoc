/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"
#include "app_config.h"

typedef enum
{
	APP_TEST_IIC_SLAVE_RECV,  		//设备做iic通信中的从, 测试数据的接收
	APP_TEST_IIC_MASTER_SEND, 		//设备做iic通信中的主, 测试数据的发送
	APP_TEST_IIC_SLAVE_SEND,  		//设备做iic通信中的从, 测试数据的发送
	APP_TEST_IIC_MASTER_RECV, 		//设备做iic通信中的主, 测试数据的接收
	APP_TEST_IIC_MEM_WRITE_READ,   	//设备做iic通信中的主, 测试mem模式数据的读写
} APP_TEST_IIC_FUNCTION;

typedef enum
{
	APP_TEST_SPI_SLAVE_RECV_MASTER_SEND, 	//设备有两个spi, 测试数据的主发从收
	APP_TEST_SPI_SLAVE_SEND,			 	//设备做spi通信中的从, 测试数据的发送
	APP_TEST_SPI_SLAVE_RECV,			 	//设备做spi通信中的从, 测试数据的接收
	APP_TEST_SPI_SLAVE_SEND_RECV,		 	//设备做spi通信中的从, 测试数据的发送和接收
	APP_TEST_SPI_MASTER_SEND,			 	//设备做spi通信中的主, 测试数据的发送
	APP_TEST_SPI_MASTER_RECV,			 	//设备做spi通信中的主, 测试数据的接收
	APP_TEST_SPI_MASTER_SEND_RECV,		 	//设备做spi通信中的主, 测试数据的发送和接收
} APP_TEST_SPI_FUNCTION;

void board_yoc_init(void);
int hal_adc_demo(uint8_t gpio_pin);
int hal_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_watchdog_demo(void);
int hal_gpio_out_demo(int port);
int hal_gpio_in_demo(int port, int trigger_method);
int hal_uart_demo(uint32_t *gpio_pins);
int hal_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins);
int hal_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_task_iic_demo(uint8_t is_master, uint8_t gpio_pin0, uint8_t gpio_pin1);
int hal_flash_demo(void);
int hal_rtc_demo(void);
int hal_clk_demo(void);
int hal_mmc_demo(void);
int hal_timer_demo(void);
int hal_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle,
                     uint32_t freq_chg, float duty_cycle_chg);
int hal_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin);

int devfs_adc_demo(uint8_t gpio_pin);
int devfs_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1);
int devfs_uart_demo(uint32_t *gpio_pins);
int devfs_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins);
int devfs_rtc_demo(void);
int devfs_wdt_demo(void);
int devfs_rtc_cancel_alarm_demo(void);
int devfs_clk_demo(void);
int devfs_gpio_out_demo(int port);
int devfs_gpio_in_demo(int port, int trigger_method);
int devfs_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1);
int devfs_display_input_demo(void);
int devfs_hci_demo(void);
int devfs_wifi_demo(void);
int devfs_timer_demo(void);
int devfs_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle,
                       uint32_t freq_chg, float duty_cycle_chg);
int devfs_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin);

#define PORT_WDG_0 0

#endif