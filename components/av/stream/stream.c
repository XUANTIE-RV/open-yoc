/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>
#include <aos/debug.h>

#include "avutil/misc.h"
#include "avutil/byte_rw.h"
#include "stream/stream.h"

#define TAG                    "stream"

#define CACHE_TASK_QUIT_EVT  (0x01)

static struct {
#define STREAM_OPS_MAX (8)
    int                     cnt;
    const struct stream_ops *ops[STREAM_OPS_MAX];
} g_streamers;

#if 0
static const struct stream_ops* _get_stream_ops_by_type(uint8_t type)
{
    int i;

    for (i = 0; i < g_streamers.cnt; i++) {
        if (g_streamers.ops[i]->type == type) {
            return g_streamers.ops[i];
        }
    }

    LOGE(TAG, "get stream ops err, type = %d\n", type);

    return NULL;
}
#endif

static const struct stream_ops* _get_stream_ops_by_url(const char *url)
{
    int i, j;

    for (i = 0; i < g_streamers.cnt; i++) {
        const char *const*protocols = g_streamers.ops[i]->protocols;

        for (j = 0; protocols[j]; j++) {
            int plen = strlen(protocols[j]);

            if (strncasecmp(url, protocols[j], plen) == 0) {
                /* 3 is '://'  */
                if (strlen(url) > (plen + 3)) {
                    if (strncasecmp(url + plen, "://", 3) == 0) {
                        return g_streamers.ops[i];
                    }
                } else {
                    goto err;
                }
            }
        }
    }

err:
    LOGE(TAG, "get stream ops err, url = %s\n", url);

    return NULL;
}

/**
 * @brief  regist stream ops
 * @param  [in] ops
 * @return 0/-1
 */
int stream_ops_register(const struct stream_ops *ops)
{
    int i;

    if (ops && (g_streamers.cnt < STREAM_OPS_MAX)) {
        for (i = 0; i < g_streamers.cnt; i++) {
            if (strcmp(ops->name, g_streamers.ops[i]->name) == 0) {
                /* replicate  */
                break;
            }
        }

        if (i == g_streamers.cnt) {
            g_streamers.ops[g_streamers.cnt] = ops;
            g_streamers.cnt++;
        }
        return 0;
    }

    LOGE(TAG, "stream ops regist fail\n");
    return -1;
}

static int _stream_read_internal(stream_cls_t *o, uint8_t *buf, size_t count)
{
    int ret = -1;

    if (o->eof) {
        return 0;
    }

    ret = o->ops->read ? o->ops->read(o, buf, count) : -1;
    if (ret < 0) {
        LOGE(TAG, "read failed\n");
    } else if (ret == 0) {
        if (o->buf_pos) {
            LOGD(TAG, "read ret is 0, may be read eof. '%s'\n", o->url);
        }
    }

    return ret;
}

static int _cache_stream_fill_buf(stream_cls_t *o)
{
    int ret = 0;
    char *pos;
    uint8_t weof;
    int rlen, remain, dlen, count = 0, first = 1;
    sfifo_t *fifo = o->fifo;

    //LOGI(TAG, "======>>> pos = %10d, cache_pos = %10d, diff = %d", o->pos, o->cache_pos, o->cache_pos - o->pos);
    while (count < STREAM_BUF_SIZE_MAX) {
        rlen = sfifo_get_rpos(fifo, &pos, (o->rcv_timeout == AOS_WAIT_FOREVER) ? 2 * 1000 : o->rcv_timeout);
        if (o->irq.handler && o->irq.handler(o->irq.arg)) {
            LOGI(TAG, "interrupt");
            break;
        }
        dlen = sfifo_get_len(fifo);
        sfifo_get_eof(fifo, NULL, &weof);
        if (rlen <= 0) {
            if (weof) {
                LOGI(TAG, "write eof. url = %s", o->url);
                break;
            }

            if (o->rcv_timeout == AOS_WAIT_FOREVER) {
                continue;
            } else {
                LOGE(TAG, "cache stream break: %s, timeout = %u, eof = %d, quit = %d, cache status = %d,"
                     "rlen = %d", o->url, o->rcv_timeout, o->eof, o->quit, o->cache_status, rlen);
                ret = rlen < 0 ? -1 : ret;
                break;
            }
        }

        if (!weof && o->cache_percent) {
            if (!o->cache_per_upto) {
                if (dlen < (o->cache_size * o->cache_percent / 100.0)) {
                    if (first) {
                        first = 0;
                        o->stat.upto_cnt++;
                        LOGD(TAG, "upto cache percent, pos = %10d, cache_pos = %10d, diff = %d", o->pos, o->cache_pos, o->cache_pos - o->pos);
                    }
                    aos_msleep(100);
                    continue;
                } else {
                    o->cache_per_upto = 1;
                }
            } else {
                if (dlen < STREAM_BUF_SIZE_MAX) {
                    o->cache_per_upto = 0;
                    if (first) {
                        first = 0;
                        o->stat.upto_cnt++;
                        LOGD(TAG, "upto cache percent, pos = %10d, cache_pos = %10d, diff = %d", o->pos, o->cache_pos, o->cache_pos - o->pos);
                    }
                    aos_msleep(100);
                    continue;
                }
            }
        }

        remain = STREAM_BUF_SIZE_MAX - count;
        rlen   = rlen > remain ? remain : rlen;
        memcpy(o->buf + count, pos, rlen);
        count += rlen;
        sfifo_set_rpos(fifo, rlen);
    }

    if (count > 0) {
        o->buf_pos  = 0;
        o->buf_len  = count;
        o->pos     += o->buf_len;
        ret         = count;
    }

    return ret;
}

static int _stream_fill_buf(stream_cls_t *o)
{
    int ret = -1;

    if (o->ops->enable_cache)
        return _cache_stream_fill_buf(o);

    ret = _stream_read_internal(o, o->buf, STREAM_BUF_SIZE_MAX);
    if (ret <= 0) {
        return ret;
    }
    o->buf_pos = 0;
    o->buf_len = ret;

    while (o->buf_len < STREAM_BUF_SIZE_MAX) {
        ret = _stream_read_internal(o, o->buf + o->buf_len, STREAM_BUF_SIZE_MAX - o->buf_len);
        if (ret > 0) {
            //LOGD(TAG, "==============>read ret = %d \n", ret);
            o->buf_len += ret;
        } else {
            break;
        }
    }
    o->pos += o->buf_len;

    return (o->buf_len > 0) ? o->buf_len : ret;
}

static void _scache_task(void *arg)
{
    char *pos;
    int rlen, wlen;
    uint8_t reof = 0;
    stream_cls_t *o = (stream_cls_t*)arg;
    sfifo_t *fifo   = o->fifo;

    while (CACHE_STATUS_STOPED != o->cache_status) {
        wlen = sfifo_get_wpos(fifo, &pos, AOS_WAIT_FOREVER);
        sfifo_get_eof(fifo, &reof, NULL);
        if (!((wlen > 0) && (CACHE_STATUS_STOPED != o->cache_status) && (!reof))) {
            LOGD(TAG, "cache task break. wlen = %d, status = %d, reof = %d\n", wlen, o->cache_status, reof);
            break;
        }

        wlen = wlen >= STREAM_BUF_SIZE_MAX ? STREAM_BUF_SIZE_MAX : wlen;
        rlen = o->ops->read ? o->ops->read(o, (uint8_t*)pos, wlen) : -1;
        if (rlen <= 0) {
            if ((rlen == -1) && (errno == EAGAIN)) {
                aos_msleep(100);
                continue;
            }

            /* patch for lwip, can't read too quickly */
            if ((rlen == 0) && (errno == EISCONN)) {
                aos_msleep(100);
                continue;
            }

            LOGD(TAG, "read rlen = %d, need read = %d, errno = %d, may be read eof. '%s'", rlen, wlen, errno, o->url);
            goto quit;
        }

        o->stat.rsize += rlen;
        o->cache_pos  += rlen;
        sfifo_set_wpos(fifo, rlen);
    }

quit:
    LOGD(TAG, "scache task quit");
    sfifo_set_eof(fifo, 0, 1);
    o->cache_status = CACHE_STATUS_STOPED;
    aos_event_set(&o->cache_quit, CACHE_TASK_QUIT_EVT, AOS_EVENT_OR);
    return;
}

/**
 * @brief  open a stream
 * @param  [in] sth
 * @return NULL on error
 */
stream_cls_t* stream_open(const sth_t *sth)
{
    int ret;
    char *name      = NULL;
    stream_cls_t *o = NULL;
    sfifo_t *fifo   = NULL;
    const struct stream_ops *ops;

    CHECK_PARAM(sth && sth->url && sth->cache_percent <= 100, NULL);
    name = strdup(sth->url);
    CHECK_RET_TAG_WITH_GOTO(name, err);
    ops = _get_stream_ops_by_url(name);
    CHECK_RET_TAG_WITH_GOTO(ops, err);
    o = aos_zalloc(sizeof(stream_cls_t));
    CHECK_RET_TAG_WITH_GOTO(o, err);

    o->url           = name;
    o->ops           = ops;
    o->seekable      = ops->seek ? 1 : 0;
    o->irq           = sth->irq;
    o->get_dec_cb    = sth->get_dec_cb;
    o->cache_size    = sth->cache_size ? sth->cache_size : SCACHE_SIZE_DEFAULT;
    o->cache_percent = sth->cache_percent ? sth->cache_percent : SCACHE_PERCENT_DEFAULT;
    o->rcv_timeout   = sth->rcv_timeout ? sth->rcv_timeout : SRCV_TIMEOUT_DEFAULT;

    if (ops->enable_cache) {
        fifo = sfifo_create(o->cache_size);
        CHECK_RET_TAG_WITH_GOTO(fifo, err);
    }

    o->fifo = fifo;
    ret = ops->open ? ops->open(o, sth->mode) : 0;
    if (ret < 0) {
        LOGE(TAG, "open failed, url = %s\n", sth->url);
        goto err;
    }

    aos_mutex_new(&o->lock);
    if (ops->enable_cache) {
        aos_event_new(&o->cache_quit, 0);
        o->cache_status = CACHE_STATUS_RUNNING;
        aos_task_new("scache", _scache_task, (void *)o, 2*1024);
    }

    return o;
err:
    if (fifo)
        sfifo_destroy(fifo);
    aos_free(name);
    aos_free(o);
    return NULL;
}

/**
 * @brief  read from a stream
 * @param  [in] o
 * @param  [in] buf
 * @param  [in] count
 * @return -1 when err happens
 */
int stream_read(stream_cls_t *o, uint8_t *buf, size_t count)
{
    int ret;
    int len = count;
    unsigned long delta = 0;
    struct timeval t1 = {0}, t2 = {0};

    CHECK_PARAM(o, -1);
    gettimeofday(&t1, NULL);
    while (len > 0) {
        int rc;

        rc = o->buf_len - o->buf_pos;
        if (rc == 0) {
            ret = _stream_fill_buf(o);
            if (ret < 0) {
                o->eof = 1;
                /* err happens */
                goto quit;
            } else if (ret == 0) {
                o->eof = 1;
                ret = count - len;
                goto quit;
            }
            rc = o->buf_len - o->buf_pos;
        }

        rc = (rc > len) ? len : rc;
        memcpy(buf, &o->buf[o->buf_pos], rc);
        o->buf_pos += rc;
        buf        += rc;
        len        -= rc;
    }
    ret = count;

quit:
    gettimeofday(&t2, NULL);
    delta = (t2.tv_sec  - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
    if (delta >= 4000) {
        o->stat.to_4000ms++;
    } else if (delta >= 2000) {
        o->stat.to_2000ms++;
    } else if (delta >= 1000) {
        o->stat.to_1000ms++;
    } else if (delta >= 500) {
        o->stat.to_500ms++;
    } else if (delta >= 200) {
        o->stat.to_200ms++;
    } else if (delta >= 100) {
        o->stat.to_100ms++;
    } else if (delta >= 50) {
        o->stat.to_50ms++;
    } else if (delta >= 20) {
        o->stat.to_20ms++;
    } else {
        o->stat.to_other++;
    }
    return ret;
}

/**
 * @brief  write data to a stream
 * @param  [in] o
 * @param  [in] buf
 * @param  [in] count
 * @return -1 when err happens
 */
int stream_write(stream_cls_t *o, const uint8_t *buf, size_t count)
{
    int ret = -1;

    CHECK_PARAM(o, -1);

    aos_mutex_lock(&o->lock, AOS_WAIT_FOREVER);
    ret = o->ops->write ? o->ops->write(o, buf, count) : -1;
    if (ret < 0) {
        LOGE(TAG, "write failed\n");
    }
    aos_mutex_unlock(&o->lock);

    return ret;
}

static int _inner_stream_seek(stream_cls_t *o, int32_t pos)
{
    int rc;
    unsigned int flag;

    if (o->ops->enable_cache) {
        sfifo_set_eof(o->fifo, 1, 1);
        if (o->cache_status == CACHE_STATUS_RUNNING) {
            o->cache_status = CACHE_STATUS_STOPED;
            aos_event_get(&o->cache_quit, CACHE_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        }
        /* clear the fifo, reuse for seeking */
        sfifo_reset(o->fifo);
    }

    rc = o->seekable ? o->ops->seek(o, pos) : -1;
    if (rc == 0) {
        o->buf_len = 0;
        o->buf_pos = 0;
        o->pos     = pos;
        if (o->ops->enable_cache) {
            o->cache_status = CACHE_STATUS_RUNNING;
            aos_event_set(&o->cache_quit, 0, AOS_EVENT_AND);
            o->stat.rsize = pos;
            o->cache_pos  = pos;
            aos_task_new("scache", _scache_task, (void *)o, 2*1024);
        }
    }

    return rc;
}

/**
 * @brief  seek to the position
 * @param  [in] o
 * @param  [in] offset
 * @param  [in] whence : SEEK_SET/SEEK_CUR/SEEK_END
 * @return 0/-1
 */
int stream_seek(stream_cls_t *o, int32_t offset, int whence)
{
    int rc = -1;
    int32_t pos = -1;

    CHECK_PARAM(o, -1);
    aos_mutex_lock(&o->lock, AOS_WAIT_FOREVER);
    switch (whence) {
    case SEEK_SET:
        pos = offset;
        break;
    case SEEK_CUR:
        pos = (o->pos - (o->buf_len - o->buf_pos)) + offset;
        break;
    case SEEK_END:
        pos = (o->size <= 0) ? pos : o->size - offset;
        break;
    default:
        rc = -1;
        goto quit;
    }

    if (pos <= o->pos) {
        //BW
        int32_t offset = o->buf_len - (o->pos - pos);
        if (offset >= 0) {
            o->buf_pos = offset;
            rc = 0;
        } else {
            rc = _inner_stream_seek(o, pos);
        }
    } else {
        //FW
        if (o->ops->enable_cache) {
            int newpos;
            //FIXME: read from cache for FW. this cost may be less than seek
            if (pos - o->pos <= o->cache_size) {
                newpos = pos / STREAM_BUF_SIZE_MAX + 1;
                newpos = newpos * STREAM_BUF_SIZE_MAX;
                while (o->pos < newpos) {
                    rc = _stream_fill_buf(o);
                    if (rc <= 0)
                        break;
                }

                /* attention: o->pos may be small than newpos normally. */
                if (o->pos >= pos) {
                    pos = o->buf_len - (o->pos - pos);
                    if ((pos >= 0) && (pos <= o->buf_len)) {
                        o->buf_pos = pos;
                        rc = 0;
                    }
                }
            } else {
                rc = _inner_stream_seek(o, pos);
            }
        } else {
            rc = _inner_stream_seek(o, pos);
        }
    }

quit:
    aos_mutex_unlock(&o->lock);
    return rc;
}

/**
* @brief  skip offset of the stream
* @param  [in] o
* @param  [in] offset
* @return 0/-1
*/
int stream_skip(stream_cls_t *o, int32_t offset)
{
    return stream_seek(o, offset, SEEK_CUR);
}

/**
 * @brief  close a stream
 * @param  [in] o
 * @return 0/-1
 */
int stream_close(stream_cls_t *o)
{
    int ret = -1;
    unsigned int flag;

    CHECK_PARAM(o, -1);
    aos_mutex_lock(&o->lock, AOS_WAIT_FOREVER);
    if (o->ops->enable_cache) {
        sfifo_set_eof(o->fifo, 1, 1);
        if (o->cache_status == CACHE_STATUS_RUNNING) {
            o->cache_status = CACHE_STATUS_STOPED;
            aos_event_get(&o->cache_quit, CACHE_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        }
    }

    if (o->ops->close)
        ret = o->ops->close(o);
    LOGI(TAG, "stream stat: to_4000ms = %u, to_2000ms = %u, to_1000ms = %u, to_500ms = %u, to_200ms = %u, "
         "to_100ms = %u, to_50ms = %u, to_20ms = %u, to_other = %u, "
         "cache_full = %u, upto_cnt = %u, rsize = %d, size = %d, url = %s\n",
         o->stat.to_4000ms,  o->stat.to_2000ms, o->stat.to_1000ms,o->stat.to_500ms, o->stat.to_200ms,
         o->stat.to_100ms, o->stat.to_50ms, o->stat.to_20ms, o->stat.to_other,
         o->stat.cache_full, o->stat.upto_cnt, o->stat.rsize, o->size, o->url);
    aos_mutex_unlock(&o->lock);

    aos_mutex_free(&o->lock);
    if (o->ops->enable_cache) {
        sfifo_destroy(o->fifo);
        aos_event_free(&o->cache_quit);
    }
    aos_free(o->url);
    aos_free(o);

    return ret;
}

/**
 * @brief  control a stream
 * @param  [in] o
 * @param  [in] cmd : command
 * @param  [in] arg
 * @param  [in/out] arg_size
 * @return
 */
int stream_control(stream_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    int ret = -1;

    CHECK_PARAM(o, -1);
    aos_mutex_lock(&o->lock, AOS_WAIT_FOREVER);
    ret = o->ops->control ? o->ops->control(o, cmd, arg, arg_size) : -1;
    if (ret < 0) {
        //LOGE(TAG, "control failed, cmd = %d", cmd);
    }
    aos_mutex_unlock(&o->lock);

    return ret;
}

/**
 * @brief  stream support seek whether
 * @param  [in] o
 * @return 0/1
 */
int stream_is_seekable(stream_cls_t *o)
{
    CHECK_PARAM(o, -1);
    return o->seekable;
}

/**
 * @brief  stream is eof whether
 * @param  [in] o
 * @return 0/1
 */
int stream_is_eof(stream_cls_t *o)
{
    CHECK_PARAM(o, -1);
    return o->eof;
}

/**
 * @brief  get the stream size
 * @param  [in] o
 * @return -1 on err
 */
int stream_get_size(stream_cls_t *o)
{
    CHECK_PARAM(o, -1);
    return o->size;
}

/**
 * @brief  get the url
 * @param  [in] o
 * @return
 */
const char* stream_get_url(stream_cls_t *o)
{
    CHECK_PARAM(o, NULL);
    return o->url;
}

int stream_r8(stream_cls_t *o)
{
    int ret;
    uint8_t buf[1];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return buf[0];
    }

    return -1;
}

uint16_t stream_r16be(stream_cls_t *o)
{
    int ret;
    uint8_t buf[2];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r16be((const uint8_t*)buf);
    }

    return -1;
}

uint32_t stream_r24be(stream_cls_t *o)
{
    int ret;
    uint8_t buf[3];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r24be((const uint8_t*)buf);
    }

    return -1;
}

uint32_t stream_r32be(stream_cls_t *o)
{
    int ret;
    uint8_t buf[4];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r32be((const uint8_t*)buf);
    }

    return -1;
}

uint64_t stream_r64be(stream_cls_t *o)
{
    int ret;
    uint8_t buf[8];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r64be((const uint8_t*)buf);
    }

    return -1;
}

uint16_t stream_r16le(stream_cls_t *o)
{
    int ret;
    uint8_t buf[2];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r16le((const uint8_t*)buf);
    }

    return -1;
}

uint32_t stream_r24le(stream_cls_t *o)
{
    int ret;
    uint8_t buf[3];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r24le((const uint8_t*)buf);
    }

    return -1;
}

uint32_t stream_r32le(stream_cls_t *o)
{
    int ret;
    uint8_t buf[4];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r32le((const uint8_t*)buf);
    }

    return -1;
}

uint64_t stream_r64le(stream_cls_t *o)
{
    int ret;
    uint8_t buf[8];

    CHECK_PARAM(o, -1);
    ret = stream_read(o, buf, sizeof(buf));
    if (ret == sizeof(buf)) {
        return byte_r64le((const uint8_t*)buf);
    }

    return -1;
}

/**
 * @brief  get the stream cur pos
 * @param  [in] o
 * @return -1 on err
 */
int stream_tell(stream_cls_t *o)
{
    CHECK_PARAM(o, -1);
    return o->pos - (o->buf_len - o->buf_pos);
}




