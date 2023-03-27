/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef INCLUDE_GUARD_BASIC_DEFS_H
#define INCLUDE_GUARD_BASIC_DEFS_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <rambus_log.h>

#ifndef NULL
#define NULL 0
#endif

/*----------------------------------------------------------------------------
*Description: warning for side-effects on the following two macros since the arguments
* are evaluated twice changing this to inline functions is problematic
* because of type incompatibilities
*/
#define MIN(_x, _y) ((_x) < (_y) ? (_x) : (_y))
#define MAX(_x, _y) ((_x) > (_y) ? (_x) : (_y))


/*----------------------------------------------------------------------------
* using postfix "U" to be compatible with uitn32
* ("UL" is not needed and gives lint warning)
*/
#define BIT_0 (0x00000001U)
#define BIT_1 (0x00000002U)
#define MASK_1_BIT (BIT_1 - 1)

#define BIT_2 (0x00000004U)
#define MASK_2_BITS (BIT_2 - 1)

#define BIT_3 (0x00000008U)
#define MASK_3_BITS (BIT_3 - 1)

#define BIT_4 (0x00000010U)
#define MASK_4_BITS (BIT_4 - 1)

#define BIT_5 (0x00000020U)
#define MASK_5_BITS (BIT_5 - 1)

#define BIT_6 (0x00000040U)
#define MASK_6_BITS (BIT_6 - 1)

#define BIT_7 (0x00000080U)
#define MASK_7_BITS (BIT_7 - 1)

#define BIT_8 (0x00000100U)
#define MASK_8_BITS (BIT_8 - 1)

#define BIT_9 (0x00000200U)
#define MASK_9_BITS (BIT_9 - 1)

#define BIT_10 (0x00000400U)
#define MASK_10_BITS (BIT_10 - 1)

#define BIT_11 (0x00000800U)
#define MASK_11_BITS (BIT_11 - 1)

#define BIT_12 (0x00001000U)
#define MASK_12_BITS (BIT_12 - 1)

#define BIT_13 (0x00002000U)
#define MASK_13_BITS (BIT_13 - 1)

#define BIT_14 (0x00004000U)
#define MASK_14_BITS (BIT_14 - 1)

#define BIT_15 (0x00008000U)
#define MASK_15_BITS (BIT_15 - 1)

#define BIT_16 (0x00010000U)
#define MASK_16_BITS (BIT_16 - 1)

#define BIT_17 (0x00020000U)
#define MASK_17_BITS (BIT_17 - 1)

#define BIT_18 (0x00040000U)
#define MASK_18_BITS (BIT_18 - 1)

#define BIT_19 (0x00080000U)
#define MASK_19_BITS (BIT_19 - 1)

#define BIT_20 (0x00100000U)
#define MASK_20_BITS (BIT_20 - 1)

#define BIT_21 (0x00200000U)
#define MASK_21_BITS (BIT_21 - 1)

#define BIT_22 (0x00400000U)
#define MASK_22_BITS (BIT_22 - 1)

#define BIT_23 (0x00800000U)
#define MASK_23_BITS (BIT_23 - 1)

#define BIT_24 (0x01000000U)
#define MASK_24_BITS (BIT_24 - 1)

#define BIT_25 (0x02000000U)
#define MASK_25_BITS (BIT_25 - 1)

#define BIT_26 (0x04000000U)
#define MASK_26_BITS (BIT_26 - 1)
#define BIT_27 (0x08000000U)
#define MASK_27_BITS (BIT_27 - 1)
#define BIT_28 (0x10000000U)
#define MASK_28_BITS (BIT_28 - 1)
#define BIT_29 (0x20000000U)
#define MASK_29_BITS (BIT_29 - 1)
#define BIT_30 (0x40000000U)
#define MASK_30_BITS (BIT_30 - 1)
#define BIT_31 (0x80000000U)
#define MASK_31_BITS (BIT_31 - 1)
/* ============ MASK_n_BITS ============ */


#define IDENTIFIER_NOT_USED(_v) (void)_v

#define __IOM volatile

typedef  uint32_t cvi_uint32_type

typedef struct {
        __IOM cvi_uint32_type TRNG_INPUT_0;   //00
        __IOM cvi_uint32_type TRNG_INPUT_1;   //04
        __IOM cvi_uint32_type TRNG_INPUT_2;   //08
        __IOM cvi_uint32_type TRNG_INPUT_3;   //0c
        __IOM cvi_uint32_type TRNG_STATUS;    //10
        __IOM cvi_uint32_type TRNG_CONTROL;   //14
        __IOM cvi_uint32_type TRNG_CONFIG;    //18
        __IOM cvi_uint32_type TRNG_ALARMCNT;  //1c
        __IOM cvi_uint32_type TRNG_FROENABLE; //20
        __IOM cvi_uint32_type TRNG_FRODETUNE; //24
        __IOM cvi_uint32_type TRNG_ALARMMASK; //28
        __IOM cvi_uint32_type TRNG_ALARMSTOP; //2c
        __IOM cvi_uint32_type TRNG_RAW_L;     //30
        __IOM cvi_uint32_type TRNG_RAW_H;     //34
        __IOM cvi_uint32_type TRNG_SPB_TESTS; //38
        __IOM cvi_uint32_type TRNG_COUNT;     //3c
        __IOM cvi_uint32_type TRNG_PS_AI_0;   //40
        __IOM cvi_uint32_type TRNG_PS_AI_1;   //44
        __IOM cvi_uint32_type TRNG_PS_AI_2;   //48
        __IOM cvi_uint32_type TRNG_PS_AI_3;   //4c
        __IOM cvi_uint32_type TRNG_PS_AI_4;   //50
        __IOM cvi_uint32_type TRNG_PS_AI_5;   //54
        __IOM cvi_uint32_type TRNG_PS_AI_6;   //58
        __IOM cvi_uint32_type TRNG_PS_AI_7;   //5c
        __IOM cvi_uint32_type TRNG_PS_AI_8;   //60
        __IOM cvi_uint32_type TRNG_PS_AI_9;   //64
        __IOM cvi_uint32_type TRNG_PS_AI_10;  //68
        __IOM cvi_uint32_type TRNG_PS_AI_11;  //6c
        __IOM cvi_uint32_type TRNG_TEST;      //70
        __IOM cvi_uint32_type TRNG_BLOCKCNT;  //74
        __IOM cvi_uint32_type TRNG_OPTIONS;   //78
        __IOM cvi_uint32_type TRNG_EIP_REV;   //7c
} eip150_trng_reg_t;

typedef struct {
        __IOM cvi_uint32_type PKA_APTR;      //00
        __IOM cvi_uint32_type PKA_BPTR;      //04
        __IOM cvi_uint32_type PKA_CPTR;      //08
        __IOM cvi_uint32_type PKA_DPTR;      //0c
        __IOM cvi_uint32_type PKA_ALENGTH;   //10
        __IOM cvi_uint32_type PKA_BLENGTH;   //14
        __IOM cvi_uint32_type PKA_SHIFT;     //18
        __IOM cvi_uint32_type PKA_FUNCTION;  //1c
        __IOM cvi_uint32_type PKA_COMPARE;   //20
        __IOM cvi_uint32_type PKA_MSW;       //24
        __IOM cvi_uint32_type PKA_DIVMSW;    //28
        __IOM cvi_uint32_type RESERVED0[1];  //2c
        __IOM cvi_uint32_type PKA_ISTA_CTRL; //30
        //__IOM cvi_uint32_type PKA_SEQFUNC;  //34
        //__IOM cvi_uint32_type PKA_SCAPCTRL; //30
        //__IOM cvi_uint32_type PKA_SEQFUNC;  //34
        __IOM cvi_uint32_type RESERVED1[35];
        __IOM cvi_uint32_type PKA_STATUS;     //c0
        __IOM cvi_uint32_type PKCP_PRIO_CTRL; //c4
        __IOM cvi_uint32_type PKA_SEQ_CTRL;   //c8
        __IOM cvi_uint32_type RESERVED2[1];
        __IOM cvi_uint32_type PKA_FW_OPTIONS; //d0
        __IOM cvi_uint32_type RESERVED3[7];
        __IOM cvi_uint32_type PKA_OPTIONS2; //f0
        __IOM cvi_uint32_type PKA_OPTIONS;  //f4
        __IOM cvi_uint32_type PKA_SW_REV;   //f8
        __IOM cvi_uint32_type PKA_REVISION; //fc
} eip150_pka_reg_t;

#define PKA_APTR (0x00)
#define PKA_BPTR (0x04)
#define PKA_CPTR (0x08)
#define PKA_DPTR (0x0c)
#define PKA_ALENGTH (0x10)
#define PKA_BLENGTH (0x14)
#define PKA_SHIFT (0x18)
#define PKA_FUNCTION (0x1c)
#define PKA_COMPARE (0x20)
#define PKA_MSW (0x24)
#define PKA_DIVMSW (0x28)
#define PKA_SCAPCTRL (0x30)
#define PKA_SEQFUNC (0x34)
#define PKA_STATUS (0xc0)
#define PKCP_PRIO_CTRL (0xc4)
#define PKA_SEQ_CTRL (0xc8)
#define PKA_FW_OPTIONS (0xd0)
#define PKA_OPTIONS2 (0xf0)
#define PKA_OPTIONS (0xf4)
#define PKA_SW_REV (0xf8)
#define PKA_REVISION (0xfc)

typedef struct {
        __IOM cvi_uint32_type AIC_POL_CTRL;     //00
        __IOM cvi_uint32_type AIC_TYPE_CTRL;    //04
        __IOM cvi_uint32_type AIC_ENABLE_CTRL;  //08
        __IOM cvi_uint32_type AIC_RAW_STAT;     //0c
        __IOM cvi_uint32_type AIC_ENABLED_STAT; //10
        __IOM cvi_uint32_type AIC_ENABLE_CLR;   //14
        __IOM cvi_uint32_type AIC_OPTIONS;      //18
        __IOM cvi_uint32_type AIC_VERSION;      //1c
} eip150_aic_reg_t;

typedef struct {
        __IOM cvi_uint32_type DMAC_CH0_CTRL0;       //0x000
        __IOM cvi_uint32_type DMAC_CH0_CTRL1;       //0x004
        __IOM cvi_uint32_type DMAC_CH0_SRCADDR;     //0x008
        __IOM cvi_uint32_type DMAC_CH0_SRCADDR_HI;  //0x00C
        __IOM cvi_uint32_type DMAC_CH0_DESTADDR;    //0x010
        __IOM cvi_uint32_type DMAC_CH0_DESTADDR_HI; //0x014
        __IOM cvi_uint32_type RESERVED0[2];
        __IOM cvi_uint32_type DMAC_CH0_DMALENGTH; //0x020
        __IOM cvi_uint32_type DMAC_STATUS;        //0x024
        __IOM cvi_uint32_type DMAC_SWRES;         //0x028
        __IOM cvi_uint32_type RESERVED1[5];
        __IOM cvi_uint32_type DMAC_CH1_CTRL0;       //0x040
        __IOM cvi_uint32_type DMAC_CH1_CTRL1;       //0x044
        __IOM cvi_uint32_type DMAC_CH1_SRCADDR;     //0x048
        __IOM cvi_uint32_type DMAC_CH1_SRCADDR_HI;  //0x04C
        __IOM cvi_uint32_type DMAC_CH1_DESTADDR;    //0x050
        __IOM cvi_uint32_type DMAC_CH1_DESTADDR_HI; //0x054
        __IOM cvi_uint32_type RESERVED2[2];
        __IOM cvi_uint32_type DMAC_CH1_DMALENGTH; //0x060
        __IOM cvi_uint32_type DMAC_ICR;           //0x064 //DMAC_ISR RO
        __IOM cvi_uint32_type DMAC_IMR;           //0x068
        __IOM cvi_uint32_type RESERVED3[14];
        __IOM cvi_uint32_type DMAC_ITR; //0x0A4 //DMAC_IRSR RO
        __IOM cvi_uint32_type DMAC_IER; //0x0A8
        __IOM cvi_uint32_type RESERVED4[14];
        __IOM cvi_uint32_type DMAC_MST_RUNPARAMS; //0x0E4
        __IOM cvi_uint32_type DMAC_PERSR;         //0x0E8
        __IOM cvi_uint32_type RESERVED5[14];
        __IOM cvi_uint32_type DMAC_IMS; //0x124
        __IOM cvi_uint32_type DMAC_IMC; //0x128
        __IOM cvi_uint32_type RESERVED6[36];
        __IOM cvi_uint32_type DMAC_OPTIONS2; //0x1BC
        __IOM cvi_uint32_type RESERVED7[14];
        __IOM cvi_uint32_type DMAC_OPTIONS; //0x1F8
        __IOM cvi_uint32_type DMAC_VERSION; //0x1FC

        __IOM cvi_uint32_type RESERVED8[64];
        __IOM cvi_uint32_type AIC_POL_CTRL;    //0x300
        __IOM cvi_uint32_type AIC_TYPE_CTRL;   //0x304
        __IOM cvi_uint32_type AIC_ENABLE_CTRL; //0x308
        __IOM cvi_uint32_type AIC_ENABLE_SET;  //0x30C AIC_RAW_STAT
        __IOM cvi_uint32_type AIC_ACK;         //0x310 //AIC_ENABLED_STAT RO
        __IOM cvi_uint32_type AIC_ENABLE_CLR;  //0x314
        __IOM cvi_uint32_type AIC_OPTIONS;     //0x318
        __IOM cvi_uint32_type AIC_VERSION;     //0x31C

        __IOM cvi_uint32_type RESERVED11[56];

        __IOM cvi_uint32_type KEY_STORE_WRITE_AREA;   //0x400
        __IOM cvi_uint32_type KEY_STORE_WRITTEN_AREA; //0x404
        __IOM cvi_uint32_type KEY_STORE_SIZE;         //0x408
        __IOM cvi_uint32_type KEY_STORE_READ_AREA;    //0x40C

        __IOM cvi_uint32_type RESERVED12[60];
        __IOM cvi_uint32_type CRYPTO_KEY2[4]; //0x500
        __IOM cvi_uint32_type CRYPTO_KEY3[4]; //0x510
        __IOM cvi_uint32_type RESERVED14[8];
        __IOM cvi_uint32_type CRYPTO_IV_IN_0;     //0x540	//CRYPTO_IV_OUT_0 RO
        __IOM cvi_uint32_type CRYPTO_IV_IN_1;     //0x544  //CRYPTO_IV_OUT_1 RO
        __IOM cvi_uint32_type CRYPTO_IV_IN_2;     //0x548  //CRYPTO_IV_OUT_2 RO
        __IOM cvi_uint32_type CRYPTO_IV_IN_3;     //0x54C  //RYPTO_IV_OUT_3 	RO
        __IOM cvi_uint32_type CRYPTO_CTRL;        //0x550
        __IOM cvi_uint32_type CRYPTO_C_LENGTH_0;  //0x554
        __IOM cvi_uint32_type CRYPTO_C_LENGTH_1;  //0x558
        __IOM cvi_uint32_type CRYPTO_AUTH_LENGTH; //0x55C
        __IOM cvi_uint32_type CRYPTO_DATA_IN_0;   //0x560  //CRYPTO_DATA_OUT_0	RO
        __IOM cvi_uint32_type CRYPTO_DATA_IN_1;   //0x564  //CRYPTO_DATA_OUT_1	RO
        __IOM cvi_uint32_type CRYPTO_DATA_IN_2;   //0x568  //CRYPTO_DATA_OUT_2 RO
        __IOM cvi_uint32_type CRYPTO_DATA_IN_3;   //0x56C  //CRYPTO_DATA_OUT_3	RO
        __IOM cvi_uint32_type CRYPTO_TAG_OUT_0;   //0x570
        __IOM cvi_uint32_type CRYPTO_TAG_OUT_1;   //0x574
        __IOM cvi_uint32_type CRYPTO_TAG_OUT_2;   //0x578
        __IOM cvi_uint32_type CRYPTO_TAG_OUT_3;   //0x57C
        __IOM cvi_uint32_type CRYPTO_BLK_CNT_0;   //0x580
        __IOM cvi_uint32_type CRYPTO_BLK_CNT_1;   //0x584
        __IOM cvi_uint32_type RESERVED15[30];

        __IOM cvi_uint32_type HASH_DATA_IN_0; //0x600
        __IOM cvi_uint32_type HASH_DATA_IN_1; //0x604
        __IOM cvi_uint32_type HASH_DATA_IN_2; //0x608
        __IOM cvi_uint32_type HASH_DATA_IN_3; //0x60C
        __IOM cvi_uint32_type HASH_DATA_IN_4; //0x610

        __IOM cvi_uint32_type HASH_DATA_IN_5;  //0x614
        __IOM cvi_uint32_type HASH_DATA_IN_6;  //0x618
        __IOM cvi_uint32_type HASH_DATA_IN_7;  //0x61C
        __IOM cvi_uint32_type HASH_DATA_IN_8;  //0x620
        __IOM cvi_uint32_type HASH_DATA_IN_9;  //0x624
        __IOM cvi_uint32_type HASH_DATA_IN_10; //0x628
        __IOM cvi_uint32_type HASH_DATA_IN_11; //
        __IOM cvi_uint32_type HASH_DATA_IN_12; //
        __IOM cvi_uint32_type HASH_DATA_IN_13; //
        __IOM cvi_uint32_type HASH_DATA_IN_14; //
        __IOM cvi_uint32_type HASH_DATA_IN_15; //0x63c
        __IOM cvi_uint32_type RESERVED16[(35 - 15)];
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_A; //690
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_B;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_C;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_D;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_E;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_F;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_G;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_H;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_I;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_J;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_K;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_L;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_M;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_N;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_O;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_P;
        __IOM cvi_uint32_type HASH_DIGEST_INOUT_SHA3;
        __IOM cvi_uint32_type RESERVED_D[3];
        __IOM cvi_uint32_type HASH_IO_BUF_CTRL; //HASH_IO_BUF_STAT 0x6E0
        __IOM cvi_uint32_type HASH_MODE_IN;
        __IOM cvi_uint32_type HASH_LENGTH_IN_L;
        __IOM cvi_uint32_type HASH_LENGTH_IN_H;
        __IOM cvi_uint32_type RESERVED17[(0x780 - 0x6EC - 4) / 4];
        __IOM cvi_uint32_type CTRL_ALG_SEL;   //0x780
        __IOM cvi_uint32_type CTRL_PROT_EN;   //0x784
        __IOM cvi_uint32_type CTRL_ERROR_CLR; //0x788
        __IOM cvi_uint32_type RESERVED18[1];
        __IOM cvi_uint32_type CTRL_ERROR_STAT; //0x790
        __IOM cvi_uint32_type RESERVED19[11];
        __IOM cvi_uint32_type CTRL_SW_RESET; //0x7C0
        __IOM cvi_uint32_type RESERVED20[13];
        __IOM cvi_uint32_type CTRL_OPTION;  //0x7F8
        __IOM cvi_uint32_type CTRL_VERSION; //0x7FC

} eip120si_reg_tt;

/* COMMON */
// #define CONFIG_ALG_PERF_TEST
//#define CONFIG_RB_SM2_RK
//#define CONFIG_RB_RAND_DEBUG
// #define CONFIG_RB_AES_DMA_MODE
// #define CONFIG_RB_SM4_DMA_MODE
#define CONFIG_RB_CACHE_ENABLE

#define EIP_DMA_MAX_LEN_IN_BYTE (1024 * 64 - 256)

#define EIP150_TRNG ((volatile eip150_trng_reg_t *)0xffff300000)

#define EIP150_SM2_P_OFFSET 0
#define EIP150_SM2_D_OFFSET (0x138 / 4)
#define EIP150_SM2_E_OFFSET (0x118 / 4)
#define EIP150_SM2_K_OFFSET (0x158 / 4)
#define EIP150_SM2_R_OFFSET (0x158 / 4)
#define EIP150_SM2_S_OFFSET (0x180 / 4)

/* verify */
#define EIP150_SM2_V_P_OFFSET 0
#define EIP150_SM2_V_PUB_OFFSET (0x180 / 4)
#define EIP150_SM2_V_E_OFFSET (0x200 / 4)
#define EIP150_SM2_V_S_OFFSET (0x280 / 4)

#define EIP28_FUNCTION_SEQ_SM2_SIGN (uint32_t)((0x04 << 12) | BIT_17)
#define EIP28_FUNCTION_SEQ_SM2_VERIFY (uint32_t)((0x05 << 12) | BIT_17)

#define EIP120_DMAC_CH0_CTRL0 0x000
#define EIP120_DMAC_CH0_CTRL1 0x004
#define EIP120_DMAC_CH0_SRCADDR 0x008
#define EIP120_DMAC_CH0_SRCADDR_HI 0x00C
#define EIP120_DMAC_CH0_DESTADDR 0x010
#define EIP120_DMAC_CH0_DESTADDR_HI 0x014
//#define EIP120_RESERVED0[2];
#define EIP120_DMAC_CH0_DMALENGTH 0x020
#define EIP120_DMAC_STATUS 0x024
#define EIP120_DMAC_SWRES 0x028
//#define EIP120_RESERVED1[5];
#define EIP120_DMAC_CH1_CTRL0 0x040
#define EIP120_DMAC_CH1_CTRL1 0x044
#define EIP120_DMAC_CH1_SRCADDR 0x048
#define EIP120_DMAC_CH1_SRCADDR_HI 0x04C
#define EIP120_DMAC_CH1_DESTADDR 0x050
#define EIP120_DMAC_CH1_DESTADDR_HI 0x054
//#define EIP120_RESERVED2[2];
#define EIP120_DMAC_CH1_DMALENGTH 0x060
#define EIP120_DMAC_ICR 0x064
#define EIP120_DMAC_IMR 0x068
//#define EIP120_RESERVED3[14];
#define EIP120_DMAC_ITR 0x0A4
#define EIP120_DMAC_IER 0x0A8
//#define EIP120_RESERVED4[14];
#define EIP120_DMAC_MST_RUNPARAMS 0x0E4
#define EIP120_DMAC_PERSR 0x0E8
// #define EIP120_RESERVED5[14];
#define EIP120_DMAC_IMS 0x124
#define EIP120_DMAC_IMC 0x128
//#define EIP120_RESERVED6[36];
#define EIP120_DMAC_OPTIONS2 0x1BC
//#define EIP120_RESERVED7[14];
#define EIP120_DMAC_OPTIONS 0x1F8
#define EIP120_DMAC_VERSION 0x1FC

//#define EIP120_RESERVED8[64];
#define EIP120_AIC_POL_CTRL 0x300
#define EIP120_AIC_TYPE_CTRL 0x304
#define EIP120_AIC_ENABLE_CTRL 0x308
#define EIP120_AIC_ENABLE_SET 0x30C
#define EIP120_AIC_ACK 0x310
#define EIP120_AIC_ENABLE_CLR 0x314
#define EIP120_AIC_OPTIONS 0x318
#define EIP120_AIC_VERSION 0x31C
// #define EIP120_RESERVED11[56];
#define EIP120_KEY_STORE_WRITE_AREA 0x400
#define EIP120_KEY_STORE_WRITTEN_AREA 0x404
#define EIP120_KEY_STORE_SIZE 0x408
#define EIP120_KEY_STORE_READ_AREA 0x40C

// #define EIP120_RESERVED12[60];
// #define EIP120_CRYPTO_KEY2[4]    0x500
// #define EIP120_CRYPTO_KEY3[4]    0x510
// #define EIP120_RESERVED14[8];
#define EIP120_CRYPTO_IV_IN_0 0x540
#define EIP120_CRYPTO_IV_IN_1 0x544
#define EIP120_CRYPTO_IV_IN_2 0x548
#define EIP120_CRYPTO_IV_IN_3 0x54C
#define EIP120_CRYPTO_CTRL 0x550
#define EIP120_CRYPTO_C_LENGTH_0 0x554
#define EIP120_CRYPTO_C_LENGTH_1 0x558
#define EIP120_CRYPTO_AUTH_LENGTH 0x55C
#define EIP120_CRYPTO_DATA_IN_0 0x560
#define EIP120_CRYPTO_DATA_IN_1 0x564
#define EIP120_CRYPTO_DATA_IN_2 0x568
#define EIP120_CRYPTO_DATA_IN_3 0x56C
#define EIP120_CRYPTO_TAG_OUT_0 0x570
#define EIP120_CRYPTO_TAG_OUT_1 0x574
#define EIP120_CRYPTO_TAG_OUT_2 0x578
#define EIP120_CRYPTO_TAG_OUT_3 0x57C
#define EIP120_CRYPTO_BLK_CNT_0 0x580
#define EIP120_CRYPTO_BLK_CNT_1 0x584
// #define EIP120_RESERVED15[30];

#define EIP120_HASH_DATA_IN_0 0x600
#define EIP120_HASH_DATA_IN_1 0x604
#define EIP120_HASH_DATA_IN_2 0x608
#define EIP120_HASH_DATA_IN_3 0x60C
#define EIP120_HASH_DATA_IN_4 0x610

#define EIP120_HASH_DATA_IN_5 0x614
#define EIP120_HASH_DATA_IN_6 0x618
#define EIP120_HASH_DATA_IN_7 0x61C
#define EIP120_HASH_DATA_IN_8 0x620
#define EIP120_HASH_DATA_IN_9 0x624
#define EIP120_HASH_DATA_IN_10 0x628
#define EIP120_HASH_DATA_IN_11 0x62C
#define EIP120_HASH_DATA_IN_12 0x630
#define EIP120_HASH_DATA_IN_13 0x634
#define EIP120_HASH_DATA_IN_14 0x638
#define EIP120_HASH_DATA_IN_15 0x63C
// #define EIP120_RESERVED16[(35 - 15)];
#define EIP120_HASH_DIGEST_INOUT_A 0x690
#define EIP120_HASH_DIGEST_INOUT_B 0x694
#define EIP120_HASH_DIGEST_INOUT_C 0x698
#define EIP120_HASH_DIGEST_INOUT_D 0x69C
#define EIP120_HASH_DIGEST_INOUT_E 0x6A0
#define EIP120_HASH_DIGEST_INOUT_F 0x6A4
#define EIP120_HASH_DIGEST_INOUT_G 0x6A8
#define EIP120_HASH_DIGEST_INOUT_H 0x6AC
// #define EIP120_HASH_DIGEST_INOUT_I;
// #define EIP120_HASH_DIGEST_INOUT_J;
// #define EIP120_HASH_DIGEST_INOUT_K;
// #define EIP120_HASH_DIGEST_INOUT_L;
// #define EIP120_HASH_DIGEST_INOUT_M;
// #define EIP120_HASH_DIGEST_INOUT_N;
// #define EIP120_HASH_DIGEST_INOUT_O;
// #define EIP120_HASH_DIGEST_INOUT_P;
// #define EIP120_HASH_DIGEST_INOUT_SHA3;
// // #define EIP120_RESERVED_D[3];
#define EIP120_HASH_IO_BUF_CTRL 0x6E0
#define EIP120_HASH_MODE_IN 0x6E4
#define EIP120_HASH_LENGTH_IN_L 0x6E8
#define EIP120_HASH_LENGTH_IN_H 0x6EC
// #define EIP120_RESERVED17[(0x780 - 0x6EC - 4) / 4];
#define EIP120_CTRL_ALG_SEL 0x780
#define EIP120_CTRL_PROT_EN 0x784
#define EIP120_CTRL_ERROR_CLR 0x788
// #define EIP120_RESERVED18[1];
#define EIP120_CTRL_ERROR_STAT 0x790
// #define EIP120_RESERVED19[11];
#define EIP120_CTRL_SW_RESET 0x7C0
// #define EIP120_RESERVED20[13];
#define EIP120_CTRL_OPTION 0x7F8
#define EIP120_CTRL_VERSION 0x7FC

/* 120 */
#define KEY_STORE_KEY_SIZE_128 0x00000001
#define KEY_STORE_KEY_SIZE_192 0x00000002
#define KEY_STORE_KEY_SIZE_256 0x00000003
#define KEY_STORE_AREA_0 BIT_0
#define KEY_STORE_AREA_1 BIT_1
#define KEY_STORE_AREA_7 BIT_7
#define KEY_STORE_READ_AREA_0 0
#define KEY_STORE_CLR BIT_31

#define ALG_SEL_KEY_STORE BIT_0
#define ALG_SEL_AES BIT_1
#define ALG_SEL_SHA256 BIT_2
#define ALG_SEL_SHA512 BIT_3
#define ALG_SEL_SM3 BIT_8
#define ALG_SEL_DES BIT_9
#define ALG_SEL_SM4 BIT_10

#define AES_DIR_BIT BIT_2
#define AES_BLOCK_SIZE_IN_BYTE 16

#define CACHE_BLOCK_SIZE 64

#define DMA_CH0_CTRL0 0x00000140
#define DMA_CH1_CTRL0 0x00004001

#define SM4_DIR_BIT BIT_2
#define SM4_BLOCK_SIZE_IN_BYTE 16

#endif