/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VPSS_PROPERTY_H
#define TM_VPSS_PROPERTY_H

#include <string>

#include <tmedia_core/common/common_inc.h>

namespace vpss_prop
{

typedef enum
{
    //CROP_RATIO_COOR = 0,    /* Ratio coordinate. */
    CROP_ABS_COOR,          /* Absolute coordinate. */
} CropCoordinate_e;

typedef enum
{
    DATA_OFFLINE_TO_OFFLINE = 0,
    DATA_OFFLINE_TO_ONLINE,
    DATA_ONLINE_TO_OFFLINE,
    DATA_ONLINE_TO_ONLINE,
    DATA_PARALLEL_TO_OFFLINE,
    DATA_PARALLEL_TO_PARALLEL,
} DataLineMode_e;

typedef enum
{
    DYNAMIC_RANGE_SDR8 = 0,
} DynamicRange_e;

typedef struct
{
    int srcFrameRate;
    int dstFrameRate;
} FrameRateCtrl_s;

} // vpss_prop

#endif  /* TM_VPSS_PROPERTY_H */
