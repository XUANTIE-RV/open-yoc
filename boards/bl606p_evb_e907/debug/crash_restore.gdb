# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  bt

    #la      t0, g_trap_sp
    #addi    t0, t0, -132
    #sw      x1, 0(t0)
    #sw      x2, 4(t0)
    #sw      x3, 8(t0)
    #sw      x4, 12(t0)
    #sw      x6, 20(t0)
    #sw      x7, 24(t0)
    #sw      x8, 28(t0)
    #sw      x9, 32(t0)
    #sw      x10, 36(t0)
    #sw      x11, 40(t0)
    #sw      x12, 44(t0)
    #sw      x13, 48(t0)
    #sw      x14, 52(t0)
    #sw      x15, 56(t0)
    #sw      x16, 60(t0)
    #sw      x17, 64(t0)
    #sw      x18, 68(t0)
    #sw      x19, 72(t0)
    #sw      x20, 76(t0)
    #sw      x21, 80(t0)
    #sw      x22, 84(t0)
    #sw      x23, 88(t0)
    #sw      x24, 92(t0)
    #sw      x25, 96(t0)
    #sw      x26, 100(t0)
    #sw      x27, 104(t0)
    #sw      x28, 108(t0)
    #sw      x29, 112(t0)
    #sw      x30, 116(t0)
    #sw      x31, 120(t0)
    #csrr    a0, mepc
    #sw      a0, 124(t0)
    #csrr    a0, mstatus
    #sw      a0, 128(t0)

  set $x1  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 132)
  set $x2  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 128)
  set $x3  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 124)
  set $x4  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 120)
  set $x5  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 116)
  set $x6  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 112)
  set $x7  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 108)
  set $x8  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 104)
  set $x9  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 100)
  set $x10 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 96)
  set $x11 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 92)
  set $x12 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 88)
  set $x13 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 84)
  set $x14 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 80)
  set $x15 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 76)
  set $x16 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 72)
  set $x17 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 68)
  set $x18 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 64)
  set $x19 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 60)
  set $x20 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 56)
  set $x21 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 52)
  set $x22 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 48)
  set $x23 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 44)
  set $x24 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 40)
  set $x25 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 36)
  set $x26 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 32)
  set $x27 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 28)
  set $x28 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 24)
  set $x29 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 20)
  set $x30 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 16)
  set $x31 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 12)
  set $pc  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 8)

  bt
end

paddr
