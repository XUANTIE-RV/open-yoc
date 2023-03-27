/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* adapter_dmabuf.h
 *
 * Adapter DMABuf internal interface
 */



#ifndef ADAPTER_DMABUF_H_
#define ADAPTER_DMABUF_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"            // bool

// Driver Framework DMAResource API
#include "dmares_types.h"          // DMAResource_Handle_t

// DMABuf API
#include "api_dmabuf.h"            // DMABuf_Handle_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define ADAPTER_DMABUF_ALIGNMENT_INVALID       -1


/*----------------------------------------------------------------------------
 * Function name
 *
 */

// DMAResource_Handle_t
// Adapter_DMABuf_Handle2DMAResourceHandle(
//         DMABuf_Handle_t Handle);


// DMABuf_Handle_t
// Adapter_DMAResource_Handle2DMABufHandle(
//         DMAResource_Handle_t Handle);


void *
Adapter_DMAResource_HostAddr(
        DMAResource_Handle_t Handle);


bool
Adapter_DMAResource_IsSubRangeOf(
        const DMAResource_Handle_t Handle1,
        const DMAResource_Handle_t Handle2);


// bool
// Adapter_DMAResource_IsForeignAllocated(
//         DMAResource_Handle_t Handle);


void
Adapter_DMAResource_Alignment_Set(
        const int Alignment);


int
Adapter_DMAResource_Alignment_Get(void);


#endif /* ADAPTER_DMABUF_H_ */


/* end of file adapter_dmabuf.h */
