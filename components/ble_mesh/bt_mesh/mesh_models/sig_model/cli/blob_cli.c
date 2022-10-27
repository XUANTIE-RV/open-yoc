/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <api/mesh.h>
#include "errno.h"
#include "mesh_model/mesh_model.h"
#include "mesh_model/multi_flag.h"
#include "ble_os_port.h"
#include "bluetooth/addr.h"
#include "bluetooth/bluetooth.h"
#include "inc/net.h"
#include "inc/mesh.h"
#include "inc/adv.h"
#define BT_DBG_ENABLED 0
#include "common/log.h"
#define TAG "BLOB_CLI"

#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI)

/********************************************************************/
// for blob procedure
/********************************************************************/

/********************************************************************/

typedef struct {
    uint8_t elem_id;
    uint8_t process_cmd;
} blob_cli_cmd;

typedef struct {
    struct k_sem queue_sem;
    uint8_t      front;
    uint8_t      rear;
    uint8_t      length;
    blob_cli_cmd cmds[CONFIG_BLOB_CLI_QUEUE_SIZE];
} blob_cli_queue;

static blob_cli_queue cli_queue;
static BT_STACK_NOINIT(blob_thread_stack, CONFIG_BLOB_CLI_STACK_SIZE);
static struct k_thread blob_thread_data;

extern void  bt_mesh_model_foreach(void (*func)(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd,
                                               bool primary, void *user_data),
                                   void *user_data);
extern u16_t bt_mesh_primary_addr(void);

enum
{
    BLOB_CLI_SRV_STATUS_NOT_RECEIVE  = 0x00,
    BLOB_CLI_SRV_STATUS_RECV_SUCCESS = 0x01,
    BLOB_CLI_SRV_STATUS_RECV_FAILED   = 0x02,
};

enum
{
    BLOB_CLI_EXIST_STATUS_CUCCESS = 0x00,
    BLOB_CLI_EXIST_STATUS_FAIL    = 0x01,
};

enum
{
    READ_SUCCESS = 0x00,
    READ_FINISH  = 0x01,
    READ_FAIL    = 0x02,
};

#define MAX_BLOB_NUM          375
#define CID_NVAL              0xffff
#define DST_PHASE_UNSET       0xff
#define WAIT_CANCEL_FLAG      0xFF
#define ADV_BUF_RESERVE_COUNT (CONFIG_BT_MESH_ADV_BUF_COUNT / 4 + 1)

/********************************************************************/

static uint16_t GET_LOG(uint32_t size)
{
    uint16_t index     = 0;
    uint32_t size_init = size;

    while (size > 1) {
        size = size >> 1;
        index++;
    }

    if (LOG_SIZE(index) < size_init) {
        index++;
    }

    return index;
}

static int _blob_cli_queue_init(blob_cli_queue *queue)
{
    int ret = 0;
    ret     = k_sem_init(&queue->queue_sem, 0, 1);

    if (ret) {
        return ret;
    }

    queue->front  = 0;
    queue->rear   = 0;
    queue->length = CONFIG_BLOB_CLI_QUEUE_SIZE;
    return 0;
}

static bool _is_blob_cli_queue_empty(blob_cli_queue *queue)
{
    return queue->front == queue->rear;
}

static bool _is_blob_cli_queue_full(blob_cli_queue *queue)
{
    uint8_t rear_flag = queue->rear + 1 >= queue->length ? queue->rear + 1 - queue->length : queue->rear + 1;

    if (rear_flag == queue->front) {
        return true;
    } else {
        return false;
    }
}

static int blob_cli_queue_put(blob_cli_queue *queue, blob_cli_cmd cmd)
{
    if (_is_blob_cli_queue_full(queue)) {
        BT_ERR("Blob cli queue full");
        return -1;
    }

    BT_DBG("Cli cmd for elem:%04x cmd start Procedure %s", cmd.elem_id, procedure_str(cmd.process_cmd));
    queue->cmds[queue->rear].elem_id     = cmd.elem_id;
    queue->cmds[queue->rear].process_cmd = cmd.process_cmd;
    queue->rear++;

    if (queue->rear >= queue->length) {
        queue->rear -= queue->length;
    }

    k_sem_give(&queue->queue_sem);
    return 0;
}

static int blob_cli_queue_get(blob_cli_queue *queue, blob_cli_cmd *cmd, int timeout)
{
    k_sem_take(&queue->queue_sem, timeout);

    if (_is_blob_cli_queue_empty(queue)) {
        return -1;
    } else {
        cmd->elem_id     = queue->cmds[queue->front].elem_id;
        cmd->process_cmd = queue->cmds[queue->front].process_cmd;
        queue->front++;

        if (queue->front >= queue->length) {
            queue->front -= queue->length;
        }
    }

    return 0;
}

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
static int set_status_recv_flag(blob_cli *blob_cli_ctx, uint32_t id)
{

    uint16_t index                                              = id / 8;
    blob_cli_ctx->blob_client_trans.sub_procedures.ack_set_flag = 0x01;

    if (index + 1 > (blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags_size + 7) >> 3) {
        BT_DBG("ID TOO BIGE %d", id);
        return 0;
    }

    if (index > blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index) {
        blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index = index;
    }

    // BT_DBG("Set id:%d %d\r\n",id,blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index);
    set_flag(blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags, CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE, id);
    return 0;
}
#endif

static void _blob_cli_clear_status_get_flag(struct bt_mesh_model *model)
{
    blob_cli *blob_cli_ctx = model->user_data;

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        blob_cli_ctx->receiver_list[i].status_get_flag = BLOB_CLI_SRV_STATUS_NOT_RECEIVE;
    }
}

static int get_need_buffer_size(uint8_t net_if, uint16_t data_size)
{
    extern int bt_mesh_get_tx_seg_size(uint8_t net_if, uint16_t size);
    uint8_t    total_need_size = 0;
    uint8_t    free_size       = 0;
    uint8_t    need_size;
    uint16_t   buffer_size = 0;
    bt_mesh_adv_free_get(&free_size, &buffer_size);
    total_need_size = bt_mesh_get_tx_seg_size(net_if, data_size);
    need_size       = free_size > total_need_size ? 0 : total_need_size - free_size;
    // BT_DBG("need total:%d free:%d need:%d",total_need_size,free_size,need_size);
    return need_size;
}

static int wait_adv_buffer(blob_cli *blob_cli_ctx, uint8_t need_count, uint32_t timeout)
{
    uint8_t  free_size   = 0;
    uint16_t buffer_size = 0;

    blob_cli_ctx->send_sem.cancel_flag = 0;

    if (need_count) {
        while (!blob_cli_ctx->send_sem.cancel_flag) {
            k_sem_take(&blob_cli_ctx->send_sem.sem, 300); // FOREVER
            bt_mesh_adv_free_get(&free_size, &buffer_size);

            if (free_size >= need_count) {
                return 0;
            }
        }

        return WAIT_CANCEL_FLAG;
    }

    return 0;
}

static int cancel_wait_adv_buffer(blob_cli *blob_cli_ctx)
{
    blob_cli_ctx->send_sem.cancel_flag = 1;
    return 0;
}

static int wait_send_end(blob_cli *blob_cli_ctx, uint8_t count)
{
    uint8_t  count_now = count;
    uint16_t size      = 0;

    blob_cli_ctx->send_end_sem.cancel_flag = 0;
    uint32_t  timeout                      = count * 300;
    long long time_start                   = aos_now_ms();

    while (count_now && !blob_cli_ctx->send_end_sem.cancel_flag && (aos_now_ms() - time_start) < timeout) {
        bt_mesh_adv_busy_get(&count_now, &size);

        if (!count_now) {
            break;
        }

        k_sem_take(&blob_cli_ctx->send_end_sem.sem, 200);
    }

    if (blob_cli_ctx->send_end_sem.cancel_flag) {
        return WAIT_CANCEL_FLAG;
    } else {
        return 0;
    }
}

static int cancel_wait_adv_buffer_end(blob_cli *blob_cli_ctx)
{
    blob_cli_ctx->send_end_sem.cancel_flag = 1;
    return 0;
}

static void dump_node_phase_and_status(blob_cli *blob_cli_ctx)
{
    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        BT_DBG("Node %04x,Phase %s,Trans status %s", blob_cli_ctx->receiver_list[i].address,
               phase_str(blob_cli_ctx->receiver_list[i].trans_phase),
               trans_status_str(blob_cli_ctx->receiver_list[i].status));
    }
}

static uint32_t _get_blob_timeout(uint16_t base, u8_t ttl)
{
    return 10 * (base + 2) + (100 * ttl); // s
}

static int get_max_try(uint8_t procedure)
{
    switch (procedure) {
        case BLOB_CLI_PROCEDURE_SET_ID:
            return CONFIG_MAX_BLOB_SET_ID_RETRY;

        case BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY:
            return CONFIG_MAX_BLOB_CAPABILITY_GET_RETRY;

        case BLOB_CLI_PROCEDURE_TRANSFER_BLOB:
            return CONFIG_MAX_BLOB_TRANS_START_SEND_RETRY;

        case BLOB_CLI_PROCEDURE_BLOCK_START:
            return CONFIG_MAX_BLOCK_BLOCK_START_SEND_RETRY;

        case BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET:
            return CONFIG_MAX_BLOCK_GET_SEND_RETRY;

        case BLOB_CLI_PROCEDURE_TRANSFER_CANCEL:
            return CONFIG_MAX_BLOB_TRANS_CANCEL_SEND_RETRY;

        default:
            return 0;
    }
}

static int block_get_procedure_process(struct bt_mesh_model *model)
{
    blob_cli *blob_cli_ctx = model->user_data;

    if (is_no_flag_set(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,
                       CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE, blob_cli_ctx->blob_cli_block_info.total_chunk)
            == true
        && blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_start_flag)
    {
        long long cost_time = aos_now_ms() - blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.start_time;
        BT_DBG("Block %d: Trans progress %d:%d,Try num %d,Cost %dms", blob_cli_ctx->blob_cli_block_info.block_num,
               blob_cli_ctx->blob_cli_block_info.total_chunk, blob_cli_ctx->blob_cli_block_info.total_chunk,
               blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num + 1, cost_time);
        (void)cost_time;
        goto next_block;
    } else { // some chunks missing
        // BT_DBG("Miss
        // Chunks:%s",bt_hex_real(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE));
        blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num++; // TRY NUM

        if (blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num >= CONFIG_MAX_BLOCK_SUB_SEND_RETRY) {
            goto next_block; // TODO Report fail node
        } else {
            return BLOB_CLI_PROCEDURE_SEND_SUB;
        }
    }

next_block:

    if (blob_cli_ctx->blob_cli_block_info.block_num == blob_cli_ctx->blob_cli_info.block_total - 1) {
        BT_DBG("Blob cli exit the trans");
        blob_cli_ctx->blob_client_trans.blob_status = BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE;
        return BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE;
    } else {
        BT_DBG("Blob cli start next block");
        return BLOB_CLI_PROCEDURE_BLOCK_START;
    }
}

static int get_procedure_next(struct bt_mesh_model *model, uint8_t procedure_now)
{
    blob_cli *blob_cli_ctx = model->user_data;

    switch (procedure_now) {
        case BLOB_CLI_PROCEDURE_SET_ID: {
            return BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY;
        } break;

        case BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY: {
            return blob_cli_ctx->blob_client_trans.trans_options & BLOB_TRANS_NO_INIT_TRANS_GET
                       ? BLOB_CLI_PROCEDURE_TRANSFER_BLOB
                       : BLOB_CLI_PROCEDURE_TRANSFER_GET;
        } break;

        case BLOB_CLI_PROCEDURE_TRANSFER_GET: { // TODO consider api case
            return BLOB_CLI_PROCEDURE_TRANSFER_BLOB;
        } break;

        case BLOB_CLI_PROCEDURE_TRANSFER_BLOB: {
            return BLOB_CLI_PROCEDURE_BLOCK_START;
        } break;

        case BLOB_CLI_PROCEDURE_BLOCK_START: {
            return BLOB_CLI_PROCEDURE_SEND_SUB;
        } break;

        case BLOB_CLI_PROCEDURE_SEND_SUB: {
            return BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET;
        } break;

        case BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET: {
            return block_get_procedure_process(model);
        } break;

        case BLOB_CLI_PROCEDURE_TRANSFER_CANCEL: {
            return BLOB_CLI_PROCEDURE_IDLE;
        } break;

        default:
            return BLOB_CLI_PROCEDURE_IDLE;
    }
}

static inline int start_procedure(uint8_t elem_id, uint8_t procedure)
{
    blob_cli_cmd cmd = {
        .elem_id     = elem_id,
        .process_cmd = procedure,
    };
    return blob_cli_queue_put(&cli_queue, cmd);
}

static uint8_t _blob_cli_get_next_block(struct bt_mesh_model *model)
{
    blob_cli *blob_cli_ctx = model->user_data;

    uint16_t block_num = blob_cli_ctx->blob_cli_block_info.block_num;

    if (block_num > blob_cli_ctx->blob_cli_info.block_total) {
        return READ_FINISH;
    }

    uint32_t data_read_offset = 0;

    if (block_num) {
        data_read_offset = block_num * blob_cli_ctx->blob_cli_info.block_total_size;
    }

    blob_cli_ctx->blob_cli_block_info.block_size
        = block_num == blob_cli_ctx->blob_cli_info.block_total - 1
              ? (blob_cli_ctx->blob_cli_info.blob_size - blob_cli_ctx->blob_cli_info.block_total_size * block_num)
              : blob_cli_ctx->blob_cli_info.block_total_size;
    blob_cli_ctx->blob_cli_block_info.chunk_size = blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size;
    blob_cli_ctx->blob_cli_block_info.data       = blob_cli_ctx->blob_cli_info.data + data_read_offset;
    blob_cli_ctx->blob_cli_block_info.total_chunk
        = (blob_cli_ctx->blob_cli_block_info.block_size + blob_cli_ctx->blob_cli_block_info.chunk_size - 1)
          / blob_cli_ctx->blob_cli_block_info.chunk_size;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_new_block_flag = 1;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.chunk_num           = 0;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_start_flag     = 0;
    blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.blocks_try_num[block_num]++;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num    = 0;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.start_time = aos_now_ms();

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) { // todo temp
        if (blob_cli_ctx->receiver_list[i].trans_phase != TRANSFER_PHASE_INACTIVE) {
            blob_cli_ctx->receiver_list[i].block_complete_flag = 0;
        }
    }

    clear_all_flags(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,
                    CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE, blob_cli_ctx->blob_cli_block_info.total_chunk);
    BT_DBG("Block Data prepare num:%d block size:%d chunk size:%d chunk total:%d %p", block_num,
           blob_cli_ctx->blob_cli_block_info.block_size, blob_cli_ctx->blob_cli_block_info.chunk_size,
           blob_cli_ctx->blob_cli_block_info.total_chunk, blob_cli_ctx->blob_cli_block_info.data);
    return READ_SUCCESS;
}

static void _blob_trans_init(struct bt_mesh_model *model)
{
    blob_cli *blob_cli_ctx                      = model->user_data;
    blob_cli_ctx->blob_cli_block_info.block_num = 0;
    blob_cli_ctx->blob_cli_info.block_size_log
        = blob_cli_ctx->blob_cli_info.blob_size < LOG_SIZE(blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log)
              ? GET_LOG(blob_cli_ctx->blob_cli_info.blob_size)
              : blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log;
    blob_cli_ctx->blob_cli_info.block_total_size = LOG_SIZE(blob_cli_ctx->blob_cli_info.block_size_log);
    blob_cli_ctx->blob_cli_info.block_total
        = (blob_cli_ctx->blob_cli_info.blob_size + blob_cli_ctx->blob_cli_info.block_total_size - 1)
          / blob_cli_ctx->blob_cli_info.block_total_size;
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_start_flag = 0;
    memset(blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.blocks_try_num, 0x00,
           CONFIG_BLOB_CLI_MAX_TOTAL_BLOCKS);
    clear_all_flags(blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.total_missing_blocks,
                    CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE, blob_cli_ctx->blob_cli_info.block_total);

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        clear_all_flags(blob_cli_ctx->receiver_list[i].missing_blocks, CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE,
                        blob_cli_ctx->blob_cli_info.block_total);
    }

    BT_DBG("Blob Data prepare:blob size:%d block size:%d block num:%d data:%p", blob_cli_ctx->blob_cli_info.blob_size,
           blob_cli_ctx->blob_cli_info.block_total_size, blob_cli_ctx->blob_cli_info.block_total,
           blob_cli_ctx->blob_cli_info.data);
}

static int _blob_cli_prepare_next_precodure(struct bt_mesh_model *model, uint8_t procedure_next)
{
    int       ret                                    = 0;
    blob_cli *blob_cli_ctx                           = model->user_data;
    blob_cli_ctx->blob_client_trans.blob_status_last = blob_cli_ctx->blob_client_trans.blob_status;
    blob_cli_ctx->blob_client_trans.blob_status      = procedure_next;
    BT_DBG("Procedure %s ---> %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status_last),
           procedure_str(procedure_next));
    dump_node_phase_and_status(blob_cli_ctx);
    cancel_wait_adv_buffer_end(blob_cli_ctx);

    if (blob_cli_ctx->blob_client_trans.blob_status_last == BLOB_CLI_PROCEDURE_TRANSFER_BLOB
        && blob_cli_ctx->blob_client_trans.cb.send_start_cb)
    {
        blob_cli_ctx->blob_client_trans.cb.send_start_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                         blob_cli_ctx->blob_cli_info.blob_id, 0, NULL);
    }

    if (procedure_next == BLOB_CLI_PROCEDURE_BLOCK_START) {
        ret = _blob_cli_get_next_block(model);
    }

    if (ret == READ_FINISH) {
        BT_DBG("Read finish");
        return 0;
    } else if (ret == READ_FAIL) {
        BT_ERR("Read fail");
        return -EIO;
    } else if (procedure_next == BLOB_CLI_PROCEDURE_TRANSFER_BLOB) {
        _blob_trans_init(model);
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    _blob_cli_clear_status_get_flag(model);
    blob_cli_ctx->blob_client_trans.sub_procedures.ack_set_flag      = 0x00;
    blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index = 0x00;
    blob_cli_ctx->blob_client_trans.sub_procedures.ack_set_status    = ACK_STATUS_RECV_NULL;
    clear_all_flags(blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags, CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE,
                    blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags_size);

    if (procedure_next == BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET
        && blob_cli_ctx->blob_client_trans.blob_status_last == BLOB_CLI_PROCEDURE_SEND_SUB
        && blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num > 1)
    {
        for (int i = 0; i < blob_cli_ctx->receiver_size; i++) { // todo temp
            if (blob_cli_ctx->receiver_list[i].trans_phase != TRANSFER_PHASE_INACTIVE
                && blob_cli_ctx->receiver_list[i].block_complete_flag == 1)
            {
                BT_DBG("Node %04x block receive complete no need ack", blob_cli_ctx->receiver_list[i].address);
                set_status_recv_flag(blob_cli_ctx, blob_cli_ctx->receiver_list[i].id);
            }
        }
    }

#endif
    blob_cli_ctx->cli_try_num = 0;
    return start_procedure(blob_cli_ctx->blob_client_trans.elem_id, procedure_next);
}

static inline struct bt_mesh_model *get_blob_cli_model(uint8_t elem_id)
{
    return ble_mesh_model_find(elem_id, BT_MESH_MODEL_ID_BLOB_CLI, CID_NVAL);
}

static inline blob_receiver *get_blob_receiver(struct bt_mesh_model *model, uint16_t dst_addr)
{
    u16_t     node_index   = 0;
    blob_cli *blob_cli_ctx = model->user_data;

    for (node_index = 0; node_index < blob_cli_ctx->receiver_size; node_index++) {
        if (blob_cli_ctx->receiver_list[node_index].address == dst_addr) {
            return &blob_cli_ctx->receiver_list[node_index];
        }
    }

    if (node_index == blob_cli_ctx->receiver_size) {
        return NULL;
    }
    return NULL;
}

static uint8_t get_node_status(uint8_t procedure)
{
    switch (procedure) { // TODO
        default:
            return TRANSFER_STATUS_INTERNAL_ERROR;
    }
}

void _send_end(int ret, void *cb_data)
{
    blob_cli *blob_cli_ctx = cb_data;

    if (blob_cli_ctx) {
        k_sem_give(&blob_cli_ctx->send_sem.sem);
        k_sem_give(&blob_cli_ctx->send_end_sem.sem);
    }
}

static struct bt_mesh_send_cb send_cb = {
    .start = NULL,
    .end   = _send_end,
};

static int _mesh_blob_send(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, uint8_t ttl,
                           struct bt_mesh_model *model, struct net_buf_simple *msg, uint32_t opcode, uint8_t trans,
                           uint8_t *data, uint16_t len, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    int                    ret;
    blob_cli *             blob_cli_ctx = model->user_data;
    struct bt_mesh_msg_ctx ctx          = { 0 };
    uint8_t                try_num      = 0;
    uint16_t               need_size    = 0;
    uint8_t                free_count   = 0;
    uint16_t               buffer_size  = 0;

    if ((0x0000 == dst_addr) || (!data && len)) {
        return -EINVAL;
    }

send:

    need_size = get_need_buffer_size(trans, len + 4);
    bt_mesh_adv_free_get(&free_count, &buffer_size);

    // BT_DBG("total len %d Free: %d,need: %d",len + 4,free_count,need_size);
    if (free_count < need_size) {
        ret = wait_adv_buffer(blob_cli_ctx, need_size, K_FOREVER);

        if (ret == WAIT_CANCEL_FLAG) {
            BT_DBG("Buffer waiting cancel");
            return 0;
        }
    }

    bt_mesh_model_msg_init(msg, opcode);

    if (data) {
        net_buf_simple_add_mem(msg, data, len);
    }

    if (!BT_MESH_ADDR_IS_UNICAST(dst_addr)) { // TODO SET INACTIVE NODE to flag 1?
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
        if (blob_cli_ctx && blob_cli_ctx->blob_client_trans.sub_procedures.ack_set_flag) {
            if (blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index + 1
                    == (blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags_size + 1) >> 3
                && is_no_flag_set(blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags,
                                  CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE,
                                  blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags_size)
                       == true)
            {
                net_buf_simple_add_u8(msg, ACK_STATUS_RECV_ALL);
            } else {
                net_buf_simple_add_u8(msg, ACK_STATUS_RECV_SOME);
                net_buf_simple_add_mem(msg, blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags,
                                       blob_cli_ctx->blob_client_trans.sub_procedures.ack_max_set_index + 1);
            }
        }

#endif
    }

    ctx.addr         = dst_addr;
    ctx.net_idx      = netkey_idx;
    ctx.app_idx      = appkey_idx;
    ctx.trans        = trans;
    ctx.send_ttl     = ttl ? ttl : BLOB_CLI_DEFAULT_TTL;
    ctx.net_transmit = BT_MESH_TRANSMIT(6, 20);

    ret = bt_mesh_model_send(model, &ctx, msg, &send_cb, blob_cli_ctx);

    if (ret) {
        BT_ERR("Blob send fail %d", ret);

        if (ret == -ENOBUFS && try_num <= 5) {
            try_num++;
            net_buf_simple_reset(msg);
            goto send;

        } else {
            return ret;
        }
    }

    return 0;
}

static void set_and_report_node_fail(blob_cli *blob_cli_ctx, blob_receiver *receiver, uint8_t procedure, uint8_t status)
{

    BT_DBG("set node %04x fail in procedure %s,node phase %s, trans status %s", receiver->address,
           procedure_str(procedure), phase_str(receiver->trans_phase), trans_status_str(status));
    receiver->active_status = 0;
    receiver->status        = status;

    if (blob_cli_ctx->blob_client_trans.cb.node_status_cb) {
        blob_cli_ctx->blob_client_trans.cb.node_status_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                          blob_cli_ctx->blob_cli_info.blob_id, receiver->address,
                                                          receiver->trans_phase, status);
    }

    receiver->trans_phase = TRANSFER_PHASE_INACTIVE;
}

static uint8_t _blob_cli_get_all_node_status_get_flag_status(struct bt_mesh_model *model, uint8_t dst_phase,
                                                             uint8_t report_fail_flag)
{
    uint16_t found_fail_flag        = 0;
    uint16_t found_success_flag     = 0;
    uint16_t found_not_receive_flag = 0;
    uint16_t unsed_node_num         = 0;

    blob_cli *blob_cli_ctx = model->user_data;
    BT_DBG("All node status get cb, Procedure now %s, Dst phase %s, Report fail flag %02x",
           procedure_str(blob_cli_ctx->blob_client_trans.blob_status),
           (dst_phase == DST_PHASE_UNSET ? "UNSET" : phase_str(dst_phase)), report_fail_flag);

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        if (blob_cli_ctx->receiver_list[i].trans_phase == TRANSFER_PHASE_INACTIVE
            || ((dst_phase != DST_PHASE_UNSET) && (blob_cli_ctx->receiver_list[i].trans_phase != dst_phase)))
        {
            unsed_node_num++;
            continue;
        }

        if (blob_cli_ctx->receiver_list[i].status_get_flag == BLOB_CLI_SRV_STATUS_RECV_FAILED) {
            found_fail_flag++;

            if (report_fail_flag && blob_cli_ctx->receiver_list[i].trans_phase != TRANSFER_PHASE_COMPLETE) {
                set_and_report_node_fail(blob_cli_ctx, &blob_cli_ctx->receiver_list[i],
                                         blob_cli_ctx->blob_client_trans.blob_status,
                                         blob_cli_ctx->receiver_list[i].status);
            }
        } else if (blob_cli_ctx->receiver_list[i].status_get_flag == BLOB_CLI_SRV_STATUS_NOT_RECEIVE) {
            found_not_receive_flag++;

            if (report_fail_flag && blob_cli_ctx->receiver_list[i].trans_phase != TRANSFER_PHASE_COMPLETE) {
                set_and_report_node_fail(blob_cli_ctx, &blob_cli_ctx->receiver_list[i],
                                         blob_cli_ctx->blob_client_trans.blob_status,
                                         get_node_status(blob_cli_ctx->blob_client_trans.blob_status));
            }
        } else {
            found_success_flag++;
        }

        if (!report_fail_flag && found_not_receive_flag && found_success_flag && found_fail_flag) {
            return PROCEDURE_STATUS_SOME_RECEIVE_SOME_FAIL;
        }
    }

    if (found_fail_flag + found_success_flag + unsed_node_num == blob_cli_ctx->receiver_size) {
        if (found_fail_flag) {
            return found_success_flag ? PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL : PROCEDURE_STATUS_ALL_RECEIVE_ALL_FAIL;
        } else {
            return PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS;
        }
    }

    if (found_not_receive_flag && (found_fail_flag || found_success_flag)) {
        if (found_fail_flag) {
            return found_success_flag ? PROCEDURE_STATUS_SOME_RECEIVE_SOME_FAIL
                                      : PROCEDURE_STATUS_SOME_RECEIVE_ALL_FAIL;
        } else {
            return PROCEDURE_STATUS_SOME_RECEIVE_ALL_SUCCESS;
        }
    }

    if (unsed_node_num == blob_cli_ctx->receiver_size) {
        return PROCEDURE_STATUS_NO_NEED_RECEIVE;
    }

    if (found_not_receive_flag + unsed_node_num == blob_cli_ctx->receiver_size) {
        return PROCEDURE_STATUS_NO_RECEIVE;
    }

    return PROCEDURE_STATUS_NO_RECEIVE;
}

static int _blob_cli_exist_trans(struct bt_mesh_model *model, uint8_t status)
{
    blob_cli *blob_cli_ctx = model->user_data;
    k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
    k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
    BT_DBG("Exist the trans procedure now %s %02x", procedure_str(blob_cli_ctx->blob_client_trans.blob_status), status);
    exist_data data = {
        .procedure = blob_cli_ctx->blob_client_trans.blob_status,
        .status    = status,
    };

    if (blob_cli_ctx->blob_client_trans.cb.send_end_cb) {
        blob_cli_ctx->blob_client_trans.cb.send_end_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                       blob_cli_ctx->blob_cli_info.blob_id, 0, &data);
    }

    uint32_t offset = (size_t) & (((blob_cli *)0)->cli_procedure_timer);
    memset(blob_cli_ctx, 0x00, offset);
    return 0;
}

int report_all_blob_srv_status(blob_cli *blob_cli_ctx, uint8_t procedure, uint8_t status)
{
    struct net_buf_simple temp_buffer = { 0x0 };
    uint8_t               data[2]     = { 0x00 };
    temp_buffer.len                   = 0; // TODO
    temp_buffer.data                  = data;
    temp_buffer.__buf                 = data;
    temp_buffer.size                  = 2;
    net_buf_simple_add_u8(&temp_buffer, procedure);
    net_buf_simple_add_u8(&temp_buffer, status);
    model_message message = { 0 };
    message.trans         = 0;
    message.source_addr   = bt_mesh_primary_addr() + blob_cli_ctx->blob_client_trans.elem_id;
    message.status_data   = &temp_buffer;
    message.user_data     = blob_cli_ctx;
    model_event(OP_BLOB_ALL_SRV_PROCEDURE_STATUS, (void *)&message);
    return 0;
}

static void _blob_cli_id_set_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                    struct net_buf_simple *buf)
{
    blob_cli *blob_cli_ctx = model->user_data;
    BT_DBG("Blob id set status cb,Procedure now %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_SET_ID) {
        BT_DBG("Blob status %02x ignore the message", blob_cli_ctx->blob_client_trans.blob_status);
        return;
    }

    blob_receiver *receiver             = NULL;
    int            ret                  = 0;
    uint8_t        set_status           = 0;
    uint8_t        send_all_status_flag = 0;
    uint8_t        status_flag          = 0;
    // uint8_t procedure_before = 0;
    struct net_buf_simple_state state;
    model_message               message = { 0 };

    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);
    net_buf_simple_save(buf, &state);
    receiver = get_blob_receiver(model, ctx->addr);

    if (!receiver) {
        BT_INFO("Blob Receiver %04x not exist in the list", ctx->addr);
        net_buf_simple_restore(buf, &state);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    set_status_recv_flag(blob_cli_ctx, receiver->id);
#endif

    if (receiver->status_get_flag == BLOB_CLI_SRV_STATUS_RECV_SUCCESS) {
        BT_DBG("Status already received");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    set_status = buf->data[0];

    if (set_status) {
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;

        if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_SET_ID) {
            receiver->status = TRANSFER_STATUS_SET_ID_FAILED;
        } else {
            receiver->status = TRANSFER_STATUS_DEL_ID_FAILED;
        }
    } else {
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_SUCCESS;
        receiver->status          = TRANSFER_STATUS_SUCCESS;
    }

    status_flag = _blob_cli_get_all_node_status_get_flag_status(model, DST_PHASE_UNSET, 0);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL
        || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS) {
        k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
        cancel_wait_adv_buffer(blob_cli_ctx);
        ret = _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY);

        if (ret) {
            _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_FAIL);
        }

        send_all_status_flag = 1;
    }

    net_buf_simple_restore(buf, &state);
    message.trans       = ctx->trans;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    message.user_data   = blob_cli_ctx;
    model_event(BT_MESH_MODEL_BLOB_SET_STATUS, (void *)&message);

    if (send_all_status_flag) {
        report_all_blob_srv_status(blob_cli_ctx, blob_cli_ctx->blob_client_trans.blob_status, status_flag);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
}

static void _blob_cli_trans_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    blob_cli *     blob_cli_ctx         = model->user_data;
    blob_receiver *receiver             = NULL;
    uint16_t       index                = 0;
    uint8_t        flags                = 0;
    uint8_t        procedure_next       = 0;
    uint8_t        status_flag          = 0;
    uint8_t        send_all_status_flag = 0;
    uint8_t        procedure_before     = 0;

    BT_DBG("Blob trans status cb,Procedure now %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));

    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);

    receiver = get_blob_receiver(model, ctx->addr);

    if (!receiver) {
        BT_INFO("Blob Receiver %04x not exist in the list", ctx->addr);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    if (receiver->status_get_flag == BLOB_CLI_SRV_STATUS_RECV_SUCCESS) {
        BT_DBG("Status already received\r\n");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_TRANSFER_GET
        && blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_TRANSFER_BLOB
        && blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_TRANSFER_CANCEL
        && blob_cli_ctx->blob_client_trans.blob_sec_status != BLOB_CLI_PROCEDURE_TRANSFER_CANCEL)
    {
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        BT_DBG("Ignore the trans status message");
        return;
    }

    struct net_buf_simple_state state;

    net_buf_simple_save(buf, &state);

    u8_t hdr = net_buf_simple_pull_u8(buf);

    u8_t status = (hdr & 0xF0) >> 4;

    u8_t trans_mode = (hdr & 0x03);

    u8_t trans_phase = net_buf_simple_pull_u8(buf);

    u8_t get_missing_blocks_flags = 0;

    model_message message = { 0 };

    if (status != TRANSFER_STATUS_SUCCESS && receiver->active_status == BLOB_RECEIVER_INACTIVE) {
        BT_INFO("Inactive Blob srv %04x status %02x", ctx->addr, status);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
        return;
    }

    if (trans_mode != blob_cli_ctx->blob_cli_trans_mode) {
        BT_ERR("Invalid trans mode");
        receiver->active_status = BLOB_RECEIVER_INACTIVE;
        receiver->status        = TRANSFER_STATUS_UNSUPPORTED_TRANSFER_MODE;
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
        return;
    }

    receiver->trans_phase = trans_phase;

    if (buf->len > 0) {
        u64_t blob_id = net_buf_simple_pull_le64(buf);

        if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) { // TODO
            BT_ERR("Invalid blob id");
            receiver->active_status   = BLOB_RECEIVER_INACTIVE;
            receiver->status          = TRANSFER_STATUS_WRONG_BLOB_ID;
            receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
            k_mutex_unlock(&blob_cli_ctx->cli_mutex);
            return;
        }
    }

    if (buf->len > 0) {
        u32_t blob_size = net_buf_simple_pull_le32(buf);

        if (blob_size != blob_cli_ctx->blob_cli_info.blob_size) { // TODO
            BT_ERR("Invalid blob size");
            receiver->active_status   = BLOB_RECEIVER_INACTIVE;
            receiver->status          = TRANSFER_STATUS_BLOB_TOO_LARGE; //
            receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
            k_mutex_unlock(&blob_cli_ctx->cli_mutex);
            return;
        }

        u16_t block_size_log = net_buf_simple_pull_le16(buf);

        if ((blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log
             && block_size_log > blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log)
            || block_size_log > CONFIG_BLOB_CLI_MAX_BLOCK_SIZE_LOG
            || (blob_cli_ctx->blob_client_trans.srv_max_min_block_size_log
                && block_size_log < blob_cli_ctx->blob_client_trans.srv_max_min_block_size_log)
            || block_size_log < CONFIG_BLOB_CLI_MIN_BLOCK_SIZE_LOG)
        {
            BT_ERR("Invalid block size :%d", block_size_log);
            receiver->active_status   = BLOB_RECEIVER_INACTIVE;
            receiver->status          = TRANSFER_STATUS_INVALID_BLOCK_SIZE;
            receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
            k_mutex_unlock(&blob_cli_ctx->cli_mutex);
            return;
        }

        u16_t srv_mtu_size = net_buf_simple_pull_le16(buf);

        if ((blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size
             && srv_mtu_size > blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size)
            || srv_mtu_size > CONFIG_BLOB_CLI_MTU_SIZE)
        { // TODO
            BT_ERR("Invalid mtu size size srv:%d cli:%d", srv_mtu_size, CONFIG_BLOB_CLI_MTU_SIZE);
            receiver->active_status   = BLOB_RECEIVER_INACTIVE;
            receiver->status          = TRANSFER_STATUS_INVALID_CHUNK_SIZE;
            receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
            k_mutex_unlock(&blob_cli_ctx->cli_mutex);
            return;
        }
    }

    while (buf->len > 0) {
        if (index >= CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE) { // TODO
            BT_ERR("Invalid missing blocks size size");
            k_mutex_unlock(&blob_cli_ctx->cli_mutex);
            return;
        }

        flags = net_buf_simple_pull_u8(buf);
        receiver->missing_blocks[index] |= flags;
        blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.total_missing_blocks[index] |= flags;
        index++;
        get_missing_blocks_flags = 1;
    }

    BT_DBG("Set node:%04x phase:%s status:%s", ctx->addr, phase_str(trans_phase), trans_status_str(status));

    if (status == TRANSFER_STATUS_SUCCESS) {
        receiver->active_status = BLOB_RECEIVER_ACTIVE;

        if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_BLOB
            && !get_missing_blocks_flags) {
            set_all_flags(receiver->missing_blocks, CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE,
                          blob_cli_ctx->blob_cli_info.block_total);
        }
    } else {
        receiver->active_status = BLOB_RECEIVER_INACTIVE;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    set_status_recv_flag(blob_cli_ctx, receiver->id);
#endif

    if (status == TRANSFER_STATUS_SUCCESS) {
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_SUCCESS;
    } else {
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
    }

    uint8_t get_flag_dst_phase = DST_PHASE_UNSET;

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL
        || blob_cli_ctx->blob_client_trans.blob_sec_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL)
    {
        get_flag_dst_phase = TRANSFER_PHASE_WAITING_CANCEL;
    }

    status_flag = _blob_cli_get_all_node_status_get_flag_status(model, get_flag_dst_phase, 0);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL
        || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS) {
        BT_DBG("All node recv trans status %02x,stop direct", status_flag);
        k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
        cancel_wait_adv_buffer(blob_cli_ctx);

        if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_BLOB) {
            if (blob_cli_ctx->blob_client_trans.cb.send_start_cb) {
                blob_cli_ctx->blob_client_trans.cb.send_start_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                                 blob_cli_ctx->blob_cli_info.blob_id, 0, NULL);
            }

            procedure_next                                   = BLOB_CLI_PROCEDURE_BLOCK_START;
            blob_cli_ctx->blob_client_trans.blob_status_last = blob_cli_ctx->blob_client_trans.blob_status;
            _blob_cli_prepare_next_precodure(model, procedure_next);
        } else if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_GET) {
            blob_cli_ctx->blob_client_trans.blob_status = BLOB_CLI_PROCEDURE_IDLE;
        } else if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL
                   || blob_cli_ctx->blob_client_trans.blob_sec_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL)
        {
            k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
#if 0

            if (blob_cli_ctx->blob_client_trans.cb.node_status_cb) {
                blob_cli_ctx->blob_client_trans.cb.node_status_cb(blob_cli_ctx->blob_client_trans.elem_id, blob_cli_ctx->blob_cli_info.blob_id, receiver->address, TRANSFER_PHASE_INACTIVE, TRANSFER_STATUS_SUCCESS);
            }

#endif
            send_all_status_flag = 1;
        }

        procedure_before = blob_cli_ctx->blob_client_trans.blob_status;
    }

    net_buf_simple_restore(buf, &state);
    message.trans       = ctx->trans;
    message.source_addr = ctx->addr;
    message.status_data = buf;
    message.user_data   = blob_cli_ctx;
    model_event(BT_MESH_MODEL_BLOB_TRANS_STATUS, (void *)&message);

    if (send_all_status_flag) {
        report_all_blob_srv_status(blob_cli_ctx, procedure_before, status_flag);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
}

static int set_blob_receiver_capability(struct bt_mesh_model *model, blob_receiver *receiver, u16_t min_block_size_log,
                                        u16_t max_block_size_log, u16_t max_total_chunks, u16_t max_chunk_size,
                                        u32_t max_blob_size, u16_t srv_mtu_size, u8_t support_trans_mode)
{

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_cli_ctx->blob_cli_trans_mode != support_trans_mode) {
        BT_ERR("Cli support trans mode %02x but srv support mode %02x", blob_cli_ctx->blob_cli_trans_mode,
               support_trans_mode);
        receiver->status          = TRANSFER_STATUS_UNSUPPORTED_TRANSFER_MODE;
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
        return -1;
    }

    receiver->capability.min_block_size_log = min_block_size_log;
    receiver->capability.max_block_size_log = max_block_size_log;
    receiver->capability.max_total_chunks   = max_total_chunks;
    receiver->capability.max_chunk_size     = max_chunk_size;
    receiver->capability.max_blob_size      = max_blob_size;
    receiver->capability.server_mtu_size    = srv_mtu_size;
    receiver->capability.support_mode       = support_trans_mode;
    receiver->trans_phase                   = TRANSFER_PHASE_WAITING_FOR_START;
    receiver->status_get_flag               = BLOB_CLI_SRV_STATUS_RECV_SUCCESS;
    BT_DBG("Srv Node %04x info:min block size log:%d;max block size log:%d;max chunks:%d;max chunk size:%d;max blob "
           "size:%d;mtu:%d;trans mode:%02x",
           receiver->address, min_block_size_log, max_block_size_log, max_total_chunks, max_chunk_size, max_blob_size,
           srv_mtu_size, support_trans_mode);
    return 0;
}

static int update_blob_cli_capability(struct bt_mesh_model *model, blob_receiver *receiver, u16_t min_block_size_log,
                                      u16_t max_block_size_log, u16_t max_total_chunks, u16_t max_chunk_size,
                                      u32_t max_blob_size, u16_t srv_mtu_size, u8_t support_trans_mode)
{
    blob_cli *blob_cli_ctx = model->user_data;

    if (!blob_cli_ctx->blob_client_trans.srv_min_capability_set_flag) {
        blob_cli_ctx->blob_client_trans.srv_max_min_block_size_log = min_block_size_log;
        blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log = max_block_size_log;

        if (blob_cli_ctx->blob_client_trans.srv_min_max_total_chunks > max_total_chunks) {
            blob_cli_ctx->blob_client_trans.srv_min_max_total_chunks = max_total_chunks;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size > max_chunk_size) {
            blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size = max_chunk_size;
        }

        blob_cli_ctx->blob_client_trans.srv_min_max_blob_size = max_blob_size;

        if (blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size > srv_mtu_size) {
            blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size = srv_mtu_size;
        }

        blob_cli_ctx->blob_client_trans.srv_min_capability_set_flag = 1;
    } else {
        if (blob_cli_ctx->blob_client_trans.srv_max_min_block_size_log < min_block_size_log) {
            blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log = min_block_size_log;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log > max_block_size_log) {
            blob_cli_ctx->blob_client_trans.srv_min_max_block_size_log = max_block_size_log;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_max_total_chunks > max_total_chunks) {
            blob_cli_ctx->blob_client_trans.srv_min_max_total_chunks = max_total_chunks;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size > max_chunk_size) {
            blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size = max_chunk_size;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_max_blob_size > max_blob_size) {
            blob_cli_ctx->blob_client_trans.srv_min_max_blob_size = max_blob_size;
        }

        if (blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size > srv_mtu_size) {
            blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size = srv_mtu_size;
        }
    }

    return 0;
}

static void _blob_cli_srv_info_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                                      struct net_buf_simple *buf)
{
    blob_cli *blob_cli_ctx = model->user_data;
    BT_DBG("Blob trans info status cb,Procedure now %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY) {
        BT_DBG("Blob status %02x ignore the message", blob_cli_ctx->blob_client_trans.blob_status);
        return;
    }

    blob_receiver *receiver             = NULL;
    int            ret                  = 0;
    uint8_t        send_all_status_flag = 0;

    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);
    receiver = get_blob_receiver(model, ctx->addr);

    if (!receiver) {
        BT_INFO("Blob Receiver %04x not exist in the list", ctx->addr);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    if (receiver->trans_phase == TRANSFER_PHASE_INACTIVE) {
        BT_INFO("Blob receiver %04x is inactive,ignore the message", receiver->address);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    set_status_recv_flag(blob_cli_ctx, receiver->id);
#endif

    if (receiver->status_get_flag == BLOB_CLI_SRV_STATUS_RECV_SUCCESS) {
        BT_DBG("Status already received");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    struct net_buf_simple_state state;

    net_buf_simple_save(buf, &state);

    u16_t min_block_size_log = net_buf_simple_pull_le16(buf);

    u16_t max_block_size_log = net_buf_simple_pull_le16(buf);

    u16_t max_total_chunks = net_buf_simple_pull_le16(buf);

    u16_t max_chunk_size = net_buf_simple_pull_le16(buf);

    u32_t max_blob_size = net_buf_simple_pull_le32(buf);

    u16_t srv_mtu_size = net_buf_simple_pull_le16(buf);

    u8_t support_trans_mode = net_buf_simple_pull_u8(buf);

    ret = set_blob_receiver_capability(model, receiver, min_block_size_log, max_block_size_log, max_total_chunks,
                                       max_chunk_size, max_blob_size, srv_mtu_size, support_trans_mode);

    if (ret) {
        BT_ERR("set blob capability failed %d", ret);
    } else {
        update_blob_cli_capability(model, receiver, min_block_size_log, max_block_size_log, max_total_chunks,
                                   max_chunk_size, max_blob_size, srv_mtu_size, support_trans_mode);
    }

    uint8_t status_flag = _blob_cli_get_all_node_status_get_flag_status(model, DST_PHASE_UNSET, 0);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL
        || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS) {
        k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
        cancel_wait_adv_buffer(blob_cli_ctx);
        ret = _blob_cli_prepare_next_precodure(model,
                                               get_procedure_next(model, blob_cli_ctx->blob_client_trans.blob_status));

        if (ret) {
            _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_FAIL);
        }

        send_all_status_flag = 1;
    }

    // TODO START THE NEXT procedure if all node success
    net_buf_simple_restore(buf, &state);
    model_message message = { 0 };
    message.trans         = ctx->trans;
    message.source_addr   = ctx->addr;
    message.status_data   = buf;
    message.user_data     = blob_cli_ctx;
    model_event(BT_MESH_MODEL_BLOB_INFO_STATUS, (void *)&message);

    if (send_all_status_flag) {
        report_all_blob_srv_status(blob_cli_ctx, blob_cli_ctx->blob_client_trans.blob_status_last, status_flag);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
}

static void _blob_cli_block_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
    blob_cli *     blob_cli_ctx   = model->user_data;
    blob_receiver *receiver       = NULL;
    uint8_t        status_flag    = 0;
    uint8_t        procedure_next = 0;
    uint8_t        procedure_now  = blob_cli_ctx->blob_client_trans.blob_status;

    BT_DBG("Blob block status cb,Procedure now %s", procedure_str(procedure_now));

    if (procedure_now != BLOB_CLI_PROCEDURE_BLOCK_START && procedure_now != BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET) {
        BT_DBG("Blob status %02x ignore the message", blob_cli_ctx->blob_client_trans.blob_status);
        return;
    }

    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);
    receiver = get_blob_receiver(model, ctx->addr);

    if (!receiver) {
        BT_INFO("Blob Receiver %04x not exist in the list", ctx->addr);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    if (receiver->trans_phase == TRANSFER_PHASE_INACTIVE) {
        BT_INFO("Blob receiver %04x is inactive,ignore the message", receiver->address);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    if (receiver->status_get_flag == BLOB_CLI_SRV_STATUS_RECV_SUCCESS) {
        BT_DBG("Status already received\r\n");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    struct net_buf_simple_state state;

    net_buf_simple_save(buf, &state);

    uint8_t hdr = net_buf_simple_pull_u8(buf);

    uint8_t status = (hdr & 0xF0) >> 0x04;

    uint8_t format = (hdr & 0x03);

    uint16_t block_num = net_buf_simple_pull_le16(buf);

    uint16_t chunk_size = net_buf_simple_pull_le16(buf);
    (void)chunk_size;
    if (block_num != blob_cli_ctx->blob_cli_block_info.block_num) {
        BT_DBG("The send block num is %d,but the received is:%d", blob_cli_ctx->blob_cli_block_info.block_num,
               block_num);
        net_buf_simple_restore(buf, &state);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    set_status_recv_flag(blob_cli_ctx, receiver->id);
#endif

    if (status == TRANSFER_STATUS_SUCCESS) {
        // BT_DBG("Miss Chunks
        // before:%s",bt_hex_real(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE));
        if (format == FORMAT_SOME_CHUNKS_MISSING && buf->len > 0) {
            uint16_t missing_chunks_index = 0;

            while (buf->len > 0) {
                uint8_t chunk_flag = net_buf_simple_pull_u8(buf);
                receiver->missing_chunks[missing_chunks_index] |= chunk_flag;
                blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks[missing_chunks_index]
                    |= chunk_flag;
                missing_chunks_index++;
            }

            // BT_DBG("Miss Chunks
            // after:%s",bt_hex_real(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,(blob_cli_ctx->blob_cli_block_info.total_chunk
            // + 7) / 8));
        } else if (format == FORMAT_ALL_CHUNKS_MISSING) {
            if (blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.blocks_try_num[block_num] >= 2) {
                set_all_flags(receiver->missing_chunks, CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE,
                              blob_cli_ctx->blob_cli_block_info.total_chunk);
                set_all_flags(blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,
                              CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE, blob_cli_ctx->blob_cli_block_info.total_chunk);
            }

            if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_BLOCK_START) {
                receiver->trans_phase = TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK;
            }
        } else if (format == FORMAT_NO_CHUNKS_MISSING) {
            clear_flag(receiver->missing_blocks, CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE, block_num);
            receiver->block_complete_flag = 1;
            receiver->trans_phase         = TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK;

            if (is_no_flag_set(receiver->missing_blocks, CONFIG_BLOB_CLI_MAX_MISS_BLOCKS_FLAG_SIZE,
                               blob_cli_ctx->blob_cli_info.block_total))
            {
                receiver->trans_phase = TRANSFER_PHASE_COMPLETE;
                receiver->status      = TRANSFER_STATUS_SUCCESS;

                if (blob_cli_ctx->blob_client_trans.cb.node_status_cb) {
                    blob_cli_ctx->blob_client_trans.cb.node_status_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                                      blob_cli_ctx->blob_cli_info.blob_id, ctx->addr,
                                                                      TRANSFER_PHASE_COMPLETE, TRANSFER_STATUS_SUCCESS);
                }
            }
        }

        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_SUCCESS;
    } else {
        receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_FAILED;
    }

    uint8_t dst_phase = 0;

    if (procedure_now == BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET) {
        dst_phase = TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK;
    } else {
        dst_phase = DST_PHASE_UNSET;
    }

    status_flag = _blob_cli_get_all_node_status_get_flag_status(model, dst_phase, 0);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS
        || status_flag == PROCEDURE_STATUS_NO_NEED_RECEIVE)
    {
        BT_DBG("All node recv block status %02x,stop direct", status_flag);
        k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
        cancel_wait_adv_buffer(blob_cli_ctx);
        report_all_blob_srv_status(blob_cli_ctx, procedure_now, status_flag);
        goto next_procedure;
    }

    net_buf_simple_restore(buf, &state);
    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
    return;

next_procedure:
    procedure_next = get_procedure_next(model, procedure_now);

    if (procedure_next == BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE) {
        _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_CUCCESS);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    } else if (procedure_next == BLOB_CLI_PROCEDURE_IDLE) {
        BT_DBG("procedure next state idle no need process");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    _blob_cli_prepare_next_precodure(model, procedure_next);
    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
}

int ble_mesh_blob_set_id(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                         blob_set_id_message *message, uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb,
                         void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg
        = NET_BUF_SIMPLE(2 + sizeof(blob_set_id_message) + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + sizeof(blob_set_id_message) + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_TRANS_SET_ID, trans,
                           (uint8_t *)message, sizeof(blob_set_id_message), cb, cb_data);
}

int ble_mesh_blob_del_id(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                         blob_del_id_message *message, uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb,
                         void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg
        = NET_BUF_SIMPLE(2 + sizeof(blob_del_id_message) + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + sizeof(blob_del_id_message) + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_TRANS_DEL_ID, trans,
                           (uint8_t *)message, sizeof(blob_del_id_message), cb, cb_data);
}

/*Retrieve capablilities procedure*/
int ble_mesh_blob_info_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                           uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb, void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_INFO_GET, trans, NULL, 0, cb,
                           cb_data);
}

/*Transfer blob procedure*/
int ble_mesh_blob_transfer_start(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr,
                                 struct bt_mesh_model *model, blob_trans_start_message *message, uint8_t trans,
                                 uint8_t ttl, const struct bt_mesh_send_cb *cb, void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg
        = NET_BUF_SIMPLE(2 + sizeof(blob_trans_start_message) + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + sizeof(blob_trans_start_message) + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_TRANS_START, trans,
                           (uint8_t *)message, sizeof(blob_trans_start_message), cb, cb_data);
}

/*Send block sub-procedure*/
int ble_mesh_blob_block_start(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                              blob_block_start_message *message, uint8_t trans, uint8_t ttl,
                              const struct bt_mesh_send_cb *cb, void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg
        = NET_BUF_SIMPLE(2 + sizeof(blob_block_start_message) + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + sizeof(blob_block_start_message) + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_BLOCK_START, trans,
                           (uint8_t *)message, sizeof(blob_block_start_message), cb, cb_data);
}

/*Send data sub-procedure*/
int ble_mesh_blob_block_chunk_send(blob_cli *blob_cli_ctx, uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr,
                                   struct bt_mesh_model *model, blob_block_chunk_message *message, uint16_t chunk_size,
                                   uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb, void *cb_data)
{
    int ret;

    struct net_buf_simple *msg         = NET_BUF_SIMPLE(2 + MAX_BLOB_NUM + 4); // TODO set staic data?
    uint8_t                try_num     = 0;
    uint16_t               need_size   = 0;
    uint8_t                free_count  = 0;
    uint16_t               buffer_size = 0;
    uint8_t                busy_cout   = 0;

send:
    bt_mesh_adv_free_get(&free_count, &buffer_size);
    need_size = get_need_buffer_size(trans, (4 + 4 + chunk_size));

    // BT_DBG("total len %d Free: %d,need: %d",chunk_size + 8,free_count,need_size);
    if (free_count < need_size + ADV_BUF_RESERVE_COUNT) {
        ret = wait_adv_buffer(blob_cli_ctx, need_size + ADV_BUF_RESERVE_COUNT, K_FOREVER);

        if (ret == WAIT_CANCEL_FLAG) {
            BT_DBG("Buffer waiting cancel");
            return 0;
        }
    }

    struct bt_mesh_msg_ctx ctx = { 0 };

    if (0x0000 == dst_addr || !message || !chunk_size || !message->chunk_data) {
        return -EINVAL;
    }

    bt_mesh_model_msg_init(msg, OP_BLOB_CHUNK_TRANS);

    net_buf_simple_add_le16(msg, message->chunk_num);
    net_buf_simple_add_mem(msg, message->chunk_data, chunk_size);
    ctx.addr         = dst_addr;
    ctx.net_idx      = netkey_idx;
    ctx.app_idx      = appkey_idx;
    ctx.trans        = trans;
    ctx.send_ttl     = ttl ? ttl : BLOB_CLI_DEFAULT_TTL;
    ctx.net_transmit = BT_MESH_TRANSMIT(3, 20);
    ret              = bt_mesh_model_send(model, &ctx, msg, cb, cb_data);

    if (ret) {
        BT_ERR("Blob send fail %d", ret);

        if (ret == -ENOBUFS && try_num <= 5) {
            try_num++;
            net_buf_simple_reset(msg);
            goto send;
        }
    } else {
        if (ttl != 0) {
            bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
            wait_send_end(blob_cli_ctx, busy_cout);
            // aos_msleep(100);
        }

        return ret;
    }

    return 0;
}

/*Determine Block Status sub-procedure*/
int ble_mesh_blob_block_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                            uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb, void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_BLOCK_GET, trans, NULL, 0, cb,
                           cb_data);
}

/*Determine Transfer Status procedure*/
int ble_mesh_blob_trans_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                            uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb, void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 0 + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_TRANS_GET, trans, NULL, 0, cb,
                           cb_data);
}

/*Cancel Transfer  procedure*/
int ble_mesh_blob_trans_cancel(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, struct bt_mesh_model *model,
                               uint64_t blob_id, uint8_t trans, uint8_t ttl, const struct bt_mesh_send_cb *cb,
                               void *cb_data)
{
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE + 1 + 4);
#else
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 8 + 4);
#endif
    return _mesh_blob_send(netkey_idx, appkey_idx, dst_addr, ttl, model, msg, OP_BLOB_TRANS_CANCEL, trans,
                           (uint8_t *)&blob_id, 8, cb, cb_data);
}

static void blob_cli_procedure_timeout(void *timer_arg, void *args)
{
    blob_cli *ctx = ((blob_cli *)((char *)timer_arg - ((size_t) & (((blob_cli *)0)->cli_procedure_timer.timer))));
    struct bt_mesh_model *model = get_blob_cli_model(ctx->blob_client_trans.elem_id);

    if (!model) {
        BT_ERR("Blob cli not found for elem:%d from timer:%p", timer_arg);
        return;
    }

    int       ret              = 0;
    blob_cli *blob_cli_ctx     = model->user_data;
    uint8_t   try_timeout_flag = 0;
    uint8_t   dst_phase        = DST_PHASE_UNSET;
    uint8_t   procedure_next   = 0;
    uint8_t   procedure_now    = blob_cli_ctx->blob_client_trans.blob_status;
    uint8_t   max_try          = get_max_try(procedure_now);

    k_timer_stop(&blob_cli_ctx->cli_procedure_timer);

    if (procedure_now == BLOB_CLI_PROCEDURE_IDLE) {
        BT_DBG("Blob Procedure Idle");
        return;
    }

    BT_DBG("Procedure timeout cb, Procedure now:%s,try num:%d,max try:%d", procedure_str(procedure_now),
           blob_cli_ctx->cli_try_num, max_try);

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_SEND_SUB
        && !blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_start_flag)
    {
        BT_WARN("Blob Block send not start");
        return;
    }

    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);

    if (procedure_now == BLOB_CLI_PROCEDURE_SEND_SUB) {
        goto next_procedure;
    }

    try_timeout_flag = blob_cli_ctx->cli_try_num >= max_try ? 1 : 0;

    if (procedure_now == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL) {
        dst_phase = TRANSFER_PHASE_WAITING_CANCEL;
    }

    uint8_t status_flag = _blob_cli_get_all_node_status_get_flag_status(model, dst_phase, try_timeout_flag);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS
        || try_timeout_flag)
    {
        report_all_blob_srv_status(blob_cli_ctx, procedure_now, status_flag);
        cancel_wait_adv_buffer(blob_cli_ctx);
        goto next_procedure;
    }

    if (try_timeout_flag
        && (status_flag == PROCEDURE_STATUS_SOME_RECEIVE_ALL_FAIL
            || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_FAIL || status_flag == PROCEDURE_STATUS_NO_RECEIVE))
    { // TODO REPORT
        report_all_blob_srv_status(blob_cli_ctx, procedure_now, status_flag);
        cancel_wait_adv_buffer(blob_cli_ctx);
        _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_FAIL);
        return;
    }

    blob_cli_ctx->cli_try_num++;
    ret = start_procedure(blob_cli_ctx->blob_client_trans.elem_id, procedure_now);

    if (ret) {
        BT_ERR("Retrieve capablity procedure failed %d", ret);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
    return;
next_procedure:
    blob_cli_ctx->blob_client_trans.blob_status_last = procedure_now;
    procedure_next                                   = get_procedure_next(model, procedure_now);

    if (procedure_next == BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE) {
        _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_CUCCESS);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    } else if (procedure_next == BLOB_CLI_PROCEDURE_IDLE) {
        BT_DBG("procedure next state idle no need process");
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return;
    }

    ret = _blob_cli_prepare_next_precodure(model, procedure_next);

    if (ret) {
        _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_FAIL);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
    return;
}

static void _blob_trans_sec_cancel_timeout(struct bt_mesh_model *model)
{
    uint8_t   try_timeout_flag = 0;
    int       ret              = 0;
    blob_cli *blob_cli_ctx     = model->user_data;
    k_mutex_lock(&blob_cli_ctx->cli_mutex, K_FOREVER);

    try_timeout_flag = blob_cli_ctx->cli_sec_try_num >= CONFIG_MAX_BLOB_TRANS_CANCEL_SEND_RETRY ? 1 : 0;
    uint8_t status_flag
        = _blob_cli_get_all_node_status_get_flag_status(model, TRANSFER_PHASE_WAITING_CANCEL, try_timeout_flag);

    if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS
        || try_timeout_flag)
    {
        cancel_wait_adv_buffer(blob_cli_ctx);
        k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
        report_all_blob_srv_status(blob_cli_ctx, blob_cli_ctx->blob_client_trans.blob_status, status_flag);
    } else {
        if (blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag == 1) {
            ret = ble_mesh_blob_trans_cancel(
                blob_cli_ctx->blob_client_trans.info.netkey_idx, blob_cli_ctx->blob_client_trans.info.appkey_idx,
                blob_cli_ctx->blob_multicast_addr, model, blob_cli_ctx->blob_cli_info.blob_id,
                blob_cli_ctx->blob_client_trans.info.trans_net_if, 0, NULL, NULL);

            if (ret) {
                BT_ERR("Blob trans cancel send failed %d", ret);
            }
        } else {
            for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
                if (blob_cli_ctx->receiver_list[i].trans_phase == TRANSFER_PHASE_WAITING_CANCEL
                    && blob_cli_ctx->receiver_list[i].status_get_flag == BLOB_CLI_SRV_STATUS_NOT_RECEIVE)
                {
                    ret = ble_mesh_blob_trans_cancel(blob_cli_ctx->blob_client_trans.info.netkey_idx,
                                                     blob_cli_ctx->blob_client_trans.info.appkey_idx,
                                                     blob_cli_ctx->receiver_list[i].address, model,
                                                     blob_cli_ctx->blob_cli_info.blob_id,
                                                     blob_cli_ctx->blob_client_trans.info.trans_net_if, 0, NULL, NULL);

                    if (ret < 0) {
                        BT_ERR("blob transfer set failed");
                    }
                }
            }
        }

        blob_cli_ctx->cli_sec_try_num++;
        k_timer_start(&blob_cli_ctx->cli_sec_procedure_timer,
                      blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.timeout);
    }

    k_mutex_unlock(&blob_cli_ctx->cli_mutex);
}

static void blob_cli_sec_procedure_timeout(void *timer_arg, void *args)
{
    blob_cli *ctx = ((blob_cli *)((char *)timer_arg - ((size_t) & (((blob_cli *)0)->cli_sec_procedure_timer.timer))));
    struct bt_mesh_model *model = get_blob_cli_model(ctx->blob_client_trans.elem_id);

    if (!model) {
        BT_ERR("Blob cli not found for elem:%d from timer:%p", ctx->blob_client_trans.elem_id, timer_arg);
        return;
    }

    blob_cli *blob_cli_ctx = model->user_data;
    BT_DBG("Sec_Procedure timeout cb, Sec_Procedure now:%s",
           procedure_str(blob_cli_ctx->blob_client_trans.blob_sec_status));

    switch (blob_cli_ctx->blob_client_trans.blob_sec_status) {
        case BLOB_CLI_PROCEDURE_TRANSFER_CANCEL: {
            _blob_trans_sec_cancel_timeout(model);
        } break;
    }
}

const struct bt_mesh_model_op g_blob_cli_op[BLOB_CLI_OPC_NUM] = {
    { OP_BLOB_TRANS_STATUS, 2, _blob_cli_trans_status },
    { OP_BLOB_BLOCK_STATUS, 5, _blob_cli_block_status },
    { OP_BLOB_INFO_STATUS, 15, _blob_cli_srv_info_status },
    { OP_BLOB_TRANS_SET_STATUS, 1, _blob_cli_id_set_status },
    BT_MESH_MODEL_OP_END,
};

/*set blob srv id procedure*/
static int _blob_cli_set_srv_id_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        if (blob_cli_ctx->receiver_list[i].status_get_flag == BLOB_CLI_SRV_STATUS_NOT_RECEIVE) {
            blob_set_id_message message = {
                .multicast_addr = blob_cli_ctx->blob_multicast_addr,
                .node_id        = i,
                .ttl            = BLOB_CLI_DEFAULT_TTL,
            };
            ret = ble_mesh_blob_set_id(ctx->info.netkey_idx, ctx->info.appkey_idx,
                                       blob_cli_ctx->receiver_list[i].address, model, &message, ctx->info.trans_net_if,
                                       0, NULL, NULL);

            if (ret < 0) {
                BT_ERR("blob set id failed %d", ret);
            }

            if (!ret) {
                bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
                wait_send_end(blob_cli_ctx, busy_cout);
            }
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.id_set.timeout);
    return 0;
}

/*get transfer status procedure*/
static int _blob_cli_get_trans_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    ret = ble_mesh_blob_trans_get(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr, model,
                                  ctx->info.trans_net_if, 0, NULL, NULL);

    if (ret) {
        BT_ERR("Blob trans get send failed %d", ret);
    } else {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.trans_get.timeout);
    return 0;
}

static int _blob_cli_retrieve_capabilities_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    ret = ble_mesh_blob_info_get(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr, model,
                                 ctx->info.trans_net_if, 0, NULL, NULL);

    if (ret) {
        BT_ERR("Blob trans get send failed %d", ret);
    } else {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.capability_get.timeout);
    return 0;
}

static int _blob_cli_trans_blob_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    blob_trans_start_message message = {
        .transfer_mode  = blob_cli_ctx->blob_cli_trans_mode,
        .blob_id        = blob_cli_ctx->blob_cli_info.blob_id,
        .blob_size      = blob_cli_ctx->blob_cli_info.blob_size,
        .block_size_log = blob_cli_ctx->blob_cli_info.block_size_log,
        .cli_mtu_size   = blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size, // TODO
    };

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    ret = ble_mesh_blob_transfer_start(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr,
                                       model, &message, ctx->info.trans_net_if, 0, NULL, NULL);

    if (ret) {
        BT_ERR("Blob trans start send failed %d", ret);
    } else {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.blob_trans.timeout);

    return 0;
}

static int _blob_cli_trans_cancel_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    if (blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag == 1) {
        ret = ble_mesh_blob_trans_cancel(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr,
                                         model, blob_cli_ctx->blob_cli_info.blob_id, ctx->info.trans_net_if, 0, NULL,
                                         NULL);

        if (ret) {
            BT_ERR("Blob trans cancel send failed %d", ret);
        }

    } else {
        for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
            if (blob_cli_ctx->receiver_list[i].trans_phase == TRANSFER_PHASE_WAITING_CANCEL
                && blob_cli_ctx->receiver_list[i].status_get_flag == BLOB_CLI_SRV_STATUS_NOT_RECEIVE)
            {
                ret = ble_mesh_blob_trans_cancel(
                    ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->receiver_list[i].address, model,
                    blob_cli_ctx->blob_cli_info.blob_id, ctx->info.trans_net_if, 0, NULL, NULL);

                if (ret < 0) {
                    BT_ERR("blob transfer set failed");
                }
            }
        }
    }

    if (!ret) {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.trans_cancel.timeout);

    return 0;
}

static int _blob_cli_trans_block_procedure(uint8_t elem_id)
{
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    blob_block_start_message message = {
        .block_num  = blob_cli_ctx->blob_cli_block_info.block_num,  // TODO
        .chunk_size = blob_cli_ctx->blob_cli_block_info.chunk_size, // TODO
    };

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    ret = ble_mesh_blob_block_start(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr,
                                    model, &message, ctx->info.trans_net_if, 0, NULL, NULL);

    if (ret) {
        BT_ERR("Blob trans start send failed %d", ret);
    } else {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.block_trans.timeout);
    return 0;
}

static int get_chunk_data(struct bt_mesh_model *model, blob_block_chunk_message *message, uint32_t *chunk_size,
                          uint16_t chunk_num)
{
    blob_cli *blob_cli_ctx = model->user_data;

    if (chunk_num >= blob_cli_ctx->blob_cli_block_info.total_chunk) {
        return READ_FINISH;
    }

    message->chunk_num = chunk_num;
    message->chunk_data
        = blob_cli_ctx->blob_cli_block_info.data + chunk_num * blob_cli_ctx->blob_cli_block_info.chunk_size;
    *chunk_size = chunk_num == blob_cli_ctx->blob_cli_block_info.total_chunk - 1
                      ? (blob_cli_ctx->blob_cli_block_info.block_size
                         - blob_cli_ctx->blob_cli_block_info.chunk_size * chunk_num)
                      : blob_cli_ctx->blob_cli_block_info.chunk_size;
    return READ_SUCCESS;
}

static int _blob_cli_trans_send_sub_procedure(uint8_t elem_id)
{
    BT_DBG("Send sub cb, for elem %d", elem_id);
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    blob_block_chunk_message message             = { 0x00 };
    uint32_t                 chunk_size          = 0;
    int                      status              = 0;
    uint8_t                  new_data_flag       = 0;
    uint16_t                 chunk_num           = 0;
    uint16_t                 send_loss_chunk_num = 0;

    while (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_SEND_SUB) {
        if (!new_data_flag) {
            if (blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_new_block_flag == 1) {
                chunk_num = blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.chunk_num++;
            } else {
                ret = get_and_clear_flag(
                    blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.total_missing_chunks,
                    CONFIG_BLOB_CLI_MAX_MISS_CHUNKS_FLAG_SIZE, blob_cli_ctx->blob_cli_block_info.total_chunk,
                    &chunk_num);

                if (ret < 0) {
                    uint16_t  received_chunk_num = blob_cli_ctx->blob_cli_block_info.total_chunk - send_loss_chunk_num;
                    long long cost_time
                        = aos_now_ms() - blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.start_time;
                    BT_DBG("Block %d :Trans progress %d:%d,Try num %d,Cost %dms",
                           blob_cli_ctx->blob_cli_block_info.block_num, received_chunk_num,
                           blob_cli_ctx->blob_cli_block_info.total_chunk,
                           blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.try_num + 1, cost_time);
                    (void)received_chunk_num;
                    (void)cost_time;
                    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.sub_send.timeout);
                    return 0;
                } else {
                    send_loss_chunk_num++;
                    // BT_DBG("Send missing chunk:%d",chunk_num);
                }
            }

            status = get_chunk_data(model, &message, &chunk_size, chunk_num);

            if (status == READ_FAIL) {
                BT_ERR("Get chunk data failed");
                return -1;
            } else if (status == READ_FINISH) {
                blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_new_block_flag = 0;
                k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.sub_send.timeout);
                return 0;
            }

            new_data_flag = 1;
        }

        ret = ble_mesh_blob_block_chunk_send(blob_cli_ctx, ctx->info.netkey_idx, ctx->info.appkey_idx,
                                             blob_cli_ctx->blob_multicast_addr, model, &message, chunk_size,
                                             ctx->info.trans_net_if, 0, &send_cb, blob_cli_ctx);

        if (ret) {
            BT_ERR("Blob trans start send failed %d", ret);
            return ret;
        } else {
            new_data_flag = 0;
        }

        blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.send_start_flag = 1;
    }

    BT_DBG("Blob Cli Send sub exit,procedure now %02x", blob_cli_ctx->blob_client_trans.blob_status);
    return 0;
}

/*get block  trans status procedure*/
static int _blob_cli_get_block_procedure(uint8_t elem_id)
{
    BT_DBG("Get block procedure cb, for elem %d", elem_id);
    int                   ret   = 0;
    struct bt_mesh_model *model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("Blob cli model not found");
        return -1;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    blob_cli_trans_ctx *ctx = &blob_cli_ctx->blob_client_trans;

    if (!ctx) {
        return -1;
    }

    uint8_t  busy_cout   = 0;
    uint16_t buffer_size = 0;

    ret = ble_mesh_blob_block_get(ctx->info.netkey_idx, ctx->info.appkey_idx, blob_cli_ctx->blob_multicast_addr, model,
                                  ctx->info.trans_net_if, 0, NULL, NULL);

    if (ret) {
        BT_ERR("Blob trans get send failed %d", ret);
    } else {
        bt_mesh_adv_busy_get(&busy_cout, &buffer_size);
        ret = wait_send_end(blob_cli_ctx, busy_cout);

        if (ret) {
            return 0;
        }
    }

    k_timer_start(&blob_cli_ctx->cli_procedure_timer, ctx->sub_procedures.block_get.timeout);

    return 0;
}

static void blob_thread(void *args)
{
    BT_DBG("started");
    int          ret = 0;
    blob_cli_cmd cmd = { 0x00 };

    while (1) {
        ret = blob_cli_queue_get(&cli_queue, &cmd, K_FOREVER);

        if (ret) {
            BT_ERR("Blob Procedure get cmd failed");
            continue;
        }

        BT_DBG("Blob Procedure:%s for Elem:%02x", procedure_str(cmd.process_cmd), cmd.elem_id);

        switch (cmd.process_cmd) {
            case BLOB_CLI_PROCEDURE_SET_ID: {
                ret = _blob_cli_set_srv_id_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob set id failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_TRANSFER_GET: {
                ret = _blob_cli_get_trans_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob trans get failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY: {
                ret = _blob_cli_retrieve_capabilities_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob retrieve capability failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_TRANSFER_BLOB: {
                ret = _blob_cli_trans_blob_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob trans blob failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_BLOCK_START: {
                ret = _blob_cli_trans_block_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob trans block start failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_SEND_SUB: {
                ret = _blob_cli_trans_send_sub_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob trans send sub failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET: {
                ret = _blob_cli_get_block_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob status get send failed %d", ret);
                }
            } break;

            case BLOB_CLI_PROCEDURE_TRANSFER_CANCEL: {
                ret = _blob_cli_trans_cancel_procedure(cmd.elem_id);

                if (ret) {
                    BT_ERR("blob trans cancel failed %d", ret);
                }
            } break;
        }

        k_yield();
    }
}

static void _blob_cli_model_init(struct bt_mesh_model *mod, struct bt_mesh_elem *elem, bool vnd, bool primary,
                                 void *user_data)
{

    if (mod->id == BT_MESH_MODEL_ID_BLOB_CLI) {
        blob_cli *blob_cli_ctx = mod->user_data;
        k_timer_init(&blob_cli_ctx->cli_procedure_timer, blob_cli_procedure_timeout, NULL);
        k_timer_init(&blob_cli_ctx->cli_sec_procedure_timer, blob_cli_sec_procedure_timeout, NULL);
        k_mutex_init(&blob_cli_ctx->cli_mutex);
        k_sem_init(&blob_cli_ctx->send_sem.sem, 0, 1);
        blob_cli_ctx->send_sem.cancel_flag = 0;
        k_sem_init(&blob_cli_ctx->send_end_sem.sem, 0, 1);
        blob_cli_ctx->send_end_sem.cancel_flag = 0;
    }
}

int ble_mesh_blob_cli_init()
{
    int ret = 0;

    ret = _blob_cli_queue_init(&cli_queue);

    if (ret) {
        BT_ERR("Blob cli queue init failed %d", ret);
        return ret;
    }

    if (CONFIG_BLOB_CLI_MTU_SIZE <= 4) {
        BT_ERR("Blob cli mtu is %d it should be larger than 4 %d", CONFIG_BLOB_CLI_MTU_SIZE);
        return -EINVAL;
    }

    if (BLOB_CLI_DEFAULT_TTL == 1) {
        BT_ERR("Blob cli TTL is not allowd to be 1");
        return -EINVAL;
    }

    ret = k_thread_spawn(&blob_thread_data, "blob cli", (uint32_t *)blob_thread_stack,
                         K_THREAD_STACK_SIZEOF(blob_thread_stack), blob_thread, NULL, CONFIG_BLOB_CLI_PRI);

    if (ret) {
        BT_ERR("Blob cli thread init failed %d", ret);
        return ret;
    }

    bt_mesh_model_foreach(_blob_cli_model_init, NULL);

    return 0;
}

/*for user*/
int ble_mesh_blob_cli_blob_trans(u8_t elem_id, blob_trans_info *info, blob_send_cb cb)
{
    struct bt_mesh_model *model;
    uint32_t              cli_timeout = 0;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_IDLE) {
        BT_ERR("Blob procedure now %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));
        return -EBUSY;
    }

    if (!info->dst_addr_list || !info->dst_addr_size) {
        BT_ERR("blob cli dst addr set null");
        return -EINVAL;
    }

    if (info->dst_addr_size > CONFIG_MAX_BLOB_RECEIVERS_SIZE) {
        BT_ERR("Uncast addr size should no more than %d", CONFIG_MAX_BLOB_RECEIVERS_SIZE);
        return -EINVAL;
    }

    if (!info->multicast_addr) {
        BT_ERR("Multicast addr not set");
        return -EINVAL;
    }

    blob_cli_ctx->blob_multicast_addr = info->multicast_addr;
    blob_cli_ctx->receiver_size       = info->dst_addr_size;
#if defined(CONFIG_BLOB_ACK_CTRL) && CONFIG_BLOB_ACK_CTRL > 0
    blob_cli_ctx->blob_client_trans.sub_procedures.ack_flags_size
        = blob_cli_ctx->receiver_size > 8 * CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE ? 8 * CONFIG_BLOB_ACK_CTRL_MAX_ACK_SIZE
                                                                              : blob_cli_ctx->receiver_size;
#endif

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        blob_cli_ctx->receiver_list[i].address         = info->dst_addr_list[i];
        blob_cli_ctx->receiver_list[i].status_get_flag = BLOB_CLI_SRV_STATUS_NOT_RECEIVE;
        blob_cli_ctx->receiver_list[i].trans_phase     = TRANSFER_PHASE_UNKNOW;
        blob_cli_ctx->receiver_list[i].id              = i;
    }

    blob_cli_ctx->blob_client_trans.info.appkey_idx          = info->appkey_idx;
    blob_cli_ctx->blob_client_trans.info.netkey_idx          = info->netkey_idx;
    blob_cli_ctx->blob_client_trans.info.timeout             = info->timeout;
    blob_cli_ctx->blob_client_trans.info.trans_net_if        = info->trans_net_if;
    blob_cli_ctx->blob_client_trans.trans_options            = info->trans_options;
    blob_cli_ctx->blob_client_trans.elem_id                  = elem_id;
    blob_cli_ctx->blob_cli_info.blob_id                      = info->blob_id;
    blob_cli_ctx->blob_cli_info.blob_size                    = info->blob_data_len;
    blob_cli_ctx->blob_cli_info.data                         = info->blob_data;
    blob_cli_ctx->blob_cli_ttl                               = info->ttl;
    blob_cli_ctx->blob_cli_trans_mode                        = info->trans_mode;
    blob_cli_ctx->blob_cli_timeout_base                      = info->timeout;
    blob_cli_ctx->blob_client_trans.cb.send_start_cb         = cb.send_start_cb;
    blob_cli_ctx->blob_client_trans.cb.send_end_cb           = cb.send_end_cb;
    blob_cli_ctx->blob_client_trans.cb.node_status_cb        = cb.node_status_cb;
    blob_cli_ctx->blob_client_trans.srv_min_server_mtu_size  = CONFIG_BLOB_CLI_MTU_SIZE;
    blob_cli_ctx->blob_client_trans.srv_min_max_chunk_size   = CONFIG_BLOB_CLI_MTU_SIZE - 4;
    blob_cli_ctx->blob_client_trans.srv_min_max_total_chunks = CONFIG_BLOB_CLI_MAX_TOTAL_CHUNKS;

    cli_timeout
        = _get_blob_timeout(blob_cli_ctx->blob_cli_timeout_base, blob_cli_ctx->blob_cli_ttl); // TODO timeout set?
    (void)cli_timeout;
    blob_cli_ctx->blob_client_trans.sub_procedures.blob_trans.timeout     = 400; // TODO
    blob_cli_ctx->blob_client_trans.sub_procedures.block_trans.timeout    = 400; // TODO
    blob_cli_ctx->blob_client_trans.sub_procedures.sub_send.timeout       = 400; // 500ms
    blob_cli_ctx->blob_client_trans.sub_procedures.block_get.timeout      = 500;
    blob_cli_ctx->blob_client_trans.sub_procedures.id_set.timeout         = 400;
    blob_cli_ctx->blob_client_trans.sub_procedures.capability_get.timeout = 400;
    blob_cli_ctx->blob_client_trans.srv_min_capability_set_flag           = 0x00;
    blob_cli_ctx->blob_client_trans.blob_status_last                      = blob_cli_ctx->blob_client_trans.blob_status;
    return _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_SET_ID);
}

int ble_mesh_blob_cli_blob_trans_status_get_all(uint8_t elem_id, uint64_t blob_id)
{
    struct bt_mesh_model *model;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_IDLE) {
        BT_ERR("Blob not idle,procedure now:%s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));
        return -EBUSY;
    }

    if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) {
        BT_ERR("Invalid blob id");
        return -EINVAL;
    }

    blob_cli_ctx->blob_client_trans.sub_procedures.trans_get.timeout = 1000;
    blob_cli_ctx->blob_client_trans.blob_status_last                 = blob_cli_ctx->blob_client_trans.blob_status;
    return _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_TRANSFER_GET);
}

int ble_mesh_blob_cli_blob_trans_status_get(uint8_t elem_id, uint64_t blob_id, uint16_t uncast_addr_size,
                                            uint16_t *uncast_addr_list)
{
    struct bt_mesh_model *model;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_IDLE) {
        BT_ERR("Blob not idle,procedure now:%s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));
        return -EBUSY;
    }

    if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) {
        BT_ERR("Invalid blob id");
        return -EINVAL;
    }

    blob_cli_ctx->blob_client_trans.sub_procedures.trans_get.timeout = 1000;
    blob_cli_ctx->blob_client_trans.blob_status_last                 = blob_cli_ctx->blob_client_trans.blob_status;
    return _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_TRANSFER_GET);
}

int ble_mesh_blob_cli_blob_trans_cancel_all(uint8_t elem_id, uint64_t blob_id)
{
    struct bt_mesh_model *model;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) {
        BT_ERR("Invalid blob id");
        return -EINVAL;
    }

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL
        && blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag == 1)
    {
        BT_WARN("Already in transfer cancel all");
        return 0;
    }

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_IDLE) {
        BT_DBG("Blob cli idle");
        return 0;
    }

    uint16_t waiting_cancel_num = 0;

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        blob_receiver *receiver = &blob_cli_ctx->receiver_list[i];

        if (receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_START
            || receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK
            || receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK
            || receiver->trans_phase == TRANSFER_PHASE_SUSPEND)
        {
            receiver->trans_phase                          = TRANSFER_PHASE_WAITING_CANCEL;
            blob_cli_ctx->receiver_list[i].status_get_flag = BLOB_CLI_SRV_STATUS_NOT_RECEIVE;
            waiting_cancel_num++;
            break;
        } else {
            receiver->trans_phase = TRANSFER_PHASE_INACTIVE;
        }
    }

    blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag = 1;
    k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
    k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
    blob_cli_ctx->blob_client_trans.blob_status = BLOB_CLI_PROCEDURE_TRANSFER_CANCEL;

    if (waiting_cancel_num) {
        blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.timeout = 1000;
        blob_cli_ctx->blob_client_trans.blob_status_last = blob_cli_ctx->blob_client_trans.blob_status;
        return _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_TRANSFER_CANCEL);
    } else {
        BT_DBG("All node status inactive,trans cancel success");
        blob_cli_ctx->blob_client_trans.blob_status = BLOB_CLI_PROCEDURE_IDLE;
    }

    return 0;
}

int ble_mesh_blob_cli_blob_trans_cancel(uint8_t elem_id, uint64_t blob_id, uint16_t uncast_addr_size,
                                        uint16_t *uncast_addr_list)
{
    struct bt_mesh_model *model;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) {
        BT_ERR("Invalid blob id");
        return -EINVAL;
    }

    if (!uncast_addr_size || !uncast_addr_list) {
        BT_ERR("cancel addr not set");
        return -EINVAL;
    }

    if (uncast_addr_size > blob_cli_ctx->receiver_size) {
        BT_ERR("cancel addr not set");
        return -EINVAL;
    }

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_TRANSFER_CANCEL
        && blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag == 1)
    {
        BT_WARN("Already in transfer cancel all");
        return 0;
    }

#if 0

    if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_IDLE) {
        BT_DBG("Blob cli idle");
        return 0;
    }

#endif

    uint16_t waiting_cancel_num = 0;
    uint16_t inacitive_num      = 0;

    for (int i = 0; i < blob_cli_ctx->receiver_size; i++) {
        blob_receiver *receiver          = &blob_cli_ctx->receiver_list[i];
        uint8_t        found_device_flag = 0;
        int            j                 = 0;

        for (j = 0; j < uncast_addr_size; j++) {
            if (uncast_addr_list[j] == receiver->address) {
                break;
            }
        }

        if (j != uncast_addr_size) {
            found_device_flag = 1;
        }

        if (receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_START
            || receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_BLOCK
            || receiver->trans_phase == TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK
            || receiver->trans_phase == TRANSFER_PHASE_SUSPEND)
        {
            if (found_device_flag == 1) {
                BT_DBG("Set node state waiting cancel");
                receiver->trans_phase                          = TRANSFER_PHASE_WAITING_CANCEL;
                blob_cli_ctx->receiver_list[i].status_get_flag = BLOB_CLI_SRV_STATUS_NOT_RECEIVE;
                waiting_cancel_num++;

                if (waiting_cancel_num == uncast_addr_size) {
                    break;
                }
            }
        } else {
            if (found_device_flag == 1) {
                receiver->trans_phase = TRANSFER_PHASE_INACTIVE;
                inacitive_num++;
            }
        }
    }

    BT_DBG("Inactive num %d waiting cancel num %04x", inacitive_num, waiting_cancel_num);

    if (waiting_cancel_num) {
        // blob_cli_ctx->blob_client_trans.blob_status_last    = blob_cli_ctx->blob_client_trans.blob_status;
        blob_cli_ctx->blob_client_trans.blob_sec_status                     = BLOB_CLI_PROCEDURE_TRANSFER_CANCEL;
        blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.timeout = 1000;

        if (inacitive_num + waiting_cancel_num == blob_cli_ctx->receiver_size) {
            k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
            k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
            blob_cli_ctx->blob_client_trans.sub_procedures.trans_cancel.cancel_all_flag = 1;
            blob_cli_ctx->blob_client_trans.blob_status_last = blob_cli_ctx->blob_client_trans.blob_status;
            return _blob_cli_prepare_next_precodure(model, BLOB_CLI_PROCEDURE_TRANSFER_CANCEL);
        } else {
            k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
            k_timer_start(&blob_cli_ctx->cli_sec_procedure_timer, 2);
        }
    } else {
        if (inacitive_num + waiting_cancel_num == blob_cli_ctx->receiver_size) {
            k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
            k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
            blob_cli_ctx->blob_client_trans.blob_status = BLOB_CLI_PROCEDURE_IDLE;
        }
    }

    return 0;
}

int ble_mesh_blob_cli_blob_trans_upper_set_success(uint8_t elem_id, uint64_t blob_id, uint16_t uncast_addr)
{
    struct bt_mesh_model *model;

    blob_receiver *receiver       = NULL;
    uint8_t        status_flag    = 0;
    uint8_t        procedure_next = 0;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_id != blob_cli_ctx->blob_cli_info.blob_id) {
        BT_ERR("Invalid blob id");
        return -EINVAL;
    }

    receiver = get_blob_receiver(model, uncast_addr);

    if (!receiver) {
        BT_INFO("Blob Receiver %04x not exist in the list", uncast_addr);
        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
        return -EINVAL;
    } else {
        BT_DBG("Set node %04x state from %s to COMPLETE by user", uncast_addr, phase_str(receiver->trans_phase));
        receiver->block_complete_flag = 1;
        receiver->trans_phase         = TRANSFER_PHASE_COMPLETE;
        receiver->status              = TRANSFER_STATUS_SUCCESS;

        if (blob_cli_ctx->blob_client_trans.cb.node_status_cb) {
            blob_cli_ctx->blob_client_trans.cb.node_status_cb(blob_cli_ctx->blob_client_trans.elem_id,
                                                              blob_cli_ctx->blob_cli_info.blob_id, uncast_addr,
                                                              TRANSFER_PHASE_COMPLETE, TRANSFER_STATUS_SUCCESS);
        }

        if (blob_cli_ctx->blob_client_trans.blob_status == BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET) {
            receiver->status_get_flag = BLOB_CLI_SRV_STATUS_RECV_SUCCESS;
            status_flag
                = _blob_cli_get_all_node_status_get_flag_status(model, TRANSFER_PHASE_WAITING_FOR_NEXT_CHUNK, 0);

            if (status_flag == PROCEDURE_STATUS_ALL_RECEIVE_SOME_FAIL
                || status_flag == PROCEDURE_STATUS_ALL_RECEIVE_ALL_SUCCESS
                || status_flag == PROCEDURE_STATUS_NO_NEED_RECEIVE)
            {
                BT_DBG("All node recv block complete %02x,stop direct", status_flag);
                k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
                cancel_wait_adv_buffer(blob_cli_ctx);
                report_all_blob_srv_status(blob_cli_ctx, BLOB_CLI_PROCEDURE_BLOCK_STATUS_GET, status_flag);
                procedure_next = get_procedure_next(model, blob_cli_ctx->blob_client_trans.blob_status);

                if (procedure_next == BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE) {
                    _blob_cli_exist_trans(model, BLOB_CLI_EXIST_STATUS_CUCCESS);
                } else {
                    _blob_cli_prepare_next_precodure(model, procedure_next);
                }
            }
        }

        k_mutex_unlock(&blob_cli_ctx->cli_mutex);
    }

    return 0;
}

int ble_mesh_blob_cli_blob_reset(uint8_t elem_id)
{
    struct bt_mesh_model *model;

    model = get_blob_cli_model(elem_id);

    if (!model) {
        BT_ERR("No blob cli found in element %d", elem_id);
        return -EINVAL;
    }

    if (!model->user_data) {
        BT_ERR("blob cli has no user data");
        return -EINVAL;
    }

    blob_cli *blob_cli_ctx = model->user_data;

    if (blob_cli_ctx->blob_client_trans.blob_status != BLOB_CLI_PROCEDURE_IDLE
        || blob_cli_ctx->blob_client_trans.blob_sec_status != BLOB_CLI_PROCEDURE_IDLE)
    {
        k_timer_stop(&blob_cli_ctx->cli_procedure_timer);
        k_timer_stop(&blob_cli_ctx->cli_sec_procedure_timer);
        BT_DBG("Exist the trans procedure now %s", procedure_str(blob_cli_ctx->blob_client_trans.blob_status));
        uint32_t offset = (size_t) & (((blob_cli *)0)->cli_procedure_timer);
        memset(blob_cli_ctx, 0x00, offset);
    }

    return 0;
}

#endif
