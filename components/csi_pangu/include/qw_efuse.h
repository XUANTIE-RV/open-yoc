/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     qw_efuse.h
 * @brief    header file for efuse driver
 * @version  V1.2
 * @date     27. May 2019
 ******************************************************************************/
#ifndef _QW_EFUSE_H_
#define _QW_EFUSE_H_

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_KEY_BIT_SIZE_256    (256)
#define AES_KEY_BIT_SIZE_192    (192)
#define AES_KEY_BIT_SIZE_128    (128)
#define BYTE_PER_WORD           (4)
#define BIT_PER_BYTE            (8)
#define BIT_PER_WORD            (32)
#define PRG_RETRY_TIMES         (16)
#define RD_RETRY_TIMES          (16)

#define QW_EFUSE_RD_FINISH      (0x00010000)
#define QW_EFUSE_PRG_FINISH     (0x01000000)
#define QW_EFUSE_RD_WORD(num)   (num << 0)
#define QW_EFUSE_PRG_WORD(num)  (num << 16)
#define QW_EFUSE_PRG_BIT(num)   (num << 22)
#define QW_EFUSE_RD_START_APB   (0x1)
#define QW_EFUSE_PRG_START_APB  (0x10000)

typedef struct {
    __IOM uint32_t EFUSE_CTRL;  /* Offset: 0x000h (R/W)  EFUSE CTRL Register */
    __IOM uint32_t EFUSE_STATUS0;/* Offset: 0x004h (R/W)  EFUSE STATUS0 Register */
    __IOM uint32_t EFUSE_STATUS1;/* Offset: 0x008h (R/W)  EFUSE STATUS1 Register */
    __IOM uint32_t EFUSE_MARGIN_ADDR;/* Offset: 0x00ch (R/W)  EFUSE MARGIN Register */
    __IOM uint32_t EFUSE_DATA;/* Offset: 0x010h (R/W)  EFUSE DATA Register */
    __IOM uint32_t EFUSE_PRG_CTL0;/* Offset: 0x014h (R/W)  EFUSE PROGRAM CTL0 Register */
    __IOM uint32_t EFUSE_PRG_CTL1;/* Offset: 0x018h (R/W)  EFUSE PROGRAM CTL1 Register */
    __IOM uint32_t EFUSE_PRG_CTL2;/* Offset: 0x01ch (R/W)  EFUSE PROGRAM CTL2 Register */
    __IOM uint32_t EFUSE_PRG_CTL3;/* Offset: 0x020h (R/W)  EFUSE PROGRAM CTL3 Register */
    __IOM uint32_t EFUSE_PRG_CTL4;/* Offset: 0x024h (R/W)  EFUSE PROGRAM CTL4 Register */
    __IOM uint32_t EFUSE_PRG_CTL5;/* Offset: 0x028h (R/W)  EFUSE PROGRAM CTL5 Register */
    __IOM uint32_t EFUSE_PRG_CTL6;/* Offset: 0x02ch (R/W)  EFUSE PROGRAM CTL6 Register */
    __IOM uint32_t EFUSE_PRG_CTL7;/* Offset: 0x030h (R/W)  EFUSE PROGRAM CTL7 Register */
    __IOM uint32_t EFUSE_PRG_CTL8;/* Offset: 0x034h (R/W)  EFUSE PROGRAM CTL8 Register */
    __IOM uint32_t EFUSE_RD_CTL0;/* Offset: 0x038h (R/W)  EFUSE READ CTL0 Register */
    __IOM uint32_t EFUSE_RD_CTL1;/* Offset: 0x03ch (R/W)  EFUSE READ CTL1 Register */
    __IOM uint32_t EFUSE_RD_CTL2;/* Offset: 0x040h (R/W)  EFUSE READ CTL2 Register */
    __IOM uint32_t EFUSE_RD_CTL3;/* Offset: 0x044h (R/W)  EFUSE READ CTL3 Register */
    __IOM uint32_t EFUSE_RD_CTL4;/* Offset: 0x04ch (R/W)  EFUSE READ CTL4 Register */
    __IOM uint32_t EFUSE_RD_CTL5;/* Offset: 0x050h (R/W)  EFUSE READ CTL5 Register */
} qw_efuse_reg_t;

/* definition for efuse handle. */
typedef void *efuse_handle_t;

/*----- QW TX216's specific config type -----*/
typedef enum {
    JTAG_DIS,       ///< JTAG disable
    SECURE_BOOT,    ///< Secure boot enable
    KEY_BURNED,     ///< The flag to indicate whether key has been burned
    SECT1_PROTECT,  ///< Section1 protect
    SECT3_PROTECT,  ///< Section2 protect
} efuse_cfg_e;

/**
  \brief       Initialize EFUSE Interface. 1. Initializes the resources needed for the EFUSE
  \return      pointer to efuse instances
*/
efuse_handle_t qw_efuse_initialize(void);

/**
  \brief       De-initialize EFUSE Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  efuse handle to operate.
  \return      error code
*/
int32_t qw_efuse_uninitialize(efuse_handle_t handle);

/**
  \brief       Reload eFuse. AES module will reload key from efuse after reload.
  \param[in]   handle  efuse handle to operate
  \return      error code
*/
int32_t qw_efuse_reload(efuse_handle_t handle);

/**
  \brief       Write EFUSE Interface. write only one bit to efuse(size is 32 words)
  \param[in]   handle  efuse handle to operate.
  \param[in]   word    index from 0 to 31.
  \param[in]   bit     index from 0 to 31.
  \return      error code
*/
int32_t qw_efuse_set_bit(efuse_handle_t handle, uint32_t word, uint32_t bit);

/**
  \brief       Read EFUSE Interface By Bit. write only one bit to efuse(size is 32 words)
  \param[in]   handle  efuse handle to operate.
  \param[out]  val     to store output bit value.
  \param[in]   word    word index from 0 to 31.
  \param[in]   bit     bit index from 0 to 31.
  \return      error code
*/
int32_t qw_efuse_get_bit(efuse_handle_t handle, bool *val, uint32_t word, uint32_t bit);

/**
  \brief       Read a word from EFUSE.
  \param[in]   handle  efuse handle to operate.
  \param[in]   val     to store word value.
  \param[in]   word    index from 0 to 31.
  \return      error code
*/
int32_t qw_efuse_read_word(efuse_handle_t handle, uint32_t *val, uint32_t word);

/**
  \brief       Write Key. write AES key to efuse
  \param[in]   handle  efuse handle to operate.
  \param[in]   key     the data of key.
  \param[in]   key_len the length of key.
  \return      error code
*/
int32_t qw_efuse_write_key(efuse_handle_t handle, char *key, uint32_t key_len);

/**
  \brief       Read key Interface. read AES key from efuse
  \param[in]   handle  efuse handle to operate
  \param[out]  key     output key buffer.
  \param[in]   key_len the length of key.
  \return      error code
*/
int32_t qw_efuse_read_key(efuse_handle_t handle, char *key, uint32_t key_len);

/**
  \brief       Set configuration stores in efuse
  \param[in]   handle efuse handle to operate.
  \param[in]   type   confiuration type.
  \return      error code
*/
int32_t qw_efuse_set_config(efuse_handle_t handle, efuse_cfg_e type);

/**
  \brief       Get configuration stores in efuse
  \param[in]   handle efuse handle to operate.
  \param[out]  status the configuration status
  \param[in]   type   confiuration type
  \return      error code
*/
int32_t qw_efuse_get_config(efuse_handle_t handle, bool *status, efuse_cfg_e type);

#ifdef __cplusplus
}
#endif

#endif /* _QW_EFUSE_H_ */
