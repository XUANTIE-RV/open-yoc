/*******************************************************************************
  Filename:       hci_c_event.c
  Revised:
  Revision:

  Description:    This file send HCI events for the controller. It implements
                  all the LL event callback and HCI events send.


*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#if !(defined(CONFIG_BT_USE_HCI_API) && CONFIG_BT_USE_HCI_API)
#include "rom_sym_def.h"
#include "comdef.h"
#include "hci_phy.h"
#include "hci_tl.h"
#include "rf_phy_driver.h"
#include "hci_event.h"
extern uint8 *osal_msg_allocate(uint16 len);
extern uint8 osal_msg_send(uint8 destination_task, uint8 *msg_ptr);
//#include "EM_platform.h"
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */
#define LE_EVT_MASK_DEFAULT                0x007AFA5F

// LE Event Lengths
#define HCI_CMD_COMPLETE_EVENT_LEN                  3
#define HCI_CMD_VS_COMPLETE_EVENT_LEN               2
#define HCI_CMD_STATUS_EVENT_LEN                    4
#define HCI_NUM_COMPLETED_PACKET_EVENT_LEN          5
#define HCI_FLUSH_OCCURRED_EVENT_LEN                2
#define HCI_REMOTE_VERSION_INFO_EVENT_LEN           8
#define HCI_CONNECTION_COMPLETE_EVENT_LEN           19
#define HCI_ENH_CONN_COMPLETE_EVENT_LEN             31
#define HCI_DISCONNECTION_COMPLETE_LEN              4
#define HCI_LL_CONN_UPDATE_COMPLETE_LEN             10
#define HCI_ADV_REPORT_EVENT_LEN                    12
#define HCI_READ_REMOTE_FEATURE_COMPLETE_EVENT_LEN  12
#define HCI_LTK_REQUESTED_EVENT_LEN                 13
#define HCI_DATA_BUF_OVERFLOW_EVENT_LEN             1
#define HCI_ENCRYPTION_CHANGE_EVENT_LEN             4
#define HCI_KEY_REFRESH_COMPLETE_EVENT_LEN          3
#define HCI_BUFFER_OVERFLOW_EVENT_LEN               1
#define HCI_LL_DATA_LENGTH_CHANGE_EVENT_LEN         11
#define HCI_LL_PHY_UPDATE_COMPLETE_EVENT_LEN        6

#define HCI_EXT_ADV_REPORT_EVENT_LEN                26   //18
#define HCI_PRD_ADV_SYNC_ESTAB_EVENT_LEN            16
#define HCI_PRD_ADV_REPORT_EVENT_LEN                8
#define HCI_PRD_ADV_SYNC_LOST_EVENT_LEN             2
#define HCI_ADV_SET_TERM_EVENT_LEN                  6
#define HCI_SCAN_REQ_RECV_EVENT_LEN                 7
#define HCI_CHN_SEL_ALGO_EVENT_LEN                  4

#define HCI_SCAN_TIMEOUT_EVENT_LEN                  1

// LE Event mask - 1st octet
#define LE_EVT_MASK_CONN_COMPLETE                   0x00000001
#define LE_EVT_MASK_ADV_REPORT                      0x00000002
#define LE_EVT_MASK_CONN_UPDATE_COMPLETE            0x00000004
#define LE_EVT_MASK_READ_REMOTE_FEATURE             0x00000008

#define LE_EVT_MASK_LTK_REQUEST                     0x00000010
#define LE_EVT_MASK_REMOTE_CONN_PARAM_REQ           0x00000020
#define LE_EVT_MASK_DATA_LENGTH_CHANGE              0x00000040
#define LE_EVT_MASK_LOCAL_P256_PUB_KEY_CMP          0x00000080

// LE Event mask - 2nd octet
#define LE_EVT_MASK_GEN_DHKEY_CMP                   0x00000100
#define LE_EVT_MASK_ENH_CONN_CMP                    0x00000200
#define LE_EVT_MASK_DIRECT_ADV_RPT                  0x00000400
#define LE_EVT_MASK_PHY_CHANGE                      0x00000800

#define LE_EVT_MASK_EXT_ADV_RPT                     0x00001000
#define LE_EVT_MASK_PRD_ADV_SYNC_EST                0x00002000
#define LE_EVT_MASK_PRD_ADV_RPT                     0x00004000
#define LE_EVT_MASK_PRD_ADV_SYNC_LOST               0x00008000

// LE Event mask - 3rd octet
#define LE_EVT_MASK_SCAN_TO                         0x00010000
#define LE_EVT_MASK_ADV_SET_TERM                    0x00020000
#define LE_EVT_MASK_SCAN_REQ_RECV                   0x00040000
#define LE_EVT_MASK_CHN_SEL_ALGO                    0x00080000

#define LE_EVT_MASK_CONNECTIONLESS_IQ_RPT           0x00100000
#define LE_EVT_MASK_CONNECTION_IQ_RPT               0x00200000
#define LE_EVT_MASK_CTE_REQ_FAIL                    0x00400000
#define LE_EVT_MASK_PRD_ADV_SYNC_TRANSFER_RECV      0x00800000


// HCI Event mask - octet index
#define HCI_EVT_INDEX_DISCONN_COMPLETE              0
#define HCI_EVT_INDEX_ENCRYPTION_CHANGE             0
#define HCI_EVT_INDEX_READ_REMOTE_VER               1
#define HCI_EVT_INDEX_HW_ERROR                      1
#define HCI_EVT_INDEX_FLUSH_OCCURRED                2
#define HCI_EVT_INDEX_BUF_OVERFLOW                  3
#define HCI_EVT_INDEX_KEY_REFRESH_COMPLETE          5
#define HCI_EVT_INDEX_LE                            7

// HCI Event mask - octet mask
#define HCI_EVT_MASK_DISCONN_COMPLETE               0x10
#define HCI_EVT_MASK_ENCRYPTION_CHANGE              0x80
#define HCI_EVT_MASK_READ_REMOTE_VER                0x08
#define HCI_EVT_MASK_HW_ERROR                       0x80
#define HCI_EVT_MASK_FLUSH_OCCURRED                 0x01
#define HCI_EVT_MASK_BUF_OVERFLOW                   0x02
#define HCI_EVT_MASK_KEY_REFRESH_COMPLETE           0x80
#define HCI_EVT_MASK_LE                             0x20

// HCI Connection Complete Roles
#define HCI_EVT_MASTER_ROLE                         0x00
#define HCI_EVT_SLAVE_ROLE                          0x01

// LE Meta Event Codes
#define HCI_BLE_SCAN_TIMEOUT_EVENT                     0x11

/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    osal_event_hdr_t  hdr;
    uint8             BLEEventCode;
    uint8             adv_handle;
    uint8             scannerAddrType;
    uint8             scannerAddr[B_ADDR_LEN];
} hciEvt_ScanRequestReceived_t;

typedef struct {
    osal_event_hdr_t  hdr;
    uint8  BLEEventCode;
} hciEvt_ScanTimeout_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

// Note: There are 8 octects for BLE event mask, if we support BLE5.2 new feature,
//  we need another word to save the higher 4 bytes event mask info.
extern uint8 pHciEvtMask[];
extern uint8 hciGapTaskID;
/*******************************************************************************
 * GLOBAL VARIABLES
 */
//#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
void LL_ReadRemoteUsedFeaturesCompleteCback(hciStatus_t status,
        uint16      connHandle,
        uint8       *featureSet);
//#endif // CTRL_CONFIG=INIT_CFG

/*******************************************************************************
 * EXTERNS
 */
extern uint8 hciPTMenabled;
extern uint8 hciCtrlCmdToken;

extern uint8 hciTaskID;

/*
** Internal Functions
*/



// TO update comments
/*******************************************************************************
 * @fn          LL_ExtAdvReportCback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       dataLen     - Length of data in bytes.
 * @param       advData     - Pointer to data.
 * @param       rssi        - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ExtAdvReportCback1(uint16 advEvt,
                           uint8   advAddrType,
                           uint8  *advAddr,
                           uint8   primaryPHY,
                           uint8   secondaryPHY,
                           uint8   advertisingSID,
                           uint8   txPower,
                           int8    rssi,
                           uint16  periodicAdvertisingInterval,
                           uint8   directAddrType,
                           uint8   *directAddr,
                           uint8   dataLen,
                           uint8   *rptData)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEExtAdvPktReport_t *pkt;
        hciEvt_ExtAdvRptInfo_t *rptInfo;
        uint8 x;

    if (dataLen > B_MAX_ADV_LEN)      // guard the memory access, 2018-10-15
    {
        return;
    }

        pkt = (hciEvt_BLEExtAdvPktReport_t *)osal_msg_allocate(
                  sizeof(hciEvt_BLEExtAdvPktReport_t) + sizeof(hciEvt_ExtAdvRptInfo_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_EXT_ADV_REPORT_EVENT;
            pkt->numReports = 1;  // assume one report each event now
            pkt->rptInfo = rptInfo = (hciEvt_ExtAdvRptInfo_t *)(pkt + 1);

            for (x = 0; x < pkt->numReports; x++, rptInfo++) {
                /* Fill in the device info */
                rptInfo->eventType = advEvt;
                rptInfo->addrType = advAddrType;
                (void)osal_memcpy(rptInfo->addr, advAddr, B_ADDR_LEN);

                rptInfo->primaryPHY = primaryPHY;

                if (secondaryPHY == PKT_FMT_BLR125K) {
                    rptInfo->secondaryPHY = 0x03;    //  convert 4 -> 3
                } else {
                    rptInfo->secondaryPHY = secondaryPHY;
                }

                rptInfo->advertisingSID = advertisingSID;
                rptInfo->txPower = txPower;
                rptInfo->rssi   = rssi;
                rptInfo->periodicAdvertisingInterval = periodicAdvertisingInterval;
                rptInfo->directAddrType = directAddrType;

                if (advEvt & LE_ADV_PROP_DIRECT_BITMASK) {
                    (void)osal_memcpy(rptInfo->directAddr, directAddr, B_ADDR_LEN);
                }

                rptInfo->dataLen = dataLen;
                (void)osal_memcpy(rptInfo->rptData, rptData, dataLen);
                rptInfo->rssi = rssi;
            }

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint16 totalLength;
        uint8 dataLength;

        if (HCI_EXT_ADV_REPORT_EVENT_LEN + dataLen > 0xFFUL)
        {
            return;
        }

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_EXT_ADV_REPORT_EVENT_LEN + dataLen;
//    if (advEvt & LE_ADV_PROP_DIRECT_BITMASK)
//      dataLength += 6;

        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_EXT_ADV_REPORT_EVENT;                // event code
            msg->pData[4] = 1;                                       // number of reports; assume 1 for now
            msg->pData[5] = LO_UINT16(advEvt);                                  // advertisement event type
            msg->pData[6] = HI_UINT16(advEvt);
            msg->pData[7] = advAddrType;                             // address type
            (void)osal_memcpy(&msg->pData[8], advAddr, B_ADDR_LEN);  // address

            msg->pData[14] = primaryPHY;
            msg->pData[15] = secondaryPHY;
            msg->pData[16] = advertisingSID;
            msg->pData[17] = txPower;
            msg->pData[18] = rssi;
            msg->pData[19]  = LO_UINT16(periodicAdvertisingInterval);
            msg->pData[20]  = HI_UINT16(periodicAdvertisingInterval);
            msg->pData[21] = directAddrType;                             // address type

            if (advEvt & LE_ADV_PROP_DIRECT_BITMASK) {
                (void)osal_memcpy(&msg->pData[22], directAddr, B_ADDR_LEN);    // address
            } else {
                osal_memset(&msg->pData[22], 0, B_ADDR_LEN);                // set zero
            }

            msg->pData[28] = dataLen;                                // data length
            (void)osal_memcpy(&msg->pData[29], rptData, dataLen);    // data

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}

/*******************************************************************************
 * @fn          LL_AdvSetTerminatedCback
 *
 * @brief       This LL callback is used to .....
 *
 *
 *
 * input parameters
 *
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_AdvSetTerminatedCback1(uint8          status,
                              uint8   adv_handle,
                              uint16  connHandle,
                              uint8   Num_Completed_Extended_Advertising_Events)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_AdvSetTerminated_t *pkt;

        pkt = (hciEvt_AdvSetTerminated_t *)osal_msg_allocate(sizeof(hciEvt_AdvSetTerminated_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_LE_ADVERTISING_SET_TERMINATED;

            pkt->status      = status;
            pkt->adv_handle  = adv_handle;
            pkt->connHandle  = connHandle;
            pkt->Num_Completed_Extended_Advertising_Events = Num_Completed_Extended_Advertising_Events;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_ADV_SET_TERM_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_LE_ADVERTISING_SET_TERMINATED;     // event code
            msg->pData[4] = status;
            msg->pData[5] = adv_handle;
            msg->pData[6] = LO_UINT16(connHandle);
            msg->pData[7] = HI_UINT16(connHandle);
            msg->pData[8] = Num_Completed_Extended_Advertising_Events;

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}


/*******************************************************************************
 * @fn          LL_ChannelSelectionAlgorithmCback Callback
 *
 * @brief       This LL callback is used to .....
 *
 *
 *
 * input parameters
 *
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ChannelSelectionAlgorithmCback(uint16 connHandle,
                                       uint8  chnSel
                                      )
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_ChannelSelAlgo_t *pkt;

        pkt = (hciEvt_ChannelSelAlgo_t *)osal_msg_allocate(sizeof(hciEvt_ChannelSelAlgo_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT;

            pkt->connHandle  = connHandle;
            pkt->chn_sel     = chnSel;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_CHN_SEL_ALGO_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT;     // event code
            msg->pData[4] = LO_UINT16(connHandle);
            msg->pData[5] = HI_UINT16(connHandle);
            msg->pData[6] = chnSel;

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}


/*******************************************************************************
 * @fn          LL_PrdAdvSyncEstablishedCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       dataLen     - Length of data in bytes.
 * @param       advData     - Pointer to data.
 * @param       rssi        - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_PrdAdvSyncEstablishedCback1(uint8        status,
                                   uint16  syncHandle,
                                   uint8   advertisingSID,
                                   uint8   advertiserAddressType,
                                   uint8   *advertiserAddress,
                                   uint8   advertiserPHY,
                                   uint16  periodicAdvertisingInterval,
                                   uint8   advertiserClockAccuracy
                                  )
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEPrdAdvSyncEstabPkt_t *pkt;

        pkt = (hciEvt_BLEPrdAdvSyncEstabPkt_t *)osal_msg_allocate(sizeof(hciEvt_BLEPrdAdvSyncEstabPkt_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_PERIODIC_ADV_SYNC_ESTABLISHED_EVENT;
            pkt->status          = status;
            pkt->syncHandle      = syncHandle;
            pkt->advertisingSID  =  advertisingSID;
            pkt->advertiserAddressType = advertiserAddressType;
            (void)osal_memcpy(&pkt->advertiserAddress[0], advertiserAddress, B_ADDR_LEN);  // address
            pkt->advertiserPHY   = advertiserPHY;
            pkt->periodicAdvertisingInterval = periodicAdvertisingInterval;
            pkt->advertiserClockAccuracy = advertiserClockAccuracy;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_PRD_ADV_SYNC_ESTAB_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_PERIODIC_ADV_SYNC_ESTABLISHED_EVENT;     // event code
            msg->pData[4] = status;
            msg->pData[5] = LO_UINT16(syncHandle);
            msg->pData[6] = HI_UINT16(syncHandle);
            msg->pData[7] = advertisingSID;
            msg->pData[8] = advertiserAddressType;
            (void)osal_memcpy(&msg->pData[9], advertiserAddress, B_ADDR_LEN);  // address
            msg->pData[15] = advertiserPHY;

            msg->pData[16] = LO_UINT16(periodicAdvertisingInterval);
            msg->pData[17] = HI_UINT16(periodicAdvertisingInterval);

            msg->pData[18] = advertiserClockAccuracy;

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}


/*******************************************************************************
 * @fn          LL_PrdAdvReportCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       dataLen     - Length of data in bytes.
 * @param       advData     - Pointer to data.
 * @param       rssi        - The RSSI of received packet.
 *
 Subevent_Code,
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_PrdAdvReportCback1(uint16 syncHandle,
                          uint8 txPower,
                          uint8 rssi,
                          uint8 cteType,
                          uint8 dataStatus,
                          uint8 dataLength,
                          uint8 *data
                         )
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEPrdAdvPktReport_t *pkt;
        hciEvt_PrdAdvRptInfo_t *rptInfo;

//    if (dataLen > B_MAX_ADV_LEN)      // guard the memory access, 2018-10-15
//    {
//        return;
//    }

        pkt = (hciEvt_BLEPrdAdvPktReport_t *)osal_msg_allocate(
                  sizeof(hciEvt_BLEPrdAdvPktReport_t) + sizeof(hciEvt_PrdAdvRptInfo_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_PERIODIC_ADV_REPORT_EVENT;
            pkt->rptInfo = rptInfo = (hciEvt_PrdAdvRptInfo_t *)(pkt + 1);

            /* Fill in the device info */
            rptInfo->syncHandle = syncHandle;
            rptInfo->txPower = txPower;

            rptInfo->cteType = cteType;
            rptInfo->rssi   = rssi;
            rptInfo->dataStatus = dataStatus;
            rptInfo->dataLength = dataLength;

            (void)osal_memcpy(rptInfo->data, data, dataLength);
            rptInfo->rssi = rssi;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 data_length;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        data_length = HCI_PRD_ADV_REPORT_EVENT_LEN + dataLength;

        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + data_length;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = data_length;

            // populate event
            msg->pData[3] = HCI_BLE_PERIODIC_ADV_REPORT_EVENT;                // event code
            msg->pData[4] = LO_UINT16(syncHandle);
            msg->pData[5] = HI_UINT16(syncHandle);
            msg->pData[6] = txPower;
            msg->pData[7] = rssi;
            msg->pData[8] = cteType;
            msg->pData[9] = dataStatus;
            msg->pData[10] = dataLength;
            (void)osal_memcpy(&msg->pData[11], data, dataLength);    // data

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}


/*******************************************************************************
 * @fn          LL_PrdAdvSyncEstablishedCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       dataLen     - Length of data in bytes.
 * @param       advData     - Pointer to data.
 * @param       rssi        - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_PrdAdvSyncLostCback(uint16  syncHandle)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEPrdAdvSyncLostPkt_t *pkt;

        pkt = (hciEvt_BLEPrdAdvSyncLostPkt_t *)osal_msg_allocate(sizeof(hciEvt_BLEPrdAdvSyncLostPkt_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_PERIODIC_ADV_SYNC_LOST_EVENT;
            pkt->syncHandle      = syncHandle;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_PRD_ADV_SYNC_LOST_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_PERIODIC_ADV_SYNC_LOST_EVENT;     // event code
            msg->pData[4] = LO_UINT16(syncHandle);
            msg->pData[5] = HI_UINT16(syncHandle);

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}

//==================== below 3 functions add BLE event mask checking, if BQB test case required, below functions are required
//void LL_ConnectionlessIQReportCback(          uint16 syncHandle,
//                                              uint8  chan_idx,
//                                              int16  rssi,
//                                              uint8  rssi_antID,
//                                              uint8  cte_type,
//                                              uint8  slot_duration,
//                                              uint8  packet_status,
//                                              uint16 PE_Cnt,
//                                              uint8  sampCnt,
//                                              uint16  *ISample,
//                                              uint16  *QSample)
//{
//  // check if this is for the Host
//  if ( hciGapTaskID != 0 )
//  {
//      hciEvt_BLEConnectionlessIQ_Pkt_t *pkt;

//      pkt = (hciEvt_BLEConnectionlessIQ_Pkt_t *)osal_msg_allocate(sizeof(hciEvt_BLEConnectionlessIQ_Pkt_t ));

//      if ( pkt )
//      {
//          pkt->hdr.event = HCI_GAP_EVENT_EVENT;
//          pkt->hdr.status = HCI_LE_EVENT_CODE;
//          pkt->BLEEventCode = HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT;
//          pkt->ConnectionlessIQ.Handle    = syncHandle;
//          pkt->ConnectionlessIQ.chan_idx  = chan_idx;
//          pkt->ConnectionlessIQ.rssi      = rssi;
//          pkt->ConnectionlessIQ.rssi_antID    = rssi_antID;
//          pkt->ConnectionlessIQ.cte_type  = cte_type;
//          pkt->ConnectionlessIQ.slot_duration = slot_duration;
//          pkt->ConnectionlessIQ.packet_status = packet_status;
//          pkt->ConnectionlessIQ.EventCnt      = PE_Cnt;
//          pkt->ConnectionlessIQ.sampCnt   = sampCnt;
//          (void)osal_memcpy(&pkt->ConnectionlessIQ.ISample[0],ISample,sampCnt);
//          (void)osal_memcpy(&pkt->ConnectionlessIQ.QSample[0],QSample,sampCnt);

//          (void)osal_msg_send( hciGapTaskID, (uint8 *)pkt );
//      }
//  }
//  else
//  {
//      hciPacket_t *msg;
//      uint8 totalLength;
//      uint8 dataLength;

//        // check if LE Meta-Events are enabled and this event is enabled
//        if ( ((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
//           (((bleEvtMask32 & LE_EVT_MASK_CONNECTIONLESS_IQ_RPT) == 0 )) )
//        {
//            // the event mask is not set for this event
//            return;
//        }
//      // data length
//      dataLength = sizeof(hciEvt_BLEConnectionlessIQ_Pkt_t);
//      // OSAL message header + HCI event header + data
//      totalLength = sizeof (hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

//      msg = (hciPacket_t *)osal_msg_allocate(totalLength);

//      if (msg)
//      {
//          // message type, length
//          msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
//          msg->hdr.status = 0xFF;

//          // create message
//          msg->pData  = (uint8*)(msg+1);
//          msg->pData[0] = HCI_EVENT_PACKET;
//          msg->pData[1] = HCI_LE_EVENT_CODE;
//          msg->pData[2] = dataLength;

//          // populate event
//          msg->pData[3]   = HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT;    // event code
//          msg->pData[4]   = LO_UINT16(syncHandle);
//          msg->pData[5]   = HI_UINT16(syncHandle);
//          msg->pData[6]   = chan_idx;
//          msg->pData[7]   = rssi;
//          msg->pData[8]   = rssi_antID;
//          msg->pData[9]   = cte_type;
//          msg->pData[10]  = slot_duration;
//          msg->pData[11]  = packet_status;
//          msg->pData[12]  = LO_UINT16(PE_Cnt);
//          msg->pData[13]  = HI_UINT16(PE_Cnt);
//          msg->pData[14]  = sampCnt;
//          (void)osal_memcpy(&msg->pData[15],ISample,sampCnt);
//          (void)osal_memcpy(&msg->pData[15 + sampCnt],QSample,sampCnt);

//          // send the message
//          (void)osal_msg_send( hciTaskID, (uint8 *)msg );
//      }
//  }

//}

//void LL_ConnectionIQReportCback(          uint16 connHandle,
//                                          uint8  rx_PHY,
//                                          uint8  data_chan_idx,
//                                          int16  rssi,
//                                          uint8  rssi_antID,
//                                          uint8  cte_type,
//                                          uint8  slot_duration,
//                                          uint8  packet_status,
//                                          uint16 connEventCounter,
//                                          uint8  sampCnt,
//                                          uint16  *ISample,
//                                          uint16  *QSample)
//{
//  // check if this is for the Host
//  if ( hciGapTaskID != 0 )
//  {
//      hciEvt_BLEConnectionIQ_Pkt_t *pkt;

//      pkt = (hciEvt_BLEConnectionIQ_Pkt_t *)osal_msg_allocate(sizeof(hciEvt_BLEConnectionIQ_Pkt_t ));

//      if ( pkt )
//      {
//          pkt->hdr.event = HCI_GAP_EVENT_EVENT;
//          pkt->hdr.status = HCI_LE_EVENT_CODE;
//          pkt->BLEEventCode = HCI_LE_CONNECTION_IQ_REPORT_EVENT;
//          pkt->RX_PHY = rx_PHY;
//          pkt->ConnectionIQ.Handle    = connHandle;
//          pkt->ConnectionIQ.chan_idx  = data_chan_idx;
//          pkt->ConnectionIQ.rssi      = rssi;
//          pkt->ConnectionIQ.rssi_antID    = rssi_antID;
//          pkt->ConnectionIQ.cte_type  = cte_type;
//          pkt->ConnectionIQ.slot_duration = slot_duration;
//          pkt->ConnectionIQ.packet_status = packet_status;
//          pkt->ConnectionIQ.EventCnt      = connEventCounter;
//          pkt->ConnectionIQ.sampCnt   = sampCnt;
//          (void)osal_memcpy(&pkt->ConnectionIQ.ISample[0],ISample,sampCnt);
//          (void)osal_memcpy(&pkt->ConnectionIQ.QSample[0],QSample,sampCnt);

//          (void)osal_msg_send( hciGapTaskID, (uint8 *)pkt );
//      }
//  }
//  else
//  {
//      hciPacket_t *msg;
//      uint8 totalLength;
//      uint8 dataLength;

//        // check if LE Meta-Events are enabled and this event is enabled
//        if ( ((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
//           (((bleEvtMask32 & LE_EVT_MASK_CONNECTION_IQ_RPT) == 0 )) )
//        {
//            // the event mask is not set for this event
//            return;
//        }
//
//      // data length
//      dataLength = sizeof(hciEvt_BLEConnectionIQ_Pkt_t);
//      // OSAL message header + HCI event header + data
//      totalLength = sizeof (hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

//      msg = (hciPacket_t *)osal_msg_allocate(totalLength);

//      if (msg)
//      {
//          // message type, length
//          msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
//          msg->hdr.status = 0xFF;

//          // create message
//          msg->pData  = (uint8*)(msg+1);
//          msg->pData[0] = HCI_EVENT_PACKET;
//          msg->pData[1] = HCI_LE_EVENT_CODE;
//          msg->pData[2] = dataLength;

//          // populate event
//          msg->pData[3]   = HCI_LE_CONNECTION_IQ_REPORT_EVENT;    // event code
//          msg->pData[4]   = LO_UINT16(connHandle);
//          msg->pData[5]   = HI_UINT16(connHandle);
//          msg->pData[6]   = rx_PHY;
//          msg->pData[7]   = data_chan_idx;
//          msg->pData[8]   = rssi;
//          msg->pData[9]   = rssi_antID;
//          msg->pData[10]  = cte_type;
//          msg->pData[11]  = slot_duration;
//          msg->pData[12]  = packet_status;
//          msg->pData[13]  = LO_UINT16(connEventCounter);
//          msg->pData[14]  = HI_UINT16(connEventCounter);
//          msg->pData[15]  = sampCnt;
//          (void)osal_memcpy(&msg->pData[16],ISample,sampCnt);
//          (void)osal_memcpy(&msg->pData[16 + sampCnt],QSample,sampCnt);

//          // send the message
//          (void)osal_msg_send( hciTaskID, (uint8 *)msg );
//      }
//  }

//}

//void LL_CTE_Report_FailedCback( uint8 status,uint16 connHandle)
//{
//  // check if this is for the Host
//  if ( hciGapTaskID != 0 )
//  {
//      hciEvt_BLE_CTEReport_Pkt_t *pkt;

//      pkt = (hciEvt_BLE_CTEReport_Pkt_t *)osal_msg_allocate(sizeof(hciEvt_BLE_CTEReport_Pkt_t ));

//      if ( pkt )
//      {
//          pkt->hdr.event = HCI_GAP_EVENT_EVENT;
//          pkt->hdr.status = HCI_LE_EVENT_CODE;
//          pkt->BLEEventCode = HCI_LE_CTE_REQUEST_FAILED_REPORT;
//          pkt->status = status;
//          pkt->connHandle = connHandle;

//          (void)osal_msg_send( hciGapTaskID, (uint8 *)pkt );
//      }
//  }
//  else
//  {
//      hciPacket_t *msg;
//      uint8 totalLength;
//      uint8 dataLength;

//        // check if LE Meta-Events are enabled and this event is enabled
//        if ( ((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
//           (((bleEvtMask32 & LE_EVT_MASK_CTE_REQ_FAIL) == 0 )) )
//        {
//            // the event mask is not set for this event
//            return;
//        }
//
//      // data length
//      dataLength = sizeof(hciEvt_BLE_CTEReport_Pkt_t);
//      // OSAL message header + HCI event header + data
//      totalLength = sizeof (hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

//      msg = (hciPacket_t *)osal_msg_allocate(totalLength);

//      if (msg)
//      {
//          // message type, length
//          msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
//          msg->hdr.status = 0xFF;

//          // create message
//          msg->pData  = (uint8*)(msg+1);
//          msg->pData[0] = HCI_EVENT_PACKET;
//          msg->pData[1] = HCI_LE_EVENT_CODE;
//          msg->pData[2] = dataLength;

//          // populate event
//          msg->pData[3]   = HCI_LE_CTE_REQUEST_FAILED_REPORT; // event code
//          msg->pData[4]   = status;
//          msg->pData[4]   = LO_UINT16(connHandle);
//          msg->pData[5]   = HI_UINT16(connHandle);

//          // send the message
//          (void)osal_msg_send( hciTaskID, (uint8 *)msg );
//      }
//  }

//}
// =================================================

/*******************************************************************************
 * @fn          LL_EnhConnectionCompleteCback Callback
 *
 * @brief       This LL callback is used to generate a Connection Complete meta
 *              event when a connection is established by either an Advertiser
 *              or an Initiator.
 *
 * input parameters
 *
 * @param       reasonCode    - Status of connection complete.
 * @param       connHandle    - Connection handle.
 * @param       role          - Connection formed as Master or Slave.
 * @param       peerAddrType  - Peer address as Public or Random.
 * @param       peerAddr      - Pointer to peer device address.
 * @param       localRpaAddr  - Pointer to local RPA
 * @param       peerRpaAddr   - Pointer to peer RPA
 * @param       connInterval  - Connection interval.
 * @param       slaveLatency  - Slave latency.
 * @param       connTimeout   - Connection timeout.
 * @param       clockAccuracy - Sleep clock accuracy (from Master only).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_EnhConnectionCompleteCback(uint8  reasonCode,
                                   uint16 connHandle,
                                   uint8  role,
                                   uint8  peerAddrType,
                                   uint8  *peerAddr,
                                   uint8  *localRpaAddr,
                                   uint8  *peerRpaAddr,
                                   uint16 connInterval,
                                   uint16 slaveLatency,
                                   uint16 connTimeout,
                                   uint8  clockAccuracy)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEEnhConnComplete_t *pkt;

        pkt = (hciEvt_BLEEnhConnComplete_t *)osal_msg_allocate(sizeof(hciEvt_BLEEnhConnComplete_t));

        if (pkt) {
            pkt->hdr.event    = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status   = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT;

            if (reasonCode == LL_STATUS_SUCCESS) {
                pkt->status = HCI_SUCCESS;
                (void)osal_memcpy(pkt->peerAddr, peerAddr, B_ADDR_LEN);

                (void)osal_memcpy(pkt->localRpaAddr, localRpaAddr, B_ADDR_LEN);
                (void)osal_memcpy(pkt->peerRpaAddr, peerRpaAddr, B_ADDR_LEN);
            } else {
                pkt->status = bleGAPConnNotAcceptable;
                (void)osal_memset(pkt->peerAddr, 0, B_ADDR_LEN);
            }

            pkt->connectionHandle = connHandle;
            pkt->role             = role;
            pkt->peerAddrType     = peerAddrType;
            pkt->connInterval     = connInterval;
            pkt->connLatency      = slaveLatency;
            pkt->connTimeout      = connTimeout;
            pkt->clockAccuracy    = clockAccuracy;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 dataLength;
        uint8 totalLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0)) {
            // the event mask is not enabled for this event
            return;
        }

        // data length
        dataLength = HCI_ENH_CONN_COMPLETE_EVENT_LEN;

        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_ENHANCED_CONNECTION_COMPLETE_EVENT;          // event code
            msg->pData[4] = reasonCode;                                 // reason code
            msg->pData[5] = LO_UINT16(connHandle);                      // connection handle (LSB)
            msg->pData[6] = HI_UINT16(connHandle);                      // connection handle (MSB)
            msg->pData[7] = role;                                       // role (master/slave)
            msg->pData[8] = peerAddrType;                               // address type

            if (reasonCode == LL_STATUS_SUCCESS) {
//        pkt->status = HCI_SUCCESS;
                (void)osal_memcpy(&msg->pData[9], peerAddr, B_ADDR_LEN);

                (void)osal_memcpy(&msg->pData[15], localRpaAddr, B_ADDR_LEN);
                (void)osal_memcpy(&msg->pData[21], peerRpaAddr, B_ADDR_LEN);
            } else {
//        pkt->status = bleGAPConnNotAcceptable;
                (void)osal_memset(&msg->pData[9], 0, B_ADDR_LEN);
            }

            // copy address
            (void)osal_memcpy(&msg->pData[9], peerAddr, B_ADDR_LEN);

            msg->pData[27] = LO_UINT16(connInterval);                   // connection interval (LSB)
            msg->pData[28] = HI_UINT16(connInterval);                   // connection interval (MSB)
            msg->pData[29] = LO_UINT16(slaveLatency);                   // slave latency (LSB)
            msg->pData[30] = HI_UINT16(slaveLatency);                   // slave latency (LSB)
            msg->pData[31] = LO_UINT16(connTimeout);                    // connectin timeout (LSB)
            msg->pData[32] = HI_UINT16(connTimeout);                    // connection timeout (MSB)
            msg->pData[33] = clockAccuracy;                             // clock accuracy

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}


/*******************************************************************************
 * @fn          LL_DirectAdvReportCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param       advEvt      - Advertise event type, or Scan Response event type.
 * @param       advAddrType - Public or Random address type.
 * @param       advAddr     - Pointer to device address.
 * @param       directAddrType
 * @param       directAddr
 * @param       rssi        - The RSSI of received packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_DirectAdvReportCback(uint8 advEvt,
                             uint8 advAddrType,
                             uint8 *advAddr,
                             uint8 directAddrType,
                             uint8 *directAddr,
                             int8  rssi)
{
#if 0

    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_BLEAdvPktReport_t *pkt;
        hciEvt_DevInfo_t *devInfo;
        uint8 x;

        if (dataLen > B_MAX_ADV_LEN) {    // guard the memory access, 2018-10-15
            return;
        }

        pkt = (hciEvt_BLEAdvPktReport_t *)osal_msg_allocate(
                  sizeof(hciEvt_BLEAdvPktReport_t) + sizeof(hciEvt_DevInfo_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_ADV_REPORT_EVENT;
            pkt->numDevices = 1;  // assume one device for now
            pkt->devInfo = devInfo = (hciEvt_DevInfo_t *)(pkt + 1);

            for (x = 0; x < pkt->numDevices; x++, devInfo++) {
                /* Fill in the device info */
                devInfo->eventType = advEvt;
                devInfo->addrType = advAddrType;
                (void)osal_memcpy(devInfo->addr, advAddr, B_ADDR_LEN);
                devInfo->dataLen = dataLen;
                (void)osal_memcpy(devInfo->rspData, advData, dataLen);
                devInfo->rssi = rssi;
            }

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
            (((bleEvtMask32 & LE_EVT_MASK_DIRECT_ADV_RPT) == 0))) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_ADV_REPORT_EVENT_LEN + dataLen;

        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_ADV_REPORT_EVENT;                // event code
            msg->pData[4] = 1;                                       // number of devices; assume 1 for now
            msg->pData[5] = advEvt;                                  // advertisement event type
            msg->pData[6] = advAddrType;                             // address type
            (void)osal_memcpy(&msg->pData[7], advAddr, B_ADDR_LEN);  // address
            msg->pData[13] = dataLen;                                // data length
            (void)osal_memcpy(&msg->pData[14], advData, dataLen);    // data
            msg->pData[14 + dataLen] = rssi;                         // RSSI

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }

#endif
}

/*******************************************************************************
 * @fn          LL_ScanRequestRecvCback
 *
 * @brief       This LL callback is used to .....
 *
 *
 *
 * input parameters
 *
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ScanRequestRecvCback(uint8    adv_handle,
                             uint8  scannerAddrType,
                             uint8 *scannerAddr)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_ScanRequestReceived_t *pkt;

        pkt = (hciEvt_ScanRequestReceived_t *)osal_msg_allocate(sizeof(hciEvt_ScanRequestReceived_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_LE_ADVERTISING_SET_TERMINATED;

            pkt->adv_handle       = adv_handle;
            pkt->scannerAddrType  = scannerAddrType;
            (void)osal_memcpy(pkt->scannerAddr, scannerAddr, B_ADDR_LEN);

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
            (((bleEvtMask & LE_EVT_MASK_SCAN_REQ_RECV) == 0))) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_SCAN_REQ_RECV_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_LE_SCAN_REQUEST_RECEIVED;     // event code
            msg->pData[4] = adv_handle;
            msg->pData[5] = scannerAddrType;
            (void)osal_memcpy(&msg->pData[6], scannerAddr, B_ADDR_LEN);

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}

// 2020-11-26
/*******************************************************************************
 * @fn          LL_ScanTimeoutCback Callback
 *
 * @brief       This LL callback is used to generate a Advertisment Report meta
 *              event when an Advertisment or Scan Response is received by a
 *              Scanner.
 *
 * input parameters
 *
 * @param
 *              None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LL_ScanTimeoutCback(void)
{
    // check if this is for the Host
    if (hciGapTaskID != 0) {
        hciEvt_ScanTimeout_t *pkt;

        pkt = (hciEvt_ScanTimeout_t *)osal_msg_allocate(sizeof(hciEvt_ScanTimeout_t));

        if (pkt) {
            pkt->hdr.event = HCI_GAP_EVENT_EVENT;
            pkt->hdr.status = HCI_LE_EVENT_CODE;
            pkt->BLEEventCode = HCI_BLE_SCAN_TIMEOUT_EVENT;

            (void)osal_msg_send(hciGapTaskID, (uint8 *)pkt);
        }
    } else {
        hciPacket_t *msg;
        uint8 totalLength;
        uint8 dataLength;

        // check if LE Meta-Events are enabled and this event is enabled
        if (((pHciEvtMask[HCI_EVT_INDEX_LE] & HCI_EVT_MASK_LE) == 0) ||
            (((bleEvtMask & LE_EVT_MASK_SCAN_TO) == 0))) {
            // the event mask is not set for this event
            return;
        }

        // data length
        dataLength = HCI_SCAN_TIMEOUT_EVENT_LEN;
        // OSAL message header + HCI event header + data
        totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLength;

        msg = (hciPacket_t *)osal_msg_allocate(totalLength);

        if (msg) {
            // message type, length
            msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
            msg->hdr.status = 0xFF;

            // create message
            msg->pData    = (uint8 *)(msg + 1);
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = dataLength;

            // populate event
            msg->pData[3] = HCI_BLE_SCAN_TIMEOUT_EVENT;     // Subevent_Code

            // send the message
            (void)osal_msg_send(hciTaskID, (uint8 *)msg);
        }
    }
}

uint8 isEnhConnCmpEvtSupport(void)
{
    if (bleEvtMask & LE_EVT_MASK_ENH_CONN_CMP)
		return TRUE;

	return FALSE;
}

/*******************************************************************************
 */

#endif