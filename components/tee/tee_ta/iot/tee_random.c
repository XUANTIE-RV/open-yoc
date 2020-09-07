/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_msg_cmd.h>
#include <crypto.h>

#if (CONFIG_ALGO_TRNG > 0)
int tee_core_random(tee_param params[4])
{
    tee_generaterandom(params[0].memref.buffer, params[0].memref.size);
    return 0;
}
#endif

