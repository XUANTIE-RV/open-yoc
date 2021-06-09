/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef __SEC_CRYPTO_COMMON_H__
#define __SEC_CRYPTO_COMMON_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sec_crypto_rsa.h"

#define SC_lOG(...)

#define CHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)

#endif

#define CHECK_RET_WITH_RET(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)



void sc_common_set_key_bits(sc_rsa_t *rsa, sc_rsa_context_t *context);