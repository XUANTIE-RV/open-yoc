#ifndef _SPI_H
#define _SPI_H

#if 0
#define SPI_CS_DN()		GPIO_SetBits(GPIOA,GPIO_Pin_4) //设置端口PA4
#define SPI_CS_EN()		GPIO_ResetBits(GPIOA,GPIO_Pin_4) //清除端口PA4
#else
#define SPI_CS_DN()
#define SPI_CS_EN()
#endif

/**
 * @brief SPI发送函数
 * 
 * @param ucData  数据域缓存
 * @param uiLength  数据域长度
 */
int SPI_Send(unsigned char * ucData, unsigned int uiLength);


/**
 * @brief SPI接收函数
 * 
 * @param ucData 数据域缓存
 * @param puiLength 返回的数据域长度
 * 
 * @note
 * 数据格式为：
 * ... 000000 + AA55 + 00 + 数据域长度[2 Bytes] + 数据域 + 数据域CRC结算结果
 */
int SPI_Recv(unsigned char *ucData, unsigned int *puiLength);

#endif
