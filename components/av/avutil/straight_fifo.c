/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/straight_fifo.h"

struct straight_fifo {
    uint8_t        *buf;
    size_t         len;
    size_t         size;
    uint32_t       ridx;
    uint32_t       widx;
    uint8_t        reof;
    uint8_t        weof;

    aos_event_t    evt;
    aos_mutex_t    lock;
};

#define TAG "sfifo"
#define SFIFO_MAGIC_NUM    (4)
#define SFIFO_WRITE_EVENT  (0x01)
#define SFIFO_READ_EVENT   (0x02)

#define lock() aos_mutex_lock(&fifo->lock, AOS_WAIT_FOREVER);
#define unlock() aos_mutex_unlock(&fifo->lock);

#define is_full(fifo) (fifo->len == fifo->size)
#define is_empty(fifo) (fifo->len == 0)

/**
 * @brief  create a straight r/w fifo
 * @param  [in] size
 * @return NULL on err
 */
sfifo_t* sfifo_create(size_t size)
{
    char *buf = NULL;
    sfifo_t *fifo = NULL;

    CHECK_PARAM(size > 0, NULL);
    buf  = aos_zalloc(size + SFIFO_MAGIC_NUM);
    fifo = aos_zalloc(sizeof(sfifo_t));
    if (!(fifo && buf)) {
        goto err;
    }

    memset(buf + size, 0xa, SFIFO_MAGIC_NUM);
    aos_mutex_new(&fifo->lock);
    aos_event_new(&fifo->evt, 0);
    fifo->buf  = (uint8_t*)buf;
    fifo->len  = 0;
    fifo->size = size;

    return fifo;
err:
    aos_free(buf);
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
int sfifo_get_rpos(sfifo_t* fifo, char **pos, uint32_t timeout)
{
    int rc = 0;
    unsigned int flag;

    CHECK_PARAM(fifo && pos, -1);
    lock();
    if (is_empty(fifo) && (!fifo->weof)) {
        unlock();
        rc = aos_event_get(&fifo->evt, SFIFO_READ_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout);
        if (rc < 0) {
            /* maybe timeout */
            LOGE(TAG, "get rpos err. may be timeout. timeout = %u, rc = %d", timeout, rc);
            return -1;
        }
        lock();
    }

    /* may be reof set, is_empty again */
    if (!is_empty(fifo)) {
        aos_event_set(&fifo->evt, ~SFIFO_READ_EVENT, AOS_EVENT_AND);
        if (fifo->widx > fifo->ridx) {
            rc   = fifo->widx - fifo->ridx;
            *pos = (char*)fifo->buf + fifo->ridx;
        } else {
            rc   = fifo->size - fifo->ridx;
            *pos = (char*)fifo->buf + fifo->ridx;
        }
    } else {
        LOGD(TAG, "get rpos. weof = %d, reof = %d size = %u, widx = %d, ridx = %d, len = %d",
             fifo->weof, fifo->reof, fifo->size, fifo->widx, fifo->ridx, fifo->len);
    }
    unlock();

    return rc;
}

/**
 * @brief  set the read pos after read
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int sfifo_set_rpos(sfifo_t* fifo, size_t count)
{
    int rc = -1;

    CHECK_PARAM(fifo && count, -1);
    lock();
    if (fifo->ridx + count <= fifo->size) {
        fifo->len  -= count;
        fifo->ridx += count;
        fifo->ridx %= fifo->size;
        aos_event_set(&fifo->evt, SFIFO_WRITE_EVENT, AOS_EVENT_OR);
        rc = 0;
    } else {
        LOGE(TAG, "set rpos err. count = %u, size = %u, widx = %d, ridx = %d, len = %d",
             count, fifo->size, fifo->widx, fifo->ridx, fifo->len);
    }
    unlock();

    return rc;
}

/**
 * @brief  get straight write pos
 * @param  [in] fifo
 * @param  [out] pos
 * @param  [in] timeout : ms
 * @return straight write size
 */
int sfifo_get_wpos(sfifo_t* fifo, char **pos, uint32_t timeout)
{
    int rc = 0;
    unsigned int flag;

    CHECK_PARAM(fifo && pos, -1);
    lock();
    if (is_full(fifo) && (!fifo->reof)) {
        unlock();
        rc = aos_event_get(&fifo->evt, SFIFO_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout);
        if (rc < 0) {
            /* maybe timeout */
            LOGE(TAG, "get wpos err. may be timeout. timeout = %u, rc = %d", timeout, rc);
            return -1;
        }
        lock();
    }

    /* may be weof set, is_full again */
    if (!is_full(fifo)) {
        aos_event_set(&fifo->evt, ~SFIFO_WRITE_EVENT, AOS_EVENT_AND);
        if (fifo->widx >= fifo->ridx) {
            rc   = fifo->size - fifo->widx;
            *pos = (char*)fifo->buf + fifo->widx;
        } else {
            rc   = fifo->ridx - fifo->widx;
            *pos = (char*)fifo->buf + fifo->widx;
        }
    } else {
        LOGD(TAG, "get wpos. weof = %d, reof = %d size = %u, widx = %d, ridx = %d, len = %d",
             fifo->weof, fifo->reof, fifo->size, fifo->widx, fifo->ridx, fifo->len);
    }
    unlock();

    return rc;
}

/**
 * @brief  set the write pos after write
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int sfifo_set_wpos(sfifo_t* fifo, size_t count)
{
    int rc = -1;

    CHECK_PARAM(fifo && count, -1);
    lock();
    if (fifo->widx + count <= fifo->size) {
        fifo->len  += count;
        fifo->widx += count;
        fifo->widx %= fifo->size;
        aos_event_set(&fifo->evt, SFIFO_READ_EVENT, AOS_EVENT_OR);
        rc = 0;
    } else {
        LOGE(TAG, "set wpos err. count = %u, size = %u, widx = %d, ridx = %d, len = %d",
             count, fifo->size, fifo->widx, fifo->ridx, fifo->len);
    }
    unlock();

    return rc;
}

/**
 * @brief  set eof to the fifo before destroy, may be block if don't call this fun
 * @param  [in] fifo
 * @param  [in] reof : sfifo_get_rpos will return immediately when set
 * @param  [in] weof : sfifo_get_wpos will return immediately when set
 * @return 0/-1
 */
int sfifo_set_eof(sfifo_t* fifo, uint8_t reof, uint8_t weof)
{
    int rc = 0;

    CHECK_PARAM(fifo, -1);
    lock();
    if (reof) {
        fifo->reof = reof;
        aos_event_set(&fifo->evt, SFIFO_WRITE_EVENT, AOS_EVENT_OR);
    }

    if (weof) {
        fifo->weof = weof;
        aos_event_set(&fifo->evt, SFIFO_READ_EVENT, AOS_EVENT_OR);
    }
    unlock();

    return rc;
}

/**
 * @brief  get eof flag from the fifo
 * @param  [in] fifo
 * @param  [out] reof : read eof flag
 * @param  [out] weof : write eof flag
 * @return 0/-1
 */
int sfifo_get_eof(sfifo_t* fifo, uint8_t *reof, uint8_t *weof)
{
    int rc = 0;

    CHECK_PARAM(fifo, -1);
    lock();
    if (reof)
        *reof = fifo->reof;

    if (weof)
        *weof = fifo->weof;
    unlock();

    return rc;
}

/**
 * @brief  reset the fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int sfifo_reset(sfifo_t *fifo)
{
    int rc = 0;

    CHECK_PARAM(fifo, -1);
    lock();
    fifo->len  = 0;
    fifo->ridx = 0;
    fifo->widx = 0;
    fifo->reof = 0;
    fifo->weof = 0;
    aos_event_set(&fifo->evt, 0, AOS_EVENT_AND);
    unlock();

    return rc;
}

/**
 * @brief  get valid data len of the fifo
 * @param  [in] fifo
 * @return -1 on error
 */
int sfifo_get_len(sfifo_t *fifo)
{
    int rc = 0;

    CHECK_PARAM(fifo, -1);
    lock();
    rc = fifo->len;
    unlock();

    return rc;
}

/**
 * @brief  destroy the fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int sfifo_destroy(sfifo_t *fifo)
{
    int rc = 0;

    CHECK_PARAM(fifo, -1);
    aos_mutex_free(&fifo->lock);
    aos_event_free(&fifo->evt);
    aos_free(fifo->buf);
    aos_free(fifo);

    return rc;
}


