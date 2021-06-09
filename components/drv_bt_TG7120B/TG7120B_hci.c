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

#include <devices/hci.h>
#include <k_api.h>
#include <devices/hal/hci_impl.h>
#include "rom_sym_def.h"

#include <devices/device.h>
#include <devices/hci.h>
#include <hci_api.h>
#include <drv/irq.h>

#include <simple_data.h>
#include <jump_function.h>
#include <global_config.h>
#include <clock.h>
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
extern uint8 LL_ENC_GenerateTrueRandNum1(uint8 *buf,
        uint8 len);
extern void osal_pwrmgr_powerconserve1();
extern llStatus_t LL_TxData1(uint16 connId,
                             uint8 *pBuf,
                             uint8  pktLen,
                             uint8  fragFlag);
extern void rf_calibrate1(void);
extern void rf_calibrate2(void);
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

#ifndef CONFIG_LARGE_HEAP_SIZE
#define CONFIG_LARGE_HEAP_SIZE (4 * 1024)
#endif

int         hclk,  pclk;
#define     LARGE_HEAP_SIZE  CONFIG_LARGE_HEAP_SIZE
uint8       largeHeap[LARGE_HEAP_SIZE];
uint32 *pGlobal_config = (uint32 *)(CONFIG_BASE_ADDR);

static hci_event_cb_t hci_cb = NULL;
static void *hci_arg;
// ===================== connection context relate definition

#define   BLE_MAX_ALLOW_CONNECTION              1

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

#define BLE_PKT_LEN (CONFIG_BT_L2CAP_RX_MTU + 5 + 15)

#define   BLE_PKT_BUF_SIZE (BLE_PKT_LEN + (sizeof(struct ll_pkt_desc) - 2))

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


uint8           g_pConnectionBuffer[BLE_CONN_BUF_SIZE] __attribute__((aligned(4)));
llConnState_t   pConnContext[BLE_MAX_ALLOW_CONNECTION];
volatile uint8 g_clk32K_config;

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

        if (hci_cb) {
            hci_cb(HCI_EVENT_READ, 0, hci_arg);
        }

        return (SUCCESS);
    } else {
        return (INVALID_TASK);
    }
}

extern uint8_t hciTaskID;
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

typedef struct {
    osal_event_hdr_t hdr;
    uint8            *pData;
} hciPacket_t;

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

__attribute__((section(".__sram.code")))  void __LL_IRQHandler(void)
{
    extern uint8_t g_intrpt_nested_level[1];
    g_intrpt_nested_level[0]++;
#if defined(CONFIG_BT_CENTRAL)
    LL_IRQHandler1();
#elif defined(CONFIG_BT_OBSERVER)
    LL_IRQHandler2();
#else
    LL_IRQHandler();
#endif
    g_intrpt_nested_level[0]--;
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
    uint32_t xtalcap = 0x10;
    extern int dut_hal_xtalcap_get(uint32_t *xtalcap);
    int ret = dut_hal_xtalcap_get(&xtalcap);
    if (ret != 0) {
        xtalcap = 0x10;
    }
    XTAL16M_CAP_SETTING(xtalcap);

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
    drv_irq_register(TIM4_IRQn, TIM4_IRQHandler);
    csi_vic_set_prio(BB_IRQn, 0);
    csi_vic_set_prio(TIM1_IRQn, 0);
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
    pGlobal_config[LL_HW_PLL_DELAY] = 52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV] = 63;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV] = 50;//65

    //------------------------------------------------2MPHY
    // LL engine settle time
    pGlobal_config[LL_HW_BB_DELAY_2MPHY] = 59;
    pGlobal_config[LL_HW_AFE_DELAY_2MPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_2MPHY] = 52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_2MPHY] = 72;//72
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
        pGlobal_config[SCAN_RSP_DELAY] = 13;//23;
    } else if (g_system_clk == SYS_CLK_DLL_48M) {
        // scan req -> scan rsp timing
        pGlobal_config[SCAN_RSP_DELAY] = 4;        // 12    //  2019/3/19 A2: 12 --> 9
    } else if (g_system_clk == SYS_CLK_DLL_64M) {   //  2019/3/26 add
        pGlobal_config[SCAN_RSP_DELAY] = 3;
    }

    // conn_req -> slave connection event calibration time, will advance the receive window
    pGlobal_config[CONN_REQ_TO_SLAVE_DELAY] = 300;//192;//500;//192;

    // calibration time for 2 connection event, will advance the next conn event receive window
    // SLAVE_CONN_DELAY for sync catch, SLAVE_CONN_DELAY_BEFORE_SYNC for sync not catch
    pGlobal_config[SLAVE_CONN_DELAY] = 400;//0;//1500;//0;//3000;//0;          ---> update 11-20
    pGlobal_config[SLAVE_CONN_DELAY_BEFORE_SYNC] = 160;//160 NG//500 OK

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
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 18;//20;      //  2019/3/19 A2: 20 --> 18
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 25;//27;      //  2019/3/19 A2: 27 --> 25
    } else if (g_system_clk == SYS_CLK_DLL_48M) {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 10;//12;      //  2019/3/19 A2: 12 --> 10
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 10;//18;
    } else if (g_system_clk == SYS_CLK_DLL_64M) {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 8;                //  2019/3/26 add
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 10;
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
    pGlobal_config[LL_NOCONN_ADV_EST_TIME] = 1400;
    pGlobal_config[LL_NOCONN_ADV_MARGIN] = 2600;  //600; to avoid mesh prox llTrigErr 0x05

    pGlobal_config[LL_SEC_SCAN_MARGIN] = 2500;//1400;  to avoid mesh proxy llTrigErr 0x15
    pGlobal_config[LL_MIN_SCAN_TIME] = 2000;

    //  BBB new
    pGlobal_config[TIMER_ISR_ENTRY_TIME] = 15;
    pGlobal_config[LL_MULTICONN_MASTER_PREEMP] = 0;
    pGlobal_config[LL_MULTICONN_SLAVE_PREEMP] = 0;

    pGlobal_config[LL_EXT_ADV_TASK_DURATION] = 20000;
    pGlobal_config[LL_PRD_ADV_TASK_DURATION] = 20000;
    pGlobal_config[LL_CONN_TASK_DURATION] = 5000;
    pGlobal_config[LL_EXT_ADV_INTER_PRI_CHN_INT] = 5000;
    pGlobal_config[LL_EXT_ADV_INTER_SEC_CHN_INT] = 5000;
    pGlobal_config[LL_EXT_ADV_PRI_2_SEC_CHN_INT] = 1500;

    pGlobal_config[LL_EXT_ADV_RSC_PERIOD] = 1000000;
    pGlobal_config[LL_EXT_ADV_RSC_SLOT_DURATION] = 10000;

    pGlobal_config[LL_PRD_ADV_RSC_PERIOD] = 1000000;
    pGlobal_config[LL_PRD_ADV_RSC_SLOT_DURATION] = 10000;

    pGlobal_config[LL_EXT_ADV_PROCESS_TARGET] = 500;
    pGlobal_config[LL_PRD_ADV_PROCESS_TARGET] = 500;

}

int TG7120B_ll_info_show()
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
    return 0;
}

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
    JUMP_FUNCTION(HCI_PROCESS_EVENT)                     = (uint32_t)&HCI_ProcessEvent1;
    JUMP_FUNCTION(LL_HW_GO)                         = (uint32_t)&ll_hw_go1;
    JUMP_FUNCTION(WAKEUP_INIT)                         = (uint32_t)&wakeup_init1;
    JUMP_FUNCTION(LL_TXDATA)                         = (uint32_t)&LL_TxData1;
//    JUMP_FUNCTION(RF_CALIBRATTE)                    = (uint32_t)&rf_calibrate1;
    JUMP_FUNCTION(RF_CALIBRATTE)                    = (uint32_t)&rf_calibrate2;
    JUMP_FUNCTION(LL_SET_ADV_PARAM)                 = (uint32_t)&LL_SetAdvParam1;
    JUMP_FUNCTION(LL_CONN_TERMINATE)                = (uint32_t)&llConnTerminate1;
    JUMP_FUNCTION(LL_SEC_ADV_ALLOW)                = (uint32_t)&llSecAdvAllow1;
    JUMP_FUNCTION(LL_CALC_MAX_SCAN_TIME)                = (uint32_t)&llCalcMaxScanTime1;
    JUMP_FUNCTION(LL_MOVE_TO_SLAVE_FUNCTION)            = (uint32_t)&move_to_slave_function2;

#ifdef CONFIG_BT_CENTRAL
    JUMP_FUNCTION(LL_MASTER_EVT_ENDOK)              = (uint32_t)&llMasterEvt_TaskEndOk1;
#endif

#ifdef LL_PATH_DEBUG
    JUMP_FUNCTION(105)                         = (uint32_t)&debug_print1;
    JUMP_FUNCTION(V4_IRQ_HANDLER)                   = (uint32_t)&LL_IRQHandler1;
    JUMP_FUNCTION(LL_SCHEDULER)                     = (uint32_t)&ll_scheduler1;
    JUMP_FUNCTION(LL_SLAVE_EVT_ENDOK)                     = (uint32_t)&llSlaveEvt_TaskEndOk1;
    JUMP_FUNCTION(LL_PROCESS_TX_DATA)                     = (uint32_t)&llProcessTxData1;
#endif

}

static int TG7120B_hci_send(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    uint16_t connHandle, param;
    uint8_t  pbFlag;
    uint16_t pktLen;
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

static aos_dev_t *TG7120B_hci_init(driver_t *drv, void *config, int id)
{
    hci_dev = (hci_dev_t *)device_new(drv, sizeof(hci_dev_t), id);

    return (aos_dev_t *)hci_dev;
}

#define TG7120B_hci_uninit device_free

static int TG7120B_hci_open(aos_dev_t *dev)
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

    osal_pwrmgr_device(PWRMGR_BATTERY);

    return 0;
}

static int TG7120B_hci_close(aos_dev_t *dev)
{
    return 0;
}

static int TG7120B_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv)
{
    hci_cb = event;
    hci_arg = priv;
    return 0;
}

static int TG7120B_recv(aos_dev_t *dev, uint8_t *data, uint32_t size)
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
            if(hci_data->pData && hci_data->pData[0] == HCI_ACL_DATA_PACKET) {
              osal_bm_free(((hciPacket_t *)pMsg)->pData);
            }
        }

        // deallocate the message
        (void)osal_msg_deallocate((uint8 *)pMsg);
        return data_len;
    }

    return 0;
}

static hci_driver_t TG7120B_driver = {
    .drv = {
        .name   = "hci",
        .type   = "hci",
        .init   = TG7120B_hci_init,
        .uninit = TG7120B_hci_uninit,
        .lpm    = NULL,
        .open   = TG7120B_hci_open,
        .close  = TG7120B_hci_close,
    },
    .set_event  = TG7120B_set_event,
    .send       = TG7120B_hci_send,
    .recv       = TG7120B_recv,
};

int hci_driver_TG7120B_register(int idx)
{
    driver_register(&TG7120B_driver.drv, NULL, idx);
    return 0;
}

