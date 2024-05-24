/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <board.h>

void board_init(void)
{
    /* some board preconfig */
    // board_xxx();

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
#if !defined(RT_DEBUG_INIT) || !RT_DEBUG_INIT
    board_uart_init();
#endif
#endif

#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
    board_pwm_init();
#endif

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
    board_adc_init();
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0
    board_button_init();
#endif

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
    board_led_init();
#endif

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT > 0
    board_audio_init();
#endif
}

#if defined(CONFIG_KERNEL_RTTHREAD)
#include <unistd.h>
#include <rthw.h>
#include <rtthread.h>

extern unsigned long __heap_start;
extern unsigned long __heap_end;

void rt_hw_board_init(void)
{
    /* initalize interrupt */
    rt_hw_interrupt_init();

#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_system_heap_init((void *)&__heap_start, (void *)&__heap_end);
#endif

#if RT_DEBUG_INIT
    board_uart_init();
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

#if CONFIG_AOS_OSAL
#include <devices/uart.h>
#include <aos/console_uart.h>

extern int32_t aos_debug_printf(const char *fmt, ...);
void rt_hw_console_output(const char *str)
{
    aos_debug_printf("\r%s", str);
}

char rt_hw_console_getchar(void)
{
    int ch = -1;

    while (rvm_hal_uart_recv(console_get_uart(), &ch, 1, -1) != 1);

    return ch;
}
#else
void rt_hw_console_output(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str));
}

char rt_hw_console_getchar(void)
{
    return fgetc(NULL);
}
#endif /* CONFIG_AOS_OSAL */
#endif

#if CONFIG_BOARD_XIAOHUI_EVB
#if defined(CONFIG_SMP) && CONFIG_SMP

#include <csi_core.h>

extern void Reset_Handler(void);

static struct c9xx_regs_struct {
    uint64_t pmpaddr0;
    uint64_t pmpaddr1;
    uint64_t pmpaddr2;
    uint64_t pmpaddr3;
    uint64_t pmpaddr4;
    uint64_t pmpaddr5;
    uint64_t pmpaddr6;
    uint64_t pmpaddr7;
    uint64_t pmpcfg0;
    uint64_t mcor;
    uint64_t mhcr;
    uint64_t mccr2;
    uint64_t mhint;
    uint64_t msmpr;
    uint64_t mie;
    uint64_t mxstatus;
    uint64_t mtvec;
    uint64_t plic_base_addr;
    uint64_t clint_base_addr;
} c9xx_regs;

#define C9xx_PLIC_CLINT_OFFSET     0x04000000  /* 64M */
#define C9xx_PLIC_DELEG_OFFSET     0x001ffffc
#define C9xx_PLIC_DELEG_ENABLE     0x1

#define CSR_MCOR         0x7c2
#define CSR_MHCR         0x7c1
#define CSR_MCCR2        0x7c3
#define CSR_MHINT        0x7c5
#define CSR_MXSTATUS     0x7c0
#define CSR_MIE          0x304
#define CSR_PLIC_BASE    0xfc1
#define CSR_MRMR         0x7c6
#define CSR_MRVBR        0x7c7
#define CSR_MSMPR        0x7f3
#define CSR_MTVEC        0x305
/* Machine Memory Protection */
#define CSR_PMPCFG0                     0x3a0
#define CSR_PMPCFG1                     0x3a1
#define CSR_PMPCFG2                     0x3a2
#define CSR_PMPCFG3                     0x3a3
#define CSR_PMPCFG4                     0x3a4
#define CSR_PMPCFG5                     0x3a5
#define CSR_PMPCFG6                     0x3a6
#define CSR_PMPCFG7                     0x3a7
#define CSR_PMPCFG8                     0x3a8
#define CSR_PMPCFG9                     0x3a9
#define CSR_PMPCFG10                    0x3aa
#define CSR_PMPCFG11                    0x3ab
#define CSR_PMPCFG12                    0x3ac
#define CSR_PMPCFG13                    0x3ad
#define CSR_PMPCFG14                    0x3ae
#define CSR_PMPCFG15                    0x3af
#define CSR_PMPADDR0                    0x3b0
#define CSR_PMPADDR1                    0x3b1
#define CSR_PMPADDR2                    0x3b2
#define CSR_PMPADDR3                    0x3b3
#define CSR_PMPADDR4                    0x3b4
#define CSR_PMPADDR5                    0x3b5
#define CSR_PMPADDR6                    0x3b6
#define CSR_PMPADDR7                    0x3b7

#define XIAOHUI_SRESET_BASE              0x18030000
#define XIAOHUI_SRESET_ADDR_OFFSET       0x10
#define PRIMARY_STARTUP_CORE_ID		0

static void c9xx_csr_copy(void)
{
    if (PRIMARY_STARTUP_CORE_ID == csi_get_cpu_id()) {
        /* Load from boot core */
        c9xx_regs.pmpaddr0 = csr_read(CSR_PMPADDR0);
        c9xx_regs.pmpaddr1 = csr_read(CSR_PMPADDR1);
        c9xx_regs.pmpaddr2 = csr_read(CSR_PMPADDR2);
        c9xx_regs.pmpaddr3 = csr_read(CSR_PMPADDR3);
        c9xx_regs.pmpaddr4 = csr_read(CSR_PMPADDR4);
        c9xx_regs.pmpaddr5 = csr_read(CSR_PMPADDR5);
        c9xx_regs.pmpaddr6 = csr_read(CSR_PMPADDR6);
        c9xx_regs.pmpaddr7 = csr_read(CSR_PMPADDR7);
        c9xx_regs.pmpcfg0  = csr_read(CSR_PMPCFG0);
        c9xx_regs.mcor     = csr_read(CSR_MCOR);
        c9xx_regs.msmpr    = csr_read(CSR_MSMPR);
        c9xx_regs.mhcr     = csr_read(CSR_MHCR);
        c9xx_regs.mccr2    = csr_read(CSR_MCCR2);
        c9xx_regs.mhint    = csr_read(CSR_MHINT);
        c9xx_regs.mtvec    = csr_read(CSR_MTVEC);
        c9xx_regs.mie      = csr_read(CSR_MIE);
        c9xx_regs.mxstatus = csr_read(CSR_MXSTATUS);

        c9xx_regs.plic_base_addr = csr_read(CSR_PLIC_BASE);
        c9xx_regs.clint_base_addr = c9xx_regs.plic_base_addr + C9xx_PLIC_CLINT_OFFSET;
    } else {
        /* Store to other core */
        // csr_write(CSR_PMPADDR0, c9xx_regs.pmpaddr0);
        // csr_write(CSR_PMPADDR1, c9xx_regs.pmpaddr1);
        // csr_write(CSR_PMPADDR2, c9xx_regs.pmpaddr2);
        // csr_write(CSR_PMPADDR3, c9xx_regs.pmpaddr3);
        // csr_write(CSR_PMPADDR4, c9xx_regs.pmpaddr4);
        // csr_write(CSR_PMPADDR5, c9xx_regs.pmpaddr5);
        // csr_write(CSR_PMPADDR6, c9xx_regs.pmpaddr6);
        // csr_write(CSR_PMPADDR7, c9xx_regs.pmpaddr7);
        // csr_write(CSR_PMPCFG0,  c9xx_regs.pmpcfg0);
        csr_write(CSR_MCOR,     c9xx_regs.mcor);
        csr_write(CSR_MSMPR,    c9xx_regs.msmpr);
        csr_write(CSR_MHCR,     c9xx_regs.mhcr);
        csr_write(CSR_MHINT,    c9xx_regs.mhint);
        // csr_write(CSR_MTVEC,    c9xx_regs.mtvec);
        // csr_write(CSR_MIE,      c9xx_regs.mie);
        csr_write(CSR_MXSTATUS, c9xx_regs.mxstatus);
    }
}

void riscv_soc_start_cpu(int cpu_num)
{
	uint32_t mrmr;

	c9xx_csr_copy();
	*(unsigned long *)((unsigned long)XIAOHUI_SRESET_BASE + XIAOHUI_SRESET_ADDR_OFFSET + ((cpu_num - 1) << 3)) = (unsigned long)Reset_Handler;
	__ASM("sync");
	mrmr = *(uint32_t *)(XIAOHUI_SRESET_BASE);
	*(uint32_t *)(XIAOHUI_SRESET_BASE) = mrmr | (0x1 << (cpu_num - 1));
	__ASM("sync");
}

void riscv_soc_init_cpu(void)
{
	c9xx_csr_copy();
}

#endif
#endif /*CONFIG_BOARD_XIAOHUI_EVB*/
