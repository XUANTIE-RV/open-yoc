# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  p *g_active_task[0]
  bt

  # store the register
  set $gdb_x0  = $x0
  set $gdb_x1  = $x1
  set $gdb_x2  = $x2
  set $gdb_x3  = $x3
  set $gdb_x4  = $x4
  set $gdb_x5  = $x5
  set $gdb_x6  = $x6
  set $gdb_x7  = $x7
  set $gdb_x8  = $x8
  set $gdb_x9  = $x9
  set $gdb_x10 = $x10
  set $gdb_x11 = $x11
  set $gdb_x12 = $x12
  set $gdb_x13 = $x13
  set $gdb_x14 = $x14
  set $gdb_x15 = $x15
  set $gdb_x16 = $x16
  set $gdb_x17 = $x17
  set $gdb_x18 = $x18
  set $gdb_x19 = $x19
  set $gdb_x20 = $x20
  set $gdb_x21 = $x21
  set $gdb_x22 = $x22
  set $gdb_x23 = $x23
  set $gdb_x24 = $x24
  set $gdb_x25 = $x25
  set $gdb_x26 = $x26
  set $gdb_x27 = $x27
  set $gdb_x28 = $x28
  set $gdb_x29 = $x29
  set $gdb_x30 = $x30
  set $gdb_x31 = $x31
  set $gdb_mcause = $mcause
  set $gdb_scause = $scause
  set $gdb_pc  = $pc

  # get list offset
  set $gdb_offset = (unsigned int)((unsigned int)&g_active_task[0]->task_stats_item - (unsigned int)g_active_task[0])

  # get tcb addr
  set $gdb_tcb_addr = (ktask_t *)((unsigned int)(klist_t *)(&g_kobj_list.task_head)->next - $gdb_offset)
  
  # get task name
  set $gdb_task_name = ((ktask_t *)($gdb_tcb_addr))->task_name
  set $gdb_i = 0

  while ((unsigned int)(&($gdb_tcb_addr->task_stats_item)) != (unsigned int)&g_kobj_list.task_head)

    # print the task info
    p $gdb_i
    p $gdb_task_name
    p *$gdb_tcb_addr

    # set register to restore the task 
    set $x1  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 0)
    set $x3  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 8)
    set $x4  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 16)
    set $x5  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 24)
    set $x6  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 32)
    set $x7  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 40)
    set $x8  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 48)
    set $x9  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 56)
    set $x10 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 64)
    set $x11 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 72)
    set $x12 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 80)
    set $x13 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 88)
    set $x14 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 96)
    set $x15 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 104)
    set $x16 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 112)
    set $x17 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 120)
    set $x18 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 128)
    set $x19 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 136)
    set $x20 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 144)
    set $x21 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 152)
    set $x22 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 160)
    set $x23 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 168)
    set $x24 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 176)
    set $x25 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 184)
    set $x26 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 192)
    set $x27 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 200)
    set $x28 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 208)
    set $x29 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 216)
    set $x30 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 224)
    set $x31 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 232)
    set $pc  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 40 + 512 + 240)

    set $sp = (cpu_stack_t)(($gdb_tcb_addr->task_stack) + (256+256) + 512 + 40 )

	  shell sleep 1
    # print the task stack backtrace
    bt
	
	
    # go to next task
    set $gdb_i = $gdb_i + 1
    set $gdb_tcb_addr = (ktask_t *) ((unsigned int)$gdb_tcb_addr->task_stats_item - $gdb_offset)
    set $gdb_task_name = ((ktask_t *)($gdb_tcb_addr))->task_name
  end
  p $gdb_task_name
  # restore the register
  set  $x0 = $gdb_x0 
  set  $x1 = $gdb_x1 
  set  $x2 = $gdb_x2 
  set  $x3 = $gdb_x3 
  set  $x4 = $gdb_x4 
  set  $x5 = $gdb_x5 
  set  $x6 = $gdb_x6 
  set  $x7 = $gdb_x7 
  set  $x8 = $gdb_x8 
  set  $x9 = $gdb_x9 
  set  $x10 = $gdb_x10
  set  $x11 = $gdb_x11
  set  $x12 = $gdb_x12
  set  $x13 = $gdb_x13
  set  $x14 = $gdb_x14
  set  $x15 = $gdb_x15
  set  $x16 = $gdb_x16
  set  $x17 = $gdb_x17
  set  $x18 = $gdb_x18
  set  $x19 = $gdb_x19
  set  $x20 = $gdb_x20
  set  $x21 = $gdb_x21
  set  $x22 = $gdb_x22
  set  $x23 = $gdb_x23
  set  $x24 = $gdb_x24
  set  $x25 = $gdb_x25
  set  $x26 = $gdb_x26
  set  $x27 = $gdb_x27
  set  $x28 = $gdb_x28
  set  $x29 = $gdb_x29
  set  $x30 = $gdb_x30
  set  $x31 = $gdb_x31
  set  $mcause = $gdb_mcause
  set  $scause = $gdb_scause
  set  $pc = $gdb_pc 
end

paddr
