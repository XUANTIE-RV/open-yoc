/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_arch.h>

#define portSP_ELx  ((cpu_stack_t)0x01)
#define portSP_EL0  ((cpu_stack_t)0x00)

#if defined( GUEST )
    #define portEL1            ((cpu_stack_t)0x04)
    #define portINITIAL_PSTATE (portEL1 | portSP_EL0)
#else
    #define portEL3            ((cpu_stack_t)0x0c)
    /* At the time of writing, the BSP only supports EL3. */
    #define portINITIAL_PSTATE (portEL3 | portSP_EL0)
#endif

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;

    /* stack aligned by 8 byte */
    stk = (cpu_stack_t *)((uintptr_t)(stack_base + stack_size) & 0xfffffff0);

    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X1         */
    *(--stk)  = (cpu_stack_t)arg;                       /* X0         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X3         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X2         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X5         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X4         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X7         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X6         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X9         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X8         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X11          */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X10          */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X13         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X12         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X15         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X14         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X17         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X16         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X19         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X18         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X21         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X20         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X23         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X22         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X25         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X24         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X27         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X26         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X29         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* X28         */
    *(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* XZR         */
    *(--stk)  = (cpu_stack_t)krhino_task_deathbed;      /* X30         */

    *(--stk)  = (cpu_stack_t)portINITIAL_PSTATE;
    *(--stk)  = (cpu_stack_t)entry;                     /* Entry Point */
    *(--stk)  = (cpu_stack_t)0x3131313131313131L;       /* Q1         */
    *(--stk)  = (cpu_stack_t)0x3131313131313131L;       /* Q1         */
    *(--stk)  = (cpu_stack_t)0x3030303030303030L;       /* Q0         */
    *(--stk)  = (cpu_stack_t)0x3030303030303030L;       /* Q0         */
    *(--stk)  = (cpu_stack_t)0x2929292929292929L;       /* Q3         */
    *(--stk)  = (cpu_stack_t)0x2929292929292929L;       /* Q3         */
    *(--stk)  = (cpu_stack_t)0x2828282828282828L;       /* Q2         */
    *(--stk)  = (cpu_stack_t)0x2828282828282828L;       /* Q2         */
    *(--stk)  = (cpu_stack_t)0x2727272727272727L;       /* Q5         */
    *(--stk)  = (cpu_stack_t)0x2727272727272727L;       /* Q5         */
    *(--stk)  = (cpu_stack_t)0x2626262626262626L;       /* Q4         */
    *(--stk)  = (cpu_stack_t)0x2626262626262626L;       /* Q4         */
    *(--stk)  = (cpu_stack_t)0x2525252525252525L;       /* Q7         */
    *(--stk)  = (cpu_stack_t)0x2525252525252525L;       /* Q7         */
    *(--stk)  = (cpu_stack_t)0x2424242424242424L;       /* Q6         */
    *(--stk)  = (cpu_stack_t)0x2424242424242424L;       /* Q6         */
    *(--stk)  = (cpu_stack_t)0x2323232323232323L;       /* Q9         */
    *(--stk)  = (cpu_stack_t)0x2323232323232323L;       /* Q9         */
    *(--stk)  = (cpu_stack_t)0x2222222222222222L;       /* Q8         */
    *(--stk)  = (cpu_stack_t)0x2222222222222222L;       /* Q8         */
    *(--stk)  = (cpu_stack_t)0x2121212121212121L;       /* Q11         */
    *(--stk)  = (cpu_stack_t)0x2121212121212121L;       /* Q11         */
    *(--stk)  = (cpu_stack_t)0x2020202020202020L;       /* Q10         */
    *(--stk)  = (cpu_stack_t)0x2020202020202020L;       /* Q10         */
    *(--stk)  = (cpu_stack_t)0x1919191919191919L;       /* Q13         */
    *(--stk)  = (cpu_stack_t)0x1919191919191919L;       /* Q13         */
    *(--stk)  = (cpu_stack_t)0x1818181818181818L;       /* Q12         */
    *(--stk)  = (cpu_stack_t)0x1818181818181818L;       /* Q12         */
    *(--stk)  = (cpu_stack_t)0x1717171717171717L;       /* Q15         */
    *(--stk)  = (cpu_stack_t)0x1717171717171717L;       /* Q15         */
    *(--stk)  = (cpu_stack_t)0x1616161616161616L;       /* Q14         */
    *(--stk)  = (cpu_stack_t)0x1616161616161616L;       /* Q14         */
    *(--stk)  = (cpu_stack_t)0x1515151515151515L;       /* Q17         */
    *(--stk)  = (cpu_stack_t)0x1515151515151515L;       /* Q17         */
    *(--stk)  = (cpu_stack_t)0x1414141414141414L;       /* Q16         */
    *(--stk)  = (cpu_stack_t)0x1414141414141414L;       /* Q16         */
    *(--stk)  = (cpu_stack_t)0x1313131313131313L;       /* Q19         */
    *(--stk)  = (cpu_stack_t)0x1313131313131313L;       /* Q19         */
    *(--stk)  = (cpu_stack_t)0x1212121212121212L;       /* Q18         */
    *(--stk)  = (cpu_stack_t)0x1212121212121212L;       /* Q18         */
    *(--stk)  = (cpu_stack_t)0x1111111111111111L;       /* Q21         */
    *(--stk)  = (cpu_stack_t)0x1111111111111111L;       /* Q21         */
    *(--stk)  = (cpu_stack_t)0x1010101010101010L;       /* Q20         */
    *(--stk)  = (cpu_stack_t)0x1010101010101010L;       /* Q20         */
    *(--stk)  = (cpu_stack_t)0x0909090909090909L;       /* Q23          */
    *(--stk)  = (cpu_stack_t)0x0909090909090909L;       /* Q23          */
    *(--stk)  = (cpu_stack_t)0x0808080808080808L;       /* Q22          */
    *(--stk)  = (cpu_stack_t)0x0808080808080808L;       /* Q22          */
    *(--stk)  = (cpu_stack_t)0x0707070707070707L;       /* Q25          */
    *(--stk)  = (cpu_stack_t)0x0707070707070707L;       /* Q25          */
    *(--stk)  = (cpu_stack_t)0x0606060606060606L;       /* Q24          */
    *(--stk)  = (cpu_stack_t)0x0606060606060606L;       /* Q24          */
    *(--stk)  = (cpu_stack_t)0x0505050505050505L;       /* Q27          */
    *(--stk)  = (cpu_stack_t)0x0505050505050505L;       /* Q27          */
    *(--stk)  = (cpu_stack_t)0x0404040404040404L;       /* Q26          */
    *(--stk)  = (cpu_stack_t)0x0404040404040404L;       /* Q26          */
    *(--stk)  = (cpu_stack_t)0x0303030303030303L;       /* Q29          */
    *(--stk)  = (cpu_stack_t)0x0303030303030303L;       /* Q29          */
    *(--stk)  = (cpu_stack_t)0x0202020202020202L;       /* Q28          */
    *(--stk)  = (cpu_stack_t)0x0202020202020202L;       /* Q28          */
    *(--stk)  = (cpu_stack_t)0x0101010101010101L;       /* Q31          */
    *(--stk)  = (cpu_stack_t)0x0101010101010101L;       /* Q31          */
    *(--stk)  = (cpu_stack_t)0x0000000000000000L;       /* Q30          */
    *(--stk)  = (cpu_stack_t)0x0000000000000000L;       /* Q30          */
    return (void *)stk;
}

