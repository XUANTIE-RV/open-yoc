# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  bt

    # la      sp, g_top_trapstack
    # addi    sp, sp, -(140+140)
    # sd      x1, ( 0 + 0 )(sp)
    # sd      x3, ( 8 + 8 )(sp)
    # sd      x4, ( 12+ 12)(sp)
    # sd      x5, ( 16+ 16)(sp)
    # sd      x6, ( 20+ 20)(sp)
    # sd      x7, ( 24+ 24)(sp)
    # sd      x8, ( 28+ 28)(sp)
    # sd      x9, ( 32+ 32)(sp)
    # sd      x10,( 36+ 36)(sp)
    # sd      x11,( 40+ 40)(sp)
    # sd      x12,( 44+ 44)(sp)
    # sd      x13,( 48+ 48)(sp)
    # sd      x14,( 52+ 52)(sp)
    # sd      x15,( 56+ 56)(sp)
    # sd      x16,( 60+ 60)(sp)
    # sd      x17,( 64+ 64)(sp)
    # sd      x18,( 68+ 68)(sp)
    # sd      x19,( 72+ 72)(sp)
    # sd      x20,( 76+ 76)(sp)
    # sd      x21,( 80+ 80)(sp)
    # sd      x22,( 84+ 84)(sp)
    # sd      x23,( 88+ 88)(sp)
    # sd      x24,( 92+ 92)(sp)
    # sd      x25,( 96+ 96)(sp)
    # sd      x26,(100+100)(sp)
    # sd      x27,(104+104)(sp)
    # sd      x28,(108+108)(sp)
    # sd      x29,(112+112)(sp)
    # sd      x30,(116+116)(sp)
    # sd      x31,(120+120)(sp)
    # csrr    a0, mepc
    # sd      a0, (124+124)(sp)
    # csrr    a0, mstatus
    # sd      a0, (128+128)(sp)
    # csrr    a0, mcause
    # sd      a0, (132+132)(sp)
    # csrr    a0, mtval
    # sd      a0, (136+136)(sp)
    # csrr    a0, MODE_PREFIX(scratch)
    # sd      a0, ( 4 + 4 )(sp)

  set $x1  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 280)
  set $x2  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 272)
  set $x3  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 266)
  set $x4  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 260)
  set $x5  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 252)
  set $x6  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 246)
  set $x7  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 240)
  set $x8  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 208)
  set $x9  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 200)
  set $x10 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 192)
  set $x11 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 184)
  set $x12 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 176)
  set $x13 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 168)
  set $x14 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 160)
  set $x15 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 152)
  set $x16 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 144)
  set $x17 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 136)
  set $x18 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 128)
  set $x19 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 120)
  set $x20 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 112)
  set $x21 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 104)
  set $x22 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 96)
  set $x23 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 88)
  set $x24 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 80)
  set $x25 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 72)
  set $x26 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 64)
  set $x27 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 56)
  set $x28 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 48)
  set $x29 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 40)
  set $x30 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 32)
  set $x31 = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 24)
  set $pc  = *(cpu_stack_t *)((cpu_stack_t)&g_top_trapstack - 16)

  bt
end

paddr
