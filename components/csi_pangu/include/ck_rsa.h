/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_rsa.h
 * @brief    header file for rsa driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_RSA_H_
#define _CK_RSA_H_

#include <stdio.h>
#include "drv/rsa.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RSA_KEY_LEN     2048
#define RSA_KEY_BYTE    (RSA_KEY_LEN >> 3)
#define RSA_KEY_WORD    (RSA_KEY_LEN >> 5)

#define BN_MAX_BITS     ((RSA_KEY_LEN << 1) + 32)
#define BN_MAX_BYTES    ((BN_MAX_BITS + 7) >> 3)
#define BN_MAX_WORDS    ((BN_MAX_BYTES + 3) >> 2)

#define MAX_RSA_LP_CNT  10000

#define GET_KEY_BYTE(k) (k >> 3)
#define GET_KEY_WORD(k) (k >> 5)

#define UINT32_TO_UINT64(data)     ((uint64_t)(((uint64_t)(data)) & 0x00000000ffffffffU))
#define UINT64L_TO_UINT32(data)    ((uint32_t)(((uint64_t)(data)) & 0x00000000ffffffffU))
#define UINT64H_TO_UINT32(data)    ((uint32_t)((((uint64_t)(data)) >> 32) & 0x00000000ffffffffU))

#define PKCS1_PADDING   0x01
#define NO_PADDING      0x02

#define MD5_PADDING     0x00
#define SHA1_PADDING    0x01
#define SHA256_PADDING  0x03

#define MD5_HASH_SZ    16
#define SHA1_HASH_SZ   20
#define SHA256_HASH_SZ 32

#define RAS_CALCULATE_Q     0x6
#define RSA_ENABLE_MODULE   0x3
#define RSA_ENDIAN_MODE     0x8
#define RSA_RESET           0x1
#define RSA_CAL_Q_DONE_OFFSET   0x5

typedef struct bignum {
    uint32_t pdata[BN_MAX_WORDS];
    uint32_t words;
} bignum_t;

typedef struct {
    __IOM uint32_t RSA_MWID;          /* Offset: 0x000 (R/W)  Width of M register */
    __IOM uint32_t RSA_CKID;          /* Offset: 0x004 (R/W)  Width of D register */
    __IOM uint32_t RSA_BWID;          /* Offset: 0x008 (R/W)  Width of B register */
    __IOM uint32_t RSA_CTRL;          /* Offset: 0x00c (R/W)  RSA control register */
    __OM  uint32_t RSA_RST;           /* Offset: 0x010 (W)    RSA reset register */
    __IM  uint32_t RSA_LP_CNT;        /* Offset: 0x014 (R)    Loop counter for inquiry register*/
    __IM  uint32_t RSA_Q0;            /* Offset: 0x018 (R)    High-radix MM algorithm assistant register,part 1*/
    __IM  uint32_t RSA_Q1;            /* Offset: 0x01c (R)    High-radix MM algorithm assistant register,part 2*/
    __IOM uint32_t RSA_ISR;           /* Offset: 0x020 (W/R)  Interrupt raw status register */
    __IOM uint32_t RSA_IMR;           /* Offset: 0x024 (W/R)  Interrupt mask register */
    __IOM uint32_t REV1[54];          /* Reserve regiser */
    __IOM uint32_t RSA_RFM;           /* Offset: 0x100 (W/R)  Register file for modulus M */
    __IOM uint32_t REV2[63];          /* Reserve regiser */
    __IOM uint32_t RSA_RFD;           /* Offset: 0x200 (W/R)  Register file for exponent D */
    __IOM uint32_t REV3[63];          /* Reserve regiser */
    __IOM uint32_t RSA_RFC;           /* Offset: 0x300 (W/R)  Register file for hard C */
    __IOM uint32_t REV4[63];          /* Reserve regiser */
    __IOM uint32_t RSA_RFB;           /* Offset: 0x400 (W/R)  Register file for data B */
    __IOM uint32_t REV5[63];          /* Reserve regiser */
    __IM  uint32_t RSA_RFR;           /* Offset: 0x500 (R)    Register file for storing the result */
} ck_rsa_reg_t;

#ifdef __cplusplus
}
#endif

#endif
