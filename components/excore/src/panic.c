/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "debug_api.h"
#include "k_api.h"
#include "aos/hal/uart.h"

int cpu_cpsr_get(void);

#if (DEBUG_CONFIG_PANIC > 0)

#if (RHINO_CONFIG_CPU_NUM > 1)
kspinlock_t g_panic_print_lock = {KRHINO_SPINLOCK_FREE_VAL,0,0};
#endif

#define REGS_TYPE \
 "PC      0x%08X\r\n\
LR      0x%08X\r\n\
SP      0x%08X\r\n\
CPSR    0x%08X\r\n\
R0      0x%08X\r\n\
R1      0x%08X\r\n\
R2      0x%08X\r\n\
R3      0x%08X\r\n\
R4      0x%08X\r\n\
R5      0x%08X\r\n\
R6      0x%08X\r\n\
R7      0x%08X\r\n\
R8      0x%08X\r\n\
R9      0x%08X\r\n\
R10     0x%08X\r\n\
R11     0x%08X\r\n\
R12     0x%08X\r\n"

#if (RHINO_CONFIG_USER_SPACE > 0)
#include "task_group.h"

#define DEBUG_SYSCALL_REG_SIZE 0x44

uspace_panic_type g_uspace_panic_type[RHINO_CONFIG_CPU_NUM] = {PANIC_UNDEFINED};

uspace_panic_type debug_uspace_panic_type_get(void)
{
    return g_uspace_panic_type[cpu_cur_get()];
}

void debug_uspace_panic_type_set(void *context)
{
    uint32_t cpsr_process_mode, pid;

    fault_context_t *flt_ctx = (fault_context_t *)context;

    uspace_panic_type type = PANIC_UNDEFINED;

    cpsr_process_mode = flt_ctx->cntx.CPSR & CPSR_PROCESS_MODE_MASK;
    pid               = g_active_task[cpu_cur_get()]->pid;

    if(cpsr_process_mode == CPSR_Mode_USR) {
        /* panic in usr */
        type = PANIC_IN_USER;
    } else {
        if (pid == 0) { /*panic in kernel*/
            type = PANIC_IN_KERNEL;
        } else { /* panic in kernel by usr syscall*/
            type = PANIC_IN_USER_KERNEL;
        }
    }

    g_uspace_panic_type[cpu_cur_get()] = type;
}

/* only in usr-kernel panic */
void debug_usapce_context_get(void* task, char **pPC, char **pLR, int **pSP)
{
    ktask_t *cur_task;
    uint32_t ktask_stask_addr;

    if(NULL == task) {
        cur_task = g_active_task[cpu_cur_get()];
    } else {
        cur_task = (ktask_t *)task;
    }

    ktask_stask_addr = (uint32_t)(cur_task->task_stack_base) + cur_task->stack_size * sizeof(cpu_stack_t) - DEBUG_SYSCALL_REG_SIZE;

    debug_syscall_context_t *syscall_context = (debug_syscall_context_t *)ktask_stask_addr;

    *pSP = (int *)syscall_context->SP;
    *pPC = (char *)syscall_context->PC;
    *pLR = (char *)syscall_context->LR;

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( syscall_context->CPSR & 0x20 ) {
        *pPC = (void *)((unsigned long)*pPC | 0x1);
    }

}
/* only use in usr-kernel panic type, usr context saved in ktask_stack in this type*/
void debug_usr_kernel_regs_show(int (*print_func)(const char *fmt, ...))
{
    uint32_t ktask_stask_addr = (uint32_t)(g_active_task[cpu_cur_get()]->task_stack_base) + g_active_task[cpu_cur_get()]->stack_size * sizeof(cpu_stack_t) - DEBUG_SYSCALL_REG_SIZE;

    debug_syscall_context_t *syscall_context = (debug_syscall_context_t *)ktask_stask_addr;

    print_func("========== Usr Regs info  ======\r\n");
    print_func(REGS_TYPE, syscall_context->PC, syscall_context->LR, syscall_context->SP, syscall_context->CPSR,
               syscall_context->R0, syscall_context->R1, syscall_context->R2, syscall_context->R3,
               syscall_context->R4, syscall_context->R5, syscall_context->R6, syscall_context->R7,
               syscall_context->R8, syscall_context->R9, syscall_context->R10, syscall_context->R11, syscall_context->R12);
}

void debug_uspace_panic_info_show(int (*print_func)(const char *fmt, ...))
{
    task_group_t *group = g_active_task[cpu_cur_get()]->task_group;

    uspace_panic_type panic_type = debug_uspace_panic_type_get();

    switch (panic_type) {
    case PANIC_IN_KERNEL:
        print_func("kernel space exception\r\n");
        break;
    case PANIC_IN_USER_KERNEL:
        print_func("uspace task result exception in kernel\r\n");
        break;
    case PANIC_IN_USER:
        print_func("uspace %s exception\r\n", group->tg_name);
        break;
    default:
        print_func("can not identify exception type\r\n");
        break;
    }

    if (panic_type == PANIC_IN_USER_KERNEL) {
        debug_usr_kernel_regs_show(print_func);
    }
}

#endif

#define get_dfsr_reg() \
    ({  \
        unsigned int __val; \
        asm("mrc    p15, 0, %0, c5, c0, 0" \
            : "=r" (__val)  \
            :   \
            : "cc");    \
            __val;  \
    })

#define REG_NAME_WIDTH 7
fault_context_t *g_fcontext[RHINO_CONFIG_CPU_NUM];

extern volatile uint32_t g_crash_steps;

extern void cpu_freeze_others();

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    if ( context != NULL ) {
        g_fcontext[cpu_cur_get()] = (fault_context_t *)context;
    }

    *pSP = (int *)g_fcontext[cpu_cur_get()]->SP;
    *pPC = (char *)g_fcontext[cpu_cur_get()]->cntx.PC;
    *pLR = (char *)g_fcontext[cpu_cur_get()]->cntx.LR;

    /* CPSR T, bit[5]  Thumb execution state bit. */
    if ( g_fcontext[cpu_cur_get()]->cntx.CPSR & 0x20 ) {
        *pPC = (void *)((unsigned long)*pPC | 0x1);
    }
}

#define DFSR_ALIGNMENT_FAULT  0x1
#define DFSR_WATCHPOINT_EVENT 0x2
#define DFSR_PERMISSION_SECTION_FAULT 0xD
#define DFSR_PERMISSION_PAGE_FAULT 0xF

void dfsr_parse(uint32_t dfsr, int (*print_func)(const char *fmt, ...))
{
    print_func("Exception Type: Data Abort. ");

    switch (dfsr & 0xf) {
    case DFSR_ALIGNMENT_FAULT:
        print_func("Alignment fault\r\n");
        break;
    case DFSR_WATCHPOINT_EVENT:
        print_func("Watchpoint debug event\r\n");
        break;
    case DFSR_PERMISSION_SECTION_FAULT:
    case DFSR_PERMISSION_PAGE_FAULT:
        print_func("Permission fault\r\n");
        break;
    default:
        print_func("\r\n");
    }
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    fault_context_t *flt_ctx = (fault_context_t *)context;

    unsigned int dfsr, dfar, ifsr;
    /* PANIC_CONTEXT */
    char s_fault_ctx[] = "DFSR   "
                         "DFAR   "
                         "IFSR   "
                         "IFAR   "
                         ;
    char s_fault_regs[REG_NAME_WIDTH + 14];
    char s_fault_coreid[] = "========== coreID:    ==========\r\n";

    if (context == NULL) {
        return;
    }

    s_fault_coreid[19] = (char)(cpu_cur_get() + '0');
    print_func(s_fault_coreid);

    switch (flt_ctx->exc_type) {
    case ARM_EXCEPT_UNDEF_INSTR :
        print_func("Exception Type: Undefined Instruction\r\n");
        break;

    case ARM_EXCEPT_PREFETCH_ABORT :
        print_func("Exception Type: Prefetch Abort\r\n");
        break;
    case ARM_EXCEPT_FIQ :
        print_func("Exception Type: fiq trigerred by other core\r\n");
        break;
    case ARM_EXCEPT_DATA_ABORT :
        dfsr_parse(get_dfsr_reg(), print_func);
        break;

    default:
        print_func("Exception Type: Unknown\r\n");
        break;
    }

#if (RHINO_CONFIG_USER_SPACE > 0)
    debug_uspace_panic_info_show(print_func);
#endif

    print_func("========== Regs info  ==========\r\n");
    print_func(REGS_TYPE, flt_ctx->cntx.PC, flt_ctx->cntx.LR, flt_ctx->cntx.SP, flt_ctx->cntx.CPSR,
               flt_ctx->cntx.R0, flt_ctx->cntx.R1, flt_ctx->cntx.R2, flt_ctx->cntx.R3,
               flt_ctx->cntx.R4, flt_ctx->cntx.R5, flt_ctx->cntx.R6, flt_ctx->cntx.R7,
               flt_ctx->cntx.R8, flt_ctx->cntx.R9, flt_ctx->cntx.R10, flt_ctx->cntx.R11, flt_ctx->cntx.R12);

    /*
     the DFSR holds information about a Data Abort exception
     the DFAR holds the faulting address for some synchronous Data Abort exceptions
     the IFSR holds information about a Prefetch Abort exception
     the IFAR holds the faulting address of a Prefetch Abort exception
    */
    print_func("========== Fault info ==========\r\n");
    dfsr = os_get_DFSR();
    memcpy(&s_fault_regs[0], &s_fault_ctx[0 * REG_NAME_WIDTH], REG_NAME_WIDTH);
    memcpy(&s_fault_regs[REG_NAME_WIDTH], " 0x", 3);
    k_int2str_hex(dfsr, &s_fault_regs[REG_NAME_WIDTH + 3]);
    s_fault_regs[REG_NAME_WIDTH + 11] = '\r';
    s_fault_regs[REG_NAME_WIDTH + 12] = '\n';
    s_fault_regs[REG_NAME_WIDTH + 13] = 0;
    print_func(s_fault_regs);

    dfar = os_get_DFAR();
    memcpy(&s_fault_regs[0], &s_fault_ctx[1 * REG_NAME_WIDTH], REG_NAME_WIDTH);
    memcpy(&s_fault_regs[REG_NAME_WIDTH], " 0x", 3);
    k_int2str_hex(dfar, &s_fault_regs[REG_NAME_WIDTH + 3]);
    s_fault_regs[REG_NAME_WIDTH + 11] = '\r';
    s_fault_regs[REG_NAME_WIDTH + 12] = '\n';
    s_fault_regs[REG_NAME_WIDTH + 13] = 0;
    print_func(s_fault_regs);

    ifsr = os_get_IFSR();
    memcpy(&s_fault_regs[0], &s_fault_ctx[2 * REG_NAME_WIDTH], REG_NAME_WIDTH);
    memcpy(&s_fault_regs[REG_NAME_WIDTH], " 0x", 3);
    k_int2str_hex(ifsr, &s_fault_regs[REG_NAME_WIDTH + 3]);
    s_fault_regs[REG_NAME_WIDTH + 11] = '\r';
    s_fault_regs[REG_NAME_WIDTH + 12] = '\n';
    s_fault_regs[REG_NAME_WIDTH + 13] = 0;
    print_func(s_fault_regs);

    /* ARMv5 do not support IFAR register */
    /*
    ifar = os_get_IFAR();
    memcpy(&s_fault_regs[0], &s_fault_ctx[3 * REG_NAME_WIDTH], REG_NAME_WIDTH);
    memcpy(&s_fault_regs[REG_NAME_WIDTH], " 0x", 3);
    k_int2str_hex(ifar, &s_fault_regs[REG_NAME_WIDTH + 3]);
    s_fault_regs[REG_NAME_WIDTH + 11] = '\r';
    s_fault_regs[REG_NAME_WIDTH + 12] = '\n';
    s_fault_regs[REG_NAME_WIDTH + 13] = 0;
    print_func(s_fault_regs);
    */
}

void exceptionHandler(void *context)
{
    int cpsr;

    g_sched_lock[cpu_cur_get()]++;

    cpsr = cpu_cpsr_get();
    printk("exception ++++++++++ cpsr 0x%x\r\n", cpsr);

    fault_context_t *flt_ctx = (fault_context_t *)context;
    /* fiq triggerd by exc of $ */
    if(flt_ctx->exc_type == ARM_EXCEPT_FIQ) {
#if (DEBUG_CONFIG_PANIC > 0)
        fiqafterpanicHandler(context);
#endif
        while(1);
    }

    g_crash_steps++;
    if (g_crash_steps > 1) {
        context = NULL;
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    cpu_freeze_others();
#endif

#if (DEBUG_CONFIG_PANIC > 0)
    panicHandler(context);
#else
    printk("exception occur!\n");
#endif

    while (1)
        ;
}

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

void panicNmiInputFilter_all(unsigned char* ch, unsigned char cnt)
{
    if (cnt == 1) {
        panicNmiInputFilter(ch[0]);
    } else if (cnt >= 4 && memcmp(ch, "$#@!", 4) == 0) {
        panicNmiFlagSet();
        __asm__ __volatile__("udf":::"memory");
    }
}

#else   /*#if (DEBUG_CONFIG_PANIC > 0)*/
void exceptionHandler(void *context)
{
    while(1);
}

void panicNmiInputFilter(uint8_t ch)
{

}
#endif

#define u32  int
#define ARM_BASE_BVR    64
#define ARM_BASE_BCR    80
#define ARM_BASE_WVR    96
#define ARM_BASE_WCR    112

#define ARM_OP2_BVR 4
#define ARM_OP2_BCR 5
#define ARM_OP2_WVR 6
#define ARM_OP2_WCR 7

#define ARM_DEBUG_ARCH_V6   1
#define ARM_DEBUG_ARCH_V6_1 2
#define ARM_DEBUG_ARCH_V7_ECP14 3
#define ARM_DEBUG_ARCH_V7_MM    4
#define ARM_DEBUG_ARCH_V7_1 5
#define ARM_DEBUG_ARCH_V8   6

#define ARM_DSCR_HDBGEN (1 << 14) /* DSCR halting bits. */
#define ARM_DSCR_MDBGEN (1 << 15) /* DSCR monitor bits. */

#define  smp_processor_id   cpu_cur_get

#define CPUID_ID    0

/* Accessor macros for the debug registers. */
#define ARM_DBG_READ(N, M, OP2, VAL) do {\
    asm volatile("mrc p14, 0, %0, " #N "," #M ", " #OP2 : "=r" (VAL));\
} while (0)

#define ARM_DBG_WRITE(N, M, OP2, VAL) do {\
    asm volatile("mcr p14, 0, %0, " #N "," #M ", " #OP2 : : "r" (VAL));\
} while (0)


#define isb() __asm__ __volatile__ ("" : : : "memory")

#define READ_WB_REG_CASE(OP2, M, VAL)   \
    case ((OP2 << 4) + M):  \
        ARM_DBG_READ(c0, c ## M, OP2, VAL); \
        break

#define WRITE_WB_REG_CASE(OP2, M, VAL)  \
    case ((OP2 << 4) + M):  \
        ARM_DBG_WRITE(c0, c ## M, OP2, VAL);    \
        break

#define GEN_READ_WB_REG_CASES(OP2, VAL) \
    READ_WB_REG_CASE(OP2, 0, VAL);  \
    READ_WB_REG_CASE(OP2, 1, VAL);  \
    READ_WB_REG_CASE(OP2, 2, VAL);  \
    READ_WB_REG_CASE(OP2, 3, VAL);  \
    READ_WB_REG_CASE(OP2, 4, VAL);  \
    READ_WB_REG_CASE(OP2, 5, VAL);  \
    READ_WB_REG_CASE(OP2, 6, VAL);  \
    READ_WB_REG_CASE(OP2, 7, VAL);  \
    READ_WB_REG_CASE(OP2, 8, VAL);  \
    READ_WB_REG_CASE(OP2, 9, VAL);  \
    READ_WB_REG_CASE(OP2, 10, VAL); \
    READ_WB_REG_CASE(OP2, 11, VAL); \
    READ_WB_REG_CASE(OP2, 12, VAL); \
    READ_WB_REG_CASE(OP2, 13, VAL); \
    READ_WB_REG_CASE(OP2, 14, VAL); \
    READ_WB_REG_CASE(OP2, 15, VAL)

#define GEN_WRITE_WB_REG_CASES(OP2, VAL)	\
    WRITE_WB_REG_CASE(OP2, 0, VAL); \
    WRITE_WB_REG_CASE(OP2, 1, VAL); \
    WRITE_WB_REG_CASE(OP2, 2, VAL); \
    WRITE_WB_REG_CASE(OP2, 3, VAL); \
    WRITE_WB_REG_CASE(OP2, 4, VAL); \
    WRITE_WB_REG_CASE(OP2, 5, VAL); \
    WRITE_WB_REG_CASE(OP2, 6, VAL); \
    WRITE_WB_REG_CASE(OP2, 7, VAL); \
    WRITE_WB_REG_CASE(OP2, 8, VAL); \
    WRITE_WB_REG_CASE(OP2, 9, VAL); \
    WRITE_WB_REG_CASE(OP2, 10, VAL);    \
    WRITE_WB_REG_CASE(OP2, 11, VAL);    \
    WRITE_WB_REG_CASE(OP2, 12, VAL);    \
    WRITE_WB_REG_CASE(OP2, 13, VAL);    \
    WRITE_WB_REG_CASE(OP2, 14, VAL);    \
    WRITE_WB_REG_CASE(OP2, 15, VAL)

static u32 read_wb_reg(int n)
{
    u32 val = 0;

    switch (n) {
        GEN_READ_WB_REG_CASES(ARM_OP2_BVR, val);
        GEN_READ_WB_REG_CASES(ARM_OP2_BCR, val);
        GEN_READ_WB_REG_CASES(ARM_OP2_WVR, val);
        GEN_READ_WB_REG_CASES(ARM_OP2_WCR, val);
    default:
        printf("attempt to read from unknown breakpoint register %d\n", n);
    }

    return val;
}

static void write_wb_reg(int n, u32 val)
{
    switch (n) {
        GEN_WRITE_WB_REG_CASES(ARM_OP2_BVR, val);
        GEN_WRITE_WB_REG_CASES(ARM_OP2_BCR, val);
        GEN_WRITE_WB_REG_CASES(ARM_OP2_WVR, val);
        GEN_WRITE_WB_REG_CASES(ARM_OP2_WCR, val);
    default:
        printf("attempt to write to unknown breakpoint register %d\n", n);
    }
    isb();
}


typedef struct {
    int magic;
    int set;  // 1:set   0:unset
    u32 addr;
} kwatchpoint_t;

#define ARM_V7A_WTATCHPOINT_NUM       4
#define ARM_V7A_WTATCHPOINT_MAGIC     0x11223344
kwatchpoint_t g_watchpoint_val[RHINO_CONFIG_CPU_NUM][ARM_V7A_WTATCHPOINT_NUM] = {0};

#if (RHINO_CONFIG_CPU_NUM > 1)

extern void cpu_watchpoint_others(void);
kspinlock_t g_watchpoint_lock = {KRHINO_SPINLOCK_FREE_VAL,0,0};
#if (RHINO_CONFIG_FIQ_AS_NMI > 0)
extern cpu_cpsr_t cpu_intrpt_all_save(void);
extern void cpu_intrpt_all_restore(cpu_cpsr_t cpsr);
#define NMI_ALLOC()    cpu_cpsr_t _cpsr_
#define NMI_DISABLE()  do{_cpsr_ = cpu_intrpt_all_save();}while(0)
#define NMI_ENABLE()   do{cpu_intrpt_all_restore(_cpsr_);}while(0)
#else
#define NMI_ALLOC()
#define NMI_DISABLE()
#define NMI_ENABLE()
#endif

#endif

//#define __stringify_1(x...) x
//#define __stringify(x...)   __stringify_1(x)

#define read_cpuid() \
    ({  \
        unsigned int __val; \
        asm("mrc    p15, 0, %0, c0, c0, 0" \
            : "=r" (__val)  \
            :   \
            : "cc");    \
            __val;  \
    })

static inline unsigned int read_cpuid_id(void)
{
    return read_cpuid();
}

static char get_debug_arch(void)
{
    u32 didr;

    /* Do we implement the extended CPUID interface? */
    if (((read_cpuid_id() >> 16) & 0xf) != 0xf) {
        printf("CPUID feature registers not supported. "
               "Assuming v6 debug is present.\n");
        return ARM_DEBUG_ARCH_V6;
    }

    ARM_DBG_READ(c0, c0, 0, didr);
    return (didr >> 16) & 0xf;
}
#define CORESIGHT_UNLOCK    0xc5acce55
/* OSLSR os lock model bits */
#define ARM_OSLSR_OSLM0 (1 << 0)

static int core_has_os_save_restore(void)
{
    u32 oslsr;

    switch (get_debug_arch()) {
    case ARM_DEBUG_ARCH_V7_1:
        return 1;
    case ARM_DEBUG_ARCH_V7_ECP14:
        ARM_DBG_READ(c1, c1, 4, oslsr);
        if (oslsr & ARM_OSLSR_OSLM0)
            return 1;
    /* Else, fall through */
    default:
        return 0;
    }
}

/* Determine number of BRP registers available. */
static int get_num_brp_resources(void)
{
    u32 didr;
    ARM_DBG_READ(c0, c0, 0, didr);
    return ((didr >> 24) & 0xf) + 1;
}

/* Determine number of WRP registers available. */
static int get_num_wrp_resources(void)
{
    u32 didr;
    ARM_DBG_READ(c0, c0, 0, didr);
    return ((didr >> 28) & 0xf) + 1;
}

static int get_num_wrps(void)
{
    /*
    * On debug architectures prior to 7.1, when a watchpoint fires, the
    * only way to work out which watchpoint it was is by disassembling
    * the faulting instruction and working out the address of the memory
    * access.
    *
    * Furthermore, we can only do this if the watchpoint was precise
    * since imprecise watchpoints prevent us from calculating register
    * based addresses.
    *
    * Providing we have more than 1 breakpoint register, we only report
    * a single watchpoint register for the time being. This way, we always
    * know which watchpoint fired. In the future we can either add a
    * disassembler and address generation emulator, or we can insert a
    * check to see if the DFAR is set on watchpoint exception entry
    * [the ARM ARM states that the DFAR is UNKNOWN, but experience shows
    * that it is set on some implementations].
    */
    if (get_debug_arch() < ARM_DEBUG_ARCH_V7_1)
        return 1;

    return get_num_wrp_resources();
}

static void reset_ctrl_regs(unsigned int cpu)
{
    int i, raw_num_brps, err = 0;
    u32 val;
    char debug_arch = get_debug_arch();
    int has_ossr;
    has_ossr = core_has_os_save_restore();
    int core_num_wrps = get_num_wrps();

    /*
    * v7 debug contains save and restore registers so that debug state
    * can be maintained across low-power modes without leaving the debug
    * logic powered up. It is IMPLEMENTATION DEFINED whether we can access
    * the debug registers out of reset, so we must unlock the OS Lock
    * Access Register to avoid taking undefined instruction exceptions
    * later on.
    */
    switch (debug_arch) {
    case ARM_DEBUG_ARCH_V6:
    case ARM_DEBUG_ARCH_V6_1:
        /* ARMv6 cores clear the registers out of reset. */
        goto out_mdbgen;
    case ARM_DEBUG_ARCH_V7_ECP14:
        /*
        * Ensure sticky power-down is clear (i.e. debug logic is
        * powered up).
        */
        ARM_DBG_READ(c1, c5, 4, val);
        if ((val & 0x1) == 0)
            err = -1;

        if (!has_ossr) {
            goto clear_vcr;
        }
        break;
    case ARM_DEBUG_ARCH_V7_1:
        /*
        * Ensure the OS double lock is clear.
        */
        ARM_DBG_READ(c1, c3, 4, val);
        if ((val & 0x1) == 1)
            err = -1;
        break;
    }

    if (err) {
        printf("CPU %d debug is powered down!\n", cpu);
        return;
    }

    /*
    * Unconditionally clear the OS lock by writing a value
    * other than CS_LAR_KEY to the access register.
    */
    ARM_DBG_WRITE(c1, c0, 4, ~CORESIGHT_UNLOCK);
    isb();

    /*
    * Clear any configured vector-catch events before
    * enabling monitor mode.
    */
clear_vcr:
    ARM_DBG_WRITE(c0, c7, 0, 0);
    isb();

    /*
    * The control/value register pairs are UNKNOWN out of reset so
    * clear them to avoid spurious debug events.
    */
    raw_num_brps = get_num_brp_resources();
    for (i = 0; i < raw_num_brps; ++i) {
        write_wb_reg(ARM_BASE_BCR + i, 0UL);
        write_wb_reg(ARM_BASE_BVR + i, 0UL);
    }

    for (i = 0; i < core_num_wrps; ++i) {
        write_wb_reg(ARM_BASE_WCR + i, 0UL);
        write_wb_reg(ARM_BASE_WVR + i, 0UL);
    }

    /*
    * Have a crack at enabling monitor mode. We don't actually need
    * it yet, but reporting an error early is useful if it fails.
    */
out_mdbgen:
    return;
}


int enable_monitor_mode(void)
{
    u32 dscr;

    reset_ctrl_regs(cpu_cur_get());

    ARM_DBG_READ(c0, c1, 0, dscr);

    /* If monitor mode is already enabled, just return. */
    if (dscr & ARM_DSCR_MDBGEN)
        goto out;

    //printf("monitor_mode:0x%x,arch:%d\r\n",dscr,get_debug_arch());

    /* Write to the corresponding DSCR. */
    switch (get_debug_arch()) {
    case ARM_DEBUG_ARCH_V6:
    case ARM_DEBUG_ARCH_V6_1:
        ARM_DBG_WRITE(c0, c1, 0, (dscr | ARM_DSCR_MDBGEN));
        break;
    case ARM_DEBUG_ARCH_V7_ECP14:
    case ARM_DEBUG_ARCH_V7_1:
    case ARM_DEBUG_ARCH_V8:
        ARM_DBG_WRITE(c0, c2, 2, (dscr | ARM_DSCR_MDBGEN));
        isb();
        break;
    default:
        return -1;
    }

    /* Check that the write made it through. */
    ARM_DBG_READ(c0, c1, 0, dscr);
    if (!(dscr & ARM_DSCR_MDBGEN)) {
        printf("Failed to enable monitor mode on CPU %d.\n",
               smp_processor_id());
        return -1;
    }

out:
    return 0;
}


/*
 * i: watchpoint reg idx, 0~3
 * addr: virtual address, aligned by 4
 * we must be running in debug monitor mode.
 */

int arm_install_hw_watchpoint(int i, u32 addr)
{
    u32 ctrl = 0x1F7;//0x1F7;
    u32 check_value;

    addr &= ~(0x4UL - 1);

    if(i >= ARM_V7A_WTATCHPOINT_NUM) {
        return -1;
    }

    /* Setup the address register. */
    write_wb_reg(ARM_BASE_WVR + i, addr);
    check_value = read_wb_reg(ARM_BASE_WVR + i);
    if (check_value != addr) {
        printf("fail to set WVR[%d] addr:0x%x check_value:0x%x\n", i, addr, check_value);
        return -1;
    }

    /* Setup the control register. */
    write_wb_reg(ARM_BASE_WCR + i, ctrl);
    check_value = read_wb_reg(ARM_BASE_WCR + i);
    if (check_value != ctrl) {
        printf("fail to set WCR[%d] ctrl:0x%x check_value:0x%x\n", i, ctrl, check_value);
        return -1;
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    NMI_ALLOC();
    NMI_DISABLE();
    krhino_spin_lock(&g_watchpoint_lock);
    for(int cpu = 0; cpu < RHINO_CONFIG_CPU_NUM; cpu++) {
        if(cpu == cpu_cur_get()) {
            continue;
        }
        g_watchpoint_val[cpu][i].magic = ARM_V7A_WTATCHPOINT_MAGIC;
        g_watchpoint_val[cpu][i].set = 1;
        g_watchpoint_val[cpu][i].addr = addr;
    }
    krhino_spin_unlock(&g_watchpoint_lock);
    NMI_ENABLE();

    cpu_watchpoint_others();
#endif
    return 0;
}

void arm_clear_hw_watchpoint(int i)
{
    if(i >= ARM_V7A_WTATCHPOINT_NUM) {
        return;
    }

    write_wb_reg(ARM_BASE_WCR + i, 0UL);
    write_wb_reg(ARM_BASE_WVR + i, 0UL);

#if (RHINO_CONFIG_CPU_NUM > 1)
    NMI_ALLOC();
    NMI_DISABLE();
    krhino_spin_lock(&g_watchpoint_lock);
    for(int cpu = 0; cpu < RHINO_CONFIG_CPU_NUM; cpu++) {
        if(cpu == cpu_cur_get()) {
            continue;
        }
        g_watchpoint_val[cpu][i].magic = ARM_V7A_WTATCHPOINT_MAGIC;
        g_watchpoint_val[cpu][i].set = 0;
        //g_watchpoint_val[i].addr = addr;
    }
    krhino_spin_unlock(&g_watchpoint_lock);
    NMI_ENABLE();

    cpu_watchpoint_others();
#endif
}

void arm_sync_hw_watchpoint()
{
#if (RHINO_CONFIG_CPU_NUM > 1)
    u32 ctrl = 0x1F7;//0x1F7;
    uint8_t cpu = cpu_cur_get();

    NMI_ALLOC();
    NMI_DISABLE();

    krhino_spin_lock(&g_watchpoint_lock);
    for(i = 0; i < ARM_V7A_WTATCHPOINT_NUM; i++) {
        if(g_watchpoint_val[cpu][i].magic == ARM_V7A_WTATCHPOINT_MAGIC) {
            addr = g_watchpoint_val[cpu][i].addr;
            if(g_watchpoint_val[cpu][i].set) {
                write_wb_reg(ARM_BASE_WVR + i, addr);
                write_wb_reg(ARM_BASE_WCR + i, ctrl);
            } else {
                write_wb_reg(ARM_BASE_WCR + i, 0UL);
                write_wb_reg(ARM_BASE_WVR + i, 0UL);
            }

            g_watchpoint_val[cpu][i].magic = 0;
        }
    }

    krhino_spin_unlock(&g_watchpoint_lock);
    NMI_ENABLE();
#endif
}

/* show dwt regs of ARCH */
void dwt_regs_show(void)
{
    int debug_arch    = get_debug_arch();
    int core_num_wrps = get_num_wrps();

    u32 DBGDIDR, DBGDSCRint, DBGDSCRext, DBGDCCINT, DBGWFAR, DBGOSECCR, DBGWVR0, DBGWVR1, DBGWVR2, DBGWVR3;
    u32 DBGWCR0, DBGWCR1, DBGWCR2, DBGWCR3, DBGOSLSR, DBGOSDLR, DBGPRCR, DBGDEVID1;
    u32 DBGCLAIMSET, DBGCLAIMCLR, DBGAUTHSTATUS;

    ARM_DBG_READ(c0, c0, 0, DBGDIDR);
    ARM_DBG_READ(c0, c1, 0, DBGDSCRint);
    ARM_DBG_READ(c0, c2, 2, DBGDSCRext);
    ARM_DBG_READ(c0, c2, 0, DBGDCCINT);
    ARM_DBG_READ(c0, c6, 0, DBGWFAR);
    ARM_DBG_READ(c0, c6, 2, DBGOSECCR);
    ARM_DBG_READ(c0, c0, 6, DBGWVR0);
    ARM_DBG_READ(c0, c1, 6, DBGWVR1);
    ARM_DBG_READ(c0, c2, 6, DBGWVR2);
    ARM_DBG_READ(c0, c3, 6, DBGWVR3);
    ARM_DBG_READ(c0, c0, 7, DBGWCR0);
    ARM_DBG_READ(c0, c1, 7, DBGWCR1);
    ARM_DBG_READ(c0, c2, 7, DBGWCR2);
    ARM_DBG_READ(c0, c3, 7, DBGWCR3);
    ARM_DBG_READ(c1, c1, 4, DBGOSLSR);
    ARM_DBG_READ(c1, c3, 4, DBGOSDLR);
    ARM_DBG_READ(c1, c4, 4, DBGPRCR);
    ARM_DBG_READ(c7, c1, 7, DBGDEVID1);
    ARM_DBG_READ(c7, c8, 6, DBGCLAIMSET);
    ARM_DBG_READ(c7, c9, 6, DBGCLAIMCLR);
    ARM_DBG_READ(c7, c14, 6, DBGAUTHSTATUS);

    printf("====== Data Watchpoint Regs Show ======= \r\n");
    printf("arch : %d  watchpoint_num : %d\r\n", debug_arch, core_num_wrps);
    printf(" DBGDIDR 0x%08X\r\n DBGDSCRint 0x%08X\r\n DBGDSCRext 0x%08X\r\n DBGDCCINT 0x%08X\r\n DBGWFAR 0x%08X\r\n DBGOSECCR 0x%08X\r\n DBGWVR0 0x%08X\r\n DBGWVR1 0x%08X\r\n DBGWVR2 0x%08X\r\n DBGWVR3 0x%08X\r\n",
           DBGDIDR, DBGDSCRint, DBGDSCRext, DBGDCCINT, DBGWFAR, DBGOSECCR, DBGWVR0, DBGWVR1, DBGWVR2, DBGWVR3);
    printf(" DBGWCR0 0x%08X\r\n DBGWCR1 0x%08X\r\n DBGWCR2 0x%08X\r\n DBGWCR3 0x%08X\r\n DBGOSLSR 0x%08X\r\n DBGOSDLR 0x%08X\r\n DBGPRCR 0x%08X\r\n DBGDEVID1 0x%08X\r\n",
           DBGWCR0, DBGWCR1, DBGWCR2, DBGWCR3, DBGOSLSR, DBGOSDLR, DBGPRCR, DBGDEVID1);
    printf(" DBGCLAIMSET 0x%08X\r\n DBGCLAIMCLR 0x%08X\r\n DBGAUTHSTATUS 0x%08X\r\n",
           DBGCLAIMSET, DBGCLAIMCLR, DBGAUTHSTATUS);
}

void debug_err_go_to_cli(void)
{
    extern void debug_cpu_goto_cli(void);
    debug_cpu_goto_cli();
    __asm__ __volatile__("udf":::"memory");
}

void debug_force_cur_free_lock(void)
{
    extern kspinlock_t g_sys_lock;
    extern kspinlock_t g_panic_print_lock;
    extern void k_cpu_spin_unlock_force(kspinlock_t *lock);

    k_cpu_spin_unlock_force(&g_sys_lock);
    k_cpu_spin_unlock_force(&g_panic_print_lock);
}

