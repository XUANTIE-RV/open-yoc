/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* adapter_interrupts_ext.h
 *
 * Host hardware specific extensions for the Adapter Interrupts interface
 */



#ifndef ADAPTER_INTERRUPTS_EXT_H_
#define ADAPTER_INTERRUPTS_EXT_H_
#include "cs_adapter.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Adapter logical EIP-28 PKA interrupts
enum
{
    IRQ_EIP28_READY = ADAPTER_EIP28_READY_IRQ,
    IRQ_LAST_LINE
};


#endif /* ADAPTER_INTERRUPTS_EXT_H_ */

/* end of file adapter_interrupts_ext.h */
