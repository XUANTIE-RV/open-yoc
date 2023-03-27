/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_eip201.h
 *
 * Configuration options for the EIP201 Driver Library module.
 */



#ifndef INCLUDE_GUARD_CS_EIP201_H
#define INCLUDE_GUARD_CS_EIP201_H

// we accept a few settings from the top-level configuration file
#include "cs_driver.h"

/* EIP201_STRICT_ARGS
 *
 * Set this option to enable checking of all arguments to all EIP201 DL
 * functions. Disable it to reduce code size and reduce overhead.
 */
#ifndef DRIVER_PERFORMANCE
#define EIP201_STRICT_ARGS
#endif

/* EIP201_STRICT_ARGS_MAX_NUM_OF_INTERRUPTS <NOI>
 *
 * This configures the maximum Number Of Interrupt (NOI) sources
 * actually available in the EIP201 AIC.
 * This can be used for strict argument checking.
 */
#define EIP201_STRICT_ARGS_MAX_NUM_OF_INTERRUPTS    30


#endif /* Include Guard */


/* end of file cs_eip201.h */
