/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "ipc.h"
#include <csi_core.h>
#include "adicore.h"
#include "icore/adicore_internal.h"

#define TAG                     "adicore_ap"

#ifdef CONFIG_CHIP_PANGU
#define ADICORE_ES_SIZE         (16*1024)
#define ADICORE_FRAME_POOL_SIZE (64*1024)

#define adicore_lock()          do {} while(0)
#define adicore_unlock()        do {} while(0)
#else
#define ADICORE_SRAME_USE
#define ADICORE_ES_SIZE         (2*1024)
#define ADICORE_FRAME_POOL_SIZE (5*1024)  // 1152 samples per frame max for mp3

#define ADICORE_SRAME_SIZE      (0x4000) // 16k
#define AD_SRAME_ADDR_START     (0x98000) //0x60000 + 256k - 32k
#define ADICORE_MSG_POOL_SIZE   (ADICORE_SRAME_SIZE - ADICORE_FRAME_POOL_SIZE - ADICORE_ES_SIZE)
#define ADICORE_MSG_POOL_START  (AD_SRAME_ADDR_START + ADICORE_FRAME_POOL_SIZE + ADICORE_ES_SIZE)

#define adicore_lock()          (aos_mutex_lock(&g_adicore.lock, AOS_WAIT_FOREVER))
#define adicore_unlock()        (aos_mutex_unlock(&g_adicore.lock))

static struct {
    char   *start;
    size_t offset;
} g_msg_pool;
#endif

typedef struct adicore adicore_t;

struct adicore {
    void            *ad;      // ad_cls_t
    sf_t            sf;
    void            *priv;    // priv only for ap/cpu0
};

struct adicore_ap_priv {
    uint8_t         *pool;
    uint8_t         *pool_orip;
    size_t          pool_size;

    uint8_t         *es_data;
    uint8_t         *es_orip;
    size_t          es_size;

    icore_msg_t     *msg_dec; // malloc only once for opt
#ifdef ADICORE_SRAME_USE
    icore_msg_t     *msg_open;
    icore_msg_t     *msg_close;
#endif
};

static struct {
    ipc_t           *ipc;
    int             init;
#ifdef ADICORE_SRAME_USE
#define ADICORE_NUM_MAX (2)
    aos_mutex_t     lock;
    int             ad_cnt;
#endif
} g_adicore;

static int _ipc_cmd_send(icore_msg_t *data, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id      = ADICORE_IPC_SERIVCE_ID;
    msg.command         = IPC_CMD_ADICORE;
    msg.req_data        = data;
    msg.req_len         = ICORE_MSG_SIZE + data->size;
    msg.resp_data       = data;
    msg.resp_len        = ICORE_MSG_SIZE + data->size;
    msg.flag           |= sync;

    ipc_message_send(g_adicore.ipc, &msg, AOS_WAIT_FOREVER);

    return data->ret.code;
}

/**
 * @brief  init the icore audio decoder
 * @return 0/-1
 */
int adicore_init()
{
    if (!g_adicore.init) {
#ifdef ADICORE_SRAME_USE
        aos_mutex_new(&g_adicore.lock);
        g_msg_pool.start = (char*)ADICORE_MSG_POOL_START;
#endif
        g_adicore.ipc = ipc_get(ADICORE_CP_IDX);
        ipc_add_service(g_adicore.ipc, ADICORE_IPC_SERIVCE_ID, NULL, NULL);
        CHECK_RET_TAG_WITH_RET(NULL != g_adicore.ipc, -1);
        g_adicore.init = 1;
    }

    return g_adicore.init ? 0 : -1;
}

#ifdef ADICORE_SRAME_USE
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
    if (g_msg_pool.offset + len <= ADICORE_MSG_POOL_SIZE) {
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
 * @brief  open/create one audio remote-ipc decoder
 * @param  [in] ash
 * @return NULL on error
 */
adicore_t* adicore_open(const adih_t *ash)
{
    int rc;
    char *es_data, *es_orip      = NULL;
    char *pool, *pool_orip       = NULL;
    uint8_t *extradata           = NULL;
    uint8_t *extradata_orip      = NULL;
    adicore_t *hdl               = NULL;
    struct adicore_ap_priv *priv = NULL;
    icore_msg_t *msg_open        = NULL;
    icore_msg_t *msg_dec         = NULL;
    icore_msg_t *msg_close       = NULL;
    adicore_open_t *inp;

    if (!(ash && ((ash->id == ICORE_CODEC_ID_MP3) ||
                  (ash->id == ICORE_CODEC_ID_AAC)))) {
        return NULL;
    }

    hdl = aos_zalloc(sizeof(adicore_t));
    CHECK_RET_TAG_WITH_RET(hdl, NULL);

    adicore_lock();
    if (g_adicore.ad_cnt > ADICORE_NUM_MAX) {
        LOGE(TAG, "adicore max = %d, cur = %d", ADICORE_NUM_MAX, g_adicore.ad_cnt);
        goto err;
    }

    g_msg_pool.offset = 0;
    pool_orip = (char*)AD_SRAME_ADDR_START;
    pool      = pool_orip;
    es_orip   = (char*)AD_SRAME_ADDR_START + ADICORE_FRAME_POOL_SIZE;
    es_data   = es_orip;
    msg_open = _icore_msg_new(ICORE_CMD_AD_OPEN, sizeof(adicore_open_t));
    CHECK_RET_TAG_WITH_GOTO(msg_open, err);
    msg_dec = _icore_msg_new(ICORE_CMD_AD_DECODE, sizeof(adicore_decode_t));
    CHECK_RET_TAG_WITH_GOTO(msg_dec, err);
    msg_close = _icore_msg_new(ICORE_CMD_AD_CLOSE, sizeof(adicore_close_t));
    CHECK_RET_TAG_WITH_GOTO(msg_close, err);

    priv = aos_zalloc(sizeof(struct adicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(priv, err);

    inp = icore_get_msg(msg_open, adicore_open_t);
    inp->ash.id = ash->id;
    if (ash->extradata && ash->extradata_size) {
        extradata_orip = aos_zalloc(ICORE_ALIGN_BUFZ(ash->extradata_size));
        CHECK_RET_TAG_WITH_GOTO(NULL != extradata_orip, err);
        extradata = ICORE_ALIGN(extradata_orip);

        memcpy(extradata, ash->extradata, ash->extradata_size);
        inp->ash.extradata      = extradata;
        inp->ash.extradata_size = ash->extradata_size;
        csi_dcache_clean_invalid_range((uint32_t*)inp->ash.extradata, inp->ash.extradata_size);
    }

    rc = _ipc_cmd_send(msg_open, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    priv->es_data   = (uint8_t*)es_data;
    priv->es_orip   = (uint8_t*)es_orip;
    priv->es_size   = ADICORE_ES_SIZE;
    priv->pool      = (uint8_t*)pool;
    priv->pool_orip = (uint8_t*)pool_orip;
    priv->pool_size = ADICORE_FRAME_POOL_SIZE;
    priv->msg_dec   = msg_dec;
    priv->msg_open  = msg_open;
    priv->msg_close = msg_close;

    hdl->ad   = inp->ad;
    hdl->sf   = inp->sf;
    hdl->priv = priv;
    aos_free(extradata_orip);
    g_adicore.ad_cnt++;
    adicore_unlock();

    return hdl;
err:
    adicore_unlock();
    aos_free(extradata_orip);
    aos_free(hdl);
    aos_free(priv);
    return NULL;
}
#else
adicore_t* adicore_open(const adih_t *ash)
{
    int rc;
    char *es_data, *es_orip      = NULL;
    char *pool, *pool_orip       = NULL;
    uint8_t *extradata           = NULL;
    uint8_t *extradata_orip      = NULL;
    adicore_t *hdl               = NULL;
    struct adicore_ap_priv *priv = NULL;
    icore_msg_t *msg             = NULL;
    icore_msg_t *msg_dec         = NULL;
    adicore_open_t *inp;

    if (!(ash && ((ash->id == ICORE_CODEC_ID_MP3) ||
                  (ash->id == ICORE_CODEC_ID_AAC)))) {
        return NULL;
    }

    hdl = aos_zalloc(sizeof(adicore_t));
    CHECK_RET_TAG_WITH_RET(hdl, NULL);

    es_orip = aos_zalloc(ICORE_ALIGN_BUFZ(ADICORE_ES_SIZE));
    CHECK_RET_TAG_WITH_GOTO(es_orip, err);
    es_data = ICORE_ALIGN(es_orip);

    pool_orip = aos_zalloc(ICORE_ALIGN_BUFZ(ADICORE_FRAME_POOL_SIZE));
    CHECK_RET_TAG_WITH_GOTO(pool_orip, err);
    pool = ICORE_ALIGN(pool_orip);

    priv = aos_zalloc(sizeof(struct adicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(priv, err);

    msg_dec = icore_msg_new(ICORE_CMD_AD_DECODE, sizeof(adicore_decode_t));
    CHECK_RET_TAG_WITH_GOTO(msg_dec, err);

    msg = icore_msg_new(ICORE_CMD_AD_OPEN, sizeof(adicore_open_t));
    CHECK_RET_TAG_WITH_GOTO(msg, err);

    inp = icore_get_msg(msg, adicore_open_t);
    inp->ash.id = ash->id;
    if (ash->extradata && ash->extradata_size) {
        extradata_orip = aos_zalloc(ICORE_ALIGN_BUFZ(ash->extradata_size));
        CHECK_RET_TAG_WITH_GOTO(extradata_orip, err);
        extradata = ICORE_ALIGN(extradata_orip);

        memcpy(extradata, ash->extradata, ash->extradata_size);
        inp->ash.extradata      = extradata;
        inp->ash.extradata_size = ash->extradata_size;
        csi_dcache_clean_invalid_range((uint32_t*)inp->ash.extradata, inp->ash.extradata_size);
    }

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    priv->es_data   = (uint8_t*)es_data;
    priv->es_orip   = (uint8_t*)es_orip;
    priv->es_size   = ADICORE_ES_SIZE;
    priv->pool      = (uint8_t*)pool;
    priv->pool_orip = (uint8_t*)pool_orip;
    priv->pool_size = ADICORE_FRAME_POOL_SIZE;
    priv->msg_dec   = msg_dec;

    hdl->ad   = inp->ad;
    hdl->sf   = inp->sf;
    hdl->priv = priv;
    icore_msg_free(msg);
    aos_free(extradata_orip);

    return hdl;
err:
    aos_free(extradata_orip);
    aos_free(hdl);
    aos_free(es_orip);
    aos_free(pool_orip);
    aos_free(priv);
    icore_msg_free(msg);
    icore_msg_free(msg_dec);
    return NULL;
}
#endif

/**
* @brief  get the audio sample format
* @param  [in] hdl
* @param  [out] sf
* @return 0/-1
*/
int adicore_get_sf(adicore_t *hdl, sf_t *sf)
{
    int rc = -1;

    if (!(hdl && sf)) {
        return rc;
    }

    *sf = hdl->sf;
    return rc;
}


/**
 * @brief  decode one audio frame
 * @param  [in] hdl
 * @param  [out] frame : frame of pcm
 * @param  [out] got_frame : whether decode one frame
 * @param  [in]  pkt : data of encoded audio
 * @return -1 when err happens, otherwise the number of bytes consumed from the
 *         input avpacket is returned
 */
int adicore_decode(adicore_t *hdl, avframe_t *frame, int *got_frame, const avpacket_t *pkt)
{
    int rc = -1, ret;
    icore_msg_t *msg;
    adicore_decode_t *inp;
    struct adicore_ap_priv *priv;

    if (!(hdl && frame && got_frame && pkt && pkt->data && pkt->len && (pkt->len < ADICORE_ES_SIZE))) {
        return rc;
    }

    adicore_lock();
    *got_frame = 0;
    priv       = hdl->priv;
    msg        = priv->msg_dec;
    inp        = icore_get_msg(msg, adicore_decode_t);
    memset(inp, 0, sizeof(adicore_decode_t));
    avframe_set_mempool(&inp->frame, priv->pool, priv->pool_size);
    inp->ad      = hdl->ad;
    inp->es_len  = pkt->len;
    inp->es_data = priv->es_data;
    memcpy(inp->es_data, pkt->data, pkt->len);
    csi_dcache_invalid_range((uint32_t*)priv->pool, priv->pool_size);
    csi_dcache_clean_invalid_range((uint32_t*)inp->es_data, inp->es_len);
    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    if (rc > 0 && inp->got_frame) {
        *got_frame = inp->got_frame;
        ret = avframe_copy_from(&inp->frame, frame);
        if (ret != 0) {
            rc = -1;
            LOGE(TAG, "frame copy failed, may be oom. ret = %d, mpool = %p, msize = %d, linesize = %d\n",
                 ret, frame->mpool, frame->msize, inp->frame.linesize[0]);
        }
    }
    adicore_unlock();

    return rc;
}

/**
 * @brief  reset the adicore
 * @param  [in] hdl
 * @return 0/-1
 */
int adicore_reset(adicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;

    if (!hdl) {
        return rc;
    }

    msg  = icore_msg_new(ICORE_CMD_AD_RESET, sizeof(adicore_reset_t));
    if (msg) {
        adicore_reset_t *inp = icore_get_msg(msg, adicore_reset_t);
        inp->ad = hdl->ad;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
        icore_msg_free(msg);
    }

    return rc;
}

/**
 * @brief  close/destroy audio remote-ipc decoder
 * @param  [in] hdl
 * @return 0/-1
 */
#ifdef ADICORE_SRAME_USE
int adicore_close(adicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct adicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    adicore_lock();
    priv = hdl->priv;
    msg  = priv->msg_close;
    if (msg) {
        adicore_close_t *inp = icore_get_msg(msg, adicore_close_t);
        inp->ad = hdl->ad;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    }

    aos_free(hdl->priv);
    aos_free(hdl);
    g_adicore.ad_cnt--;
    adicore_unlock();

    return rc;
}
#else
int adicore_close(adicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    struct adicore_ap_priv *priv;

    if (!hdl) {
        return rc;
    }

    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_AD_CLOSE, sizeof(adicore_close_t));
    if (msg) {
        adicore_close_t *inp = icore_get_msg(msg, adicore_close_t);
        inp->ad = hdl->ad;

        rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
        icore_msg_free(msg);
    }

    aos_free(priv->es_orip);
    aos_free(priv->pool_orip);
    icore_msg_free(priv->msg_dec);
    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}
#endif



