/*
 * avdtp_internal.h - avdtp handling

 * Copyright (c) 2015-2016 Intel Corporation
 * Copyright (c) 2022 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __AVDTP_INTERNAL_H__
#define __AVDTP_INTERNAL_H__

#define BT_AVDTP_VERSION 0x103

#define BT_AVDTP_RTP_VERSION     2
#define BT_AVDTP_RTP_PADDING     0
#define BT_AVDTP_RTP_EXTENSION   0
#define BT_AVDTP_RTP_CC          0
#define BT_AVDTP_RTP_MARKER      0
#define BT_AVDTP_RTP_PAYLOADTYPE 0x60

#define BT_AVDTP_RTP_HEADER                                                           \
    ((BT_AVDTP_RTP_VERSION << 14) | (BT_AVDTP_RTP_PADDING << 13)                      \
     | (BT_AVDTP_RTP_EXTENSION << 12) | (BT_AVDTP_RTP_CC << 8)                        \
     | (BT_AVDTP_RTP_MARKER) << 7 | (BT_AVDTP_RTP_PAYLOADTYPE))

/** @brief AVDTP stream Role */
#define BT_AVDTP_SRC_ROLE 0x00
#define BT_AVDTP_SNK_ROLE 0x01

/** @brief AVDTP signal Role */
#define BT_AVDTP_INT 0x00
#define BT_AVDTP_ACP 0x01

#define BT_L2CAP_PSM_AVDTP 0x0019

/** @brief AVDTP SIGNAL HEADER - Packet Type*/
#define BT_AVDTP_PACKET_TYPE_SINGLE   0x00
#define BT_AVDTP_PACKET_TYPE_START    0x01
#define BT_AVDTP_PACKET_TYPE_CONTINUE 0x02
#define BT_AVDTP_PACKET_TYPE_END      0x03

/** @brief AVDTP SIGNAL HEADER - MESSAGE TYPE */
#define BT_AVDTP_CMD        0x00
#define BT_AVDTP_GEN_REJECT 0x01
#define BT_AVDTP_ACCEPT     0x02
#define BT_AVDTP_REJECT     0x03

/** @brief AVDTP SIGNAL HEADER - Signal Identifier */
#define BT_AVDTP_DISCOVER             0x01
#define BT_AVDTP_GET_CAPABILITIES     0x02
#define BT_AVDTP_SET_CONFIGURATION    0x03
#define BT_AVDTP_GET_CONFIGURATION    0x04
#define BT_AVDTP_RECONFIGURE          0x05
#define BT_AVDTP_OPEN                 0x06
#define BT_AVDTP_START                0x07
#define BT_AVDTP_CLOSE                0x08
#define BT_AVDTP_SUSPEND              0x09
#define BT_AVDTP_ABORT                0x0a
#define BT_AVDTP_SECURITY_CONTROL     0x0b
#define BT_AVDTP_GET_ALL_CAPABILITIES 0x0c
#define BT_AVDTP_DELAYREPORT          0x0d

/** @brief AVDTP STREAM STATE */
#define BT_AVDTP_STREAM_STATE_IDLE       0x00
#define BT_AVDTP_STREAM_STATE_CONFIGURED 0x01
#define BT_AVDTP_STREAM_STATE_OPEN       0x02
#define BT_AVDTP_STREAM_STATE_STREAMING  0x03
#define BT_AVDTP_STREAM_STATE_CLOSING    0x04
#define BT_AVDTP_STREAM_STATE_ABORTING   0x05

/** @brief AVDTP Service Catagory */
#define BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT    0x01
#define BT_AVDTP_SERVICE_CAT_REPORTING          0x02
#define BT_AVDTP_SERVICE_CAT_RECOVERY           0x03
#define BT_AVDTP_SERVICE_CAT_CONTENT_PROTECTION 0x04
#define BT_AVDTP_SERVICE_CAT_HDR_COMPRESSION    0x05
#define BT_AVDTP_SERVICE_CAT_MULTIPLEXING       0x06
#define BT_AVDTP_SERVICE_CAT_MEDIA_CODEC        0x07
#define BT_AVDTP_SERVICE_CAT_DELAYREPORTING     0x08

/** @brief AVDTP Error Codes */
#define BT_AVDTP_SUCCESS                        0x00
#define BT_AVDTP_ERR_BAD_HDR_FORMAT             0x01
#define BT_AVDTP_ERR_BAD_LENGTH                 0x11
#define BT_AVDTP_ERR_BAD_ACP_SEID               0x12
#define BT_AVDTP_ERR_SEP_IN_USE                 0x13
#define BT_AVDTP_ERR_SEP_NOT_IN_USE             0x14
#define BT_AVDTP_ERR_BAD_SERV_CATEGORY          0x17
#define BT_AVDTP_ERR_BAD_PAYLOAD_FORMAT         0x18
#define BT_AVDTP_ERR_NOT_SUPPORTED_COMMAND      0x19
#define BT_AVDTP_ERR_INVALID_CAPABILITIES       0x1a
#define BT_AVDTP_ERR_BAD_RECOVERY_TYPE          0x22
#define BT_AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT 0x23
#define BT_AVDTP_ERR_BAD_RECOVERY_FORMAT        0x25
#define BT_AVDTP_ERR_BAD_ROHC_FORMAT            0x26
#define BT_AVDTP_ERR_BAD_CP_FORMAT              0x27
#define BT_AVDTP_ERR_BAD_MULTIPLEXING_FORMAT    0x28
#define BT_AVDTP_ERR_UNSUPPORTED_CONFIGURAION   0x29
#define BT_AVDTP_ERR_BAD_STATE                  0x31

#define BT_AVDTP_MAX_MTU BT_L2CAP_RX_MTU

#define BT_AVDTP_MIN_SEID 0x01
#define BT_AVDTP_MAX_SEID 0x3E

struct bt_avdtp;
struct bt_avdtp_req;

/** @brief Stream End Point Media Type */
enum bt_avdtp_media_type
{
    /** Audio Media Type */
    BT_AVDTP_AUDIO = 0x00,
    /** Video Media Type */
    BT_AVDTP_VIDEO = 0x01,
    /** Multimedia Media Type */
    BT_AVDTP_MULTIMEDIA = 0x02
};

/** @brief Stream End Point Role */
enum bt_avdtp_role_type
{
    /** Source Role */
    BT_A2DP_SOURCE = 0,
    /** Sink Role */
    BT_A2DP_SINK = 1
};

/** @brief AVDTP signal request confirm callback define */
typedef int (*bt_avdtp_cmf_cb_t)(struct bt_avdtp *session, struct bt_avdtp_req *req,
                               struct net_buf *buf);

/** @brief AVDTP signal request structure */
struct bt_avdtp_req {
    /** Signal Identifier */
    u8_t sig;
    /** Transaction Label */
    u8_t tid;
    /** Signal confirm callback */
    bt_avdtp_cmf_cb_t func;
    /** Signal request timer */
    struct k_delayed_work timeout_work;
};

/** @brief AVDTP signal command and response headers for single packets
 * revert struct member in byte for memory order.
 * so that the memory in packet could map to the structure.
 */
struct bt_avdtp_single_sig_hdr {
    /** packet Type */
    u8_t msgtype : 2;
    /** packet Type */
    u8_t packtype : 2;
    /** Transation Lable */
    u8_t tid : 4;
    /** Signal Identifier */
    u8_t signal_id;
} __packed;

/** @brief AVDTP signal command and response headers for start packets
 * revert struct member in byte for memory order.
 * so that the memory in packet could map to the structure.
 */
struct bt_avdtp_start_sig_hdr {
    /** packet Type */
    u8_t packtype : 2;
    /** packet Type */
    u8_t msgtype : 2;
    /** Transation Lable */
    u8_t tid : 4;
    /**  Number Of Signal Packets */
    u8_t nosp;
    /** Signal Identifier */
    u8_t signal_id;
} __packed;

/** @brief AVDTP signal command and response headers for continue and end packets */
struct bt_avdtp_continue_end_sig_hdr {
    /** packet Type */
    u8_t msgtype : 2;
    /** packet Type */
    u8_t packtype : 2;
    /** Transation Lable */
    u8_t tid : 4;
    /**  Number Of Signal Packets */
} __packed;

#define BT_AVDTP_SIG_HDR_LEN sizeof(struct bt_avdtp_single_sig_hdr)

/** @brief AVDTP SEID Information
 * revert struct member in byte for memory order.
 * so that the memory in packet could map to the structure.
 */
struct bt_avdtp_seid_info {
    /** Reserved 0 */
    u8_t rfa0 : 1;
    /** End Point usage status */
    u8_t inuse : 1;
    /** Stream End Point ID */
    u8_t id : 6;
    /** Reserved 1 */
    u8_t rfa1 : 3;
    /** TSEP of the End Point */
    u8_t tsep : 1;
    /** Media-type of the End Point \ref bt_avdtp_media_type */
    u8_t media_type : 4;
} __packed;

/** @brief AVDTP Stream End Point media codec capability*/
struct bt_avdtp_media_codec_cap {
    /** Stream End Point media type \ref bt_avdtp_media_type */
    u8_t media_type;
    /** Stream End Point codec type \ref bt_a2dp_codec_type */
    u8_t codec_type;
    /** Stream End Point media codec information len */
    u16_t codec_info_len;
    /** Stream End Point media codec information */
    u8_t *codec_info;
};

/** @brief AVDTP Stream End Point capability*/
struct bt_avdtp_cap {
    /** Service Catagory bit map */
    u16_t service_cat_bitmap;
    /** media codec capability */
    struct bt_avdtp_media_codec_cap *media_codec;
};

/** @brief AVDTP Local SEP structure */
struct bt_avdtp_seid_lsep {
    /** Stream End Point information */
    struct bt_avdtp_seid_info sep;
    /** Stream End Point capalility */
    struct bt_avdtp_cap *caps;
    /** Stream End Point configuration */
    struct bt_avdtp_cap *conf;
    /** Pointer to next local Stream End Point structure */
    struct bt_avdtp_seid_lsep *next;
};

/** @brief AVDTP signal indicate callback. */
struct bt_avdtp_ind_cb {
    int (*connected)(struct bt_avdtp *session);
    int (*disconnected)(struct bt_avdtp *session);
    /** set configuration indicate callback*/
    int (*set_config_ind)(struct bt_avdtp *session, struct bt_avdtp_cap *caps);
    /** stream open indicate callback*/
    int (*open_ind)(struct bt_avdtp *session);
    /** stream start indicate callback*/
    int (*start_ind)(struct bt_avdtp *session);
    /** stream suspend indicate callback*/
    int (*suspend_ind)(struct bt_avdtp *session);
    /** stream close indicate callback*/
    int (*close_ind)(struct bt_avdtp *session);
    /** stream abort indicate callback*/
    int (*abort_ind)(struct bt_avdtp *session);
};

/** @brief AVDTP Stream structure */
struct bt_avdtp_stream {
    /** Transport Channel*/
    struct bt_l2cap_br_chan chan;
    /** Configured Local SEP id */
    u8_t lseid;
    /** Configured Remote SEP*/
    u8_t rseid;
    /** Current state of the stream */
    u8_t state;
    /** Pointer to next stream */
    struct bt_avdtp_stream *next;
};

/** @brief Global AVDTP session structure. */
struct bt_avdtp {
    /** l2cap channel */
    struct bt_l2cap_br_chan br_chan;
    /** List of AV streams */
    struct bt_avdtp_stream *streams;
    /** AVDTP signal request */
    struct bt_avdtp_req *req;
};

/** @brief AVDTP event callback structure. */
struct bt_avdtp_event_cb {
    /** Callback for incoming signal requests*/
    struct bt_avdtp_ind_cb *ind;
    /** media data */
    int (*data)(struct bt_avdtp_stream *stream, struct bt_avdtp_seid_lsep *sep, struct net_buf *buf);
    /** accept avdtp connection and return session */
    int (*accept)(struct bt_conn *conn, struct bt_avdtp **session);
};

/** @brief AVDTP media patcket structure referenced from RFC 3550.*/
struct bt_avdtp_media_hdr {
    /** 
     * header format
     *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |V=2|P|X|   CC  |M|      PT     |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     */
    u16_t hdr;
    /** Sequence Number */
    u16_t sn;
    /** Time Stamp */
    u32_t ts;
    /** SSRC */
    u32_t ssrc;
    /** media payload */
    u8_t payload[0];
};

/**
 * @brief Initialize AVDTP layer
 * @return int    0: success; other: failed
 */
int bt_avdtp_init(void);

/**
 * @brief      Application register with AVDTP layer
 * @param[in]  cb       Upper Layer callback structure
 * @return     int      0: success; other: failed
 */
int bt_avdtp_register(struct bt_avdtp_event_cb *cb);

/**
 * @brief      AVDTP connect
 * @param[in]  conn     ACL connection
 * @param[in]  session  avdtp l2cap connect session
 * @return     int      0: success; other: failed
 */
int bt_avdtp_connect(struct bt_conn *conn, struct bt_avdtp *session);

/**
 * @brief      AVDTP disconnect
 * @param[in]  session  avdtp l2cap connect session
 * @return     int      0: success; other: failed
 */
int bt_avdtp_disconnect(struct bt_avdtp *session);

/**
 * @brief      AVDTP SEP register function
 * @param[in]  media_type  media type for bt_avdtp_media_type
 * @param[in]  role        role for bt_avdtp_role_type
 * @param[in]  sep         Upper Layer callback structure
 * @return     int         0: success; other: failed
 */
int bt_avdtp_register_sep(u8_t media_type, u8_t role, struct bt_avdtp_seid_lsep *sep);

/**
 * @brief      AVDTP Discover Request
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_discover(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Get capabilites
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_get_capabilities(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Set capabilites
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @param[in]  conf     stream end point capabilities configuration
 * @return     int      0: success; other: failed
 */
int bt_avdtp_set_configuration(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb,
                               struct bt_avdtp_cap *conf);

/**
 * @brief      AVDTP Set capabilites
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_get_configuration(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Reconfig
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_reconfig(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb,
                      struct bt_avdtp_cap *conf);

/**
 * @brief      AVDTP Stream open
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_stream_open(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Stream start
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_stream_start(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Stream suspend
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_stream_suspend(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Stream close
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_stream_close(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Stream abort
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_stream_abort(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb);

/**
 * @brief      AVDTP Delay report
 * @param[in]  session      avdtp l2cap connect session
 * @param[in]  cb           signal requset confirm callback
 * @param[in]  delay_100us  delay time in 100us
 * @return     int      0: success; other: failed
 */
int bt_avdtp_delay_report(struct bt_avdtp *session, bt_avdtp_cmf_cb_t cb,
                          u16_t delay_100us);

/**
 * @brief      AVDTP create mediat packet, add media head to packet
 * @param[in]  session  avdtp l2cap connect session
 * @return     int      0: success; other: failed
 */
struct net_buf *bt_avdtp_create_media_packet(struct bt_avdtp *session);

/**
 * @brief      AVDTP send media packet to SNK
 * @param[in]  session  avdtp l2cap connect session
 * @param[in]  cb       signal requset confirm callback
 * @return     int      0: success; other: failed
 */
int bt_avdtp_steam_send_media(struct bt_avdtp *session, struct net_buf *buf);

#endif /* __AVDTP_INTERNAL_H__ */
