/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "ipc.h"
#include <csi_core.h>
#include "icore/icore_msg.h"
#include "aoicore.h"
#include "icore/aoicore_internal.h"

#define TAG                    "aoicore_ap"

#define AOICORE_PCM_SIZE (8*1024)  // 1152 samples per frame max for mp3

#ifdef CONFIG_CHIP_PANGU
#else
#define AOICORE_SRAME_SIZE      (0x3800) // 14k. 16k = 14k + 2k(reserve)
#define AO_SRAME_ADDR_START     (0x9c000) //0x60000 + 256k - 32k + 16k
#define AOICORE_MSG_POOL_SIZE   (AOICORE_SRAME_SIZE - AOICORE_PCM_SIZE)
#define AOICORE_MSG_POOL_START  (AO_SRAME_ADDR_START + AOICORE_PCM_SIZE)
static struct {
    char   *start;
    size_t offset;
} g_msg_pool;
#endif

typedef struct aoicore aoicore_t;

struct aoicore {
    void            *ao;      // ao_cls_t
    sf_t            sf;
    void            *priv;    // priv only for ap/cpu0
};

struct aoicore_ap_priv {
    uint8_t         *pool;
    uint8_t         *pool_orip;
    size_t          pool_size;

    uint8_t         *pcm_data;
    uint8_t         *pcm_orip;
    size_t          pcm_size;

    icore_msg_t     *msg_open;
    icore_msg_t     *msg_start;
    icore_msg_t     *msg_write;
    icore_msg_t     *msg_drain;
    icore_msg_t     *msg_stop;
    icore_msg_t     *msg_close;
};

static struct {
    ipc_t           *ipc;
    int             init;
    icore_msg_t     *msg_volset;
    aos_mutex_t     lock;
} g_aoicore;
#define aoicore_lock()   (aos_mutex_lock(&g_aoicore.lock, AOS_WAIT_FOREVER))
#define aoicore_unlock() (aos_mutex_unlock(&g_aoicore.lock))

static int _ipc_cmd_send(icore_msg_t *data, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id  = AOICORE_IPC_SERIVCE_ID;
    msg.command     = IPC_CMD_AOICORE;
    msg.req_data        = data;
    msg.req_len         = ICORE_MSG_SIZE + data->size;
    msg.resp_data       = data;
    msg.resp_len        = ICORE_MSG_SIZE + data->size;
    msg.flag       |= sync;

    /* patch for vol set */
    aoicore_lock();
    ipc_message_send(g_aoicore.ipc, &msg, AOS_WAIT_FOREVER);
    aoicore_unlock();

    return data->ret.code;
}

#ifdef CONFIG_CHIP_PANGU
/**
 * @brief  init the icore audio output
 * @return 0/-1
 */
int aoicore_init()
{
    if (!g_aoicore.init) {
        aos_mutex_new(&g_aoicore.lock);
        g_aoicore.ipc = ipc_get(AOICORE_CP_IDX);
        CHECK_RET_TAG_WITH_RET(NULL != g_aoicore.ipc, -1);
        ipc_add_service(g_aoicore.ipc, AOICORE_CP_IDX, NULL, NULL);
        g_aoicore.msg_volset = icore_msg_new(ICORE_CMD_VOL_SET, sizeof(volicore_set_t));
        g_aoicore.init = 1;
    }

    return g_aoicore.init ? 0 : -1;
}

/**
 * @brief  open/create one audio remote-ipc output
 * @param  [in] ash
 * @return NULL on error
 */
aoicore_t* aoicore_open(const aoih_t *ash)
{
    int rc;
    unsigned int rate;
    char *pcm_data, *pcm_orip    = NULL;
    aoicore_t *hdl               = NULL;
    struct aoicore_ap_priv *priv = NULL;
    icore_msg_t *msg_open        = NULL;
    icore_msg_t *msg_start       = NULL;
    icore_msg_t *msg_write       = NULL;
    icore_msg_t *msg_drain       = NULL;
    icore_msg_t *msg_stop        = NULL;
    icore_msg_t *msg_close       = NULL;
    aoicore_open_t *inp;

    if (!(ash && ash->sf && (ash->id == ICORE_AO_ID_ALSA))) {
        return NULL;
    }
    rate = sf_get_rate(ash->sf);
    switch (rate) {
    case 8000 :
    case 16000:
    case 32000:

    case 12000:
    case 24000:
    case 48000:

    case 11025:
    case 22050:
    case 44100:
        break;
    default:
        LOGE(TAG, "rate is not support, rate = %d\n", rate);
        return NULL;
    }

    hdl = aos_zalloc(sizeof(aoicore_t));
    CHECK_RET_TAG_WITH_RET(NULL != hdl, NULL);

    pcm_orip   = aos_zalloc(ICORE_ALIGN_BUFZ(AOICORE_PCM_SIZE));
    CHECK_RET_TAG_WITH_GOTO(NULL != pcm_orip, err);
    pcm_data = ICORE_ALIGN(pcm_orip);

    msg_open = icore_msg_new(ICORE_CMD_AO_OPEN, sizeof(aoicore_open_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_open, err);
    msg_start = icore_msg_new(ICORE_CMD_AO_START, sizeof(aoicore_start_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_start, err);
    msg_write = icore_msg_new(ICORE_CMD_AO_WRITE, sizeof(aoicore_write_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_write, err);
    msg_drain = icore_msg_new(ICORE_CMD_AO_DRAIN, sizeof(aoicore_drain_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_drain, err);
    msg_stop = icore_msg_new(ICORE_CMD_AO_STOP, sizeof(aoicore_stop_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_stop, err);
    msg_close = icore_msg_new(ICORE_CMD_AO_CLOSE, sizeof(aoicore_close_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_close, err);

    priv = aos_zalloc(sizeof(struct aoicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(NULL != priv, err);

    inp = icore_get_msg(msg_open, aoicore_open_t);
    memcpy(&inp->ash, ash, sizeof(aoih_t));

    rc = _ipc_cmd_send(msg_open, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    priv->pcm_data  = (uint8_t*)pcm_data;
    priv->pcm_orip  = (uint8_t*)pcm_orip;
    priv->pcm_size  = AOICORE_PCM_SIZE;
    priv->msg_open  = msg_open;
    priv->msg_start = msg_start;
    priv->msg_write = msg_write;
    priv->msg_drain = msg_drain;
    priv->msg_stop  = msg_stop;
    priv->msg_close = msg_close;

    hdl->ao   = inp->ao;
    hdl->priv = priv;

    return hdl;
err:
    aos_free(hdl);
    aos_free(priv);
    aos_free(pcm_orip);
    return NULL;
}

#else
/**
 * @brief  new one icore message by msdid, can't free
 * @param  [in] id
 * @param  [in] size : size of the msg actualy
 * @return NULL on error
 */
static icore_msg_t *_icore_msg_new(msgid_t id, size_t size)
{
    char *orip;
    size_t len;
    icore_msg_t *msg = NULL;

    len = ICORE_ALIGN_BUFZ(size + ICORE_MSG_SIZE);
    if (g_msg_pool.offset + len <= AOICORE_MSG_POOL_SIZE) {
        orip               = g_msg_pool.start + g_msg_pool.offset;
        memset(orip, 0, len);

        msg                = ICORE_ALIGN(orip);
        msg->orip          = orip;
        msg->id            = id;
        msg->size          = size;
        msg->ret.code      = -1;
        g_msg_pool.offset += len;
    }

    return msg;
}

/**
 * @brief  init the icore audio output
 * @return 0/-1
 */
int aoicore_init()
{
    if (!g_aoicore.init) {
        aos_mutex_new(&g_aoicore.lock);
        g_msg_pool.start = (char*)AOICORE_MSG_POOL_START;
        g_aoicore.ipc = ipc_get(AOICORE_CP_IDX);
        ipc_add_service(g_aoicore.ipc, AOICORE_CP_IDX, NULL, NULL);
        // ipc_service_shm_set(g_aoicore.ipc, AOICORE_CP_IDX, aos_malloc_check(10*1024), 10240, SHM_CACHE);
        CHECK_RET_TAG_WITH_RET(NULL != g_aoicore.ipc, -1);
        g_aoicore.msg_volset = _icore_msg_new(ICORE_CMD_VOL_SET, sizeof(volicore_set_t));
        g_aoicore.init = 1;
    }

    return g_aoicore.init ? 0 : -1;
}

/**
 * @brief  open/create one audio remote-ipc output
 * @param  [in] ash
 * @return NULL on error
 */
aoicore_t* aoicore_open(const aoih_t *ash)
{
    int rc;
    unsigned int rate;
    char *pcm_data, *pcm_orip    = NULL;
    aoicore_t *hdl               = NULL;
    struct aoicore_ap_priv *priv = NULL;
    icore_msg_t *msg_open        = NULL;
    icore_msg_t *msg_start       = NULL;
    icore_msg_t *msg_write       = NULL;
    icore_msg_t *msg_drain       = NULL;
    icore_msg_t *msg_stop        = NULL;
    icore_msg_t *msg_close       = NULL;
    aoicore_open_t *inp;

    if (!(ash && ash->sf && (ash->id == ICORE_AO_ID_ALSA))) {
        return NULL;
    }
    rate = sf_get_rate(ash->sf);
    switch (rate) {
    case 8000 :
    case 16000:
    case 32000:

    case 12000:
    case 24000:
    case 48000:

    case 11025:
    case 22050:
    case 44100:
        break;
    default:
        LOGE(TAG, "rate is not support, rate = %d\n", rate);
        return NULL;
    }

    //TODO: reserve for volset
    g_msg_pool.offset = 1024;
    hdl = aos_zalloc(sizeof(aoicore_t));
    CHECK_RET_TAG_WITH_RET(NULL != hdl, NULL);

    pcm_orip   = (char*)AO_SRAME_ADDR_START;
    pcm_data   = pcm_orip;
    msg_open = _icore_msg_new(ICORE_CMD_AO_OPEN, sizeof(aoicore_open_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_open, err);
    msg_start = _icore_msg_new(ICORE_CMD_AO_START, sizeof(aoicore_start_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_start, err);
    msg_write = _icore_msg_new(ICORE_CMD_AO_WRITE, sizeof(aoicore_write_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_write, err);
    msg_drain = _icore_msg_new(ICORE_CMD_AO_DRAIN, sizeof(aoicore_drain_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_drain, err);
    msg_stop = _icore_msg_new(ICORE_CMD_AO_STOP, sizeof(aoicore_stop_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_stop, err);
    msg_close = _icore_msg_new(ICORE_CMD_AO_CLOSE, sizeof(aoicore_close_t));
    CHECK_RET_TAG_WITH_GOTO(NULL != msg_close, err);

    priv = aos_zalloc(sizeof(struct aoicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(NULL != priv, err);

    inp = icore_get_msg(msg_open, aoicore_open_t);
    memcpy(&inp->ash, ash, sizeof(aoih_t));

    rc = _ipc_cmd_send(msg_open, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    priv->pcm_data  = (uint8_t*)pcm_data;
    priv->pcm_orip  = (uint8_t*)pcm_orip;
    priv->pcm_size  = AOICORE_PCM_SIZE;
    priv->msg_open  = msg_open;
    priv->msg_start = msg_start;
    priv->msg_write = msg_write;
    priv->msg_drain = msg_drain;
    priv->msg_stop  = msg_stop;
    priv->msg_close = msg_close;

    hdl->ao   = inp->ao;
    hdl->priv = priv;

    return hdl;
err:
    aos_free(hdl);
    aos_free(priv);
    return NULL;
}
#endif

/**
* @brief  start the audio output
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_start(aoicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct aoicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_start;
    if (msg) {
        aoicore_start_t *inp = icore_get_msg(msg, aoicore_start_t);
        inp->ao = hdl->ao;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    return rc;
}

/**
* @brief  stop the audio output
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_stop(aoicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct aoicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_stop;
    if (msg) {
        aoicore_stop_t *inp = icore_get_msg(msg, aoicore_stop_t);
        inp->ao = hdl->ao;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    return rc;
}

/**
* @brief  drain out the tail pcm
* @param  [in] hdl
* @return 0/-1
*/
int aoicore_drain(aoicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct aoicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_drain;
    if (msg) {
        aoicore_drain_t *inp = icore_get_msg(msg, aoicore_drain_t);
        inp->ao = hdl->ao;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    return rc;
}

/**
 * @brief  write data to audio out
 * @param  [in] hdl
 * @param  [in] buf
 * @param  [in] count
 * @return -1 when err
 */
int aoicore_write(aoicore_t *hdl, const uint8_t *buf, size_t count)
{
    int rc = -1;
    int remain, len, offset = 0;
    icore_msg_t *msg;
    aoicore_write_t *inp;
    struct aoicore_ap_priv *priv;

    if (!(hdl && buf && count)) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_write;
    inp  = icore_get_msg(msg, aoicore_write_t);

    while (offset < count) {
        memset(inp, 0, sizeof(aoicore_write_t));
        remain = count - offset;
        len = (remain > AOICORE_PCM_SIZE) ? AOICORE_PCM_SIZE : remain;
        inp->ao    = hdl->ao;
        inp->count = len;
        inp->buf   = priv->pcm_data;
        memcpy(inp->buf, buf + offset, len);
        csi_dcache_clean_invalid_range((uint32_t*)inp->buf, inp->count);
        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
        if (rc <= 0) {
            break;
        }
        offset += rc;
    }

    return rc >= 0 ? offset: -1;
}

#ifdef CONFIG_CHIP_PANGU
/**
 * @brief  close/destroy audio remote-ipc output
 * @param  [in] hdl
 * @return 0/-1
 */
int aoicore_close(aoicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct aoicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_close;
    if (msg) {
        aoicore_close_t *inp = icore_get_msg(msg, aoicore_close_t);
        inp->ao = hdl->ao;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    icore_msg_free(priv->msg_open);
    icore_msg_free(priv->msg_start);
    icore_msg_free(priv->msg_write);
    icore_msg_free(priv->msg_drain);
    icore_msg_free(priv->msg_stop);
    icore_msg_free(priv->msg_close);
    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}
#else
/**
 * @brief  close/destroy audio remote-ipc output
 * @param  [in] hdl
 * @return 0/-1
 */
int aoicore_close(aoicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct aoicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = priv->msg_close;
    if (msg) {
        aoicore_close_t *inp = icore_get_msg(msg, aoicore_close_t);
        inp->ao = hdl->ao;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}
#endif

/**
 * @brief  set the volume(0-100)
 * @param  [in] vol
 * @return 0/-1
 */
int volicore_set(size_t vol)
{
    int rc = -1;
    icore_msg_t *msg;

    if (vol > 100) {
        return rc;
    }

    aoicore_init();
    msg  = g_aoicore.msg_volset;
    if (msg) {
        volicore_set_t *inp = icore_get_msg(msg, volicore_set_t );
        inp->vol = vol;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    return rc;
}


