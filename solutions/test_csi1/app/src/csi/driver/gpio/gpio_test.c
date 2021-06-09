#include <gpio_test.h>

test_func_info_t gpio_test_funcs_map[] = {
		{"GPIO_INTERFACE",test_gpio_interface,1},
		{"GPIO_PIN_WRITE",test_gpio_pinWrite,4},
//		{"GPIO_PINS_WRITE",test_gpio_pinsWrite,4},
		{"GPIO_PIN_READ",test_gpio_pinRead,3},
//		{"GPIO_PINS_READ",test_gpio_pinsRead,3},
		{"GPIO_INTERRUPT_CAPTURE",test_gpio_interrupt_Capture,3},
		{"GPIO_INTERRUPT_TRIGGER",test_gpio_interrupt_Trigger,4}
};


int test_gpio_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;


	for(i=0;i<sizeof(gpio_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, gpio_test_funcs_map[i].name)){
			args_value = malloc(sizeof(uint32_t)*gpio_test_funcs_map[i].args_num);
			if (args_value == NULL){
				TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
			}
			ret = args_parsing(args, (uint32_t *)args_value, gpio_test_funcs_map[i].args_num);
			if (ret != 0){
				free(args_value);
				TEST_CASE_WARN_QUIT("parameter resolution error");
			}
			(*(gpio_test_funcs_map[i].function))(args_value);
			free(args_value);
			return 0;
		}
	}

	TEST_CASE_TIPS("gpio module don't support this command.");
	return -1;
}

