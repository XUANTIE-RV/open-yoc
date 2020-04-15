/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/named_straightfifo.h"
#include <aos/list.h>

struct named_sfifo {
    char               *name;
    int                ref;
    sfifo_t            *sfifo;
    aos_mutex_t        lock;
    dlist_t            node;
};

static struct {
    int                init;
    dlist_t            head;
    aos_mutex_t        lock;
} g_fifo_list;

#define lock() aos_mutex_lock(&fifo->lock, AOS_WAIT_FOREVER);
#define unlock() aos_mutex_unlock(&fifo->lock);

#define TAG                    "nsfifo"

/**
 * @brief  open or create a named fifo
 * @param  [in] name
 * @param  [in] mode : O_CREAT/O_RDONLY
 * @param  [in] ...  : if mode == O_CREAT, params size is needed
 * @param              size : size of the straight fifo
 * @return NULL on err
 */
nsfifo_t* nsfifo_open(const char *name, int mode, ...)
{
    va_list ap;
    size_t size;
    nsfifo_t *fifo = NULL;
    sfifo_t *sfifo = NULL;
    dlist_t *node  = &g_fifo_list.head;

    CHECK_PARAM(name && strlen(name), NULL);
    if (!g_fifo_list.init) {
        dlist_init(&g_fifo_list.head);
        aos_mutex_new(&g_fifo_list.lock);
        g_fifo_list.init = 1;
    }

    aos_mutex_lock(&g_fifo_list.lock, AOS_WAIT_FOREVER);
    if (!(mode & O_CREAT)) {
        /* read only */
        for (node = node->next; node != &g_fifo_list.head; node = node->next) {
            fifo = list_entry(node, nsfifo_t, node);
            if (!strcmp(fifo->name, name)) {
                lock();
                fifo->ref++;
                unlock();
                aos_mutex_unlock(&g_fifo_list.lock);

                return fifo;
            }
        }

        fifo = NULL;
        goto err;
    }

    va_start(ap, mode);
    size = va_arg(ap, size_t);
    va_end(ap);

    if (!size)
        goto err;

    sfifo = sfifo_create(size);
    CHECK_RET_TAG_WITH_GOTO(sfifo, err);

    fifo =(nsfifo_t*)aos_zalloc(sizeof(nsfifo_t));
    CHECK_RET_TAG_WITH_GOTO(fifo, err);

    fifo->name = strdup(name);
    CHECK_RET_TAG_WITH_GOTO(fifo->name, err);

    fifo->ref   = 1;
    fifo->sfifo = sfifo;
    aos_mutex_new(&fifo->lock);

    dlist_add_tail(&fifo->node, &g_fifo_list.head);
    aos_mutex_unlock(&g_fifo_list.lock);

    return fifo;
err:
    aos_mutex_unlock(&g_fifo_list.lock);
    sfifo_destroy(sfifo);
    aos_free(fifo);

    return NULL;
}

/**
 * @brief  get straight read pos
 * @param  [in] fifo
 * @param  [out] pos
 * @param  [in] timeout : ms
 * @return straight read size
 */
int nsfifo_get_rpos(nsfifo_t* fifo, char **pos, uint32_t timeout)
{
    int ret;

    CHECK_PARAM(fifo && pos, -1);
    ret = sfifo_get_rpos(fifo->sfifo, pos, timeout);

    return ret;
}

/**
 * @brief  set the read pos after read
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int nsfifo_set_rpos(nsfifo_t* fifo, size_t count)
{
    int ret;

    CHECK_PARAM(fifo && count, -1);
    ret = sfifo_set_rpos(fifo->sfifo, count);

    return ret;
}

/**
 * @brief  get straight write pos
 * @param  [in] fifo
 * @param  [out] pos
 * @param  [in] timeout : ms
 * @return straight write size
 */
int nsfifo_get_wpos(nsfifo_t* fifo, char **pos, uint32_t timeout)
{
    int ret;

    CHECK_PARAM(fifo && pos, -1);
    ret = sfifo_get_wpos(fifo->sfifo, pos, timeout);

    return ret;
}

/**
 * @brief  set the write pos after write
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int nsfifo_set_wpos(nsfifo_t* fifo, size_t count)
{
    int ret;

    CHECK_PARAM(fifo && count, -1);
    ret = sfifo_set_wpos(fifo->sfifo, count);

    return ret;
}

/**
 * @brief  set eof to the fifo before destroy, may be block if don't call this fun
 * @param  [in] fifo
 * @param  [in] reof : nsfifo_get_rpos will return immediately when set
 * @param  [in] weof : nsfifo_get_wpos will return immediately when set
 * @return 0/-1
 */
int nsfifo_set_eof(nsfifo_t* fifo, uint8_t reof, uint8_t weof)
{
    int ret;

    CHECK_PARAM(fifo, -1);
    ret = sfifo_set_eof(fifo->sfifo, reof, weof);

    return ret;
}

/**
 * @brief  get eof flag from the fifo
 * @param  [in] fifo
 * @param  [out] reof : read eof flag
 * @param  [out] weof : write eof flag
 * @return 0/-1
 */
int nsfifo_get_eof(nsfifo_t* fifo, uint8_t *reof, uint8_t *weof)
{
    int ret;

    CHECK_PARAM(fifo, -1);
    ret = sfifo_get_eof(fifo->sfifo, reof, weof);

    return ret;
}

/**
 * @brief  reset the named fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int nsfifo_reset(nsfifo_t *fifo)
{
    int ret;

    CHECK_PARAM(fifo, -1);
    ret = sfifo_reset(fifo->sfifo);

    return ret;
}

/**
 * @brief  get valid data len of the named fifo
 * @param  [in] fifo
 * @return -1 on error
 */
int nsfifo_get_len(nsfifo_t *fifo)
{
    int ret;

    CHECK_PARAM(fifo, -1);
    ret = sfifo_get_len(fifo->sfifo);

    return ret;
}

/**
 * @brief  close the fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int nsfifo_close(nsfifo_t *fifo)
{
    int ret = 0;

    CHECK_PARAM(fifo, -1);
    aos_mutex_lock(&g_fifo_list.lock, AOS_WAIT_FOREVER);
    lock();
    fifo->ref--;
    if (0 == fifo->ref) {
        dlist_t *item, *tmp;
        sfifo_destroy(fifo->sfifo);

        dlist_for_each_safe(item, tmp, &g_fifo_list.head) {
            if (&fifo->node == item) {
                dlist_del(item);
                break;
            }
        }
        aos_mutex_unlock(&g_fifo_list.lock);

        unlock();
        aos_mutex_free(&fifo->lock);
        aos_free(fifo->name);
        aos_free(fifo);
        return ret;
    }
    unlock();
    aos_mutex_unlock(&g_fifo_list.lock);

    return ret;
}


