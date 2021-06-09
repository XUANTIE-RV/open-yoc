/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     arch_reg_save.S
 * @brief    save the cpu context
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/


.global arch_resume_context

#undef  CSKY_LPM_BASE
#define CSKY_LPM_BASE   0x30000014

arch_resume_context:
    mfcr    r0, cr<8, 3>
    bseti   r0, 1
    bseti   r0, 0
    mtcr    r0, cr<8, 3>
    br      .

    lrw     r0, g_top_irqstack
    mov     sp, r0
#    jbsr     resume_context_from_stop_mode

#    lrw     r0, CSKY_LPM_BASE
#    ldw     r1, (r0, 0x0)
#    btsti   r1, 3
#    bt      arch_sp_resume_from_standby

#arch_sp_resume_from_standby:
#    lrw     r0, Reset_Handler
#    lrw     r0, 0x8001000
    lrw     r0, CSKY_LPM_BASE
    ldw     r1, (r0, 0x0)
    ori     r1, r1, 8
    stw     r1, (r0, 0x0)
    lrw     r0, 0x84
    jsr     r0

