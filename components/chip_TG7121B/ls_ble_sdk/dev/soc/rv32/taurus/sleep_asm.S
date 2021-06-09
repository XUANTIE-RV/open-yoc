
    .option rvc
    .section .xip_banned,"x"
    .global cpu_sleep_asm
    .type   cpu_sleep_asm, %function
cpu_sleep_asm:
    jal t0, __riscv_save_0
    jal store_sp
    jal before_wfi
    wfi
    .size	cpu_sleep_asm, . - cpu_sleep_asm

    .section .xip_banned,"x"
    .global cpu_recover_asm
    .type   cpu_recover_asm, %function
cpu_recover_asm:
    jal restore_sp
    jal after_wfi
    j __riscv_restore_0
    .size	cpu_recover_asm, . - cpu_recover_asm
