/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sec_crypto_platform.h>


int sc_get_bootkey(uint8_t *bk, int bk_size);
int sc_verify_bootkey(uint8_t *bk, int bk_size, uint8_t *bk_hash);
