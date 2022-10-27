/*******************************************************************************
  Filename:       hci_tl.c
  Revised:
  Revision:

  Description:    This file includes implementation for HCI task, event handler,
                  HCI Command, Data, and Event procoessing and sending, for the
                  BLE Transport Layer.


*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#if !(defined(CONFIG_BT_USE_HCI_API) && CONFIG_BT_USE_HCI_API)
#include "rom_sym_def.h"
#include "comdef.h"
#include "hci_tl.h"
//#include "osal_bufmgr.h"
#include "jump_function.h"
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <aos/hal/uart.h>
#include <aos/kernel.h>
#include <drv/usart.h>
#include <aos/ringbuffer.h>
#include <ll_common.h>
#define BT_DBG_ENABLED 0
#include "common/log.h"
//#include "h5.h"
//extern uint32 bleEvtMask;
extern uint8 pHciEvtMask[];
extern uint8 hciPTMenabled;

#ifdef  HCI_TL_NONE
#undef  HCI_TL_NONE
#endif

#ifdef  HCI_BLE_50
#undef  HCI_BLE_50
#endif

#define HCI_BLE_50

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#define MAX_UART_BUF_SIZE    256

/* States for Command and Data packet parser */
#define HCI_PARSER_STATE_PKT_TYPE          0
//
#define HCI_CMD_PARSER_STATE_OPCODE        1
#define HCI_CMD_PARSER_STATE_LENGTH        2
#define HCI_CMD_PARSER_STATE_DATA          3
//
#define HCI_DATA_PARSER_STATE_HANDLE       4
#define HCI_DATA_PARSER_STATE_LENGTH       5
#define HCI_DATA_PARSER_STATE_DATA         6

// HCI Command Subgroup
#define HCI_OPCODE_CSG_LINK_LAYER          0
#define HCI_OPCODE_CSG_CSG_L2CAP           1
#define HCI_OPCODE_CSG_CSG_ATT             2
#define HCI_OPCODE_CSG_CSG_GATT            3
#define HCI_OPCODE_CSG_CSG_GAP             4
#define HCI_OPCODE_CSG_CSG_SM              5
#define HCI_OPCODE_CSG_CSG_Reserved        6
#define HCI_OPCODE_CSG_CSG_USER_PROFILE    7

#define H5_VDRSPEC_PKT                 0x0E
// Vendor Specific OGF
#define VENDOR_SPECIFIC_OGF                0x3F

#define ADD_IDX(a,b)   (a==b)? a=0:a++;
#define MAX_UART_BUF_ID    (MAX_UART_BUF_SIZE-1)


#ifndef BASE_TIME_UINTS
#define BASE_TIME_UNITS   (0x3fffff)
#endif

/*******************************************************************************
 * TYPEDEFS
 */

typedef hciStatus_t (*hciFunc_t)(uint8 *pBuf);

typedef struct {
    uint16    opCode;
    hciFunc_t hciFunc;
} hciCmdFunc_t;

typedef const hciCmdFunc_t cmdPktTable_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

uint8 swUpgradeMode = FALSE;

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern uint8 hciTaskID;
extern uint8 hciTestTaskID;
//
extern uint8 hciGapTaskID;
extern uint8 hciL2capTaskID;
extern uint8 hciSmpTaskID;
extern uint8 hciExtTaskID;

extern uint8 hciCtrlCmdToken;

extern void HCI_ReverseBytes( uint8 *buf,
                              uint8 len );


/*
** Controller Prototypes
*/
void hciProcessHostToCtrlCmd(hciPacket_t *pBuf);
void hciProcessHostToCtrlData(hciDataPacket_t *pMsg);
void hciProcessCtrlToHost(hciPacket_t *pBuf);

//uint16 HCI_H5_Send(hci_data_type_t type, uint8 *buf, uint16 len);
//void   HCI_H5_RecvCallback(hci_data_type_t packet_type, uint8_t *data, uint32_t length);

extern hciStatus_t HCI_LE_ConnUpdateCmd1(uint16 connHandle,
        uint16 connIntervalMin,
        uint16 connIntervalMax,
        uint16 connLatency,
        uint16 connTimeout,
        uint16 minLen,
        uint16 maxLen);

hciStatus_t HCI_LE_ReadLocalResolvableAddressCmd1(uint8  peerIdAddrType,
        uint8 *peerIdAddr);

// Linker Control Commands
hciStatus_t hciDisconnect(uint8 *pBuf);
hciStatus_t hciReadRemoteVersionInfo(uint8 *pBuf);

// Controller and Baseband Commands
hciStatus_t hciSetEventMask(uint8 *pBuf);
hciStatus_t hciReset(uint8 *pBuf);

hciStatus_t hciReadTransmitPowerLevel(uint8 *pBuf);
hciStatus_t hciSetControllerToHostFlowCtrl(uint8 *pBuf);
hciStatus_t hciHostBufferSize(uint8 *pBuf);
hciStatus_t hciHostNumCompletedPkt(uint8 *pBuf);


// Information Parameters
hciStatus_t hciReadLocalVersionInfo(uint8 *pBuf);
hciStatus_t hciReadLocalSupportedCommands(uint8 *pBuf);
hciStatus_t hciReadLocalSupportedFeatures(uint8 *pBuf);
hciStatus_t hciReadBDADDR(uint8 *pBuf);
hciStatus_t hciReadRssi(uint8 *pBuf);

// LE Commands
hciStatus_t hciLESetEventMask(uint8 *pBuf);
hciStatus_t hciLEReadBufSize(uint8 *pBuf);
hciStatus_t hciLEReadLocalSupportedFeatures(uint8 *pBuf);
hciStatus_t hciLESetRandomAddr(uint8 *pBuf);

hciStatus_t hciLESetAdvParam(uint8 *pBuf);
hciStatus_t hciLESetAdvData(uint8 *pBuf);
hciStatus_t hciLESetScanRspData(uint8 *pBuf);
hciStatus_t hciLESetAdvEnab(uint8 *pBuf);
hciStatus_t hciLEReadAdvChanTxPower(uint8 *pBuf);


hciStatus_t hciLESetScanParam(uint8 *pBuf);
hciStatus_t hciLESetScanEnable(uint8 *pBuf);


hciStatus_t hciLECreateConn(uint8 *pBuf);
hciStatus_t hciLECreateConnCancel(uint8 *pBuf);

hciStatus_t hciLEReadWhiteListSize(uint8 *pBuf);
hciStatus_t hciLEClearWhiteList(uint8 *pBuf);
hciStatus_t hciLEAddWhiteList(uint8 *pBuf);
hciStatus_t hciLERemoveWhiteList(uint8 *pBuf);

hciStatus_t hciLEConnUpdate(uint8 *pBuf);
hciStatus_t hciLESetHostChanClass(uint8 *pBuf);
hciStatus_t hciLEReadChanMap(uint8 *pBuf);
hciStatus_t hciLEReadRemoteUsedFeatures(uint8 *pBuf);

hciStatus_t hciLEEncrypt(uint8 *pBuf);
hciStatus_t hciLERand(uint8 *pBuf);

hciStatus_t hciLEStartEncrypt(uint8 *pBuf);


hciStatus_t hciLELtkReqReply(uint8 *pBuf);
hciStatus_t hciLELtkReqNegReply(uint8 *pBuf);

hciStatus_t hciLEReadSupportedStates(uint8 *pBuf);
hciStatus_t hciLEReceiverTest(uint8 *pBuf);
hciStatus_t hciLETransmitterTest(uint8 *pBuf);
hciStatus_t hciLETestEnd(uint8 *pBuf);

hciStatus_t hciLESetDataLength(uint8 *pBuf);

hciStatus_t hciLEReadSuggestedDefaultDataLength(uint8 *pBuf);
hciStatus_t hciLEWriteSuggestedDefaultDataLength(uint8 *pBuf);
hciStatus_t hciLESetDefaultPhyMode(uint8 *pBuf);
hciStatus_t hciLESetPhyMode(uint8 *pBuf);
hciStatus_t hciLEReadPhyMode(uint8 *pBuf);

// 2020-07 add
hciStatus_t hciLEAddDeviceToRL(uint8 *pBuf);
hciStatus_t hciLERemoveDeviceFromRL(uint8 *pBuf);
hciStatus_t hciLEClearRL(uint8 *pBuf);
hciStatus_t hciLEReadRLSize(uint8 *pBuf);
hciStatus_t hciLEReadPeerRA(uint8 *pBuf);
hciStatus_t hciLEReadLocalRA(uint8 *pBuf);
hciStatus_t hciLESetAddrResolutionEnable(uint8 *pBuf);
hciStatus_t hciLESetRpaTo(uint8 *pBuf);

// extended adv
hciStatus_t hciLESetAdvSetRandomAddress(uint8 *pBuf);
hciStatus_t hciLESetExtAdvParam(uint8 *pBuf);
hciStatus_t hciLESetExtAdvData(uint8 *pBuf);
hciStatus_t hciLESetExtScanRspData(uint8 *pBuf);
hciStatus_t hciLESetExtAdvEnable(uint8 *pBuf);
hciStatus_t hciLEReadMaximumAdvDataLength(uint8 *pBuf);
hciStatus_t hciLEReadNumberOfSupportAdvSet(uint8 *pBuf);
hciStatus_t hciLERemoveAdvSet(uint8 *pBuf);
hciStatus_t hciLEClearAdvSets(uint8 *pBuf);

hciStatus_t hciLESetPeriodicAdvParameter(uint8 *pBuf);
hciStatus_t hciLESetPeriodicAdvData(uint8 *pBuf);
hciStatus_t hciLESetPeriodicAdvEnable(uint8 *pBuf);
hciStatus_t hciLESetExtendedScanParameters(uint8 *pBuf);
hciStatus_t hciLESetExtendedScanEnableCmd(uint8 *pBuf);
hciStatus_t hciLEExtendedCreateConnection(uint8 *pBuf);
hciStatus_t hciLEPeriodicAdvertisingCreateSync(uint8 *pBuf);
hciStatus_t hciLEPeriodicAdvertisingCreateSyncCancel(uint8 *pBuf);
hciStatus_t hciLEPeriodicAdvertisingTerminateSync(uint8 *pBuf);
hciStatus_t hciLEAddDevToPeriodicAdvList(uint8 *pBuf);
hciStatus_t hciLERemovePeriodicAdvList(uint8 *pBuf);
hciStatus_t hciLEClearPeriodicAdvList(uint8 *pBuf);
hciStatus_t hciLEReadPeriodicAdvListSize(uint8 *pBuf);
hciStatus_t hciLEReadTransmitPower(uint8 *pBuf);
hciStatus_t hciLEClearPeriodicAdvList(uint8 *pBuf);
hciStatus_t hciLEReadPeriodicAdvListSize(uint8 *pBuf);

hciStatus_t hciLESetPrivacyMode(uint8 *pBuf);



// =====
hciStatus_t hciLEReadMaxDataLength(uint8 *pBuf);

hciStatus_t hciLESetPrdAdvRecvEnableCmd(uint8 *pBuf);

// Vendor Specific Commands
hciStatus_t hciExtSetRxGain(uint8 *pBuf);
hciStatus_t hciExtSetTxPower(uint8 *pBuf);
hciStatus_t hciExtExtendRfRange(uint8 *pBuf);
hciStatus_t hciExtHaltDuringRf(uint8 *pBuf);

hciStatus_t hciExtOnePktPerEvt(uint8 *pBuf);

hciStatus_t hciExtClkDivOnHalt(uint8 *pBuf);
hciStatus_t hciExtDeclareNvUsage(uint8 *pBuf);

hciStatus_t hciExtDelayPostProc(uint8 *pBuf);

hciStatus_t hciExtDecrypt(uint8 *pBuf);
hciStatus_t hciExtSetLocalSupportedFeatures(uint8 *pBuf);

hciStatus_t hciExtSetFastTxResponseTime(uint8 *pBuf);
hciStatus_t hciExtSetSlaveLatencyOverride(uint8 *pBuf);

hciStatus_t hciExtModemTestTx(uint8 *pBuf);
hciStatus_t hciExtModemHopTestTx(uint8 *pBuf);
hciStatus_t hciExtModemtestRx(uint8 *pBuf);
hciStatus_t hciExtEndModemTest(uint8 *pBuf);
hciStatus_t hciExtSetBDADDR(uint8 *pBuf);

hciStatus_t hciExtSetSCA(uint8 *pBuf);

hciStatus_t hciExtEnablePTM(uint8 *pBuf);
hciStatus_t hciExtSetFreqTune(uint8 *pBuf);
hciStatus_t hciExtSaveFreqTune(uint8 *pBuf);
hciStatus_t hciExtSetMaxDtmTxPower(uint8 *pBuf);
//hciStatus_t hciExtMapPmIoPort                      ( uint8 *pBuf );
hciStatus_t hciExtBuildRevision(uint8 *pBuf);
hciStatus_t hciExtDelaySleep(uint8 *pBuf);
hciStatus_t hciExtResetSystem(uint8 *pBuf);

hciStatus_t hciExtDisconnectImmed(uint8 *pBuf);
hciStatus_t hciExtPER(uint8 *pBuf);
hciStatus_t hciExtOverlappedProcessing(uint8 *pBuf);
hciStatus_t hciExtNumComplPktsLimit(uint8 *pBuf);

// 2021-3-8, enter sw upgrade mode. It will exit upgrade mode when hci reset
hciStatus_t hciExtSwUpgrade(uint8 *pBuf);

hciStatus_t hciLEReadRfPathCompensation(uint8 *pBuf);

hciStatus_t hciLEWriteRfPathCompensation(uint8 *pBuf);

// 2020-10-27 CTE
hciStatus_t hciLESet_ConnectionCTE_ReceiveParam(uint8 *pBuf);
hciStatus_t hciLE_ConnectionCTERequestEnable(uint8 *pBuf);

int bt_mesh_adv_enable();
int bt_mesh_adv_disable();
int bt_mesh_scan_disable(void);
int bt_mesh_scan_enable(uint8 filterReports);

extern int rtk_unrel_is_high_level();
extern ssize_t _ble_ais_service_write(const void *p_buf, u16_t len);

// handle how the transport layer is built for a source build
#if !defined(HCI_TL_FULL) && !defined(HCI_TL_PTM)  && !defined(HCI_TL_NONE)
#if defined(HAL_UART) || defined(HAL_UART_SPI)
#if (HAL_UART == TRUE) || (HAL_UART_SPI != 0)
#define HCI_TL_FULL
#else // HAL_UART==FALSE/HAL_UART_SPI==0 or no/other defined value
#define HCI_TL_NONE
#endif // HAL_UART==TRUE || HAL_UART_SPI!=0
#endif // HAL_UART || HAL_UART_SPI
#endif // !HCI_TL_FULL && !HCI_TL_PTM && !HCI_TL_NONE

// HCI Packet Opcode Jump Table
cmdPktTable_t hciCmdTable[] = {
    // Linker Control Commands
    {HCI_DISCONNECT, hciDisconnect                    },
    {HCI_READ_REMOTE_VERSION_INFO, hciReadRemoteVersionInfo         },

    // Controller and Baseband Commands
    {HCI_SET_EVENT_MASK, hciSetEventMask                  },
    {HCI_RESET, hciReset                         },

    {HCI_READ_TRANSMIT_POWER, hciReadTransmitPowerLevel        },
    {HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL, hciSetControllerToHostFlowCtrl   },
    {HCI_HOST_BUFFER_SIZE, hciHostBufferSize                },
    {HCI_HOST_NUM_COMPLETED_PACKETS, hciHostNumCompletedPkt           },


// Informational Parameters
    {HCI_READ_LOCAL_VERSION_INFO, hciReadLocalVersionInfo          },
    {HCI_READ_LOCAL_SUPPORTED_COMMANDS, hciReadLocalSupportedCommands    },
    {HCI_READ_LOCAL_SUPPORTED_FEATURES, hciReadLocalSupportedFeatures    },
    {HCI_READ_BDADDR, hciReadBDADDR                    },
    {HCI_READ_RSSI, hciReadRssi                      },

    // LE Commands
    {HCI_LE_SET_EVENT_MASK, hciLESetEventMask                },
    {HCI_LE_READ_BUFFER_SIZE, hciLEReadBufSize                 },
    {HCI_LE_READ_LOCAL_SUPPORTED_FEATURES, hciLEReadLocalSupportedFeatures  },
    {HCI_LE_SET_RANDOM_ADDR, hciLESetRandomAddr               },

    {HCI_LE_SET_ADV_PARAM, hciLESetAdvParam                 },
    {HCI_LE_SET_ADV_DATA, hciLESetAdvData                  },
    {HCI_LE_SET_SCAN_RSP_DATA, hciLESetScanRspData              },
    {HCI_LE_SET_ADV_ENABLE, hciLESetAdvEnab                  },
    {HCI_LE_READ_ADV_CHANNEL_TX_POWER, hciLEReadAdvChanTxPower          },

    {HCI_LE_SET_SCAN_PARAM, hciLESetScanParam                },
    {HCI_LE_SET_SCAN_ENABLE, hciLESetScanEnable               },

    {HCI_LE_CREATE_CONNECTION, hciLECreateConn                  },
    {HCI_LE_CREATE_CONNECTION_CANCEL, hciLECreateConnCancel            },

    {HCI_LE_READ_WHITE_LIST_SIZE, hciLEReadWhiteListSize           },
    {HCI_LE_CLEAR_WHITE_LIST, hciLEClearWhiteList              },
    {HCI_LE_ADD_WHITE_LIST, hciLEAddWhiteList                },
    {HCI_LE_REMOVE_WHITE_LIST, hciLERemoveWhiteList             },

    {HCI_LE_CONNECTION_UPDATE, hciLEConnUpdate                  },
    {HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION, hciLESetHostChanClass            },
    {HCI_LE_READ_CHANNEL_MAP, hciLEReadChanMap                 },
    {HCI_LE_READ_REMOTE_USED_FEATURES, hciLEReadRemoteUsedFeatures      },

    {HCI_LE_ENCRYPT, hciLEEncrypt                     },
    {HCI_LE_RAND, hciLERand                        },

    {HCI_LE_START_ENCRYPTION, hciLEStartEncrypt                },

    {HCI_LE_LTK_REQ_REPLY, hciLELtkReqReply                 },
    {HCI_LE_LTK_REQ_NEG_REPLY, hciLELtkReqNegReply              },

    {HCI_LE_READ_SUPPORTED_STATES, hciLEReadSupportedStates         },
    {HCI_LE_RECEIVER_TEST, hciLEReceiverTest                },
    {HCI_LE_TRANSMITTER_TEST, hciLETransmitterTest             },
    {HCI_LE_TEST_END, hciLETestEnd                     },
    {HCI_LE_SET_DATA_LENGTH, hciLESetDataLength               },

    {HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH, hciLEReadSuggestedDefaultDataLength    },
    {HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH, hciLEWriteSuggestedDefaultDataLength    },
    {HCI_LE_READ_PHY, hciLEReadPhyMode                 },
    {HCI_LE_SET_DEFAULT_PHY, hciLESetDefaultPhyMode           },
    {HCI_LE_SET_PHY, hciLESetPhyMode                  },

// ===== 2020-07 add, RPA
    {HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST, hciLEAddDeviceToRL               },
    {HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST, hciLERemoveDeviceFromRL          },
    {HCI_LE_CLEAR_RESOLVING_LIST, hciLEClearRL                     },
    {HCI_LE_READ_RESOLVING_LIST_SIZE, hciLEReadRLSize                  },
    {HCI_LE_READ_PEER_RESOLVABLE_ADDRESS, hciLEReadPeerRA                  },
    {HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS, hciLEReadLocalRA                 },
    {HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE, hciLESetAddrResolutionEnable     },
    {HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TO, hciLESetRpaTo                    },
#ifdef HCI_BLE_50
    // extended adv
    {HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS, hciLESetAdvSetRandomAddress},
    {HCI_LE_SET_EXTENDER_ADVERTISING_PARAMETERS, hciLESetExtAdvParam        },
    {HCI_LE_SET_EXTENDED_ADVERTISING_DATA, hciLESetExtAdvData         },
    {HCI_LE_Set_EXTENDED_SCAN_RESPONSE_DATA, hciLESetExtScanRspData     },
    {HCI_LE_Set_EXTENDED_ADVERTISING_ENABLE, hciLESetExtAdvEnable       },
    {HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH, hciLEReadMaximumAdvDataLength},
    {HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS, hciLEReadNumberOfSupportAdvSet},
    {HCI_LE_REMOVE_ADVERTISING_SET, hciLERemoveAdvSet          },
    {HCI_LE_CLEAR_ADVERTISING_SETS, hciLEClearAdvSets          },

    // periodic adv
    {HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS, hciLESetPeriodicAdvParameter},
    {HCI_LE_SET_PERIODIC_ADVERTISING_DATA, hciLESetPeriodicAdvData   },
    {HCI_LE_Set_PERIODIC_ADVERTISING_ENABLE, hciLESetPeriodicAdvEnable },

    // extended scan
    {HCI_LE_SET_EXTENDED_SCAN_PARAMETERS, hciLESetExtendedScanParameters  },
    {HCI_LE_SET_EXTENDED_SCAN_ENABLE, hciLESetExtendedScanEnableCmd   },
    {HCI_LE_EXTENDED_CREATE_CONNECTION, hciLEExtendedCreateConnection   },

    // periodic scan
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC, hciLEPeriodicAdvertisingCreateSync       },
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL, hciLEPeriodicAdvertisingCreateSyncCancel },
    {HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC, hciLEPeriodicAdvertisingTerminateSync    },

    {HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST, hciLEAddDevToPeriodicAdvList      },
    {HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST, hciLERemovePeriodicAdvList        },
    {HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST, hciLEClearPeriodicAdvList         },
    {HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE, hciLEReadPeriodicAdvListSize      },

    {HCI_LE_READ_TRANSMIT_POWER, hciLEReadTransmitPower        },
    {HCI_LE_READ_RF_PATH_COMPENSATION, hciLEReadRfPathCompensation   },
    {HCI_LE_WRITE_RF_PATH_COMPENSATION, hciLEWriteRfPathCompensation  },

    {HCI_LE_SET_PRIVACY_MODE, hciLESetPrivacyMode},

    // 2020-10-27 CTE
    {HCI_LE_SET_CONNCTE_RECV_PARAMETER,   hciLESet_ConnectionCTE_ReceiveParam},
    {HCI_LE_CONN_CTE_REQUEST_ENABLE,   hciLE_ConnectionCTERequestEnable},

    // TODO: add  CTE relate HCI entries, 0x2051 - 0x2058

    // {HCI_LE_SET_PERIODIC_ADV_RECV_ENABLE        , hciLESetPrdAdvRecvEnableCmd},
#endif

#if 0
    // Vendor Specific Commands
    {HCI_EXT_SET_RX_GAIN, hciExtSetRxGain                  },
    {HCI_EXT_SET_TX_POWER, hciExtSetTxPower                 },
    {HCI_EXT_EXTEND_RF_RANGE, hciExtExtendRfRange              },
    {HCI_EXT_HALT_DURING_RF, hciExtHaltDuringRf               },

    {HCI_EXT_ONE_PKT_PER_EVT, hciExtOnePktPerEvt               },

    {HCI_EXT_CLK_DIVIDE_ON_HALT, hciExtClkDivOnHalt               },
    {HCI_EXT_DECLARE_NV_USAGE, hciExtDeclareNvUsage             },

    {HCI_EXT_DECRYPT, hciExtDecrypt                    },
    {HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES, hciExtSetLocalSupportedFeatures  },

    {HCI_EXT_SET_FAST_TX_RESP_TIME, hciExtSetFastTxResponseTime      },
    {HCI_EXT_OVERRIDE_SL, hciExtSetSlaveLatencyOverride    },

    {HCI_EXT_MODEM_TEST_TX, hciExtModemTestTx                },
    {HCI_EXT_MODEM_HOP_TEST_TX, hciExtModemHopTestTx             },
    {HCI_EXT_MODEM_TEST_RX, hciExtModemtestRx                },
    {HCI_EXT_END_MODEM_TEST, hciExtEndModemTest               },
    {HCI_EXT_SET_BDADDR, hciExtSetBDADDR                  },

    {HCI_EXT_SET_SCA, hciExtSetSCA                     },

    {HCI_EXT_SET_MAX_DTM_TX_POWER, hciExtSetMaxDtmTxPower           },
//  {HCI_EXT_MAP_PM_IO_PORT                   , hciExtMapPmIoPort                },
    {HCI_EXT_SET_FREQ_TUNE, hciExtSetFreqTune                },
    {HCI_EXT_SAVE_FREQ_TUNE, hciExtSaveFreqTune               },

    {HCI_EXT_DISCONNECT_IMMED, hciExtDisconnectImmed            },
    {HCI_EXT_PER, hciExtPER                        },
    {HCI_EXT_OVERLAPPED_PROCESSING, hciExtOverlappedProcessing       },
    {HCI_EXT_NUM_COMPLETED_PKTS_LIMIT, hciExtNumComplPktsLimit          },

    {HCI_EXT_BUILD_REVISION, hciExtBuildRevision              },
    {HCI_EXT_DELAY_SLEEP, hciExtDelaySleep                 },
    // TEMP: OVERLAPPED PROCESSING HOLDER
    {HCI_EXT_RESET_SYSTEM, hciExtResetSystem                },
#endif
    //=====
    {HCI_LE_READ_MAXIMUM_DATA_LENGTH,          hciLEReadMaxDataLength            },
    // Last Table Entry Delimiter
    {0xFFFF, NULL                             }
};

cmdPktTable_t hciVendorCmdTable[] = {

    // Last Table Entry Delimiter
    {0xFFFF, NULL                             }
};
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
#if 0
void HCI_CommandCompleteEvent1(uint16 opcode,
                               uint8  numParam,
                               uint8  *param)
{
    uint8_t msg[5 + numParam];
    msg[0] = 0x0e;
    msg[1] = 3 + numParam;
    msg[2] = hciCtrlCmdToken;
    msg[3] = LO_UINT16(opcode);
    msg[4] = HI_UINT16(opcode);
    memcpy(&msg[5], param, numParam);

    HCI_H5_Send(DATA_TYPE_EVENT, msg, 5 + numParam);
}
#endif
/*
** HCI OSAL API
*/
//#pragma GCC optimize("O0")
/*******************************************************************************
 * This is the HCI OSAL task initialization routine.
 *
 * Public function defined in hci.h.
 */
void HCI_Init1(uint8 taskID)
{
    // initialize the task for HCI-Controller
    hciTaskID      = taskID;
    hciTestTaskID  = 0;
    hciGapTaskID   = 0;
    hciL2capTaskID = 0;
    hciSmpTaskID   = 0;
    hciExtTaskID   = 0;

    // reset the Bluetooth and the BLE event mask bits
    hciInitEventMasks();

    // disable PTM runtime flag
    hciPTMenabled = FALSE;

    hciCtrlCmdToken = 1;

    return;
}
#if 0
//#pragma GCC optimize("Os")
/*******************************************************************************
 * This is the HCI OSAL task process event handler.
 *
 * Public function defined in hci.h.
 */
uint16 HCI_ProcessEvent1(uint8 task_id, uint16 events)
{
    osal_event_hdr_t *pMsg;

    // check for system messages
    if (events & SYS_EVENT_MSG) {
        pMsg = (osal_event_hdr_t *)osal_msg_receive(hciTaskID);

        if (pMsg) {
#if !defined(HCI_TL_NONE)

            switch (pMsg->event) {
                case HCI_HOST_TO_CTRL_DATA_EVENT:
                    // process HCI data packet
                    hciProcessHostToCtrlData((hciDataPacket_t *)pMsg);
                    break;

                case HCI_HOST_TO_CTRL_CMD_EVENT:
                    // process HCI command packet
                    hciProcessHostToCtrlCmd((hciPacket_t *)pMsg);
                    break;

                case HCI_CTRL_TO_HOST_EVENT:
                    hciProcessCtrlToHost((hciPacket_t *)pMsg);
                    break;

                default:
                    break;
            }

#else // HCI_TL_NONE

            if ((pMsg->event == HCI_HOST_TO_CTRL_DATA_EVENT) ||
                (pMsg->event == HCI_CTRL_TO_HOST_EVENT)) {
                // deallocate data
                osal_bm_free(((hciDataPacket_t *)pMsg)->pData);
            }

            // deallocate the message
            (void)osal_msg_deallocate((uint8 *)pMsg);
#endif // !HCI_TL_NONE
        }

        // return unproccessed events
        return (events ^ SYS_EVENT_MSG);
    }

    return (0);
}
#endif


void hciProcessHostToCtrlVendorCmd_RAW(uint8_t *Data, uint32_t len)
{
    uint16 cmdOpCode;
    uint8  status;
    uint8  i = 0;

    // retrieve opcode
    cmdOpCode = BUILD_UINT16(Data[0], Data[1]);
    status = 0;

    //printf("====> cmd: %x\n", cmdOpCode);

    // lookup corresponding function
    while ((hciVendorCmdTable[i].opCode != 0xFFFF) && (hciVendorCmdTable[i].hciFunc != NULL)) {
        // there's a valid entry at this index, but check if it's the one we want
        if (hciVendorCmdTable[i].opCode == cmdOpCode) { //((pMsg->pData[2] << 8 ) + pMsg->pData[1]))//cmdOpCode)

            // it is, so jump to this function
            (void)(hciVendorCmdTable[i].hciFunc)(&Data[3]);
            // done
            break;
        }

        // next...
        i++;
    }

    // check if a matching opcode was found
    if ((hciVendorCmdTable[i].opCode == 0xFFFF) && (hciVendorCmdTable[i].hciFunc == NULL)) {
        // none found, so return error
        status = HCI_ERROR_CODE_UNKNOWN_HCI_CMD;

        HCI_CommandCompleteEvent(cmdOpCode, 1, &status);
    }

    return;
}

void hciProcessHostToCtrlCmd_RAW(uint8_t *Data, uint32_t len)
{
    uint16 cmdOpCode;
    uint8  status;
    uint8  i = 0;

    // retrieve opcode
    cmdOpCode = BUILD_UINT16(Data[0], Data[1]);
    status = 0;

    //printf("====> cmd: %x\n", cmdOpCode);

    // lookup corresponding function
    while ((hciCmdTable[i].opCode != 0xFFFF) && (hciCmdTable[i].hciFunc != NULL)) {
        // there's a valid entry at this index, but check if it's the one we want
        if (hciCmdTable[i].opCode == cmdOpCode) { //((pMsg->pData[2] << 8 ) + pMsg->pData[1]))//cmdOpCode)

            // it is, so jump to this function
            (void)(hciCmdTable[i].hciFunc)(&Data[3]);
            // done
            break;
        }

        // next...
        i++;
    }

#if 1

    // check if a matching opcode was found
    if ((hciCmdTable[i].opCode == 0xFFFF) && (hciCmdTable[i].hciFunc == NULL)) {
        // none found, so return error
        status = HCI_ERROR_CODE_UNKNOWN_HCI_CMD;

        HCI_CommandCompleteEvent(cmdOpCode, 1, &status);
    }

#endif
    return;
}

/*******************************************************************************
 * @fn          hciProcessHostToCtrlCmd
 *
 * @brief       This routine handles HCI controller commands received from Host.
 *
 * input parameters
 *
 * @param       pMsg - Pointer to HCI command packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessHostToCtrlCmd(hciPacket_t *pMsg)
{
    uint16 cmdOpCode;
    uint8  status;
    uint8  i = 0;

    // retrieve opcode
    cmdOpCode = BUILD_UINT16(pMsg->pData[1], pMsg->pData[2]);
    status = 0;

    //printf("====> cmd: %x\n", cmdOpCode);

    // lookup corresponding function
    while ((hciCmdTable[i].opCode != 0xFFFF) && (hciCmdTable[i].hciFunc != NULL)) {
        // there's a valid entry at this index, but check if it's the one we want
        if (hciCmdTable[i].opCode == cmdOpCode) { //((pMsg->pData[2] << 8 ) + pMsg->pData[1]))//cmdOpCode)

            // it is, so jump to this function
            (void)(hciCmdTable[i].hciFunc)(&pMsg->pData[4]);
            // done
            break;
        }

        // next...
        i++;
    }

    // check if a matching opcode was found
    if ((hciCmdTable[i].opCode == 0xFFFF) && (hciCmdTable[i].hciFunc == NULL)) {
        // none found, so return error
        status = HCI_ERROR_CODE_UNKNOWN_HCI_CMD;

        HCI_CommandCompleteEvent(cmdOpCode, 1, &status);
    }

    // deallocate the message
    (void)osal_msg_deallocate((uint8 *)pMsg);

    return;
}

void hciProcessHostToCtrlData_Raw(hciDataPacket_t *pMsg)
{
    // two types of data possible
    switch (pMsg->pktType) {
        case HCI_ACL_DATA_PACKET:

            // check for a problem sending data
            // Note: Success either means the packet was sent and the buffer was
            //       freed, or the packet was queued for a later transmission.
            //printf("====> ACL data, len = %d\n", pMsg->pktLen);
            if (HCI_SendDataPkt(pMsg->connHandle,
                                pMsg->pbFlag,
                                pMsg->pktLen,
                                pMsg->pData) != HCI_SUCCESS) {
                // packet wasn't sent or queued, so free the user's data
                osal_bm_free((void *)pMsg->pData);
            }

            break;

        case HCI_SCO_DATA_PACKET:

        // ASSERT
        // DROP THROUGH
        //lint -fallthrough

        default:
            break;
    }

    // deallocate message
    (void)osal_msg_deallocate((uint8 *)pMsg);

    return;
}

//#pragma GCC optimize("Os")
/*******************************************************************************
 * @fn          hciProcessHostToCtrlData
 *
 * @brief       This routine handles HCI controller data received from Host.
 *
 *              HCI Data Packet frame format, and size in bytes:
 *              | Packet Type (1) | Handle (2) | Length (2) | Data (N) |
 *
 * input parameters
 *
 * @param       pMsg - Pointer to HCI data packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessHostToCtrlData(hciDataPacket_t *pMsg)
{
    // two types of data possible
    switch (pMsg->pktType) {
        case HCI_ACL_DATA_PACKET:

            // check for a problem sending data
            // Note: Success either means the packet was sent and the buffer was
            //       freed, or the packet was queued for a later transmission.
            //printf("====> ACL data, len = %d\n", pMsg->pktLen);
            if (HCI_SendDataPkt(pMsg->connHandle,
                                pMsg->pbFlag,
                                pMsg->pktLen,
                                pMsg->pData) != HCI_SUCCESS) {
                // packet wasn't sent or queued, so free the user's data
                osal_bm_free((void *)pMsg->pData);
            }

            break;

        case HCI_SCO_DATA_PACKET:

        // ASSERT
        // DROP THROUGH
        //lint -fallthrough

        default:
            break;
    }

    // deallocate message
    (void)osal_msg_deallocate((uint8 *)pMsg);

    return;
}
#if 0

/*******************************************************************************
 * @fn          hciProcessCtrlToHost
 *
 * @brief       This routine handles sending an HCI packet out the serial
 *              interface.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to the HCI packet.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void hciProcessCtrlToHost(hciPacket_t *pBuf)
{
    static uint8_t tmp_data[128];

    switch (pBuf->pData[0]) {
        case HCI_ACL_DATA_PACKET:
        case HCI_SCO_DATA_PACKET:

            // send through UART - dual solution
//      if ( HCI_UART_WriteTransport( pBuf->pData,
//                               HCI_DATA_MIN_LENGTH + BUILD_UINT16(pBuf->pData[3],
//                                                                  pBuf->pData[4]) ) == 0 )
//printf("<=== data: %d\n", HCI_DATA_MIN_LENGTH + BUILD_UINT16(pBuf->pData[3], pBuf->pData[4]));
            HCI_H5_Send(pBuf->pData[0],
                        &pBuf->pData[1],
                        HCI_DATA_MIN_LENGTH + BUILD_UINT16(pBuf->pData[3], pBuf->pData[4]) - 1);

            // free the packet buffer
            osal_bm_free(pBuf->pData);

            break;

        case HCI_EVENT_PACKET:

#if 0

            // send event through UART - dual solution
            if (HCI_UART_WriteTransport(&pBuf->pData[0],
                                        HCI_EVENT_MIN_LENGTH + pBuf->pData[2]) == 0) {
                // the data was not written. Restore it in the mesage queue.
                (void)osal_msg_push_front(hciTaskID, (uint8 *)pBuf);

                return; // we're done here!
            }

#endif
            //printf("<==== event: %d\n", HCI_EVENT_MIN_LENGTH + pBuf->pData[2]);
            memcpy(tmp_data, pBuf->pData,  HCI_EVENT_MIN_LENGTH + pBuf->pData[2]);
            osal_msg_deallocate((uint8 *)pBuf);

            //dischar adv report, when h5 queue is in high level
            if (tmp_data[1] == 0x3e && tmp_data[3] == 0x02) {
                //printf("dischar adv report\n");
                if (! rtk_unrel_is_high_level()) {
                    HCI_H5_Send(0x0D,
                                &tmp_data[1],
                                HCI_EVENT_MIN_LENGTH + tmp_data[2] - 1);
                }

                return;
            }

            //printf("<==== event: %d\n", HCI_EVENT_MIN_LENGTH + tmp_data[2]);
            HCI_H5_Send(tmp_data[0],
                        &tmp_data[1],
                        HCI_EVENT_MIN_LENGTH + tmp_data[2] - 1);
            return;

        default:
            break;
    }

    // deallocate the message if controller only
    (void)osal_msg_deallocate((uint8 *)pBuf);

    return;
}
#endif

/*
** Serial Packet Translation Functions for HCI APIs
*/

/*******************************************************************************
 * @fn          hciDisconnect
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciDisconnect(uint8 *pBuf)
{
    return HCI_DisconnectCmd(BUILD_UINT16(pBuf[0],
                                          pBuf[1]),
                             pBuf[2]);
}

/*******************************************************************************
 * @fn          hciReadRemoteVersionInfo
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadRemoteVersionInfo(uint8 *pBuf)
{
    return HCI_ReadRemoteVersionInfoCmd(BUILD_UINT16(pBuf[0],
                                        pBuf[1]));
}


/*******************************************************************************
 * @fn          hciSetEventMask
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciSetEventMask(uint8 *pBuf)
{
    return HCI_SetEventMaskCmd(pBuf);
}


/*******************************************************************************
 * @fn          hciReset
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReset(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    int err = HCI_ResetCmd();
    extern void llInitFeatureSetDLE(uint8 enable);
    llInitFeatureSetDLE(1);

#if defined(CONFIG_ENABLE_2M_PHY) && CONFIG_ENABLE_2M_PHY
    extern void llInitFeatureSet2MPHY(uint8 enable);
    llInitFeatureSet2MPHY(1);
#endif
    return err;
}


/*******************************************************************************
 * @fn          hciReadTransmitPowerLevel
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadTransmitPowerLevel(uint8 *pBuf)
{
    return HCI_ReadTransmitPowerLevelCmd(BUILD_UINT16(pBuf[0],
                                         pBuf[1]),
                                         pBuf[2]);
}


/*******************************************************************************
 * @fn          hciSetControllerToHostFlowCtrl
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciSetControllerToHostFlowCtrl(uint8 *pBuf)
{
    return HCI_SetControllerToHostFlowCtrlCmd(pBuf[0]);
}



/*******************************************************************************
 * @fn          hciHostBufferSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       hostAclPktLen        - Host ACL data packet length.
 * @param       hostSyncPktLen       - Host SCO data packet length .
 * @param       hostTotalNumAclPkts  - Host total number of ACL data packets.
 * @param       hostTotalNumSyncPkts - Host total number of SCO data packets.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciHostBufferSize(uint8 *pBuf)
{
    return HCI_HostBufferSizeCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                 pBuf[2],
                                 BUILD_UINT16(pBuf[3], pBuf[4]),
                                 BUILD_UINT16(pBuf[5], pBuf[6]));
}


/*******************************************************************************
 * @fn          hciHostNumCompletedPkt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciHostNumCompletedPkt(uint8 *pBuf)
{
    uint16 connHandle, numCmp;

    connHandle = BUILD_UINT16(pBuf[1], pBuf[2]);
    numCmp     = BUILD_UINT16(pBuf[3], pBuf[4]);
    return HCI_HostNumCompletedPktCmd(pBuf[0],
                                      &connHandle,
                                      &numCmp);
}

#define HCI_VERSION                                  0x09    // BT Core Specification V5.0
//0x06    // BT Core Specification V4.0

// Major Version (8 bits) . Minor Version (4 bits) . SubMinor Version (4 bits)
#define HCI_REVISION                                 0x0120  // HCI Version 1.2.0


/*
** Information Parameters
*/

/*******************************************************************************
 * This BT API is used to read the local version information.
 *
 * Public function defined in hci.h.
 */
hciStatus_t HCI_ReadLocalVersionInfoCmd_1(void)
{
    // 0: Status
    // 1: HCI Version Number
    // 2: HCI Revision Number LSB
    // 3: HCI Revision Number MSB
    // 4: Version Number
    // 5: Connection Handle LSB
    // 6: Connection Handle MSB
    // 7: LL Subversion Number LSB
    // 8: LL Subversion Number MSB
    uint8  rtnParam[9];
    uint8  version;
    uint16 comID;
    uint16 subverNum;

    // status
    rtnParam[0] = LL_ReadLocalVersionInfo(&version,
                                          &comID,
                                          &subverNum);

    // HCI version and revision
    rtnParam[1] = HCI_VERSION;
    rtnParam[2] = LO_UINT16(HCI_REVISION);
    rtnParam[3] = HI_UINT16(HCI_REVISION);

    // LL version, manufacturer name, LL subversion
    rtnParam[4] = version;
    rtnParam[5] = LO_UINT16(comID);
    rtnParam[6] = HI_UINT16(comID);
    rtnParam[7] = LO_UINT16(subverNum);
    rtnParam[8] = HI_UINT16(subverNum);

    HCI_CommandCompleteEvent(HCI_READ_LOCAL_VERSION_INFO, sizeof(rtnParam), rtnParam);

    return (HCI_SUCCESS);
}


/*******************************************************************************
 * @fn          hciReadLocalVersionInfo
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */



hciStatus_t hciReadLocalVersionInfo(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_ReadLocalVersionInfoCmd_1();
}


/*******************************************************************************
 * @fn          hciReadLocalSupportedCommands
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadLocalSupportedCommands(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_ReadLocalSupportedCommandsCmd();
}


/*******************************************************************************
 * @fn          hciReadLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadLocalSupportedFeatures(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_ReadLocalSupportedFeaturesCmd();
}


/*******************************************************************************
 * @fn          hciReadRssi
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadRssi(uint8 *pBuf)
{
    return HCI_ReadRssiCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}


hciStatus_t HCI_LE_SetEventMaskCmd1(uint8 *pEventMask)
{
    hciStatus_t status;

    // check parameters
    if (pEventMask != NULL) {
        // set the BLE event mask
        // Note: So far, only the first byte is used.
        bleEvtMask = BUILD_UINT32(pEventMask[0], pEventMask[1], pEventMask[2], pEventMask[3]);
        bleEvtMask |= 0x01;
        status = HCI_SUCCESS;
    } else { // bad parameters
        status = HCI_ERROR_CODE_INVALID_HCI_CMD_PARAMS;
    }

    HCI_CommandCompleteEvent(HCI_LE_SET_EVENT_MASK, sizeof(status), &status);

    return (HCI_SUCCESS);
}

/*******************************************************************************
 * @fn          hciLESetEventMask
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetEventMask(uint8 *pBuf)
{
    return HCI_LE_SetEventMaskCmd1(pBuf);
}


/*******************************************************************************
 * @fn          hciLEReadBufSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
#ifndef   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX
#define   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX       2
#endif
hciStatus_t hciLEReadBufSize(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;
    uint8 msg[4] = {0};

    msg[0] = HCI_SUCCESS;
    msg[1] = LO_UINT16(27);
    msg[2] = HI_UINT16(27);
    msg[3] = CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX;
    HCI_CommandCompleteEvent(HCI_LE_READ_BUFFER_SIZE, sizeof(msg), msg);
    return 0;
}


/*******************************************************************************
 * @fn          hciLEReadLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadLocalSupportedFeatures(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_ReadLocalSupportedFeaturesCmd();
}


/*******************************************************************************
 * @fn          hciLESetRandomAddr
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetRandomAddr(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_SetRandomAddressCmd(pBuf);
}


/*******************************************************************************
 * @fn          hciLESetAdvParam
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvParam(uint8 *pBuf)
{
    hciStatus_t status;

    if (llState == LL_STATE_INIT) {
        status = HCI_ERROR_CODE_CMD_DISALLOWED;
    } else {
        if (adv_param.advMode == 1) {
            /* Try to fix advMode missmatch bug */
            adv_param.advMode = 0;
        }

        status = LL_SetAdvParam(BUILD_UINT16(pBuf[0], pBuf[1]),
                                BUILD_UINT16(pBuf[2], pBuf[3]),
                                pBuf[4],
                                pBuf[5],
                                pBuf[6],
                                &pBuf[7],
                                pBuf[13],
                                pBuf[14]);
    }

    HCI_CommandCompleteEvent(HCI_LE_SET_ADV_PARAM, sizeof(status), &status);

    return (HCI_SUCCESS);
}


/*******************************************************************************
 * @fn          hciLESetAdvData
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvData(uint8 *pBuf)
{
    hciStatus_t status;

    if (llState == LL_STATE_INIT) {
        status = HCI_ERROR_CODE_CMD_DISALLOWED;
    } else {
        status = LL_SetAdvData(pBuf[0],
                               &pBuf[1]);
    }

    HCI_CommandCompleteEvent(HCI_LE_SET_ADV_DATA, sizeof(status), &status);

    return (HCI_SUCCESS);
}


/*******************************************************************************
 * @fn          hciLESetScanRspData
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanRspData(uint8 *pBuf)
{
    return HCI_LE_SetScanRspDataCmd(pBuf[0],
                                    &pBuf[1]);
}

llStatus_t HCI_LL_SetAdvControl(uint8 advMode);
/*******************************************************************************
 * @fn          hciLESetAdvEnab
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetAdvEnab(uint8 *pBuf)
{
    hciStatus_t status;

    int ret;

    if (pBuf[0]) {
        ret = wait_llWaitingIrq(100);

        if (ret) {
            return ret;
        }
    }

    if (llState == LL_STATE_INIT) {
        status = HCI_ERROR_CODE_CMD_DISALLOWED;
    } else {
        status = HCI_LL_SetAdvControl(pBuf[0]);
    }

    if (llSecondaryState == LL_SEC_STATE_IDLE_PENDING) {
        llSecondaryState = LL_SEC_STATE_IDLE;
    } else {
        osal_stop_timerEx(LL_TaskID, LL_EVT_SECONDARY_ADV);
    }

    if (ret == 0) {
        extern struct buf_rx_desc g_rx_adv_buf;
        g_rx_adv_buf.rxheader = 0;
    }

    HCI_CommandCompleteEvent(HCI_LE_SET_ADV_ENABLE, sizeof(status), &status);

    return (HCI_SUCCESS);
}

/*******************************************************************************
 * @fn          hciLEReadAdvChanTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadAdvChanTxPower(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_ReadAdvChanTxPowerCmd();
}

/*******************************************************************************
 * @fn          hciLESetScanParam
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanParam(uint8 *pBuf)
{
    return HCI_LE_SetScanParamCmd(pBuf[0],
                                  BUILD_UINT16(pBuf[1], pBuf[2]),
                                  BUILD_UINT16(pBuf[3], pBuf[4]),
                                  pBuf[5],
                                  pBuf[6]);
}
llStatus_t HCI_LL_SetScanControl(uint8 scanMode,
                                 uint8 filterReports);
/*******************************************************************************
 * @fn          hciLESetScanEnable
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetScanEnable(uint8 *pBuf)
{
    hciStatus_t status = 0;

    if (pBuf[0]) {
        int ret = wait_llWaitingIrq(100);

        if (ret) {
            return ret;
        }
    }

    if (llState == LL_STATE_INIT) {
        status = HCI_ERROR_CODE_CMD_DISALLOWED;
    } else {
        status = HCI_LL_SetScanControl(pBuf[0], pBuf[1]);
    }

    HCI_CommandCompleteEvent(HCI_LE_SET_SCAN_ENABLE, sizeof(status), &status);

    return (HCI_SUCCESS);
}

uint8_t peerInfo_recored[7] = {0};

/*******************************************************************************
 * @fn          hciLECreateConn
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLECreateConn(uint8 *pBuf)
{
    //hciStatus_t status;

#if 0
    if (llState != LL_STATE_IDLE) {
        status = HCI_ERROR_CODE_CMD_DISALLOWED;
        HCI_CommandCompleteEvent(HCI_LE_CREATE_CONNECTION, sizeof(status), &status);
        return HCI_SUCCESS;
    }

#endif

    int ret = wait_llWaitingIrq(100);

    if (ret) {
        return ret;
    }

    memcpy(peerInfo_recored, &pBuf[5], 7);
    return HCI_LE_CreateConnCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                BUILD_UINT16(pBuf[2], pBuf[3]),
                                pBuf[4],
                                pBuf[5],
                                &pBuf[6],
                                pBuf[12],
                                BUILD_UINT16(pBuf[13], pBuf[14]),
                                BUILD_UINT16(pBuf[15], pBuf[16]),
                                BUILD_UINT16(pBuf[17], pBuf[18]),
                                BUILD_UINT16(pBuf[19], pBuf[20]),
                                BUILD_UINT16(pBuf[21], pBuf[22]),
                                BUILD_UINT16(pBuf[23], pBuf[24]));
}

/*******************************************************************************
 * @fn          hciLECreateConnCancel
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLECreateConnCancel(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;
    return HCI_LE_CreateConnCancelCmd();
}



/*******************************************************************************
 * @fn          hciLEReadWhiteListSize
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadWhiteListSize(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_ReadWhiteListSizeCmd();
}


/*******************************************************************************
 * @fn          hciLEClearWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEClearWhiteList(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_ClearWhiteListCmd();
}


/*******************************************************************************
 * @fn          hciLEAddWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEAddWhiteList(uint8 *pBuf)
{
    uint8 addrType = pBuf[0];
    uint8_t *devAddr = &pBuf[1];
    int i, j;

    for (i = 0; i < LL_WHITELIST_ENTRY_NUM; i++) {
        if (g_llWhitelist[i].peerAddrType == addrType) {
            for (j = 0; j < LL_DEVICE_ADDR_LEN; j++) {
                if (g_llWhitelist[i].peerAddr[j] != devAddr[j]) {
                    break;
                }
            }

            if (j == LL_DEVICE_ADDR_LEN) {
                uint8  status = 0;
                HCI_CommandCompleteEvent(HCI_LE_ADD_WHITE_LIST, sizeof(status), &status);
                return 0;
            }
        }
    }

    return HCI_LE_AddWhiteListCmd(pBuf[0],
                                  &pBuf[1]);
}


/*******************************************************************************
 * @fn          hciLERemoveWhiteList
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERemoveWhiteList(uint8 *pBuf)
{
    return HCI_LE_RemoveWhiteListCmd(pBuf[0],
                                     &pBuf[1]);
}


/*******************************************************************************
 * @fn          hciLEConnUpdate
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEConnUpdate(uint8 *pBuf)
{
    uint32_t cs;
    cs = csi_irq_save();

    //walkaround LL_ConnUpdata bug in ROM
    extern llConns_t    g_ll_conn_ctx;
    g_ll_conn_ctx.scheduleInfo[BUILD_UINT16(pBuf[0], pBuf[1])].linkRole = LL_ROLE_INVALID;
    int ret = HCI_LE_ConnUpdateCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                   BUILD_UINT16(pBuf[2], pBuf[3]),
                                   BUILD_UINT16(pBuf[4], pBuf[5]),
                                   BUILD_UINT16(pBuf[6], pBuf[7]),
                                   BUILD_UINT16(pBuf[8], pBuf[9]),
                                   BUILD_UINT16(pBuf[10], pBuf[11]),
                                   BUILD_UINT16(pBuf[12], pBuf[13]));

    g_ll_conn_ctx.scheduleInfo[BUILD_UINT16(pBuf[0], pBuf[1])].linkRole = LL_ROLE_MASTER;
    csi_irq_restore(cs);

    return ret;
}



/*******************************************************************************
 * @fn          hciLESetHostChanClass
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLESetHostChanClass(uint8 *pBuf)
{
    return HCI_LE_SetHostChanClassificationCmd(pBuf);
}



/*******************************************************************************
 * @fn          hciLEReadChanMap
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadChanMap(uint8 *pBuf)
{
    return HCI_LE_ReadChannelMapCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}



/*******************************************************************************
 * @fn          hciLEReadRemoteUsedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadRemoteUsedFeatures(uint8 *pBuf)
{
    return HCI_LE_ReadRemoteUsedFeaturesCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}



/*******************************************************************************
 * @fn          hciLEEncrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEEncrypt(uint8 *pBuf)
{
    // reverse byte order of key (MSB..LSB required)
    HCI_ReverseBytes(&pBuf[0], KEYLEN);

    // reverse byte order of plaintext (MSB..LSB required)
    HCI_ReverseBytes(&pBuf[KEYLEN], KEYLEN);

    return HCI_LE_EncryptCmd(&pBuf[0],
                             &pBuf[KEYLEN]);
}


/*******************************************************************************
 * @fn          hciLERand
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLERand(uint8 *pBuf)
{
    (void)pBuf;

    return HCI_LE_RandCmd();
}


/*******************************************************************************
 * @fn          hciLEStartEncrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEStartEncrypt(uint8 *pBuf)
{
    return HCI_LE_StartEncyptCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                 &pBuf[2],
                                 &pBuf[10],
                                 &pBuf[12]);
}


/*******************************************************************************
 * @fn          hciLELtkReqReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLELtkReqReply(uint8 *pBuf)
{
    return HCI_LE_LtkReqReplyCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                 &pBuf[2]);
}


/*******************************************************************************
 * @fn          hciLELtkReqNegReply
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLELtkReqNegReply(uint8 *pBuf)
{
    return HCI_LE_LtkReqNegReplyCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}


/*******************************************************************************
 * @fn          hciLEReadSupportedStates
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReadSupportedStates(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_ReadSupportedStatesCmd();
}


/*******************************************************************************
 * @fn          hciLEReceiverTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLEReceiverTest(uint8 *pBuf)
{
    return HCI_LE_ReceiverTestCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciLETransmitterTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLETransmitterTest(uint8 *pBuf)
{
    return HCI_LE_TransmitterTestCmd(pBuf[0],
                                     pBuf[1],
                                     pBuf[2]);
}

/*******************************************************************************
 * @fn          hciLETestEnd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciLETestEnd(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_LE_TestEndCmd();
}


hciStatus_t hciLESetDataLength(uint8 *pBuf)
{
    return HCI_LE_SetDataLengthCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                   BUILD_UINT16(pBuf[2], pBuf[3]),
                                   BUILD_UINT16(pBuf[4], pBuf[5]));
}

hciStatus_t hciLEReadSuggestedDefaultDataLength(uint8 *pBuf)
{
    (void)pBuf;

    return HCI_LE_ReadSuggestedDefaultDataLengthCmd();
}

hciStatus_t hciLEWriteSuggestedDefaultDataLength(uint8 *pBuf)
{
    return HCI_LE_WriteSuggestedDefaultDataLengthCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
            BUILD_UINT16(pBuf[2], pBuf[3]));
}

hciStatus_t hciLEReadPhyMode(uint8 *pBuf)
{
    return HCI_LE_ReadPhyMode(BUILD_UINT16(pBuf[0], pBuf[1]));
}

hciStatus_t hciLESetDefaultPhyMode(uint8 *pBuf)
{
    return HCI_LE_SetDefaultPhyMode(0, pBuf[0], pBuf[1], pBuf[2]);
}

hciStatus_t hciLESetPhyMode(uint8 *pBuf)
{
    return HCI_LE_SetPhyMode(BUILD_UINT16(pBuf[0], pBuf[1]),
                             pBuf[2], pBuf[3], pBuf[4],
                             BUILD_UINT16(pBuf[5], pBuf[6]));
}

// 2020-07  add
hciStatus_t hciLEAddDeviceToRL(uint8 *pBuf)
{
    return HCI_LE_AddDevToResolvingListCmd(pBuf[0],
                                           &pBuf[1],
                                           &pBuf[7],
                                           &pBuf[23]);
}

hciStatus_t hciLERemoveDeviceFromRL(uint8 *pBuf)
{
    return HCI_LE_RemoveResolvingListCmd(pBuf[0],
                                         &pBuf[1]);
}

hciStatus_t hciLEClearRL(uint8 *pBuf)
{
    (void)pBuf;
    return HCI_LE_ClearResolvingListCmd();
}

hciStatus_t hciLEReadRLSize(uint8 *pBuf)
{
    (void)pBuf;
    return HCI_LE_ReadResolvingListSizeCmd();
}

hciStatus_t hciLEReadPeerRA(uint8 *pBuf)
{
//    return HCI_LE_ReadPeerResolvableAddressCmd(pBuf[0],
//                                            &pBuf[1]);
    return 0;
}

hciStatus_t hciLEReadLocalRA(uint8 *pBuf)
{
    //return HCI_LE_ReadLocalResolvableAddressCmd(pBuf[0],
    //        &pBuf[1]);
    return 0;
}

hciStatus_t hciLESetAddrResolutionEnable(uint8 *pBuf)
{
    return HCI_LE_SetAddressResolutionEnableCmd(pBuf[0]);
}

hciStatus_t hciLESetRpaTo(uint8 *pBuf)
{
    return HCI_LE_SetResolvablePrivateAddressTimeoutCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}

#ifdef  HCI_BLE_50
// extended adv
hciStatus_t hciLESetAdvSetRandomAddress(uint8 *pBuf)
{
    return HCI_LE_SetExtAdvSetRandomAddressCmd(pBuf[0],
            &pBuf[1]);
}

hciStatus_t hciLESetExtAdvParam(uint8 *pBuf)
{
    return HCI_LE_SetExtAdvParamCmd(pBuf[0],
                                    BUILD_UINT16(pBuf[1], pBuf[2]),
                                    BUILD_UINT32(pBuf[3], pBuf[4], pBuf[5], 0),
                                    BUILD_UINT32(pBuf[6], pBuf[7], pBuf[8], 0),
                                    pBuf[9],
                                    pBuf[10],
                                    pBuf[11],
                                    &pBuf[12],
                                    pBuf[18],
                                    pBuf[19],
                                    pBuf[20],
                                    pBuf[21],
                                    pBuf[22],
                                    pBuf[23],
                                    pBuf[24]
                                   );
}

hciStatus_t hciLESetExtAdvData(uint8 *pBuf)
{
    return HCI_LE_SetExtAdvDataCmd(pBuf[0],
                                   pBuf[1],
                                   pBuf[2],
                                   pBuf[3],
                                   &pBuf[4]);
}

hciStatus_t hciLESetExtScanRspData(uint8 *pBuf)
{
    return HCI_LE_SetExtScanRspDataCmd(pBuf[0],
                                       pBuf[1],
                                       pBuf[2],
                                       pBuf[3],
                                       &pBuf[4]
                                      );
}
extern uint8 g_llAdvMode;


// note: number_of_sets should be 1
#define   LL_MAX_ADV_SET                     6
hciStatus_t hciLESetExtAdvEnable(uint8 *pBuf)
{
    uint8 number_of_sets = pBuf[1];
    uint8  adv_handler[LL_MAX_ADV_SET];
    uint16 duration[LL_MAX_ADV_SET];
    uint8  max_ext_adv_evt[LL_MAX_ADV_SET];

    if (number_of_sets > LL_MAX_ADV_SET || number_of_sets == 0) {
        return 0x12;
    }

    for (int i = 0; i < number_of_sets; i++) {
        adv_handler[i] = pBuf[2 + i * 4];
        duration[i]    = BUILD_UINT16(pBuf[3 + i * 4], pBuf[4 + i * 4]);
        max_ext_adv_evt[i] = pBuf[5 + i * 4];
    }

    return HCI_LE_SetExtAdvEnableCmd(pBuf[0],
                                     number_of_sets,                     // uint8  number_of_sets,
                                     adv_handler,                     // uint8  *advertising_handle,
                                     duration,                      // uint16 *duration,
                                     max_ext_adv_evt                // uint8  *max_extended_advertising_events
                                    );
}

hciStatus_t hciLEReadMaximumAdvDataLength(uint8 *pBuf)
{
    return HCI_LE_ReadMaximumAdvDataLengthCmd();

}

hciStatus_t hciLEReadNumberOfSupportAdvSet(uint8 *pBuf)
{
    return HCI_LE_ReadNumberOfSupportAdvSetCmd();
}

hciStatus_t hciLERemoveAdvSet(uint8 *pBuf)
{
    return HCI_LE_RemoveAdvSetCmd(pBuf[0]);

}

hciStatus_t hciLEClearAdvSets(uint8 *pBuf)
{
    return HCI_LE_ClearAdvSetsCmd();
}


hciStatus_t hciLESetPeriodicAdvParameter(uint8 *pBuf)
{
    return HCI_LE_SetPeriodicAdvParameterCmd(pBuf[0],
            BUILD_UINT16(pBuf[1], pBuf[2]),
            BUILD_UINT16(pBuf[3], pBuf[4]),
            BUILD_UINT16(pBuf[5], pBuf[6])
                                            );
}

hciStatus_t hciLESetPeriodicAdvData(uint8 *pBuf)
{
    return HCI_LE_SetPeriodicAdvDataCmd(pBuf[0],
                                        pBuf[1],
                                        pBuf[2],
                                        &pBuf[3]);
}

hciStatus_t hciLESetPeriodicAdvEnable(uint8 *pBuf)
{

    return HCI_LE_SetPeriodicAdvEnableCmd(pBuf[0],
                                          pBuf[1]
                                         );
}

hciStatus_t hciLESetExtendedScanParameters(uint8 *pBuf)
{
    uint8 scanPhy, numPhy, offset = 3;
    uint8 scan_types[2];
    uint16  scan_interval[2], scan_window[2];

    scanPhy = pBuf[2];

    numPhy = (scanPhy & LL_SCAN_PHY_1M_BITMASK) + ((scanPhy & LL_SCAN_PHY_CODED_BITMASK) >> 2);

    for (int i = 0; i < numPhy; i++) {
        scan_types[i] = pBuf[offset];
        offset ++;
        scan_interval[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        scan_window[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
    }

    return HCI_LE_SetExtendedScanParametersCmd(pBuf[0],
            pBuf[1],
            pBuf[2],
            scan_types,                 // uint8 *scan_sype,
            scan_interval,             // uint16 *scan_interval
            scan_window);             // uint16 *scan_window
}

hciStatus_t hciLESetExtendedScanEnableCmd(uint8 *pBuf)
{
    return HCI_LE_SetExtendedScanEnableCmd(pBuf[0],
                                           pBuf[1],
                                           BUILD_UINT16(pBuf[2], pBuf[3]),
                                           BUILD_UINT16(pBuf[4], pBuf[5]));
}

hciStatus_t hciLEExtendedCreateConnection(uint8 *pBuf)
{
    uint8 initPhy = pBuf[9], numPhy;
    uint8 offset;

    uint16 scan_interval[3];
    uint16 scan_window[3];
    uint16 conn_interval_min[3];
    uint16 conn_interval_max[3];
    uint16 conn_latency[3];
    uint16 supervision_timeout[3];
    uint16 minimum_CE_length[3];
    uint16 maximum_CE_length[3];

    numPhy = (initPhy & 0x01)
             + ((initPhy & 0x02) >> 1)
             + ((initPhy & 0x04) >> 2);

    offset = 10;

    for (int i = 0; i < numPhy; i++) {
        scan_interval[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        scan_window[i]   = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        conn_interval_min[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        conn_interval_max[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        conn_latency[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        supervision_timeout[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        minimum_CE_length[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
        maximum_CE_length[i] = BUILD_UINT16(pBuf[offset], pBuf[offset + 1]);
        offset += 2;
    }


    return HCI_LE_ExtendedCreateConnectionCmd(pBuf[0],     // uint8 initiator_filter_policy,
            pBuf[1],     // uint8 own_address_type,
            pBuf[2],     // uint8 peer_address_type,
            &pBuf[3],     // uint8 *peer_address,
            pBuf[9],     // uint8  initiating_PHYs,
            scan_interval,        // uint16 *scan_interval,
            scan_window,          // uint16 *scan_window,
            conn_interval_min,    // uint16 *conn_interval_min,
            conn_interval_max,    // uint16 *conn_interval_max,
            conn_latency,         // uint16 *conn_latency,
            supervision_timeout,  // uint16 *supervision_timeout,
            minimum_CE_length,    // uint16 *minimum_CE_length,
            maximum_CE_length);   // uint16 *maximum_CE_length)
}


hciStatus_t hciLEPeriodicAdvertisingCreateSync(uint8 *pBuf)
{
    return HCI_LE_PeriodicAdvertisingCreateSyncCmd(pBuf[0],
            pBuf[1],
            pBuf[2],
            &pBuf[3],
            BUILD_UINT16(pBuf[9], pBuf[10]),
            BUILD_UINT16(pBuf[11], pBuf[12]),
            pBuf[13]);
}

hciStatus_t hciLEPeriodicAdvertisingCreateSyncCancel(uint8 *pBuf)
{
    return HCI_LE_PeriodicAdvertisingCreateSyncCancelCmd();
}

hciStatus_t hciLEPeriodicAdvertisingTerminateSync(uint8 *pBuf)
{
    return HCI_LE_PeriodicAdvertisingTerminateSyncCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}


hciStatus_t hciLEAddDevToPeriodicAdvList(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_AddDevToPeriodicAdvListCmd    (pBuf[0],
//                                              &pBuf[1],
//                                               pBuf[7]);
}

hciStatus_t hciLERemovePeriodicAdvList(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_RemovePeriodicAdvListCmd    (pBuf[0],
//                                            &pBuf[1],
//                                             pBuf[7]);
}

hciStatus_t hciLEClearPeriodicAdvList(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_ClearPeriodicAdvListCmd    ();
}

hciStatus_t hciLEReadPeriodicAdvListSize(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_ReadPeriodicAdvListSizeCmd    ();
}


hciStatus_t hciLEReadTransmitPower(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_Read_Transmit_PowerCmd    ();
}

hciStatus_t hciLEReadRfPathCompensation(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_Read_Rf_Path_CompensationCmd    ();
}

hciStatus_t hciLEWriteRfPathCompensation(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_Write_Rf_Path_CompensationCmd    (BUILD_UINT16(pBuf[0], pBuf[1]),
//                                                  BUILD_UINT16(pBuf[2], pBuf[3]));
}

hciStatus_t hciLESetPrdAdvRecvEnableCmd(uint8 *pBuf)
{
    return HCI_LE_SetPrdAdvRecvEnableCmd(BUILD_UINT16(pBuf[0], pBuf[1]),
                                         pBuf[2]);
}

hciStatus_t hciLESet_ConnectionCTE_ReceiveParam(uint8 *pBuf)
{
    return 0;
//  return (HCI_LE_Set_ConnectionCTE_ReceiveParamCmd(BUILD_UINT16(pBuf[0],pBuf[1]),         //connHandle,
//                                              pBuf[2],                                     // enable,
//                                              pBuf[3],                                     // slot_Duration,
//                                              pBuf[4],                                     // pattern_len,
//                                              &pBuf[5]));                                  // *AnaIDs);

}

hciStatus_t hciLE_ConnectionCTERequestEnable(uint8 *pBuf)
{
    return 0;
//  return (HCI_LE_Connection_CTE_Request_EnableCmd(    BUILD_UINT16(pBuf[0],pBuf[1]),  // connHandle,
//                                                      pBuf[2],                        // enable,
//                                                      BUILD_UINT16(pBuf[3],pBuf[4]),  // Interval,
//                                                      pBuf[5],                        // len,
//                                                      pBuf[6]));                      // type

}

hciStatus_t hciLESetPrivacyMode(uint8 *pBuf)
{
    return 0;
//    return HCI_LE_Set_Privacy_ModeCmd    (pBuf[0],
//                                       &pBuf[1],
//                                        pBuf[7]);
}

#endif



// ================
hciStatus_t hciLEReadMaxDataLength(uint8 *pBuf)
{
    return HCI_LE_ReadMaxDataLengthCmd();
}

/*******************************************************************************
 * @fn          hciReadBDADDR
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciReadBDADDR(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_ReadBDADDRCmd();
}

#if 0
/*
** Vendor Specific Commands
*/

/*******************************************************************************
 * @fn          hciExtSetRxGain
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetRxGain(uint8 *pBuf)
{
    return HCI_EXT_SetRxGainCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtSetTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetTxPower(uint8 *pBuf)
{
    return HCI_EXT_SetTxPowerCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtExtendRfRange
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtExtendRfRange(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return 0;
//  return HCI_EXT_ExtendRfRangeCmd();
}


/*******************************************************************************
 * @fn          hciExtHaltDuringRf
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtHaltDuringRf(uint8 *pBuf)
{
    return HCI_EXT_HaltDuringRfCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtSetMaxDtmTxPower
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetMaxDtmTxPower(uint8 *pBuf)
{
    return HCI_EXT_SetMaxDtmTxPowerCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtDisconnectImmed
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDisconnectImmed(uint8 *pBuf)
{
    return HCI_EXT_DisconnectImmedCmd(BUILD_UINT16(pBuf[0],
                                      pBuf[1]));
}


/*******************************************************************************
 * @fn          hciExtPER
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtPER(uint8 *pBuf)
{
    return HCI_EXT_PacketErrorRateCmd(BUILD_UINT16(pBuf[0],
                                      pBuf[1]),
                                      pBuf[2]);
}



/*******************************************************************************
 * @fn          hciExtOverlappedProcessing
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtOverlappedProcessing(uint8 *pBuf)
{
    return HCI_EXT_OverlappedProcessingCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtNumComplPktsLimit
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtNumComplPktsLimit(uint8 *pBuf)
{
    return HCI_EXT_NumComplPktsLimitCmd(pBuf[0],
                                        pBuf[1]);
}

/*******************************************************************************
 * @fn          hciExtOnePktPerEvt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtOnePktPerEvt(uint8 *pBuf)
{
    return HCI_EXT_OnePktPerEvtCmd(pBuf[0]);
}



/*******************************************************************************
 * @fn          hciExtClkDivOnHalt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtClkDivOnHalt(uint8 *pBuf)
{
    return HCI_EXT_ClkDivOnHaltCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtDeclareNvUsage
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDeclareNvUsage(uint8 *pBuf)
{
    return HCI_EXT_DeclareNvUsageCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtDecrypt
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDecrypt(uint8 *pBuf)
{
    // reverse byte order of key (MSB..LSB required)
    HCI_ReverseBytes(&pBuf[0], KEYLEN);

    // reverse byte order of encText (MSB..LSB required)
    HCI_ReverseBytes(&pBuf[KEYLEN], KEYLEN);

    return HCI_EXT_DecryptCmd(&pBuf[0],
                              &pBuf[KEYLEN]);
}


/*******************************************************************************
 * @fn          hciExtSetLocalSupportedFeatures
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetLocalSupportedFeatures(uint8 *pBuf)
{
    return HCI_EXT_SetLocalSupportedFeaturesCmd(pBuf);
}


/*******************************************************************************
 * @fn          hciExtSetFastTxResponseTime
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetFastTxResponseTime(uint8 *pBuf)
{
    return HCI_EXT_SetFastTxResponseTimeCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtSetSlaveLatencyOverride
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetSlaveLatencyOverride(uint8 *pBuf)
{
    return HCI_EXT_SetSlaveLatencyOverrideCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtSetSCA
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetSCA(uint8 *pBuf)
{
    return HCI_EXT_SetSCACmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}


/*******************************************************************************
 * @fn          hciExtBuildRevision
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtBuildRevision(uint8 *pBuf)
{
    // unused input parameter; PC-Lint error 715.
    (void)pBuf;

    return HCI_EXT_BuildRevisionCmd(pBuf[0], BUILD_UINT16(pBuf[1],
                                    pBuf[2]));
}


/*******************************************************************************
 * @fn          hciExtDelaySleep
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtDelaySleep(uint8 *pBuf)
{
    return HCI_EXT_DelaySleepCmd(BUILD_UINT16(pBuf[0], pBuf[1]));
}


/*******************************************************************************
 * @fn          hciExtResetSystem
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtResetSystem(uint8 *pBuf)
{
    return HCI_EXT_ResetSystemCmd(pBuf[0]);
}


/*
** Allowed PTM Commands
*/




/*******************************************************************************
 * @fn          hciExtModemTestTx
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemTestTx(uint8 *pBuf)
{
    return HCI_EXT_ModemTestTxCmd(pBuf[0], pBuf[1]);
}


/*******************************************************************************
 * @fn          hciExtModemHopTestTxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemHopTestTx(uint8 *pBuf)
{
    return HCI_EXT_ModemHopTestTxCmd();
}


/*******************************************************************************
 * @fn          hciExtModemtestRxCmd
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtModemtestRx(uint8 *pBuf)
{
    return HCI_EXT_ModemTestRxCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtEndModemTest
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEndModemTest(uint8 *pBuf)
{
    return HCI_EXT_EndModemTestCmd();
}


/*******************************************************************************
 * @fn          hciExtSetBDADDR
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetBDADDR(uint8 *pBuf)
{
    return HCI_EXT_SetBDADDRCmd(pBuf);
}


/*******************************************************************************
 * @fn          hciExtEnablePTM
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtEnablePTM(uint8 *pBuf)
{
    return HCI_EXT_EnablePTMCmd();
}


/*******************************************************************************
 * @fn          hciExtSetFreqTune
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSetFreqTune(uint8 *pBuf)
{
    return HCI_EXT_SetFreqTuneCmd(pBuf[0]);
}


/*******************************************************************************
 * @fn          hciExtSaveFreqTune
 *
 * @brief       Serial interface translation function for HCI API.
 *
 * input parameters
 *
 * @param       pBuf - Pointer to command parameters and payload.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      hciStatus_t
 */
hciStatus_t hciExtSaveFreqTune(uint8 *pBuf)
{
    return HCI_EXT_SaveFreqTuneCmd();
}

#endif

#if 0

void HCI_H5_RecvCallback(hci_data_type_t packet_type, uint8_t *data, uint32_t length)
{
    if (packet_type == HCI_CMD_PACKET) {


        hciProcessHostToCtrlCmd_RAW(data, length);
#if 0
        hciPacket_t *pMsg;

        // there's enough serial data to finish this packet, so allocate memory
        pMsg = (hciPacket_t *)osal_msg_allocate(sizeof(hciPacket_t) +
                                                HCI_CMD_MIN_LENGTH   +
                                                length);

        if (pMsg) {
            // fill in message data
            pMsg->pData    = (uint8 *)(pMsg + 1);
            pMsg->pData[0] = packet_type;
            memcpy(&pMsg->pData[1], data, length);

            // this is a normal host-to-controller event
            pMsg->hdr.event = HCI_HOST_TO_CTRL_CMD_EVENT;

            // so send it to the HCI handler
            (void)osal_msg_send(hciTaskID, (uint8 *)pMsg);
        } else {
            printf("malloc pMsg fail\n");
        }

#endif
    } else if (packet_type == HCI_ACL_DATA_PACKET) {
#if 1
        hciDataPacket_t *pMsg;
        //printf("acl recv %d:%s\n", length, bt_hex_real(data, length));
        // there's enough serial data to finish this packet; allocate memory
        pMsg = (hciDataPacket_t *)osal_msg_allocate(sizeof(hciDataPacket_t));

        if (pMsg) {
            uint16 param1;
            uint16 pktLen;
            pMsg->hdr.event  = HCI_HOST_TO_CTRL_DATA_EVENT;
            pMsg->hdr.status = 0xFF;

            // fill in message data
            pMsg->pktType    = packet_type;
            param1 = BUILD_UINT16(data[0], data[1]);
            pktLen = BUILD_UINT16(data[2], data[3]);
            pMsg->connHandle = param1 & 0x0FFF;         // mask out PB and BC flags
            pMsg->pbFlag     = (param1 & 0x3000) >> 12; // isolate PB flag
            pMsg->pktLen     = pktLen;
            pMsg->pData      = HCI_bm_alloc(pMsg->pktLen);

            // check if we have a BM buffer for payload
            if (pMsg->pData) {
                memcpy(pMsg->pData, &data[4], pMsg->pktLen);

                // send the message
                (void)osal_msg_send(hciTaskID, (uint8 *)pMsg);

            } else { // no memory available for payload
                // so give back memory allocated for message
                (void)osal_msg_deallocate((uint8 *)pMsg);
                printf("malloc pData fail\n");
            }
        } else {
            printf("malloc pMsg fail\n");
        }

#endif
    } else  if (packet_type == H5_VDRSPEC_PKT) {
        hciProcessHostToCtrlVendorCmd_RAW(data, length);
    }

    else {
        printf("unhandle packet type %d\n", packet_type);
    }

}

uint16 HCI_H5_Send(hci_data_type_t type, uint8 *buf, uint16 len)
{
    int i = 0;
    const h5_t *h5_ctx;

    h5_ctx = get_h5_controller_interface();
    h5_ctx->h5_send_acl_data(type, buf, len);

    return i;
}
#endif
/*******************************************************************************
 * @fn          HCI_UART_GetMaxTxBufSize
 *
 * @brief       This routine returns the max size transmit buffer.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Returns the max size of the transmit buffer.
 */
uint16 HCI_UART_GetMaxTxBufSize(void)
{
    return (MAX_UART_BUF_SIZE);
}

llStatus_t HCI_LL_SetScanControl(uint8 scanMode,
                                 uint8 filterReports)
{
    int ret;

    if (llState == LL_STATE_INIT) {
        return LL_STATUS_ERROR_COMMAND_DISALLOWED;
    }

    if (scanMode) {
        ret = wait_llWaitingIrq(100);

        if (ret) {
            return ret;
        }
    }

    ret = LL_SetScanControl(scanMode, filterReports);
    return ret;
}

llStatus_t HCI_LL_SetAdvControl(uint8 advMode)
{
    int ret;

    if (llState == LL_STATE_INIT) {
        return LL_STATUS_ERROR_COMMAND_DISALLOWED;
    }

    extern struct buf_rx_desc g_rx_adv_buf;
    g_rx_adv_buf.rxheader = 0;

    if (advMode) {
        int ret = wait_llWaitingIrq(100);

        if (ret) {
            return ret;
        }
    }

    ret = LL_SetAdvControl(advMode);

    if (llSecondaryState == LL_SEC_STATE_IDLE_PENDING) {
        llSecondaryState = LL_SEC_STATE_IDLE;
    } else {
        osal_stop_timerEx(LL_TaskID, LL_EVT_SECONDARY_ADV);
    }

    if (ret == 0) {
        extern struct buf_rx_desc g_rx_adv_buf;
        g_rx_adv_buf.rxheader = 0;
    }

    return ret;
}

#if 0

#define SCHD_LOGD(fmt, ...) //printf(fmt, ##__VA_ARGS__)
#define SCHD_LOGE(...) LOGE("ADV", ##__VA_ARGS__)

#define CONN_ADV_DATA_TIEMOUT   (10)
#define NOCONN_ADV_DATA_TIEMOUT (5)

typedef enum {
    SCHD_IDLE = 0,
    SCHD_ADV,
    SCHD_SCAN,
    SCHD_ADV_SCAN,

    SCHD_INVAILD,
} adv_scan_schd_state_en;

typedef enum {
    ADV_ON = 0,
    ADV_OFF,
    SCAN_ON,
    SCAN_OFF,

    ACTION_INVAILD,
} adv_scan_schd_action_en;

typedef int (*adv_scan_schd_func_t)(adv_scan_schd_state_en st);
static int adv_scan_schd_idle_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_idle_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_scan_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_scan_exit(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_scan_enter(adv_scan_schd_state_en st);
static int adv_scan_schd_adv_scan_exit(adv_scan_schd_state_en st);

struct {
    adv_scan_schd_func_t enter;
    adv_scan_schd_func_t exit;
} adv_scan_schd_funcs[] = {
    {adv_scan_schd_idle_enter, adv_scan_schd_idle_exit},
    {adv_scan_schd_adv_enter, adv_scan_schd_adv_exit},
    {adv_scan_schd_scan_enter, adv_scan_schd_scan_exit},
    {adv_scan_schd_adv_scan_enter, adv_scan_schd_adv_scan_exit},
};

adv_scan_schd_state_en adv_scan_schd_st_change_map[4][4] = {
    {SCHD_ADV, SCHD_IDLE, SCHD_SCAN, SCHD_IDLE},
    {SCHD_ADV, SCHD_IDLE, SCHD_ADV_SCAN, SCHD_ADV},
    {SCHD_ADV_SCAN, SCHD_SCAN, SCHD_SCAN, SCHD_IDLE},
    {SCHD_ADV_SCAN, SCHD_SCAN, SCHD_ADV_SCAN, SCHD_ADV},
};

#ifndef CONFIG_ADV_SCAN_INTERVAL_TIMER
#define CONFIG_ADV_SCAN_INTERVAL_TIMER (1)//ms
#endif

#ifndef CONFIG_ADV_INTERVAL_TIMER
#define CONFIG_ADV_INTERVAL_TIMER      (20) //ms
#endif

#include <ble_os.h>

struct adv_scan_data_t {
    uint8_t scan_on;
    uint8_t adv_on;
    uint8_t filterReports;
};

#define FLAG_RESTART 1
#define FLAG_STOP    2

struct {
    struct k_mutex mutex;
    k_timer_t timer;
    uint8_t flag;
    adv_scan_schd_state_en cur_st;
    struct adv_scan_data_t param;
} adv_scan_schd = {0};

static int adv_scan_schd_idle_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("idle enter\n");
    memset(&adv_scan_schd.param, 0, sizeof(struct adv_scan_data_t));
    return 0;
}

static int adv_scan_schd_idle_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("idle exit\n");
    // do nothing
    return 0;
}

static int adv_scan_schd_adv_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv on enter\n");

    if (st == SCHD_IDLE || st == SCHD_ADV_SCAN || st == SCHD_ADV) {
        if (adv_scan_schd.param.adv_on) {
            adv_scan_schd.flag = FLAG_RESTART;
            k_timer_start(&adv_scan_schd.timer, 1);
            return 0;
        }
    }

    return -EINVAL;;
}

static int adv_scan_schd_adv_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv on exit\n");
    int ret = 0;

    if (st == SCHD_ADV_SCAN || st == SCHD_IDLE || st == SCHD_ADV) {
        adv_scan_schd.flag = FLAG_STOP;
        k_timer_stop(&adv_scan_schd.timer);
        ret = HCI_LL_SetAdvControl(0);

        if (ret) {
            SCHD_LOGE("adv stop err %d\n", ret);
        }

        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_scan_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("scan on enter\n");
    int ret = 0;

    if (st == SCHD_IDLE || st == SCHD_ADV_SCAN || st == SCHD_SCAN) {
        ret = HCI_LL_SetScanControl(1, adv_scan_schd.param.filterReports);

        if (ret) {
            SCHD_LOGE("scan start err %d\n", ret);
            return ret;
        }

        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_scan_exit(adv_scan_schd_state_en st)
{
    SCHD_LOGD("scan on exit\n");
    int ret = 0;

    if (st == SCHD_ADV_SCAN || st == SCHD_IDLE || st == SCHD_SCAN) {
        ret = HCI_LL_SetScanControl(0, adv_scan_schd.param.filterReports);

        if (ret) {
            SCHD_LOGE("scan stop err %d\n", ret);
        }

        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_adv_scan_enter(adv_scan_schd_state_en st)
{
    SCHD_LOGD("adv scan on enter\n");

    if (st == SCHD_ADV || st == SCHD_SCAN || st == SCHD_ADV_SCAN || st == SCHD_IDLE) {
        adv_scan_schd.flag = FLAG_RESTART;
        k_timer_start(&adv_scan_schd.timer, 1);
        return 0;
    }

    return -EINVAL;
}

static int adv_scan_schd_adv_scan_exit(adv_scan_schd_state_en st)
{
    int ret;
    SCHD_LOGD("adv scan on exit\n");

    if (st == SCHD_ADV || st == SCHD_SCAN || st == SCHD_ADV_SCAN) {
        adv_scan_schd.flag = FLAG_STOP;
        k_timer_stop(&adv_scan_schd.timer);

        ret = HCI_LL_SetScanControl(0, adv_scan_schd.param.filterReports);

        if (ret) {
            SCHD_LOGE("scan stop err %d\n", ret);
        }

        ret = HCI_LL_SetAdvControl(0);

        if (ret) {
            SCHD_LOGE("adv stop err %d\n", ret);
        }

        return 0;
    }

    return -EINVAL;
}

int bt_mesh_adv_scan_schd(adv_scan_schd_state_en st)
{
    int ret;
    SCHD_LOGD("%d->%d\n", adv_scan_schd.cur_st, st);

    if (st < SCHD_INVAILD) {
        ret = adv_scan_schd_funcs[adv_scan_schd.cur_st].exit(st);

        if (ret) {
            return ret;
        }

        adv_scan_schd.cur_st = SCHD_IDLE;

        ret = adv_scan_schd_funcs[st].enter(adv_scan_schd.cur_st);

        if (ret) {
            return ret;
        }

        adv_scan_schd.cur_st = st;

        return 0;
    }

    return -EINVAL;
}

int bt_mesh_adv_scan_schd_action(adv_scan_schd_action_en action)
{
    int ret;

    if (action < ACTION_INVAILD) {
        adv_scan_schd_state_en cur_st = adv_scan_schd.cur_st;
        adv_scan_schd_state_en target_st = adv_scan_schd_st_change_map[cur_st][action];
        k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
        ret = bt_mesh_adv_scan_schd(target_st);
        k_mutex_unlock(&adv_scan_schd.mutex);

        if (ret && ret != -EALREADY) {
            SCHD_LOGE("action %d, cur_st %d target_st %d, ret %d\n", action, cur_st, target_st, ret);
        }

        return ret;
    }

    return -EINVAL;
}

void adv_scan_timer(void *timer, void *arg)
{
    int ret;
    static enum  {
        ADV = 0,
        SCAN,
        ADV_IDLE,
    } next_state = ADV;
    static int adv_time = 0;
    static int negative_flag = 0;
    uint8_t random_delay = 0;
    uint32_t next_time = 0;
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);

    if (adv_scan_schd.flag == FLAG_RESTART) {
        next_state = ADV;
        adv_scan_schd.flag = 0;
    } else if (adv_scan_schd.flag == FLAG_STOP) {
        k_mutex_unlock(&adv_scan_schd.mutex);
        return;
    }

    SCHD_LOGD("adv_time %d, next_state %d, flag %d\n", adv_time, next_state, adv_scan_schd.flag);

    if (next_state == ADV) {
        ret = HCI_LL_SetScanControl(0, adv_scan_schd.param.filterReports);

        if (ret) {
            SCHD_LOGE("scan stop err %d\n", ret);
        }

        ret = HCI_LL_SetAdvControl(1);

        if (ret) {
            SCHD_LOGE("adv start err %d\n", ret);
        }

        if (adv_scan_schd.cur_st == SCHD_ADV_SCAN) {
            next_state = SCAN;
        } else if (adv_scan_schd.cur_st == SCHD_ADV) {
            next_state = ADV_IDLE;
        }

        adv_time = NOCONN_ADV_DATA_TIEMOUT;
        next_time = adv_time;
    } else if (next_state == SCAN) {
        ret = HCI_LL_SetAdvControl(0);

        if (ret) {
            SCHD_LOGE("adv stop err %d\n", ret);
        }

        bt_rand(&random_delay, 1);

        random_delay = random_delay % 10;

        if (random_delay <= 5) {
            random_delay = 5;
        }

        if (negative_flag) {
            next_time = CONFIG_ADV_INTERVAL_TIMER - NOCONN_ADV_DATA_TIEMOUT - random_delay;
        } else {
            next_time = CONFIG_ADV_INTERVAL_TIMER - NOCONN_ADV_DATA_TIEMOUT + random_delay;
        }

        negative_flag = !negative_flag;

        if (next_time > CONFIG_ADV_SCAN_INTERVAL_TIMER) {
            ret = HCI_LL_SetScanControl(1, adv_scan_schd.param.filterReports);

            if (ret) {
                SCHD_LOGE("%d,%d,%d,%d\n", ret, llState, llSecondaryState, scanInfo.scanMode);
            }
        }

        adv_time = 0;
        next_state = ADV;
    } else if (next_state == ADV_IDLE) {

        next_time = CONFIG_ADV_INTERVAL_TIMER - adv_time;

        ret = HCI_LL_SetAdvControl(0);

        if (ret) {
            SCHD_LOGE("adv stop err %d\n", ret);
        }

        adv_time = 0;
        next_state = ADV;
    }

    k_mutex_unlock(&adv_scan_schd.mutex);
    k_timer_start(&adv_scan_schd.timer, next_time);
}

int bt_mesh_adv_scan_schd_init()
{
    memset(&adv_scan_schd, 0, sizeof(adv_scan_schd));
    k_timer_init(&adv_scan_schd.timer, adv_scan_timer,  &adv_scan_schd);
    k_mutex_init(&adv_scan_schd.mutex);
    return 0;
}

int bt_mesh_adv_enable()
{

    SCHD_LOGD("bt_mesh_scan_disable\n");
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.adv_on = 1;
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(ADV_ON);
    return 0;
}

int bt_mesh_adv_disable()
{
    SCHD_LOGD("bt_mesh_scan_disable\n");
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.adv_on = 0;
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(ADV_OFF);
    return 0;
}

int bt_mesh_scan_enable(uint8 filterReports)
{


    SCHD_LOGD("bt_mesh_scan_disable\n");
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.scan_on = 1;
    adv_scan_schd.param.filterReports = filterReports;
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(SCAN_ON);
    return 0;
}

int bt_mesh_scan_disable(void)
{
    SCHD_LOGD("bt_mesh_scan_disable\n");
    k_mutex_lock(&adv_scan_schd.mutex, K_FOREVER);
    adv_scan_schd.param.scan_on = 0;
    k_mutex_unlock(&adv_scan_schd.mutex);
    bt_mesh_adv_scan_schd_action(SCAN_OFF);
    return 0;
}
#endif

#endif