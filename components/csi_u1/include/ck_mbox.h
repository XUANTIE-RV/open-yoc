/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     ck_mbox.h
 * @brief    head file for ck mailbox
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _CK_MBOX_H_
#define _CK_MBOX_H_

#include "soc.h"

#define CONFIG_MBOX_CHANNEL_NUM     16
#define CONFIG_MBOX_INTR_EN_BIT     16

typedef struct {
    __IOM uint32_t INTR_SET;
    __IOM uint32_t INTR_EN;
    __IOM uint32_t INTR_STA;
    __IOM uint32_t INTR_STA_RAW;
} ck_mbox_reg_t;

#endif
