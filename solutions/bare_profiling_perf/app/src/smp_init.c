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
#include <stdio.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <asm/riscv_csr.h>
#include "app_init.h"
#include <drv/irq.h>
#include <soc.h>

#if CONFIG_SMP

enum MP
{
    CORE0 = 0,
    CORE1,
    CORE2,
    CORE3
};

long hw_atomic_exchange(volatile void *ptr, long val)
{
    long result = 0;
#if __riscv_xlen == 32
    asm volatile ("amoswap.w.aqrl %0, %1, (%2)" : "=r"(result) : "r"(val), "r"(ptr) : "memory");
#elif __riscv_xlen == 64
    asm volatile ("amoswap.d.aqrl %0, %1, (%2)" : "=r"(result) : "r"(val), "r"(ptr) : "memory");
#endif
    return result;
}

void hw_atomic_store(volatile void *ptr, long val)
{
    long result = 0;
#if __riscv_xlen == 32
    asm volatile ("amoswap.w.aqrl %0, %1, (%2)" : "=r"(result) : "r"(val), "r"(ptr) : "memory");
#elif __riscv_xlen == 64
    asm volatile ("amoswap.d.aqrl %0, %1, (%2)" : "=r"(result) : "r"(val), "r"(ptr) : "memory");
#endif
}

static inline int spinlock_trylock(spinlock_t *lock)
{
    int res = hw_atomic_exchange(&lock->lock, -1);
    return res;
}

static inline void spinlock_lock(spinlock_t *lock)
{
    while(spinlock_trylock(lock))
        ;
}

static inline void spinlock_unlock(spinlock_t *lock)
{
    hw_atomic_store(&lock->lock, 0);
}

void spin_lock_init(spinlock_t *lock)
{
    lock->lock = 0;
}

void spin_lock(spinlock_t *lock)
{
    spinlock_lock(lock);
}

void spin_unlock(spinlock_t *lock)
{
    spinlock_unlock(lock);
}

__attribute__((weak)) void riscv_soc_start_cpu(int cpu_num)
{
	//do nothing here. overide if necessary
}

__attribute__((weak)) void riscv_soc_init_cpu(void)
{
	//do nothing here. overide if necessary
}

static void _secondary_cpu_init(void)
{
    riscv_soc_init_cpu();
#if 1
    // FIXME: Needs to be matched with the specific CPU model
    /* enable theadisaee & MM */
    unsigned long status = __get_MXSTATUS();
    status |= (1 << 22 | 1 << 15);
#if __riscv_matrix || __riscv_xtheadmatrix
    /* enable matrix */
    status &= ~(1ul << 0);
#endif /* __riscv_matrix || __riscv_xtheadmatrix */
    __set_MXSTATUS(status);

#if __riscv_flen == 64
    /* enable float ISA */
    status = __get_MSTATUS();
    status |= SR_FS_INITIAL;
    __set_MSTATUS(status);
#endif
#ifdef __riscv_vector
    /* enable vector ISA */
    status = __get_MSTATUS();
    status |= SR_VS_INITIAL;
    __set_MSTATUS(status);
#endif

#if __riscv_matrix || __riscv_xtheadmatrix
    /* enable matrix ISA */
    status = __get_MSTATUS();
    status |= SR_MS_INITIAL;
    __set_MSTATUS(status);
#endif

#if CONFIG_ECC_L1_ENABLE
    /* enable L1 cache ecc */
    uint64_t mhint = __get_MHINT();
    mhint |= (0x1 << 19);
    __set_MHINT(mhint);
#endif

#ifdef CONFIG_RISCV_SMODE
    /* enable ecall delegate */
    unsigned long medeleg = __get_MEDELEG();
    medeleg |= (1 << 9);
    __set_MEDELEG(medeleg);

    /* enable interrupt delegate */
    unsigned long mideleg = __get_MIDELEG();
    mideleg |= 0x222;
    __set_MIDELEG(mideleg);
#endif

#ifdef CONFIG_RISCV_SMODE
    /* enable mcounteren for s-mode */
    __set_MCOUNTEREN(0xffffffff);

#if CBO_INSN_SUPPORT
    unsigned long envcfg = __get_MENVCFG();
    /* enable CBIE & CBCFE & CBZE on lower priviledge */
    envcfg |= (3 << 4 | 1 << 6 | 1 << 7);
    __set_MENVCFG(envcfg);
#endif
#endif
#endif
}

#if CONFIG_INTC_CLIC_PLIC
static void clic_init(void)
{
    int i;

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    for (i = 0; i < 64; i++) {
        uint8_t nlbits = (CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos;
        CLIC->CLICINT[i].CTL = (CLIC->CLICINT[i].CTL & (~CLIC_INTCFG_PRIO_Msk)) | (0x1 << (8 - nlbits));
        CLIC->CLICINT[i].IP = 0;
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
    }

    /* tspend use positive interrupt */
    CLIC->CLICINT[Machine_Software_IRQn].ATTR = 0x3;
    csi_irq_enable(Machine_Software_IRQn);

    /* enable external plic interrupt */
    csi_irq_enable(Machine_External_IRQn);

#if CONFIG_ECC_L1_ENABLE || CONFIG_ECC_ITCM_ENABLE || CONFIG_ECC_DTCM_ENABLE
    CLIC->CLICINT[L1_CACHE_ECC_IRQn].ATTR = 0x3;
    csi_irq_enable(L1_CACHE_ECC_IRQn);
#endif
}
#endif

void _scondary_interrupt_init(void)
{
    int i;
    int core_id = 0;
    core_id = csi_get_cpu_id();

    if (core_id == CORE0)
    {
        return;
    }
#if CONFIG_INTC_CLIC_PLIC
    clic_init();
    if (core_id == CORE1)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM - PLIC_IRQ_OFFSET + 31) / 32; i++)
        {
            PLIC->PLIC_H1_MIE[i] = 0;
            PLIC->PLIC_H1_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H1_MTH = 0;
        PLIC->PLIC_H1_STH = 0;
    }
    else if (core_id == CORE2)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM - PLIC_IRQ_OFFSET + 31) / 32; i++)
        {
            PLIC->PLIC_H2_MIE[i] = 0;
            PLIC->PLIC_H2_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H2_MTH = 0;
        PLIC->PLIC_H2_STH = 0;
    }
    else if (core_id == CORE3)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM - PLIC_IRQ_OFFSET + 31) / 32; i++)
        {
            PLIC->PLIC_H3_MIE[i] = 0;
            PLIC->PLIC_H3_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H3_MTH = 0;
        PLIC->PLIC_H3_STH = 0;
    }
    else
    {
        printf("Illegal CPU core, ID = %d", core_id);
        while (1)
            ;
    }
#else
    if (core_id == CORE1)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM + 31) / 32; i++)
        {
            PLIC->PLIC_H1_MIE[i] = 0;
            PLIC->PLIC_H1_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H1_MTH = 0;
        PLIC->PLIC_H1_STH = 0;
    }
    else if (core_id == CORE2)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM + 31) / 32; i++)
        {
            PLIC->PLIC_H2_MIE[i] = 0;
            PLIC->PLIC_H2_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H2_MTH = 0;
        PLIC->PLIC_H2_STH = 0;
    }
    else if (core_id == CORE3)
    {
        for (i = 0; i < (CONFIG_IRQ_NUM + 31) / 32; i++)
        {
            PLIC->PLIC_H3_MIE[i] = 0;
            PLIC->PLIC_H3_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H3_MTH = 0;
        PLIC->PLIC_H3_STH = 0;
    }
    else
    {
        printf("Illegal CPU core, ID = %d\n", core_id);
        while (1)
            ;
    }

    /* enable MEIE & MSIE */
    unsigned long mie = __get_MIE();
    mie |= (1 << 11 | 1 << 7 | 1 << 3);
#if CONFIG_ECC_L1_ENABLE || CONFIG_ECC_ITCM_ENABLE || CONFIG_ECC_DTCM_ENABLE
    mie |= (1 << 16);
#endif
    __set_MIE(mie);
#endif

#ifdef CONFIG_KERNEL_NONE
    unsigned long mstatus = __get_MSTATUS();
    mstatus |= (1 << 3);
    __set_MSTATUS(mstatus);
#endif
}

static int _tick_init(void)
{
    int core_id = csi_get_cpu_id();

    if (core_id == CORE0)
    {
        return 0;
    }
    csi_tick_init();
    return 0;
}

static int _clint_ipi_enable(void)
{
    /* Set the Machine-Software bit in MIE */
    rv_csr_set(0x304, SR_MIE);
    return 0;
}

extern long secondary_boot_flag;
void secondary_cpu_up(void)
{
    for (int i = 1; i < CONFIG_NR_CPUS; i++) {
        riscv_soc_start_cpu(i);
    }
    __DMB();
    secondary_boot_flag = 0xa55a;
}

void secondary_cpu_c_start(void)
{
    _secondary_cpu_init();

    /* initialize interrupt controller */
    _scondary_interrupt_init();

    /* System timer initialization */
    _tick_init();

    _clint_ipi_enable();

#if CONFIG_DEMO_PERF_RECORD
    extern int perf_record_test(void);
    perf_record_test();
#endif

#if CONFIG_DEMO_PERF_STAT
    extern int perf_stat_test(void);
    perf_stat_test();
#endif
}
#endif