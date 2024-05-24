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
    set $x1  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  0 * 8)
    set $x3  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  2 * 8)
    set $x4  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  3 * 8)
    set $x5  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  4 * 8)
    set $x6  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  5 * 8)
    set $x7  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  6 * 8)
    set $x8  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  7 * 8)
    set $x9  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  8 * 8)
    set $x10 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 +  9 * 8)
    set $x11 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 10 * 8)
    set $x12 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 11 * 8)
    set $x13 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 12 * 8)
    set $x14 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 13 * 8)
    set $x15 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 14 * 8)
    set $x16 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 15 * 8)
    set $x17 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 16 * 8)
    set $x18 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 17 * 8)
    set $x19 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 18 * 8)
    set $x20 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 19 * 8)
    set $x21 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 20 * 8)
    set $x22 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 21 * 8)
    set $x23 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 22 * 8)
    set $x24 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 23 * 8)
    set $x25 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 24 * 8)
    set $x26 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 25 * 8)
    set $x27 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 26 * 8)
    set $x28 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 27 * 8)
    set $x29 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 28 * 8)
    set $x30 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 29 * 8)
    set $x31 = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 30 * 8)
    set $pc  = *(struct rt_thread *)((unsigned long)$thread_node->sp + 512 + 40 + 264 + 31 * 8)
    # (vector + float + general) registers offset
    set $sp = (unsigned long)$thread_node->sp + ((512 + 40) + 264 + 264)

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