#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
//#include "ls_mesh.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "lsuart.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"
#include "main.h"

#define UART_SERVER_MASTER_NUM 2 //SDK_MAX_CONN_NUM
#define UART_SERVER_MAX_MTU  517
#define UART_SERVER_MTU_DFT  23
#define UART_SVC_RX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SVC_TX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_CLIENT_NUM (SDK_MAX_CONN_NUM - UART_SERVER_MASTER_NUM)

#define CON_IDX_INVALID_VAL 0xff

uint8_t ble_device_name[DEV_NAME_MAX_LEN] = "LS501X_AT_DEMO";
static const uint8_t ls_uart_svc_uuid_128[] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e};
static const uint8_t ls_uart_rx_char_uuid_128[] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e};
static const uint8_t ls_uart_tx_char_uuid_128[] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e};
static const uint8_t att_decl_char_array[] = {0x03, 0x28};
static const uint8_t att_desc_client_char_cfg_array[] = {0x02, 0x29};
enum uart_svc_att_db_handles
{
    UART_SVC_IDX_RX_CHAR,
    UART_SVC_IDX_RX_VAL,
    UART_SVC_IDX_TX_CHAR,
    UART_SVC_IDX_TX_VAL,
    UART_SVC_IDX_TX_NTF_CFG,
    UART_SVC_ATT_NUM
};

static struct att_decl ls_uart_server_att_decl[UART_SVC_ATT_NUM] =
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
static struct svc_decl ls_uart_server_svc =
{
    .uuid = ls_uart_svc_uuid_128,
    .att = (struct att_decl*)ls_uart_server_att_decl,
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
static struct gatt_svc_env ls_uart_server_svc_env;
// static uint8_t connected_num = 0;
static uint8_t uart_server_connected_num = 0;

// static bool uart_server_tx_busy;
static bool uart_server_ntf_done_array[UART_SERVER_MASTER_NUM];
static uint8_t con_idx_array[UART_SERVER_MASTER_NUM];
static uint16_t cccd_config_array[UART_SERVER_MASTER_NUM];
/************************************************data for client*****************************************************/
enum initiator_status
{
    INIT_IDLE,
    INIT_BUSY,
};
enum scan_status
{
    SCAN_IDLE,
    SCAN_BUSY,
};
enum adv_status
{
    ADV_IDLE,
    ADV_BUSY,
};
static uint8_t uart_client_connected_num = 0;
static uint8_t con_idx_client_array[UART_CLIENT_NUM];
static bool uart_client_wr_cmd_done_array[UART_CLIENT_NUM];

static uint16_t uart_client_svc_attribute_handle[UART_CLIENT_NUM]; // handle for primary service attribute handle
static uint16_t uart_client_svc_end_handle[UART_CLIENT_NUM];

static uint16_t uart_client_tx_attribute_handle[UART_CLIENT_NUM];
static uint16_t uart_client_tx_pointer_handle[UART_CLIENT_NUM];

static uint16_t uart_client_rx_attribute_handle[UART_CLIENT_NUM];
static uint16_t uart_client_rx_pointer_handle[UART_CLIENT_NUM];

static uint16_t uart_client_cccd_handle[UART_CLIENT_NUM];
/********************************************************************************************************************/

static uint8_t adv_obj_hdl;
static uint8_t scan_response_data[31];
static uint8_t scan_obj_hdl = 0xff;
static uint8_t init_obj_hdl = 0xff;
static uint8_t adv_status = ADV_IDLE;
static uint8_t init_status = INIT_IDLE;
static uint8_t scan_status = SCAN_IDLE;
static bool update_adv_intv_flag = false;

static void ls_uart_server_init(void);
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx);
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value);

static void ls_uart_client_init(void);
static void start_scan(void);

static void ls_uart_client_service_dis(uint8_t con_idx);
static void ls_uart_client_char_tx_dis(uint8_t con_idx);
static void ls_uart_client_char_rx_dis(uint8_t con_idx);
static void ls_uart_client_char_desc_dis(uint8_t con_idx);

uint8_t get_ble_con_num(void)
{
    return uart_server_connected_num + uart_client_connected_num;
}

uint8_t get_adv_status(void)
{
    return adv_status;
}

uint8_t search_conidx(uint8_t con_idx)
{
    uint8_t index = 0xff;
    for (uint8_t i = 0; i < UART_SERVER_MASTER_NUM; i++)
    {
        if (con_idx_array[i] == con_idx)
        {
            index = i;
            break;
        }
    }
    return index;
}
uint8_t search_client_conidx(uint8_t con_idx)
{
    uint8_t index = 0xff;
    for (uint8_t i = 0; i < UART_CLIENT_NUM; i++)
    {
        if (con_idx_client_array[i] == con_idx)
        {
            index = i;
            break;
        }
    }
    return index;
}
bool get_con_status(uint8_t con_idx)
{
    if (search_conidx(con_idx) != 0xff || search_client_conidx(con_idx) != 0xff)
    {
        return true;
    }
    else
    {
        return false;
    }
}
static void ls_uart_server_init(void)
{
    for (uint8_t i = 0; i < UART_SERVER_MASTER_NUM; i++)
    {
        con_idx_array[i] = CON_IDX_INVALID_VAL;
        uart_server_ntf_done_array[i] = true;
    }
}
static void ls_uart_client_init(void)
{
    for (uint8_t i = 0; i < UART_CLIENT_NUM; i++)
    {
        con_idx_client_array[i] = CON_IDX_INVALID_VAL;
        uart_client_wr_cmd_done_array[i] = true;
        uart_client_svc_attribute_handle[i] = 0x1;
        uart_client_svc_end_handle[i] = 0xffff;
    }
}

void ble_slave_recv_data_ind(uint8_t con_idx, uint8_t const *value, uint16_t len)
{
    uart_tx_it((uint8_t*)value,len);
}

void ble_master_recv_data_ind(uint8_t con_idx, uint8_t const *value, uint16_t len)
{
    uart_tx_it((uint8_t*)value, len);
}

void ble_slave_send_data(uint8_t con_idx, uint8_t *value, uint16_t len)
{
    LOG_I("Send:slave[%d],%d", con_idx,len);
    uint8_t idx = search_conidx(con_idx);
    if (con_idx != CON_IDX_INVALID_VAL && uart_server_ntf_done_array[idx])
    {
        uart_server_ntf_done_array[idx] = false;
        uint16_t handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, UART_SVC_IDX_TX_VAL);
        uint16_t tx_len = co_min(len, gattc_get_mtu(con_idx)-3);
        enter_critical();
        gatt_manager_server_send_notification(con_idx, handle, value, tx_len, NULL);
        exit_critical();
    }
}

void ble_master_send_data(uint8_t con_idx, uint8_t *value, uint16_t len)
{
    uint8_t idx = search_client_conidx(con_idx);
    if(con_idx != CON_IDX_INVALID_VAL) //&& uart_client_wr_cmd_done_array[idx]) 
    {
        uart_client_wr_cmd_done_array[idx] = false;
        uint16_t tx_len = co_min(len, gattc_get_mtu(con_idx)-3);
        enter_critical();
        gatt_manager_client_write_no_rsp(con_idx, uart_client_rx_pointer_handle[idx], value, tx_len);
        exit_critical();
    }
    
}
void dev_connected_fun(uint8_t con_idx)
{
    uint8_t search_idx = 0xff;
    LOG_I("connected! new con_idx = %d", con_idx);
    if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_SLAVE)
    {
        uart_server_connected_num++;
        search_idx = search_conidx(con_idx);
        LS_ASSERT(search_idx == 0xff);                   // new con_idx should be not found
        search_idx = search_conidx(CON_IDX_INVALID_VAL); // search the first idle idx
        LS_ASSERT(search_idx != 0xff);
        con_idx_array[search_idx] = con_idx;
    }
    else
    {
        uart_client_connected_num++;
        search_idx = search_client_conidx(con_idx);
        LS_ASSERT(search_idx == 0xff);                          // new con_idx should be not found
        search_idx = search_client_conidx(CON_IDX_INVALID_VAL); // search the first idle idx
        LS_ASSERT(search_idx != 0xff);
        con_idx_client_array[search_idx] = con_idx;

        // ls_uart_client_service_dis(con_idx);
        gatt_manager_client_mtu_exch_send(con_idx);
    }

    if(ls_at_ctl_env.transparent_start == false)
    {
        uint8_t at_rsp[30];
        uint8_t msg_len = sprintf((char *)at_rsp,"\r\n+CONN:%d\r\nOK\r\n",con_idx);
        uart_write(at_rsp,msg_len);
    }

    if(get_ble_con_num()==1)
    {
        trans_mode_enter();
    }
    else
    {
        trans_mode_exit();
    }
    
}
void dev_disconnected_fun(uint8_t con_idx,uint8_t reason)
{
    uint8_t search_idx = 0xff;
    LOG_I("disconnected! reason:%d delete con_idx = %d", reason,con_idx);

    uint8_t at_rsp[30];
    uint8_t msg_len = sprintf((char *)at_rsp,"\r\n+DISCONN:%d\r\nOK\r\n",con_idx);
    uart_write(at_rsp,msg_len);

    if ((search_idx = search_conidx(con_idx)) != 0xff)
    {
        uart_server_connected_num--;

        con_idx_array[search_idx] = CON_IDX_INVALID_VAL;
        if (uart_server_connected_num < UART_SERVER_MASTER_NUM)
        {
            at_start_adv();
        }
    }
    else if ((search_idx = search_client_conidx(con_idx)) != 0xff)
    {
        uart_client_connected_num--;
        con_idx_client_array[search_idx] = CON_IDX_INVALID_VAL;
    }
    else
    {
        LS_ASSERT(0);
    }

    if(ls_at_ctl_env.one_slot_send_start && ls_at_ctl_env.one_slot_send_len > 0)
    {
        if(get_con_status(ls_at_ctl_env.transparent_conidx)==false)
        {
            at_clr_uart_buff();
            ls_at_ctl_env.one_slot_send_start = false;
            ls_at_ctl_env.one_slot_send_len = 0;
            uint8_t at_rsp[] = "\r\nSEND FAIL\r\n";
            uart_write(at_rsp,sizeof(at_rsp));
        }
    }
}

static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx)
{
    uint16_t handle = 0;
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, att_idx);
        gatt_manager_server_read_req_reply(con_idx, handle, 0, (void*)&cccd_config_array[array_idx], 2);
    }
}
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value)
{
    uint8_t array_idx = search_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    if(att_idx == UART_SVC_IDX_RX_VAL)
    {
        ble_slave_recv_data_ind(con_idx, value, length);
    }
    else if(att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config_array[array_idx], value, length);
    }
}

static void ls_uart_client_recv_ntf_ind(uint8_t handle, uint8_t con_idx, uint16_t length, uint8_t const *value)
{
    uint8_t array_idx = search_client_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    enter_critical();
    ble_master_recv_data_ind(con_idx, value, length);
    exit_critical();
}

static void gap_manager_callback(enum gap_evt_type type, union gap_evt_u *evt, uint8_t con_idx)
{
    switch (type)
    {
    case CONNECTED:
        dev_connected_fun(con_idx);
        break;
    case DISCONNECTED:
        dev_disconnected_fun(con_idx,evt->disconnected.reason);
        break;
    case CONN_PARAM_REQ:
        LOG_I("CONN_PARAM_REQ");

        break;
    case CONN_PARAM_UPDATED:
        LOG_I("conn_param_updated,intv:%d latency:%d sup_to:%d", evt->conn_param_updated.con_interval, 
                                                                 evt->conn_param_updated.con_latency, 
                                                                 evt->conn_param_updated.sup_to);
        break;
    default:

        break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type, union gatt_evt_u *evt, uint8_t con_idx)
{
    uint8_t array_idx;
    if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_SLAVE)
    {
        array_idx = search_conidx(con_idx);
    }
    else
    {
        array_idx = search_client_conidx(con_idx);
    }
    LS_ASSERT(array_idx != 0xff);
    //LOG_I("con_idx = %d", con_idx);
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
        if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_MASTER)
        {
            ls_uart_client_service_dis(con_idx);
        }

        LOG_I("mtu exch ind, mtu = %d", evt->mtu_changed_ind.mtu);
        break;
    case CLIENT_RECV_NOTIFICATION:
        ls_uart_client_recv_ntf_ind(evt->client_recv_notify_indicate.handle, con_idx, evt->client_recv_notify_indicate.length, evt->client_recv_notify_indicate.value);
        LOG_I("svc dis notification, length = %d", evt->client_recv_notify_indicate.length);
        break;
    case CLIENT_PRIMARY_SVC_DIS_IND:
        if (!memcmp(evt->client_svc_disc_indicate.uuid, ls_uart_svc_uuid_128, sizeof(ls_uart_svc_uuid_128)))
        {
            uart_client_svc_attribute_handle[array_idx] = evt->client_svc_disc_indicate.handle_range.begin_handle;
            uart_client_svc_end_handle[array_idx] = evt->client_svc_disc_indicate.handle_range.end_handle;
            ls_uart_client_char_tx_dis(con_idx);
            LOG_I("svc dis success, attribute_handle = %d, end_handle = %d", uart_client_svc_attribute_handle[array_idx], uart_client_svc_end_handle[array_idx]);
        }
        else
        {
            LOG_I("unexpected svc uuid");
        }
        break;
    case CLIENT_CHAR_DIS_BY_UUID_IND:
        if (!memcmp(evt->client_disc_char_indicate.uuid, ls_uart_tx_char_uuid_128, sizeof(ls_uart_tx_char_uuid_128)))
        {
            uart_client_tx_attribute_handle[array_idx] = evt->client_disc_char_indicate.attr_handle;
            uart_client_tx_pointer_handle[array_idx] = evt->client_disc_char_indicate.pointer_handle;
            ls_uart_client_char_rx_dis(con_idx);
            LOG_I("tx dis success, attribute handle = %d, pointer handler = %d", uart_client_tx_attribute_handle[array_idx], uart_client_tx_pointer_handle[array_idx]);
        }
        else if (!memcmp(evt->client_disc_char_indicate.uuid, ls_uart_rx_char_uuid_128, sizeof(ls_uart_rx_char_uuid_128)))
        {
            uart_client_rx_attribute_handle[array_idx] = evt->client_disc_char_indicate.attr_handle;
            uart_client_rx_pointer_handle[array_idx] = evt->client_disc_char_indicate.pointer_handle;
            ls_uart_client_char_desc_dis(con_idx);
            LOG_I("rx dis success, attribute handle = %d, pointer handler = %d", uart_client_rx_attribute_handle[array_idx], uart_client_rx_pointer_handle[array_idx]);
        }
        else
        {
            LOG_I("unexpected char uuid");
        }
        break;
    case CLIENT_CHAR_DESC_DIS_BY_UUID_IND:
        if (!memcmp(evt->client_disc_char_desc_indicate.uuid, att_desc_client_char_cfg_array, sizeof(att_desc_client_char_cfg_array)))
        {
            uart_client_cccd_handle[array_idx] = evt->client_disc_char_desc_indicate.attr_handle;
            LOG_I("cccd dis success, cccd handle = %d", uart_client_cccd_handle[array_idx]);
            gatt_manager_client_cccd_enable(con_idx, uart_client_cccd_handle[array_idx], 1, 0);
        }
        else
        {
            LOG_I("unexpected desc uuid");
        }
        break;
    case CLIENT_WRITE_WITH_RSP_DONE:
        if (evt->client_write_rsp.status == 0)
        {
            LOG_I("write success");
        }
        else
        {
            LOG_I("write fail, status = %d", evt->client_write_rsp.status);
        }
        break;
    case CLIENT_WRITE_NO_RSP_DONE:
        if (evt->client_write_no_rsp.status == 0)
        {
            LS_ASSERT(gap_manager_get_role(con_idx) == LS_BLE_ROLE_MASTER);
            uart_client_wr_cmd_done_array[array_idx] = true;
            LOG_I("write no rsp success");
        }
        else
        {
            LOG_I("write fail, status = %d", evt->client_write_rsp.status);
        }
        break;
    default:
        LOG_I("Event not handled!");
        break;
    }
}

static void ls_uart_client_service_dis(uint8_t con_idx)
{
    gatt_manager_client_svc_discover_by_uuid(con_idx, (uint8_t *)&ls_uart_svc_uuid_128[0], UUID_LEN_128BIT, 1, 0xffff);
}

static void ls_uart_client_char_tx_dis(uint8_t con_idx)
{
    uint8_t array_idx = search_client_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    gatt_manager_client_char_discover_by_uuid(con_idx, (uint8_t *)&ls_uart_tx_char_uuid_128[0], UUID_LEN_128BIT, uart_client_svc_attribute_handle[array_idx], uart_client_svc_end_handle[array_idx]);
}

static void ls_uart_client_char_rx_dis(uint8_t con_idx)
{
    uint8_t array_idx = search_client_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    gatt_manager_client_char_discover_by_uuid(con_idx, (uint8_t *)&ls_uart_rx_char_uuid_128[0], UUID_LEN_128BIT, uart_client_svc_attribute_handle[array_idx], uart_client_svc_end_handle[array_idx]);
}

static void ls_uart_client_char_desc_dis(uint8_t con_idx)
{
    uint8_t array_idx = search_client_conidx(con_idx);
    LS_ASSERT(array_idx != 0xff);
    gatt_manager_client_desc_char_discover(con_idx, uart_client_svc_attribute_handle[array_idx], uart_client_svc_end_handle[array_idx]);
}

void at_stop_adv(void)
{
    if(adv_status == ADV_BUSY)
        dev_manager_stop_adv(adv_obj_hdl);
}

void at_start_adv(void)
{
    uint8_t *pos;
    uint8_t adv_data_len = 0;
    uint8_t adv_data[28];

    if(adv_status != ADV_IDLE)
    {
        return ;
    }
    adv_status = ADV_BUSY;

    pos = &adv_data[0];
    *pos++ = strlen((const char *)ble_device_name) + 1;
    *pos++ = '\x08';
    memcpy(pos, ble_device_name, strlen((const char *)ble_device_name));
    pos += strlen((const char *)ble_device_name);

    uint8_t manufacturer_value[] = {0x08, 0xB8};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++ = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));

    dev_manager_start_adv(adv_obj_hdl, adv_data, adv_data_len, scan_response_data, sizeof(scan_response_data));
}

void at_update_adv_data(void)
{
    uint8_t *pos;
    uint8_t adv_data_len = 0;
    uint8_t adv_data[28];

    pos = &adv_data[0];
    *pos++ = strlen((const char *)ble_device_name) + 1;
    *pos++ = '\x08';
    memcpy(pos, ble_device_name, strlen((const char *)ble_device_name));
    pos += strlen((const char *)ble_device_name);

    uint8_t manufacturer_value[] = {0x08, 0xB8};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++ = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    adv_data_len = ((uint32_t)pos - (uint32_t)(&adv_data[0]));

    dev_manager_update_adv_data(adv_obj_hdl, adv_data, adv_data_len, scan_response_data, sizeof(scan_response_data));
}

void update_conn_param(uint8_t conidx,uint16_t latency)
{
    struct gap_update_conn_param param = {
        .intv_min = 8,
        .intv_max = 8,
        .latency = 0,
        .sup_timeout = 400,
    };
    gap_manager_update_conn_param(conidx, &param);
}

void update_adv_intv(uint32_t new_adv_intv)
{
    LOG_I("adv_intv:%d",new_adv_intv);
    dev_manager_update_adv_interval(adv_obj_hdl,new_adv_intv,new_adv_intv);
    if(adv_status)
    {
        dev_manager_stop_adv(adv_obj_hdl);
        update_adv_intv_flag = true;
    }    
}

void create_adv_obj(uint32_t adv_intv)
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = adv_intv,
        .adv_intv_max = adv_intv,
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

static void create_init_obj(void)
{
    dev_manager_create_init_object(PUBLIC_OR_RANDOM_STATIC_ADDR);
}
void start_init(uint8_t *peer_addr)
{
    struct dev_addr peer_dev_addr_str;
    memcpy(peer_dev_addr_str.addr, peer_addr, BLE_ADDR_LEN);
    struct start_init_param init_param = {
        .scan_intv = 64,
        .scan_window = 48,
        .conn_to = 0,
        .conn_intv_min = 16,
        .conn_intv_max = 16,
        .conn_latency = 0,
        .supervision_to = 200,

        .peer_addr = &peer_dev_addr_str,
        .peer_addr_type = 0,
        .type = DIRECT_CONNECTION,
    };
    dev_manager_start_init(init_obj_hdl, &init_param);
}
static void start_scan(void)
{
    struct start_scan_param scan_param = {
        .scan_intv = 0x4000,
        .scan_window = 0x4000,
        .duration = 0,
        .period = 0,
        .type = OBSERVER,
        .active = 0,
        .filter_duplicates = 0,
    };
    dev_manager_start_scan(scan_obj_hdl, &scan_param);
    LOG_I("start scan");
}

static void dev_manager_callback(enum dev_evt_type type, union dev_evt_u *evt)
{
    switch (type)
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
        dev_manager_get_identity_bdaddr(addr, &type);
        LOG_I("type:%d,addr:", type);
        LOG_HEX(addr, sizeof(addr));

        dev_manager_add_service(&ls_uart_server_svc);
        ls_uart_server_init();
        ls_uart_client_init();
    }
    break;
    case SERVICE_ADDED:
        gatt_manager_svc_register(evt->service_added.start_hdl, UART_SVC_ATT_NUM, &ls_uart_server_svc_env);
        create_adv_obj(adv_int_arr[ls_at_buff_env.default_info.advint]);
        create_init_obj();
        break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
        at_start_adv();
        LOG_I("adv start hdl:%d", adv_obj_hdl);

        break;
    case ADV_STOPPED:
        LOG_I("adv stopped");
        adv_status = ADV_IDLE;
        if (update_adv_intv_flag)
        {
            update_adv_intv_flag = false;
            at_start_adv();
        }
        break;
    case SCAN_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        scan_obj_hdl = evt->obj_created.handle;
        LOG_I("scan obj hdl:%d", scan_obj_hdl);
        start_scan();
        scan_status = SCAN_BUSY;
        break;
    case SCAN_STOPPED:

        break;
    case ADV_REPORT:

        break;
    case INIT_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        init_obj_hdl = evt->obj_created.handle;
        LOG_I("init obj hdl:%d", init_obj_hdl);
        break;
    case INIT_STOPPED:
        init_status = INIT_IDLE;
        LOG_I("init stopped");
        break;
    default:

        break;
    }
}

int main()
{
    sys_init_app();
    ble_init();
    at_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
