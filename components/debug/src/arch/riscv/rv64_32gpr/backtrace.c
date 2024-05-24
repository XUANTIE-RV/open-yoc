/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <k_arch.h>
#include "k_api.h"
#include <debug/dbg.h>

//#define OS_BACKTRACE_DEBUG

extern char *__etext;
extern char *__stext;

#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
extern char *__post_text_start;
extern char *__post_text_end;
#define BT_CHK_PC_AVAIL(pc)   (((uintptr_t)(pc) < (uintptr_t)(&__etext) \
                              && (uintptr_t)(pc) > (uintptr_t)(&__stext)) || \
                              ((uintptr_t)(pc) < (uintptr_t)(&__post_text_end) \
                              && (uintptr_t)(pc) > (uintptr_t)(&__post_text_start)))
#else
#define BT_CHK_PC_AVAIL(pc)   ((uintptr_t)(pc) < (uintptr_t)(&__etext) \
                              && (uintptr_t)(pc) > (uintptr_t)(&__stext))
#endif

#define BT_PC2ADDR(pc)        ((char *)(((uintptr_t)(pc))))

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

extern void krhino_task_deathbed(void);
extern int debug_task_is_running(ktask_t *task);
extern void *debug_task_stack_bottom(ktask_t *task);
extern char *k_int64tostr(int64_t num, char *str);
extern char *k_int2str(int num, char *str);
extern void _interrupt_return_address(void);

void backtrace_handle(char *PC, int *SP, char *LR, int (*print_func)(const char *fmt, ...));

/* get framesize from c ins */
static int riscv_backtrace_framesize_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* addi sp, sp, -im */
    if ((inst & 0xFF83) == 0x1101) {
        imm = (inst >> 2) & 0x1F;
        imm = (~imm & 0x1F) + 1;
        return imm >> 3;
    }

    /* c.addi16sp sp, nzuimm6<<4 */
    if ((inst & 0xFF83) == 0x7101) {
        imm = (inst >> 3) & 0x3;
        imm = (imm << 1) | ((inst >> 5) & 0x1);
        imm = (imm << 1) | ((inst >> 2) & 0x1);
        imm = (imm << 1) | ((inst >> 6) & 0x1);
        imm = ((~imm & 0x1f) + 1) << 4;
        return imm >> 3;
    }

    return -1;
}

/* get framesize from c ins32 */
static int riscv_backtrace_framesize_get1(unsigned int inst)
{
    unsigned int imm = 0;
    /* addi sp, sp, -im */
    if ((inst & 0x800FFFFF) == 0x80010113) {
        imm = (inst >> 20) & 0x7FF;
        imm = (~imm & 0x7FF) + 1;
        return imm >> 3;
    }

    return -1;
}

/* get ra position in the stack */
static int riscv_backtrace_ra_offset_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* c.fsdsp rs2, uimm6<<3(sp) */
    if ((inst & 0xE07F) == 0xE006) {
        imm = (inst >> 7) & 0x7;
        imm = (imm << 3) | ((inst >> 10) & 0x7);
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm;
    }

    return -1;
}

static int riscv_backtrace_ra_offset_get1(unsigned int inst)
{
    unsigned int imm = 0;
    /* sd ra,552(sp) */
    if ((inst & 0x81FFF07F) == 0x113023) {
        imm = (inst >> 7) & 0x1F;
        imm |= ((inst >> 25) & 0x7F) << 5;
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm >> 3;
    }

    return -1;
}

/* get the offset between the jump instruction and the return address */
static int backtraceFindLROffset(char *LR, int (*print_func)(const char *fmt, ...))
{
    int offset = 0;
    char *LR_indeed;
    unsigned int ins32;
    char         s_panic_call[] = "backtrace : 0x         \r\n";

    LR_indeed = BT_PC2ADDR(LR);

    /* callstack bottom */
    if (LR_indeed == BT_PC2ADDR(&_interrupt_return_address)) {
        /* EXC_RETURN, so here is callstack bottom of interrupt handler */
        if (print_func != NULL) {
            print_func("backtrace : ^interrupt^\r\n");
        }
        return 0;
    }

    if (LR_indeed == BT_PC2ADDR(&krhino_task_deathbed)) {
        /* task delete, so here is callstack bottom of task */
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 0;
    }

    /* Usually jump using the JAL instruction */
    ins32 = *(unsigned int *)(LR_indeed - 4);
    if ((ins32 & 0x3) == 0x3) {
        offset = 4;
    } else {
        offset = 2;
    }

    if (print_func != NULL) {
        k_int64tostr((int)((unsigned long)LR_indeed - offset), &s_panic_call[14]);
        print_func(s_panic_call);
    }

    return offset;
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int riscv_backtraceFromStack(long **pSP, char **pPC,
                                    int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    long  *SP      = *pSP;
    char *PC       = *pPC;
    char *LR;
    int   i;
    int   framesize;
    int   offset = 0;
    unsigned int ins32;
    unsigned short ins16;

#ifdef OS_BACKTRACE_DEBUG
    printk("[riscv_backtraceFromStack in ] SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if (SP == debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    }

    /* 1. scan code, find lr pushed */
    for (i = 0; i < BT_FUNC_LIMIT;) {
        /* FIXME: not accurate from bottom to up. how to judge 2 or 4byte inst */
        //CodeAddr = (char *)(((long)PC & (~0x3)) - i);
        CodeAddr = (char *)(PC - i);
        ins32 = *(unsigned int *)(CodeAddr);
        if ((ins32 & 0x3) == 0x3) {
            ins16 = *(unsigned short *)(CodeAddr - 2);
            if ((ins16 & 0x3) != 0x3) {
                i += 4;
                framesize = riscv_backtrace_framesize_get1(ins32);
                if (framesize >= 0) {
                    CodeAddr += 4;
                    break;
                }
                continue;
            }
        }
        i += 2;
        ins16 = (ins32 >> 16) & 0xffff;
        framesize = riscv_backtrace_framesize_get(ins16);
        if (framesize >= 0) {
            CodeAddr += 2;
            break;
        }
    }

    if (i == BT_FUNC_LIMIT) {
        /* error branch */
        if (print_func != NULL) {
            print_func("Backtrace fail!\r\n");
        }
        return -1;
    }

    /* 2. scan code, find ins: sd ra,24(sp) or sd ra,552(sp) */
    for (i = 0; CodeAddr + i < PC;) {
        ins32 = *(unsigned int *)(CodeAddr + i);
        if ((ins32 & 0x3) == 0x3) {
            i += 4;
            offset = riscv_backtrace_ra_offset_get1(ins32);
            if (offset >= 0) {
                break;
            }
        } else {
            i += 2;
            ins16 = ins32 & 0xffff;
            offset = riscv_backtrace_ra_offset_get(ins16);
            if (offset >= 0) {
                break;
            }
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    printk("[arm_backtraceFromStack out] frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR     = (char *) * (SP + offset);

    if (BT_CHK_PC_AVAIL(LR) == 0) {
        if (print_func != NULL) {
            print_func("backtrace : invalid lr\r\n");
        }
        return -1;
    }
    *pSP   = SP + framesize;
    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

static int backtraceFromStack(long **pSP, char **pPC,
                              int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_AVAIL(*pPC) == 0) {
        return -1;
    }

    return riscv_backtraceFromStack(pSP, pPC, print_func);
}

/* printf call stack
   return levels of call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    long  *SP;
    int   lvl;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    SP = RHINO_GET_SP();
    PC = RHINO_GET_PC();

    print_func("========== Call stack ==========\r\n");
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, print_func);
        if (ret != 0) {
            break;
        }
    }
    print_func("==========    End     ==========\r\n");
    return lvl;
}

static void getPLSfromTaskCtx(void *context, char **PC, char **LR, long**SP)
{
    context_t *task_ctx = (context_t *)context;

    *PC = (char *)task_ctx->PC;
    *LR = (char *)task_ctx->X1;
    *SP = (long *)(task_ctx + 1);
}

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, long *SP, char *LR,
                     int (*print_func)(const char *fmt, ...))
{
    long *bt_sp;
    char *bt_pc;
    int   lvl, ret;
    char  panic_call[] = "backtrace : 0x         \r\n";

    /* caller must save LR in stack, so find LR from stack */

    if (SP == NULL) {
        return 0;
    }

    bt_sp = SP;
    bt_pc = PC;
    ret   = -1;

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, NULL);
        if (ret != 0) {
            break;
        }
    }

    if (ret == 1) {
        bt_sp = SP;
        bt_pc = PC;
    } else {
        int offset = backtraceFindLROffset(LR, NULL);

        bt_sp = SP;
        bt_pc = LR - offset;
        if (print_func != NULL) {
            k_int64tostr((int)((unsigned long)BT_PC2ADDR(bt_pc)), &panic_call[14]);
            print_func(panic_call);
        }
    }

    for (lvl = 1; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
        if (ret != 0) {
            break;
        }
    }
    return lvl;
}

/* printf call stack for task
   return levels of call stack */
void backtrace_task(void *task, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    long     *SP;
    int      ret;
    char     panic_call[] = "backtrace : 0x         \r\n";
    ktask_t *task_inner = (ktask_t *)task;

    if (print_func == NULL) {
        print_func = printf;
    }

    if (task_inner == NULL) {
        return;
    }

    getPLSfromTaskCtx(task_inner->task_stack, &PC, &LR, &SP);

    print_func("========== Call stack ==========\r\n");
    k_int64tostr((int)((unsigned long)BT_PC2ADDR(PC)), &panic_call[14]);
    if (print_func != NULL) {
        print_func(panic_call);
    }

    ret = debug_task_is_running(task_inner);
    if (ret == 3) {
        print_func("Status of task is 'Running', Can not backtrace!\n");
    } else {
        backtrace_caller(PC, SP, LR, print_func);
    }

    print_func("==========    End     ==========\r\n");

    return;
}

/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(void *trace[], int size, int offset)
{
    char *PC;
    long  *SP;
    int   lvl;
    int   ret;

    SP = RHINO_GET_SP();
    PC = RHINO_GET_PC();

    memset(trace, 0, size * sizeof(void *));

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, NULL);
        if (ret != 0) {
            break;
        }
        if (lvl >= offset && lvl - offset < size) {
            trace[lvl - offset] = PC;
        }
        if (lvl - offset >= size) {
            break;
        }
    }

    return lvl - offset < 0 ? 0 : lvl - offset;
}

void backtrace_handle(char *PC, int *SP, char *LR,
                      int (*print_func)(const char *fmt, ...))
{
    long  *pSP = (long *)SP;
    char  panic_call[] = "backtrace : 0x         \r\n";

    if (print_func == NULL) {
        print_func = printf;
    }

    if (print_func != NULL) {
        k_int64tostr((int)((unsigned long)BT_PC2ADDR(PC)), &panic_call[14]);
        print_func(panic_call);
    }

#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
    long  cause;
    __asm__ volatile("csrr %0, scause" : "=r"(cause));
    /* FIXME: pc may be in post segment */
    if (cause == CAUSE_FETCH_PAGE_FAULT || cause == CAUSE_LOAD_PAGE_FAULT || cause == CAUSE_STORE_PAGE_FAULT) {
        int offset = backtraceFindLROffset(LR, NULL);

        PC = LR - offset;
    }
#endif

    backtrace_caller(PC, pSP, LR, print_func);

    return;
}

