/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_CLS_H__
#define __STREAM_CLS_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avutil/straight_fifo.h"
#include "stream/stream_ioctl.h"

__BEGIN_DECLS__

#define STREAM_PROTOCOLS_MAX     (5)
#define STREAM_BUF_SIZE_MIN      (2*1024)
#define STREAM_BUF_SIZE_MAX      (STREAM_BUF_SIZE_MIN * 2)
#define STREAM_CACHE_SIZE        (20 * STREAM_BUF_SIZE_MAX)

#define SRCV_TIMEOUT_DEFAULT     (6*1000) ///< ms
#define SCACHE_SIZE_DEFAULT      (20 * STREAM_BUF_SIZE_MAX)
#define SCACHE_THRESHOLD_DEFAULT (80)

typedef struct stream_cls stream_cls_t;

/**
 * @brief  get decrypt callback
 * @param  [in]
 * @param  [in] ilen
 * @param  [in] out
 * @param  [in] olen
 * @return 0/-1
 */
typedef int (*get_decrypt_cb_t)(const void *in, size_t ilen, void *out, size_t *olen);

struct stream_ops {
    const char                *name;
    uint8_t                   type;
    uint8_t                   enable_cache;
    const char*               protocols[STREAM_PROTOCOLS_MAX];

    int      (*open)          (stream_cls_t *o, int mode);
    int      (*read)          (stream_cls_t *o, uint8_t *buf, size_t count);
    int      (*write)         (stream_cls_t *o, const uint8_t *buf, size_t count);
    int      (*seek)          (stream_cls_t *o, int32_t pos);
    int      (*control)       (stream_cls_t *o, int cmd, void *arg, size_t *arg_size);
    int      (*close)         (stream_cls_t *o);
};

#define CACHE_STATUS_STOPED   (0)
#define CACHE_STATUS_RUNNING  (1)

struct stream_cls {
    int32_t                   buf_pos;
    int32_t                   buf_len;

    int32_t                   pos;
    int32_t                   size;                    ///< length of the stream

    char                      *url;
    uint8_t                   eof;
    uint8_t                   quit;
    uint8_t                   seekable;
    irq_av_t                  irq;
    get_decrypt_cb_t          get_dec_cb;              ///< used for get decrypt info

    uint32_t                  rcv_timeout;             ///< ms. 0 use default & AOS_WAIT_FOREVER means wait forever
    aos_event_t               cache_quit;
    uint32_t                  cache_size;              ///< size of the web cache. 0 use default
    uint32_t                  cache_start_threshold;   ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint8_t                   cache_start_upto;        ///<
    uint8_t                   cache_status;
    int32_t                   cache_pos;               ///< cache position. used when cache enable

    struct {
        uint32_t  to_4000ms;
        uint32_t  to_2000ms;
        uint32_t  to_1000ms;
        uint32_t  to_500ms;
        uint32_t  to_200ms;
        uint32_t  to_100ms;
        uint32_t  to_50ms;
        uint32_t  to_20ms;
        uint32_t  to_other;
        uint32_t  cache_full;
        uint32_t  upto_cnt;
        uint32_t  rsize;
    }                         stat;

    void                      *priv;
    const struct stream_ops   *ops;
    aos_mutex_t               lock;
    sfifo_t                   *fifo;
    uint8_t                   buf[STREAM_BUF_SIZE_MAX];
};

__END_DECLS__

#endif /* __STREAM_CLS_H__ */

