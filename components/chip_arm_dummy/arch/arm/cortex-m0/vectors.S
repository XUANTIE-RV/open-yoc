/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     vectors.S
 * @brief    define default vector handlers. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     28. Nove 2020
 ******************************************************************************/

#include <csi_config.h>

    .align  2
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    b       .
    .size   Default_Handler, . - Default_Handler
