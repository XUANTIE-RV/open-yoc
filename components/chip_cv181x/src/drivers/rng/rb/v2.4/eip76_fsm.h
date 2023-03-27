/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */


/* eip76_fsm.h
 *
 * EIP-76 Driver Library State Machine Internal interface
 */


#ifndef EIP76_FSM_H_
#define EIP76_FSM_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

#include "eip76_types.h"            // EIP76_Status_t

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// EIP-76 Driver Library States
typedef enum
{
    // Super state EIP76_STATE_RESET: 1 (11, 12)
    EIP76_STATE_RESET                               = 11,
    EIP76_STATE_RESET_BCDF                          = 12,
    // Super state EIP76_STATE_RANDOM: 2 (21, 22)
    EIP76_STATE_RANDOM_GENERATING                   = 21,
    EIP76_STATE_RANDOM_READY                        = 22,
    EIP76_STATE_RANDOM_READING                      = 30,
    // Super state EIP76_STATE_SP80090_RESEED: 5 (51, 52, 53)
    EIP76_STATE_SP80090_RESEED_START                = 51,
    EIP76_STATE_SP80090_RESEED_READY                = 52,
    EIP76_STATE_SP80090_RESEED_WRITING              = 53,
    // Super state EIP76_STATE_KAT: 6 (61, 62)
    EIP76_STATE_KAT_START                           = 61,
    EIP76_STATE_KAT_READY                           = 62,
    // Super state EIP76_STATE_KAT_PRM: 7 (71, 72, 73)
    EIP76_STATE_KAT_PRM_PROCESSING                  = 71,
    EIP76_STATE_KAT_PRM_M_FAILED                    = 72,
    EIP76_STATE_KAT_PRM_DONE                        = 73,
    // Super state EIP76_STATE_SP80090: 9 (91, 92, 93, 94)
    EIP76_STATE_KAT_SP80090_PROCESSING              = 91,
    EIP76_STATE_KAT_SP80090_BCDF_RESEEDED           = 92,
    EIP76_STATE_KAT_SP80090_BCDF_NOISE              = 93,
    EIP76_STATE_KAT_SP80090_BCDF_READY              = 94,
    EIP76_STATE_KAT_SP80090_BCDF_PROCESSING         = 95,
    // Super state EIP76_STATE_KAT_CF: 10 (101,102)
    EIP76_STATE_KAT_CF_PROCESSING                   = 101,
    EIP76_STATE_KAT_CF_DONE                         = 102,
    EIP76_STATE_KAT_REP_PROCESSING                  = 200,
    EIP76_STATE_KAT_ADAP_PROCESSING                 = 300
} EIP76_State_t;


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * EIP76_State_Set
 *
 * This function check whether the transition from the "CurrentState" to the
 * "NewState" is allowed and if yes changes the former to the latter.
 *
  * Return value
 *     EIP76_NO_ERROR : operation is completed
 *     EIP76_ILLEGAL_IN_STATE : state transition is not allowed
 */
EIP76_Status_t
EIP76_State_Set(
        volatile EIP76_State_t * const CurrentState,
        const EIP76_State_t NewState);


#endif /* EIP76_FSM_H_ */

/* end of file eip76_fsm.h */
