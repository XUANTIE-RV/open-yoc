/*
* Copyright (C) 2017-2020 Alibaba Group Holding Limited
*/

#ifndef __SC_SOFTSE_H_
#define __SC_SOFTSE_H_
#include <stdint.h>
#include <sec_crypto_errcode.h>

#define CHECK_RET(r)                                \
    do {                                            \
        if(r != SC_OK)                             \
        {                                           \
            return( ret );                          \
        }                                           \
    } while( 0 )


#endif
