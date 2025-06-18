# dump the task info for RT-Thread
set height 0

define tasklist_bt
  # get the first node address from the container

  set $type = (enum rt_object_class_type)(RT_Object_Class_Thread & ~RT_Object_Class_Static)
  set $index = 0
  while ($index < RT_Object_Info_Unknown)
    if (_object_container[$index].type == $type)
	    set $gdb_info =  &_object_container[$index]
    end
    set $index = $index + 1
  end
  set $gdb_node = (&(((struct rt_object_information *)($gdb_info))->object_list))->next

  while(($gdb_node) != &(((struct rt_object_information *)($gdb_info))->object_list))

    # get Thread object node
    set $object = ((struct rt_object *)((char *)($gdb_node) - (unsigned long)(&((struct rt_object *)0)->list)))

    # get thread handle
    set $thread_node = (struct rt_thread *)($object)

    if $thread_node != $current_thread
      printf "\n"
      p $thread_node->parent.name
      p *$thread_node

      # set register to restore the task
      set $x1  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  0 * sizeof(rt_ubase_t))
      set $x3  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  2 * sizeof(rt_ubase_t))
      set $x4  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  3 * sizeof(rt_ubase_t))
      set $x5  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  4 * sizeof(rt_ubase_t))
      set $x6  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  5 * sizeof(rt_ubase_t))
      set $x7  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  6 * sizeof(rt_ubase_t))
      set $x8  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  7 * sizeof(rt_ubase_t))
      set $x9  = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  8 * sizeof(rt_ubase_t))
      set $x10 = *(rt_ubase_t *)((unsigned long)$thread_node->sp +  9 * sizeof(rt_ubase_t))
      set $x11 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 10 * sizeof(rt_ubase_t))
      set $x12 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 11 * sizeof(rt_ubase_t))
      set $x13 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 12 * sizeof(rt_ubase_t))
      set $x14 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 13 * sizeof(rt_ubase_t))
      set $x15 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 14 * sizeof(rt_ubase_t))
      set $x16 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 15 * sizeof(rt_ubase_t))
      set $x17 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 16 * sizeof(rt_ubase_t))
      set $x18 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 17 * sizeof(rt_ubase_t))
      set $x19 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 18 * sizeof(rt_ubase_t))
      set $x20 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 19 * sizeof(rt_ubase_t))
      set $x21 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 20 * sizeof(rt_ubase_t))
      set $x22 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 21 * sizeof(rt_ubase_t))
      set $x23 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 22 * sizeof(rt_ubase_t))
      set $x24 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 23 * sizeof(rt_ubase_t))
      set $x25 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 24 * sizeof(rt_ubase_t))
      set $x26 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 25 * sizeof(rt_ubase_t))
      set $x27 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 26 * sizeof(rt_ubase_t))
      set $x28 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 27 * sizeof(rt_ubase_t))
      set $x29 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 28 * sizeof(rt_ubase_t))
      set $x30 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 29 * sizeof(rt_ubase_t))
      set $x31 = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 30 * sizeof(rt_ubase_t))
      set $pc  = *(rt_ubase_t *)((unsigned long)$thread_node->sp + 31 * sizeof(rt_ubase_t))

      # F31 ~ F0 + FCSR + vxsat
      set $offset  = (32 * 4) + (1 * 4) + (1 * 4)
      set $sp = (unsigned long)$thread_node->sp + $offset + 33 * sizeof(rt_ubase_t)

      # print the thread stack backtrace
      printf "\n"
      bt
      printf "--------thread information output completed--------\n\n"
    end
    set $gdb_node = ($gdb_node)->next
    shell sleep 1
  end
end

define paddr
  printf "\n                   ------- step 1:store current the register ------\n"
  printf "\n------current cpu register information-------\n"
  i r
  set $current_thread = rt_current_thread
  printf "\n"
  p  $current_thread->parent.name
  p *$current_thread
  printf "\n"
  bt
  # store the register
  printf "\nstore current register......\n"
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
  set $gdb_pc  = $pc
  set $gdb_sp  = $sp

  printf "\n                   ------- step 2:print all thread information ------\n"
  tasklist_bt
  printf "\n                   ------- step 3:restore current register ------\n"
  printf "\nrestore current register......\n"
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
  set  $pc = $gdb_pc
  set  $sp = $gdb_sp
  printf "\n                   ------- script execution finished!!!!! ------\n"
  end

  paddr