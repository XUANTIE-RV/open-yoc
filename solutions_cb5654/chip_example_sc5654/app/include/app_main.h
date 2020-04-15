/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <hw_config.h>
#include <app_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/cli_cmd.h>
#include <vfs.h>

#include <pin_name.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <pinmux.h>
#include <aos/hal/adc.h>
#include <devices/devicelist.h>

#include <yoc/yoc.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

void yoc_base_init(void);

#endif