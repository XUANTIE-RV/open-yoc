/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef __SC_SEC_MCU_H_
#define __SC_SEC_MCU_H_

#include <sec_crypto_platform.h>
#include <sec_crypto_memprot.h>
#include <sec_crypto_aes.h>
#include <sec_crypto_sha.h>
#include <sec_crypto_rsa.h>


int sc_mpu_set_memory_attr(uint32_t region_addr, int region_size, enum region_access_perm rap);

#endif