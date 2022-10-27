/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#include <ble_os.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <misc/byteorder.h>
#include <misc/util.h>

#include <aos/bt.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <atomic.h>
#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_AVRCP)
#define LOG_MODULE_NAME bt_avrcp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "avrcp_internal.h"

#define AVRCP_CALL_CT_DATA(session, tl, buf)                                                                           \
    do {                                                                                                               \
        if (avrcp_ct_cb && avrcp_ct_cb->data) {                                                                        \
            avrcp_ct_cb->data(session, tl, buf);                                                                       \
        }                                                                                                              \
    } while (0);

#define AVRCP_CALL_TG_DATA(session, tl, buf)                                                                           \
    do {                                                                                                               \
        if (avrcp_tg_cb && avrcp_tg_cb->data) {                                                                        \
            avrcp_tg_cb->data(session, tl, buf);                                                                       \
        }                                                                                                              \
    } while (0);

#define AVRCP_CALL_CT_IND(name, session)                                                                               \
    do {                                                                                                               \
        if (avrcp_ct_cb && avrcp_ct_cb->ind && avrcp_ct_cb->ind->name) {                                               \
            avrcp_ct_cb->ind->name(session);                                                                           \
        }                                                                                                              \
    } while (0);

#define AVRCP_CALL_TG_IND(name, session)                                                                               \
    do {                                                                                                               \
        if (avrcp_tg_cb && avrcp_tg_cb->ind && avrcp_tg_cb->ind->name) {                                               \
            avrcp_tg_cb->ind->name(session);                                                                           \
        }                                                                                                              \
    } while (0);

#define AVRCP_CHAN(_ch) CONTAINER_OF(_ch, struct bt_avrcp, br_chan.chan);

static void bt_avrcp_l2cap_connected(struct bt_l2cap_chan *chan);
static void bt_avrcp_l2cap_disconnected(struct bt_l2cap_chan *chan);
static int  bt_avrcp_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf);

/** Connections */
static struct bt_avrcp avrcp_connection[CONFIG_BT_BR_MAX_CONN];

static const struct bt_avrcp_event_cb *avrcp_tg_cb;
static const struct bt_avrcp_event_cb *avrcp_ct_cb;

static const struct bt_l2cap_chan_ops avrcp_l2cap_ops = {
    .connected    = bt_avrcp_l2cap_connected,
    .disconnected = bt_avrcp_l2cap_disconnected,
    .recv         = bt_avrcp_l2cap_recv,
};

static atomic_t init;

uint8_t avrcp_get_tl()
{
    static u8_t tl;

    /** Loop for 16*/
    tl %= 16;

    return tl++;
}

static void avrcp_reset(struct bt_avrcp *avrcp)
{
    (void)memset(avrcp, 0, sizeof(struct bt_avrcp));
}

static enum bt_avdtp_cr avrcp_get_cr_from_ctype(u8_t ctype)
{
    if (ctype < BT_AVRCP_RESPONSE_NOT_IMPLEMENTED) {
        return BT_AVCTP_COMMAND;
    }

    return BT_AVCTP_RESPONSE;
}

static struct bt_avrcp *get_new_connection(struct bt_conn *conn, int *errno)
{
    s8_t i, free;

    free = -1;

    if (!conn) {
        BT_ERR("Invalid Input (err: %d)", -EINVAL);
        *errno = -EINVAL;
        return NULL;
    }

    /* Find a space */
    for (i = 0; i < CONFIG_BT_BR_MAX_CONN; i++) {
        if (avrcp_connection[i].br_chan.chan.conn == conn) {
            BT_DBG("Conn already exists");
            *errno = -EALREADY;
            return &avrcp_connection[i];
        }

        if (!avrcp_connection[i].br_chan.chan.conn && free == -1) {
            free = i;
        }
    }

    if (free == -1) {
        BT_DBG("More avrcp_connection cannot be supported");
        *errno = -ENOMEM;
        return NULL;
    }

    /* Clean the memory area before returning */
    avrcp_reset(&avrcp_connection[free]);

    return &avrcp_connection[free];
}

static struct bt_avrcp *get_connected_session()
{
    s8_t i;

    /* Find a space */
    for (i = 0; i < CONFIG_BT_BR_MAX_CONN; i++) {
        if (avrcp_connection[i].br_chan.chan.conn) {
            return &avrcp_connection[i];
        }
    }

    return NULL;
}

/** L2CAP Interface callbacks */
static void bt_avrcp_l2cap_connected(struct bt_l2cap_chan *chan)
{
    BT_DBG("");
    struct bt_avrcp *session = AVRCP_CHAN(chan);

    AVRCP_CALL_CT_IND(connected, session);
    AVRCP_CALL_TG_IND(connected, session);
}

static void bt_avrcp_l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    BT_DBG("");
    struct bt_avrcp *        session = AVRCP_CHAN(chan);
    struct bt_l2cap_br_chan *ch      = CONTAINER_OF(chan, struct bt_l2cap_br_chan, chan);

    AVRCP_CALL_CT_IND(disconnected, session);
    AVRCP_CALL_TG_IND(disconnected, session);

    ch->chan.conn = NULL;
    ch->tx.cid    = 0;
    ch->rx.cid    = 0;
}

static void bt_avrcp_reponse_IPID(struct bt_avctp_hdr *hdr)
{
    struct bt_avctp_hdr *avctp_hdr;
    struct net_buf *     buf;

    buf = bt_l2cap_create_pdu(NULL, 0);

    if (buf == NULL) {
        return;
    }

    avctp_hdr = net_buf_add(buf, sizeof(*avctp_hdr));

    avctp_hdr->tl       = hdr->tl;
    avctp_hdr->IPID     = 1;
    avctp_hdr->packtype = BT_AVCTP_SINGLE_PACKET;
    avctp_hdr->cr       = BT_AVCTP_RESPONSE;
    avctp_hdr->pid      = hdr->pid;

    bt_avrcp_send(buf);
}

static int bt_avrcp_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    u16_t                pid;
    struct bt_avctp_hdr *hdr;
    struct bt_avrcp *    session = AVRCP_CHAN(chan);

    if (buf->len < sizeof(*hdr)) {
        BT_ERR("Recvd Wrong AVCTP Header");
        return -EINVAL;
    }

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));

    BT_DBG("packet_type[0x%02x] cr[0x%02x] tl[0x%02x]", hdr->packtype, hdr->cr, hdr->tl);

    if (hdr->cr == BT_AVCTP_COMMAND) {
        pid = sys_be16_to_cpu(hdr->pid);
        BT_DBG("pid: 0x%04x", pid);
        /** repsonse controller with IPID */
        if (pid != BT_SDP_AV_REMOTE_SVCLASS) {
            bt_avrcp_reponse_IPID(hdr);
            return 0;
        }
        /** deal with command from controller */
        AVRCP_CALL_TG_DATA(session, hdr->tl, buf);
    } else {
        /** deal with response from target */
        AVRCP_CALL_CT_DATA(session, hdr->tl, buf);
    }

    return 0;
}

static int bt_avrcp_l2cap_accept(struct bt_conn *conn, struct bt_l2cap_chan **chan)
{
    struct bt_avrcp *session = NULL;
    int              result  = 0;

    BT_DBG("conn %p", conn);
    /* Get the AVRCP session from upper layer */
    session = get_new_connection(conn, &result);

    if (result < 0) {
        return result;
    }
    session->br_chan.chan.ops = &avrcp_l2cap_ops;
    session->br_chan.rx.mtu   = BT_AVRCP_MAX_MTU;
    *chan                     = &session->br_chan.chan;

    return 0;
}

/** AVRCP Iint function */
int bt_avrcp_init(void)
{
    static struct bt_l2cap_server avrcp_l2cap = {
        .psm       = BT_L2CAP_PSM_AVCTP,
        .sec_level = BT_SECURITY_L2,
        .accept    = bt_avrcp_l2cap_accept,
    };
    int err;

    if (!atomic_cas(&init, 0, 1)) {
        return 0;
    }

    BT_DBG("");

    /* Register AVRCP PSM with L2CAP */
    err = bt_l2cap_br_server_register(&avrcp_l2cap);
    if (err < 0) {
        BT_ERR("AVRCP L2CAP Registration failed %d", err);
        return err;
    }

    return 0;
}

/** AVRCP register event callback from controller */
void bt_avrcp_ct_register(const struct bt_avrcp_event_cb *cb)
{
    avrcp_ct_cb = cb;
}

/** AVRCP register event callback from target */
void bt_avrcp_tg_register(const struct bt_avrcp_event_cb *cb)
{
    avrcp_tg_cb = cb;
}

struct net_buf *avrcp_create_pdu(u8_t cr, u8_t opcode, u8_t ctype, u8_t tl)
{
    struct net_buf *     buf;
    struct bt_avctp_hdr *avctp_hdr;
    struct bt_avc_hdr *  avc_hdr;

    buf = bt_l2cap_create_pdu(NULL, 0);

    if (buf == NULL) {
        return NULL;
    }

    avctp_hdr = net_buf_add(buf, sizeof(*avctp_hdr));

    avctp_hdr->tl       = tl;
    avctp_hdr->IPID     = 0;
    avctp_hdr->packtype = BT_AVCTP_SINGLE_PACKET;
    avctp_hdr->cr       = cr;
    sys_put_be16(BT_SDP_AV_REMOTE_SVCLASS, (uint8_t *)&avctp_hdr->pid);

    avc_hdr = net_buf_add(buf, sizeof(*avc_hdr));

    avc_hdr->cr     = ctype;
    avc_hdr->rfa    = 0;
    avc_hdr->opcode = opcode;
    switch (opcode) {
        case BT_AVCTP_OPCODE_VENDOR_DEPENDENT:
        case BT_AVCTP_OPCODE_PASS_THROUGH:
            avc_hdr->subinit      = BT_AVRCP_SUBUNIT_ID;
            avc_hdr->Subunit_type = BT_AVRCP_SUBUNIT_TYPE_PANEL;
            break;
        case BT_AVCTP_OPCODE_UNIT_INFO:
        case BT_AVCTP_OPCODE_SUBUNIT_INFO:
            avc_hdr->subinit      = BT_AVRCP_SUBUNIT_ID_IGNORE;
            avc_hdr->Subunit_type = BT_AVRCP_SUBUNIT_TYPE_UNIT;
            break;
    }

    return buf;
}

/** AVRCP create vendor depenent response packet */
struct net_buf *bt_avrcp_create_vendor_depenent_pdu(u8_t pdu_id, void *param, u16_t param_len, u8_t tl, u8_t ctype)
{
    struct net_buf *     buf;
    struct bt_avrcp_hdr *avrcp_hdr;

    BT_DBG("pdu_id %d", pdu_id);

    buf = avrcp_create_pdu(avrcp_get_cr_from_ctype(ctype), BT_AVCTP_OPCODE_VENDOR_DEPENDENT, ctype, tl);

    if (buf == NULL) {
        return NULL;
    }

    avrcp_hdr = net_buf_add(buf, sizeof(*avrcp_hdr));

    sys_put_be24(BT_SIG_AVC_COMPANY_ID, avrcp_hdr->company_id);
    avrcp_hdr->packet_type = BT_AVRCP_SINGLE_PACKET;
    avrcp_hdr->pdu_id      = pdu_id;
    net_buf_add_be16(buf, param_len);
    net_buf_add_mem(buf, param, param_len);

    return buf;
}

/** AVRCP create pass through packet */
struct net_buf *bt_avrcp_create_pass_through_pdu(u8_t operation_id, u8_t operation_state, u8_t tl, u8_t ctype)
{
    struct net_buf *                     buf;
    struct bt_avrcp_pass_througt_packet *packet;

    BT_DBG("operation_id %d, operation_state %d", operation_id, operation_state);

    buf = avrcp_create_pdu(avrcp_get_cr_from_ctype(ctype), BT_AVCTP_OPCODE_PASS_THROUGH, ctype, tl);

    if (buf == NULL) {
        return NULL;
    }

    packet = net_buf_add(buf, sizeof(*packet));

    packet->operation_id    = operation_id;
    packet->operation_state = operation_state;
    packet->field_len       = 0;

    return buf;
}

/** AVRCP send packet */
int bt_avrcp_send(struct net_buf *buf)
{
    int              result;
    struct bt_avrcp *session;

    /** FIXME: only support one ACL connecion for now */
    session = get_connected_session();

    if (session == NULL) {
        BT_ERR("No connected session");
        return -EINVAL;
    }

    result = bt_l2cap_chan_send(&session->br_chan.chan, buf);
    if (result < 0) {
        BT_ERR("Error:L2CAP send fail - result = %d", result);
        return result;
    }

    return 0;
}

int bt_avrcp_connect(struct bt_conn *conn, struct bt_avrcp *session)
{
    if (!session) {
        return -EINVAL;
    }

    session->br_chan.chan.ops                = &avrcp_l2cap_ops;
    session->br_chan.chan.required_sec_level = BT_SECURITY_L2;
    session->br_chan.rx.mtu                  = BT_AVRCP_MAX_MTU;

    return bt_l2cap_chan_connect(conn, &session->br_chan.chan, BT_L2CAP_PSM_AVCTP);
}

int bt_avrcp_disconnect(struct bt_avrcp *session)
{
    if (!session) {
        return -EINVAL;
    }

    BT_DBG("session %p", session);

    return bt_l2cap_chan_disconnect(&session->br_chan.chan);
}

/** AVRCP target connect to controller */
bt_stack_status_t bt_prf_avrcp_connect(bt_dev_addr_t *peer_addr)
{
    int              err;
    struct bt_conn * conn;
    bt_addr_t        addr;
    struct bt_avrcp *session;

    memcpy(addr.val, peer_addr->val, BT_STACK_BD_ADDR_LEN);
    conn = bt_conn_lookup_addr_br(&addr);

    if (conn == NULL) {
        BT_ERR("Connection failed");
        return -ENOMEM;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    session = get_new_connection(conn, &err);
    if (!session || err == -EALREADY) {
        BT_ERR("Already Connected");
        return -EALREADY;
    }

    err = bt_avrcp_connect(conn, session);
    if (err < 0) {
        /** If error occurs, undo the saving and return the error */
        BT_DBG("AVDTP Connect failed err %d", err);
        return -EINVAL;
    }

    BT_DBG("Connect request sent");

    return 0;
}

/** AVRCP target disconnect to controller */
bt_stack_status_t bt_prf_avrcp_disconnect(bt_dev_addr_t *peer_addr)
{
    int              err;
    struct bt_conn * conn;
    bt_addr_t        addr;
    struct bt_avrcp *session;

    memcpy(addr.val, peer_addr->val, BT_STACK_BD_ADDR_LEN);
    conn = bt_conn_lookup_addr_br(&addr);

    if (!conn) {
        BT_DBG("NO Connection");
        return 0;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    session = get_connected_session();

    if (session == NULL) {
        BT_ERR("No connected session");
        return -EINVAL;
    }

    err = bt_avrcp_disconnect(session);
    if (err < 0) {
        /** If error occurs, undo the saving and return the error */
        BT_DBG("AVDTP Disonnect failed");
        return -EINVAL;
    }

    BT_DBG("Disconnect req send");

    return 0;
}
