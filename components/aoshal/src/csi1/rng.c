/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include "drv/trng.h"
#include "pin_name.h"
#include "pinmux.h"
#include "aos/hal/rng.h"
#include "string.h"

int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
    trng_handle_t trng;

    trng = csi_trng_initialize(random.port, NULL);

    return csi_trng_get_data(trng, buf, bytes);
}
