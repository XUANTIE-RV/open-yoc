#ifndef SWINT_CALL_ASM_H_
#define SWINT_CALL_ASM_H_

#define SWINT_FUNC_CALL_INLINE_ASM(func,swint_set)\
        __asm ("la a4,"#func"\n"\
               "j "#swint_set"\n"\
               "ret" : : : )

#define SWINT_SET_INLINE_ASM(irq_num) \
        uint32_t mask = 1;\
        uint32_t pending_addr = 0xe0801000 + (irq_num)*4;\
        __asm ("sb %0,0(%1)": : \
            "r"(mask),"r"(pending_addr) \
            : "a0", "a1", "a2", "a3","a4")

#endif
