/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */


#include "stdint.h"
#include "cs_eip28.h"

typedef struct firmware_eip28 {
    uint32_t Version_MaMiPa;
    uint32_t *Image_p;
    uint32_t WordCount;
} Firmware_EIP28_t;


/*----------------------------------------------------------------------------
 * Firmware_EIP28_GetReferences
 *
 * This function returns references to the firmware images required by
 * EIP28 Driver Library.
 */
void
Firmware_EIP28_GetReferences(
        Firmware_EIP28_t * const FW_p);