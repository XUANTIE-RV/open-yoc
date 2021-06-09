# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  p *g_active_task[0]
  bt

  # store the register
  set $gdb_r0  = $r0
  set $gdb_r1  = $r1
  set $gdb_r2  = $r2
  set $gdb_r3  = $r3
  set $gdb_r4  = $r4
  set $gdb_r5  = $r5
  set $gdb_r6  = $r6
  set $gdb_r7  = $r7
  set $gdb_r8  = $r8
  set $gdb_r9  = $r9
  set $gdb_r10 = $r10
  set $gdb_r11 = $r11
  set $gdb_r12 = $r12
  set $gdb_r13 = $r13
  set $gdb_r14 = $r14
  set $gdb_r15 = $r15
  set $gdb_psr = $psr
  set $gdb_pc  = $pc

  # get list offset
  set $gdb_offset = (int)((int)&g_active_task[0]->task_stats_item - (int)g_active_task[0])

  # get tcb addr
  set $gdb_tcb_addr = (ktask_t *)((int)(klist_t *)(&g_kobj_list.task_head)->next - $gdb_offset)
  
  # get task name
  set $gdb_task_name = ((ktask_t *)($gdb_tcb_addr))->task_name
  set $gdb_i = 0

  while (&($gdb_tcb_addr->task_stats_item) != &g_kobj_list.task_head)

    # print the task info
    p $gdb_i
    p $gdb_task_name
    p *$gdb_tcb_addr

    # set register to restore the task 
    set $r0  = *(int *)((int)$gdb_tcb_addr->task_stack + 0)
    set $r1  = *(int *)((int)$gdb_tcb_addr->task_stack + 4)
    set $r2  = *(int *)((int)$gdb_tcb_addr->task_stack + 8)
    set $r3  = *(int *)((int)$gdb_tcb_addr->task_stack + 12)
    set $r4  = *(int *)((int)$gdb_tcb_addr->task_stack + 16)
    set $r5  = *(int *)((int)$gdb_tcb_addr->task_stack + 20)
    set $r6  = *(int *)((int)$gdb_tcb_addr->task_stack + 24)
    set $r7  = *(int *)((int)$gdb_tcb_addr->task_stack + 28)
    set $r8  = *(int *)((int)$gdb_tcb_addr->task_stack + 32)
    set $r9  = *(int *)((int)$gdb_tcb_addr->task_stack + 36)
    set $r10 = *(int *)((int)$gdb_tcb_addr->task_stack + 40)
    set $r11 = *(int *)((int)$gdb_tcb_addr->task_stack + 44)
    set $r12 = *(int *)((int)$gdb_tcb_addr->task_stack + 48)
    set $r13 = *(int *)((int)$gdb_tcb_addr->task_stack + 52)
    set $r15 = *(int *)((int)$gdb_tcb_addr->task_stack + 56)
    set $psr = *(int *)((int)$gdb_tcb_addr->task_stack + 380)
    set $pc  = *(int *)((int)$gdb_tcb_addr->task_stack + 384)
    set $r14 = (int)($gdb_tcb_addr->task_stack) + 388

    # print the task stack backtrace
    bt

    # go to next task
    set $gdb_i = $gdb_i + 1
    set $gdb_tcb_addr = (ktask_t *) ((int)$gdb_tcb_addr->task_stats_item - $gdb_offset)
    set $gdb_task_name = ((ktask_t *)($gdb_tcb_addr))->task_name
  end

  # restore the register
  set $r0  = $gdb_r0
  set $r1  = $gdb_r1
  set $r2  = $gdb_r2
  set $r3  = $gdb_r3
  set $r4  = $gdb_r4
  set $r5  = $gdb_r5
  set $r6  = $gdb_r6
  set $r7  = $gdb_r7
  set $r8  = $gdb_r8
  set $r9  = $gdb_r9
  set $r10 = $gdb_r10
  set $r11 = $gdb_r11
  set $r12 = $gdb_r12
  set $r13 = $gdb_r13
  set $r14 = $gdb_r14
  set $r15 = $gdb_r15
  set $psr = $gdb_psr
  set $pc  = $gdb_pc
end

paddr
