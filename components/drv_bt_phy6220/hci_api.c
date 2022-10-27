/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "string.h"
#include <csi_core.h>
#include "ll.h"
#include "hci.h"
#include "hci_api.h"
#include "hci.h"
#include "ll_def.h"
#include "aos/aos.h"
#include "bluetooth/addr.h"
#include "global_config.h"
#include <ll_buf.h>
#include <rom_sym_def.h>
//#include <OSAL_Timers.h>

#ifndef BASE_TIME_UINTS
#define BASE_TIME_UNITS   (0x3fffff)
#endif

typedef uint8_t hciStatus_t;

extern uint8_t llState, llSecondaryState;

#ifndef ENOTSUP
#define ENOTSUP 35
#endif


extern unsigned int irq_lock(void);
extern void irq_unlock(unsigned int key);

uint8 patchRandomAddr[ LL_DEVICE_ADDR_LEN ] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

hciStatus_t HCI_LE_LL_ReadMaxDataLengthCmd(uint16 *TxOctets, uint16 *TxTime, uint16 *RxOctets, uint16 *RxTime)
{
    if (NULL == TxOctets || NULL == TxTime || NULL == RxOctets || NULL == RxTime) {
        return LL_STATUS_ERROR_BAD_PARAMETER;
    }

    *TxOctets = LL_PDU_LENGTH_SUPPORTED_MAX_TX_OCTECTS;
    *TxTime = LL_PDU_LENGTH_SUPPORTED_MAX_TX_TIME;
    *RxOctets = LL_PDU_LENGTH_SUPPORTED_MAX_RX_OCTECTS;
    *RxTime = LL_PDU_LENGTH_SUPPORTED_MAX_RX_TIME;

    return 0;
}
hciStatus_t HCI_LL_ReadLocalSupportedFeaturesCmd(uint8 *feature)
{
    // 0:    Status
    // 1..8: Supported Features
    memset(feature, 0, 8);

    // set byte 4 of the feature list, which is the only byte that matters
    feature[4] = 0x60;

    return (0);
}

extern uint32_t read_current_fine_time(void);

static inline int wait_llWaitingIrq(uint16_t ms)
{
    extern uint32_t llWaitingIrq;
    volatile uint32_t *pwait_flag = &llWaitingIrq;

    uint32_t t0 = read_current_fine_time();
    uint32_t t1 = 0, tmp = 0;

    while (*pwait_flag) {
        tmp = read_current_fine_time();
        t1 = tmp >= t0 ? tmp - t0 : BASE_TIME_UNITS - t0 + tmp;

        if (t1 > ms * 1000) {
            return LL_STATUS_ERROR_COMMAND_DISALLOWED;
        }
    }

    return 0;
}

int hci_api_le_scan_enable(uint8_t enable, uint8_t filter_dup)
{
    if (enable) {
        int ret = wait_llWaitingIrq(100);

        if (ret) {
            return ret;
        }
    }

    int ret = LL_SetScanControl(enable, filter_dup);

    return ret;
}

int hci_api_le_scan_param_set(uint8_t scan_type, uint16_t interval, uint16_t window, uint8_t addr_type, uint8_t filter_policy)
{
    return LL_SetScanParam(scan_type, interval, window, addr_type, filter_policy);
}

int hci_api_le_get_max_data_len(uint16_t *tx_octets, uint16_t *tx_time)
{
    uint16_t rx_octets = 0;
    uint16_t rx_time = 0;

    return HCI_LE_LL_ReadMaxDataLengthCmd(tx_octets, tx_time, &rx_octets, &rx_time);
}

int bt_set_bdaddr(const bt_addr_le_t *addr)
{
    return hci_api_le_set_bdaddr((uint8_t *)addr->a.val);
}

int hci_api_le_get_default_data_len(uint16_t *tx_octets, uint16_t *tx_time)
{
    return -ENOTSUP;
}

int hci_api_le_set_default_data_len(uint16_t tx_octets, uint16_t tx_time)
{
    return LL_WriteSuggestedDefaultDataLength(tx_octets, tx_time);
}

int hci_api_le_set_data_len(int16_t conn_handle, uint16_t tx_octets, uint16_t tx_time)
{
    return LL_SetDataLengh(conn_handle, tx_octets, tx_time);
}

int hci_api_le_set_bdaddr(uint8_t bdaddr[6])
{
    extern uint32 *pGlobal_config;
    static uint8_t bd_addr[6] = {0};
    bd_addr[3] = bdaddr[0];
    bd_addr[2] = bdaddr[1];
    bd_addr[1] = bdaddr[2];
    bd_addr[0] = bdaddr[3];
    bd_addr[5] = bdaddr[4];
    bd_addr[4] = bdaddr[5];
    pGlobal_config[MAC_ADDRESS_LOC] = (uint32_t)bd_addr;
    return 0;
}

int hci_api_reset()
{
    LL_Reset();
    extern void llInitFeatureSetDLE(uint8 enable);
    llInitFeatureSetDLE(1);
    return 0;
}

int hci_api_read_local_feature(uint8_t feature[8])
{
    extern hciStatus_t HCI_LL_ReadLocalSupportedFeaturesCmd(uint8 * feature);
    return HCI_LL_ReadLocalSupportedFeaturesCmd(feature);
}

#define HCI_VERSION                                  0x09    // BT Core Specification V5.0
//0x06    // BT Core Specification V4.0

// Major Version (8 bits) . Minor Version (4 bits) . SubMinor Version (4 bits)
#define HCI_REVISION                                 0x0120  // HCI Version 1.2.0

int hci_api_read_local_version_info(uint8_t *hci_version,
                                    uint8_t            *lmp_version,
                                    uint16_t               *hci_revision,
                                    uint16_t               *lmp_subversion,
                                    uint16_t           *manufacturer)
{
    LL_ReadLocalVersionInfo(lmp_version,
                            manufacturer,
                            lmp_subversion);
    *hci_version = HCI_VERSION;
    *hci_revision = HCI_REVISION;
    return 0;
}

int hci_api_read_bdaddr(uint8_t bdaddr[6])
{
    return LL_ReadBDADDR(bdaddr);
}

typedef const uint8 supportedCmdsTable_t;
#define SUPPORTED_COMMAND_LEN 64
extern supportedCmdsTable_t supportedCmdsTable[SUPPORTED_COMMAND_LEN + 1];


int HCI_LL_ReadLocalSupportedCommandsCmd(uint8 *support_commands)
{
    memcpy(support_commands, (uint8 *)(supportedCmdsTable + 1), SUPPORTED_COMMAND_LEN);
    return (0);
}


int hci_api_read_local_support_command(uint8_t supported_commands[64])
{
    int ret;

    ret = HCI_LL_ReadLocalSupportedCommandsCmd(supported_commands);
#ifdef CONFIG_BT_ECC
    supported_commands[34] |= 0x02;
    supported_commands[34] |= 0x04;
#endif
    return ret;
}

int hci_api_set_host_buffer_size(uint16_t acl_mtu, uint8_t  sco_mtu,
                                 uint16_t acl_pkts, uint16_t sco_pkts)
{
    extern uint16 numHostBufs;
    numHostBufs = acl_pkts;
    return 0;
}

int hci_api_set_host_flow_enable(uint8_t enable)
{
    extern uint8  ctrlToHostEnable;
    ctrlToHostEnable = enable ? 1 : 0;
    return 0;
}

int hci_api_le_read_local_feature(uint8_t feature[8])
{
    int ret = LL_ReadLocalSupportedFeatures(feature);
    //feature[0] |= (BIT(6)); //enable LL privacy
    return ret;
}

int hci_api_le_read_support_states(uint64_t *states)
{
    return LL_ReadSupportedStates((uint8_t *)states);
}

int hci_api_le_read_rl_size(uint8_t *rl_size)
{
    return LL_ReadResolvingListSize(rl_size);
}

int hci_api_le_set_event_mask(uint64_t mask)
{
    extern uint32 bleEvtMask;
    bleEvtMask = (uint32)(mask & 0xFFFFFFFF);
    return 0;
}

int hci_api_set_event_mask(uint64_t mask)
{
    extern uint8 pHciEvtMask[8];
    memcpy(pHciEvtMask, (uint8_t *)(&mask), 8);
    return 0;
}

int hci_api_vs_init()
{
    return 0;
}

int hci_api_le_adv_enable(uint8_t enable)
{
    int ret;

    if (enable) {
        int ret = wait_llWaitingIrq(100);
        if (ret) {
            return ret;
        }
    }

    ret = LL_SetAdvControl(enable);

    if (llSecondaryState == LL_SEC_STATE_IDLE_PENDING) {
        llSecondaryState = LL_SEC_STATE_IDLE;
    }else {
       osal_stop_timerEx(LL_TaskID, LL_EVT_SECONDARY_ADV);
    }

    if (ret == 0) {
        extern struct buf_rx_desc g_rx_adv_buf;
        g_rx_adv_buf.rxheader = 0;
    }

    return ret;
}

int hci_api_le_adv_param(uint16_t        min_interval,
                         uint16_t        max_interval,
                         uint8_t         type,
                         uint8_t         own_addr_type,
                         uint8_t  direct_addr_type,
                         uint8_t  direct_addr[6],
                         uint8_t         channel_map,
                         uint8_t         filter_policy)
{
    uint32_t t0 = read_current_fine_time();
    uint32_t t1 = 0, tmp = 0;

    while ((llState != LL_STATE_IDLE && llState != LL_STATE_CONN_SLAVE && llState != LL_STATE_CONN_MASTER) ||
           ((llState == LL_STATE_CONN_SLAVE || llState == LL_STATE_CONN_MASTER) && llSecondaryState != LL_SEC_STATE_IDLE)) {
        tmp = read_current_fine_time();
        t1 = tmp >= t0 ? tmp - t0 : BASE_TIME_UNITS - t0 + tmp;

        if (t1 > 100 * 1000) {
            return LL_STATUS_ERROR_COMMAND_DISALLOWED;
        }
    }

    if (adv_param.advMode == 1)
    {
        /* Try to fix advMode missmatch bug */
        adv_param.advMode = 0;
    }

    return LL_SetAdvParam(min_interval,
                          max_interval,
                          type,
                          own_addr_type,
                          direct_addr_type,
                          direct_addr,
                          channel_map,
                          filter_policy);
}

int hci_api_le_set_random_addr(const uint8_t addr[6])
{
    for (int i = 0; i < 6; i++) {
        patchRandomAddr[i] = addr[i];
    }

    return LL_SetRandomAddress((uint8 *)addr);
}

int hci_api_le_set_ad_data(uint8_t len, uint8_t data[31])
{
    return LL_SetAdvData(len, data);
}

int hci_api_le_set_sd_data(uint8_t len, uint8_t data[31])
{
    return LL_SetScanRspData(len, data);
}

int hci_api_le_create_conn(uint16_t scan_interval,
                           uint16_t scan_window,
                           uint8_t filter_policy,
                           uint8_t peer_addr_type,
                           const uint8_t peer_addr[6],
                           uint8_t own_addr_type,
                           uint16_t conn_interval_min,
                           uint16_t conn_interval_max,
                           uint16_t conn_latency,
                           uint16_t supervision_timeout,
                           uint16_t min_ce_len,
                           uint16_t max_ce_len)
{
    int ret = wait_llWaitingIrq(100);

    if (ret) {
        return ret;
    }

    return LL_CreateConn(scan_interval,
                         scan_window,
                         filter_policy,
                         peer_addr_type,
                         (uint8_t *)peer_addr,
                         own_addr_type,
                         conn_interval_min,
                         conn_interval_max,
                         conn_latency,
                         supervision_timeout,
                         min_ce_len,
                         max_ce_len);
}

int hci_api_le_create_conn_cancel()
{
    return LL_CreateConnCancel();
}

int hci_api_le_disconnect(uint16_t conn_handle, uint8_t reason)
{
    return LL_Disconnect(conn_handle, reason);
}

int hci_api_le_read_remote_features(uint16_t conn_handle)
{
    return LL_ReadRemoteUsedFeatures(conn_handle);
}

int hci_api_host_num_complete_packets(uint8_t num_handles, struct handle_count *phc)
{
    extern hciStatus_t HCI_HostNumCompletedPktCmd(uint8  numHandles,
            uint16 * connHandles,
            uint16 * numCompletedPkts);
    return HCI_HostNumCompletedPktCmd(num_handles, &phc->handle, &phc->count);
}

int hci_api_le_conn_updata(uint16_t conn_handle,
                           uint16_t conn_interval_min,
                           uint16_t conn_interval_max,
                           uint16_t conn_latency,
                           uint16_t supervision_timeout,
                           uint16_t min_ce_len,
                           uint16_t max_ce_len)
{
//    return LL_ConnUpdate(conn_handle,
//                         conn_interval_min,
//                         conn_interval_max,
//                         conn_latency,
//                         supervision_timeout,
//                         min_ce_len,
//                         max_ce_len);
   // extern size_t csi_irq_save();
  //  extern void csi_irq_restore(size_t psr);
    uint32_t cs;
    cs = csi_irq_save();

    //walkaround LL_ConnUpdata bug in ROM
    extern llConns_t    g_ll_conn_ctx;
    g_ll_conn_ctx.scheduleInfo[conn_handle].linkRole = LL_ROLE_INVALID;
    uint8_t ret = LL_ConnUpdate(conn_handle,
                                conn_interval_min,
                                conn_interval_max,
                                conn_latency,
                                supervision_timeout,
                                min_ce_len,
                                max_ce_len);
    g_ll_conn_ctx.scheduleInfo[conn_handle].linkRole = LL_ROLE_MASTER;
    csi_irq_restore(cs);
    return ret;

}

int hci_api_le_start_encrypt(uint16_t conn_handle,
                             uint8_t rand[8],
                             uint8_t ediv[2],
                             uint8_t  ltk[16])
{
    return LL_StartEncrypt(conn_handle, rand,  ediv, ltk);
}

int hci_api_le_enctypt_ltk_req_reply(uint16_t conn_handle, uint8_t  ltk[16])
{
    return LL_EncLtkReply(conn_handle, ltk);
}

int hci_api_le_enctypt_ltk_req_neg_reply(uint16_t conn_handle)
{
    return LL_EncLtkNegReply(conn_handle);
}


int hci_api_le_rand(uint8_t random_data[8])
{
    extern uint8_t TRNG_Rand(uint8_t* buf, uint8 len);
	uint8_t status = 0;
    unsigned int irq_key = 0;
    irq_key = irq_lock();
	status = TRNG_Rand(random_data, 8);
    irq_unlock(irq_key);
    return status;
}

int hci_api_le_enc(uint8_t key[16], uint8_t plaintext[16], uint8_t ciphertext[16])
{
    uint8_t status = 0;
	unsigned int irq_key = 0;
    irq_key = irq_lock();
    status = LL_Encrypt(key, plaintext, ciphertext);
	irq_unlock(irq_key);
	return status;
}

int hci_api_le_set_phy(uint16_t  handle,
                       uint8_t   all_phys,
                       uint8_t   tx_phys,
                       uint8_t   rx_phys,
                       uint16_t  phy_opts)
{
    return LL_SetPhyMode(handle, all_phys, tx_phys, rx_phys, phy_opts);
}

int hci_api_le_conn_param_req_reply(uint16_t handle,
                                    uint16_t interval_min,
                                    uint16_t interval_max,
                                    uint16_t latency,
                                    uint16_t timeout,
                                    uint16_t min_ce_len,
                                    uint16_t max_ce_len)
{
    return -ENOTSUP;
}

int hci_api_le_conn_param_neg_reply(uint16_t handle, uint8_t reason)
{
    return -ENOTSUP;
}

int hci_api_le_add_dev_to_rl(uint8_t type,
                             uint8_t peer_id_addr[6],
                             uint8_t peer_irk[16],
                             uint8_t local_irk[16])
{
    return LL_AddResolvingListLDevice(type, peer_id_addr, peer_irk, local_irk);
}

int hci_api_le_remove_dev_from_rl(uint8_t type,
                                  const uint8_t peer_id_addr[6])
{
    return LL_RemoveResolvingListDevice((uint8_t *)peer_id_addr, type);
}

int hci_api_le_clear_rl()
{
    return LL_ClearResolvingList();
}

int hci_api_le_set_addr_res_enable(uint8_t enable)
{
    return LL_SetAddressResolutionEnable(enable);
}

int hci_api_le_set_privacy_mode(uint8_t type,
                                uint8_t id_addr[6],
                                uint8_t mode)
{
    return -ENOTSUP;
}

int hci_api_num_complete_packets(uint8_t num_handles,
                                 uint16   *handles,
                                 uint16   *counts)
{
    int i;

    for (i = 0; i < num_handles; i++) {
        uint16 handle, count;

        handle = handles[i];
        count = counts[i];
        extern void bt_hci_num_complete_packets(uint16 handle, uint16_t count);
        bt_hci_num_complete_packets(handle, count);
    }

    return 0;
}

int hci_api_read_buffer_size(uint16_t *acl_max_len,
                             uint8_t  *sco_max_len,
                             uint16_t *acl_max_num,
                             uint16_t *sco_max_num)
{
    return -ENOTSUP;
}

int hci_api_le_write_host_supp(uint8_t le, uint8_t simul)
{
    return -ENOTSUP;
}

int hci_api_white_list_size(uint8_t *size)
{
    return LL_ReadWlSize(size);
}

int hci_api_white_list_add(uint8_t peer_addr_type, uint8_t peer_addr[6])
{
    return LL_AddWhiteListDevice(peer_addr, peer_addr_type);
}

int hci_api_white_list_remove(uint8_t peer_addr_type, uint8_t peer_addr[6])
{
    return LL_RemoveWhiteListDevice(peer_addr, peer_addr_type);
}

int hci_api_white_list_clear()
{
    return LL_ClearWhiteList();
}

#ifdef CONFIG_BT_ECC

#include <tinycrypt/constants.h>
#include <tinycrypt/utils.h>
#include <tinycrypt/ecc.h>
#include <tinycrypt/ecc_dh.h>
#include <misc/byteorder.h>

/* based on Core Specification 4.2 Vol 3. Part H 2.3.5.6.1 */
static const u32_t debug_private_key[8] = {
    0xcd3c1abd, 0x5899b8a6, 0xeb40b799, 0x4aff607b, 0xd2103f50, 0x74c9b3e3,
    0xa3c55f38, 0x3f49f6d4
};

#if defined(CONFIG_BT_USE_DEBUG_KEYS)
static const u8_t debug_public_key[64] = {
    0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc, 0xdb, 0xfd, 0xf4, 0xac,
    0x11, 0x91, 0xf4, 0xef, 0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
    0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20, 0x8b, 0xd2, 0x89, 0x15,
    0xd0, 0x8e, 0x1c, 0x74, 0x24, 0x30, 0xed, 0x8f, 0xc2, 0x45, 0x63, 0x76,
    0x5c, 0x15, 0x52, 0x5a, 0xbf, 0x9a, 0x32, 0x63, 0x6d, 0xeb, 0x2a, 0x65,
    0x49, 0x9c, 0x80, 0xdc
};
#endif

enum {
    PENDING_PUB_KEY = 0x01,
    PENDING_DHKEY = 0x02,
};

static uint8_t ecc_flags = 0;

static struct {
    uint8_t private_key[32];

    union {
        uint8_t pk[64];
        uint8_t dhkey[32];
    };
} ecc;

static u8_t generate_keys(void)
{
    do {
        int rc;

        rc = uECC_make_key(ecc.pk, ecc.private_key, &curve_secp256r1);

        if (rc == TC_CRYPTO_FAIL) {
            return 0x1f;
        }

        /* make sure generated key isn't debug key */
    } while (memcmp(ecc.private_key, debug_private_key, 32) == 0);

    return 0;
}

static void emulate_le_p256_public_key_cmd(void)
{
    uint8_t status;
    uint8_t key[64];

    status = generate_keys();

    if (status) {
        memset(key, 0, sizeof(key));
    } else {
        sys_memcpy_swap(key, ecc.pk, 32);
        sys_memcpy_swap(&key[32], &ecc.pk[32], 32);
    }

    ecc_flags &= ~PENDING_PUB_KEY;
    extern int hci_api_le_event_pkey_complete(uint8_t status, uint8_t key[64]);
    hci_api_le_event_pkey_complete(status, key);
}

static void emulate_le_generate_dhkey(void)
{
    int ret;
    uint8_t status = 0;
    uint8_t dhkey[32];
    ret = uECC_valid_public_key(ecc.pk, &curve_secp256r1);

    if (ret < 0) {
        ret = TC_CRYPTO_FAIL;
    } else {
        ret = uECC_shared_secret(ecc.pk, ecc.private_key, ecc.dhkey,
                                 &curve_secp256r1);
    }

    if (ret == TC_CRYPTO_SUCCESS) {
        sys_memcpy_swap(dhkey, ecc.dhkey, sizeof(ecc.dhkey));
    } else {
        status = 0x1f;
    }

    ecc_flags &= ~PENDING_DHKEY;
    extern int hci_api_le_event_dhkey_complete(uint8_t status, uint8_t dhkey[32]);
    hci_api_le_event_dhkey_complete(status, dhkey);
}

void ecc_work_handler()
{
    if (ecc_flags & PENDING_PUB_KEY) {
        emulate_le_p256_public_key_cmd();
    } else if (ecc_flags & PENDING_DHKEY) {
        emulate_le_generate_dhkey();
    }
}

int hci_ecc_task_init()
{
    return 0;
}

int hci_api_le_gen_p256_pubkey()
{
    uint8_t status;

    if ((ecc_flags & PENDING_DHKEY) || (ecc_flags & PENDING_PUB_KEY)) {
        status = 0x0c;
    } else {
        ecc_flags  |= PENDING_PUB_KEY;
        status = 0;
    }

    return status;
}

int hci_api_le_gen_dhkey(uint8_t remote_pk[64])
{
    uint8_t status;

    if ((ecc_flags & PENDING_DHKEY) || (ecc_flags & PENDING_PUB_KEY)) {
        status = 0x0c;
    } else {
        sys_memcpy_swap(ecc.pk, remote_pk, 32);
        sys_memcpy_swap(&ecc.pk[32], &remote_pk[32], 32);
        ecc_flags  |= PENDING_DHKEY;
        status = 0;
    }

    return status;
}
#endif

int hci_api_init()
{
    int ret = 0;
#ifdef CONFIG_BT_ECC
    ret = hci_ecc_task_init();
#endif
    return ret;
}

int hci_api_le_rpa_timeout_set(uint16_t timeout)
{
    return LL_SetResolvablePrivateAddressTimeout(timeout);
}


