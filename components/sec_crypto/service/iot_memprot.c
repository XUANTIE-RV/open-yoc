/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sec_crypto_memprot.h>



int sc_srv_memprot(uint32_t region_addr, int region_size, enum region_access_perm rap)
{
    (void)region_addr;
    (void)region_size;
    (void)rap;
    return SC_OK;
}
