/**
 * @file wm_gpio_afsel.h
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_GPIO_AFSEL_H
#define WM_GPIO_AFSEL_H

#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_osal.h"
#include "tls_common.h"
/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup IOMUX_Driver_APIs IOMUX Driver APIs
 * @brief IO Multiplex driver APIs
 */

/**
 * @addtogroup IOMUX_Driver_APIs
 * @{
 */



/**
 * @brief  config the pins used for spi ck
 * @param  io_name: config spi ck pins name
 *			WM_IO_PB_01
 *			WM_IO_PB_02
 *				
 * @return None
 */
void wm_spi_ck_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi cs
 * @param  io_name: config spi cs pins name
 *			WM_IO_PA_00
 *			WM_IO_PB_04
 *				
 * @return None
 */
void wm_spi_cs_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi di
 * @param  io_name: config spi di pins name
 *			WM_IO_PB_00
 *			WM_IO_PB_03
 *				
 * @return None
 */
void wm_spi_di_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for spi do
 * @param  io_name: config spi do pins name
 *			WM_IO_PA_07
 *			WM_IO_PB_05
 *				
 * @return None
 */
void wm_spi_do_config(enum tls_io_name io_name);


/**
 * @brief  config the pins used for uart0 tx
 * @param  io_name: config uart0 tx pins name
 *			WM_IO_PB_19
 *				
 * @return None
 */
void wm_uart0_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart0 rx
 * @param  io_name: config uart0 rx pins name
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_uart0_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 tx
 * @param  io_name: config uart1 tx pins name
 *			WM_IO_PB_06
 *				
 * @return None
 */
void wm_uart1_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 rx
 * @param  io_name: config uart1 rx pins name
 *			WM_IO_PB_07
 *				
 * @return None
 */
void wm_uart1_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 rts
 * @param  io_name: config uart1 rts pins name
 *			WM_IO_PB_19
 *				
 * @return None
 */
void wm_uart1_rts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart1 cts
 * @param  io_name: config uart1 cts pins name
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_uart1_cts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 tx or 7816-io
 * @param  io_name: config uart2 tx or 7816-io pins name
 *			WM_IO_PB_02
 *				
 * @return None
 */
void wm_uart2_tx_scio_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 rx
 * @param  io_name: config uart2 rx pins name
 *			WM_IO_PB_03
 *				
 * @return None
 */
void wm_uart2_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 rts or 7816-clk
 * @param  io_name: config uart2 rts or 7816-clk pins name
 *			WM_IO_PB_04
 *				
 * @return None
 */
void wm_uart2_rts_scclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart2 cts
 * @param  io_name: config uart2 cts pins name
 *			WM_IO_PB_05
 *				
 * @return None
 */
void wm_uart2_cts_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart3 tx
 * @param  io_name: config uart1 tx pins name
 *			WM_IO_PB_00
 *				
 * @return None
 */
void wm_uart3_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart3 rx
 * @param  io_name: config uart1 rx pins name
 *			WM_IO_PB_01
 *				
 * @return None
 */
void wm_uart3_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart4 tx
 * @param  io_name: config uart1 tx pins name
 *			WM_IO_PB_04
 *				
 * @return None
 */
void wm_uart4_tx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for uart4 rx
 * @param  io_name: config uart1 rx pins name
 *			WM_IO_PB_05
 *				
 * @return None
 */
void wm_uart4_rx_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s ck
 * @param  io_name: config i2s master ck pins name
 *			WM_IO_PA_04	 
 *			WM_IO_PB_08
 *				
 * @return None
 */
void wm_i2s_ck_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s ws
 * @param  io_name: config i2s master ws pins name
 *			WM_IO_PA_01
 *			WM_IO_PB_09
 *				
 * @return None
 */
void wm_i2s_ws_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s do
 * @param  io_name: config i2s master do pins name
 *			WM_IO_PA_00
 *			WM_IO_PB_11
 *				
 * @return None
 */
void wm_i2s_do_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s di
 * @param  io_name: config i2s slave di pins name
 *			WM_IO_PA_07
 *			WM_IO_PB_10
 *				
 * @return None
 */
void wm_i2s_di_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s mclk
 * @param  io_name: config i2s mclk pins name
 *			WM_IO_PA_00
 *				
 * @return None
 */
void wm_i2s_mclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2s extclk
 * @param  io_name: config i2s extclk pins name
 *			WM_IO_PA_07
 *				
 * @return None
 */
void wm_i2s_extclk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2c scl
 * @param  io_name: config i2c scl pins name
 *			WM_IO_PA_01
 *			WM_IO_PB_19
 *				
 * @return None
 */
void wm_i2c_scl_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for i2c sda
 * @param  io_name: config i2c sda pins name
 *			WM_IO_PA_04
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_i2c_sda_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm1
 * @param  io_name: config pwm1 pins name
 *			WM_IO_PA_00
 *			WM_IO_PB_00
 *			WM_IO_PB_19
 *				
 * @return None
 */
void wm_pwm1_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm1
 * @param  io_name: config pwm1 pins name
 *			WM_IO_PA_01
 *			WM_IO_PB_01
 *			WM_IO_PB_20
 *				
 * @return None
 */
void wm_pwm2_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm3
 * @param  io_name: config pwm3 pins name
 *			WM_IO_PB_02
 *				
 * @return None
 */
void wm_pwm3_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm4
 * @param  io_name: config pwm4 pins name
 *			WM_IO_PB_03
 *				
 * @return None
 */
void wm_pwm4_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm5
 * @param  io_name: config pwm5 pins name
 *			WM_IO_PA_04
 *				
 * @return None
 */
void wm_pwm5_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for pwm break
 * @param  io_name: config pwm break pins name
 *			WM_IO_PB_08
 *				
 * @return None
 */
void wm_pwmbrk_config(enum tls_io_name io_name);

/**
 * @brief  config the pins used for swd
 * @param  enable: enable or disable chip swd function
 *			1: enable
 *			0: disable
 *				
 * @return None
 */
void wm_swd_config(bool enable);

/**
 * @brief  config the pins used for adc
 * @param  Channel: the channel that shall be used
 *			0~1: single-ended input
 *			0~1: differential input
 *				
 * @return None
 */
void wm_adc_config(u8 Channel);

/**
 * @brief  disable all the gpio af
 *				
 * @return None
 *
 * @note  This function must call before anyothers for configure 
 * 		  gpio Alternate functions
 */
void wm_gpio_af_disable(void);
/**
 * @}
 */

/**
 * @}
 */
void wm_psram_ck_config(enum tls_io_name io_name);

void wm_psram_cs_config(enum tls_io_name io_name);

void wm_psram_dat0_config(enum tls_io_name io_name);

void wm_psram_dat1_config(enum tls_io_name io_name);

void wm_psram_dat2_config(enum tls_io_name io_name);

void wm_psram_dat3_config(enum tls_io_name io_name);


#endif /* end of WM_GPIO_AFSEL_H */

