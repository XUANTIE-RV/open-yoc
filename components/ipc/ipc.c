/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <aos/debug.h>
#include <aos/aos.h>
#include <csi_core.h>
#include <ipc_mem.h>
#include <assert.h>

#include "internal.h"

#ifndef CONFIG_IPC_RECV_TASK_STACK_SIZE
#define CONFIG_IPC_RECV_TASK_STACK_SIZE    1024
#endif
#ifndef CONFIG_IPC_SERVICE_TASK_STACK_SIZE
#define CONFIG_IPC_SERVICE_TASK_STACK_SIZE 4096
#endif

#define IPC_WRITE_EVENT CHANNEL_WRITE_EVENT
#define IPC_READ_EVENT CHANNEL_READ_EVENT

#define MIN(a, b) (a)<(b)? (a) : (b)

#define TAG "ipc"
static slist_t ipc_list;

static void ipc_event(int event, void *priv)
{
    ipc_t *ipc = (ipc_t*)priv;
    aos_event_set(&ipc->evt, event, AOS_EVENT_OR);
}

static int ipc_channel_send(ipc_t *ipc, phy_data_t *msg, int timeout_ms)
{
    unsigned int flag;

    aos_event_get(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms);
    aos_mutex_lock(&ipc->ch_mutex, AOS_WAIT_FOREVER);
    msg->seq = ipc->seq ++;
    int ret = channel_put_message(ipc->ch, msg, sizeof(phy_data_t), timeout_ms);
    aos_mutex_unlock(&ipc->ch_mutex);

    //printf("ipc send msg:%d %d %d %d %llx %d\r\n", msg->seq, msg->service_id, msg->flag, msg->command, msg->data, msg->len);

    return ret;
}

static int ipc_channel_recv(ipc_t *ipc, phy_data_t *msg, int timeout_ms)
{
    unsigned int flag;

    aos_event_get(&ipc->evt, IPC_READ_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms);

    aos_mutex_lock(&ipc->ch_mutex, AOS_WAIT_FOREVER);
    int ret = channel_get_message(ipc->ch, msg, sizeof(phy_data_t), timeout_ms);
    aos_mutex_unlock(&ipc->ch_mutex);

    if ((ipc->seq_bake + 1) != msg->seq) {
        if (ipc->seq_bake == msg->seq) {
            LOGW(TAG, "ipc repeated(%s) cur:%u", aos_task_get_name(&ipc->thread), ipc->seq_bake);
            return -1;
        }
        LOGI(TAG, "ipc lost(%s) cur:%d recv_msg:%d %d %d %d %llx %d", 
            aos_task_get_name(&ipc->thread), ipc->seq_bake, msg->seq, msg->service_id, msg->flag, msg->command, msg->data, msg->len);
    } else {
        ipc->seq_bake ++;
    }

    return ret;
}

static int phy_recv(ipc_t *ipc, phy_data_t *msg, int ms)
{
    phy_data_t m;
    int ret;

    while (1) {
        ret = ipc_channel_recv(ipc, &m, ms);

        if (ret < 0) {
            continue;
        }

#ifdef CONFIG_IPC_PROBE_ENABLE
        if (m.flag & MESSAGE_PROBE) {
            continue;
        }
#endif

        // if (m.flag & PHY_ACK) {
        //     aos_sem_signal(&m.sem);
        //     continue;
        // }

        if ((msg->flag & SHM_CACHE) && m.data) {
            #if defined(__riscv)
            csi_dcache_invalid_range((unsigned long *)m.data, SHM_ALIGN_SIZE(m.len, SHM_ALIGN_CACHE));
            #endif
        }

        memcpy(msg, &m, sizeof(phy_data_t));
        msg->data = (int64_t)aos_malloc_check(m.len);
        msg->len  = m.len;

        if (m.data)
            memcpy((void *)msg->data, (void *)m.data, m.len);

        // m.flag |= PHY_ACK;
        // ipc_channel_send(ipc, &m, ms);
        break;
    }

    return ret;
}

static int phy_send(ipc_t *ipc, phy_data_t *msg, int ms)
{
    if (msg->flag & SHM_CACHE) {
        #if defined(__riscv)
        csi_dcache_clean_range((unsigned long *)msg->data, SHM_ALIGN_SIZE(msg->len, SHM_ALIGN_CACHE));
        #endif
    }

    // memcpy(&msg->sem, &ipc->sem, sizeof(aos_sem_t));
    int ret = ipc_channel_send(ipc, msg, ms);
    // aos_sem_wait(&msg->sem, AOS_WAIT_FOREVER);
    return ret;
}

static service_t *find_service(ipc_t *ipc, int service_id)
{
    service_t *ser;

    slist_for_each_entry (&ipc->service_list, ser, service_t, next) {
        if (ser->id == service_id) {
            return ser;
        }
    }

    return NULL;
}

static void shm_reset(shm_t *shm)
{
    shm->widx = 0;
}

static int shm_write(shm_t *shm, void *data, size_t len)
{
    size_t copy = MIN(len, shm->size - shm->widx);

    memcpy((char *)shm->addr + shm->widx, data, copy);
    shm->widx += copy;

    return copy;
}

static int shm_available_read_space(shm_t *shm)
{
    return shm->widx;
}

static int transfer_send(service_t *ser, message_t *msg, int timeout_ms)
{
    ipc_t *ipc = ser->ipc;

    aos_check_return_einval(ser);

    phy_data_t phy_msg;

    aos_mutex_lock(&ipc->tx_mutex, AOS_WAIT_FOREVER);
    shm_t *shm = &ipc->shm[ipc->seq % BLOCK_SIZE];

    memcpy(&phy_msg, msg, sizeof(phy_data_t));
    phy_msg.data = (int64_t)shm->addr;

    if (shm->flag & SHM_CACHE) {
        phy_msg.flag |= SHM_CACHE;
    }

    char *data    = msg->req_data;
    int total_len = msg->req_len;
    dispatch_t dispatch;
    int ret;

    dispatch.total_len = total_len;
    dispatch.resp_len = msg->resp_len;
    memcpy(&dispatch.queue, &msg->queue, sizeof(uint64_t));

    shm_reset(shm);
    shm_write(shm, (uint8_t *)&dispatch, sizeof(dispatch_t));

    while ((total_len > 0) || (shm_available_read_space(shm) > 0)) {
        ret = shm_write(shm, data, total_len);
        phy_msg.data = (int64_t)shm->addr;
        phy_msg.len = shm_available_read_space(shm);
        phy_send(ipc, &phy_msg, timeout_ms);
        shm = &ipc->shm[ipc->seq % BLOCK_SIZE];
        total_len -= ret;
        data += ret;
        shm_reset(shm);
    }

    aos_mutex_unlock(&ipc->tx_mutex);

    return 0;
}

static int phy_wait(service_t *ser, phy_data_t *msg, int ms)
{
    size_t len;
    return (aos_queue_recv(&ser->queue, ms, msg, &len));
}

static int transfer_recv(service_t *ser, message_t *msg, int timeout_ms)
{
    phy_data_t phy_msg;
    ipc_t *ipc = ser->ipc;

    int ret = phy_wait(ser, &phy_msg, timeout_ms);

    aos_mutex_lock(&ipc->rx_mutex, AOS_WAIT_FOREVER);

    dispatch_t *dispatch = (dispatch_t *)(phy_msg.data);
    int total_len        = dispatch->total_len;
    int recv_len         = 0;
    char *recv           = NULL;

    if (total_len > 0) {
        msg->req_data = aos_malloc_check(total_len);
    } else {
        msg->req_data = NULL;
    }
    msg->req_len = total_len;
    memcpy(&msg->queue, &dispatch->queue, sizeof(uint64_t));
    msg->resp_len  = dispatch->resp_len;

    if (msg->req_data != NULL) {
        recv = (char *)msg->req_data;
        memcpy(recv, (char *)phy_msg.data + sizeof(dispatch_t), phy_msg.len - sizeof(dispatch_t));
        recv_len += (phy_msg.len - sizeof(dispatch_t));
        total_len -= recv_len;
        recv += recv_len;
    }

    aos_free((void *)phy_msg.data);

    while(recv && total_len > 0) {
        phy_wait(ser, &phy_msg, timeout_ms);
        memcpy(recv, (void *)phy_msg.data, phy_msg.len);
        recv += phy_msg.len;
        total_len -= phy_msg.len;
        aos_free((void *)phy_msg.data);
    }
    aos_mutex_unlock(&ipc->rx_mutex);

    msg->flag          = phy_msg.flag;
    msg->service_id    = phy_msg.service_id;
    msg->command       = phy_msg.command;
    msg->seq           = phy_msg.seq;
    msg->resp_data = NULL;

    return ret;
}

static void ipc_task_process_entry(void *arg)
{
    ipc_t *ipc = (ipc_t *)arg;
    phy_data_t data;
    service_t *ser;

    while (1) {
        phy_recv(ipc, &data, AOS_WAIT_FOREVER);

        ser = find_service(ipc, data.service_id);
        if (ser) {
            while(aos_queue_send(&ser->queue, &data, sizeof(phy_data_t)) != 0) {
                aos_msleep(10);
            }
        }
    }

    aos_task_exit(0);
}

static void ipc_service_entry(void *priv)
{
    service_t *ser = (service_t *)priv;
    ipc_t     *ipc = ser->ipc;
    message_t msg;
    memset(&msg, 0x00, sizeof(message_t));

    while (1) {
        transfer_recv(ser, &msg, AOS_WAIT_FOREVER);

        if (msg.flag & MESSAGE_ACK) {
            msg.resp_data = msg.req_data;
            msg.resp_len  = msg.req_len;
            int ret = aos_queue_send((aos_queue_t *)&msg.queue, &msg, sizeof(message_t));
            if (ret < 0) {
                LOGE(TAG, "ipc queue send fail(%d)(%p)\n", ret, msg.queue);
            }
            continue;
        }

        if (msg.flag & MESSAGE_SYNC) {
            if (msg.resp_len) {
                msg.resp_data = aos_malloc_check(msg.resp_len);
            }
        }

        if (ser->process)
            ser->process(ipc, &msg, ser->priv);

        if (msg.req_data && !(msg.flag & MESSAGE_SYNC)) {
            aos_free((char *)msg.req_data);
            msg.req_data = NULL;
        }
    }

    aos_task_exit(0);
}

#ifdef CONFIG_IPC_PROBE_ENABLE
static void ipc_probe_entry(void *arg) {
  ipc_t *ipc = (ipc_t *)arg;
  phy_data_t data = { .flag = MESSAGE_PROBE, .seq = 1 };
  unsigned int flag;
  unsigned int timeout_ms = 100;

  // clear initial value
  aos_event_get(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms);

  while (1) {
    channel_put_message(ipc->ch, &data, sizeof(phy_data_t), 0);
    if (0 == aos_event_get(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms)) {
      // recover the writable value
      aos_event_set(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR);
      ipc->seq = 2;
      ipc->probe_status = 0;
      aos_event_set(&ipc->probe_freeze_evt, IPC_PROBE_EVENT, AOS_EVENT_OR);
      break;
    }
  }

  aos_task_exit(0);
}
#endif

int ipc_message_send(ipc_t *ipc, message_t *m, int timeout_ms)
{
    aos_check_return_einval(ipc && m);

    message_t msg_buf[2];

    memset(&msg_buf, 0x00, sizeof(message_t));
    if (m->flag & MESSAGE_SYNC) {
        int ret = aos_queue_new((aos_queue_t *)&m->queue, &msg_buf, sizeof(message_t) * 2, sizeof(message_t));

        if (ret != 0) {
            return -1;
        }
    }

    service_t *ser = find_service(ipc, m->service_id);

    if (ser == NULL) {
        return -1;
    }

#ifdef CONFIG_IPC_PROBE_ENABLE
    if (ipc->probe_status) {
        unsigned int flag;
        long long freeze_t0 = 0;
        if ((timeout_ms != AOS_WAIT_FOREVER) && (timeout_ms != AOS_NO_WAIT)) {
            freeze_t0 = aos_now_ms();
        }
        if (0 != aos_event_get(&ipc->probe_freeze_evt, IPC_PROBE_EVENT,
            AOS_EVENT_OR, &flag, timeout_ms)) {
            return -1;
        }
        if (freeze_t0 > 0) {
            int diff = (int)(aos_now_ms() - freeze_t0);
            if (diff > timeout_ms) {
                return -1;
            }
            timeout_ms -= diff;
        }
    }
#endif

    transfer_send(ser, m, timeout_ms);

    if (m->flag & MESSAGE_SYNC) {
        size_t len;
        message_t msg;

#if defined(CONFIG_IPC_DEBUG) && defined(CONFIG_IPC_DEBUG_MSGSEND_TIMEOUT)
        int ret = aos_queue_recv((aos_queue_t *)&m->queue, CONFIG_IPC_DEBUG_MSGSEND_TIMEOUT, &msg, &len);
        if (ret != 0) {
            LOGE(TAG, "%s queue %d",  __func__, ret);
            abort();
        }
#else
        aos_queue_recv((aos_queue_t *)&m->queue, AOS_WAIT_FOREVER, &msg, &len);
#endif
        aos_queue_free((aos_queue_t *)&m->queue);

        if (msg.resp_len == m->resp_len) {
            memcpy((char *)m->resp_data, (char *)msg.resp_data, msg.resp_len);
            aos_free((char *)msg.resp_data);
        } else {
            aos_assert(0);
        }
    }

    return 0;
}

int ipc_message_ack(ipc_t *ipc, message_t *msg, int timeout_ms)
{
    aos_check_return_einval(ipc && msg);
    msg->flag |= MESSAGE_ACK;

    if (msg->req_data) {
        aos_free((char *)msg->req_data);
    }
    msg->req_data = msg->resp_data;
    msg->req_len  = msg->resp_len;

    service_t *ser = find_service(ipc, msg->service_id);
    if (ser == NULL) {
        return -1;
    }

#ifdef CONFIG_IPC_PROBE_ENABLE
    if (ipc->probe_status) {
        unsigned int flag;
        long long freeze_t0 = 0;
        if ((timeout_ms != AOS_WAIT_FOREVER) && (timeout_ms != AOS_NO_WAIT)) {
            freeze_t0 = aos_now_ms();
        }
        if (0 != aos_event_get(&ipc->probe_freeze_evt, IPC_PROBE_EVENT,
            AOS_EVENT_OR, &flag, timeout_ms)) {
            return -1;
        }
        if (freeze_t0 > 0) {
            int diff = (int)(aos_now_ms() - freeze_t0);
            if (diff > timeout_ms) {
                return -1;
            }
            timeout_ms -= diff;
        }
    }
#endif

    transfer_send(ser, msg, timeout_ms);
    if (msg->resp_data) {
        aos_free((char *)msg->resp_data);
        msg->resp_data = NULL;
        msg->req_data = NULL;
    }

    return 0;
}

int ipc_lpm(int cpu_id, int state)
{
    ipc_t *ipc;

    slist_for_each_entry (&ipc_list, ipc, ipc_t, next) {
        if (ipc->des_cpu_id == cpu_id) {
            break;
        }
    }

    if (ipc == NULL) {
        return -1;
    }

    // if (state == 0) {
    //     ipc->seq = 1;
    //     ipc->seq_bake = 0;
    // }

    channel_mailbox_lpm(ipc->ch, state);

    return 0;
}

ipc_t *ipc_get(int cpu_id)
{
#define IPC_NAME_MAX_LEN 16

    ipc_t *ipc;

    slist_for_each_entry (&ipc_list, ipc, ipc_t, next) {
        if (ipc->des_cpu_id == cpu_id) {
            return ipc;
        }
    }

    ipc  = calloc(1, sizeof(ipc_t) + IPC_NAME_MAX_LEN);
    if (ipc == NULL) {
        return NULL;
    }

    char *name_buf = (char *)((uint32_t)ipc + sizeof(ipc_t));

    ipc->ch = channel_mailbox_get(cpu_id, ipc_event, ipc);

    if (ipc->ch != NULL) {
        slist_add_tail(&ipc->next, &ipc_list);
        ipc->des_cpu_id = cpu_id;

        aos_event_new(&ipc->evt, 0);
        aos_mutex_new(&ipc->ch_mutex);
        aos_mutex_new(&ipc->tx_mutex);
        aos_mutex_new(&ipc->rx_mutex);
        aos_sem_new(&ipc->sem, 0);
        aos_event_set(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR);

        snprintf(name_buf, IPC_NAME_MAX_LEN, "ipc->%d", cpu_id);
        aos_task_new_ext(&ipc->thread, name_buf, ipc_task_process_entry, ipc, CONFIG_IPC_RECV_TASK_STACK_SIZE, 10);

        ipc->seq     = 1;

        drv_ipc_mem_init();
        int shm_size = 0;
        char *shm_start_addr = (char *)drv_ipc_mem_alloc(&shm_size);
        int blk_len = shm_size / BLOCK_SIZE;

        for (int i = 0; i < BLOCK_SIZE; i ++) {
            shm_t *shm = &ipc->shm[i];

            memset(shm, 0x00, sizeof(shm_t));
            shm->addr = shm_start_addr + i * blk_len;
            shm->size = blk_len;
            if (drv_ipc_mem_use_cache() == 1) {
                shm->flag |= SHM_CACHE;
            }
        }

#ifdef CONFIG_IPC_PROBE_ENABLE
        ipc->probe_status = 1;
        aos_event_new(&ipc->probe_freeze_evt, 0);
        aos_task_new_ext(&ipc->probe_thread, "ipc_probe", ipc_probe_entry, ipc, 3072, 32);
#endif
        return ipc;
    }

    free(ipc);

    return NULL;
}

int ipc_add_service(ipc_t *ipc, int service_id, ipc_process_t cb, void *priv)
{
    aos_check_param(ipc);

    service_t *ser = find_service(ipc, service_id);

    if (ser != NULL) {
        assert(NULL);
        return -1;
    }

    ser = aos_zalloc(sizeof(service_t));

    if (ser) {
        ser->id      = service_id;
        ser->process = cb;
        ser->priv    = priv;
        ser->ipc     = ipc;
        slist_add_tail(&ser->next, &ipc->service_list);
        aos_queue_new(&ser->queue, ser->que_buf, SERVER_QUEUE_SIZE, sizeof(phy_data_t));
        snprintf(ser->ser_name, SER_NAME_MAX_LEN, "ser%d->%d", service_id, ipc->des_cpu_id);
        aos_task_new_ext(&ser->task, ser->ser_name, ipc_service_entry, ser, CONFIG_IPC_SERVICE_TASK_STACK_SIZE, 9);

        return 0;
    }

    return -1;
}

