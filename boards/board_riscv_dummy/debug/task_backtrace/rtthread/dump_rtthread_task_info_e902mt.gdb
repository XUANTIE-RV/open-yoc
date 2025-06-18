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
      set $pc =  *(rt_ubase_t *)((unsigned long)$thread_node->sp + 15 * sizeof(rt_ubase_t))

      set $sp = (unsigned long)$thread_node->sp + 17 * sizeof(rt_ubase_t)
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
  set  $pc = $gdb_pc
  set  $sp = $gdb_sp
  printf "\n                   ------- script execution finished!!!!! ------\n"
  end

  paddr