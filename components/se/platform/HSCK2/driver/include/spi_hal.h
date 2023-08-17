#ifndef _SPI_HAL_H_
#define _SPI_HAL_H_
#include "stdint.h"
#include "devices/driver.h"

extern unsigned char ins[];
/**
 * @brief SPI配置初始化
 * 
 */
rvm_dev_t *SPI_Init_Config(uint32_t spi_idx, void *config);

void SPI_unInit_Config(rvm_dev_t *dev);

//发送一个字节
unsigned char hal_spi_send_one_byte(unsigned char sbuf);

//接收一个字节
unsigned char hal_spi_receive_one_byte(void);

#endif
