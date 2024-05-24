# dump the thread info for RT-Thread RTOS
# please file the elf before use this script
# Author: WangShun In RT-Thread
set height 0

define paddr
  # print current info
  printf "---RT-Thread Debug Information---\n"
  printf "-Current register information-\n"
  i r
  printf "-Current thread information-\n"
  p *rt_current_thread
  printf "-stack trace-\n"
  bt

  # store the register
  printf "-store the register-\n"
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

  printf "\n"

  # get the first node address from the container
  set $gdb_info = rt_object_get_information((enum rt_object_class_type)RT_Object_Class_Thread) 
  
  set $gdb_node = (&(((struct rt_object_information *)($gdb_info))->object_list))->next

  while(($gdb_node) != &(((struct rt_object_information *)($gdb_info))->object_list))

    # get Thread object node
    set $object = ((struct rt_object *)((char *)($gdb_node) - (unsigned long)(&((struct rt_object *)0)->list)))

    # get thread handle
    set $thread_node = (struct rt_thread *)($object)

    printf "-RT-Thread Thread Information-\n"
    p *$thread_node

    # set register to restore the task
    set $x1  = *(unsigned long *)((unsigned long)($thread_node->sp) +  0 * 4)
    set $x2  = *(unsigned long *)((unsigned long)($thread_node->sp) +  1 * 4)
    set $x3  = *(unsigned long *)((unsigned long)($thread_node->sp) +  2 * 4)
    set $x4  = *(unsigned long *)((unsigned long)($thread_node->sp) +  3 * 4)
    set $x5  = *(unsigned long *)((unsigned long)($thread_node->sp) +  4 * 4)
    set $x6  = *(unsigned long *)((unsigned long)($thread_node->sp) +  5 * 4)
    set $x7  = *(unsigned long *)((unsigned long)($thread_node->sp) +  6 * 4)
    set $x8  = *(unsigned long *)((unsigned long)($thread_node->sp) +  7 * 4)
    set $x9  = *(unsigned long *)((unsigned long)($thread_node->sp) +  8 * 4)
    set $x10 = *(unsigned long *)((unsigned long)($thread_node->sp) +  9 * 4)
    set $x11 = *(unsigned long *)((unsigned long)($thread_node->sp) + 10 * 4)
    set $x12 = *(unsigned long *)((unsigned long)($thread_node->sp) + 11 * 4)
    set $x13 = *(unsigned long *)((unsigned long)($thread_node->sp) + 12 * 4)
    set $x14 = *(unsigned long *)((unsigned long)($thread_node->sp) + 13 * 4)
    set $x15 = *(unsigned long *)((unsigned long)($thread_node->sp) + 14 * 4)
    set $x16 = *(unsigned long *)((unsigned long)($thread_node->sp) + 15 * 4)
    set $x17 = *(unsigned long *)((unsigned long)($thread_node->sp) + 16 * 4)
    set $x18 = *(unsigned long *)((unsigned long)($thread_node->sp) + 17 * 4)
    set $x19 = *(unsigned long *)((unsigned long)($thread_node->sp) + 18 * 4)
    set $x20 = *(unsigned long *)((unsigned long)($thread_node->sp) + 19 * 4)
    set $x21 = *(unsigned long *)((unsigned long)($thread_node->sp) + 20 * 4)
    set $x22 = *(unsigned long *)((unsigned long)($thread_node->sp) + 21 * 4)
    set $x23 = *(unsigned long *)((unsigned long)($thread_node->sp) + 22 * 4)
    set $x24 = *(unsigned long *)((unsigned long)($thread_node->sp) + 23 * 4)
    set $x25 = *(unsigned long *)((unsigned long)($thread_node->sp) + 24 * 4)
    set $x26 = *(unsigned long *)((unsigned long)($thread_node->sp) + 25 * 4)
    set $x27 = *(unsigned long *)((unsigned long)($thread_node->sp) + 26 * 4)
    set $x28 = *(unsigned long *)((unsigned long)($thread_node->sp) + 27 * 4)
    set $x29 = *(unsigned long *)((unsigned long)($thread_node->sp) + 28 * 4)
    set $x30 = *(unsigned long *)((unsigned long)($thread_node->sp) + 29 * 4)
    set $x31 = *(unsigned long *)((unsigned long)($thread_node->sp) + 30 * 4)
    set $pc  = *(unsigned long *)((unsigned long)($thread_node->sp) + 31 * 4)
    # (dsp + float + general) registers offset
    set $sp = (unsigned long)($thread_node->sp) + (4 + 132 + 132)

    # print the task stack backtrace
    bt

    set $gdb_node = ($gdb_node)->next

    printf "Information output completed\n"
    printf "\n"
  end
  printf "-restore the register-\n"
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
printf "---End---\n"
end

paddr