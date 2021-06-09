/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for smartl. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef CONFIG_ARCH_INTERRUPTSTACK
#define CONFIG_ARCH_INTERRUPTSTACK 4096
#endif

#include <rom_sym_def.h>

.section .vectors
    .align 10
    .globl   __Vectors
    .type    __Vectors, @object
__Vectors:
    .long   Reset_Handler
    .size    __Vectors, . - __Vectors

    br       Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2

_start:
    .text
    .long    Reset_Handler
    .align  2
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    lrw     r0, 0x80000200
    mtcr    r0, psr

    lrw     r0, 0x1fff0d88
    mov     sp, r0

    lrw     r1, __data_start__
    lrw     r2, __data_end__
    lrw     r3, __erodata

    subu    r2, r1
    addu    r1, r3, r2

    lrw     r2, __data_copy_first_start__
    lrw     r3, __data_copy_first_end__

    subu    r3, r2
    cmpnei  r3, 0
    bf      .Data_copy_firset_done

    mov      r0, r2
    mov      r2, r3
    lrw      r3, osal_memcpy  //osal_memcpy in rom
    jsr      r3

.Data_copy_firset_done:

    jbsr     check_before_startup

    jbsr     Startup

    .size   Reset_Handler, . - Reset_Handler


.section .__data_copy_first__
    .align  2
    .globl  Startup
    .type   Startup, %function
Startup:
/*
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    lrw     r1, __erodata
    lrw     r2, __data_start__
    lrw     r3, __data_end__

    subu    r3, r2
    cmpnei  r3, 0
    bf      .L_loop0_done

    mov      r0, r2
    mov      r2, r3
    lrw      r3, osal_memcpy  //osal_memcpy in rom
    jsr      r3

    //ldw     r0, (r1, 0)
    //stw     r0, (r2, 0)
    // addi    r1, 4
    // addi    r2, 4
    // subi    r3, 4
    // cmpnei  r3, 0
    // bt      .L_loop0

.L_loop0_done:

/*
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    lrw     r1, __bss_start__
    lrw     r2, __bss_end__

    movi    r0, 0

    subu    r2, r1
    cmpnei  r2, 0
    bf      .L_loop1_done

    mov     r0, r1
    movi    r1, 0
    lrw     r3, osal_memset  ////osal_memcpy in rom
    jsr     r3

//.L_loop1:
//    stw     r0, (r1, 0)
//    addi    r1, 4
//    subi    r2, 4
//    cmpnei  r2, 0
//    bt      .L_loop1

.L_loop1_done:

#ifndef __NO_SYSTEM_INIT
    lrw    r3, SystemInit
    jsr    r3
#endif

    lrw    r3, pre_main
    jsr    r3

    .size   Startup, . - Startup

__exit:
    br      __exit

.section .bss

    .align  2
    .global g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
    .global __initial_sp
g_intstackalloc:
g_intstackbase:
    .space 4
g_top_irqstack:
__initial_sp:
