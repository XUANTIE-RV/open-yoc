#include <devices/spi.h>
#include <aos/kernel.h>
#include <string.h>
#include "crc.h"
#include "spi.h"
#include "spi_hal.h"
// #include "stm32f10x_conf.h" //SPI_I2S_GetFlagStatus用到该头文件

unsigned char spi_io_buf[1024];

void delay_time(int times)
{
	while(times--)
	{
		printf(".");
		// __nop();
	}
}


/**
 * @brief SPI发送函数
 * 
 * @param ucData  数据域缓存
 * @param uiLength  数据域长度
 *
 */
int SPI_Send(unsigned char * ucData, unsigned int uiLength)
{
	unsigned int len = 0, i = 0;
	unsigned short crc;
	
	//输入数据CRC校验
	g_crc.init(CRC_CCIT_KERMIT, 1);
	g_crc.update(ucData, uiLength);
	crc = g_crc.get_result();
	
	//SPI协议主发送数据
	spi_io_buf[i++] = 0x00;
	spi_io_buf[i++] = 0x00;
	spi_io_buf[i++] = 0x00;
	spi_io_buf[i++] = 0x55;
	spi_io_buf[i++] = 0xAA;
	spi_io_buf[i++] = 0x00;
	spi_io_buf[i++] = (uiLength >> 8) & 0xff;   //长度域
	spi_io_buf[i++] = uiLength & 0xff;
	
	memmove(spi_io_buf + i, ucData, uiLength);  //数据域
	i += uiLength;
	
	spi_io_buf[i++] = (crc >> 8) & 0xff;  //校验域
	spi_io_buf[i++] = crc & 0xff;
	
	SPI_CS_EN(); //NSS电平由高变低，SPI通讯的起始信号
	for (len = 0; len < 8; len++)
	{
		hal_spi_send_one_byte(spi_io_buf[len]);
	}
	// delay_time(100);
	aos_msleep(1);

	for (; len < i; len++)
	{
		hal_spi_send_one_byte(spi_io_buf[len]);
	}
	
	// while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
    
	SPI_CS_DN();
	
  return 0;
}

/**
 * @brief SPI接收函数
 * 
 * @param ucData 数据域缓存
 * @param puiLength 返回的数据长度
 * 
 * @note
 * 返回的数据格式为：
 * ... 000000 + AA55 + 00 + 数据域长度[2 Bytes] + 数据域 + 数据域CRC结算结果
 */
int SPI_Recv(unsigned char *ucData, unsigned int *puiLength)
{
	unsigned short crc = 0, crc_result;
	int len = 0, i = 0;
		
	SPI_CS_EN();
	
	while (0x00 != hal_spi_receive_one_byte());
	while (0x00 != hal_spi_receive_one_byte());
	while (0x00 != hal_spi_receive_one_byte());
	while (0xAA != hal_spi_receive_one_byte());
	if (0x55 != hal_spi_receive_one_byte())
	{
		return 1;
	}
	if (0x00 != hal_spi_receive_one_byte())
	{
		return 1;
	}
	
	//接收的数据域长度
	len = hal_spi_receive_one_byte();
	len <<= 8;
	len |= hal_spi_receive_one_byte();
	*puiLength = len;
	
	//接收的数据域内容
	for (i = 0; i < len; i++)
	{
		spi_io_buf[i] = hal_spi_receive_one_byte();
		ucData[i] = spi_io_buf[i];
	}

	//CRC校验
	
	g_crc.init(CRC_CCIT_KERMIT, 1);//理论CRC
	g_crc.update(spi_io_buf, len);
	crc_result = g_crc.get_result();
	
	crc = hal_spi_receive_one_byte();//实际接收的CRC
	crc <<= 8;
	crc |= hal_spi_receive_one_byte();
	
	if (crc != crc_result)
	{
		return 1;
	}
	
	// while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
	SPI_CS_DN();
  return 0;
}


