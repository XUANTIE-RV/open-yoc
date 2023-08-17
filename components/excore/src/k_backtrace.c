/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_arch.h"
#include "debug_api.h"

//#define OS_BACKTRACE_DEBUG

#if defined(__CC_ARM)
#ifdef __BIG_ENDIAN
#error "Not support big-endian!"
#endif
#elif defined(__ICCARM__)
#if (__LITTLE_ENDIAN__ == 0)
#error "Not support big-endian!"
#endif
#elif defined(__GNUC__)
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error "Not support big-endian!"
#endif
#endif

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

#define BT_PC2ADDR(pc)          ((char *)(((unsigned long)(pc)) & 0xfffffffe))
#define BT_CHK_PC_THUMB(pc)     ((unsigned long)(pc) & 0x1)
#define BT_MODESET_THUMB(pc)    do{(pc) = (void *)((unsigned long)(pc) | 0x1);}while(0)

/* part of ktask_t */
typedef struct {
    void *task_stack;
} ktask_t_shadow;

extern char *k_int2str_hex(int num, char *str);
extern void *debug_task_stack_bottom(ktask_t *task);
extern ktask_t *debug_task_find(char *name);
extern int debug_task_is_running(ktask_t *task);

extern char *_mmu_text_start;
extern char *_mmu_text_end;
extern char *_mmu_post_text_start;
extern char *_mmu_post_text_end;

extern ktask_t *debug_task_find_running(char **name);
extern void krhino_task_deathbed(void);
extern void _interrupt_return_address(void);

extern uint32_t *sys_stack_top;

#if (RHINO_CONFIG_USER_SPACE > 0)
extern kproc_info_t g_proc_info;
#endif

static int backtrace_space = BACKTRACE_KSPACE;

int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...));

int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...));

#if defined(__ICCARM__)
static unsigned int __builtin_popcount(unsigned int u)
{
    unsigned int ret = 0;
    while (u) {
        u = (u & (u - 1));
        ret++;
    }
    return ret;
}
#endif

/* check pc is valid or not
   ret 1: pc is valid
   ret 0: invalid */
static int BT_CHK_PC_AVAIL(char* pc)
{
    int val = 0;
    unsigned long addr_start = (unsigned long)(&_mmu_text_start);
    unsigned long addr_end   = (unsigned long)(&_mmu_text_end);

    val = ((unsigned long)(pc) < addr_end) && ((unsigned long)(pc) > addr_start);

    if(val) {
        return val;
    }

    addr_start = (unsigned long)(&_mmu_post_text_start);
    addr_end   = (unsigned long)(&_mmu_post_text_end);
    val = ((unsigned long)(pc) < addr_end) && ((unsigned long)(pc) > addr_start);

    if(val) {
        return val;
    }

#if (RHINO_CONFIG_USER_SPACE > 0)
    kproc_app_t *proc     = NULL;
    preamble_t  *preamble = NULL;
    uint32_t        pid;
    for(pid = 0; pid < CONFIG_EXCORE_PROCESS_MAX; pid++ ) {
        if(OS_ASID_KERNEL == pid) {
            continue;
        }
        proc = &g_proc_info.procs[pid];
        if(proc->state != K_PROC_RDY) {
            break;  //pid find end
        }
        preamble   = &proc->preamble;
        addr_start = (unsigned long)preamble->text_start;
        addr_end   = (unsigned long)preamble->text_end;
        val = ((unsigned long)(pc) < addr_end) && ((unsigned long)(pc) > addr_start);
        if(val) {
            return val;
        }
    }

#endif

    return 0;
}

/* check callstack bottom of task
   ret 1: addr is bottom of task(deathbed)
   ret 0 : not task bottom */
static int backtrace_task_bottom_check(int addr)
{
    int task_bottom = (int)BT_PC2ADDR(&krhino_task_deathbed);

#if (RHINO_CONFIG_USER_SPACE > 0)
    kproc_app_t *proc = NULL;
    proc = &g_proc_info.procs[g_active_task[cpu_cur_get()]->pid];
    if (backtrace_space == BACKTRACE_USPACE) {
        task_bottom = (int)BT_PC2ADDR(proc->preamble.ret_entry);
    }
#endif

    return (addr == task_bottom);
}


static void getPLSfromTaskCtx(void *context, char **PC, char **LR, int **SP)
{
    context_t *task_ctx;

    task_ctx = (context_t *)context;

    *PC = (char *)task_ctx->PC;
    *LR = (char *)task_ctx->LR;
    *SP = (int *)(task_ctx + 1);
    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( task_ctx->CPSR & 0x20 ) {
        BT_MODESET_THUMB(*PC);
    }
}
#if 0
static void getPLSfromFaultCtx(void *context, char **PC, char **LR, int **SP)
{
    fault_context_t *task_ctx;

    task_ctx = (fault_context_t *)context;

    *PC = (char *)task_ctx->cntx.PC;
    *LR = (char *)task_ctx->cntx.LR;
    *SP = (int *)task_ctx->SP;;
    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( task_ctx->cntx.CPSR & 0x20 ) {
        BT_MODESET_THUMB(*PC);
    }
}
#endif

/* get "blx" or "bl" before LR, return offset */
static int backtraceFindLROffset(char *LR,
                                 int (*print_func)(const char *fmt, ...))
{
    int offset = 0;
    char *LR_indeed;
    unsigned short ins16;
    char           s_panic_call[] = "backtrace : 0x         \r\n";

    LR_indeed = BT_PC2ADDR(LR);

    /* callstack bottom */
    if (LR_indeed == BT_PC2ADDR(&_interrupt_return_address)) {
        /* EXC_RETURN, so here is callstack bottom of interrupt handler */
        if (print_func != NULL) {
            print_func("backtrace : ^interrupt^\r\n");
        }
        return 0;
    }

    if (backtrace_task_bottom_check((int)LR_indeed)) {
        /* here is callstack bottom of task */
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 0;
    }

    /* check if thumb mode for next level */
    if (BT_CHK_PC_THUMB(LR)) {
        ins16 = *(unsigned short *)(LR_indeed - 4);
        if ((ins16 & 0xf000) == 0xf000) {
            offset = 4;
        } else {
            offset = 2;
        }
    } else {
        offset = 4;
    }

    if (print_func != NULL) {
        k_int2str_hex((int)LR_indeed - offset, &s_panic_call[14]);
        print_func(s_panic_call);
    }

    return offset;
}


/* check if push lr instruction, return framesize pushed */
static int arm_backtraceInsPushLR(unsigned int inst, int *lr_offset)
{
    /* push lr */
    if ((inst & 0xFFFF4000) == 0xE92D4000) {
        *lr_offset = (inst & 0x8000) == 0x8000 ? 2 : 1;
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* str lr, [sp, #-4]! */
    if (inst == 0xE52DE004) {
        *lr_offset = 1;
        return 1;
    }

    return -1;
}

/* check if Function return instruction, return framesize popped */
static int arm_backtraceInsFuncRet(unsigned int inst)
{
    /* pop pc */
    if ((inst & 0x0FFF8000) == 0x08BD8000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* bx lr */
    if ((inst & 0x0FFFFFFF) == 0x012FFF1E) {
        return 0;
    }

    /* ldr pc, [sp], #4 */
    if ((inst & 0x0FFFFFFF) == 0x049DF004) {
        return 1;

    }

    return -1;
}

/* check if open stack instruction, return framesize pushed */
static int arm_backtraceInsStackOpen(unsigned int inst)
{
    unsigned int   sub;
    unsigned int   shift;

    /* find "sub sp, sp, ..." */
    if ((inst & 0x0FFFF000) == 0x024DD000) {
        sub = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if(shift != 0) {
            shift = 32 - 2*shift;
            sub = sub << shift;
        }
        return sub/4;
    }

    /* find "push" */
    if ((inst & 0x0FFF0000) == 0x092D0000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* find "vpush" */
    if ((inst & 0x0FBF0F00) == 0x0D2D0B00) {
        return inst & 0xFF;
    }

    /* find "str *, [sp, #-4]!" */
    if ((inst & 0xFFFF0FFF) == 0xE52D0004) {
        return 1;
    }

    return -1;
}

/* check if open stack instruction, before 'push' pushed */
static int arm_backtraceInsStackOpenBeforePushLR(unsigned int inst)
{
    /* example:
       sub	sp, sp, #8
       push	{r4, r5, lr} */
    /* find "sub sp, sp, ..." */
    if ((inst & 0xFFFFFF00) == 0xE24DD000) {
        return (inst & 0xFF)/4;
    }

    /* example:
       push	{r0, r1, r2, r3}
       push	{r0, r1, r2, lr} */
    /* find "push" */
    if ((inst & 0xFFFFFFF0) == 0xE92D0000) {
        return __builtin_popcount(inst & 0xF);
    }

    return -1;
}
/* check if close stack instruction, return framesize pushed */
static int arm_backtraceInsStackClose(unsigned int inst)
{
    unsigned int   sub;
    unsigned int   shift;

    /* find "sub sp, sp, ..." */
    if ((inst & 0x0FFFF000) == 0x024DD000) {
        sub = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if(shift != 0) {
            shift = 32 - 2*shift;
            sub = sub << shift;
        }
        return sub/4;
    }

    /* find "pop" */
    if ((inst & 0x0FFF0000) == 0x08BD0000) {
        return __builtin_popcount(inst & 0xFFFF);
    }

    /* find "vpop" */
    if ((inst & 0x0FBF0F00) == 0x0CBD0B00) {
        return inst & 0xFF;
    }

    /* find "ldr *, [sp], #4" */
    if ((inst & 0x0FFF0FFF) == 0x049D0004) {
        return 1;
    }

    return -1;
}


/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int arm_backtraceFromStack(int **pSP, char **pPC,
                                  int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    int  *SP       = *pSP;
    char *PC       = *pPC;
    char *LR;
    int   i;
    int   temp;
    int   framesize;
    int   offset = 0;
    unsigned int   ins32;

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if ((unsigned long)SP == (unsigned long)debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    } else if ((g_intrpt_nested_level[cpu_cur_get()] == 0u)
               && ((unsigned long)SP > (unsigned long)debug_task_stack_bottom(NULL))) {
        if (print_func != NULL) {
            print_func("backtrace : invalid task sp\r\n");
        }
        return 1;
    } else if ((g_intrpt_nested_level[cpu_cur_get()] > 0u)
               && ((unsigned long)SP > (unsigned long)&sys_stack_top)) {
        if (print_func != NULL) {
            print_func("backtrace : invalid interrupt sp\r\n");
        }
        return 1;
    }

    /* 1. scan code, find lr pushed */
    for (i = 4; i < BT_FUNC_LIMIT; i += 4) {
        CodeAddr = PC - i;
        if ( BT_CHK_PC_AVAIL(CodeAddr) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid pc\r\n");
            }
            return -1;
        }

        ins32 = *(unsigned int *)CodeAddr;
        framesize = arm_backtraceInsPushLR(ins32, &offset);
        if (framesize >= 0) {
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

    /* 2. scan code, find frame size from "sub" and "vpush" */
    for (i = 4; CodeAddr + i < PC; i += 4) {
        ins32 = *(unsigned int *)(CodeAddr + i);
        temp = arm_backtraceInsStackOpen(ins32);
        if ( temp >= 0 ) {
            framesize += temp;
        }
    }

    if ( BT_CHK_PC_AVAIL(CodeAddr - 4) == 0) {
        if (print_func != NULL) {
            print_func("backtrace : invalid pc\r\n");
        }
        return -1;
    }

    /* 3. check if there is stack open befre LR pushed */
    ins32 = *(unsigned int *)(CodeAddr - 4);
    temp = arm_backtraceInsStackOpenBeforePushLR(ins32);
    if ( temp >= 0 ) {
        framesize += temp;
        offset += temp;
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR = (char *)*(SP + framesize - offset);
    if ( BT_CHK_PC_AVAIL(LR) == 0) {
        if (print_func != NULL) {
            print_func("backtrace : invalid lr\r\n");
        }
        return -1;
    }
    *pSP   = SP + framesize;
    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("LR=%p  next_PC=%p   next_SP=%p offset=%d\n\r", LR, *pPC, *pSP, offset);
#endif

    return offset == 0 ? 1 : 0;
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int arm_backtraceFromLR(int **pSP, char **pPC, char *LR,
                               int (*print_func)(const char *fmt, ...))
{
    int  *SP       = *pSP;
    char *PC       = *pPC;
    char *CodeAddr = NULL;
    int   i;
    int   temp;
    int   framesize = 0;
    int   offset = 0;
    unsigned int   ins32;

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("SP = %p, PC = %p, LR = %p\n\r", *pSP, *pPC, LR);
#endif

    if (BT_CHK_PC_AVAIL(PC) == 0) {
        if ( BT_CHK_PC_AVAIL(LR) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid lr\r\n");
            }
            return -1;
        }
        offset = backtraceFindLROffset(LR, print_func);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    /* 1. scan code, find frame size from "pop" or "bx lr" or "ldr pc" */
    for (i = 0; i < BT_FUNC_LIMIT; i += 4) {
        CodeAddr = PC + i;
        if ( BT_CHK_PC_AVAIL(CodeAddr) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid pc\r\n");
            }
            return -1;
        }
        ins32 = *(unsigned int *)CodeAddr;
        framesize = arm_backtraceInsFuncRet(ins32);
        if (framesize >= 0) {
            break;
        }

        temp = arm_backtraceInsPushLR(ins32, &offset);
        if (temp >= 0) {
            framesize = 0;
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

    /* 2. scan code, find frame size from "sub" and "vpush" */
    for (i = 4; CodeAddr - i >= PC; i += 4) {
        ins32 = *(unsigned int *)(CodeAddr - i);
        temp = arm_backtraceInsStackClose(ins32);
        if ( temp >= 0 ) {
            framesize += temp;
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    if ( BT_CHK_PC_AVAIL(LR) == 0) {
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

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int thumb_backtraceFromStack(int **pSP, char **pPC,
                                    int (*print_func)(const char *fmt, ...))
{
    char          *CodeAddr = NULL;
    int           *SP       = *pSP;
    char          *PC       = BT_PC2ADDR(*pPC);
    char          *LR;
    int            i;
    unsigned short ins16;
    unsigned int   ins32;
    unsigned int   framesize = 0;
    unsigned int   shift = 0;
    unsigned int   sub = 0;
    unsigned int   offset    = 1;

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("SP = %p, PC = %p\n\r", *pSP, *pPC);
#endif

    if ((unsigned long)SP == (unsigned long)debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : ^task entry^\r\n");
        }
        return 1;
    } else if ((unsigned long)SP > (unsigned long)debug_task_stack_bottom(NULL)) {
        if (print_func != NULL) {
            print_func("backtrace : invalid sp\r\n");
        }
        return 1;
    }

    /* func call ways:
       1. "stmdb sp!, ..." or "push ..." to open stack frame and save LR
       2. "sub  sp, ..." or "sub.w  sp, ..." to open stack more
       3. call
       */

    /* 1. scan code, find frame size from "push" or "stmdb sp!" */
    for (i = 2; i < BT_FUNC_LIMIT; i += 2) {
        if ( BT_CHK_PC_AVAIL(PC - i - 2) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid pc\r\n");
            }
            return -1;
        }
        /* find nearest "push   {..., lr}" */
        ins16 = *(unsigned short *)(PC - i);
        if ((ins16 & 0xff00) == 0xb500) {
            framesize = __builtin_popcount((unsigned char)ins16);
            framesize++;
            ins16 = *(unsigned short *)(PC - i - 2);
            /* find double push */
            if ((ins16 & 0xfff0) == 0xb400) {
                /* "push" before LR pushed */
                offset += __builtin_popcount((ins16 & 0xf));
                framesize += __builtin_popcount((ins16 & 0xf));
            } else if(ins16 == 0xb082) {
                /* "sub sp, #8" before LR pushed */
                offset += 2;
                framesize += 2;
            }
            CodeAddr = PC - i;
            break;
        }

        /* find "stmdb sp!, ..." */
        /* The Thumb instruction stream is a sequence of halfword-aligned
         * halfwords */
        ins32 = *(unsigned short *)(PC - i);
        ins32 <<= 16;
        ins32 |= *(unsigned short *)(PC - i + 2);
        if ((ins32 & 0xFFFF4000) == 0xe92d4000) {
            framesize = __builtin_popcount(ins32 & 0xffff);
            CodeAddr = PC - i;
            break;
        }
    }

    if (CodeAddr == NULL) {
        /* error branch */
        if (print_func != NULL) {
            print_func("Backtrace fail!\r\n");
        }
        return -1;
    }

    /* 2. scan code, find frame size from "sub" or "sub.w" */
    for (i = 0; i < BT_FUNC_LIMIT;) {
        if (CodeAddr + i > PC) {
            break;
        }
        /* find "sub    sp, ..." */
        ins16 = *(unsigned short *)(CodeAddr + i);
        if ((ins16 & 0xff80) == 0xb080) {
            framesize += (ins16 & 0x7f);
            i += 2;
            continue;
        }

        /* find "sub.w	sp, sp, ..." */
        ins32 = *(unsigned short *)(CodeAddr + i);
        ins32 <<= 16;
        ins32 |= *(unsigned short *)(CodeAddr + i + 2);
        if ((ins32 & 0xFBFF8F00) == 0xF1AD0D00) {
            sub = 128 + (ins32 & 0x7f);
            shift  = (ins32 >> 7) & 0x1;
            shift += ((ins32 >> 12) & 0x7) << 1;
            shift += ((ins32 >> 26) & 0x1) << 4;
            framesize += sub<<(30 - shift);
            i += 4;
            continue;
        }

        /* find "vpush" */
        if ((ins32 & 0x0FBF0F00) == 0x0D2D0B00) {
            framesize += (ins32 & 0x7f);
            i += 4;
            continue;
        }

        if ((ins16 & 0xf800) >= 0xe800) {
            i += 4;
        } else {
            i += 2;
        }
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    LR     = (char *)*(SP + framesize - offset);
    if ( BT_CHK_PC_AVAIL(LR - 5) == 0) {
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

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int thumb_backtraceFromLR(int **pSP, char **pPC, char *LR,
                                 int (*print_func)(const char *fmt, ...))
{
    int           *SP       = *pSP;
    char          *PC       = BT_PC2ADDR(*pPC);
    char          *CodeAddr = NULL;
    int            i;
    unsigned short ins16;
    unsigned int   framesize = 0;
    unsigned int   offset = 0;

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("SP = %p, PC = %p, LR = %p\n\r", *pSP, *pPC, LR);
#endif

    if (BT_CHK_PC_AVAIL(PC) == 0) {
        if ( BT_CHK_PC_AVAIL(LR) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid lr\r\n");
            }
            return -1;
        }
        offset = backtraceFindLROffset(LR, print_func);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    /*find stack framesize:
       1. "push ..." to open stack
       2. "sub  sp, ..." to open stack
       3. 1 + 2
       4. do not open stack
       */

    /* 1. scan code, find frame size from "push" or "sub" */
    for (i = 2; i < BT_FUNC_LIMIT; i += 2) {
        if ( BT_CHK_PC_AVAIL(PC - i - 2) == 0) {
            if (print_func != NULL) {
                print_func("backtrace : invalid pc\r\n");
            }
            return -1;
        }
        ins16 = *(unsigned short *)(PC - i);
        /* find "push   {..., lr}" */
        if ((ins16 & 0xff00) == 0xb500) {
            /* another function */
            break;
        }
        /* find "push   {...}" */
        if ((ins16 & 0xff00) == 0xb400) {
            framesize = __builtin_popcount((unsigned char)ins16);
            CodeAddr  = PC - i;
            break;
        }
        /* find "sub    sp, ..." */
        if ((ins16 & 0xff80) == 0xb080) {
            framesize = (ins16 & 0x7f);
            CodeAddr  = PC - i;
            /* find push before sub */
            ins16 = *(unsigned short *)(PC - i - 2);
            if ((ins16 & 0xff00) == 0xb400) {
                framesize += __builtin_popcount((unsigned char)ins16);
                CodeAddr = PC - i - 2;
            }
            break;
        }
    }

    /* 2. check the "push" or "sub sp" belongs to another function */
    if (CodeAddr != NULL) {
        for (i = 2; i < PC - CodeAddr; i += 2) {
            ins16 = *(unsigned short *)(PC - i);
            /* find "pop   {..., pc}" or "bx   lr" */
            if ((ins16 & 0xff00) == 0xbd00 || ins16 == 0x4770) {
                /* SP no changed */
                framesize = 0;
            }
        }
    } /* else: SP no changed */

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("frsz = %d offset = %d SP=%p\n\r", framesize, offset, SP);
#endif

    /* 3. output */
    if ( BT_CHK_PC_AVAIL(LR - 5) == 0) {
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


/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int backtraceFromStack(int **pSP, char **pPC,
                              int (*print_func)(const char *fmt, ...))
{
    if ( BT_CHK_PC_AVAIL(*pPC) == 0) {
        return -1;
    }

    if (BT_CHK_PC_THUMB(*pPC)) {
        return thumb_backtraceFromStack(pSP, pPC, print_func);
    } else {
        return arm_backtraceFromStack(pSP, pPC, print_func);
    }
}

/* find current function caller, update PC and SP
   returns: 0  success
            1  success and find buttom
            -1 fail */
static int backtraceFromLR(int **pSP, char **pPC, char *LR,
                           int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_THUMB(*pPC)) {
        return thumb_backtraceFromLR(pSP, pPC, LR, print_func);
    } else {
        return arm_backtraceFromLR(pSP, pPC, LR, print_func);
    }
}

/* printf call stack
   return levels of call stack */
int backtrace_now(char *pPC, int *pSP, int bt_space, int (*print_func)(const char *fmt, ...))
{
    int   lvl;

    char *PC = pPC;
    int  *SP = pSP;
    int CPSR = RHINO_GET_CPSR();

    backtrace_space = bt_space;

    if (print_func == NULL) {
        print_func = printf;
    }

    if (bt_space == BACKTRACE_KSPACE) {
        PC = RHINO_GET_PC();
        SP = RHINO_GET_SP();
    }
    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( CPSR & 0x20 ) {
        BT_MODESET_THUMB(PC);
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("%s %p %p 0x%x\r\n", __func__, PC, SP, CPSR);
#endif

    if (bt_space == BACKTRACE_KSPACE) {
        print_func("========== Kernel Call stack ==========\r\n");
    } else if (bt_space == BACKTRACE_USPACE) {
        print_func("========== Uspace Call stack ==========\r\n");
    }
    /*
        for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
            ret = backtraceFromStack(&SP, &PC, print_func);
            if (ret != 0) {
                break;
            }
        }
    */
    lvl = backtrace_caller(PC, SP, print_func);
    print_func("==========    End     ==========\r\n");

    backtrace_space = BACKTRACE_KSPACE;

    return lvl;
}

#if (RHINO_CONFIG_USER_SPACE > 0)

typedef enum {
    TASK_RUN_IN_KERNEL,       /* KERNEL TASK*/
    TASK_RUN_IN_USER_KERNEL,  /* USER TASK RUN IN SVC MODE */
    TASK_RUN_IN_USER,         /* USER TASK RUN IN USER MODE */
    TASK_RUN_UNDEFINED
} uspace_task_mode_type;

static uspace_task_mode_type task_running_mode_get(ktask_t* task)
{
    uspace_task_mode_type type = TASK_RUN_UNDEFINED;
    uint32_t cpsr_process_mode, pid;
    context_t * context = task->task_stack;

    cpsr_process_mode = context->CPSR & CPSR_PROCESS_MODE_MASK;
    pid               = task->pid;

    if(cpsr_process_mode == CPSR_Mode_USR) {
        /* panic in usr */
        type = TASK_RUN_IN_USER;
    } else {
        if (pid == 0) { /*panic in kernel*/
            type = TASK_RUN_IN_KERNEL;
        } else { /* panic in kernel by usr syscall*/
            type = TASK_RUN_IN_USER_KERNEL;
        }
    }
    return type;
}
#endif

void backtrace_task_distinguish(ktask_t* task, char *PC, int *SP, char *LR,
                                int (*print_func)(const char *fmt, ...))
{
#if (RHINO_CONFIG_USER_SPACE > 0)
    static int  *U_SP = NULL;
    static char *U_PC = NULL;
    static char *U_LR = NULL;

    uspace_task_mode_type type = task_running_mode_get(task);

    /* need uspace backtrace usr-kernel */
    if (type == TASK_RUN_IN_USER_KERNEL) {
        debug_usapce_context_get((void *)task, &U_PC, &U_LR, &U_SP);
        print_func("========== Uspace Call stack ======\r\n");
        backtrace_handle(U_PC, U_SP, U_LR, BACKTRACE_USPACE, print_func);
    }

    if (type == TASK_RUN_IN_USER) {
        /* PS/SP/LR have saved */
        print_func("========== Uspace Call stack ======\r\n");
        backtrace_handle(PC, SP, LR, BACKTRACE_USPACE, print_func);
        return;
    }
#endif
    print_func("========== Kernel Call stack ==========\r\n");
    backtrace_handle(PC, SP, LR, BACKTRACE_KSPACE, print_func);
}


void backtrace_task(void* task, int (*print_func)(const char *fmt, ...))
{
    char    *PC;
    char    *LR;
    int     *SP;
    int      ret;

    getPLSfromTaskCtx(((ktask_t *)task)->task_stack, &PC, &LR, &SP);
#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("%s %p %p %p %p %p\n", ((ktask_t *)task)->task_name, task, ((ktask_t *)task)->task_stack, PC, LR, SP);
#endif

    ret = debug_task_is_running((ktask_t*)task);
    switch ( ret ) {
    case 0 :
    case 1 :
    case 2 :
        backtrace_task_distinguish((ktask_t*)task, PC, SP, LR, print_func);
        break;

    default:
        print_func("Status of task \"%s\" is 'Running', Can not backtrace!\n",
                   ((ktask_t *)task)->task_name ? ((ktask_t *)task)->task_name : "anonym");
        break;
    }

    print_func("==========    End     ==========\r\n");

}

/* call from panic, show running task info */
int backtrace_task_now(int (*print_func)(const char *fmt, ...))
{
    char *taskname = NULL;
    char *PC;
    char *LR;
    int  *SP;
    int   i;
    int   lvl;
    char  panic_call[] = "backtrace : 0x         \r\n";
    char  name_show[]  = "TaskName  :                      \r\n";
    ktask_t_shadow *task;

    if (print_func == NULL) {
        print_func = printf;
    }

    print_func("========= Running Task =========\r\n");
    task = (ktask_t_shadow *)debug_task_find_running(&taskname);
    if (task == NULL) {
        print_func("Task not found\n");
        return 0;
    }

    getPLSfromTaskCtx(task->task_stack, &PC, &LR, &SP);

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("%s %p %p %p %p %p\n", taskname, task, task->task_stack, PC, LR, SP);
#endif

    /* set name */
    for (i = 0; i < 20; i++) {
        name_show[12 + i] = ' ';
    }
    for (i = 0; i < 20; i++) {
        if (taskname[i] == '\0') {
            break;
        }
        name_show[12 + i] = taskname[i];
    }
    print_func(name_show);

    k_int2str_hex((int)BT_PC2ADDR(PC), &panic_call[14]);
    print_func(panic_call);

    lvl = backtrace_caller(PC, SP, print_func);
    if ( lvl == 0 ) {
        lvl = backtrace_callee(PC, SP, LR, print_func);
    }

    print_func("==========    End     ==========\r\n");

    return lvl;
}

/* backtrace start with PC and SP, find LR from stack memory
   return levels of call stack */
int backtrace_caller(char *PC, int *SP,
                     int (*print_func)(const char *fmt, ...))
{
    int  *bt_sp;
    char *bt_pc;
    int   lvl, ret;
    char  s_panic_call[] = "backtrace : 0x         \r\n";

    /* caller must save LR in stack, so find LR from stack */

    if (SP == NULL) {
        return 0;
    }

    /* assume right! print */
    k_int2str_hex((int)BT_PC2ADDR(PC), &s_panic_call[14]);
    if (print_func != NULL) {
        print_func(s_panic_call);
    }
    bt_sp = SP;
    bt_pc = PC;
    ret   = -1;
    for (lvl = 1; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
        if (ret != 0) {
            break;
        }
    }

    return lvl;
}

/* backtrace start with PC SP and LR
   return levels of call stack */
int backtrace_callee(char *PC, int *SP, char *LR,
                     int (*print_func)(const char *fmt, ...))
{
    int  *bt_sp;
    char *bt_pc;
    char *bt_lr;
    int   lvl, ret;
    char  s_panic_call[] = "backtrace : 0x         \r\n";

    if (SP == NULL) {
        return 0;
    }

    /* Backtrace: assume ReturnAddr is saved in LR when exception */
    k_int2str_hex((int)BT_PC2ADDR(PC), &s_panic_call[14]);
    if (print_func != NULL) {
        print_func(s_panic_call);
    }
    lvl   = 1;
    bt_sp = SP;
    bt_pc = PC;
    bt_lr = LR;
    ret   = backtraceFromLR(&bt_sp, &bt_pc, bt_lr, print_func);
    if (ret == 0) {
        for (; lvl < BT_LVL_LIMIT; lvl++) {
            ret = backtraceFromStack(&bt_sp, &bt_pc, print_func);
            if (ret != 0) {
                break;
            }
        }
    }

    return lvl;
}


void *g_back_trace;
/**
 Get call stack, return levels of call stack
 trace[] output buffer
 size    buffer size
 offset  which lvl start
 */
int backtrace_now_get(char *pPC, int *pSP, int bt_space, void *trace[], int size, int offset)
{
    int   lvl;
    int   ret;

    char *PC = pPC;
    int  *SP = pSP;
    int CPSR = RHINO_GET_CPSR();

    backtrace_space = bt_space;

    if (bt_space == BACKTRACE_KSPACE) {
        PC = RHINO_GET_PC();
        SP = RHINO_GET_SP();
    }

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( CPSR & 0x20 ) {
        BT_MODESET_THUMB(PC);
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("%s %p %p 0x%x\r\n", __func__, PC, SP, CPSR);
#endif

    memset(trace, 0, size*sizeof(void *));

    g_back_trace = trace;

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
    backtrace_space = BACKTRACE_KSPACE;
    return lvl - offset < 0 ? 0 : lvl - offset;
}

int backtrace_task_get(void *task, void *trace[], int level, int offset)
{
    int   lvl;
    int   ret;

    char *PC;
    int  *SP;
    char *LR;

    int CPSR = RHINO_GET_CPSR();

    if (task == NULL) {
        return 0;
    }

    uspace_task_mode_type type = task_running_mode_get(task);

    if (TASK_RUN_IN_KERNEL == type) {
        backtrace_space = BACKTRACE_KSPACE;
        getPLSfromTaskCtx(((ktask_t *)task)->task_stack, &PC, &LR, &SP);
    } else {
        backtrace_space = BACKTRACE_USPACE;
        debug_usapce_context_get((void *)task, &PC, &LR, &SP);
    }

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( CPSR & 0x20 ) {
        BT_MODESET_THUMB(PC);
    }

#ifdef OS_BACKTRACE_DEBUG
    OS_PRINT_DBG("%s %p %p 0x%x\r\n", __func__, PC, SP, CPSR);
#endif

    if (level < 1) {
        return 0;
    }

    memset(trace, 0, level*sizeof(void *));

    trace[0] = PC;
    level   -= 1;

    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, NULL);
        if (ret < 0) {
            break;
        }
        if (lvl >= offset && lvl - offset < level) {
            trace[lvl - offset + 1] = PC;
        }
        if (lvl - offset >= level) {
            break;
        }
        if (ret == 1) {
            break;
        }
    }

    return lvl - offset < 0 ? 0 : lvl - offset;
}

void backtrace_handle(char *PC, int *SP, char *LR, int space_type,
                      int (*print_func)(const char *fmt, ...))
{
    if (SP == NULL) {
        print_func("SP is NULL, Can't backtrace\r\n");
        return;
    }

    backtrace_space = space_type;

    if (backtrace_caller(PC, SP, print_func) > 0) {
        /* Backtrace 1st try: assume ReturnAddr is saved in stack when exception */
        /* backtrace success, do not try other way */
        goto exit;
    } else if (backtrace_callee(PC, SP, LR, print_func) > 0) {
        /* Backtrace 2nd try: assume ReturnAddr is saved in LR when exception */
        /* backtrace success, do not try other way */
        goto exit;
    } else {
        /* Backtrace 3rd try: assume PC is invalalb, backtrace from LR */
        backtrace_caller(LR, SP, print_func);
    }

exit:
    backtrace_space = BACKTRACE_KSPACE;
    return;
}

/* return 0: kspace 1: uspace*/
int check_backtrace_space(void)
{
    return (backtrace_space == BACKTRACE_USPACE);
}
