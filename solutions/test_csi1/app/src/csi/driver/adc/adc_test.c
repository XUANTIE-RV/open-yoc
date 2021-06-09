#include <adc_test.h>

#if 0
AT+ADC_***=s_time,continue_mode,freq_div,max,min,ids
s_time：采样时间<br/>
continue_mode：0-禁用连续模式，1-启用连续模式<br/>
freq_div：ADC分频<br/>
max：预期最大值<br/>
min：预期最小值<br/>
ids：设备号


#endif

int test_adc_main(char *args)
{	
	char *case_name[] = {
		"ADC_FUNC",
        "ADC_INTERFACE",
	};

	int (*case_func[])(char *args) = {
		test_adc_func,
        test_adc_interface,
	};

	uint8_t i = 0;
	

    for (i=0; i<sizeof(case_name)/sizeof(char *); i++) {
        if (!strcmp((void *)_mc_name, case_name[i])) {
            case_func[i](args);
            return 0;
        }
    }

    TEST_CASE_TIPS("ADC don't supported this command");
    return -1;
}
