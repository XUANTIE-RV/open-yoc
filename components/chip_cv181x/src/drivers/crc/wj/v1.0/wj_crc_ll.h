/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_crc_ll.h
 * @brief    header file for crc ll driver
 * @version  V2.0
 * @date     02. Mar 2020
 ******************************************************************************/

#ifndef __WJ_CRC_LL_H__
#define __WJ_CRC_LL_H__

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CRC_DATA Offset 0x000 */
#define WJ_CRC_DATA_Pos                        (0U)
#define WJ_CRC_DATA_Msk                        (0xFFFFFFFFU << WJ_CRC_DATA_Pos)

/* CRC_SEL Offset 0x004 */
#define WJ_CRC_SEL_POLY_SEL_Pos                (0U)
#define WJ_CRC_SEL_POLY_SEL_Msk                (0x3U << WJ_CRC_SEL_POLY_SEL_Pos)
#define WJ_CRC_SEL_POLY_SEL_x8005              (0x0U << WJ_CRC_SEL_POLY_SEL_Pos)
#define WJ_CRC_SEL_POLY_SEL_x1021              (0x1U << WJ_CRC_SEL_POLY_SEL_Pos)
#define WJ_CRC_SEL_POLY_SEL_x31                (0x2U << WJ_CRC_SEL_POLY_SEL_Pos)
#define WJ_CRC_SEL_POLY_SEL_x07                (0x3U << WJ_CRC_SEL_POLY_SEL_Pos)
#define WJ_CRC_SEL_Polarity_Sel_Pos            (2U)
#define WJ_CRC_SEL_Polarity_Sel_Msk            (0x1U << WJ_CRC_SEL_Polarity_Sel_Pos)
#define WJ_CRC_SEL_Polarity_Sel_EN             WJ_CRC_SEL_Polarity_Sel_Msk

/*CRC_INIT Offset 0x004*/
#define WJ_CRC_INIT_Pos                        (0U)
#define WJ_CRC_INIT_Msk                        (0xFFFFU << WJ_CRC_INIT_Pos)

typedef struct {
    __IOM uint32_t CRC_DATA;              /* Offset 0x000(R/W) */
    __IOM uint32_t CRC_SEL;               /* Offset 0x004(R/W) */
    __IOM uint32_t CRC_INIT;              /* Offset 0x008(R/W) */
} ck_crc_reg_t;



/*----- CRC Control Codes: Mode -----*/
typedef enum {
    CRC_MODE_CRC8                   = 0,   ///< Mode CRC8
    CRC_MODE_CRC16,                        ///< Mode CRC16
    CRC_MODE_CRC32                         ///< Mode CRC32
} csi_crc_mode_t;

/*----- CRC Control Codes: Mode Parameters: Key length -----*/
typedef enum {
    CRC_STANDARD_ROHC         = 0,    ///< Standard CRC RHOC
    CRC_STANDARD_MAXIM,               ///< Standard CRC MAXIAM
    CRC_STANDARD_X25,                 ///< Standard CRC X25
    CRC_STANDARD_CCITT,               ///< Standard CRC CCITT
    CRC_STANDARD_CCITT_FALSE,         ///< Standard CRC CCITT-FALSE
    CRC_STANDARD_USB,                 ///< Standard CRC USB
    CRC_STANDARD_IBM,                 ///< Standard CRC IBM
    CRC_STANDARD_MODBUS,              ///< Standard CRC MODBUS
    CRC_STANDARD_ITU,                 ///< Standard CRC ITU
    CRC_STANDARD_PMEQ_2,              ///< Standard CRC PMEQ_2
    CRC_STANDARD_XMODEM,              ///< Standard CRC XMODEM
    CRC_STANDARD_DNP,                 ///< Standard CRC DNP
    CRC_STANDARD_NONE,                ///< Standard CRC NONE
    CRC_STANDARD_CRC8_31,
    CRC_STANDARD_CRC8_7,
} csi_crc_standard_t;

/**
\brief CRC State
*/
typedef struct {
    uint32_t busy             : 1;        ///< busy flag
    uint32_t timeout          : 1;        ///< timeout state
    uint32_t error            : 1;        ///< error
} csi_crc_state_t;

/**
\brief CRC config param
 */
typedef struct {
   csi_crc_mode_t       mode;
   csi_crc_standard_t   standard;
} csi_crc_config_t;

/**
\brief CRC ctrl block
 */
typedef struct {
    csi_dev_t           dev;
    csi_crc_config_t    config;
    csi_crc_state_t     state;
} csi_crc_t;



static inline uint32_t  wj_crc_get_data(ck_crc_reg_t *addr)
{
    return addr->CRC_DATA;
}

static inline void  wj_crc_set_data(ck_crc_reg_t *addr, uint32_t value)
{
    addr->CRC_DATA = value;
}

static inline void  wj_crc_set_crc_sel_poly_sel(ck_crc_reg_t *addr, uint32_t value)
{
    addr->CRC_SEL &= (~WJ_CRC_SEL_POLY_SEL_Msk);
    addr->CRC_SEL |= value & WJ_CRC_SEL_POLY_SEL_Msk;
}

static inline uint32_t  wj_crc_get_crc_sel_poly_sel(ck_crc_reg_t *addr)
{
    return (addr->CRC_SEL & WJ_CRC_SEL_POLY_SEL_Msk);
}

static inline void wj_crc_set_polarity_sel_negative(ck_crc_reg_t *addr)
{
    addr->CRC_SEL |=  WJ_CRC_SEL_Polarity_Sel_EN;
}
static inline void wj_crc_set_polarity_sel_positive(ck_crc_reg_t *addr)
{
    addr->CRC_SEL &= ~WJ_CRC_SEL_Polarity_Sel_EN;
}

static inline void  wj_crc_set_init_value(ck_crc_reg_t *addr, uint32_t value)
{
    addr->CRC_INIT = value & WJ_CRC_INIT_Msk;
}

static inline uint32_t   wj_crc_get_init_value(ck_crc_reg_t *addr)
{
    return addr->CRC_INIT;
}



#ifdef __cplusplus
}
#endif

#endif /* __WJ_CRC_LL_H__ */
