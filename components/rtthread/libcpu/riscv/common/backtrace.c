/*
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-23     WangShun     the first version
 * 2024-09-19     WangShun     support rv32
 */

#include <csi_core.h>
#include "rthw.h"
#include "rtthread.h"
#include "stack.h"

#if CONFIG_CPU_XUANTIE_E902 || CONFIG_CPU_XUANTIE_E902M || CONFIG_CPU_XUANTIE_E902MT || CONFIG_CPU_XUANTIE_E902T
#define LS_ADDR_NOT_ALIGNED_SUPPORT 0
#else
#define LS_ADDR_NOT_ALIGNED_SUPPORT 1
#endif

/* Please check that the following symbols are defined in the linked scripts ！*/ 
/* If not, define the following symbols at the beginning and end of the text segment */
extern char *__etext;
extern char *__stext;

#define BT_CHK_PC_AVAIL(pc)   ((uintptr_t)(pc) < (uintptr_t)(&__etext) \
                              && (uintptr_t)(pc) > (uintptr_t)(&__stext))

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

#define BT_PC2ADDR(pc)        ((char *)(((uintptr_t)(pc))))

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

/* get framesize from c ins */
static int riscv_backtrace_framesize_get(unsigned short inst)
{
#if __riscv_xlen == 64
#define IMM_RIGHT_SHIFT 3
#else
#define IMM_RIGHT_SHIFT 2
#endif
    unsigned int imm = 0;

    /* addi sp, sp, -im */
    if ((inst & 0xFF83) == 0x1101) {
        imm = (inst >> 2) & 0x1F;
        imm = (~imm & 0x1F) + 1;
        return imm >> IMM_RIGHT_SHIFT;
    }

    /* c.addi16sp sp, nzuimm6<<4 */
    if ((inst & 0xFF83) == 0x7101) {
        imm = (inst >> 3) & 0x3;
        imm = (imm << 1) | ((inst >> 5) & 0x1);
        imm = (imm << 1) | ((inst >> 2) & 0x1);
        imm = (imm << 1) | ((inst >> 6) & 0x1);
        imm = ((~imm & 0x1f) + 1) << 4;
        return imm >> IMM_RIGHT_SHIFT;
    }

    return -1;
}

static int riscv_backtrace_ra_offset_get1(unsigned int inst)
{
    unsigned int imm = 0;

#if __riscv_xlen == 64
    /* sd ra,552(sp) */
    if ((inst & 0x81FFF07F) == 0x113023) {
#else
    /* sw ra,552(sp) */
    if ((inst & 0x81FFF07F) == 0x112023) {
#endif
        imm = (inst >> 7) & 0x1F;
        imm |= ((inst >> 25) & 0x7F) << 5;
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm >> 3;
    }

    return -1;
}

/* get ra position in the stack */
static int riscv_backtrace_ra_offset_get(unsigned short inst)
{
    unsigned int imm = 0;
#if __riscv_xlen == 64
    /* c.fsdsp rs2, uimm6<<3(sp) */
    if ((inst & 0xE07F) == 0xE006) {
        imm = (inst >> 7) & 0x7;
        imm = (imm << 3) | ((inst >> 10) & 0x7);
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm;
    }
#else
    if ((inst & 0xE07F) == 0xc006) {
        imm = (inst >> 7) & 0x3;
        imm = (imm << 4) | ((inst >> 9) & 0xf);
        /* The unit is size_t, So we don't have to move 2 bits to the left */
        return imm;
    }
#endif
    return -1;
}

/* get the offset between the jump instruction and the return address */
static int backtraceFindLROffset(char *LR, int (*print_func)(const char *fmt, ...))
{
    int offset = 0;
    char *LR_indeed;
    unsigned int ins32;

    LR_indeed = BT_PC2ADDR(LR);

    /* Usually jump using the JAL instruction */
#if !LS_ADDR_NOT_ALIGNED_SUPPORT
    ins32 = *(unsigned char *)(LR_indeed - 4 + 3);
    ins32 <<= 8;
    ins32 |= *(unsigned char *)(LR_indeed - 4 + 2);
    ins32 <<= 8;
    ins32 |= *(unsigned char *)(LR_indeed - 4 + 1);
    ins32 <<= 8;
    ins32 |= *(unsigned char *)(LR_indeed - 4);
#else
    ins32 = *(unsigned int *)(LR_indeed - 4);
#endif
    if ((ins32 & 0x3) == 0x3) {
        offset = 4;
    } else {
        offset = 2;
    }

    if (print_func != NULL) {
        print_func("backtrace : 0x%lx\n", (unsigned long)LR_indeed - offset);
    }

    return offset;
}

static int riscv_backtraceFromStack(long **pSP, char **pPC, int lvl,
                                    int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    long  *SP      = *pSP;
    char *PC       = *pPC;
    char *LR; // ra
    int   i;
    int   framesize;
    int   offset = 0;
    unsigned int ins32;
    unsigned short ins16;

    /* 1. scan code, find lr pushed */
    for (i = 0; i < BT_FUNC_LIMIT;) {
        CodeAddr = (char *)(PC - i);
#if !LS_ADDR_NOT_ALIGNED_SUPPORT
        ins32 = *(unsigned char *)(CodeAddr + 3);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr + 2);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr + 1);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr);
#else
        ins32 = *(unsigned int *)(CodeAddr);
#endif
        if ((ins32 & 0x3) == 0x3) {
#if !LS_ADDR_NOT_ALIGNED_SUPPORT
            ins16 = *(unsigned char *)(CodeAddr - 2 + 1);
            ins16 <<= 8;
            ins16 |= *(unsigned char *)(CodeAddr - 2);
#else
            ins16 = *(unsigned short *)(CodeAddr - 2);
#endif
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
        ins16 = (ins32) & 0xffff;
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
    for (i = 0;;) {
        if (lvl == 0) {
            if (i >= BT_FUNC_LIMIT)
                break;
        } else {
            if (CodeAddr + i >=  PC) {
                break;
            }
        }
#if !LS_ADDR_NOT_ALIGNED_SUPPORT
        ins32 = *(unsigned char *)(CodeAddr + i + 3);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr + i + 2);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr + i + 1);
        ins32 <<= 8;
        ins32 |= *(unsigned char *)(CodeAddr + i);
#else
        ins32 = *(unsigned int *)(CodeAddr + i);
#endif
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

    /* 3. output */
    if (lvl == 0) {
        LR = (char *) * (SP - framesize + offset);
    } else {
        LR = (char *) * (SP + offset);
    }

    if (BT_CHK_PC_AVAIL(LR) == 0) {
        if (print_func != NULL) {
            print_func("End of stack backtracking\r\n");
        }
        return -1;
    }
    if (lvl == 0) {
        *pSP = SP;
    } else {
        *pSP = SP + framesize;
    }

    offset = backtraceFindLROffset(LR, print_func);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

static int backtraceFromStack(long **pSP, char **pPC, int lvl,
                              int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_PC_AVAIL(*pPC) == 0) {
        if (print_func != NULL) {
            print_func("pc is not in range.\r\n");
        }
        return -1;
    }

    return riscv_backtraceFromStack(pSP, pPC, lvl, print_func);
}

rt_err_t rt_hw_backtrace_frame_unwind(rt_thread_t thread, struct rt_hw_backtrace_frame *frame)
{
    int ret;
    int lvl;
    char *PC;
    long *SP;

    if (thread == NULL || frame == NULL) {
        return RT_EINVAL;
    }

    PC = (char *)frame->pc;
    SP = (long *)frame->fp;

    // rt_kprintf("=========>PC: %p, SP: %p\n", PC, SP);
    rt_kprintf("------------------------------Thread: %s backtrace------------------------------\r\n", thread->parent.name);
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, lvl, rt_kprintf);
        if (ret != 0) {
            break;
        }
    }
    rt_kprintf("\r\n");
    return ret;
}

#define ARCH_CONTEXT_FETCH(pctx, id)    (*(((unsigned long *)pctx) + (id)))

static int arch_context_offset_from_start(void)
{
    int offset = 0;

#if !CONFIG_CPU_XUANTIE_E9XX
#if defined(__riscv_flen) && defined(ENABLE_FPU)
    offset += sizeof(rt_hw_stack_f_frame_t);
#endif
#endif
#if defined(__riscv_vector) && defined(ENABLE_VECTOR)
    int vlenb = csi_vlenb_get_value();
    offset += sizeof(rt_hw_stack_v_frame_t) + CTX_VECTOR_REG_NR * vlenb;
#endif
#if defined(__riscv_matrix) || defined(__riscv_xtheadmatrix)
    int xmlenb = csi_xmlenb_get_value();
    offset += sizeof(rt_hw_stack_x_frame_t) + xmlenb * 8;
#endif
    return offset;
}

rt_err_t rt_hw_backtrace_frame_get(rt_thread_t thread, struct rt_hw_backtrace_frame *frame)
{
    rt_err_t rc;
    int offset;

    if (!thread || !frame)
    {
        rc = -RT_EINVAL;
    }
    else
    {
        offset = arch_context_offset_from_start();
        frame->pc = ARCH_CONTEXT_FETCH(thread->sp, -offset);
        frame->fp = ARCH_CONTEXT_FETCH(thread->sp, -offset + 1);
        rc = RT_EOK;
    }
    return rc;
}