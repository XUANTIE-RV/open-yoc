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
#include <aos/ble.h>
#include <devices/hci.h>
#include <k_api.h>
#include <devices/hal/hci_impl.h>
#include "rom_sym_def.h"

#include <devices/device.h>
#include <devices/hci.h>
#include <hci_api.h>
#include <drv/irq.h>

#include <jump_function.h>
#include <global_config.h>
#include <clock.h>
#include <comdef.h>
//#include <OSAL_Tasks.h>
//#include <OSAL_Memory.h>
//#include <osal_bufmgr.h>
#include <gpio.h>
#include <pwrmgr.h>
#include <ll.h>
#include "rf_phy_driver.h"
#include <ll_sleep.h>
//#include <OSAL.h>

extern osal_msg_q_t osal_qHead;
extern uint16 HCI_ProcessEvent(uint8 task_id, uint16 events);
extern uint16 LL_ProcessEvent(uint8 task_id, uint16 events);
uint16 LL_ProcessEvent1( uint8 task_id, uint16 events );
extern uint32_t  __initial_sp;
extern uint32_t  g_smartWindowSize;

extern void TIM1_IRQHandler2(void);
extern void LL_IRQHandler(void);
extern void LL_IRQHandler1(void);
extern void LL_IRQHandler2(void);
extern uint16 HCI_ProcessEvent1(uint8 task_id, uint16 events);
extern void rf_phy_change_cfg0(uint8_t pktFmt);
extern uint8 osal_set_event1(uint8 task_id, uint16 event_flag);
extern void TIM1_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void wakeup_init1();
extern void ll_hw_go1(void);
extern void config_RTC1(uint32 time);
extern uint8 ll_hw_get_tr_mode(void);
extern void gpio_write(gpio_pin_e pin, uint8_t en);
extern void WaitRTCCount(uint32_t rtcDelyCnt);
extern llStatus_t LL_SetAdvParam0(uint16 advIntervalMin,
                                  uint16 advIntervalMax,
                                  uint8  advEvtType,
                                  uint8  ownAddrType,
                                  uint8  directAddrType,
                                  uint8  *directAddr,
                                  uint8  advChanMap,
                                  uint8  advWlPolicy);
extern int ll_hw_get_rfifo_depth(void);
extern uint32_t read_current_fine_time(void);
extern void move_to_master_function(void);
extern void move_to_slave_function0(void);
extern uint8 LL_ENC_GenerateTrueRandNum1(uint8 *buf,
        uint8 len);
extern void osal_pwrmgr_powerconserve1();
extern llStatus_t LL_TxData1(uint16 connId,
                             uint8 *pBuf,
                             uint8  pktLen,
                             uint8  fragFlag);
extern void rf_calibrate1(void);
extern llStatus_t LL_SetAdvParam1(uint16 advIntervalMin,
                                  uint16 advIntervalMax,
                                  uint8  advEvtType,
                                  uint8  ownAddrType,
                                  uint8  peerAddrType,
                                  uint8  *peerAddr,
                                  uint8  advChanMap,
                                  uint8  advWlPolicy);
extern void llMasterEvt_TaskEndOk1(void);

extern void HCI_Init(uint8 task_id);
extern size_t __heap_start;
extern size_t __heap_end;
extern void ll_patch_init();
extern void *HCI_bm_alloc(uint16 size);
extern void  ble_main(void);
extern void llConnTerminate1(llConnState_t *connPtr,
                             uint8          reason);
extern uint8 llSecAdvAllow1(void);
extern uint32 llCalcMaxScanTime1(void);
extern void move_to_slave_function2();
extern void __TIM1_IRQHandler(void);
extern void config_RTC1(uint32 time);
extern void ll_scheduler1(uint32 time);
extern uint8 _symrom_osal_msg_send0(uint8 destination_task, uint8 *msg_ptr);
extern void TIM4_IRQHandler1(void);
extern void ll_adptive_adj_next_time1(uint32_t next_time);
extern uint8 isTimer1Running(void);
extern uint32  read_LL_remainder_time(void);
extern void hciProcessHostToCtrlCmd_RAW(uint8_t *Data, uint32_t len);
uint8 ll_processBasicIRQ_SRX(uint32_t      irq_status);
uint8 ll_processBasicIRQ_SRX2(uint32_t      irq_status);
extern void osalTimeUpdate(void);
extern volatile uint32_t llWaitingIrq;
extern uint8_t hciTaskID;
extern advInfo_t           adv_param;
extern uint8 ll_processBasicIRQ_secondaryAdvTRX0(uint32_t              irq_status );
extern uint8 ll_processBasicIRQ_secondaryInitSRX0(uint32_t              irq_status);
extern uint8 ll_processBasicIRQ_secondaryScanSRX0(uint32_t              irq_status);
extern void llSetupExtScan1(uint8 chan);
#ifndef CONFIG_LARGE_HEAP_SIZE
#define CONFIG_LARGE_HEAP_SIZE (4 * 1024)
#endif
extern uint8_t hciCtrlCmdToken;
int         hclk,  pclk;
#define     LARGE_HEAP_SIZE  CONFIG_LARGE_HEAP_SIZE
uint8       largeHeap[LARGE_HEAP_SIZE] = {0};
uint32 *pGlobal_config = (uint32 *)(CONFIG_BASE_ADDR);

static hci_event_cb_t hci_cb = NULL;
static void *hci_arg = NULL;

#define LL_HW_MODE_STX           0x00
#define LL_HW_MODE_SRX           0x01
#define LL_HW_MODE_TRX           0x02
#define LL_HW_MODE_RTX           0x03
#define LL_HW_MODE_TRLP          0x04
#define LL_HW_MODE_RTLP          0x05

typedef struct {
    osal_event_hdr_t hdr;
    uint8            *pData;
} hciPacket_t;

#define RAM_CODE_SECTION(func)  __attribute__((section(".__sram.code."#func)))  func
uint8 RAM_CODE_SECTION(osal_msg_send1)(uint8 destination_task, uint8 *msg_ptr);
static inline void RAM_CODE_SECTION(rx_signal_trigger)();
void RAM_CODE_SECTION(rx_signal_handler)();
void RAM_CODE_SECTION(move_to_slave_function3)(void);
void RAM_CODE_SECTION(LL_IRQHandler_process)(void);
void RAM_CODE_SECTION(__LL_IRQHandler)(void);

// ===================== connection context relate definition

#define   BLE_MAX_ALLOW_CONNECTION              (CONFIG_BT_MAX_CONN)

#ifndef   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX
#define   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX        2
#endif
#ifndef   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_RX
#define   CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_RX        2
#endif

#define   BLE_MAX_ALLOW_PKT_PER_EVENT_TX  CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_TX
#define   BLE_MAX_ALLOW_PKT_PER_EVENT_RX  CONFIG_BLE_MAX_ALLOW_PKT_PER_EVENT_RX

#define   BLE_PKT_VERSION                   ((CONFIG_BT_L2CAP_RX_MTU > 23 || CONFIG_BT_L2CAP_TX_MTU > 23) ?  BLE_PKT_VERSION_5_1 :  BLE_PKT_VERSION_4_0) //BLE_PKT_VERSION_5_1

#if 0
#define   BLE_PKT_BUF_SIZE                  (((BLE_PKT_VERSION == BLE_PKT_VERSION_5_1) ? 1 : 0) *  BLE_PKT51_LEN \
        + ((BLE_PKT_VERSION == BLE_PKT_VERSION_4_0) ? 1 : 0) * BLE_PKT40_LEN \
        + (sizeof(struct ll_pkt_desc) - 2))
#endif

#define BLE_PKT_LEN ((CONFIG_BT_L2CAP_RX_MTU + 4) < 37 ? 37 :  (CONFIG_BT_L2CAP_RX_MTU + 4))

#define   BLE_PKT_BUF_SIZE (((BLE_PKT_LEN + (sizeof(struct ll_pkt_desc) - 2)) + 3) / 4 * 4)

/* BLE_MAX_ALLOW_PER_CONNECTION
{
    ...
    struct ll_pkt_desc *tx_conn_desc[MAX_LL_BUF_LEN];     // new Tx data buffer
    struct ll_pkt_desc *rx_conn_desc[MAX_LL_BUF_LEN];

    struct ll_pkt_desc *tx_not_ack_pkt;
    struct ll_pkt_desc *tx_ntrm_pkts[MAX_LL_BUF_LEN];
    ...
 }
 tx_conn_desc[] + tx_ntrm_pkts[]    --> BLE_MAX_ALLOW_PKT_PER_EVENT_TX * BLE_PKT_BUF_SIZE*2
 rx_conn_desc[]             --> BLE_MAX_ALLOW_PKT_PER_EVENT_RX * BLE_PKT_BUF_SIZE
 tx_not_ack_pkt             --> 1*BLE_PKT_BUF_SIZE

*/
#define   BLE_MAX_ALLOW_PER_CONNECTION          ( (BLE_MAX_ALLOW_PKT_PER_EVENT_TX * BLE_PKT_BUF_SIZE * 2) \
        +(BLE_MAX_ALLOW_PKT_PER_EVENT_RX * BLE_PKT_BUF_SIZE)   \
        + BLE_PKT_BUF_SIZE )

#define   BLE_CONN_BUF_SIZE                 (BLE_MAX_ALLOW_CONNECTION * BLE_MAX_ALLOW_PER_CONNECTION)


uint8           g_pConnectionBuffer[BLE_CONN_BUF_SIZE] __attribute__((aligned(4))) = {0};
llConnState_t   pConnContext[BLE_MAX_ALLOW_CONNECTION] = {0};
volatile uint8 g_clk32K_config = 0;

int hci_api_le_read_buffer_size_complete(uint16_t *le_max_len, uint8_t *le_max_num)
{
    *le_max_len = 27;
    *le_max_num = BLE_MAX_ALLOW_PKT_PER_EVENT_TX;

    return 0;
}

//#define BLE_SUPPORT_CTE_IQ_SAMPLE TRUE
#ifdef BLE_SUPPORT_CTE_IQ_SAMPLE
uint16 g_llCteSampleI[LL_CTE_MAX_SUPP_LEN * LL_CTE_SUPP_LEN_UNIT];
uint16 g_llCteSampleQ[LL_CTE_MAX_SUPP_LEN * LL_CTE_SUPP_LEN_UNIT];
#endif

extern volatile sysclk_t       g_system_clk;

typedef struct {
    aos_dev_t           device;
} hci_dev_t;

static hci_dev_t *hci_dev = NULL;

pTaskEventHandlerFn tasksArr[] = {
    LL_ProcessEvent,                                                  // task 0
    HCI_ProcessEvent,                                                 // task 1
};

uint8 tasksCnt = sizeof(tasksArr) / sizeof(tasksArr[0]);
uint16 *tasksEvents;

uint8 adv_report_count = 0;

#define ADV_REPORT_LIMIT (4)


#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV

#define TEST_MAX         1
#define    MAX_EXT_ADV_INSTANCE     1
#ifdef CONFG_BT_MAX_EXT_ADV_DATA_LEN
#define    MAX_EXT_ADV_DATA_LENGTH (CONFG_BT_MAX_EXT_ADV_DATA_LEN)
#else
#define    MAX_EXT_ADV_DATA_LENGTH  250
#endif
// extended advertiser variables
extAdvInfo_t extAdvInfo[MAX_EXT_ADV_INSTANCE] = {0};
uint8     advertisingData[MAX_EXT_ADV_INSTANCE][MAX_EXT_ADV_DATA_LENGTH] = {0};
uint8     extScanRspData[MAX_EXT_ADV_INSTANCE][MAX_EXT_ADV_DATA_LENGTH] = {0};
#endif
#if defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV
#define    MAX_PERIODIC_EXT_ADV_INSTANCE     1
#define    MAX_PERIODIC_EXT_ADV_DATA_LENGTH  250
uint8     periodicadvertisingData[MAX_PERIODIC_EXT_ADV_INSTANCE][MAX_PERIODIC_EXT_ADV_DATA_LENGTH] = {0};
periodicAdvInfo_t periodicAdvInfo[MAX_PERIODIC_EXT_ADV_INSTANCE] = {0};

#endif

uint8 osal_msg_send1(uint8 destination_task, uint8 *msg_ptr)
{
    if (hciTaskID == destination_task) {
        hciPacket_t *hci_data = (hciPacket_t *)msg_ptr;

        if (hci_data->pData[0] == 0x04 && hci_data->pData[1] == 0x3e && (hci_data->pData[3] == 0x02 || hci_data->pData[3] == 0x0d)) {
            if (adv_report_count > ADV_REPORT_LIMIT) {
                (void)osal_msg_deallocate((uint8 *)msg_ptr);
                return 0;
            }

            adv_report_count++;
        }

    }

    return _symrom_osal_msg_send0(destination_task, msg_ptr);
}

static inline void rx_signal_trigger()
{
    csi_vic_set_pending_irq(RX_SIGNAL_IRQn);
}

uint8 osal_set_event1(uint8 task_id, uint16 event_flag)
{
    uint16 *events;
    uint8 task_cnt = *(uint8 *)JUMP_FUNCTION(TASK_COUNT);

    if (task_id < task_cnt) { //tasksCnt )
        CPSR_ALLOC();
        RHINO_CPU_INTRPT_DISABLE();    // Hold off interrupts
        events = *(uint16 **)(JUMP_FUNCTION(TASK_EVENTS));

        if (events != NULL) {
            events[task_id] |= event_flag;
        }

        RHINO_CPU_INTRPT_ENABLE();    // Release interrupts

        extern uint8_t g_intrpt_nested_level[1];

        if (g_intrpt_nested_level[0] == 0 && event_flag == LL_EVT_SECONDARY_ADV) {
            osal_run_system();
        }

        rx_signal_trigger();

        return (SUCCESS);
    } else {
        return (INVALID_TASK);
    }
}

void rx_signal_handler()
{
    if (hci_cb) {
        hci_cb(HCI_EVENT_READ, 0, hci_arg);
    }
}

// Minimum length for EVENT packet is 1+1+1
// | Packet Type (1) | Event Code(1) | Length(1) |
//
#define HCI_EVENT_MIN_LENGTH           3

//
// Minimum length for DATA packet is 1+2+2
// | Packet Type (1) | Handler(2) | Length(2) |
//
#define HCI_DATA_MIN_LENGTH            5

// OSAL Message Header Events
#define HCI_CTRL_TO_HOST_EVENT         0x01
#define HCI_HOST_TO_CTRL_CMD_EVENT     0x02
#define HCI_HOST_TO_CTRL_DATA_EVENT    0x03

// HCI Packet Types
#define HCI_CMD_PACKET                 0x01
#define HCI_ACL_DATA_PACKET            0x02
#define HCI_SCO_DATA_PACKET            0x03
#define HCI_EVENT_PACKET               0x04

/*******************************************************************************
 * This is the HCI OSAL task process event handler.
 *
 * Public function defined in hci.h.
 */
uint16 HCI_ProcessEvent1(uint8 task_id, uint16 events)
{
    if (events & SYS_EVENT_MSG) {
        return (events ^ SYS_EVENT_MSG);
    }

    return 0;
}

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks(void)
{
    uint8 taskID = 0;

    tasksEvents = (uint16 *)osal_mem_alloc(sizeof(uint16) * tasksCnt);
    osal_memset(tasksEvents, 0, (sizeof(uint16) * tasksCnt));

    LL_Init(taskID++);

    /* HCI Task */
    HCI_Init(taskID++);
}

__attribute__((weak)) void LL_AdvSetTerminatedCback1(uint8          status,
        uint8   adv_handle,
        uint16  connHandle,
        uint8   Num_Completed_Extended_Advertising_Events)
{

}

__attribute__((weak)) void LL_ExtAdvReportCback1(uint16 advEvt,
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

}

__attribute__((weak))void LL_PrdAdvSyncEstablishedCback1(uint8        status,
        uint16  syncHandle,
        uint8   advertisingSID,
        uint8   advertiserAddressType,
        uint8   *advertiserAddress,
        uint8   advertiserPHY,
        uint16  periodicAdvertisingInterval,
        uint8   advertiserClockAccuracy
                                                        )
{

}

__attribute__((weak))void LL_PrdAdvReportCback1(uint16 syncHandle,
        uint8 txPower,
        uint8 rssi,
        uint8 cteType,
        uint8 dataStatus,
        uint8 dataLength,
        uint8 *data
                                               )
{

}

void move_to_slave_function3(void)
{
    uint8_t       *pBuf = g_rx_adv_buf.data;

    uint8_t  winSize;
    uint16_t winOffset;
    uint16_t connInterval;
    uint16_t slaveLatency;
    uint16_t connTimeout;

    pBuf += 19;      // skip initA and AdvA and accessAddr and initCRC
    pBuf = llMemCopySrc((uint8 *)&winSize,       pBuf, 1);
    pBuf = llMemCopySrc((uint8 *)&winOffset,     pBuf, 2);
    pBuf = llMemCopySrc((uint8 *)&connInterval,  pBuf, 2);
    pBuf = llMemCopySrc((uint8 *)&slaveLatency,  pBuf, 2);
    pBuf = llMemCopySrc((uint8 *)&connTimeout,   pBuf, 2);

    // TI style: convert to 625us tick
    winSize      <<= 1;
    winOffset    <<= 1;
    connInterval <<= 1;
    connTimeout  <<= 4;

    if (((connTimeout <= ((slaveLatency) * connInterval << 1)))
        || (connInterval == 0)) {
        return;
    }

    move_to_slave_function0();
}

void LL_IRQHandler_process(void)
{
    uint32         irq_status;
    int8 ret;

    ISR_entry_time = read_current_fine_time();

    osalTimeUpdate();

    irq_status = ll_hw_get_irq_status();

    if (!(irq_status & LIRQ_MD)) {        // only process IRQ of MODE DONE
        ll_hw_clr_irq();                  // clear irq status
        return;
    }

    llWaitingIrq = FALSE;

#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
    if (llTaskState == LL_TASK_EXTENDED_ADV) {
        ret = ll_processExtAdvIRQ(irq_status);
        // TODO: consider whether need process secondary adv/scan here
        if (ret == TRUE) {
            return;
        }
    }
    else if (llTaskState == LL_TASK_EXTENDED_SCAN) {

        ret = ll_processExtScanIRQ(irq_status);
        // TODO: consider whether need process secondary adv/scan here
        if (ret == TRUE) {
            return;
        }
    }
#if (defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL)
    else if (llTaskState == LL_TASK_EXTENDED_INIT) {

        ret = ll_processExtInitIRQ(irq_status);

        // TODO: consider whether need process secondary adv/scan here
        if (ret == TRUE) {
            return;
        }
    }
#endif
#if defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV
    else if (llTaskState == LL_TASK_PERIODIC_ADV) {

        ret = ll_processPrdAdvIRQ(irq_status);

        // TODO: consider whether need process secondary adv/scan here
        if (ret == TRUE) {
            return;
        }
    } else if (llTaskState == LL_TASK_PERIODIC_SCAN) {
        ret = ll_processPrdScanIRQ(irq_status);

        // TODO: consider whether need process secondary adv/scan here
        if (ret == TRUE) {
            return;
        }
    }
#endif
    else

#endif
    {
        uint8         mode;
        mode = ll_hw_get_tr_mode();
#if (defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL)
        if (mode == LL_HW_MODE_SRX && (llState == LL_STATE_SCAN || llState == LL_STATE_INIT)) {
            ret = ll_processBasicIRQ_SRX(irq_status);
        } else if ((llSecondaryState == LL_SEC_STATE_ADV || llSecondaryState ==LL_SEC_STATE_IDLE_PENDING)
            && (mode == LL_HW_MODE_TRX)
            && (adv_param.advEvtType == LL_ADV_CONNECTABLE_UNDIRECTED_EVT || adv_param.advEvtType == LL_ADV_SCANNABLE_UNDIRECTED_EVT))
        {
            ret = ll_processBasicIRQ_secondaryAdvTRX0(irq_status);
        }
        else if (llSecondaryState == LL_SEC_STATE_SCAN && mode == LL_HW_MODE_SRX)
        {
            ret = ll_processBasicIRQ_secondaryScanSRX0(irq_status);
        }
#if defined(CONFIG_BT_MAX_CONN) && CONFIG_BT_MAX_CONN > 1
        else if (mode == LL_HW_MODE_SRX && llSecondaryState == LL_SEC_STATE_INIT)
        {
            ret = ll_processBasicIRQ_secondaryInitSRX0(irq_status);
        }
#endif
        else
        {
            ret = ll_processBasicIRQ(irq_status);
        }

#elif defined(CONFIG_BT_OBSERVER)

        if (mode == LL_HW_MODE_SRX && (llState == LL_STATE_SCAN)) {
            ret = ll_processBasicIRQ_SRX2(irq_status);
        } else if ((llSecondaryState == LL_SEC_STATE_ADV || llSecondaryState ==LL_SEC_STATE_IDLE_PENDING)
            && (mode == LL_HW_MODE_TRX)
            && (adv_param.advEvtType == LL_ADV_CONNECTABLE_UNDIRECTED_EVT || adv_param.advEvtType == LL_ADV_SCANNABLE_UNDIRECTED_EVT))
        {
            ret = ll_processBasicIRQ_secondaryAdvTRX0(irq_status);
        }
        else if (llSecondaryState == LL_SEC_STATE_SCAN && mode == LL_HW_MODE_SRX)
        {
            ret = ll_processBasicIRQ_secondaryScanSRX0(irq_status);
        }
#if defined(CONFIG_BT_MAX_CONN) && CONFIG_BT_MAX_CONN > 1
        else if (mode == LL_HW_MODE_SRX && llSecondaryState == LL_SEC_STATE_INIT)
        {
            ret = ll_processBasicIRQ_secondaryInitSRX0(irq_status);
        }
#endif
        else {
            ret = ll_processBasicIRQ(irq_status);
        }

#else
        if ((llSecondaryState == LL_SEC_STATE_ADV || llSecondaryState ==LL_SEC_STATE_IDLE_PENDING)
            && (mode == LL_HW_MODE_TRX)
            && (adv_param.advEvtType == LL_ADV_CONNECTABLE_UNDIRECTED_EVT || adv_param.advEvtType == LL_ADV_SCANNABLE_UNDIRECTED_EVT))
        {
            ret = ll_processBasicIRQ_secondaryAdvTRX0(irq_status);
        }
        else if (llSecondaryState == LL_SEC_STATE_SCAN && mode == LL_HW_MODE_SRX)
        {
            ret = ll_processBasicIRQ_secondaryScanSRX0(irq_status);
        }
#if defined(CONFIG_BT_MAX_CONN) && CONFIG_BT_MAX_CONN > 1
        else if (mode == LL_HW_MODE_SRX && llSecondaryState == LL_SEC_STATE_INIT)
        {
            ret = ll_processBasicIRQ_secondaryInitSRX0(irq_status);
        }
#endif
        ret = ll_processBasicIRQ(irq_status);
#endif
    }
#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
    if (llTaskState == LL_TASK_EXTENDED_SCAN_PENDING)
    {
        llSetupExtScan1(extScanInfo.current_chn);
    }
#endif

    // ================ Post ISR process: secondary pending state process
    // conn-adv case 2: other ISR, there is pending secondary advertise event, make it happen
    if (llSecondaryState == LL_SEC_STATE_ADV_PENDING) {
        if (llSecAdvAllow()) {  // for multi-connection case, it is possible still no enough time for adv
            llSetupSecAdvEvt();

            llSecondaryState = LL_SEC_STATE_ADV;
        }
    }
    // there is pending scan event, make it happen, note that it may stay pending if there is no enough idle time
    else if (llSecondaryState == LL_SEC_STATE_SCAN_PENDING) {
        // trigger scan
        llSetupSecScan(scanInfo.nextScanChan);
    }
#if (defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL)
    // there is pending init event, make it happen, note that it may stay pending if there is no enough idle time
    else if (llSecondaryState == LL_SEC_STATE_INIT_PENDING) {
        // trigger scan
        llSetupSecInit(initInfo.nextScanChan);
    }
#endif
    (void)ret;
}

void __LL_IRQHandler(void)
{
    extern uint8_t g_intrpt_nested_level[1];
    g_intrpt_nested_level[0]++;
    LL_IRQHandler_process();
    g_intrpt_nested_level[0]--;
}


void patch_efuse_init(void)
{
    write_reg(0x4000f054, 0x0);
    write_reg(0x4000f140, 0x0);
    write_reg(0x4000f144, 0x0);
}

void hal_rfphy_init(void)
{
    ll_patch_init();

    //============config the txPower
    g_rfPhyTxPower  = RF_PHY_TX_POWER_0DBM;
    //============config BLE_PHY TYPE
    g_rfPhyPktFmt   = PKT_FMT_BLE1M;
    //============config RF Frequency Offset
    g_rfPhyFreqOffSet   = RF_PHY_FREQ_FOFF_00KHZ;
    //============config xtal 16M cap
    /* Patch for wakeup issue_20210531, use 9 to avoid wakeup problem */
    uint32_t xtalcap = RF_PHY_DEFAULT_XTAL_CAP;//default is 0x9;
#ifdef CONFIG_CALIBRATE_WITH_FREQOFF
    extern int dut_hal_freqoff_get(int32_t *freqoff);
    int32_t freqoff = RF_PHY_FREQ_FOFF_00KHZ;
    int ret = dut_hal_freqoff_get(&freqoff);

    if (ret == 0 && freqoff != 0) {
        g_rfPhyFreqOffSet = freqoff >> 2; // x/4
    };

#else
    extern int dut_hal_xtalcap_get(uint32_t *xtalcap);

    int ret = dut_hal_xtalcap_get(&xtalcap);

    if (ret != 0) {
        xtalcap = RF_PHY_DEFAULT_XTAL_CAP;
    }

#endif

    XTAL16M_CAP_SETTING(xtalcap);
    XTAL16M_CURRENT_SETTING(0x1);

    patch_efuse_init();
    ble_main();
    NVIC_DisableIRQ(TIM2_IRQn);

    NVIC_SetPriority((IRQn_Type)BB_IRQn,    IRQ_PRIO_REALTIME);
    NVIC_SetPriority((IRQn_Type)TIM1_IRQn,  IRQ_PRIO_HIGH);     //ll_EVT
    // NVIC_SetPriority((IRQn_Type)TIM2_IRQn,  IRQ_PRIO_HIGH);     //OSAL_TICK
    NVIC_SetPriority((IRQn_Type)TIM4_IRQn,  IRQ_PRIO_HIGH);     //LL_EXA_ADV


    drv_irq_register(BB_IRQn,   __LL_IRQHandler);
    drv_irq_register(TIM1_IRQn, __TIM1_IRQHandler);
    // drv_irq_register(TIM2_IRQn, __TIM2_IRQHandler);
    drv_irq_register(TIM3_IRQn, TIM3_IRQHandler);

#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
    drv_irq_register(TIM4_IRQn, TIM4_IRQHandler1);
#else
    drv_irq_register(TIM4_IRQn, TIM4_IRQHandler);
#endif

    csi_vic_set_prio(BB_IRQn, 0);
    csi_vic_set_prio(TIM1_IRQn, 0);

    drv_irq_register(RX_SIGNAL_IRQn, rx_signal_handler);
    drv_irq_enable(RX_SIGNAL_IRQn);
    csi_vic_set_prio(RX_SIGNAL_IRQn, 2);

    // csi_vic_set_prio(TIM2_IRQn, 0);
}

/*******************************************************************************
 * @fn          LL_InitConnectContext
 *
 * @brief       This function initialize the LL connection-orient context
 *
 * input parameters
 *
 * @param       pConnContext   - connection-orient context, the memory is allocated by application
 *              maxConnNum     - the size of connect-orient context
 *              maxPktPerEvent - number of packets transmit/receive per connection event
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern uint8               g_maxConnNum;
extern uint8               g_maxPktPerEventTx;
extern uint8               g_maxPktPerEventRx;
extern uint8               g_blePktVersion;
llStatus_t LL_InitConnectContext0(llConnState_t    *pConnContext,
                                  uint8 *pConnBuffer,
                                  uint8 maxConnNum,
                                  uint8 maxPktPerEventTx,
                                  uint8 maxPktPerEventRx,
                                  uint8 blePktVersion)
{
    int i, j;
    uint8 *p;
    int pktLen = 0;
    int total = 0;

    if (maxConnNum > MAX_NUM_LL_CONN) {
        return LL_STATUS_ERROR_BAD_PARAMETER;
    }

    if (pConnContext == NULL) {
        return LL_STATUS_ERROR_BAD_PARAMETER;
    }

    if (pConnBuffer == NULL) {
        return LL_STATUS_ERROR_BAD_PARAMETER;
    }

    //if (blePktVersion == BLE_PKT_VERSION_4_0) {       // BLE4.0
    //    pktLen = BLE_PKT40_LEN;
    //} else if (blePktVersion == BLE_PKT_VERSION_5_1) {     // BLE5.1
    //    pktLen = BLE_PKT51_LEN;
    //}
    pktLen = BLE_PKT_LEN;

    pktLen += 6;             // header

    g_maxConnNum = maxConnNum;
    conn_param   = pConnContext;
    g_maxPktPerEventTx = maxPktPerEventTx;
    g_maxPktPerEventRx = maxPktPerEventRx;
    g_blePktVersion = blePktVersion;

    p  = pConnBuffer;

    for (i = 0; i < maxConnNum; i++) {
        memset(&conn_param[i], 0, sizeof(llConnState_t));

        for (j = 0; j < maxPktPerEventTx; j++) {
            conn_param[i].ll_buf.tx_conn_desc[j] = (struct ll_pkt_desc *)p;
            p += pktLen;
            total += pktLen;
        }

        for (j = 0; j < maxPktPerEventRx; j++) {
            conn_param[i].ll_buf.rx_conn_desc[j] = (struct ll_pkt_desc *)p;
            p += pktLen;
            total += pktLen;
        }

        conn_param[i].ll_buf.tx_not_ack_pkt = (struct ll_pkt_desc *)p;
        p += pktLen;
        total += pktLen;

        for (j = 0; j < maxPktPerEventTx; j++) {
            conn_param[i].ll_buf.tx_ntrm_pkts[j] = (struct ll_pkt_desc *)p;
            p += pktLen;
            total += pktLen;
        }

    }

//  LOG("total = %d\n", total);

    return (LL_STATUS_SUCCESS);
}


void init_config(void)
{
    int i;

    for (i = 0; i < 256; i ++) {
        pGlobal_config[i] = 0;
    }

    //save the app initial_sp  which will be used in wakeupProcess 20180706 by ZQ
    //  pGlobal_config[INITIAL_STACK_PTR] = (uint32_t)&__initial_sp;

    // LL switch setting
    pGlobal_config[LL_SWITCH] =  LL_DEBUG_ALLOW | SLAVE_LATENCY_ALLOW | LL_WHITELIST_ALLOW
                                 | SIMUL_CONN_ADV_ALLOW | SIMUL_CONN_SCAN_ALLOW; //RC32_TRACKINK_ALLOW

    if (g_clk32K_config == CLK_32K_XTAL) {
        pGlobal_config[LL_SWITCH] &= 0xffffffee;
    } else {
        pGlobal_config[LL_SWITCH] |= RC32_TRACKINK_ALLOW | LL_RC32K_SEL;
    }

    // sleep delay
#ifdef CONFIG_USE_XTAL_CLK
    pGlobal_config[MIN_TIME_TO_STABLE_32KHZ_XOSC] = 10;      // 10ms, temporary set
#else
    //for 32K RC, no stable time need
    pGlobal_config[MIN_TIME_TO_STABLE_32KHZ_XOSC] = 0;
#endif

    // system clock setting
    pGlobal_config[CLOCK_SETTING] = g_system_clk;//CLOCK_32MHZ;

    //------------------------------------------------------------------------
    // wakeup time cose
    // t1. HW_Wakeup->MCU relase 62.5us
    // t2. wakeup_process in waitRTCCounter 30.5us*[WAKEUP_DELAY] about 500us
    // t3. dll_en -> hclk_sel in hal_system_ini 100us in run as RC32M
    // t4. sw prepare cal sleep tick initial rf_ini about 300us @16M this part depends on HCLK
    // WAKEUP_ADVANCE should be larger than t1+t2+t3+t4
    //------------------------------------------------------------------------
    // wakeup advance time, in us

    pGlobal_config[WAKEUP_ADVANCE] = 1850;//650;//600;//310;

    if (g_system_clk == SYS_CLK_XTAL_16M) {
        pGlobal_config[WAKEUP_DELAY] = 16;
    } else if (g_system_clk == SYS_CLK_DLL_48M) {
        pGlobal_config[WAKEUP_DELAY] = 20;
    }

    // sleep time, in us
    pGlobal_config[MAX_SLEEP_TIME] = MAX_32KRC_CALIBRATION_TIME;//1500000;
    pGlobal_config[MIN_SLEEP_TIME] = 1500;

    pGlobal_config[ALLOW_TO_SLEEP_TICK_RC32K] = 60;// 30.5 per tick

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // LL engine settle time
    pGlobal_config[LL_HW_BB_DELAY] = 54;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY] = 40;//52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV] = 63 - RF_PHY_EXT_PREAMBLE_US; //63
    pGlobal_config[LL_HW_Tx_TO_RX_INTV] = 50;//65

    //------------------------------------------------2MPHY
    // LL engine settle time
    pGlobal_config[LL_HW_BB_DELAY_2MPHY] = 59;
    pGlobal_config[LL_HW_AFE_DELAY_2MPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_2MPHY] = 52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_2MPHY] = 72 - RF_PHY_EXT_PREAMBLE_US; //72
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_2MPHY] = 57;//72

    //------------------------------------------------CODEPHY 500K
    // LL engine settle time CODEPHY 500K
    pGlobal_config[LL_HW_BB_DELAY_500KPHY] = 50;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY_500KPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_500KPHY] = 52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_500KPHY] =  2;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_500KPHY] = 66;//72

    //------------------------------------------------CODEPHY 125K
    // LL engine settle time CODEPHY 125K
    pGlobal_config[LL_HW_BB_DELAY_125KPHY] = 30;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY_125KPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_125KPHY] = 52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_125KPHY] = 5;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_125KPHY] = 66;//72



    // LL engine settle time, for advertisement
    pGlobal_config[LL_HW_BB_DELAY_ADV] = 90;
    pGlobal_config[LL_HW_AFE_DELAY_ADV] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_ADV] = 60;

    // adv channel interval
    pGlobal_config[ADV_CHANNEL_INTERVAL] = 1400;//6250;

    pGlobal_config[NON_ADV_CHANNEL_INTERVAL] = 666;//6250;

    if (g_system_clk == SYS_CLK_XTAL_16M) {
        // scan req -> scan rsp timing
        pGlobal_config[SCAN_RSP_DELAY] = 10 + RF_PHY_EXT_PREAMBLE_US; //13;//23;
    } else if (g_system_clk == SYS_CLK_DLL_48M) {
        // scan req -> scan rsp timing
        pGlobal_config[SCAN_RSP_DELAY] = 10 + RF_PHY_EXT_PREAMBLE_US;      // 12    //  2019/3/19 A2: 12 --> 9
    } else if (g_system_clk == SYS_CLK_DLL_64M) {   //  2019/3/26 add
        pGlobal_config[SCAN_RSP_DELAY] = 3 + RF_PHY_EXT_PREAMBLE_US;
    }

    // conn_req -> slave connection event calibration time, will advance the receive window
    pGlobal_config[CONN_REQ_TO_SLAVE_DELAY] = 300;//192;//500;//192;

    // calibration time for 2 connection event, will advance the next conn event receive window
    // SLAVE_CONN_DELAY for sync catch, SLAVE_CONN_DELAY_BEFORE_SYNC for sync not catch
    pGlobal_config[SLAVE_CONN_DELAY] = 400;//0;//1500;//0;//3000;//0;          ---> update 11-20
    pGlobal_config[SLAVE_CONN_DELAY_BEFORE_SYNC] = 110;//160 NG//500 OK

    // RTLP timeout
    pGlobal_config[LL_HW_RTLP_LOOP_TIMEOUT] = 50000;
    pGlobal_config[LL_HW_RTLP_TO_GAP]       = 1000;

    pGlobal_config[LL_HW_RTLP_1ST_TIMEOUT]  = 4000 + pGlobal_config[SLAVE_CONN_DELAY] * 2;//500;

    // direct adv interval configuration
    pGlobal_config[HDC_DIRECT_ADV_INTERVAL] = 1000;
    pGlobal_config[LDC_DIRECT_ADV_INTERVAL] = 6250;

    // A1 ROM metal change for HDC direct adv,
    pGlobal_config[DIR_ADV_DELAY] = 115;   // in us, consider both direct adv broadcast time & SW delay, ... etc.

    // A1 ROM metal change
    pGlobal_config[LL_TX_PKTS_PER_CONN_EVT] = 6;//8;
    pGlobal_config[LL_RX_PKTS_PER_CONN_EVT] = 6;//8;

    pGlobal_config[LL_TRX_NUM_ADAPTIVE_CONFIG] = 8;     //0:        disable adaptive
    //other:    adaptive max limitation


//    pGlobal_config[LL_TX_PWR_TO_REG_BIAS]   = 0x15;   // assume when g_rfPhyTxPower = 0x1f, tx power = 10dBm

    //smart window configuration
    pGlobal_config[LL_SMART_WINDOW_COEF_ALPHA]      = 2;
    pGlobal_config[LL_SMART_WINDOW_TARGET]          = 600;
    pGlobal_config[LL_SMART_WINDOW_INCREMENT]       = 9;
    pGlobal_config[LL_SMART_WINDOW_LIMIT]           = 20000;
    pGlobal_config[LL_SMART_WINDOW_ACTIVE_THD]      = 8;
    pGlobal_config[LL_SMART_WINDOW_ACTIVE_RANGE]    = 0;//300


    pGlobal_config[LL_SMART_WINDOW_FIRST_WINDOW]    = 5000;

    g_smartWindowSize = pGlobal_config[LL_HW_RTLP_1ST_TIMEOUT] ;

    //====== A2 metal change add, for scanner & initiator
    if (g_system_clk == SYS_CLK_XTAL_16M) {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 18 + RF_PHY_EXT_PREAMBLE_US; //20;      //  2019/3/19 A2: 20 --> 18
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 15 + RF_PHY_EXT_PREAMBLE_US; //27;      //  2019/3/19 A2: 27 --> 25
    } else if (g_system_clk == SYS_CLK_DLL_48M) {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 10 + RF_PHY_EXT_PREAMBLE_US; //12;      //  2019/3/19 A2: 12 --> 10
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 12 + RF_PHY_EXT_PREAMBLE_US; //18;
    } else if (g_system_clk == SYS_CLK_DLL_64M) {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 8 + RF_PHY_EXT_PREAMBLE_US;              //  2019/3/26 add
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 10 + RF_PHY_EXT_PREAMBLE_US;
    }


    // TRLP timeout
    pGlobal_config[LL_HW_TRLP_LOOP_TIMEOUT] = 50000;    // enough for 8Tx + 8Rx : (41 * 8 + 150) * 16 - 150 = 7498us
    pGlobal_config[LL_HW_TRLP_TO_GAP]       = 1000;
    pGlobal_config[LL_MOVE_TO_MASTER_DELAY] = 100;

    pGlobal_config[LL_CONN_REQ_WIN_SIZE] = 5;
    pGlobal_config[LL_CONN_REQ_WIN_OFFSET] = 2;

    pGlobal_config[LL_MASTER_PROCESS_TARGET] = 200;   // reserve time for preparing master conn event, delay should be insert if needn't so long time
    pGlobal_config[LL_MASTER_TIRQ_DELAY] = 0;         // timer IRQ -> timer ISR delay


    pGlobal_config[OSAL_SYS_TICK_WAKEUP_TRIM] = 56;  // 0.125us

    pGlobal_config[MAC_ADDRESS_LOC] = 0x11004000;

    // for simultaneous conn & adv/scan
    pGlobal_config[LL_NOCONN_ADV_EST_TIME] = 1400 * 3;
    pGlobal_config[LL_NOCONN_ADV_MARGIN] = 600;

    pGlobal_config[LL_SEC_SCAN_MARGIN] = 2500;//1400;  to avoid mesh proxy llTrigErr 0x15
    pGlobal_config[LL_MIN_SCAN_TIME] = 2000 * 2;

    //  BBB new
    pGlobal_config[TIMER_ISR_ENTRY_TIME] = 15;
    pGlobal_config[LL_MULTICONN_MASTER_PREEMP] = 0;
    pGlobal_config[LL_MULTICONN_SLAVE_PREEMP] = 0;

    pGlobal_config[LL_EXT_ADV_TASK_DURATION] = 2000; //20000
    pGlobal_config[LL_PRD_ADV_TASK_DURATION] = 20000;
    pGlobal_config[LL_CONN_TASK_DURATION] = 5000;
#if (defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV)
    pGlobal_config[LL_EXT_ADV_INTER_PRI_CHN_INT] = 450; // 5000 --> 450
#else
    pGlobal_config[LL_EXT_ADV_INTER_PRI_CHN_INT] = 1500;    // 5000
#endif
    pGlobal_config[LL_EXT_ADV_INTER_SEC_CHN_INT] = 2500;    // 5000 ---> 2500		1500: temp add for 2M OTA, shall not archive
    pGlobal_config[LL_EXT_ADV_PRI_2_SEC_CHN_INT] = 1500;

    pGlobal_config[LL_EXT_ADV_RSC_PERIOD] = 20000;//1000000;
    pGlobal_config[LL_EXT_ADV_RSC_SLOT_DURATION] = 10000;

    pGlobal_config[LL_PRD_ADV_RSC_PERIOD] = 50000;      // 1000000
    pGlobal_config[LL_PRD_ADV_RSC_SLOT_DURATION] = 50000; //10000

    pGlobal_config[LL_EXT_ADV_PROCESS_TARGET] = 150;  //500
    pGlobal_config[LL_PRD_ADV_PROCESS_TARGET] = 150;

    if (g_system_clk == SYS_CLK_DLL_48M) {
        pGlobal_config[EXT_ADV_AUXSCANRSP_DELAY_1MPHY] = 7 + RF_PHY_EXT_PREAMBLE_US;
        pGlobal_config[EXT_ADV_AUXCONNRSP_DELAY_1MPHY] = 7 + RF_PHY_EXT_PREAMBLE_US;

        pGlobal_config[EXT_ADV_AUXSCANRSP_DELAY_2MPHY] = 7 + RF_PHY_EXT_PREAMBLE_US;
        pGlobal_config[EXT_ADV_AUXCONNRSP_DELAY_2MPHY] = 7 + RF_PHY_EXT_PREAMBLE_US;

        pGlobal_config[EXT_ADV_AUXSCANRSP_DELAY_125KPHY] = 63;
        pGlobal_config[EXT_ADV_AUXCONNRSP_DELAY_125KPHY] = 63;
    }

}

int phy6220_ll_info_show()
{
    printf("ll_recv_ctrl_pkt_cnt        : %d\n",     conn_param[0].pmCounter.ll_recv_ctrl_pkt_cnt);
    printf("ll_recv_data_pkt_cnt        : %d\n",     conn_param[0].pmCounter.ll_recv_data_pkt_cnt);
    printf("ll_recv_invalid_pkt_cnt     : %d\n",     conn_param[0].pmCounter.ll_recv_invalid_pkt_cnt);
    printf("ll_recv_abnormal_cnt        : %d\n",     conn_param[0].pmCounter.ll_recv_abnormal_cnt);
    printf("ll_send_data_pkt_cnt        : %d\n",     conn_param[0].pmCounter.ll_send_data_pkt_cnt);
    printf("ll_conn_event_cnt           : %d\n",     conn_param[0].pmCounter.ll_conn_event_cnt);
    printf("ll_recv_crcerr_event_cnt    : %d\n",     conn_param[0].pmCounter.ll_recv_crcerr_event_cnt);
    printf("ll_conn_event_timeout_cnt   : %d\n",     conn_param[0].pmCounter.ll_conn_event_timeout_cnt);
    printf("ll_to_hci_pkt_cnt           : %d\n",     conn_param[0].pmCounter.ll_to_hci_pkt_cnt);
    printf("ll_hci_to_ll_pkt_cnt        : %d\n",     conn_param[0].pmCounter.ll_hci_to_ll_pkt_cnt);
    printf("ll_hci_buffer_alloc_err_cnt : %d\n",     conn_param[0].pmCounter.ll_hci_buffer_alloc_err_cnt);
    printf("ll_miss_master_evt_cnt      : %d\n",     conn_param[0].pmCounter.ll_miss_master_evt_cnt);
    printf("ll_miss_slave_evt_cnt       : %d\n",     conn_param[0].pmCounter.ll_miss_slave_evt_cnt);
    printf("ll_tbd_cnt1                 : %d\n",     conn_param[0].pmCounter.ll_tbd_cnt1);
    printf("ll_tbd_cnt2                 : %d\n",     conn_param[0].pmCounter.ll_tbd_cnt2);
    printf("ll_tbd_cnt3                 : %d\n",     conn_param[0].pmCounter.ll_tbd_cnt3);
    printf("ll_tbd_cnt4                 : %d\n",     conn_param[0].pmCounter.ll_tbd_cnt4);

    printf("ll_send_undirect_adv_cnt    : %d\n",     g_pmCounters.ll_send_undirect_adv_cnt);
    printf("ll_send_nonconn_adv_cnt     : %d\n",     g_pmCounters.ll_send_nonconn_adv_cnt);
    printf("ll_send_scan_adv_cnt        : %d\n",     g_pmCounters.ll_send_scan_adv_cnt);

    printf("ll_send_scan_rsp_cnt        : %d\n",     g_pmCounters.ll_send_scan_rsp_cnt);
    printf("ll_send_scan_req_cnt        : %d\n",     g_pmCounters.ll_send_scan_req_cnt);
    printf("ll_send_conn_rsp_cnt        : %d\n",     g_pmCounters.ll_send_conn_rsp_cnt);
    printf("ll_recv_adv_pkt_cnt         : %d\n",     g_pmCounters.ll_recv_adv_pkt_cnt);
    printf("ll_recv_conn_req_cnt        : %d\n",     g_pmCounters.ll_recv_conn_req_cnt);
    printf("ll_recv_scan_req_cnt        : %d\n",     g_pmCounters.ll_recv_scan_req_cnt);
    printf("ll_recv_scan_req_cnt        : %d\n",     g_pmCounters.ll_recv_scan_req_cnt);
    printf("ll_recv_scan_rsp_cnt        : %d\n",     g_pmCounters.ll_recv_scan_rsp_cnt);
    printf("ll_recv_scan_req_cnt        : %d\n",     g_pmCounters.ll_recv_scan_req_cnt);
    printf("ll_conn_adv_pending_cnt        : %d\n",  g_pmCounters.ll_conn_adv_pending_cnt);
    printf("ll_conn_scan_pending_cnt        : %d\n", g_pmCounters.ll_conn_scan_pending_cnt);

    printf("llAdjBoffUpperLimitFailure  : %d\n",     g_pmCounters.ll_tbd_cnt4);
    printf("scanInfo.numSuccess         : %d\n",     scanInfo.numSuccess);
    printf("scanInfo.numFailure         : %d\n",     scanInfo.numFailure);
    printf("scanInfo.currentBackoff     : %d\n",     scanInfo.currentBackoff);
    printf("llState llSecStatae         : %d %d\n",  llState, llSecondaryState);
    printf("ll_trigger_err              : %d \n",    g_pmCounters.ll_trigger_err);
    printf("timer1 is runing            : %d\r\n",   isTimer1Running());
    printf("read ll remainder time      : %d\r\n",    read_LL_remainder_time());
    printf("ll_event_scheduler_err      : %d\n",      g_pmCounters.ll_evt_shc_err);
    return 0;
}

u8_t patch_conn_get_role(u8_t id)
{
    connect_info_t info;
    int ret = ble_stack_connect_info_get(id, &info);

    if (ret) {
        return 0xff;
    }

    return info.role;
}


#if (   (defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV) || \
        (defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV) || \
        (defined(CONFIG_BT_PER_ADV_SYNC) && CONFIG_BT_PER_ADV_SYNC))
void ll_patch_extended_com()
{
    extern void ll_adv_scheduler1(void);
    extern void ll_add_adv_task1(extAdvInfo_t *pExtAdv);
    extern void ll_delete_adv_task1(uint8 index);
    JUMP_FUNCTION(LL_ADV_SCHEDULER)                 = (uint32_t)&ll_adv_scheduler1;
    JUMP_FUNCTION(LL_ADV_ADD_TASK)                  = (uint32_t)&ll_add_adv_task1;
    JUMP_FUNCTION(LL_ADV_DEL_TASK)                  = (uint32_t)&ll_delete_adv_task1;

    extern void TIM4_IRQHandler1(void);
    JUMP_FUNCTION(V23_IRQ_HANDLER)                  = (uint32_t)&TIM4_IRQHandler1;

#if  (  (defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV) || \
        (defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV) )
    extern void llSetupAdvExtIndPDU1(extAdvInfo_t  *pAdvInfo, periodicAdvInfo_t *pPrdAdv);
    JUMP_FUNCTION(LL_SETUP_ADV_EXT_IND_PDU)         = (uint32_t)&llSetupAdvExtIndPDU1;

    extern void llSetupAuxAdvIndPDU1(extAdvInfo_t  *pAdvInfo, periodicAdvInfo_t *pPrdAdv);
    JUMP_FUNCTION(LL_SETUP_AUX_ADV_IND_PDU)         = (uint32_t)&llSetupAuxAdvIndPDU1;

    extern void llSetupAuxChainIndPDU1(extAdvInfo_t *pAdvInfo, periodicAdvInfo_t *pPrdAdv);
    JUMP_FUNCTION(LL_SETUP_AUX_CHAIN_IND_PDU)       = (uint32_t)&llSetupAuxChainIndPDU1;

    extern void llSetupAuxScanRspPDU1(extAdvInfo_t  *pAdvInfo);
    JUMP_FUNCTION(LL_SETUP_AUX_SCAN_RSP_PDU)        = (uint32_t)&llSetupAuxScanRspPDU1;

    extern void llSetupAuxConnectRspPDU1(extAdvInfo_t  *pAdvInfo);
    JUMP_FUNCTION(LL_SETUP_AUX_CONN_RSP_PDU)        = (uint32_t)&llSetupAuxConnectRspPDU1;

    extern uint8 llSetupExtAdvEvent1(extAdvInfo_t   *pAdvInfo);
    JUMP_FUNCTION(LL_SETUP_EXT_ADV_EVENT)           = (uint32_t)&llSetupExtAdvEvent1;
    extern uint16 LL_ProcessEvent1( uint8 task_id, uint16 events );
    JUMP_FUNCTION(LL_PROCESS_EVENT)           = (uint32_t)&LL_ProcessEvent1;
#endif

#if defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV
    extern void ll_adv_scheduler_periodic1(void);
    extern void ll_add_adv_task_periodic1(periodicAdvInfo_t *pPrdAdv, extAdvInfo_t *pExtAdv);
    extern void ll_delete_adv_task_periodic1(uint8 index);
    JUMP_FUNCTION(LL_ADV_SCHEDULER_PRD)             = (uint32_t)&ll_adv_scheduler_periodic1;
    JUMP_FUNCTION(LL_ADV_ADD_TASK_PRD)              = (uint32_t)&ll_add_adv_task_periodic1;
    JUMP_FUNCTION(LL_ADV_DEL_TASK_PRD)              = (uint32_t)&ll_delete_adv_task_periodic1;

    extern void llSetupAuxSyncIndPDU1(extAdvInfo_t  *pAdvInfo, periodicAdvInfo_t *pPrdAdv);
    JUMP_FUNCTION(LL_SETUP_AUX_SYNC_IND_PDU)        = (uint32_t)&llSetupAuxSyncIndPDU1;

#endif

}
#endif

void ll_patch_rfu()
{
    // update for coded PHY
    //extern void LL_slave_conn_event1(void) ;
    //JUMP_FUNCTION(LL_SLAVE_CONN_EVENT)              =   (uint32_t)&LL_slave_conn_event1;

    // update for nanoIC : for no limit 20ms adv condition
//  JUMP_FUNCTION(LL_SET_ADV_PARAM)                 = (uint32_t)&LL_SetAdvParam1;
//  JUMP_FUNCTION(LL_SET_ADV_CONTROL)               = (uint32_t)LL_SetAdvControl1;
}



#if defined(CONFIG_BT_MAX_CONN) && CONFIG_BT_MAX_CONN > 1
void ll_patch_multi()
{
    extern  void  LL_ENC_Encrypt1(llConnState_t *connPtr, uint8 pktHdr, uint8 pktLen, uint8 * pBuf);
    extern uint8 LL_ENC_Decrypt1(llConnState_t *connPtr, uint8 pktHdr, uint8 pktLen, uint8 * pBuf);
    extern uint8 llProcessMasterControlProcedures1(llConnState_t *connPtr);
    extern uint8 llProcessSlaveControlProcedures1(llConnState_t *connPtr);

    JUMP_FUNCTION(LL_TXDATA)                        =   0;
    JUMP_FUNCTION(LL_SCHEDULER)                     = (uint32_t)&ll_scheduler1;
    JUMP_FUNCTION(LL_ENC_ENCRYPT)                   = (uint32_t)&LL_ENC_Encrypt1;
    JUMP_FUNCTION(LL_ENC_DECRYPT)                   = (uint32_t)&LL_ENC_Decrypt1;

    // for llSetupStartEncRsp()
    // TODO:Optimize
    JUMP_FUNCTION(LL_PROCESS_MASTER_CTRL_PROC)      = (uint32_t)&llProcessMasterControlProcedures1;
    JUMP_FUNCTION(LL_PROCESS_SLAVE_CTRL_PROC)       = (uint32_t)&llProcessSlaveControlProcedures1;
}
#endif

void ll_patch_init()
{
    JUMP_FUNCTION(OSAL_INIT_TASKS) = (uint32_t)osalInitTasks;
    JUMP_FUNCTION(TASKS_ARRAY) = (uint32_t)tasksArr;
    JUMP_FUNCTION(TASK_COUNT) = (uint32_t)&tasksCnt;
    JUMP_FUNCTION(TASK_EVENTS) = (uint32_t)&tasksEvents;
    JUMP_FUNCTION(RF_INIT) = (uint32_t)&rf_phy_ini;
    JUMP_FUNCTION(RF_PHY_CHANGE)                    = (uint32_t)&rf_phy_change_cfg0;
    JUMP_FUNCTION(LL_GEN_TRUE_RANDOM)               = (uint32_t)&LL_ENC_GenerateTrueRandNum1;
    JUMP_FUNCTION(OSAL_POWER_CONSERVE)              = (uint32_t)&osal_pwrmgr_powerconserve1;
    JUMP_FUNCTION(OSAL_SET_EVENT)                     = (uint32_t)&osal_set_event1;
    JUMP_FUNCTION(OSAL_MSG_SEND)                     = (uint32_t)&osal_msg_send1;
    JUMP_FUNCTION(HCI_PROCESS_EVENT)                     = (uint32_t)&HCI_ProcessEvent1;
    JUMP_FUNCTION(LL_HW_GO)                         = (uint32_t)&ll_hw_go1;
    JUMP_FUNCTION(WAKEUP_INIT)                         = (uint32_t)&wakeup_init1;

    // TODO : check diff
    //JUMP_FUNCTION(LL_TXDATA)                         = (uint32_t)&LL_TxData1;
    JUMP_FUNCTION(RF_CALIBRATTE)                    = (uint32_t)&rf_calibrate1;

    JUMP_FUNCTION(LL_CONN_TERMINATE)                = (uint32_t)&llConnTerminate1;
    JUMP_FUNCTION(LL_SEC_ADV_ALLOW)                = (uint32_t)&llSecAdvAllow1;
    JUMP_FUNCTION(LL_CALC_MAX_SCAN_TIME)                = (uint32_t)&llCalcMaxScanTime1;

    // TODO: merge 1,2,3
    JUMP_FUNCTION(LL_MOVE_TO_SLAVE_FUNCTION)            = (uint32_t)&move_to_slave_function3;
    JUMP_FUNCTION(CONFIG_RTC)                       = (uint32_t)&config_RTC1;
    JUMP_FUNCTION(LL_ADP_ADJ_NEXT_TIME)            = (uint32_t)&ll_adptive_adj_next_time1;

#ifdef CONFIG_BT_CENTRAL
    JUMP_FUNCTION(LL_MASTER_EVT_ENDOK)              = (uint32_t)&llMasterEvt_TaskEndOk1;

    // bugfix for terminate with other BLE Device
    extern void llProcessMasterControlPacket1(llConnState_t *connPtr, uint8 * pBuf);
    JUMP_FUNCTION(LL_PROCESS_MASTER_CTRL_PKT)       = (uint32_t)&llProcessMasterControlPacket1;

#endif

#ifdef LL_PATH_DEBUG
    JUMP_FUNCTION(105)                         = (uint32_t)&debug_print1;
    JUMP_FUNCTION(V4_IRQ_HANDLER)                   = (uint32_t)&LL_IRQHandler1;
    JUMP_FUNCTION(LL_SCHEDULER)                     = (uint32_t)&ll_scheduler1;
    JUMP_FUNCTION(LL_SLAVE_EVT_ENDOK)                     = (uint32_t)&llSlaveEvt_TaskEndOk1;
    JUMP_FUNCTION(LL_PROCESS_TX_DATA)                     = (uint32_t)&llProcessTxData1;
#endif

#if (   (defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV) || \
        (defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV) || \
        (defined(CONFIG_BT_PER_ADV_SYNC) && CONFIG_BT_PER_ADV_SYNC))
    ll_patch_extended_com();
#endif

#if defined(CONFIG_BT_MAX_CONN) && CONFIG_BT_MAX_CONN > 1
    ll_patch_multi();
#endif

    // TODO: double check
    ll_patch_rfu();



}

static int phy6220_hci_send(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    uint16_t connHandle, param;
    uint8_t  pbFlag;
    uint16_t pktLen;
#if !(defined(CONFIG_BT_USE_HCI_API) && CONFIG_BT_USE_HCI_API)

    if (data[0] == HCI_CMD_PACKET) {

        hciProcessHostToCtrlCmd_RAW(data + 1, size - 1);

        return size;
    } else
#endif
    {
        uint8_t *pdata = data + 1;
        uint8_t *send_buf = NULL;

        param = BUILD_UINT16(pdata[0], pdata[1]);
        connHandle = param & 0xfff;
        pbFlag = (param & 0x3000) >> 12;
        pktLen = BUILD_UINT16(pdata[2], pdata[3]);

        send_buf = (uint8_t *)HCI_bm_alloc(pktLen);

        if (!send_buf) {
            return -ENOMEM;
        }

        osal_memcpy(send_buf, &pdata[4], pktLen);

        int ret = LL_TxData(connHandle, send_buf, pktLen, pbFlag);

        if (ret) {
            osal_bm_free(send_buf);
            return -ret;
        }

        return  size;
    }
}

static aos_dev_t *phy6220_hci_init(driver_t *drv, void *config, int id)
{
    hci_dev = (hci_dev_t *)device_new(drv, sizeof(hci_dev_t), id);

    return (aos_dev_t *)hci_dev;
}

#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
extern llStatus_t LL_InitialExtendedAdv(extAdvInfo_t *extAdvInfo,
                                        uint8         extAdvSetNumber,
                                        uint16        advSetMaxLen);
static void initLlExtAdvtiser(void)
{
    int i;

    // init advertisement data buffer
    for (i = 0; i < MAX_EXT_ADV_INSTANCE; i ++) {
        extAdvInfo[i].data.advertisingData = NULL;
    }

    for (i = 0; i < MAX_EXT_ADV_INSTANCE; i ++) {
        extAdvInfo[i].data.advertisingData = &advertisingData[i][0];
        extAdvInfo[i].scanRspData          = &extScanRspData[i][0];			// redundancy
    }

    // init LL extended global variables
    LL_InitialExtendedAdv(extAdvInfo, MAX_EXT_ADV_INSTANCE, MAX_EXT_ADV_DATA_LENGTH);
}
#endif

#if defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV
// init LL layer periodic advertiser related buffer
static void initLlPeriodAdvtiser(void)
{
    int i;
    for (i = 0; i < MAX_PERIODIC_EXT_ADV_INSTANCE; i ++) {
        periodicAdvInfo[i].data.advertisingData = &periodicadvertisingData[i][0];
    }

	LL_InitPeriodicAdv(NULL, periodicAdvInfo, MAX_PERIODIC_EXT_ADV_INSTANCE, MAX_EXT_ADV_DATA_LENGTH);
}
#endif

#define phy6220_hci_uninit device_free

static int phy6220_hci_open(aos_dev_t *dev)
{
    osal_mem_set_heap((osalMemHdr_t *)largeHeap, LARGE_HEAP_SIZE);

    LL_InitConnectContext0(pConnContext,
                           g_pConnectionBuffer,
                           BLE_MAX_ALLOW_CONNECTION,
                           BLE_MAX_ALLOW_PKT_PER_EVENT_TX,
                           BLE_MAX_ALLOW_PKT_PER_EVENT_RX,
                           BLE_PKT_VERSION);

#ifdef  BLE_SUPPORT_CTE_IQ_SAMPLE
    LL_EXT_Init_IQ_pBuff(g_llCteSampleI, g_llCteSampleQ);
#endif

    setSleepMode(SYSTEM_SLEEP_MODE);

    /* Initialize the operating system */
    osal_init_system();

#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
	initLlExtAdvtiser();
#if defined(CONFIG_BT_PER_ADV) && CONFIG_BT_PER_ADV
	initLlPeriodAdvtiser();
#endif
#endif

    hciCtrlCmdToken = 1;

    osal_pwrmgr_device(PWRMGR_BATTERY);

    return 0;
}

static int phy6220_hci_close(aos_dev_t *dev)
{
    return 0;
}

static int phy6220_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv)
{
    hci_cb = event;
    hci_arg = priv;
    return 0;
}

static int phy6220_recv(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    osal_event_hdr_t *pMsg;
    uint32_t data_len = 0;
    pMsg = (osal_event_hdr_t *)osal_msg_receive(hciTaskID);
    osal_run_system();

    if (pMsg) {
        if ((pMsg->event == HCI_HOST_TO_CTRL_DATA_EVENT) ||
            (pMsg->event == HCI_CTRL_TO_HOST_EVENT)) {
            hciPacket_t *hci_data = (hciPacket_t *)pMsg;

            if (hci_data->pData[0] == HCI_EVENT_PACKET) {
                data_len = HCI_EVENT_MIN_LENGTH + hci_data->pData[2];
            } else if (hci_data->pData[0] == HCI_ACL_DATA_PACKET) {
                data_len = HCI_DATA_MIN_LENGTH + hci_data->pData[3];
            }

            if (data_len > 0 && data_len <= size) {
                //printf("event %d, status %d, data %s\n", pMsg->event, pMsg->status, bt_hex_real(hci_data->pData, data_len));
                memcpy(data, hci_data->pData, data_len);
            } else {
                data_len = 0;
            }

            // deallocate data
            if (hci_data->pData && hci_data->pData[0] == HCI_ACL_DATA_PACKET) {
                osal_bm_free(((hciPacket_t *)pMsg)->pData);
            }

            if (hci_data->pData[0] == 0x04 && hci_data->pData[1] == 0x3e && (hci_data->pData[3] == 0x02 || hci_data->pData[3] == 0x0d)) {
                if (adv_report_count) {
                    adv_report_count--;
                }
            }
        }

        // deallocate the message
        (void)osal_msg_deallocate((uint8 *)pMsg);
        return data_len;
    }

    return 0;
}

static hci_driver_t phy6220_driver = {
    .drv = {
        .name   = "hci",
        .type   = "hci",
        .init   = phy6220_hci_init,
        .uninit = phy6220_hci_uninit,
        .lpm    = NULL,
        .open   = phy6220_hci_open,
        .close  = phy6220_hci_close,
    },
    .set_event  = phy6220_set_event,
    .send       = phy6220_hci_send,
    .recv       = phy6220_recv,
};

int hci_driver_phy6220_register(int idx)
{
    driver_register(&phy6220_driver.drv, NULL, idx);
    return 0;
}

