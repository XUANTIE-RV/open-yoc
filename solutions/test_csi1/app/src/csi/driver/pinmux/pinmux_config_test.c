#include <pinmux_test.h>

/**
	\brief		Configure the pins used by the test module.
	\param[in]	pins	pinmux config data,\ref pin_config_t.
	\return		None.

*/
int test_pinmux_config(void *args)
{
	int32_t ret;

	uint8_t pin_name, pin_func;

	pin_name = (uint8_t)*((uint32_t *)args);
	pin_func = (uint8_t)*((uint32_t *)args+1);

	ret = drv_pinmux_config(pin_name, pin_func);
	TEST_CASE_ASSERT_QUIT(ret == 0, "when pin is %d, pin_func is %d; pinmux config error", pin_name, pin_func);
	
	return 0;
}
