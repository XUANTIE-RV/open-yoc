    PRESERVE8
    THUMB
    AREA    |.xip_banned|, CODE, READONLY
    FRAME UNWIND ON
arm_cm_delay_asm PROC
    EXPORT arm_cm_delay_asm
    SUBS r0,r0,#1
    CMP r0,#0
    BNE arm_cm_delay_asm
    BX LR
    ENDP
    
    END