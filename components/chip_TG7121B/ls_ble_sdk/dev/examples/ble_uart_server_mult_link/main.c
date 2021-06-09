#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "lsuart.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"

#define UART_SVC_ADV_NAME "LS UART Server Mult Link"
#define UART_SERVER_MAX_MTU  517
#define UART_SERVER_MTU_DFT  23
#define UART_SVC_RX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SVC_TX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SERVER_MAX_DATA_LEN(i) (uart_server_mtu_array[i] - 3)

#define UART_SERVER_MASTER_NUM SDK_MAX_CONN_NUM
#define UART_SERVER_TIMEOUT 50 // timer units: ms

#define CON_IDX_INVALID_VAL 0xff
#define UART_SYNC_BYTE  0xA5
#define UART_SYNC_BYTE_LEN 1
#define UART_LEN_LEN  2
#define UART_LINK_ID_LEN 1
#define UART_HEADER_LEN (UART_SYNC_BYTE_LEN + UART_LEN_LEN + UART_LINK_ID_LEN)
#define UART_SVC_BUFFER_SIZE (UART_SERVER_MAX_MTU + UART_HEADER_LEN)
#define UART_RX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)
#define UART_TX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)

#define CONNECTED_MSG_PATTERN 0x5ce5
#define CONNECTED_MSG_PATTERN_LEN 2
#define DISCONNECTED_MSG_PATTERN 0xdead
#define DISCONNECTED_MSG_PATTERN_LEN 2

static const uint8_t ls_uart_svc_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e};
static const uint8_t ls_uart_rx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e};
static const uint8_t ls_uart_tx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e};
static const uint8_t att_decl_char_array[] = {0x03,0x28};
static const uint8_t att_desc_client_char_cfg_array[] = {0x02,0x29};
enum uart_svc_att_db_handles
{
    UART_SVC_IDX_RX_CHAR,
    UART_SVC_IDX_RX_VAL,
    UART_SVC_IDX_TX_CHAR,
    UART_SVC_IDX_TX_VAL,
    UART_SVC_IDX_TX_NTF_CFG,
    UART_SVC_ATT_NUM
};

static const struct att_decl ls_uart_server_att_decl[UART_SVC_ATT_NUM] =
{
    [UART_SVC_IDX_RX_CHAR] = {
        .uuid = att_decl_char_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,   
        .char_prop.rd_en = 1,
    },
    [UART_SVC_IDX_RX_VAL] = {
        .uuid = ls_uart_rx_char_uuid_128,
        .s.max_len = UART_SVC_RX_MAX_LEN,
        .s.uuid_len = UUID_LEN_128BIT,
        .s.read_indication = 1,
        .char_prop.wr_cmd = 1,
        .char_prop.wr_req = 1,
    },
    [UART_SVC_IDX_TX_CHAR] = {
        .uuid = att_decl_char_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,
        .char_prop.rd_en = 1, 
    },
    [UART_SVC_IDX_TX_VAL] = {
        .uuid = ls_uart_tx_char_uuid_128,
        .s.max_len = UART_SVC_TX_MAX_LEN,
        .s.uuid_len = UUID_LEN_128BIT,
        .s.read_indication = 1,
        .char_prop.ntf_en = 1,
    },
    [UART_SVC_IDX_TX_NTF_CFG] = {
        .uuid = att_desc_client_char_cfg_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,
        .char_prop.rd_en = 1,
        .char_prop.wr_req = 1,
    },
};
static const struct svc_decl ls_uart_server_svc =
{
    .uuid = ls_uart_svc_uuid_128,
    .att = (struct att_decl *)ls_uart_server_att_decl,
    .nb_att = UART_SVC_ATT_NUM,
    .uuid_len = UUID_LEN_128BIT,
};
enum uart_rx_status
{
    UART_IDLE,
    UART_SYNC,
    UART_LEN_REV,
    UART_LINK_ID,
    UART_RECEIVING,
};
static uint8_t uart_state = UART_IDLE;
static struct gatt_svc_env ls_uart_server_svc_env;
static uint8_t connected_num = 0; 
static uint8_t uart_server_rx_buf[UART_SVC_BUFFER_SIZE];
static uint8_t uart_server_tx_buf[UART_SERVER_MASTER_NUM][UART_SVC_BUFFER_SIZE];
static uint8_t uart_server_ble_buf_array[UART_SERVER_MASTER_NUM][UART_SVC_BUFFER_SIZE];
static uint16_t uart_server_recv_data_length_array[UART_SERVER_MASTER_NUM];
static UART_HandleTypeDef UART_Server_Config; 
static bool uart_server_tx_busy;
static uint8_t current_uart_tx_idx;
static bool uart_server_ntf_done_array[UART_SERVER_MASTER_NUM];
static uint16_t uart_server_mtu_array[UART_SERVER_MASTER_NUM];
static uint8_t con_idx_array[UART_SERVER_MASTER_NUM];
static uint16_t cccd_config_array[UART_SERVER_MASTER_NUM];
static struct builtin_timer *uart_server_timer_inst = NULL;

static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28];
static uint8_t scan_response_data[31];

static void ls_uart_server_init(void);
static void ls_uart_server_timer_cb(void *param);
static void ls_uart_init(void);
static void ls_uart_server_uart_tx(void);
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx);
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value);
static void ls_uart_server_send_notification(void);
static void start_adv(void);

static uint8_t search_conidx(uint8_t con_idx)
{
    uint8_t index = 0xff;
    for(uint8_t i = 0; i < UART_SERVER_MASTER_NUM; i++)
    {
        if(con_idx_array[i] == con_idx)
        {
            index = i;
            break;
        }
    }
    return index;
}
static void ls_uart_server_init(void)
{
    uart_server_timer_inst = builtin_timer_create(ls_uart_server_timer_cb);
    builtin_timer_start(uart_server_timer_inst, UART_SERVER_TIMEOUT, NULL);
    for (uint8_t i = 0; i < UART_SERVER_MASTER_NUM; i++)
    {
        con_idx_array[i] = CON_IDX_INVALID_VAL;
        uart_server_ntf_done_array[i] = true;
        uart_server_mtu_array[i] = UART_SERVER_MTU_DFT;
        uart_server_recv_data_length_array[i] = 0;
    }    
}
static void ls_uart_server_timer_cb(void *param)
{
    ls_uart_server_send_notification();
    ls_uart_server_uart_tx();
    if(uart_server_timer_inst)
    {
        builtin_timer_start(uart_server_timer_inst, UART_SERVER_TIMEOUT, NULL); 
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart,void *tx_arg)
{
    uart_server_tx_busy = false;
    uart_server_tx_buf[current_uart_tx_idx][0] = 0; // clear sync byte
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart,void *rx_arg)
{
    uint16_t len;
    uint8_t con_idx, array_idx;
    bool restart = true;
    switch (uart_state)
    {
    case UART_IDLE:
        if(uart_server_rx_buf[0] == UART_SYNC_BYTE)
        {
            uart_state = UART_SYNC;
            HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_buf[UART_SYNC_BYTE_LEN], UART_LEN_LEN + UART_LINK_ID_LEN, NULL);
            restart = false;
        }
        break;
    case UART_SYNC:
        memcpy((void*)&len, (void*)&uart_server_rx_buf[UART_SYNC_BYTE_LEN], UART_LEN_LEN);
        con_idx = uart_server_rx_buf[UART_SYNC_BYTE_LEN + UART_LEN_LEN];
        if(len > 0 && len <= UART_RX_PAYLOAD_LEN_MAX && search_conidx(con_idx) != 0xff)
        {
            uart_state = UART_RECEIVING;
            HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_buf[UART_HEADER_LEN], len, NULL);
            restart = false;
        }
        break;
    case UART_RECEIVING:
        memcpy((void*)&len, (void*)&uart_server_rx_buf[UART_SYNC_BYTE_LEN], UART_LEN_LEN);
        con_idx = uart_server_rx_buf[UART_SYNC_BYTE_LEN + UART_LEN_LEN];
        array_idx = search_conidx(con_idx);
        if(array_idx != 0xff && uart_server_recv_data_length_array[array_idx] == 0)
        {
            memcpy((void*)&uart_server_ble_buf_array[array_idx][0], (void*)&uart_server_rx_buf[UART_HEADER_LEN], len);  // discard UART_HEADER_LEN bytes uart header
            uart_server_recv_data_length_array[array_idx] = len;
        }        
        // restart = false;
        break;
    default:
        break;
    }
    if(restart)
    {
        uart_state = UART_IDLE;
        HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_buf[0], UART_SYNC_BYTE_LEN, NULL);
    }
}
static void ls_uart_init(void)
{
    uart1_io_init(PB00, PB01);
    UART_Server_Config.UARTX = UART1;
    UART_Server_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_Server_Config.Init.MSBEN = 0;
    UART_Server_Config.Init.Parity = UART_NOPARITY;
    UART_Server_Config.Init.StopBits = UART_STOPBITS1;
    UART_Server_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Server_Config);
}
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx)
{
    uint16_t handle = 0;
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    if(att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, att_idx);
        gatt_manager_server_read_req_reply(con_idx, handle, 0, (void*)&cccd_config_array[array_idx], 2);
    }
}
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value)
{
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    if(att_idx == UART_SVC_IDX_RX_VAL && uart_server_tx_buf[array_idx][0] != UART_SYNC_BYTE)
    { 
        LS_ASSERT(length <= UART_TX_PAYLOAD_LEN_MAX);
        uart_server_tx_buf[array_idx][0] = UART_SYNC_BYTE;
        uart_server_tx_buf[array_idx][1] = length & 0xff;
        uart_server_tx_buf[array_idx][2] = (length >> 8) & 0xff;
        uart_server_tx_buf[array_idx][3] = con_idx; // what uart will receive should be the real connection index. array_idx is internal.
        memcpy((void*)&uart_server_tx_buf[array_idx][UART_HEADER_LEN], value, length);
        enter_critical();
        if(!uart_server_tx_busy)
        {
            uart_server_tx_busy = true;
            current_uart_tx_idx = array_idx;
            HAL_UART_Transmit_IT(&UART_Server_Config, &uart_server_tx_buf[array_idx][0], length + UART_HEADER_LEN, NULL);
        } 
        exit_critical();
    }
    else if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config_array[array_idx], value, length);
    }
        
}
static void ls_uart_server_send_notification(void)
{
    for(uint8_t idx = 0; idx < UART_SERVER_MASTER_NUM; idx++)
    {
        uint8_t con_idx = con_idx_array[idx];
        enter_critical();
        if(con_idx != CON_IDX_INVALID_VAL && uart_server_recv_data_length_array[idx] != 0 && uart_server_ntf_done_array[idx])
        {
            // LS_ASSERT(con_idx < UART_SERVER_MASTER_NUM);
            uart_server_ntf_done_array[idx] = false;
            uint16_t handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, UART_SVC_IDX_TX_VAL);
            uint16_t tx_len = uart_server_recv_data_length_array[idx] > co_min(UART_SERVER_MAX_DATA_LEN(idx), UART_SVC_TX_MAX_LEN) ? 
                            co_min(UART_SERVER_MAX_DATA_LEN(idx), UART_SVC_TX_MAX_LEN) : uart_server_recv_data_length_array[idx];
            uart_server_recv_data_length_array[idx] -= tx_len;
            gatt_manager_server_send_notification(con_idx, handle, &uart_server_ble_buf_array[idx][0], tx_len, NULL);         
            memcpy((void*)&uart_server_ble_buf_array[idx][0], (void*)&uart_server_ble_buf_array[idx][tx_len], uart_server_recv_data_length_array[idx]);
        }
        exit_critical();
    }
}
static void ls_uart_server_uart_tx(void)
{
    for (uint8_t array_idx = 0; array_idx < UART_SERVER_MASTER_NUM; array_idx++)
    {
        if (uart_server_tx_buf[array_idx][0] == UART_SYNC_BYTE)
        {
            enter_critical();
            if (!uart_server_tx_busy)
            {
                uint16_t length = (uart_server_tx_buf[array_idx][2] << 8) | uart_server_tx_buf[array_idx][1];
                uart_server_tx_busy = true;
                current_uart_tx_idx = array_idx;
                HAL_UART_Transmit_IT(&UART_Server_Config, &uart_server_tx_buf[array_idx][0], length + UART_HEADER_LEN, NULL);
            }
            exit_critical();
            break;
        }
    }
}
static void connect_pattern_send_prepare(uint8_t con_idx)
{
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    uart_server_tx_buf[array_idx][0] = UART_SYNC_BYTE;
    uart_server_tx_buf[array_idx][1] = CONNECTED_MSG_PATTERN_LEN & 0xff;
    uart_server_tx_buf[array_idx][2] = (CONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
    uart_server_tx_buf[array_idx][3] = con_idx;
    uart_server_tx_buf[array_idx][4] = CONNECTED_MSG_PATTERN & 0xff;
    uart_server_tx_buf[array_idx][5] = (CONNECTED_MSG_PATTERN >> 8) & 0xff;
}
static void disconnect_pattern_send_prepare(uint8_t con_idx)
{
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    uart_server_tx_buf[array_idx][0] = UART_SYNC_BYTE;
    uart_server_tx_buf[array_idx][1] = DISCONNECTED_MSG_PATTERN_LEN & 0xff;
    uart_server_tx_buf[array_idx][2] = (DISCONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
    uart_server_tx_buf[array_idx][3] = con_idx;
    uart_server_tx_buf[array_idx][4] = DISCONNECTED_MSG_PATTERN & 0xff;
    uart_server_tx_buf[array_idx][5] = (DISCONNECTED_MSG_PATTERN >> 8) & 0xff;
}
static void gap_manager_callback(enum gap_evt_type type,union gap_evt_u *evt,uint8_t con_idx)
{
    uint8_t search_idx = 0xff;
    switch(type)
    {
    case CONNECTED:
        connected_num++;
        search_idx = search_conidx(con_idx);
        LS_ASSERT(search_idx == 0xff); // new con_idx should be not found
        search_idx = search_conidx(CON_IDX_INVALID_VAL); // search the first idle idx
        LS_ASSERT(search_idx != 0xff);
        con_idx_array[search_idx] = con_idx;   
        connect_pattern_send_prepare(con_idx); 
        LOG_I("connected! new con_idx = %d", con_idx);
    break;
    case DISCONNECTED:
        connected_num--;
        search_idx = search_conidx(con_idx);
        LS_ASSERT(search_idx != 0xff);
        disconnect_pattern_send_prepare(con_idx);
        con_idx_array[search_idx] = CON_IDX_INVALID_VAL;        
        uart_server_mtu_array[search_idx] = UART_SERVER_MTU_DFT;
        if (connected_num == UART_SERVER_MASTER_NUM - 1)
        {
            start_adv();
        }
        LOG_I("disconnected! delete con_idx = %d", con_idx);
    break;
    case CONN_PARAM_REQ:
        //LOG_I
    break;
    case CONN_PARAM_UPDATED:

    break;
    default:

    break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type,union gatt_evt_u *evt,uint8_t con_idx)
{
    uint8_t array_idx;
    array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    switch (type)
    {
    case SERVER_READ_REQ:
        LOG_I("read req");
        ls_uart_server_read_req_ind(evt->server_read_req.att_idx, con_idx);
    break;
    case SERVER_WRITE_REQ:
        LOG_I("write req");
        ls_uart_server_write_req_ind(evt->server_write_req.att_idx, con_idx, evt->server_write_req.length, evt->server_write_req.value);
    break;
    case SERVER_NOTIFICATION_DONE:
        uart_server_ntf_done_array[array_idx] = true;
        LOG_I("ntf done");
    break;
    case MTU_CHANGED_INDICATION:
        uart_server_mtu_array[array_idx] = evt->mtu_changed_ind.mtu;
    break;
    default:
        LOG_I("Event not handled!");
        break;
    }
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 0x20,
        .adv_intv_max = 0x20,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_GEN_DISC,
        .prop = {
            .connectable = 1,
            .scannable = 1,
            .directed = 0,
            .high_duty_cycle = 0,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}
static void start_adv(void)
{
    LS_ASSERT(adv_obj_hdl != 0xff);
    uint8_t adv_data_length = ADV_DATA_PACK(advertising_data, 1, GAP_ADV_TYPE_SHORTENED_NAME, UART_SVC_ADV_NAME, sizeof(UART_SVC_ADV_NAME));
    dev_manager_start_adv(adv_obj_hdl, advertising_data, adv_data_length, scan_response_data, 0);
    LOG_I("adv start");
}
static void dev_manager_callback(enum dev_evt_type type,union dev_evt_u *evt)
{
    switch(type)
    {
    case STACK_INIT:
    {
        struct ble_stack_cfg cfg = {
            .private_addr = false,
            .controller_privacy = false,
        };
        dev_manager_stack_init(&cfg);
    }
    break;
    case STACK_READY:
    {
        uint8_t addr[6];
        bool type;
        dev_manager_get_identity_bdaddr(addr,&type);
        LOG_I("type:%d,addr:",type);
        LOG_HEX(addr,sizeof(addr));
        dev_manager_add_service((struct svc_decl *)&ls_uart_server_svc);
        ls_uart_init(); 
        HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_buf[0], UART_SYNC_BYTE_LEN, NULL);
        ls_uart_server_init();      
    }
    break;
    case SERVICE_ADDED:
        gatt_manager_svc_register(evt->service_added.start_hdl, UART_SVC_ATT_NUM, &ls_uart_server_svc_env);
        create_adv_obj();
    break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
        start_adv();
    break;
    case ADV_STOPPED:
        LOG_I("adv stopped");
        if(connected_num < UART_SERVER_MASTER_NUM)
        {
            start_adv();
        }
    break;
    case SCAN_STOPPED:
    
    break;
    default:

    break;
    }
    
}

int main()
{
    sys_init_app();
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
