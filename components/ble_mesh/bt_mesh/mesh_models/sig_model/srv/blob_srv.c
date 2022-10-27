/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include "mesh_model/mesh_model.h"
#include "inc/net.h"
#include "inc/mesh.h"
#include "mesh_model/multi_flag.h"
#include "aos/kv.h"

#define BT_DBG_ENABLED 0
#include "common/log.h"

#define TAG "BLOB_SRV"

#if defined(CONFIG_BT_MESH_MODEL_BLOB_SRV) && CONFIG_BT_MESH_MODEL_BLOB_SRV > 0

#ifndef CID_NVAL
#define CID_NVAL 0xFFFF
#endif

#define TTL_NOT_SET 0xFF

#ifndef BLOB_NUM_NOT_SET
#define BLOB_NUM_NOT_SET 0xFFFF
#endif

enum
{
    BLOB_ID_SET_IDLE    = 0x00,
    BLOB_ID_SET_FAILED   = 0x01,
    BLOB_ID_SET_SUCCESS = 0x02,
};

extern int         bt_rand(void *buf, size_t len);
extern u8_t        bt_mesh_mod_sub_del(struct bt_mesh_model *model, u16_t sub_addr);
extern const char *bt_hex_real(const void *buf, size_t len);

static u8_t get_send_ttl(u8_t cli_ttl, u8_t recv_ttl)
{
    if (cli_ttl == TTL_NOT_SET || recv_ttl > cli_ttl) {
        return BLOB_SRV_DEFAULT_TTL;
    } else {
        return cli_ttl - recv_ttl == 0 ? 0 : cli_ttl - recv_ttl + 1;
    }
}

static inline struct bt_mesh_model *get_blob_srv_model(u8_t elem_id)
{
    return ble_mesh_model_find(elem_id, BT_MESH_MODEL_ID_BLOB_SRV, CID_NVAL);
}

static void sleep_random()
{
    u8_t delay;
    bt_rand(&delay, 1);

    if (delay < BLOB_ACK_MIN_RANDOM_DELAY) {
        delay = BLOB_ACK_MIN_RANDOM_DELAY;
    } else if (delay > BLOB_ACK_MAX_RANDOM_DELAY) {
        delay = BLOB_ACK_MAX_RANDOM_DELAY;
    }

    k_sleep(delay);
}

static u8_t _blob_set_multiaddr(struct bt_mesh_model *model, uint16_t group_addr)
{
    int       ret          = 0;
    u8_t      status       = 0;
    blob_srv *blob_srv_ctx = model->user_data;

    status = bt_mesh_mod_sub_add(model, group_addr);

    if (status) {
        BT_ERR("Trans set id set sub failed %02x", status);
        blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_FAILED;
    } else {
        blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_SUCCESS;
    }

    ret = aos_kv_set("MULTI_ADDR", &group_addr, 2, 0);

    if (ret) {
        BT_ERR("multi addr kv save fail %d", ret);
    }

    return status;
}

static u8_t _blob_del_multiaddr(struct bt_mesh_model *model, uint16_t addr)
{
    int       ret          = 0;
    u8_t      status       = 0;
    blob_srv *blob_srv_ctx = model->user_data;
    status                 = bt_mesh_mod_sub_del(model, addr);

    if (status) {
        BT_ERR("Trans del id del sub failed %02x", status);
        blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_FAILED;
    } else {
        blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_SUCCESS;
    }

    ret = aos_kv_del("MULTI_ADDR");

    if (ret) {
        BT_WARN("multi addr kv key erase fail %d", ret);
    }

    return status;
}

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
static bool _is_send_ack_status(blob_srv *blob_srv_ctx, struct net_buf_simple *buf)
{
    uint16_t id_index        = blob_srv_ctx->node_id / 8;
    u8_t     status_ack_flag = 0x0;
    u8_t     flag            = net_buf_simple_pull_u8(buf);

    if (flag == ACK_STATUS_RECV_NULL) {
        return true;
    } else if (flag == ACK_STATUS_RECV_ALL) {
        blob_srv_ctx->ack_last = 0;
        return false;
    }

    BT_DBG("id set flag:%02x", flag);

    if (buf->len >= id_index + 1) {
        status_ack_flag = buf->data[id_index] & (0x01 << (blob_srv_ctx->node_id % 8)) ? 0x01 : 0x00;
        BT_DBG("id flag:%02x flag:%d", buf->data[id_index], status_ack_flag);
    } else {
        BT_DBG("Ignore the id info");
        return true;
    }

    if (status_ack_flag) {
        blob_srv_ctx->ack_last = 0;
        BT_DBG("Not ack the message");
        return false;
    }

    return true;
}
#endif

static u8_t status_data_report(mesh_model_event_en event, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf,
                               void *user_data)
{
    model_message message = { 0 };
    message.trans         = ctx->trans;
    message.source_addr   = ctx->addr;
    message.dst_addr      = ctx->recv_dst;
    message.status_data   = buf;
    message.user_data     = user_data;
    model_event(event, (void *)&message);
    return 0;
}

static int _blob_srv_trans_status_send(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, u8_t status)
{
    blob_srv *blob_srv_ctx = model->user_data;
    long long ack_now      = 0;
    u8_t      send_ttl     = 0;
    u8_t      hdr          = 0;
    int       err          = 0;

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + MAX_BLOB_TRANS_STATUS_LEN + 4);

    struct bt_mesh_msg_ctx send_ctx = { 0 };

    send_ctx.trans = ctx->trans;

    bt_mesh_model_msg_init(msg, OP_BLOB_TRANS_STATUS);

    hdr = TRANS_STATUS_HDR(status, blob_srv_ctx->blob_srv_trans_mode);

    net_buf_simple_add_u8(msg, hdr);
    net_buf_simple_add_u8(msg, blob_srv_ctx->blob_trans_phase);

    if (blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_INACTIVE) {
        net_buf_simple_add_le64(msg, blob_srv_ctx->blob_srv_info.blob_id);

        if (blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_WAITING_FOR_START) {
            net_buf_simple_add_le32(msg, blob_srv_ctx->blob_srv_info.blob_size);
            net_buf_simple_add_le16(msg, blob_srv_ctx->blob_srv_info.block_size_log);
            net_buf_simple_add_le16(msg, blob_srv_ctx->blob_srv_mtu_size);
            net_buf_simple_add_mem(msg, blob_srv_ctx->blob_blocks_missing,
                                   (blob_srv_ctx->blob_srv_info.block_total + 7) / 8);
        }
    }

    send_ctx.addr    = ctx->addr;
    send_ctx.net_idx = ctx->net_idx;
    send_ctx.app_idx = ctx->net_idx;
    send_ctx.trans   = ctx->trans;
    ack_now          = aos_now_ms();

    if (blob_srv_ctx->ack_last == 0 || ack_now - blob_srv_ctx->ack_last > DEF_MAX_STATUS_GET_RETRY) {
        send_ttl               = get_send_ttl(blob_srv_ctx->blob_cli_ttl, ctx->recv_ttl);
        blob_srv_ctx->ack_last = ack_now;
    } else {
        send_ttl = blob_srv_ctx->blob_cli_ttl == TTL_NOT_SET ? BLOB_SRV_DEFAULT_TTL : blob_srv_ctx->blob_cli_ttl;
    }

    send_ctx.send_ttl = send_ttl;

    err = bt_mesh_model_send(model, &send_ctx, msg, NULL, NULL);

    if (err) {
        BT_ERR("Blob send fail %d", err);
        return err;
    }

    return 0;
}

static int _blob_srv_block_status_send(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, u8_t status,
                                       uint16_t block_num, uint16_t chunk_size)
{

    blob_srv *blob_srv_ctx = model->user_data;
    long long ack_now      = 0;
    u8_t      send_ttl     = 0;
    int       err          = 0;
    u8_t      hdr          = 0;
    u8_t      format       = 0;

    if (block_num == blob_srv_ctx->blob_srv_block_info.block_num) {
        if (is_no_flag_set(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE,
                           blob_srv_ctx->blob_srv_block_info.total_chunk)
            == true)
        {
            format = FORMAT_NO_CHUNKS_MISSING;
        } else if (is_all_flags_set(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE,
                                    blob_srv_ctx->blob_srv_block_info.total_chunk)
                   == true)
        {
            format = FORMAT_ALL_CHUNKS_MISSING;
        } else {
            format = FORMAT_SOME_CHUNKS_MISSING;
        } // TODO FOR CASE FORMAT ENCODED CASE
    } else {
        if (is_flag_set(blob_srv_ctx->blob_blocks_missing, CONFIG_BLOB_SRV_MAX_MISS_BLOCKS_FLAG_SIZE, block_num)) {
            format = FORMAT_ALL_CHUNKS_MISSING; // TODO for single block chunks state save
        } else {
            format = FORMAT_NO_CHUNKS_MISSING;
        }
    }

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE) {
        format = FORMAT_ALL_CHUNKS_MISSING;
    }

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + MAX_BLOCK_STATUS_LEN + 4);

    struct bt_mesh_msg_ctx send_ctx = { 0 };

    send_ctx.trans = ctx->trans;

    bt_mesh_model_msg_init(msg, OP_BLOB_BLOCK_STATUS);

    hdr = BLOCK_STATUS_HDR(status, format);

    net_buf_simple_add_u8(msg, hdr);

    net_buf_simple_add_le16(msg, block_num);

    net_buf_simple_add_le16(msg, chunk_size);

    if (format != FORMAT_ALL_CHUNKS_MISSING || format != FORMAT_NO_CHUNKS_MISSING) { // TODO status ok?
        if (format == FORMAT_SOME_CHUNKS_MISSING) {
            BT_DBG("Send missing chunks %s", bt_hex_real(blob_srv_ctx->blob_chunks_missing,
                                                         (blob_srv_ctx->blob_srv_block_info.total_chunk + 7) / 8));
            net_buf_simple_add_mem(msg, blob_srv_ctx->blob_chunks_missing,
                                   (blob_srv_ctx->blob_srv_block_info.total_chunk + 7) / 8);
        } // TODO FOR ENCODED
    }

    send_ctx.addr    = ctx->addr;
    send_ctx.net_idx = ctx->net_idx;
    send_ctx.app_idx = ctx->net_idx;
    send_ctx.trans   = ctx->trans;
    ack_now          = aos_now_ms();

    if (blob_srv_ctx->ack_last == 0 || ack_now - blob_srv_ctx->ack_last > DEF_MAX_STATUS_GET_RETRY) {
        send_ttl               = get_send_ttl(blob_srv_ctx->blob_cli_ttl, ctx->recv_ttl);
        blob_srv_ctx->ack_last = ack_now;
    } else {
        send_ttl = blob_srv_ctx->blob_cli_ttl == TTL_NOT_SET ? BLOB_SRV_DEFAULT_TTL : blob_srv_ctx->blob_cli_ttl;
    }

    send_ctx.send_ttl = send_ttl;
    err               = bt_mesh_model_send(model, &send_ctx, msg, NULL, NULL);

    if (err) {
        BT_ERR("Blob send fail %d", err);
        return err;
    }

    return 0;
}

static int _blob_srv_info_send(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx)
{
    blob_srv *blob_srv_ctx = model->user_data;
    long long ack_now      = 0;
    u8_t      send_ttl     = 0;
    int       err          = 0;
    // TODO  TEMP foramt

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 15 + 4);

    struct bt_mesh_msg_ctx send_ctx = { 0 };

    send_ctx.trans = ctx->trans;

    bt_mesh_model_msg_init(msg, OP_BLOB_INFO_STATUS);

    net_buf_simple_add_le16(msg, CONFIG_BLOB_SRV_MIN_BLOCK_SIZE_LOG);
    net_buf_simple_add_le16(msg, CONFIG_BLOB_SRV_MAX_BLOCK_SIZE_LOG);
    net_buf_simple_add_le16(msg, CONFIG_BLOB_SRV_MAX_TOTAL_CHUNKS);
    net_buf_simple_add_le16(msg, CONFIG_BLOB_SRV_MAX_CHUNK_SIZE);
    net_buf_simple_add_le32(msg, CONFIG_BLOB_SRV_MAX_BLOB_SIZE);
    net_buf_simple_add_le16(msg, blob_srv_ctx->blob_srv_mtu_size); // TODO init mtu size before use
    net_buf_simple_add_u8(msg, blob_srv_ctx->blob_srv_trans_mode);

    send_ctx.addr    = ctx->addr;
    send_ctx.net_idx = ctx->net_idx;
    send_ctx.app_idx = ctx->net_idx;
    send_ctx.trans   = ctx->trans;
    ack_now          = aos_now_ms();

    if (blob_srv_ctx->ack_last == 0 || ack_now - blob_srv_ctx->ack_last > DEF_MAX_STATUS_GET_RETRY) {
        send_ttl               = get_send_ttl(blob_srv_ctx->blob_cli_ttl, ctx->recv_ttl);
        blob_srv_ctx->ack_last = ack_now;
    } else {
        send_ttl = blob_srv_ctx->blob_cli_ttl == TTL_NOT_SET ? BLOB_SRV_DEFAULT_TTL : blob_srv_ctx->blob_cli_ttl;
    }

    send_ctx.send_ttl = send_ttl;

    err = bt_mesh_model_send(model, &send_ctx, msg, NULL, NULL);

    if (err) {
        BT_ERR("Blob send fail %d", err);
        return err;
    }

    return 0;
}

static int _blob_srv_trans_set_status_send(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, u8_t status)
{
    blob_srv *blob_srv_ctx = model->user_data;
    u8_t      send_ttl;
    long long ack_now = 0;
    int       err     = 0;
    // TODO  TEMP STATUS

    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);

    struct bt_mesh_msg_ctx send_ctx = { 0 };

    bt_mesh_model_msg_init(msg, OP_BLOB_TRANS_SET_STATUS);

    net_buf_simple_add_u8(msg, status);

    send_ctx.addr    = ctx->addr;
    send_ctx.net_idx = ctx->net_idx;
    send_ctx.app_idx = ctx->net_idx;
    send_ctx.trans   = NET_TRANS_LEGACY;
    ack_now          = aos_now_ms();

    if (blob_srv_ctx->ack_last == 0 || ack_now - blob_srv_ctx->ack_last > DEF_MAX_STATUS_GET_RETRY) {
        send_ttl               = get_send_ttl(blob_srv_ctx->blob_cli_ttl, ctx->recv_ttl);
        blob_srv_ctx->ack_last = ack_now;
    } else {
        send_ttl = blob_srv_ctx->blob_cli_ttl == TTL_NOT_SET ? BLOB_SRV_DEFAULT_TTL : blob_srv_ctx->blob_cli_ttl;
    }

    send_ctx.send_ttl = send_ttl;

    err = bt_mesh_model_send(model, &send_ctx, msg, NULL, NULL);

    if (err) {
        BT_ERR("Blob send fail %d", err);
        return err;
    }

    return 0;
}

static u8_t _blob_trans_start_process(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf, u8_t *ack_flag)
{
    blob_srv *blob_srv_ctx = model->user_data;

    if (blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_WAITING_FOR_START
        && blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK
        && blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK
        && blob_srv_ctx->blob_trans_phase != TRANSFER_PHASE_COMPLETE)
    {
        return TRANSFER_STATUS_WRONG_PHASE;
    }

    u8_t hdr  = net_buf_simple_pull_u8(buf);
    u8_t mode = hdr & 0x3;

    if (mode != blob_srv_ctx->blob_srv_trans_mode) {
        return TRANSFER_STATUS_UNSUPPORTED_TRANSFER_MODE;
    }

    uint64_t blob_id = net_buf_simple_pull_le64(buf);

    if (blob_srv_ctx->blob_expected_blob_id != UNKNOW_EXCEPTED_BLOB_ID
        && blob_srv_ctx->blob_expected_blob_id != blob_id) {

        return TRANSFER_STATUS_WRONG_BLOB_ID;
    }

    uint32_t blob_size = net_buf_simple_pull_le32(buf);

    if (blob_size > CONFIG_BLOB_SRV_MAX_BLOB_SIZE) {
        return TRANSFER_STATUS_BLOB_TOO_LARGE;
    }

    uint16_t block_size_log = net_buf_simple_pull_le16(buf);

    if (block_size_log > CONFIG_BLOB_SRV_MAX_BLOCK_SIZE_LOG || block_size_log < CONFIG_BLOB_SRV_MIN_BLOCK_SIZE_LOG) {
        return TRANSFER_STATUS_INVALID_BLOCK_SIZE;
    }

    uint16_t cli_mtu_size = net_buf_simple_pull_le16(buf); // todo consider status?

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        *ack_flag = 0x00;
        return TRANSFER_STATUS_SUCCESS; // todo
    } else {
        *ack_flag = 0x01;
    }

#endif

    // TODO HOW TO cal
    // blob_srv_ctx->blob_trans_phase   = TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK;//TODO
    blob_srv_ctx->blob_srv_info.blob_id        = blob_id;
    blob_srv_ctx->blob_srv_info.blob_size      = blob_size;
    blob_srv_ctx->blob_srv_info.block_size_log = block_size_log;
    blob_srv_ctx->blob_srv_info.block_total_size
        = blob_size < LOG_SIZE(block_size_log) ? blob_size : LOG_SIZE(block_size_log);
    blob_srv_ctx->blob_srv_mtu_size
        = cli_mtu_size <= blob_srv_ctx->blob_srv_mtu_size ? cli_mtu_size : blob_srv_ctx->blob_srv_mtu_size;
    blob_srv_ctx->blob_srv_info.block_total
        = (blob_size + blob_srv_ctx->blob_srv_info.block_total_size - 1) / blob_srv_ctx->blob_srv_info.block_total_size;
    // blob_srv_ctx->state.blob_max_chunk_size     = (blob_size + blob_srv_ctx->blob_srv_mtu_size - 1) /
    // blob_srv_ctx->blob_srv_mtu_size;
    set_all_flags(blob_srv_ctx->blob_blocks_missing, CONFIG_BLOB_SRV_MAX_MISS_BLOCKS_FLAG_SIZE,
                  blob_srv_ctx->blob_srv_info.block_total);
    BT_DBG("Trans start info blob_id:%08x blob size:%d block size log:%d block size:%d mtu size:%d total blocks:%d",
           blob_id, blob_srv_ctx->blob_srv_info.blob_size, blob_srv_ctx->blob_srv_info.block_size_log,
           blob_srv_ctx->blob_srv_info.block_total_size, blob_srv_ctx->blob_srv_mtu_size,
           blob_srv_ctx->blob_srv_info.block_total);
    // TODO Start a timer
    return TRANSFER_STATUS_SUCCESS;
}

static int _blob_srv_reset(struct bt_mesh_model *model)
{
    blob_srv *blob_srv_ctx                   = model->user_data;
    blob_srv_ctx->blob_trans_phase           = TRANSFER_PHASE_INACTIVE;
    blob_srv_ctx->node_id                    = 0;
    blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_IDLE;
    _blob_del_multiaddr(model, blob_srv_ctx->blob_multicast_addr);
    blob_srv_ctx->blob_multicast_addr = 0x0;
    return 0;
}

static u8_t _blob_trans_cancel_process(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                       struct net_buf_simple *buf, u8_t *ack_flag)
{
    blob_srv *blob_srv_ctx = model->user_data;
    int       ret          = 0;

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE) {
        return TRANSFER_STATUS_SUCCESS;
    }

    uint64_t blob_id = net_buf_simple_pull_le64(buf);

    if (blob_srv_ctx->blob_expected_blob_id != UNKNOW_EXCEPTED_BLOB_ID
        && blob_srv_ctx->blob_expected_blob_id != blob_id) {
        return TRANSFER_STATUS_WRONG_BLOB_ID;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        *ack_flag = 0x00;
        return TRANSFER_STATUS_SUCCESS; // todo
    } else {
        *ack_flag = 0x01;
    }

#endif

    ret = _blob_srv_reset(model);

    if (ret < 0) {
        return TRANSFER_STATUS_INTERNAL_ERROR;
    }

    return TRANSFER_STATUS_SUCCESS;
}

static u8_t _blob_block_start_process(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf, uint16_t *send_block_num, uint16_t *send_chunk_size,
                                      u8_t *ack_flag)
{
    blob_srv *blob_srv_ctx = model->user_data;

    uint16_t block_num  = net_buf_simple_pull_le16(buf);
    uint16_t chunk_size = net_buf_simple_pull_le16(buf);

    *send_block_num  = block_num;
    *send_chunk_size = chunk_size;

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        *ack_flag = 0x00;
        return TRANSFER_STATUS_SUCCESS; // todo
    } else {
        *ack_flag = 0x01;
    }

#endif

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_WAITING_FOR_START
        || blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE)
    {
        return TRANSFER_STATUS_WRONG_PHASE;
    } else if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_COMPLETE) {
        return TRANSFER_STATUS_SUCCESS;
    }

    uint16_t max_chunk_total = (blob_srv_ctx->blob_srv_info.block_total_size + chunk_size - 1) / chunk_size;

    if (block_num > blob_srv_ctx->blob_srv_info.block_total - 1) { // TODO 7.4.1.4.6
        return TRANSFER_STATUS_INVALID_BLOCK_NUMBER;
    }

    if (chunk_size > CONFIG_BLOB_SRV_MAX_CHUNK_SIZE || max_chunk_total > CONFIG_BLOB_SRV_MAX_TOTAL_CHUNKS) {
        return TRANSFER_STATUS_INVALID_CHUNK_SIZE;
    }

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK) { // TODO
        if (block_num == blob_srv_ctx->blob_srv_block_info.block_num
            && chunk_size == blob_srv_ctx->blob_srv_block_info.chunk_size)
        { // the same block
            return TRANSFER_STATUS_SUCCESS;
        } else {
            return TRANSFER_STATUS_WRONG_PHASE;
        }

    } else if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK
               || blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_SUSPEND)
    {
        if (is_flag_set(blob_srv_ctx->blob_blocks_missing, CONFIG_BLOB_SRV_MAX_MISS_BLOCKS_FLAG_SIZE, block_num))
        { // new block not received
            blob_srv_ctx->blob_trans_phase               = TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK;
            blob_srv_ctx->blob_srv_block_info.chunk_size = chunk_size;
            blob_srv_ctx->blob_srv_block_info.block_num  = block_num;
            uint32_t last_block_size
                = blob_srv_ctx->blob_srv_info.blob_size % LOG_SIZE(blob_srv_ctx->blob_srv_info.block_size_log) == 0
                      ? LOG_SIZE(blob_srv_ctx->blob_srv_info.block_size_log)
                      : blob_srv_ctx->blob_srv_info.blob_size % LOG_SIZE(blob_srv_ctx->blob_srv_info.block_size_log);
            blob_srv_ctx->blob_srv_block_info.block_size
                = (blob_srv_ctx->blob_srv_block_info.block_num == blob_srv_ctx->blob_srv_info.block_total - 1)
                      ? last_block_size
                      : LOG_SIZE(blob_srv_ctx->blob_srv_info.block_size_log);
            blob_srv_ctx->blob_srv_block_info.total_chunk
                = (blob_srv_ctx->blob_srv_block_info.block_size + blob_srv_ctx->blob_srv_block_info.chunk_size - 1)
                  / blob_srv_ctx->blob_srv_block_info.chunk_size;
            BT_DBG("Block %d start,total chunks %d", block_num, blob_srv_ctx->blob_srv_block_info.total_chunk);
            set_all_flags(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE,
                          blob_srv_ctx->blob_srv_block_info.total_chunk);
        } else { // the block has been received
            return TRANSFER_STATUS_SUCCESS;
        }
    }

    return TRANSFER_STATUS_SUCCESS;
}

static u8_t _blob_block_get_process(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf, u8_t *ack_flag)
{
    blob_srv *blob_srv_ctx = model->user_data;

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        *ack_flag = 0x00;
        return TRANSFER_STATUS_SUCCESS; // todo
    } else {
        *ack_flag = 0x01;
    }

#endif

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_SUSPEND) {
        return TRANSFER_STATUS_INFOMATION_UNAVAILABLE;
    } else if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE
               || blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_WAITING_FOR_START)
    {
        return TRANSFER_STATUS_WRONG_PHASE;
    }

    return TRANSFER_STATUS_SUCCESS;
}

static void _blob_srv_trans_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    BT_DBG("blob trans get");
    blob_srv *blob_srv_ctx = model->user_data;

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && blob_srv_ctx && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        return;
    }

#endif

    int ret = 0;

    u8_t status = TRANSFER_STATUS_SUCCESS;

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    ret = _blob_srv_trans_status_send(model, ctx, status);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }
}

static void _blob_srv_trans_start(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    int ret = 0;

    u8_t                        send_status_flag = 1;
    blob_srv *                  blob_srv_ctx     = model->user_data;
    struct net_buf_simple_state state;

    u8_t status = _blob_trans_start_process(model, ctx, buf, &send_status_flag);
    BT_DBG("blob trans start process status %02x", status);

    if (!send_status_flag) {
        return;
    }

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    ret = _blob_srv_trans_status_send(model, ctx, status);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        // return;
    }

    blob_srv_ctx->blob_trans_phase = TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK;
    net_buf_simple_save(buf, &state);
    status_data_report(BT_MESH_MODEL_BLOB_TRANS_START, ctx, buf, blob_srv_ctx);
    net_buf_simple_restore(buf, &state);
}

static void _blob_srv_trans_cancel(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    int ret = 0;

    u8_t                        send_status_flag = 1;
    blob_srv *                  blob_srv_ctx     = model->user_data;
    struct net_buf_simple_state state;

    BT_DBG("blob trans cancel");
    u8_t status = _blob_trans_cancel_process(model, ctx, buf, &send_status_flag);

    if (!send_status_flag) {
        return;
    }

    net_buf_simple_save(buf, &state);
    status_data_report(BT_MESH_MODEL_BLOB_TRANS_CANCEL, ctx, buf, blob_srv_ctx);
    net_buf_simple_restore(buf, &state);

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    ret = _blob_srv_trans_status_send(model, ctx, status);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }
}

static void _blob_srv_block_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    int ret = 0;

    u8_t send_status_flag = 1;
    BT_DBG("blob block get");

    blob_srv *blob_srv_ctx = model->user_data;

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE) {
        BT_DBG("Node inactive,ignore the message");
        return;
    }

    u8_t status = _blob_block_get_process(model, ctx, buf, &send_status_flag);

    if (!send_status_flag) {
        return;
    }

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    ret = _blob_srv_block_status_send(model, ctx, status, blob_srv_ctx->blob_srv_block_info.block_num,
                                      blob_srv_ctx->blob_srv_block_info.chunk_size);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }

    status_data_report(BT_MESH_MODEL_BLOB_BLOCK_GET, ctx, NULL, blob_srv_ctx);
}

static void _blob_srv_block_start(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    BT_DBG("blob block start");
    int ret = 0;

    uint16_t                    block_num        = 0;
    uint16_t                    chunk_size       = 0;
    u8_t                        send_status_flag = 1;
    struct net_buf_simple_state state;
    blob_srv *                  blob_srv_ctx = model->user_data;

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE) {
        BT_DBG("Node inactive,ignore the message");
        return;
    }

    net_buf_simple_save(buf, &state);
    u8_t status = _blob_block_start_process(model, ctx, buf, &block_num, &chunk_size, &send_status_flag);

    if (!send_status_flag) {
        net_buf_simple_restore(buf, &state);
        return;
    }

    net_buf_simple_save(buf, &state);
    status_data_report(BT_MESH_MODEL_BLOB_BLOCK_START, ctx, buf, blob_srv_ctx);
    net_buf_simple_restore(buf, &state);

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    ret = _blob_srv_block_status_send(model, ctx, status, block_num, chunk_size);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }
}

static void _blob_srv_chunk_transfer(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                     struct net_buf_simple *buf)
{
    blob_srv *                  blob_srv_ctx = model->user_data;
    struct net_buf_simple_state state;

    if (blob_srv_ctx->blob_trans_phase == TRANSFER_PHASE_INACTIVE) {
        BT_DBG("Node inactive,ignore the message");
        return;
    }

    net_buf_simple_save(buf, &state);
    uint16_t chunk_num = net_buf_simple_pull_le16(buf);

    if (is_flag_set(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE, chunk_num) == false) {
        // BT_DBG("Chunk %d already received",chunk_num);
        net_buf_simple_restore(buf, &state);
        return;
    }

    clear_flag(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE, chunk_num);

    // BT_DBG("Get chunk:%d flags index %d status now %02x",chunk_num,chunk_num /
    // 8,blob_srv_ctx->blob_chunks_missing[chunk_num / 8]);
    if (is_no_flag_set(blob_srv_ctx->blob_chunks_missing, CONFIG_BLOB_SRV_MAX_MISS_CHUNKS_FLAG_SIZE,
                       blob_srv_ctx->blob_srv_block_info.total_chunk))
    { // All chunks received
        clear_flag(blob_srv_ctx->blob_blocks_missing, CONFIG_BLOB_SRV_MAX_MISS_BLOCKS_FLAG_SIZE,
                   blob_srv_ctx->blob_srv_block_info.block_num);
        BT_DBG("Blocks %d Received Complete", blob_srv_ctx->blob_srv_block_info.block_num);

        if (is_no_flag_set(blob_srv_ctx->blob_blocks_missing, CONFIG_BLOB_SRV_MAX_MISS_BLOCKS_FLAG_SIZE,
                           blob_srv_ctx->blob_srv_info.block_total))
        { // all blocks received
            blob_srv_ctx->blob_trans_phase = TRANSFER_PHASE_COMPLETE;
        } else {
            blob_srv_ctx->blob_trans_phase = TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK;
        }
    } else {
        blob_srv_ctx->blob_trans_phase = TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK;
    }

    net_buf_simple_restore(buf, &state);

    if (buf->len) {
        net_buf_simple_save(buf, &state);
        status_data_report(BT_MESH_MODEL_BLOB_CHUNK_DATA, ctx, buf, blob_srv_ctx);
        net_buf_simple_restore(buf, &state);
    }
}

static void _blob_srv_info_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    blob_srv *blob_srv_ctx = model->user_data;
    BT_DBG("blob info get");
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && blob_srv_ctx && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        return;
    }

#endif

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

    _blob_srv_info_send(model, ctx);
}

static void _blob_srv_trans_set_id(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    BT_DBG("blob trans set id");

    if (!buf || buf->len != 7) {
        BT_ERR("Trans set message err");
        return;
    }

    blob_srv *blob_srv_ctx = model->user_data;
    int       ret          = 0;
    u8_t      status       = 0x00;

    uint16_t group_addr        = net_buf_simple_pull_le16(buf);
    uint32_t id                = net_buf_simple_pull_le32(buf);
    blob_srv_ctx->blob_cli_ttl = net_buf_simple_pull_u8(buf);

    if (blob_srv_ctx->blob_trans_ctx.id_set_flag == BLOB_ID_SET_SUCCESS) {
        if (blob_srv_ctx->blob_multicast_addr == group_addr) {
            goto send;
        } else {
            status = _blob_del_multiaddr(model, blob_srv_ctx->blob_multicast_addr);

            if (status) {
                BT_ERR("Del old id %04x failed err: %02x", blob_srv_ctx->blob_multicast_addr, status);
                goto send;
            } else {
                BT_DBG("Del old id %04x success", blob_srv_ctx->blob_multicast_addr);
            }
        }
    }

    status = _blob_set_multiaddr(model, group_addr);

    if (!status) {
        blob_srv_ctx->node_id             = id;
        blob_srv_ctx->blob_multicast_addr = group_addr;
        BT_DBG("Blob id set %d Multiaddr set %04", id, group_addr);
    } else {
        BT_DBG("Blob id set fail %d", status);
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        return;
    }

#endif

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

send:
    ret = _blob_srv_trans_set_status_send(model, ctx, status);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }
}

static void _blob_srv_trans_del_id(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    BT_DBG("blob trans del id");

    if (!buf || buf->len != 2) {
        BT_ERR("Trans del message err");
        return;
    }

    blob_srv *blob_srv_ctx = model->user_data;
    int       ret          = 0;
    u8_t      status       = 0;

    if (blob_srv_ctx->blob_trans_ctx.id_set_flag == BLOB_ID_SET_IDLE) {
        goto send;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    u8_t send_status_flag = 1;
#endif
    uint16_t group_addr = net_buf_simple_pull_le16(buf);

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (buf->len && _is_send_ack_status(blob_srv_ctx, buf) == false) {
        send_status_flag = 0;
    }

#endif

    status = _blob_del_multiaddr(model, group_addr);

    if (status) {
        BT_ERR("Multiaddr del failed %02x", status);
    } else {
        blob_srv_ctx->blob_trans_ctx.id_set_flag = BLOB_ID_SET_IDLE;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0

    if (!send_status_flag) {
        return;
    }

#endif

    if (BT_MESH_ADDR_IS_GROUP(ctx->recv_dst)) {
        sleep_random();
    }

send:
    ret = _blob_srv_trans_set_status_send(model, ctx, status);

    if (ret) {
        BT_ERR("Send trans status failed %d", ret);
        return;
    }
}

int blob_srv_trans_init(u8_t elem_id, u8_t mode, uint64_t except_blob_id)
{
    int        ret = 0;
    uint16_t   multiAddr;
    static int init_flag = 0;
    int        kv_size   = 2;

    if (init_flag) {
        return 0;
    }

    struct bt_mesh_model *blob_srv_model = get_blob_srv_model(elem_id);

    if (!blob_srv_model) {
        BT_ERR("Blob srv model not found");
        return -1;
    }

    if (CONFIG_MAX_BLOB_SRV_MTU_SIZE <= 4) {
        BT_ERR("Blob cli mtu is %d it should be larger than 4 %d", CONFIG_MAX_BLOB_SRV_MTU_SIZE);
        return -EINVAL;
    }

    blob_srv *blob_srv_ctx = blob_srv_model->user_data;

    blob_srv_ctx->blob_srv_trans_mode            = mode;
    blob_srv_ctx->blob_expected_blob_id          = except_blob_id;
    blob_srv_ctx->blob_trans_phase               = TRANSFER_PHASE_WAITING_FOR_START;
    blob_srv_ctx->blob_srv_block_info.block_num  = 0xFFFF;
    blob_srv_ctx->blob_srv_block_info.chunk_size = 0xFFFF;
    blob_srv_ctx->blob_srv_mtu_size              = CONFIG_MAX_BLOB_SRV_MTU_SIZE;
    blob_srv_ctx->blob_cli_ttl                   = TTL_NOT_SET;
    blob_srv_ctx->ack_last                       = 0;
    ret                                          = aos_kv_get("MULTI_ADDR", &multiAddr, &kv_size);

    if (ret) {
        BT_DBG("get multi addr from kv failed %d", ret);
    } else {
        _blob_del_multiaddr(blob_srv_model, multiAddr);
    }

    init_flag = 1;
    return 0;
}

int blob_srv_trans_set_expect(u8_t elem_id, uint64_t expect_blob_id)
{
    struct bt_mesh_model *blob_srv_model = get_blob_srv_model(elem_id);

    if (!blob_srv_model) {
        BT_ERR("Blob srv model not found");
        return -1;
    }

    blob_srv *blob_srv_ctx = blob_srv_model->user_data; // TODO

    blob_srv_ctx->blob_expected_blob_id = expect_blob_id;
    return 0;
}

const struct bt_mesh_model_op g_blob_srv_op[BLOB_SRV_OPC_NUM] = {
    { OP_BLOB_TRANS_GET, 0, _blob_srv_trans_get },       { OP_BLOB_TRANS_START, 17, _blob_srv_trans_start },
    { OP_BLOB_TRANS_CANCEL, 8, _blob_srv_trans_cancel }, { OP_BLOB_BLOCK_GET, 0, _blob_srv_block_get },
    { OP_BLOB_BLOCK_START, 4, _blob_srv_block_start },   { OP_BLOB_CHUNK_TRANS, 2, _blob_srv_chunk_transfer },
    { OP_BLOB_INFO_GET, 0, _blob_srv_info_get },         { OP_BLOB_TRANS_SET_ID, 4, _blob_srv_trans_set_id },
    { OP_BLOB_TRANS_DEL_ID, 2, _blob_srv_trans_del_id }, BT_MESH_MODEL_OP_END,
};

#endif
