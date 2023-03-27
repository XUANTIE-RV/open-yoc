/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* adapter_init.h
 *
 * Adapter Initialization interface
 */



#ifndef INCLUDE_GUARD_ADAPTER_INIT_H
#define INCLUDE_GUARD_ADAPTER_INIT_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"     // bool

/*----------------------------------------------------------------------------
 *                           Adapter initialization
 *----------------------------------------------------------------------------
 */

bool
Adapter_Init(void);

void
Adapter_UnInit(void);

void
Adapter_Report_Build_Params(void);

#endif /* Include Guard */

/* end of file adapter_init.h */
