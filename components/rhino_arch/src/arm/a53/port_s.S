#include <k_config.h>
#include <k_arch.h>

/******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************/
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.extern task_restore
.extern _os_vector_table
/******************************************************************************
@                        CODE GENERATION DIRECTIVES
@*******************************************************************************/
.text
.align 3

/******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************/
cpu_intrpt_save:
    mrs x0,DAIF
    MSR DAIFSET, #2
    DSB SY
    ISB SY
    RET

cpu_intrpt_restore:
    msr DAIF,x0
    DSB SY
    ISB SY
    RET

/******************************************************************************
@ Functions:
@     void cpu_intrpt_switch(void);
@     void cpu_task_switch(void);
@******************************************************************************/
cpu_task_switch:
    SVC 0
    RET

cpu_intrpt_switch:
    RET

/******************************************************************************
@ Functions:
@     void cpu_first_task_start(void);
@******************************************************************************/
.align 8
.func
cpu_first_task_start:
.set aos_vector_base, _os_vector_table
    LDR X1, =aos_vector_base
    MSR VBAR_EL1, X1
    DSB SY
    ISB SY
    bl   task_restore
.endfunc

