/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __PIN_TEST__
#define __PIN_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>
#include <drv/pin.h>

typedef struct {
    uint32_t    idx;    //参考pin_name_t
    uint32_t    func;   //参考pin_func_t
    uint32_t    mode;   //参考csi_pin_mode_t
    uint32_t    speed;  //参考csi_pin_speed_t
    uint32_t    drive;  //参考csi_pin_drive_t
}test_pin_args_t;


extern int test_pin_config(char *args);

#endif
