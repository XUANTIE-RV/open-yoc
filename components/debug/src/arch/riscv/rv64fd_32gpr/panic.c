/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <csi_core.h>
#include <k_arch.h>

#define REG_NAME_WIDTH 9
extern volatile uint32_t g_crash_steps;

typedef struct {
    long X1;
    long X2;
    long X3;
    long X4;
    long X5;
    long X6;
    long X7;
    long X8;
    long X9;
    long X10;
    long X11;
    long X12;
    long X13;
    long X14;
    long X15;
    long X16;
    long X17;
    long X18;
    long X19;
    long X20;
    long X21;
    long X22;
    long X23;
    long X24;
    long X25;
    long X26;
    long X27;
    long X28;
    long X29;
    long X30;
    long X31;
    long MS_EPC;
    long MS_STATUS;
    long MS_CAUSE;
    long MS_TVAL;
} fault_context_t;

static char *k_ll2str(intptr_t num, char *str)
{
    int i = 15;
    char         index[] = "0123456789ABCDEF";
    uintptr_t usnum   = (uintptr_t)num;

    for(; i >= 0; i--) {
        str[i] = index[usnum % 16];
        usnum /= 16;
    }

    return str;
}

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    fault_context_t *rv64_context = (fault_context_t *)context;

    *pSP = (int *)rv64_context->X2;
    *pPC = (char *)rv64_context->MS_EPC;
    *pLR = (char *)rv64_context->X1;
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int        x;
    long       *regs = (long *)context;
    char       s_panic_regs[REG_NAME_WIDTH + 14 + 8];
    /* fault_context_t */
    char s_panic_ctx[] = "X1(ra)   "
                         "X2(sp)   "
                         "X3(gp)   "
                         "X4(tp)   "
                         "X5(t0)   "
                         "X6(t1)   "
                         "X7(t2)   "
                         "X8(s0)   "
                         "X9(s1)   "
                         "X10(a0)  "
                         "X11(a1)  "
                         "X12(a2)  "
                         "X13(a3)  "
                         "X14(a4)  "
                         "X15(a5)  "
                         "X16(a6)  "
                         "X17(a7)  "
                         "X18(s2)  "
                         "X19(s3)  "
                         "X20(s4)  "
                         "X21(s5)  "
                         "X22(s6)  "
                         "X23(s7)  "
                         "X24(s8)  "
                         "X25(s9)  "
                         "X26(s10) "
                         "X27(s11) "
                         "X28(t3)  "
                         "X29(t4)  "
                         "X30(t5)  "
                         "X31(t6)  "
                         "MS_EPC   "
                         "MS_STATUS"
                         "MCAUSE   "
                         "MTVAL    ";

    if (regs == NULL) {
        return;
    }

    print_func("========== Regs info  ==========\r\n");
    /* show fault_context_t */
    for (x = 0; x < sizeof(s_panic_ctx) / REG_NAME_WIDTH; x++) {
        memcpy(&s_panic_regs[0], &s_panic_ctx[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_ll2str(regs[x], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11 + 8] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12 + 8] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13 + 8] = 0;
        print_func(s_panic_regs);
    }
}

#if defined(RHINO_CONFIG_CLI_AS_NMI) && (RHINO_CONFIG_CLI_AS_NMI > 0)
void panicNmiInputFilter(uint8_t ch)
{
    static int  check_cnt = 0; /* for '$#@!' */

    if ( ch == '$' && check_cnt == 0) {
        check_cnt++;
    } else if ( ch == '#' && check_cnt == 1) {
        check_cnt++;
    } else if ( ch == '@' && check_cnt == 2) {
        check_cnt++;
    } else if ( ch == '!' && check_cnt == 3) {
        panicNmiFlagSet();
        __asm__ __volatile__("udf":::"memory");
    } else {
        check_cnt = 0;
    }
}
#else
void panicNmiInputFilter(uint8_t ch) {}
#endif

extern void panicHandler(void *context);
extern int32_t aos_debug_printf(const char *fmt, ...);
void exceptionHandler(void *context)
{
    g_crash_steps++;
    if (g_crash_steps > 1) {
        context = NULL;
    } else {
        fault_context_t *fc = (fault_context_t *)context;
        aos_debug_printf("Exception ++++++++++ MS_EPC 0x%lx, MS_STATUS 0x%lx, MS_TVAL 0x%lx, CPU Exception: NO.0x%lx\r\n",
                         fc->MS_EPC, fc->MS_STATUS, fc->MS_TVAL, fc->MS_CAUSE);
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
        extern bool csi_irq_context(void);
        extern volatile int g_kernel_elf_post_prepare_ok;
        if (fc->MS_CAUSE == CAUSE_FETCH_PAGE_FAULT || fc->MS_CAUSE == CAUSE_LOAD_PAGE_FAULT ||
            fc->MS_CAUSE == CAUSE_STORE_PAGE_FAULT) {
            aos_debug_printf("page falt happens. kenel elf post prepare_ok: %d, in irq context: %d\r\n",
                             g_kernel_elf_post_prepare_ok, csi_irq_context());
        }
#endif
    }
    panicHandler(context);
}

