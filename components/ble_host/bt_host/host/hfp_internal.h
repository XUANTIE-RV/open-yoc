/** @file
 *  @brief Internal APIs for Bluetooth Handsfree profile handling.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aos/classic/bt_stack_hfp_hf.h"

#define BT_HFP_HF_VERSION    0x108
#define BT_HFP_MAX_MTU       140
#define BT_HF_CLIENT_MAX_PDU BT_HFP_MAX_MTU

/* HFP AG Features */
#define BT_HFP_AG_FEATURE_3WAY_CALL   0x00000001 /* Three-way calling */
#define BT_HFP_AG_FEATURE_ECNR        0x00000002 /* EC and/or NR function */
#define BT_HFP_AG_FEATURE_VOICE_RECG  0x00000004 /* Voice recognition */
#define BT_HFP_AG_INBAND_RING_TONE    0x00000008 /* In-band ring capability */
#define BT_HFP_AG_VOICE_TAG           0x00000010 /* Attach no. to voice tag */
#define BT_HFP_AG_FEATURE_REJECT_CALL 0x00000020 /* Ability to reject call */
#define BT_HFP_AG_FEATURE_ECS         0x00000040 /* Enhanced call status */
#define BT_HFP_AG_FEATURE_ECC         0x00000080 /* Enhanced call control */
#define BT_HFP_AG_FEATURE_EXT_ERR     0x00000100 /* Extented error codes */
#define BT_HFP_AG_FEATURE_CODEC_NEG   0x00000200 /* Codec negotiation */
#define BT_HFP_AG_FEATURE_HF_IND      0x00000400 /* HF Indicators */
#define BT_HFP_AG_FEARTURE_ESCO_S4    0x00000800 /* eSCO S4 Settings */

/* HFP HF Features */
#define BT_HFP_HF_FEATURE_ECNR       0x00000001 /* EC and/or NR */
#define BT_HFP_HF_FEATURE_3WAY_CALL  0x00000002 /* Three-way calling */
#define BT_HFP_HF_FEATURE_CLI        0x00000004 /* CLI presentation */
#define BT_HFP_HF_FEATURE_VOICE_RECG 0x00000008 /* Voice recognition */
#define BT_HFP_HF_FEATURE_VOLUME     0x00000010 /* Remote volume control */
#define BT_HFP_HF_FEATURE_ECS        0x00000020 /* Enhanced call status */
#define BT_HFP_HF_FEATURE_ECC        0x00000040 /* Enhanced call control */
#define BT_HFP_HF_FEATURE_CODEC_NEG  0x00000080 /* CODEC Negotiation */
#define BT_HFP_HF_FEATURE_HF_IND     0x00000100 /* HF Indicators */
#define BT_HFP_HF_FEATURE_ESCO_S4    0x00000200 /* eSCO S4 Settings */

/* HFP HF extended call handling - masks not related to any spec */
#define BT_HF_CLIENT_CHLD_REL          0x00000001 /* 0  Release waiting call or held calls */
#define BT_HF_CLIENT_CHLD_REL_ACC      0x00000002 /* 1  Release active calls and accept other (waiting or held) cal */
#define BT_HF_CLIENT_CHLD_REL_X        0x00000004 /* 1x Release x call*/
#define BT_HF_CLIENT_CHLD_HOLD_ACC     0x00000008 /* 2  Active calls on hold and accept other call */
#define BT_HF_CLIENT_CHLD_PRIV_X       0x00000010 /* 2x Active multiparty call on hold except call x */
#define BT_HF_CLIENT_CHLD_MERGE        0x00000020 /* 3  Add held call to multiparty */
#define BT_HF_CLIENT_CHLD_MERGE_DETACH 0x00000040 /* 4  Add held call to multiparty */

/* HFP HF Supported features */
#define BT_HFP_HF_SUPPORTED_FEATURES                                                                                   \
    (BT_HFP_HF_FEATURE_CLI | BT_HFP_HF_FEATURE_VOLUME | BT_HFP_HF_FEATURE_3WAY_CALL | BT_HFP_HF_FEATURE_CODEC_NEG)

#define HF_MAX_BUF_LEN       BT_HF_CLIENT_MAX_PDU
#define HF_MAX_AG_INDICATORS 20

typedef enum {
    HFP_HF_AT_NONE,
    HFP_HF_AT_BRSF,
    HFP_HF_AT_BAC,
    HFP_HF_AT_CIND,
    HFP_HF_AT_CIND_STATUS,
    HFP_HF_AT_CMER,
    HFP_HF_AT_CHLD,
    HFP_HF_AT_CMEE,
    HFP_HF_AT_BIA,
    HFP_HF_AT_CLIP,
    HFP_HF_AT_CCWA,
    HFP_HF_AT_COPS,
    HFP_HF_AT_CLCC,
    HFP_HF_AT_BVRA,
    HFP_HF_AT_VGS,
    HFP_HF_AT_VGM,
    HFP_HF_AT_ATD,
    HFP_HF_AT_BLDN,
    HFP_HF_AT_ATA,
    HFP_HF_AT_CHUP,
    HFP_HF_AT_BTRH,
    HFP_HF_AT_VTS,
    HFP_HF_AT_BCC,
    HFP_HF_AT_BCS,
    HFP_HF_AT_CNUM,
    HFP_HF_AT_NREC,
    HFP_HF_AT_BINP,
} hfp_hf_at_cmd_t;

struct bt_hfp_hf {
    struct bt_rfcomm_dlc             rfcomm_dlc;
    char                             hf_buffer[HF_MAX_BUF_LEN];
    struct at_client                 at;
    struct bt_conn *                 sco_conn;
    u32_t                            hf_features;
    u32_t                            ag_features;
    u32_t                            chld_features;
    s8_t                             ind_table[HF_MAX_AG_INDICATORS];
    bt_prf_hfp_hf_connection_state_t connection_state;
    hfp_hf_at_cmd_t                  last_cmd;
    uint8_t                          negotiated_codec;
};

enum hfp_hf_ag_indicators {
    HF_SERVICE_IND,
    HF_CALL_IND,
    HF_CALL_SETUP_IND,
    HF_CALL_HELD_IND,
    HF_SINGNAL_IND,
    HF_ROAM_IND,
    HF_BATTERY_IND
};
