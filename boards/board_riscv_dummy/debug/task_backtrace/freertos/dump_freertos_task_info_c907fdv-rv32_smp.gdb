# dump the task info for FreeRTOS
set height 0

define tasklist_bt
  set $gdb_task_list = (List_t*)$arg0

  if ($gdb_task_list)
    set $gdb_task_num = (unsigned long)$gdb_task_list->uxNumberOfItems
    set $gdb_list_next = (ListItem_t*)$gdb_task_list->xListEnd->pxNext

    while ($gdb_task_num)
      # get tcb addr
      set $gdb_tcb_addr = (TCB_t*)((ListItem_t*)$gdb_list_next->pvOwner)
      if ($gdb_tcb_addr == 0x0)
        set $gdb_list_next = (ListItem_t*)$gdb_list_next->pxNext
        set $gdb_tcb_addr = (TCB_t*)((ListItem_t*)$gdb_list_next->pvOwner)
      end
      # get task name
      set $gdb_task_name = ((TCB_t*)($gdb_tcb_addr))->pcTaskName

      # Is the task running
      set $is_task_run = ((TCB_t*)($gdb_tcb_addr)->xTaskRunState >= 0) && ((TCB_t*)($gdb_tcb_addr)->xTaskRunState < $cpu_num)

      if ($is_task_run == 0)
        # print the task info
        p $gdb_task_name
        p *$gdb_tcb_addr

        # (F31 ~ F0 + FCSR) + (V31 ~ V0 + 5*VCSR)
        set $gdb_vlenb = $vlenb
        set $offset    = (32 * 8 + 1 * sizeof(unsigned long)) + (32 * ($gdb_vlenb) + 5 * sizeof(unsigned long))
        # set register to restore the task
        set $x1  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 0  * sizeof(unsigned long))
        set $x3  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 1  * sizeof(unsigned long))
        set $x4  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 2  * sizeof(unsigned long))
        set $x5  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 3  * sizeof(unsigned long))
        set $x6  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 4  * sizeof(unsigned long))
        set $x7  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 5  * sizeof(unsigned long))
        set $x8  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 6  * sizeof(unsigned long))
        set $x9  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 7  * sizeof(unsigned long))
        set $x10 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 8  * sizeof(unsigned long))
        set $x11 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 9  * sizeof(unsigned long))
        set $x12 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 10 * sizeof(unsigned long))
        set $x13 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 11 * sizeof(unsigned long))
        set $x14 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 12 * sizeof(unsigned long))
        set $x15 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 13 * sizeof(unsigned long))
        set $x16 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 14 * sizeof(unsigned long))
        set $x17 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 15 * sizeof(unsigned long))
        set $x18 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 16 * sizeof(unsigned long))
        set $x19 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 17 * sizeof(unsigned long))
        set $x20 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 18 * sizeof(unsigned long))
        set $x21 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 19 * sizeof(unsigned long))
        set $x22 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 20 * sizeof(unsigned long))
        set $x23 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 21 * sizeof(unsigned long))
        set $x24 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 22 * sizeof(unsigned long))
        set $x25 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 23 * sizeof(unsigned long))
        set $x26 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 24 * sizeof(unsigned long))
        set $x27 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 25 * sizeof(unsigned long))
        set $x28 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 26 * sizeof(unsigned long))
        set $x29 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 27 * sizeof(unsigned long))
        set $x30 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 28 * sizeof(unsigned long))
        set $x31 = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 29 * sizeof(unsigned long))
        set $pc  = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 30 * sizeof(unsigned long))
        set $mstatus = *(unsigned long *)((unsigned long)$gdb_tcb_addr->pxTopOfStack + $offset + 31 * sizeof(unsigned long))
        set $sp = (unsigned long)($gdb_tcb_addr->pxTopOfStack) + $offset + 32 * sizeof(unsigned long)

        # print the task stack backtrace
        bt
        shell sleep 1
      end

      set $gdb_task_num = $gdb_task_num - 1
      # go to next task
      set $gdb_list_next = (ListItem_t*)$gdb_list_next->pxNext
    end
  end
end


define paddr
  # print current info
  i r
  set $current_cpu_id = $mhartid
  set $current_task_name = ((TCB_t*)(pxCurrentTCBs[$current_cpu_id]))->pcTaskName
  p $current_task_name
  p *(TCB_t*)pxCurrentTCBs[$mhartid]
  bt

  # store the register
  set $gdb_x0  = $x0
  set $gdb_x1  = $x1
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
  set $gdb_mstatus = $mstatus
  set $gdb_pc  = $pc
  set $gdb_sp  = $x2

  printf "\n-print out the currently running task information for other cpus-\n"
  set $cpu_num = sizeof(pxCurrentTCBs)/sizeof(pxCurrentTCBs[0])
  set $cpu_id = 0
  while ($cpu_id < $cpu_num)
      if ($cpu_id != $current_cpu_id)
        set $current_task_name = ((TCB_t*)(pxCurrentTCBs[$cpu_id]))->pcTaskName
        printf "task: %s runing on %d cpu\n", $current_task_name, $cpu_id
        p $current_task_name
        p *(TCB_t*)pxCurrentTCBs[$cpu_id]
      end
      set $cpu_id = $cpu_id + 1
      printf "\n"
  end
  printf "\n"

  printf "-backtrace for pxReadyTasksLists-\n"
  set $gdb_max_priority = sizeof(pxReadyTasksLists)/sizeof(pxReadyTasksLists[0])
  set $gdb_priority_index = 0
  while($gdb_max_priority > $gdb_priority_index)
      set $ready_nums = pxReadyTasksLists[$gdb_priority_index].uxNumberOfItems
      if($ready_nums != 0)
        tasklist_bt &pxReadyTasksLists[$gdb_priority_index]
      end
      set $gdb_priority_index = $gdb_priority_index + 1
  end
  printf "-backtrace for pxDelayedTaskList-\n"
  tasklist_bt pxDelayedTaskList
  printf "-backtrace for pxOverflowDelayedTaskList-\n"
  tasklist_bt pxOverflowDelayedTaskList
  printf "-backtrace for xTasksWaitingTermination-\n"
  tasklist_bt &xTasksWaitingTermination
  printf "-backtrace for xSuspendedTaskList-\n"
  tasklist_bt &xSuspendedTaskList

  # restore the register
  set  $x0 = $gdb_x0
  set  $x1 = $gdb_x1
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
  set  $mstatus = $gdb_mstatus
  set  $pc = $gdb_pc
  set  $sp = $gdb_sp
end

paddr
