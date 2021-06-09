/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <gpio_test.h>

test_func_map_t gpio_test_funcs_map[] = {
    {"GPIO_PIN_WRITE", test_gpio_pinWrite},
    {"GPIO_PINS_WRITE", test_gpio_pinsWrite},
    {"GPIO_PIN_DEBONCE_WRITE", test_gpio_pinDebonceWrite},
    {"GPIO_PINS_DEBONCE_WRITE", test_gpio_pinsDebonceWrite},
    {"GPIO_TOGGLE", test_gpio_toggle},
    {"GPIO_DEBONCE_TOGGLE", test_gpio_debonceToggle},
    {"GPIO_PIN_READ", test_gpio_pinRead},
    {"GPIO_DEBONCE_PIN_READ", test_gpio_deboncePinRead},
    {"GPIO_PINS_READ", test_gpio_pinsRead},
    {"GPIO_DEBONCE_PINS_READ", test_gpio_deboncePinsRead},
    {"GPIO_PINS_TOGGLE_READ", test_gpio_pinsToggleRead},
    {"GPIO_DEBONCE_PINS_TOGGLE_READ", test_gpio_deboncePinsToggleRead},
    {"GPIO_INTERRUPT_CAPTURE", test_gpio_interruptCapture},
    {"GPIO_INTERRUPT_TRIGGER", test_gpio_interruptTrigger},
};

int test_gpio_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(gpio_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, gpio_test_funcs_map[i].test_func_name)) {
            (*(gpio_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("GPIO module don't support this command.");
    return -1;
}
