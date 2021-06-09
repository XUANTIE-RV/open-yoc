/**
 * @file    hci_hal_h4.c
 *
 * @brief   Bluetooth driver for YOC platform
 *
 * @author  WinnerMicro
 *
 * Copyright (c) 2020 Winner Microelectronics Co., Ltd.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <aos/debug.h>
#include <aos/queue.h>

#include <devices/hal/hci_impl.h>
#include <devices/device.h>
#include <devices/driver.h>
#include <drv/gpio.h>
#include <pin_name.h>
#include <pinmux.h>
#include <devices/hci.h>

#include "wm_regs.h"
#include "wm_bt.h"
#include "hci_hal_h4.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define TAG "bt_hci_h4"
#define HCI_DRV_D(...)
#define HCI_RESP_QUEUE_FULL      (10)
#define HCI_RESP_QUEUE_HALF_FULL (5)

#ifndef MIN
    #define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#define HCI_COMMAND_PKT         0x01
#define HCI_ACLDATA_PKT         0x02
#define HCI_SCODATA_PKT         0x03
#define HCI_EVENT_PKT           0x04
#define BT_HCI_EVT_LE_META_EVENT                0x3e
#define BT_HCI_EVT_LE_ADVERTISING_REPORT        0x02

#define HCI_DBG FALSE


#ifndef HCI_DBG
#define HCI_DBG TRUE
#endif

#if (HCI_DBG == TRUE)
#define HCIDBG(fmt, ...)  \
    do{\
        if(1) \
            LOGD(TAG, "%s(L%d): " fmt, __FUNCTION__, __LINE__,  ## __VA_ARGS__); \
    }while(0)
#else
#define HCIDBG(param, ...)
#endif

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

struct QUEUE_ITEM{  
    int size;
    uint8_t *payload;
    TAILQ_ENTRY(QUEUE_ITEM) entries;  
};

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */

static void controller_rcv_pkt_ready(int available_hci_packets_num);
static void host_rcv_pkt(uint8_t *data, uint16_t len);
static void  hci_dbg_hexstring(const char *msg, const uint8_t *ptr, int a_length);


/*
 * GLOBAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern ATTRIBUTE_ISR void BLE_IRQHandler(void);
extern ATTRIBUTE_ISR void BT_IRQHandler(void);

extern void drv_irq_register(uint32_t irq_num, void *irq_handler);
extern void w800_board_init(void);

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static void *g_priv = NULL;
static hci_event_cb_t  g_event;
static aos_mutex_t hci_resp_queue_mutex;
static aos_sem_t hci_send_sem;
static TAILQ_HEAD(,QUEUE_ITEM) hci_resp_queue_head; 
static volatile uint8_t hci_resp_queue_counter = 0;
static const tls_bt_host_if_t vuart_host_cb = {
    .notify_controller_avaiable_hci_buffer = controller_rcv_pkt_ready,
    .notify_host_recv_h4 = host_rcv_pkt,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#if (HCI_DBG == TRUE)
static void  hci_dbg_hexstring(const char *msg, const uint8_t *ptr, int a_length)
{
#define DBG_TRACE_WARNING_MAX_SIZE_W  252
    //123
#define DBG_TRACE_WARNING_MAX_SIZE    256
    //128
    char sbuffer[DBG_TRACE_WARNING_MAX_SIZE];
    uint8_t offset = 0;
    int i = 0;
    int length = 0;

    if(msg)
    {
        printf("%s", msg);
    }

    if(a_length <= 0 || ptr == NULL)
    {
        return;
    }

    //length = MIN(40, a_length);
    length = a_length;

    do
    {
        for(; i < length; i++)
        {
            offset += sprintf(sbuffer + offset, "%02X ", (uint8_t)ptr[i]);

            if(offset > DBG_TRACE_WARNING_MAX_SIZE_W)
            {
                break;
            }
        }

        sbuffer[offset - 1] = '\r';
        sbuffer[offset] = '\n';
        sbuffer[offset+1] = 0;

        if(offset > DBG_TRACE_WARNING_MAX_SIZE_W)
        {
            sbuffer[offset - 2] = '.';
            sbuffer[offset - 3] = '.';
        }

        printf("%s", sbuffer);
        offset = 0;
    } while(i < length);
}
#endif

static void controller_rcv_pkt_ready(int available_hci_packets_num)
{
    //HCIDBG("%s", __func__);
    aos_sem_signal(&hci_send_sem);
}
static void host_rcv_pkt(uint8_t *data, uint16_t len)
{
    //TODO, we need to check advertisements packets. it may eats up the ram.
    //FOR SCAN RESPONSE MSG, if(hci_resp_queue_counter > HCI_RESP_QUEUE_HALF_FULL) discard it;
    if(data == NULL || len == 0) return;
    
#if (HCI_DBG == TRUE)    
    //hci_dbg_hexstring("<<<", data, len);
    HCIDBG("%s, pending_counter=%d", __func__, hci_resp_queue_counter);
#endif    
    if(data[0] == HCI_EVENT_PKT && data[1] == BT_HCI_EVT_LE_META_EVENT && data[3] == BT_HCI_EVT_LE_ADVERTISING_REPORT)
    {
        if(hci_resp_queue_counter > HCI_RESP_QUEUE_HALF_FULL)
        {
            HCIDBG("Too much hci_adv_report_evt, discard it");
            return;
        }
    }
    if(hci_resp_queue_counter > HCI_RESP_QUEUE_FULL)
    {
        HCIDBG("Too much(%d) hci_response_evt, discard it", hci_resp_queue_counter);
        return;
    }
    
    
    struct QUEUE_ITEM *item = aos_malloc(sizeof(struct QUEUE_ITEM));
    if(item == NULL)
    {
        LOGE(TAG, "Alloc queue item failed, no memory available");
        return;
    }
    item->payload = aos_malloc(len);
    if(item->payload == NULL)
    {
        LOGE(TAG, "Alloc queue item payload failed, no memory available");
        aos_free(item);
        return;
    }
    memcpy(item->payload, data, len);
    item->size = len;
    aos_mutex_lock(&hci_resp_queue_mutex, AOS_WAIT_FOREVER);
    TAILQ_INSERT_TAIL(&hci_resp_queue_head, item, entries);
    hci_resp_queue_counter++;
    aos_mutex_unlock(&hci_resp_queue_mutex);

    //notify host stack to read;
    if(g_event)
    {
        //HCIDBG("Notify host stack to read(%d)", len);
        g_event(HCI_EVENT_READ, len, g_priv);
    }
}
static int h4_hal_open(aos_dev_t *dev)
{
    LOGD(TAG, "%s", __func__);
    tls_bt_status_t status;
    int ret;
    
    TAILQ_INIT(&hci_resp_queue_head);
    hci_resp_queue_counter = 0;
    
    ret = aos_mutex_new(&hci_resp_queue_mutex);
    if(ret !=0 )
    {
        LOGE(TAG, "create aos_mutex_new error");
        return -1;    
    }
    ret = aos_sem_new(&hci_send_sem, 0);
    if (ret != 0) 
    {
        LOGE(TAG, "create sem error");
        aos_mutex_free(&hci_resp_queue_mutex);
        return -1;
    }
    //running the controller task;
    status = tls_bt_ctrl_enable(NULL, 0);
    if(status != TLS_BT_STATUS_SUCCESS)
    {
        LOGE(TAG, "### bt controller enable fail");
        tls_bt_ctrl_if_unregister();
        aos_mutex_free(&hci_resp_queue_mutex);
        aos_sem_free(&hci_send_sem);
        return -1;        
    }
    //register the host notify interface
    tls_bt_ctrl_if_register(&vuart_host_cb);
    
    //tls_rf_bt_mode(1);
    return 0;
}

static int h4_hal_close(aos_dev_t *dev)
{
    LOGD(TAG, "%s", __func__);
    struct QUEUE_ITEM *item = NULL;
    
    //free the host notify interface
    tls_bt_ctrl_if_unregister();
    //release the controller resouce;
    tls_bt_ctrl_disable();
    aos_mutex_free(&hci_resp_queue_mutex);
    aos_sem_free(&hci_send_sem);
    hci_resp_queue_counter = 0;
    //release all pending response queue  if necessary;
    item = TAILQ_FIRST(&hci_resp_queue_head);
    while(item)
    {
        TAILQ_REMOVE(&hci_resp_queue_head, item, entries);
        aos_free(item->payload);
        aos_free(item);
        item = TAILQ_FIRST(&hci_resp_queue_head);
    }
    return 0;
}

static int h4_send_data(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    HCIDBG("%s", __FUNCTION__);    
#if (HCI_DBG == TRUE)    
    hci_dbg_hexstring(">>>", data, size);
#endif
    int ret = -1;

    if(data == NULL || size == 0) return 0;
    
    if(0==aos_sem_wait(&hci_send_sem, AOS_WAIT_FOREVER)){
        if (tls_bt_vuart_host_check_send_available()) {
            HCIDBG("%s>>> sent out", __FUNCTION__);
            tls_bt_vuart_host_send_packet(data, size);
            ret = size;
        }else{
            HCIDBG("%s,VHCI NOT AVA>>>", __FUNCTION__);
        }
    }else{
        HCIDBG("WAIT SEND AVA TIMEOUT");
    }
    
    return ret;
}
static int h4_recv_data(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    HCIDBG("%s, size=%d", __FUNCTION__, size);
    uint32_t act_size = 0;
    struct QUEUE_ITEM *item = NULL;
    //struct QUEUE_ITEM *item_next = NULL;

    if(data == NULL) return 0;
    
    aos_mutex_lock(&hci_resp_queue_mutex, AOS_WAIT_FOREVER);
    item = TAILQ_FIRST(&hci_resp_queue_head);
    if(item)
    {
        TAILQ_REMOVE(&hci_resp_queue_head, item, entries);
        //item_next = TAILQ_FIRST(&hci_resp_queue_head);
        hci_resp_queue_counter--;
    }    
    aos_mutex_unlock(&hci_resp_queue_mutex);

    if(item)
    {
        //assert(size == item->size)
        //if(data == NULL) return -1;
        //Make sure there is enough room for reading out
        act_size = MIN(size, item->size);
        memcpy(data, item->payload, act_size);
        aos_free(item->payload);
        aos_free(item);
#if (HCI_DBG == TRUE)        
        hci_dbg_hexstring("***", data, act_size);
#endif
    }

    /*More hci response available, notify the host statck for reading now*/
    //Not necessary, host will read again if more data available;
#if 0    
    if(item_next)
    {
        if(g_event)
        {
            LOGD(TAG, "More hci response available, notify host stack again...");
            g_event(HCI_EVENT_READ, item_next->size, g_priv);
        }
    }
#endif    
    return act_size;
}

static int h4_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv)
{
    LOGD(TAG,"%s", __FUNCTION__);
    g_event = event;
    g_priv = priv;

    return 0;
}

static int h4_start(aos_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    LOGD(TAG,"%s", __FUNCTION__);
    //maybe, we do not care this function;
    return 0;
}
static aos_dev_t *h4_hal_init(driver_t *drv, void *config, int id)
{
    LOGD(TAG,"%s", __FUNCTION__);
    
    drv_irq_register(BLE_IRQn, BLE_IRQHandler);
    drv_irq_register(BT_IRQn, BT_IRQHandler);
    
    hci_driver_t *h4_dev = (hci_driver_t *)device_new(drv, sizeof(hci_driver_t), id);

    return (aos_dev_t *)h4_dev;
}

#define h4_hal_uninit device_free

static hci_driver_t h4_driver = {
    .drv = {
        .name   = "hci",
        .init   = h4_hal_init,
        .uninit = h4_hal_uninit,
        .lpm    = NULL,
        .open   = h4_hal_open,
        .close  = h4_hal_close,
    },
    .set_event = h4_set_event,
    .send      = h4_send_data,
    .recv      = h4_recv_data,
    .start     = h4_start,
};

/**
 ****************************************************************************************
 * @brief bluetooth register function.
 ****************************************************************************************
 */

void bt_w800_register()
{
    LOGD(TAG,"%s", __FUNCTION__);
    w800_board_init();
    driver_register(&h4_driver.drv, NULL, 0);
}

/**
 ****************************************************************************************
 * @WITH PHYSICAL UARTn, W800 SUPPORTS DIRECT TEST MODE,
 * @THE FOLLOWING CODES IMPLY THE UART R/W OPERTION
 ****************************************************************************************
 */

__attribute__((weak)) int tls_uart_read(uint16_t uart_no, uint8_t * buf, uint16_t readsize)
{
    return readsize;
}

__attribute__((weak)) void tls_uart_rx_callback_register(uint16_t uart_no, int16_t(*rx_callback) (uint16_t len, void* user_data), void *priv_data)
{
    return;
}
__attribute__((weak)) int tls_uart_try_read(uint16_t uart_no)
{
    return 0;
}
__attribute__((weak)) void tls_uart_tx_sent_callback_register(uint16_t uart_no, int16_t(*tx_callback) (void *port))
{
    
}
__attribute__((weak)) int tls_uart_write_async(uint16_t uart_no, int8_t *buf, uint16_t writesize)
{
    return 0;
}
__attribute__((weak)) int tls_uart_port_init(uint16_t uart_no, void * opts, uint8_t modeChoose)
{
    return 0;
}
