# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  bt

    #la      t0, g_trap_sp
    #addi    t0, t0, -(132+132)
    #sd      x1, ( 0 + 0 )(t0)
    #sd      x2, ( 4 + 4 )(t0)
    #sd      x3, ( 8 + 8 )(t0)
    #sd      x4, ( 12+ 12)(t0)
    #sd      x5, ( 16+ 16)(t0)
    #sd      x6, ( 20+ 20)(t0)
    #sd      x7, ( 24+ 24)(t0)
    #sd      x8, ( 28+ 28)(t0)
    #sd      x9, ( 32+ 32)(t0)
    #sd      x10,( 36+ 36)(t0)
    #sd      x11,( 40+ 40)(t0)
    #sd      x12,( 44+ 44)(t0)
    #sd      x13,( 48+ 48)(t0)
    #sd      x14,( 52+ 52)(t0)
    #sd      x15,( 56+ 56)(t0)
    #sd      x16,( 60+ 60)(t0)
    #sd      x17,( 64+ 64)(t0)
    #sd      x18,( 68+ 68)(t0)
    #sd      x19,( 72+ 72)(t0)
    #sd      x20,( 76+ 76)(t0)
    #sd      x21,( 80+ 80)(t0)
    #sd      x22,( 84+ 84)(t0)
    #sd      x23,( 88+ 88)(t0)
    #sd      x24,( 92+ 92)(t0)
    #sd      x25,( 96+ 96)(t0)
    #sd      x26,(100+100)(t0)
    #sd      x27,(104+104)(t0)
    #sd      x28,(108+108)(t0)
    #sd      x29,(112+112)(t0)
    #sd      x30,(116+116)(t0)
    #sd      x31,(120+120)(t0)
    #csrr    a0, mepc
    #sd      a0, (124+124)(t0)
    #csrr    a0, mstatus
    #sd      a0, (128+128)(t0)

  set $x1  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 264)
  set $x2  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 256)
  set $x3  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 248)
  set $x4  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 240)
  set $x5  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 232)
  set $x6  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 224)
  set $x7  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 216)
  set $x8  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 208)
  set $x9  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 200)
  set $x10 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 192)
  set $x11 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 184)
  set $x12 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 176)
  set $x13 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 168)
  set $x14 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 160)
  set $x15 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 152)
  set $x16 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 144)
  set $x17 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 136)
  set $x18 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 128)
  set $x19 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 120)
  set $x20 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 112)
  set $x21 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 104)
  set $x22 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 96)
  set $x23 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 88)
  set $x24 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 80)
  set $x25 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 72)
  set $x26 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 64)
  set $x27 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 56)
  set $x28 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 48)
  set $x29 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 40)
  set $x30 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 32)
  set $x31 = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 24)
  set $pc  = *(cpu_stack_t *)((cpu_stack_t)&g_trap_sp - 16)

  bt
end

paddr
