/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sec_crypto_platform.h>

#include <sec_crypto_errcode.h>

enum region_access_perm {
    RAP_ONLY_X = 0,
    RAP_X,
    RAP_R,
    RAP_W
};

int sc_srv_memprot(uint32_t region_addr, int region_size, enum region_access_perm rap);
