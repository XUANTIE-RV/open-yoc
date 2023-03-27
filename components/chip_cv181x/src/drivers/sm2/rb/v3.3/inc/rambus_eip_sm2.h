/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef INC_RAMBUS_EIP_SM2_H
#define INC_RAMBUS_EIP_SM2_H
#include "sfxlpka.h"
#include "rambus.h"
#include "eip28.h"

EIP28_Status_t eip_sm2_sign(unsigned long addr, SfxlPKA_Vector_t *d,
                            SfxlPKA_Vector_t *e, SfxlPKA_Vector_t *k,
                            SfxlPKA_Vector_t *r);
EIP28_Status_t eip_sm2_verify(unsigned long addr, SfxlPKA_Vector_t *pub,
                              SfxlPKA_Vector_t *e, SfxlPKA_Vector_t *sig);

#endif