/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_aes_tx216.h
 * @brief    header file for aes driver
 * @version  V1.2
 * @date     22. May 2019
 ******************************************************************************/
#ifndef _CK_AES_TX216_H_
#define _CK_AES_TX216_H_

#include <stdio.h>
#include <io.h>
#include <soc.h>
#include <drv/aes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BIG_ENDIAN_MODE   BIT(8)

#define AES_MAX_KEY_LENGTH  32
#define AES_IT_DATAINT      0x4
#define AES_IT_KEYINT       0x2
#define AES_IT_BUSY         0x1
#define AES_IT_ALL          0x7
#define AES_CRYPTO_KEYEXP   0x2

#define AES_WORK_ENABLE_OFFSET  0
#define AES_INT_ENABLE_OFFSET   2
#define AES_MODE_OFFSET         3
#define AES_KEY_LEN_OFFSET      4
#define AES_OPCODE_OFFSET       6

typedef enum {
    AES_KEY_MODE_SOFTWARE = 0x00,
    AES_KEY_MODE_HARDWARE = 0x01,
} aes_key_mode_e;

typedef struct {
    __IOM uint32_t DATAIN[4];                 /* Offset: 0x000 (R/W) Data input 0~127 */
    __IOM uint32_t KEY[8];                    /* Offset: 0x010 (R/W) Key 0~255 */
    __IOM uint32_t IV[4];                     /* Offset: 0x030 (R/W) Initial Vector: 0~127 */
    __IOM uint32_t CTRL;                      /* Offset: 0x040 (R/W) AES Control Register */
    __IOM uint32_t STATE;                     /* Offset: 0x044 (R/W) AES State Register */
    __IOM uint32_t DATAOUT[4];                /* Offset: 0x048 (R/W) Data Output 0~31 */
    __IOM uint32_t KEY_MODE;                  /* Offset: 0x058 (R/W) Auto get key from EFuse */
} ck_aes_reg_t;

/**
  \brief       Set the key source mode
  \param[in]   handle  aes handle to operate.
  \param[out]  mode    aes key source mode.
  \return      error code
*/
int32_t drv_aes_get_key_mode(aes_handle_t handle, aes_key_mode_e *mode);

/**
  \brief       Set the key source mode
  \param[in]   handle  aes handle to operate.
  \param[in]   mode    aes key source mode.
  \return      error code
*/
int32_t drv_aes_set_key_mode(aes_handle_t handle, aes_key_mode_e mode);

#ifdef __cplusplus
}
#endif

#endif
