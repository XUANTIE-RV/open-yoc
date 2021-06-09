/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <sec_crypto_errcode.h>
#include "sec_crypto_common.h"
#include "sec_crypto_rsa.h"

void sc_common_set_key_bits(sc_rsa_t *rsa, sc_rsa_context_t *context)
{
        switch (rsa->bits) {
                case SC_RSA_KEY_BITS_1024:
                        context->key_bits = 1024;
                        break;
                case SC_RSA_KEY_BITS_2048:
                        context->key_bits = 2048;
                        break;
                /* set later */
                case 0:
                        break;
                default:
                        context->key_bits = 0;
                        return;
        }
}