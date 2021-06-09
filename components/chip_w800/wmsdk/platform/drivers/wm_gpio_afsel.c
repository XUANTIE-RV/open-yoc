/**
 * @file wm_gpio.c
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_osal.h"
#include "tls_common.h"
#include "wm_gpio_afsel.h"
#include "wm_debug.h"

#define WM_SWD_ENABLE   1

void wm_spi_ck_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PB_01:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);			
			break;	

		case WM_IO_PB_02:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);			
			break;	
		
		default:
			TLS_DBGPRT_ERR("spi ck afsel config error!");
			break;
    }
}

void wm_spi_cs_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PA_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);			
			break;	

		case WM_IO_PB_04:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);			
			break;	

		default:
			TLS_DBGPRT_ERR("spi cs afsel config error!");
			break;
    }
}


void wm_spi_di_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PB_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);			
			break;	

		case WM_IO_PB_03:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);			
			break;	

		default:
			TLS_DBGPRT_ERR("spi di afsel config error!");
			break;
    }
}

void wm_spi_do_config(enum tls_io_name io_name)
{
    switch(io_name)
    {
		case WM_IO_PA_07:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;	

		case WM_IO_PB_05:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);			
			break;	
		
		default:
			TLS_DBGPRT_ERR("spi do afsel config error!");
			break;
    }
}


void wm_uart0_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_19:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 tx afsel config error!");
			break;
	}
}

void wm_uart0_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_20:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 20, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart0 rx afsel config error!");
			break;
	}
}

void wm_uart1_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_06:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 tx afsel config error!");
			break;
	}
}

void wm_uart1_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_07:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 7, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 rx afsel config error!");
			break;
	}
}

void wm_uart1_rts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{			
		case WM_IO_PB_19:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("uart1 rts afsel config error!");
			break;
	}
}

void wm_uart1_cts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{		
		case WM_IO_PB_20:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;
		default:
			TLS_DBGPRT_ERR("uart1 cts afsel config error!");
			break;
	}
}

void wm_uart2_tx_scio_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_02:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 tx afsel config error!");
			break;
	}
}

void wm_uart2_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_03:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 3, 0);
			break;	

		default:
			TLS_DBGPRT_ERR("uart2 rx afsel config error!");
			break;
	}
}

void wm_uart2_rts_scclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_04:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 rts afsel config error!");
			break;
	}
}

void wm_uart2_cts_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_05:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;

		default:
			TLS_DBGPRT_ERR("uart2 cts afsel config error!");
			break;
	}
}

void wm_uart3_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("uart3 tx afsel config error!");
			break;
	}
}

void wm_uart3_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_01:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 1, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart3 rx afsel config error!");
			break;
	}
}

void wm_uart4_tx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_04:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("uart4 tx afsel config error!");
			break;
	}
}

void wm_uart4_rx_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_05:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			tls_bitband_write(HR_GPIOB_DATA_PULLEN, 5, 0);
			break;

		default:
			TLS_DBGPRT_ERR("uart4 rx afsel config error!");
			break;
	}
}

void wm_i2s_ck_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;

		case WM_IO_PB_08:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("i2s master ck afsel config error!");
			break;
		}
}

void wm_i2s_ws_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_01:
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;
					
		case WM_IO_PB_09:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			break;
		}
}

void wm_i2s_do_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;
		
		case WM_IO_PB_11:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;
			
		default:
			TLS_DBGPRT_ERR("i2s master do afsel config error!");
			break;
		}
}

void wm_i2s_di_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_07:
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;
	
		case WM_IO_PB_10:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("i2s slave di afsel config error!");
			break;
		}
}

void wm_i2s_mclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("i2s mclk afsel config error!");
			break;		
	}
}

void wm_i2s_extclk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_07:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("i2s extclk afsel config error!");
			break;		
	}
}


void wm_i2c_scl_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_01:
			tls_gpio_cfg(io_name, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;

		case WM_IO_PB_20:
			tls_gpio_cfg(io_name, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;

		default:
			TLS_DBGPRT_ERR("i2c scl afsel config error!");
			break;
	}
}

void wm_i2c_sda_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_gpio_cfg(io_name, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;

		case WM_IO_PB_19:
			tls_gpio_cfg(io_name, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
			tls_io_cfg_set(io_name, WM_IO_OPTION4);
			break;

		default:
			TLS_DBGPRT_ERR("i2c sda afsel config error!");
			break;
	}
}

void wm_pwm1_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		case WM_IO_PB_19:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;			

		default:
			TLS_DBGPRT_ERR("pwm1 afsel config error!");
			break;
	}
}


void wm_pwm2_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PB_01:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		case WM_IO_PB_20:
			tls_io_cfg_set(io_name, WM_IO_OPTION2);
			break;	

		default:
			TLS_DBGPRT_ERR("pwm2 afsel config error!");
			break;
	}
}

void wm_pwm3_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_00:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		case WM_IO_PB_02:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm3 afsel config error!");
			break;
	}
}

void wm_pwm4_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_01:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		case WM_IO_PB_03:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm4 afsel config error!");
			break;
	}
}

void wm_pwm5_config(enum tls_io_name io_name)
{
	switch(io_name)
	{
		case WM_IO_PA_04:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		case WM_IO_PA_07:
			tls_io_cfg_set(io_name, WM_IO_OPTION1);
			break;

		default:
			TLS_DBGPRT_ERR("pwm5 afsel config error!");
			break;
	}
}

void wm_pwmbrk_config(enum tls_io_name io_name)
{
	switch(io_name)
	{	
		case WM_IO_PB_08:
			tls_io_cfg_set(io_name, WM_IO_OPTION3);
			break;

		default:
			TLS_DBGPRT_ERR("pwmbrk afsel config error!");
			break;
	}
}

void wm_swd_config(bool enable)
{
	if (enable)
	{
		tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION1);
		tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION1);
	}
	else
	{
		tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION5);
		tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION5);
	}
}

void wm_adc_config(u8 Channel)
{
        switch(Channel)
        {
            case 0:
				tls_io_cfg_set(WM_IO_PA_01, WM_IO_OPTION6);
				 break;			
				 
            case 1:
                tls_io_cfg_set(WM_IO_PA_04, WM_IO_OPTION6);
                break;

            default:
                break;                
        }
}

void wm_gpio_af_disable(void)
{
	tls_reg_write32(HR_GPIOA_DATA_DIR, 0x0);
	tls_reg_write32(HR_GPIOB_DATA_DIR, 0x0);

#if	WM_SWD_ENABLE
	tls_reg_write32(HR_GPIOA_AFSEL, 0x12); /*PA1:JTAG_CK,PA4:JTAG_SWO*/
#else
	tls_reg_write32(HR_GPIOA_AFSEL, 0x0);
#endif
	tls_reg_write32(HR_GPIOB_AFSEL, 0x0);

	tls_reg_write32(HR_GPIOA_DATA_PULLEN, 0xffff);
	tls_reg_write32(HR_GPIOB_DATA_PULLEN, 0xffffffff);
}

