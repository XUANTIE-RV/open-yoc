#ifndef _AP_IOMUX_REGS_H
#define _AP_IOMUX_REGS_H

enum{
	I2S_IN1=0,
	I2S_IN2,
	I2S_IN3,
	I2S_IN_51,
	I2S_OUT1,
	I2S_OUT2,
	I2S_OUT3,
	I2S_OUT_51,
	SDMMC,
	UART1,
	UART2,
	UART3,
	UART4,
	IIC1,
	IIC2,
	SPI1,
	SPI2,
	SPDIFIN1,
	SPDIFIN2,
	SPDIFIN3,
	SPDIFIN4,
	SPDIFOUT1,
	SPDIFOUT2,
	SPDIFOUT3,
	SPDIFOUT4
};
enum{
	OFF=0,
	ON
};

typedef union io_attr {
	/** raw data */
	char d8;
	/** bits def */
	struct {
		unsigned func:3;
		unsigned ds:2;
#define IO_ATTR_DS_2mA     0
#define IO_ATTR_DS_4mA     1
#define IO_ATTR_DS_8mA     2
#define IO_ATTR_DS_24mA    3

		unsigned ren:1;
#define IO_ATTR_PULL_EN    0
#define IO_ATTR_PULL_DIS   1

		unsigned ie:1;
#define IO_ATTR_INPUT_EN   1
#define IO_ATTR_INPUT_DIS  0

		unsigned sr:1;
#define IO_ATTR_SR_LS      0
#define IO_ATTR_SR_HS      1

	} __attribute__((packed)) b;
} __attribute__((packed)) io_attr_t;

//typedef union io_attr_reg {
//	/** raw data */
//	uint32_t d32;
//	/** bits def */
//	io_attr_t io_attr[4] __attribute__((packed));
//}  __attribute__((packed)) io_attr_reg_t;

#endif
