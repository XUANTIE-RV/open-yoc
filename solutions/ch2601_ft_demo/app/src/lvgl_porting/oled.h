/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef DISP_H
#define DISP_H

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include "lv_conf.h"
#include "src/lv_misc/lv_color.h"
#include "src/lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define TFT_HOR_RES LV_HOR_RES_MAX
#define TFT_VER_RES LV_VER_RES_MAX

#define TFT_EXT_FB  0               /*Frame buffer is located into an external SDRAM*/
#define TFT_USE_GPU 0               /*Enable hardware accelerator*/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    OT_NORMAL,          // output in normal;
    OT_NORMAL_TRANS,    // output in normal, backgrd trans;
    OT_ROTATE_R_90,     // rotate right 90 degree;
    OT_MIRROR_L_R,      // mirror left to right;
} LVGL_OUTPUT_TYP;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void oled_init();

/**********************
 *      MACROS
 **********************/

#endif
