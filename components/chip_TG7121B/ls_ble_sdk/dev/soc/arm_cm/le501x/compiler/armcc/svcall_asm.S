    PRESERVE8
    THUMB
    AREA    |.text|, CODE, READONLY
    FRAME UNWIND ON
SVC_Handler PROC
    EXPORT SVC_Handler
    IMPORT SVC_Handler_C
    MOVS R0,#4
    MOV R1,LR
    TST R0,R1
    BEQ stacking_used_MSP
    MRS R0,PSP
    B SVC_C_Routine
stacking_used_MSP
    MRS R0,MSP
SVC_C_Routine
    LDR R1,=SVC_Handler_C
    BX R1
    ALIGN
    ENDP
    
    END