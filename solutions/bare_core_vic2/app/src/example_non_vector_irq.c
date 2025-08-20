/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <csi_core.h>
#include <drv/irq.h>
#include <soc.h>
#include <drv/porting.h>
#include <board.h>

#define  HIGH_IRQ_NUM    26
#define  LOW_IRQ_NUM     25

#define  LOW_PRIO        2
#define  HIGH_PRIO       3

enum irq_trig_mode {
    LEVEL     = 0,
    RISING    = 2,
    FAILLING  = 6
};

enum irq_tailchain_test_flag {
    SUCCESS = 0,
    FAILED = 1
};

static volatile int test_result = FAILED;
extern int* g_top_irqstack;
extern volatile uint32_t g_irq_tailchain_loops;


static csi_dev_t test_high_irq_dev;
static csi_dev_t test_low_irq_dev;

int rst_isr_install(int irq_num, int mode,  int priority,  void(*irq_handler)(void *),  csi_dev_t* test_irq_dev)
{
    if (irq_num > 64 || irq_num < 0 ) {
        printf("Error: IRQ handler is NULL for IRQ %d\n", irq_num);
        return -1;
    }

    if (!irq_handler || !test_irq_dev)
    {
         printf("Error: Pointer is NULL\n");
         return -1;
    }

    test_irq_dev->irq_num = irq_num;
    csi_irq_attach(test_irq_dev->irq_num, irq_handler, test_irq_dev);

    CLIC->CLICINT[irq_num].ATTR = mode;
    csi_vic_set_prio(irq_num, priority);
    csi_irq_enable(irq_num);
    csi_vic_clear_pending_irq(irq_num);
    return 0;
}

void rst_isr_trigger(int irq_num)
{
    csi_vic_set_pending_irq(irq_num);
}

void low_interrupt_isr(void* arg)
{
    printf("low interrupt isr enter, g_irq_tailchain_loops = %d\n", g_irq_tailchain_loops);
    if (g_irq_tailchain_loops == 2)
    {
        test_result = SUCCESS;
    }
    else
    {
        test_result = FAILED;
    }
    printf("low interrupt isr exit\n");
}

void high_interrupt_isr(void* arg)
{
    printf("high interrupt isr enter, g_irq_tailchain_loops = %d\n", g_irq_tailchain_loops);
    rst_isr_trigger(LOW_IRQ_NUM);
    printf("high interrupt isr exit\n");
}

void non_vector_interrupt_example(void)
{
    test_result = FAILED;

    /*disable the timer interrupt to ensure that only the two IRQs  trigger in the example demo*/
    csi_coret_irq_disable();

    printf("non vector irq example start\n");
    if (rst_isr_install(LOW_IRQ_NUM, FAILLING, LOW_PRIO, low_interrupt_isr, &test_low_irq_dev)) {
        printf("low isr install fail\n");
        return;
    }

    if (rst_isr_install(HIGH_IRQ_NUM, FAILLING, HIGH_PRIO, high_interrupt_isr, &test_high_irq_dev)) {
        printf("high isr install fail\n");
    }
    rst_isr_trigger(HIGH_IRQ_NUM);

    /*enable the timer interrupt*/
    csi_coret_irq_enable();

    mdelay(100);

    if (test_result == SUCCESS) {
        printf("non vector irq example success\n");
    } else {
        printf("non vector irq example fail\n");
    }
}