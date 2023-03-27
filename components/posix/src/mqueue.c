/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <aos/kernel.h>
#include "mqueue.h"

struct mq_struct {
    int                   refs;  // if 0 means the mq is unused
    int                   is_nonblock;
    aos_queue_t           mq;
    struct mq_attr        attr;
    char                  *name;
};

static struct {
#define MQ_TOTAL_CNT_INIT (8)
    int                   total_cnt;
    int                   used_cnt;
    struct mq_struct      *mqs;
    aos_mutex_t           lock;
} g_mq_cb;

#define mq_is_init()          (g_mq_cb.total_cnt)
#define mq_lock()             (aos_mutex_lock(&g_mq_cb.lock, AOS_WAIT_FOREVER))
#define mq_unlock()           (aos_mutex_unlock(&g_mq_cb.lock))

static int _mq_find(const char *name, mqd_t *mqdes)
{
    int i, find = 0;

    if (g_mq_cb.used_cnt) {
        for (i = 0; i < g_mq_cb.total_cnt; i++) {
            if (g_mq_cb.mqs[i].refs && strcmp(g_mq_cb.mqs[i].name, name) == 0) {
                find   = 1;
                *mqdes = i;
                break;
            }
        }
    }

    return find;
}

static int _mqdes_is_valid(mqd_t mqdes)
{
    if (mqdes >= 0 && mqdes < g_mq_cb.total_cnt) {
        return g_mq_cb.mqs[mqdes].refs > 0 ? 1 : 0;
    }

    return 0;
}

mqd_t mq_open(const char *name, int oflag, ...)
{
    mode_t mode;
    mqd_t mqdes = -1;
    int i, find = 0;
    struct mq_attr *attr = NULL;
    size_t old_size, new_size;
    size_t msg_size = DEFAULT_MAX_MSG_SIZE;
    size_t max_msg = DEFAULT_MQUEUE_SIZE / DEFAULT_MAX_MSG_SIZE;

    if (!(name && strlen(name) > 1 && name[0] == '/')) {
        return -1;
    }

    if (!mq_is_init()) {
        aos_mutex_new(&g_mq_cb.lock);
        g_mq_cb.mqs       = aos_calloc(MQ_TOTAL_CNT_INIT, sizeof(struct mq_struct) * MQ_TOTAL_CNT_INIT);
        g_mq_cb.total_cnt = MQ_TOTAL_CNT_INIT;
    }

    mq_lock();
    find = _mq_find(name, &mqdes);
    if (oflag & O_CREAT) {
        if (find) {
            goto err;
        }

        va_list ap;
        va_start(ap, oflag);
        mode = va_arg(ap, mode_t);
        attr = va_arg(ap, struct mq_attr *);
        va_end(ap);

        (void)mode;
        if (attr) {
            if (!(attr->mq_maxmsg > 0 && attr->mq_msgsize > 0)) {
                goto err;
            }
            max_msg  = attr->mq_maxmsg;
            msg_size = attr->mq_msgsize;
        }

        if (g_mq_cb.used_cnt >= g_mq_cb.total_cnt) {
            old_size = sizeof(struct mq_struct) * g_mq_cb.total_cnt;
            g_mq_cb.total_cnt += MQ_TOTAL_CNT_INIT;
            new_size = sizeof(struct mq_struct) * g_mq_cb.total_cnt;
            g_mq_cb.mqs = aos_realloc((void*)g_mq_cb.mqs, new_size);
            memset((char*)g_mq_cb.mqs + old_size, 0, new_size - old_size);
        }

        for (i = 0; i < g_mq_cb.total_cnt; i++) {
            if (g_mq_cb.mqs[i].refs == 0) {
                mqdes = i;
                g_mq_cb.used_cnt++;
                g_mq_cb.mqs[i].refs++;
                g_mq_cb.mqs[i].is_nonblock     = oflag & O_NONBLOCK ? 1 : 0;
                g_mq_cb.mqs[i].name            = strdup(name);
                g_mq_cb.mqs[i].attr.mq_maxmsg  = max_msg;
                g_mq_cb.mqs[i].attr.mq_msgsize = msg_size;
                aos_queue_create(&g_mq_cb.mqs[i].mq, max_msg * msg_size, msg_size, 0);
                break;
            }
        }
    } else {
        if (find) {
            g_mq_cb.mqs[(int)mqdes].refs++;
        }
    }
    mq_unlock();

    return mqdes;
err:
    mq_unlock();
    return -1;
}

static void _mq_close_no_lock(mqd_t mqdes)
{
    g_mq_cb.mqs[mqdes].refs--;
    if (g_mq_cb.mqs[mqdes].refs == 0) {
        g_mq_cb.used_cnt--;
        free(g_mq_cb.mqs[mqdes].name);
        aos_queue_free(&g_mq_cb.mqs[mqdes].mq);
    }
}

int mq_close(mqd_t mqdes)
{
    int rc = -1;

    mq_lock();
    if (_mqdes_is_valid(mqdes)) {
        _mq_close_no_lock(mqdes);
        rc = 0;
    }
    mq_unlock();

    return rc;
}

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio)
{
    int ret;
    size_t msg_size;
    unsigned int timeout_ms;

    if (!(mqdes != -1 && msg_ptr))
        return -1;

    mq_lock();
    if (!_mqdes_is_valid(mqdes)) {
        goto err;
    }
    if (msg_len < g_mq_cb.mqs[mqdes].attr.mq_msgsize) {
        goto err;
    }
    timeout_ms = g_mq_cb.mqs[mqdes].is_nonblock ? AOS_NO_WAIT : AOS_WAIT_FOREVER;
    mq_unlock();

    if (msg_prio)
        *msg_prio = 0;
    ret = aos_queue_recv(&g_mq_cb.mqs[mqdes].mq, timeout_ms, msg_ptr, &msg_size);

    return ret < 0 ? -1 : msg_size;
err:
    mq_unlock();
    return -1;
}

int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)
{
    int ret;
    unsigned int timeout_ms;

    if (!(mqdes != -1 && msg_ptr && msg_len))
        return -1;

    mq_lock();
    if (!_mqdes_is_valid(mqdes)) {
        goto err;
    }
    if (msg_len > g_mq_cb.mqs[mqdes].attr.mq_msgsize) {
        goto err;
    }
    timeout_ms = g_mq_cb.mqs[mqdes].is_nonblock ? AOS_NO_WAIT : AOS_WAIT_FOREVER;
    mq_unlock();

retry:
    ret = aos_queue_send(&g_mq_cb.mqs[mqdes].mq, (void *)msg_ptr, msg_len);
    if (ret < 0) {
        /* FIXME: may be queue full */
        if (timeout_ms) {
            timeout_ms = timeout_ms > 20 ? (timeout_ms - 20) : 0;
            aos_msleep(20);
            goto retry;
        }
    }

    return ret < 0 ? -1 : 0;
err:
    mq_unlock();
    return -1;
}

int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat, struct mq_attr *omqstat)
{
    //TODO:
    return 0;
}

int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat)
{
    //TODO:
    return 0;
}

ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                        unsigned *msg_prio, const struct timespec *abs_timeout)
{
    int ret;
    size_t msg_size;
    unsigned int timeout_ms = 0;

    if (!(mqdes != -1 && msg_ptr))
        return -1;

    mq_lock();
    if (!_mqdes_is_valid(mqdes)) {
        goto err;
    }
    if (msg_len < g_mq_cb.mqs[mqdes].attr.mq_msgsize) {
        goto err;
    }
    mq_unlock();

    if (msg_prio)
        *msg_prio = 0;
    if (abs_timeout)
        timeout_ms = abs_timeout->tv_sec * 1000 + abs_timeout->tv_nsec / 1000;
    ret = aos_queue_recv(&g_mq_cb.mqs[mqdes].mq, timeout_ms, msg_ptr, &msg_size);

    return ret < 0 ? -1 : msg_size;
err:
    mq_unlock();
    return -1;
}

int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
                 unsigned msg_prio, const struct timespec *abs_timeout)
{
    int ret;
    unsigned int timeout_ms = 0;

    if (!(mqdes != -1 && msg_ptr && msg_len))
        return -1;

    mq_lock();
    if (!_mqdes_is_valid(mqdes)) {
        goto err;
    }
    if (msg_len > g_mq_cb.mqs[mqdes].attr.mq_msgsize) {
        goto err;
    }
    mq_unlock();

    if (abs_timeout)
        timeout_ms = abs_timeout->tv_sec * 1000 + abs_timeout->tv_nsec / 1000;
retry:
    ret = aos_queue_send(&g_mq_cb.mqs[mqdes].mq, (void *)msg_ptr, msg_len);
    if (ret < 0) {
        /* FIXME: may be queue full */
        if (timeout_ms) {
            timeout_ms = timeout_ms > 20 ? (timeout_ms - 20) : 0;
            aos_msleep(20);
            goto retry;
        }
    }

    return ret < 0 ? -1 : 0;
err:
    mq_unlock();
    return -1;
}

int mq_unlink(const char *name)
{
    int find = 0;
    mqd_t mqdes = -1;

    if (!(name && strlen(name) > 1 && name[0] == '/')) {
        return -1;
    }

    mq_lock();
    find = _mq_find(name, &mqdes);
    if (find) {
        _mq_close_no_lock(mqdes);
    }
    mq_unlock();

    return 0;
}

