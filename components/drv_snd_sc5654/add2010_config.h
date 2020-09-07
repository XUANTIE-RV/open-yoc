/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     add2010_config.h
 * @brief    header file for add2010 configuration
 * @version  V1.0
 * @date     31. Oct. 2019
 ******************************************************************************/

#ifndef __ADD2010_CONFIG_H__
#define __ADD2010_CONFIG_H__

#include <stdint.h>

typedef struct add2010_reg_table {
    uint8_t addr;
    uint8_t data;
} add2010_reg_table_t;

typedef struct add2010_ram_table {
    uint8_t addr;
    uint8_t first_byte;
    uint8_t second_byte;
    uint8_t third_byte;
} add2010_ram_table_t;

extern add2010_reg_table_t g_add2010_reg_tab[];
extern int                 g_add2010_reg_tab_size;
extern add2010_ram_table_t g_add2010_ram1_tab[];
extern int                 g_add2010_ram1_tab_size;
extern add2010_ram_table_t g_add2010_ram2_tab[];
extern int                 g_add2010_ram2_tab_size;

#endif
