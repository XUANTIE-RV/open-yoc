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

#ifndef CHECK_PARAM
#define CHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)

#endif /* CHECK_PARAM */

#ifndef CHECK_RET_WITH_RET
#define CHECK_RET_WITH_RET(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)
#endif /* CHECK_RET_WITH_RET */


void sc_common_set_key_bits(sc_rsa_t *rsa, sc_rsa_context_t *context);
#endif /* __SEC_CRYPTO_COMMON_H__ */