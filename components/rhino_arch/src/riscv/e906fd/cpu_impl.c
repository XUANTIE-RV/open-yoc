/*
 * Copyright (C) 2015-2023 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_config.h>

#if (RHINO_CONFIG_STACK_OVF_CHECK_HW != 0)
void cpu_intrpt_stack_protect(void)
{
}

void task_stack_crash_warning(void)
{
    printf("****The task stack base has been broken !!!****\n");
}

void cpu_task_stack_protect(cpu_stack_t *base, size_t size)
{
    uint32_t base_addr = (uint32_t)base;

    int num_return = wp_register(base_addr, AWATCH, task_stack_crash_warning);
    if (num_return == 1) {
        wp_enable(1);
    } else if (num_return == 2 || num_return == -1) {
          wp_unregister(1);
          int number_tmp = wp_register(base_addr, AWATCH, task_stack_crash_warning);
          if (number_tmp == 1){
              wp_enable(1);
          }
      }
}
#endif
