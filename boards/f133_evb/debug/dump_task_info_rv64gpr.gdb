# dump the task info for rhino RTOS
# please file the elf before use this script
set height 0

define paddr
  # print current info
  i r
  p *g_active_task[0]
  bt

    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F31         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F30         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F29         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F28         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F27         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F26         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F25         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F24         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F23         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F22         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F21          *#/
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F20          *#/
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F19         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F18         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F17         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F16         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F15         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F14         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F13         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F12         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F11         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F10         */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F9          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F8          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F7          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F6          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F5          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F4          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F3          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F2          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F1          */#
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* F0          */#

    #/* FS=0b01 MPP=0b11 MPIE=0b1 */
    #*(--stk)  = (cpu_stack_t)SR_FS_INITIAL | SR_MPP_M | SR_MPIE; /* mstatus */
    #*(--stk)  = (cpu_stack_t)entry;                     /* Entry Point */

    #*(--stk)  = (cpu_stack_t)0x3131313131313131L;       /* X31         */
    #*(--stk)  = (cpu_stack_t)0x3030303030303030L;       /* X30         */
    #*(--stk)  = (cpu_stack_t)0x2929292929292929L;       /* X29         */
    #*(--stk)  = (cpu_stack_t)0x2828282828282828L;       /* X28         */
    #*(--stk)  = (cpu_stack_t)0x2727272727272727L;       /* X27         */
    #*(--stk)  = (cpu_stack_t)0x2626262626262626L;       /* X26         */
    #*(--stk)  = (cpu_stack_t)0x2525252525252525L;       /* X25         */
    #*(--stk)  = (cpu_stack_t)0x2424242424242424L;       /* X24         */
    #*(--stk)  = (cpu_stack_t)0x2323232323232323L;       /* X23         */
    #*(--stk)  = (cpu_stack_t)0x2222222222222222L;       /* X22         */
    #*(--stk)  = (cpu_stack_t)0x2121212121212121L;       /* X21         */
    #*(--stk)  = (cpu_stack_t)0x2020202020202020L;       /* X20         */
    #*(--stk)  = (cpu_stack_t)0x1919191919191919L;       /* X19         */
    #*(--stk)  = (cpu_stack_t)0x1818181818181818L;       /* X18         */
    #*(--stk)  = (cpu_stack_t)0x1717171717171717L;       /* X17         */
    #*(--stk)  = (cpu_stack_t)0x1616161616161616L;       /* X16         */
    #*(--stk)  = (cpu_stack_t)0x1515151515151515L;       /* X15         */
    #*(--stk)  = (cpu_stack_t)0x1414141414141414L;       /* X14         */
    #*(--stk)  = (cpu_stack_t)0x1313131313131313L;       /* X13         */
    #*(--stk)  = (cpu_stack_t)0x1212121212121212L;       /* X12         */
    #*(--stk)  = (cpu_stack_t)0x1111111111111111L;       /* X11         */
    #*(--stk)  = (cpu_stack_t)arg;                       /* X10         */
    #*(--stk)  = (cpu_stack_t)0x0909090909090909L;       /* X9          */
    #*(--stk)  = (cpu_stack_t)0x0808080808080808L;       /* X8          */
    #*(--stk)  = (cpu_stack_t)0x0707070707070707L;       /* X7          */
    #*(--stk)  = (cpu_stack_t)0x0606060606060606L;       /* X6          */
    #*(--stk)  = (cpu_stack_t)0x0505050505050505L;       /* X5          */
    #*(--stk)  = (cpu_stack_t)0x0404040404040404L;       /* X4          */
    #*(--stk)  = (uint64_t)gp;                           /* X3          */
    #*(--stk)  = (cpu_stack_t)krhino_task_deathbed;      /* X1          */

    #*(--stk)  = (cpu_stack_t)0x0L;                      /* VXRM        */
    #*(--stk)  = (cpu_stack_t)0x0L;                      /* VXSAT       */
    #*(--stk)  = (cpu_stack_t)0x0L;                      /* VSTART      */
    #*(--stk)  = (cpu_stack_t)0x0L;                      /* VTYPE       */
    #*(--stk)  = (cpu_stack_t)0x0L;                      /* VL          */

    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V31         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V31         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V30         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V30         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V29         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V29         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V28         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V28         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V27         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V27         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V26         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V26         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V25         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V25         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V24         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V24         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V23         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V23         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V22         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V22         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V21          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V21          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V20          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V20          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V19         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V19         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V18         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V18         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V17         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V17         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V16         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V16         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V15         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V15         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V14         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V14         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V13         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V13         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V12         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V12         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V11         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V11         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V10         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V10         */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V9          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V9          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V8          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V8          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V7          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V7          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V6          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V6          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V5          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V5          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V4          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V4          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V3          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V3          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V2          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V2          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V1          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V1          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V0          */
    #*(--stk)  = (cpu_stack_t)0x1234567812345678L;       /* V0          */




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
    set $x1  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +0)
    set $x3  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +8)
    set $x4  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +16)
    set $x5  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +24)
    set $x6  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +32)
    set $x7  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +40)
    set $x8  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +48)
    set $x9  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +56)
    set $x10 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +64)
    set $x11 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +72)
    set $x12 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +80)
    set $x13 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +88)
    set $x14 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +96)
    set $x15 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +104)
    set $x16 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +112)
    set $x17 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +120)
    set $x18 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +128)
    set $x19 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +136)
    set $x20 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +144)
    set $x21 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +152)
    set $x22 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +160)
    set $x23 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +168)
    set $x24 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +176)
    set $x25 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +184)
    set $x26 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +192)
    set $x27 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +200)
    set $x28 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +208)
    set $x29 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +216)
    set $x30 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +224)
    set $x31 = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +232)
    set $pc  = *(cpu_stack_t *)((cpu_stack_t)$gdb_tcb_addr->task_stack + 512 + 40 +240)
    set $sp = (cpu_stack_t)(($gdb_tcb_addr->task_stack) + (256+256 + 512 + 40))

	
    # print the task stack backtrace
    bt
	
	
    # go to next task
    set $gdb_i = $gdb_i + 1
    set $gdb_tcb_addr = (ktask_t *) ((int)$gdb_tcb_addr->task_stats_item - $gdb_offset)
    set $gdb_task_name = ((ktask_t *)($gdb_tcb_addr))->task_name
  end
  p $gdb_task_name
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
end

paddr
