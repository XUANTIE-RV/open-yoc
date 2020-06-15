/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>

__attribute__((naked)) void tee_wsl(uint32_t send_msg)
{
    __asm__ volatile(
        "push r15 \n"
        "subi sp, sp, 40 \n"
        "stm  r4-r13, (sp) \n"
        "subi sp, sp, 64 \n"
        "stm  r16-r31, (sp) \n"
        "mov r0, %0 \n"
        "wsc \n"
        "ldm r16-r31, (sp) \n"
        "addi sp, sp, 64 \n"
        "ldm r4-r13, (sp) \n"
        "addi sp, sp, 40 \n"
        "pop r15 \n"
        :
        : "rw"(send_msg)
        : "r0"
    );
}
