
    .option rvc
    .section .xip_banned,"x"
    .align 6
    .global exception_entry
    .type   exception_entry, %function
exception_entry:
    addi    sp, sp, -48
    sw      a0, 36(sp)
    csrr    a0, mcause
    sw      a0, 44(sp)
    sw      a1, 32(sp)
    csrr    a1, mepc
    sw      a1, 40(sp)
    sw      a2, 28(sp)
    sw      a3, 24(sp)
    sw      a4, 20(sp)
    sw      a5, 16(sp)
    sw      t0, 12(sp)
    sw      t1, 8(sp)
    sw      t2, 4(sp)
    sw      ra, 0(sp)
    bgtz    a0, trap_handler
int_call:
    csrrwi  t0, mnxti, 8
    beqz    t0, exception_exit
    lw      t0, 0(t0)
    jalr    t0   
    j       int_call
exception_exit:
    lw      ra, 0(sp)
    lw      t2, 4(sp)
    lw      t1, 8(sp)
    lw      t0, 12(sp)
    lw      a5, 16(sp)
    lw      a4, 20(sp)
    lw      a3, 24(sp)
    lw      a2, 28(sp)
    lw      a1, 32(sp)
    csrc    mstatus, 8
    lw      a0, 40(sp)
    csrw    mepc, a0
    lw      a0, 44(sp)
    csrw    mcause, a0
    lw      a0, 36(sp)
    addi    sp, sp, 48
    mret
    .size	exception_entry, . - exception_entry

    .global trap_handler
    .type   trap_handler, %function
trap_handler:
    j trap_handler
    