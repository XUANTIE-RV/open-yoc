#ifndef SWINT_CALL_ASM_H_
#define SWINT_CALL_ASM_H_

#define SWINT_FUNC_CALL_INLINE_ASM(func,swint_set)\
        __asm ("push {lr}\n"\
                "ldr r3,="#func"\n"\
               "bl "#swint_set"\n"\
               "pop {pc}" : : : )

#define SWINT_SET_INLINE_ASM(irq_num) \
        uint32_t mask = 1<<((irq_num)&0x1f);\
        uint32_t pending_addr = 0xe000e200 + ((irq_num)>>5)*4;\
        __asm ("str %0,[%1,#0]": : \
            "r"(mask),"r"(pending_addr) \
            : "r0", "r1", "r2", "r3")

#endif
