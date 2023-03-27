/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_internal_pp.h
 *
 * EIP-76 Internal Post Processor Interface
 */

#ifndef EIP76_INTERNAL_PP_H_
#define EIP76_INTERNAL_PP_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint32_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define EIP76_MIN_PS_AI_WORD_COUNT  1
#define EIP76_MAX_PS_AI_WORD_COUNT  12


/*----------------------------------------------------------------------------
 * EIP76_Internal_PostProcessor_PS_AI_Write
 *
 */
void
EIP76_Internal_PostProcessor_PS_AI_Write(
        const Device_Handle_t Device,
        const uint32_t * PS_AI_Data_p,
        const unsigned int PS_AI_WordCount);


#endif /* EIP76_INTERNAL_PP_H_ */

/* end of file eip76_internal_pp.h */
