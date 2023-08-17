#include <devices/device.h>
#include <devices/driver.h>
#include <devices/spi.h>
#include <devices/devicelist.h>
#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"
#include "spi_hal.h"

#include <bl606p_gpio.h>
#include <bl606p_glb.h>
#include <bl606p_spi.h>

#define SPI0_PORT_NUM       0
#define SPI1_PORT_NUM       1
#define SPI_DATA_NUM        10
#define SPI_TX_TIMEOUT      2000
#define SPI_RX_TIMEOUT      2000

/**
 * @brief SPI Init
 * 
 */
rvm_dev_t *spi_dev = NULL;
rvm_dev_t *SPI_Init_Config(uint32_t spi_idx, void *config)
{
    int ret = -1;
    rvm_hal_spi_config_t *spi_config = (rvm_hal_spi_config_t *)config;
    char spi_filename[8];

	sprintf(spi_filename, "spi%d", spi_idx);
	spi_dev = rvm_hal_spi_open(spi_filename);
	if (!spi_dev)
		printf("ERROR: SPI DEV OPEN FAILED!\r\n");

	ret = rvm_hal_spi_config(spi_dev, spi_config);
	if (ret)
		printf("ERROR: SPI config failed !\r\n");

	return spi_dev;
}

void SPI_unInit_Config(rvm_dev_t *dev)
{
	rvm_hal_spi_close(dev);
}

unsigned char hal_spi_send_one_byte(unsigned char sdata)
{
	uint8_t data = sdata;
	if (rvm_hal_spi_send(spi_dev, &data, 1, SPI_TX_TIMEOUT)) {
		printf("%s error\r\n", __func__);
		return 1;
	}
	
	return 0;
}


unsigned char hal_spi_receive_one_byte(void)
{
	unsigned char rdata[64] __attribute__((aligned(32)));

#if 1
	if (rvm_hal_spi_recv(spi_dev, rdata, 1, SPI_RX_TIMEOUT)) {
		printf("%s error\r\n", __func__);
		return 1;
	}
#endif

	return rdata[0];
}
