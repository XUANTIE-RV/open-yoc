/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#ifndef __AVRCP_INTERNAL_H__
#define __AVRCP_INTERNAL_H__

#define BT_AVCTP_VERSION 0x104
#define BT_AVRCP_VERSION 0x106

#define BT_SIG_AVC_COMPANY_ID 0x001958

#define BT_L2CAP_PSM_AVCTP          0x0017
#define BT_L2CAP_PSM_AVCTP_BROWSING 0x001B

#define BT_AVRCP_SUBUNIT_TYPE_UNIT  0x1F
#define BT_AVRCP_SUBUNIT_TYPE_PANEL 0x09
#define BT_AVRCP_SUBUNIT_ID         0
#define BT_AVRCP_SUBUNIT_ID_IGNORE  7

#define BT_AVRCP_MAX_MTU BT_L2CAP_RX_MTU

/** @brief AVCTP packet head command/respone type
 */
enum bt_avdtp_cr {
    BT_AVCTP_COMMAND = 0,
    BT_AVCTP_RESPONSE,
};

/** @brief AVRCP response status codetype
 */
enum bt_avrcp_status_code_t {
    BT_AVRCP_STATUS_INVALID_COMMAND = 0,
    BT_AVRCP_STATUS_INVALID_PARAMETER,
    BT_AVRCP_STATUS_SPECIFIED_PARAMETER_NOT_FOUND,
    BT_AVRCP_STATUS_INTERNAL_ERROR,
    BT_AVRCP_STATUS_SUCCESS,
    BT_AVRCP_STATUS_UID_CHANGED,
    BT_AVRCP_STATUS_RESERVED_6,
    BT_AVRCP_STATUS_INVALID_DIRECTION,
    BT_AVRCP_STATUS_NOT_A_DIRECTORY,
    BT_AVRCP_STATUS_DOES_NOT_EXIST,
    BT_AVRCP_STATUS_INVALID_SCOPE,
    BT_AVRCP_STATUS_RANGE_OUT_OF_BOUNDS,
    BT_AVRCP_STATUS_UID_IS_A_DIRECTORY,
    BT_AVRCP_STATUS_MEDIA_IN_USE,
    BT_AVRCP_STATUS_NOW_PLAYING_LIST_FULL,
    BT_AVRCP_STATUS_SEARCH_NOT_SUPPORTED,
    BT_AVRCP_STATUS_SEARCH_IN_PROGRESS,
    BT_AVRCP_STATUS_INVALID_PLAYER_ID,
    BT_AVRCP_STATUS_PLAYER_NOT_BROWSABLE,
    BT_AVRCP_STATUS_PLAYER_NOT_ADDRESSED,
    BT_AVRCP_STATUS_NO_VALID_SEARCH_RESULTS,
    BT_AVRCP_STATUS_NO_AVAILABLE_PLAYERS,
    BT_AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED,
    BT_AVRCP_STATUS_RESERVED,
};

/** @brief AVRCP vendor depenent opecode pdu id
 */
enum bt_avrcp_pdu_id {
    BT_AVRCP_PDU_ID_GET_CAPABILITIES                              = 0x10,
    BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES    = 0x11,
    BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUE         = 0x12,
    BT_AVRCP_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE  = 0x13,
    BT_AVRCP_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE          = 0x14,
    BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT = 0x15,
    BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT     = 0x16,
    BT_AVRCP_PDU_ID_INFORM_DISPLAYABLE_CHARACTER_SET              = 0x17,
    BT_AVRCP_PDU_ID_INFORM_BATTERY_STATUS_OF_CT                   = 0x18,
    BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES                        = 0x20,
    BT_AVRCP_PDU_ID_GET_PLAY_STATUS                               = 0x30,
    BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION                         = 0x31,
    BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE                   = 0x40,
    BT_AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE             = 0x41,
    BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME                           = 0x50,
    BT_AVRCP_PDU_ID_SET_ADDRESSED_PLAYER                          = 0x60,
    BT_AVRCP_PDU_ID_SET_BROWSED_PLAYER                            = 0x70,
    BT_AVRCP_PDU_ID_GET_FOLDER_ITEMS                              = 0x71,
    BT_AVRCP_PDU_ID_CHANGE_PATH                                   = 0x72,
    BT_AVRCP_PDU_ID_GET_ITEM_ATTRIBUTES                           = 0x73,
    BT_AVRCP_PDU_ID_PLAY_ITEM                                     = 0x74,
    BT_AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS                     = 0x75,
    BT_AVRCP_PDU_ID_SEARCH                                        = 0x80,
    BT_AVRCP_PDU_ID_ADD_TO_NOW_PLAYING                            = 0x90,
    BT_AVRCP_PDU_ID_GENERAL_REJECT                                = 0xA0,
};

/** @brief AVCTP packet type
 */
enum bt_avctp_packet_type {
    BT_AVCTP_SINGLE_PACKET = 0,
    BT_AVCTP_START_PACKET,
    BT_AVCTP_CONTINUE_PACKET,
    BT_AVCTP_END_PACKET,
};

/** @brief AVRCP packet type
 */
enum bt_avrcp_packet_type {
    BT_AVRCP_SINGLE_PACKET = 0,
    BT_AVRCP_START_PACKET,
    BT_AVRCP_CONTINUE_PACKET,
    BT_AVRCP_END_PACKET,
};

/** @brief AV/C opcode
 */
enum bt_avctp_opcode {
    BT_AVCTP_OPCODE_VENDOR_DEPENDENT = 0,
    BT_AVCTP_OPCODE_UNIT_INFO        = 0x30,
    BT_AVCTP_OPCODE_SUBUNIT_INFO     = 0x31,
    BT_AVCTP_OPCODE_PASS_THROUGH     = 0x7C,
};

/** @brief AVRCP get capablitity pdu capablitity
 */
enum bt_avrcp_capablitity_id {
    BT_AVRCP_CAPABLITITY_COMPANY_ID = 0x02,
    BT_AVRCP_CAPABLITITY_EVENT_ID   = 0x03,
};

/** @brief AVCTP signal command and response headers for single packets
 * revert struct member in byte for memory order.
 * so that the memory in packet could map to the structure.
 */
struct bt_avctp_hdr {
    /** Invaild PID */
    u8_t IPID : 1;
    /** command/response \ref bt_avdtp_cr */
    u8_t cr : 1;
    /** packet Type \ref bt_avctp_packet_type */
    u8_t packtype : 2;
    /** Transation Lable 0 ~ 15 */
    u8_t tl : 4;
    /** Profile Identifier == BT_SDP_AV_REMOTE_SVCLASS */
    u16_t pid;
    /* payload for av/c */
    u8_t payload[0];
} __packed;

/** @brief AV/C packet head Structure
 */
struct bt_avc_hdr {
    /** Ctype/Response \ref bt_avrcp_ctype_response */
    u8_t cr : 4;
    /** Reserved */
    u8_t rfa : 4;
    /** subinit == 0 */
    u8_t subinit : 3;
    /** Subunit_type == 9 */
    u8_t Subunit_type : 5;
    /** opcode \ref bt_avctp_opcode */
    u8_t opcode;
} __packed;

/** @brief AVRCP packet head Structure
 */
struct bt_avrcp_hdr {
    /** Bluetooth SIG registered CompanyID == 0x001958 */
    u8_t company_id[3];
    /** pdu_id \ref bt_avrcp_pdu_id */
    u8_t pdu_id;
    /** packet_type \ref bt_avrcp_packet_type */
    u8_t packet_type : 2;
    /** Reserved */
    u8_t rfa : 6;
} __packed;

/** @brief AVRCP Pass Through packet data Structure
 */
struct bt_avrcp_pass_througt_packet {
    /** operation_id \ref bt_prf_avrcp_op_id_t */
    u8_t operation_id : 7;
    /** operation_state \ref bt_prf_avrcp_op_stat_t */
    u8_t operation_state : 1;
    /** operation_data_field_lengh == 0 */
    u8_t field_len;
} __packed;

/** @brief AVRCP Subunit Info respense packet data Structure
 */
struct bt_avc_subunit_info_resp_packet {
    /** extention code */
    u8_t extention_code: 3;
    /** Reserved */
    u8_t rfa1: 1;
    /** page */
    u8_t page: 3;
    /** Reserved */
    u8_t rfa2: 1;
    /** uint */
    u8_t max_subunit_id : 3;
    /** uint type: panel */
    u8_t subunit_type : 5;
    /** constant 0xFF */
    u8_t constant_FF[3];
} __packed;

/** @brief AVRCP Unit Info respense packet data Structure
 */
struct bt_avc_unit_info_resp_packet {
    /** constant 7 */
    u8_t constant_7;
    /** uint */
    u8_t unit : 3;
    /** uint type: panel */
    u8_t unit_type : 5;
    /** Bluetooth SIG registered CompanyID == 0x001958 */
    u8_t company_id[3];
} __packed;

/** @brief AVRCP Register norification packet data Structure
 */
struct bt_avrcp_register_notification_packet {
    /** Notifucation event id  \ref bt_prf_avrcp_rn_event_ids_t */
    u8_t event_id;
    /** playback interval */
    u32_t interval;
} __packed;

/** @brief AVRCP Set Absolute Volume packet data Structure
 */
struct bt_avrcp_absolute_volume_packet {
    /** absolute volume range: 0 ~ 127 */
    u8_t abs_volume;
} __packed;

/** @brief AVRCP Set Absolute Volume packet data Structure
 */
struct bt_avrcp_metadata_req_packet {
    /** absolute volume range: 0 ~ 127 */
    u64_t identifier;
    u8_t attrnum;
    uint32_t attr[BT_AVRCP_MAX_METADETA_ATTR_SIZE];
} __packed;

/** @brief AVRCP Get capablitity request data Structure
 */
struct bt_avrcp_get_capablitity_req_packet {
    /** capablitity id \ref bt_avrcp_register_notification_packet */
    u8_t capablitity_id;
} __packed;

/** @brief AVRCP Connection Session Structure
 */
struct bt_avrcp {
    /** l2cap channel */
    struct bt_l2cap_br_chan br_chan;
};

/** @brief AVRCP signal indicate callback. */
struct bt_avrcp_ind_cb {
    int (*connected)(struct bt_avrcp *session);
    int (*disconnected)(struct bt_avrcp *session);
};

/** @brief AVRCP event callback structure. */
struct bt_avrcp_event_cb {
    /** Callback for incoming signal requests*/
    const struct bt_avrcp_ind_cb *ind;
    /** packet data */
    const int (*data)(struct bt_avrcp *session, uint8_t tl, struct net_buf *buf);
};

/** @brief AVRCP Iint function
 */
int bt_avrcp_init(void);

/** @brief AVRCP Get Transcation Lable
 */
uint8_t avrcp_get_tl(void);

/** @brief AVRCP register event callback from controller
 */
void bt_avrcp_ct_register(const struct bt_avrcp_event_cb *cb);

/** @brief AVRCP register event callback from target
 */
void bt_avrcp_tg_register(const struct bt_avrcp_event_cb *cb);

/** @brief AVRCP create general pdu
 */
struct net_buf *avrcp_create_pdu(u8_t cr, u8_t opcode, u8_t ctype, u8_t tl);

/** @brief AVRCP create pass through packet */
struct net_buf *bt_avrcp_create_pass_through_pdu(u8_t operation_id, u8_t operation_state, u8_t tl, u8_t ctype);

/** @brief AVRCP vendor depenent responese packet */
struct net_buf *bt_avrcp_create_vendor_depenent_pdu(u8_t pdu_id, void *param, u16_t param_len, u8_t tl, u8_t response);

/** @brief AVRCP send packet */
int bt_avrcp_send(struct net_buf *buf);

#endif /* __AVRCP_INTERNAL_H__ */
