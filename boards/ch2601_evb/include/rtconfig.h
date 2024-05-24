#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

#ifdef CONFIG_KERNEL_RTTHREAD

#ifndef __ASSEMBLY__
/* define extra stack size */
#if __riscv_matrix
static inline int _csi_xmlenb_get_value(void)
{
    int result;
    __asm volatile("csrr %0, xmlenb" : "=r"(result) : : "memory");
    return result;
}
#define STACK_M_EXTRAL_SIZE     (_csi_xmlenb_get_value() * 8 + 24)
#else
#define STACK_M_EXTRAL_SIZE     0
#endif

#ifdef __riscv_vector
static inline int _csi_vlenb_get_value(void)
{
    int result;
    __asm volatile("csrr %0, vlenb" : "=r"(result) : : "memory");
    return result;
}

#define STACK_V_EXTRAL_SIZE     (_csi_vlenb_get_value() * 32 + 40)
#else
#define STACK_V_EXTRAL_SIZE     0
#endif

#ifdef __riscv_flen
#define STACK_F_EXTRAL_SIZE     (__riscv_flen / 8 * 32 + 8)
#else
#define STACK_F_EXTRAL_SIZE     0
#endif /*__riscv_flen*/

#define CSK_CPU_STACK_EXTRAL    (STACK_M_EXTRAL_SIZE + STACK_V_EXTRAL_SIZE + STACK_F_EXTRAL_SIZE)

#if defined(__riscv_matrix) || defined(__riscv_vector)
/* FIXME: for static allocate stack */
#define STATIC_CSK_CPU_STACK_EXTRAL       (4096)
#else
#define STATIC_CSK_CPU_STACK_EXTRAL       (0)
#endif
#endif /* __ASSEMBLY__ */


/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */
// #define ENABLE_FPU
// #define ENABLE_VECTOR // Enable vector operations
// #define ARCH_CPU_64BIT
/* RT-Thread Kernel */
#define RT_NAME_MAX 48
#define RT_ALIGN_SIZE 8
#ifdef CONFIG_NR_CPUS
#define RT_CPUS_NR CONFIG_NR_CPUS
#else
#define RT_CPUS_NR 1
#endif
#if defined(CONFIG_SMP) && CONFIG_SMP
#define RT_USING_SMP
#endif
#define RT_THREAD_PRIORITY_64
#define RT_THREAD_PRIORITY_MAX 64
#ifndef CONFIG_SYSTICK_HZ
#define RT_TICK_PER_SECOND 100
#else
#define RT_TICK_PER_SECOND CONFIG_SYSTICK_HZ
#endif
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE (4096 + STATIC_CSK_CPU_STACK_EXTRAL)
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE (4096 + STATIC_CSK_CPU_STACK_EXTRAL)
#define RT_MAIN_THREAD_STACK_SIZE (4096 + STATIC_CSK_CPU_STACK_EXTRAL)
/* kservice optimization */

#define RT_DEBUG
#define RT_DEBUG_COLOR
#define RT_DEBUG_SCHEDULER  0
#define RT_DEBUG_THREAD 0
#define RT_DEBUG_TIMER  0
#define RT_DEBUG_IRQ    0
#define RT_DEBUG_INIT   0

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_THREAD_SEM
/* Memory Management */

#define RT_PAGE_MAX_ORDER 11
#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP
#define RT_USING_HEAP_ISR

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart1"
#define RT_VER_NUM 0x50001
#define RT_USING_HW_ATOMIC
/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_MSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE (4096 + CSK_CPU_STACK_EXTRAL)
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_UNAMED_PIPE_NUMBER 64
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE (4096 + CSK_CPU_STACK_EXTRAL)
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23

/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* Utilities */

#define PKG_USING_EZXML
#define PKG_USING_EZXML_LATEST_VERSION

/* RT-Thread online packages */
#define FASTLZ_SAMPLE_COMPRESSION_LEVEL 1
/* IoT - internet of things */

#define PKG_USING_KAWAII_MQTT
#define KAWAII_MQTT_LOG_IS_SALOF
#define SALOF_USING_LOG
#define SALOF_USING_SALOF
#define SALOF_BUFF_SIZE 512
#define SALOF_FIFO_SIZE 2048
#define SALOF_TASK_STACK_SIZE (2048 + CSK_CPU_STACK_EXTRAL)
#define SALOF_TASK_TICK 50
#define SALOF_LOG_LEVEL 4
#define KAWAII_MQTT_LOG_LEVEL 4
#define KAWAII_MQTT_VERSION 4
#define KAWAII_MQTT_KEEP_ALIVE_INTERVAL 100
#define KAWAII_MQTT_THREAD_STACK_SIZE (4096 + CSK_CPU_STACK_EXTRAL)
#define KAWAII_MQTT_THREAD_PRIO 5
#define KAWAII_MQTT_THREAD_TICK 50
#define KAWAII_MQTT_MAX_PACKET_ID 0xFFFE
#define KAWAII_MQTT_TOPIC_LEN_MAX 64
#define KAWAII_MQTT_ACK_HANDLER_NUM_MAX 64
#define KAWAII_MQTT_DEFAULT_BUF_SIZE 1024
#define KAWAII_MQTT_DEFAULT_CMD_TIMEOUT 4000
#define KAWAII_MQTT_MAX_CMD_TIMEOUT 20000
#define KAWAII_MQTT_MIN_CMD_TIMEOUT 1000
#define KAWAII_MQTT_RECONNECT_DEFAULT_DURATION 1000

#endif /*CONFIG_KERNEL_RTTHREAD*/

#endif