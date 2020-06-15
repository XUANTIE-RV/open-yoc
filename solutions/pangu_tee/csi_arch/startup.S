/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "tee_misc.h"
#include "tee_addr_map.h"

.section .exp_table
/* Vector table space. */
.align 10
ckcpu_vsr_table:

/* 0: Rest */
.long wsc_intr_handler
/* 1 ~~ WSC_INTR_ID-1: default exception */
.rept WSC_INTR_ID - 1
.long sys_exception_handler
.endr

/* WSC_INTR_ID: */
.long wsc_intr_handler

/* WSC_INTR_ID+1 ~~ SYS_EXCEPTION_NUM: default exception */
.rept SYS_EXCEPTION_NUM - WSC_INTR_ID -1
.long sys_exception_handler
.endr

.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler
.long Default_Handler

    .text
    /*
     * This is the codes first entry point. This is where it all
     * begins...
     */
    br       Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2
_start:
    .text
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:

    /*
     * Init psr value, disable exception, disable interrupt and fast interrupt.
     * psr = 0xc0000000
     */
    movi     a3, 0
    bseti    a3, 31 /* S */
    bseti    a3, 30 /* SE */
    mtcr     a3, psr

    /*
     * Setup secure initial vector base table for interrupts and exceptions
     */
    lrw      a2, ckcpu_vsr_table
    mtcr     a2, vbr
    mtcr     a2, cr<1,1>

    /* Re-Init psr */
    mfcr     a3, psr
    bseti    a3, 8  /* enable EE */
    mtcr     a3, psr

    /* Initialize the normal stack pointer from the end of TW volatile mem. */
    lrw      a1, TW_RW_ADDR + TW_RW_SIZE
    bclri    a1, 0
    bclri    a1, 1

    mov      sp, a1

    lrw      a3, __data_copy_start
    lrw      a2, __data_start
    lrw      a1, __data_end
    subu     a1, a2
    lsri     a1, 2


    cmpnei   a1, 0
    bf       __skip_data_copy

7:
    ld.w     a0, (a3)
    st.w     a0, (a2)
    addi     a3, 4
    addi     a2, 4
    decne    a1
    bt       7b
__skip_data_copy:

    /*
     * Zero out the bss region.
     * NOTE: __bss_start must align 4
     */
    lrw      a3, __bss_start
    lrw      a2, __bss_end
    subu     a2, a3
    lsri     a2, 2
    cmpnei   a2, 0
    bf       __goto_c
    movi     a1, 0
2:
    stw      a1, (a3)
    addi     a3, 4
    decne    a2
    bt       2b

    /*
     * Assember start up done, C codes start here.
     */
__goto_c:
    jbsr     board_init

    jbsr     main

    /* Should never get here. */
__exit:
    br       __exit

    .align    1
    .weak    Default_Handler
    .type    Default_Handler, %function
Default_Handler:
    bsr     reboot
    .size    Default_Handler, . - Default_Handler

    /*
     * Macro to define default handlers. Default handler
     * will be weak symbol and just dead loops. They can be
     * overwritten by other handlers
     */
    .macro def_irq_handler handler_name
    .weak  \handler_name
    .set   \handler_name, Default_Handler
    .endm

    def_irq_handler CORET_IRQHandler
    def_irq_handler TIMA0_IRQHandler
    def_irq_handler TIMA1_IRQHandler
    def_irq_handler TIMB0_IRQHandler
    def_irq_handler TIMB1_IRQHandler
    def_irq_handler USART0_IRQHandler
    def_irq_handler USART1_IRQHandler
    def_irq_handler USART2_IRQHandler
    def_irq_handler USART3_IRQHandler
    def_irq_handler GPIOA_IRQHandler
    def_irq_handler GPIOB_IRQHandler
    def_irq_handler I2C0_IRQHandler
    def_irq_handler I2C1_IRQHandler
    def_irq_handler SPI0_IRQHandler
    def_irq_handler SPI1_IRQHandler
    def_irq_handler RTC_IRQHandler
    def_irq_handler WDT_IRQHandler
    def_irq_handler PWM_IRQHandler
    def_irq_handler DMAC_IRQHandler
    def_irq_handler AES_IRQHandler
    def_irq_handler SHA_IRQHandler
