/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* adapter_internal.h
 *
 * Data types and Interfaces
 *
 * Extensions for the Security-IP-28 hardware.
 * This file is included from adapter_internal.h
 */



// ensure inclusion from adapter_internal.h
#ifndef INCLUDE_GUARD_ADAPTER_EIP28_H
#define INCLUDE_GUARD_ADAPTER_EIP28_H 1

/*----------------------------------------------------------------------------
 *                           Adapter_EIP28
 *----------------------------------------------------------------------------
 */

#include "eip28.h"
#include "stdbool.h"

extern EIP28_IOArea_t Adapter_EIP28_IOArea;

extern bool Adapter_EIP28_IsUsable;

bool Adapter_EIP28_Init(void);

void
Adapter_EIP28_UnInit(void);

#endif /* Include Guard */

/* end of file adapter_internal_ext.h */
