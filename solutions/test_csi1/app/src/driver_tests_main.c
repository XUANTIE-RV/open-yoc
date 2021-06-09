#include <autotest.h>
#include <test_config.h>
#include <test_log.h>
#include <test_common.h>
#include <string.h>
#include <driver_test.h>

volatile char _mc_name[MODULES_LENGTH] = {'\0'}; // define module and testcase name
volatile int _test_result = 0;
static char at_para[PARAMETERS_LENGTH];

volatile int8_t _dev_idx = -1;

int32_t driver_mc_name_parsing(char *cmd)
{
	cmd_parsing(cmd, (void *)_mc_name, at_para);
    
    if(strstr((const char *)_mc_name, "PINMUX")) {
		TEST_CASE_START();
		_dev_idx = test_pinmux_main(at_para);
	}

#if defined(CONFIG_UART)
	else if (strstr((const char *)_mc_name, "UART_")) {
		TEST_CASE_START();
		test_uart_main(at_para);
	}
#endif
#if defined(CONFIG_GPIO)
	else if (strstr((const char *)_mc_name, "GPIO_")) {
		TEST_CASE_START();
		test_gpio_main(at_para); 
	}
#endif
#if defined(CONFIG_DMA)
    else if (strstr((const char *)_mc_name, "DMA_")) {
        TEST_CASE_START();
        test_dma_main(at_para);
    }
#endif
#if defined(CONFIG_RTC)
    else if (strstr((const char *)_mc_name, "RTC_")) {
        TEST_CASE_START();
        test_rtc_main(at_para);
    }
#endif
#if defined(CONFIG_ADC)
    else if (strstr((const char *)_mc_name, "ADC_")) {
        TEST_CASE_START();
        test_adc_main(at_para);
    }
#endif
#if defined(CONFIG_WDT)
    else if (strstr((const char *)_mc_name, "WDT_")) {
        TEST_CASE_START();
        test_wdt_main(at_para);
    }
#endif
#if defined(CONFIG_IIC)
    else if (strstr((const char *)_mc_name, "IIC_")) {
        TEST_CASE_START();
        test_iic_main(at_para);
    }
#endif
#if defined(CONFIG_SPI)
    else if (strstr((const char *)_mc_name, "SPI_")) {
        TEST_CASE_START();
        test_spi_main(at_para);
    }
#endif
#if defined(CONFIG_I2S)
    else if (strstr((const char *)_mc_name, "I2S_")) {
        TEST_CASE_START();
        test_i2s_main(at_para);
    }
#endif
#if defined(CONFIG_SPIFLASH)
    else if (strstr((const char *)_mc_name, "SPIFLASH_")) {
        TEST_CASE_START();
        test_spiflash_main(at_para);
    }
#endif
#if defined(CONFIG_EFLASH)
    else if (strstr((const char *)_mc_name, "EFLASH_")) {
        TEST_CASE_START();
        test_eflash_main(at_para);
    }
#endif
#if defined(CONFIG_TIMER)
    else if (strstr((const char *)_mc_name, "TIMER_")) {
        TEST_CASE_START();
        test_timer_main(at_para);
    }
#endif
#if defined(CONFIG_PWM)
    else if (strstr((const char *)_mc_name, "PWM_")) {
        TEST_CASE_START();
        test_pwm_main(at_para);
    }
#endif
#if defined(CONFIG_CODEC)
    else if (strstr((const char *)_mc_name, "CODEC_")) {
        TEST_CASE_START();
        test_codec_main(at_para);
    }
#endif
#if defined(CONFIG_MBOX)
    else if (strstr((const char *)_mc_name, "MBOX_")) {
        TEST_CASE_START();
        test_mbox_main(at_para);
    }
#endif
#if defined(CONFIG_EFUSE)
    else if (strstr((const char *)_mc_name, "EFUSE")) {
        TEST_CASE_START();
        test_efuse_main(at_para);
    }
#endif
#if defined(CONFIG_AES)
    else if (strstr((const char *)_mc_name, "AES_")) {
        TEST_CASE_START();
        test_aes_main(at_para);
    }
#endif
#if defined(CONFIG_SHA)
    else if (strstr((const char *)_mc_name, "SHA_")) {
        TEST_CASE_START();
        test_sha_main(at_para);
    }
#endif
#if defined(CONFIG_RSA)
    else if (strstr((const char *)_mc_name, "RSA_")) {
        TEST_CASE_START();
        test_rsa_main(at_para);
    }
#endif
#if defined(CONFIG_TRNG)
    else if (strstr((const char *)_mc_name, "TRNG_")) {
        TEST_CASE_START();
        test_trng_main(at_para);
    }
#endif
#if defined(CONFIG_PIN)
    else if (strstr((const char *)_mc_name, "PIN_")) {
        TEST_CASE_START();
        test_pin_main(at_para);
    }
#endif
#if defined(CONFIG_PWRMNG)
    else if (strstr((const char *)_mc_name, "PM_")) {
        TEST_CASE_START();
        test_pm_main(at_para);
    }
#endif
#if defined(CONFIG_TICK)
    else if (strstr((const char *)_mc_name, "TICK_")) {
        TEST_CASE_START();
        test_tick_main(at_para);
    }
#endif
#if defined(CONFIG_TIPC)
    else if (strstr((const char *)_mc_name, "TIPC_")) {
        TEST_CASE_START();
        test_tipc_main(at_para);
    }
#endif 
#if defined(CONFIG_INTNEST)
    else if (strstr((const char *)_mc_name, "INTNEST_")) {
        TEST_CASE_START();
        test_intnest_main(at_para);
    }
#endif

#if defined(CONFIG_BAUD_CALC)
    else if (strstr((const char *)_mc_name, "BAUDCALC")) {
        TEST_CASE_START();
        test_baudcalc_main(at_para);
    }
#endif

#if defined(CONFIG_SASC)
    else if (strstr((const char *)_mc_name, "SASC_")) {
        TEST_CASE_START();
        test_sasc_main(at_para);
    }
#endif


#if defined(CONFIG_GCOV)
	else if (strstr((const char *)_mc_name, "GCOV")) {
    	extern void ck_gcov_exit(void);

    	ck_gcov_exit();
	}
#endif

	else {
		TEST_CASE_TIPS("don't supported this AT command");
		return -1;
	}
	TEST_CASE_RESULT();
	return 0;
}

void test_init(void)
{
    int32_t ret;

	ret = tst_init(NULL);

	if (ret != 0) {
		while (1);
	}


	while (1) {
		char *receive = NULL;

		do {
			receive = atserver_get_test_cmdline();
		}while (receive == NULL);

		TEST_CASE_TIPS("%s", receive);
		driver_mc_name_parsing(receive);

        _mc_name[0] = '\0';
    }

}
