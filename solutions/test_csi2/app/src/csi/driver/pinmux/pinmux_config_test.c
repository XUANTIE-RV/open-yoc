/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <drv/pin.h>
#include <pinmux_test.h>

int test_pinmux_config(test_pinmux_args_t *pins)
{
    int32_t ret;

    ret = csi_pin_set_mux(pins->pin, pins->pin_func);
    TEST_CASE_ASSERT_QUIT(ret == 0, "when pin is %d, pin_func is %d; pinmux config error", pins->pin, pins->pin_func);


    return 0;
}

