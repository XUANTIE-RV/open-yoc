#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "atomic.h"
#include "buf.h"

#include "errno.h"
#include <aos/kernel.h>
#include <aos/aos.h>
#include <k_api.h>
#include <port.h>
#include "bl_port.h"
#include "slist.h"

#if defined(HOST_BL702)
#include "bl702.h"
#endif

extern int bl_rand();
//CPSR_ALLOC();

//static sys_dlist_t q_db;


void k_queue_init(struct k_queue *queue,int size)
{
    void *msg_start;
    int stat;
    uint8_t blk_size = sizeof(void *) + 4;

    msg_start = (void*)aos_malloc(size * blk_size);
    assert(msg_start);

    queue->hdl= (_queue_t *)aos_malloc(sizeof(aos_queue_t));
    assert(queue->hdl);

    stat = aos_queue_new((aos_queue_t *)queue->hdl, msg_start, size * blk_size, sizeof(void *));
    assert(stat == 0);

    sys_dlist_init(&queue->poll_events);
}

void k_queue_insert(struct k_queue *queue, void *prev, void *data)
{
    int ret;

    ret = aos_queue_send((aos_queue_t *)queue->hdl,&data, sizeof(void *));
    if(ret){
        printf("k_queue_insert:%d %p\r\n",ret,(aos_queue_t *)queue->hdl);
    }
    assert(ret == 0);    
}

void k_queue_append(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_insert_from_isr(struct k_queue *queue, void *prev, void *data)
{
#if 0
    BaseType_t xHigherPriorityTaskWoken;
    
    xQueueSendFromISR(queue->hdl, &data, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
#endif
    k_queue_insert(queue,prev,data);
}

void k_queue_append_from_isr(struct k_queue *queue, void *data)
{
    k_queue_insert_from_isr(queue, NULL, data);
}

void k_queue_free(struct k_queue *queue)
{
    if(queue != NULL && queue->hdl){
        aos_free(queue->hdl);
        queue->hdl = NULL;
    }
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_append_list(struct k_queue *queue, void *head, void *tail)
{
    struct net_buf *buf_tail = (struct net_buf *)head;

    for (buf_tail = (struct net_buf *)head; buf_tail; buf_tail = buf_tail->frags) {
        k_queue_append(queue, buf_tail);
    }
}

void *k_queue_get(struct k_queue *queue, s32_t timeout)
{
    void *msg = NULL;
    unsigned int t = timeout;
    unsigned int len;

    if (timeout == K_FOREVER) {
        t = 0xffffffffu;
    } else if (timeout == K_NO_WAIT) {
        t = 0x0;
    }

    aos_queue_recv((aos_queue_t *)queue->hdl, t, &msg, &len); 
    return msg;
}

int k_queue_is_empty(struct k_queue *queue)
{
    return k_queue_get_cnt(queue) ? 0 : 1;
	//kbuf_queue_t *k_queue = queue->hdl;

	//return k_queue->cur_num? 0: 1;
}

int k_queue_get_cnt(struct k_queue *queue)
{
    return aos_queue_get_count(queue->hdl);

    //kbuf_queue_t *k_queue = queue->hdl;
	//return k_queue->cur_num;
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    int ret;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = aos_sem_new((aos_sem_t *)&sem->sem, initial_count);
    sys_dlist_init(&sem->poll_events);
    return ret;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    unsigned int t = timeout;

    if (timeout == K_FOREVER) {
        t =  0xffffffffu;
    } else if (timeout == K_NO_WAIT) {
        t = 0x0;
    }
    return aos_sem_wait((aos_sem_t *)&sem->sem, t);
}

int k_sem_give(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    aos_sem_signal((aos_sem_t *)&sem->sem);
    return 0;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    aos_sem_free((aos_sem_t *)&sem->sem);
    return 0;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
    sem_count_t count;
    ksem_t *k_sem = (ksem_t *)&(sem->sem.hdl);

    krhino_sem_count_get(k_sem, &count);

    return (int)count;
}

void k_mutex_init(struct k_mutex *mutex)
{
    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return ;
    }

    aos_mutex_new((aos_mutex_t *)&mutex->mutex);
    sys_dlist_init(&mutex->poll_events);
    return ;
}

int64_t k_uptime_get()
{
    return aos_now_ms();
}

u32_t k_uptime_get_32(void)
{
    return (u32_t)aos_now_ms();
}

struct bthread_t{
    aos_task_t  *btask;
    sys_snode_t	node;
};

//static sys_dlist_t t_db;

typedef void (*task_entry_t)(void *args);
int k_thread_create(struct k_thread *new_thread, const char *name,
                    size_t stack_size, k_thread_entry_t entry,
                    int prio)
{

    int ret;

    ret = aos_task_new_ext((aos_task_t *)&(new_thread->task), name, (task_entry_t)entry, NULL, stack_size, prio);
    if (ret) {
        printf("create ble task fail\n");
    }

    return ret;
}

void k_thread_delete(struct k_thread *new_thread)
{
    if(NULL == new_thread || NULL == new_thread->task.hdl)
    {
        BT_ERR("task is NULL\n");
        return;
    }
    
    krhino_task_dyn_del((ktask_t *)new_thread->task.hdl);
    new_thread->task.hdl = NULL;
}

bool k_is_current_thread(struct k_thread *thread)
{
/*
    eTaskState thread_state = eTaskGetState((void *)(thread->task));
    if(thread_state == eRunning)
        return true;
    else
        return false;
*/
    aos_task_t running = aos_task_self();
    if (thread->task.hdl == running) {
        return true;
    } else {
        return false;
    }
}

int k_yield(void)
{
    return 0;
}

void k_sleep(s32_t dur_ms)
{
    aos_msleep(dur_ms);
}

unsigned int irq_lock(void)
{
    /*CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    return cpsr;*/
    return cpu_intrpt_save();
}

void irq_unlock(unsigned int key)
{
    /*CPSR_ALLOC();
    cpsr = key;
    RHINO_CPU_INTRPT_ENABLE();*/
    cpu_intrpt_restore(key);
    //RHINO_CRITICAL_EXIT_SCHED();
}

int k_is_in_isr(void)
{
#if 0
    #if defined(ARCH_RISCV)
    return (xPortIsInsideInterrupt());
    #else
    /* IRQs + PendSV (14) + SYSTICK (15) are interrupts. */
    return (__get_IPSR() > 13);
    #endif
#endif
    return 0;
}

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,handle %p,args %p", timer, handle, args);
    timer->handler = handle;
    timer->args = args;
    aos_timer_new_ext((aos_timer_t *)&timer->timer, timer->handler, args, 1000, 0, 0);
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p,timeout %u", timer, timeout);
    timer->timeout = timeout;
    timer->start_ms = aos_now_ms();
    aos_timer_stop((aos_timer_t *)&timer->timer);
    aos_timer_change((aos_timer_t *)&timer->timer, timeout);
    aos_timer_start((aos_timer_t *)&timer->timer);
}

void k_timer_stop(k_timer_t *timer)
{
    ASSERT(timer, "timer is NULL");
    BT_DBG("timer %p", timer);
    aos_timer_stop((aos_timer_t *)&timer->timer);
}

void k_timer_delete(k_timer_t *timer)
{
#if 0
    BaseType_t ret;
    (void) ret;
    
    BT_ASSERT(timer != NULL);
    
    ret = xTimerDelete(timer->timer.hdl, 0);
    BT_ASSERT(ret == pdPASS);
#endif
    aos_timer_free((aos_timer_t *)&timer->timer);
}

long long k_now_ms(void)
{
    return aos_now_ms();
}

void k_get_random_byte_array(uint8_t *buf, size_t len)
{
    // bl_rand() return a word, but *buf may not be word-aligned
    for(int i = 0; i < len; i++){
        *(buf + i) = (uint8_t)(bl_rand() & 0xFF);
    }

}

void *k_malloc(size_t size)
{
    return aos_malloc(size);
}

void k_free(void *buf)
{
    return aos_free(buf);
}


