#include "debug_api.h"
#include "syscall_no.h"
#include "svc_debug.h"
#include "utask.h"

extern const void *const syscall_tbl[];

typedef int (*syscall_func_t)(void *arg);

unsigned long do_syscall(unsigned long syscall_no, void *arg)
{
    unsigned long ret = 0;

    syscall_func_t pfunc;

    if (syscall_no > MAX_SYSCALL_NO) {
        printk("invalid syscall no %d, MAX_SYSCALL_NO %d\r\n",
               syscall_no, MAX_SYSCALL_NO);
        k_err_proc(RHINO_SYS_FATAL_ERR);
        return ret;
    }

#if (DEBUG_CONFIG_AOS_TRACE > 0)
    debug_trace_syscall_hook(syscall_no);
#endif

    pfunc = syscall_tbl[syscall_no];
    if (pfunc) {
        ret = pfunc(arg);
    } else {
        printk("NULL syscall func, syscall num %d\r\n", syscall_no);
        k_err_proc(RHINO_SYS_FATAL_ERR);
    }

    krhino_utask_cancel_check();

    return ret;
}

void bad_svc_no_exception(void *context, int svc_no)
{
    context_t *ct = context;

    ct = ct;

    printk("SVC exception: bad svc nubmer 0x%x\r\n", svc_no);

    printk("R0 %08x R1 %08x R2 %08x R3 %08x\r\n", ct->R0, ct->R1, ct->R2, ct->R3);
    printk("R4 %08x R5 %08x R6 %08x R7 %08x\r\n", ct->R4, ct->R5, ct->R6, ct->R7);
    printk("R8 %08x R9 %08x R10 %08x R11 %08x\r\n", ct->R8, ct->R9, ct->R10, ct->R11);
    printk("R12 %08x SP %08x LR %08x PC %08x\r\n", ct->R12, ct->SP, ct->LR, ct->PC);
    printk("SPSR %08x\r\n", ct->CPSR);

    long *pc = (long *)ct->PC;
    printk("read svc instruction from text\r\n");
    printk("addr %08x, val %08x\r\n", pc, *pc);
    pc--;
    printk("addr %08x, val %08x\r\n", pc, *pc);

    k_err_proc(RHINO_SYS_FATAL_ERR);
}

