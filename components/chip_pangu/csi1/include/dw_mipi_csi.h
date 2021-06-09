/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     dw_mipi_csi.h
 * @brief    header file for MIPI Driver
 * @version  V1.1
 * @date     24 May 2019
 ******************************************************************************/
#ifndef _DW_MIPI_CSI_H_
#define _DW_MIPI_CSI_H_

#include <stdio.h>
#include "errno.h"
#include "soc.h"

#define MIPI_CSI_CTRL_1 0
#define MIPI_CSI_CTRL_2 1

#define MIPI_CSI_CTRL1_ADDR 0x100
#define MIPI_CSI_CTRL2_ADDR 0x200

typedef struct {
    __IOM uint32_t R_CSI2_VERSION     ;          //0x0,
    __IOM uint32_t R_CSI2_N_LANES     ;          //0x4,
    __IOM uint32_t R_CSI2_CTRL_RESETN ;          //0x8,
    __IOM uint32_t R_CSI2_INTERRUPT   ;          //0xc,
    __IOM uint32_t R_CSI2_DATA_IDS_1  ;          //0x10,
    __IOM uint32_t R_CSI2_DATA_IDS_2  ;          //0x14,
    __IOM uint32_t rev_1[(0x40 - 0x14) / 4 - 1];
    __IOM uint32_t R_CSI2_DPHY_SHUTDOWNZ;        //0x40,
    __IOM uint32_t R_CSI2_DPHY_RSTZ;             //0x44,
    __IOM uint32_t R_CSI2_DPHY_RX;               //0x48,
    __IOM uint32_t R_CSI2_DPHY_STOPSTATE;        //0x4c,
    __IOM uint32_t R_CSI2_DPHY_TST_CTRL0;        //0x50,
    __IOM uint32_t R_CSI2_DPHY_TST_CTRL1;        //0x54,
    __IOM uint32_t R_CSI2_DPHY2_TST_CTRL0;       //0x58,
    __IOM uint32_t R_CSI2_DPHY2_TST_CTRL1;       //0x5c,
    __IOM uint32_t rev_2[(0x80 - 0x5c) / 4 - 1];
    __IOM uint32_t R_CSI2_IPI_MODE;              //0x80,
    __IOM uint32_t R_CSI2_IPI_VCID;              //0x84,
    __IOM uint32_t R_CSI2_IPI_DATA_TYPE;         //0x88,
    __IOM uint32_t R_CSI2_IPI_MEM_FLUSH;         //0x8c,
    __IOM uint32_t R_CSI2_IPI_HSA_TIME;          //0x90,
    __IOM uint32_t R_CSI2_IPI_HBP_TIME;          //0x94,
    __IOM uint32_t R_CSI2_IPI_HSD_TIME;          //0x98,
    __IOM uint32_t R_CSI2_IPI_HLINE_TIME;        //0x9c,
    __IOM uint32_t rev_3[(0xa0 - 0x9c) / 4 - 1];
    __IOM uint32_t R_CSI2_IPI_SOFTRSTN;          //0xa0,
    __IOM uint32_t rev_4[2];
    __IOM uint32_t R_CSI2_IPI_ADV_FEATURES;      //0xac
    __IOM uint32_t R_CSI2_IPI_VSA_LINES;         //0xb0,
    __IOM uint32_t R_CSI2_IPI_VBP_LINES;         //0xb4,
    __IOM uint32_t R_CSI2_IPI_VFP_LINES;         //0xb8,
    __IOM uint32_t R_CSI2_IPI_VACTIVE_LINES;     //0xbc,
    __IOM uint32_t rev_5[(0xe0 - 0xbc) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_PHY_FATAL;         //0xe0,
    __IOM uint32_t R_CSI2_MASK_INT_PHY_FATAL;    //0xe4,
    __IOM uint32_t R_CSI2_FORCE_INT_PHY_FATAL;   //0xe8,
    __IOM uint32_t rev_6[(0xf0 - 0xe8) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_PKT_FATAL;         //0xf0,
    __IOM uint32_t R_CSI2_MASK_INT_PKT_FATAL;    //0xf4,
    __IOM uint32_t R_CSI2_FORCE_INT_PKT_FATAL;   //0xf8,
    __IOM uint32_t rev_7[(0x100 - 0xf8) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_FRAME_FATAL;       //0x100,
    __IOM uint32_t R_CSI2_MASK_INT_FRAME_FATAL;  //0x104,
    __IOM uint32_t R_CSI2_FORCE_INT_FRAME_FATAL; //0x108,
    __IOM uint32_t rev_8[(0x110 - 0x108) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_PHY;               //0x110,
    __IOM uint32_t R_CSI2_MASK_INT_PHY;          //0x114,
    __IOM uint32_t R_CSI2_FORCE_INT_PHY;         //0x118,
    __IOM uint32_t rev_9[(0x120 - 0x118) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_PKT;               //0x120,
    __IOM uint32_t R_CSI2_MASK_INT_PKT;          //0x124,
    __IOM uint32_t R_CSI2_FORCE_INT_PKT;         //0x128,
    __IOM uint32_t rev_10[(0x130 - 0x128) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_LINE;              //0x130,
    __IOM uint32_t R_CSI2_MASK_INT_LINE;         //0x134,
    __IOM uint32_t R_CSI2_FORCE_INT_LINE;        //0x138,
    __IOM uint32_t rev_11[(0x140 - 0x138) / 4 - 1];
    __IOM uint32_t R_CSI2_INT_IPI;               //0x140,
    __IOM uint32_t R_CSI2_MASK_INT_IPI;          //0x144,
    __IOM uint32_t R_CSI2_FORCE_INT_IPI;         //0x148,
    __IOM uint32_t rev_12[(0x300 - 0x148) / 4 - 1];
    __IOM uint32_t SCRAMBLING;                   //0x300,
} mipi_csi_reg_t;

#endif /* __DW_MIPI_CSI_H__ */
