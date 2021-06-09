#include "at_recv_cmd.h"
#include "at_cmd_parse.h"
#include "platform.h"
#include "modem_rf_le501x.h"

uint8_t *find_int_from_str(uint8_t *buff)
{
    uint8_t *pos = buff;
    while (1)
    {
        if (*pos == ',' || *pos == '\r')
        {
            *pos = 0;
            break;
        }
        pos++;
    }
    return pos;
}

enum
{
    AT_CMD_IDX_NAME,
    AT_CMD_IDX_MAC,
    AT_CMD_IDX_ADVINT,
    AT_CMD_IDX_ADV,
    AT_CMD_IDX_Z,
    AT_CMD_IDX_LINK,
    AT_CMD_IDX_CONN,
    AT_CMD_IDX_SLEEP,
    AT_CMD_IDX_DISCONN,
    AT_CMD_IDX_FLASH,
    AT_CMD_IDX_SEND,
    AT_CMD_IDX_TRANSPARENT,
    AT_CMD_IDX_AUTO_TRANSPARENT,
    AT_CMD_IDX_POWER,
};
const char *cmds[] =
    {
        [AT_CMD_IDX_NAME] = "NAME",
        [AT_CMD_IDX_MAC] = "MAC",
        [AT_CMD_IDX_ADVINT] = "ADVINT",
        [AT_CMD_IDX_ADV] = "ADV",
        [AT_CMD_IDX_Z] = "Z",
        [AT_CMD_IDX_LINK] = "LINK",
        [AT_CMD_IDX_CONN] = "CONN",
        [AT_CMD_IDX_SLEEP] = "SLEEP",
        [AT_CMD_IDX_DISCONN] = "DISCONN",
        [AT_CMD_IDX_FLASH] = "FLASH",
        [AT_CMD_IDX_SEND] = "SEND",
        [AT_CMD_IDX_TRANSPARENT] = "+++",
        [AT_CMD_IDX_AUTO_TRANSPARENT] = "AUTO+++",
        [AT_CMD_IDX_POWER] = "POWER",
};

const uint16_t adv_int_arr[6] = {80, 160, 320, 800, 1600, 3200};
//4: 0dbm
//0: -20dbm
//2: -8dbm
//8: 4dbm
//7: 10dbm
//b: 12dbm
const uint16_t tx_power_arr[6] = {4, 0, 2, 8, 7, 0xb};
void hex_arr_to_str(uint8_t *str, const uint8_t hex_arr[], uint8_t arr_len)
{
    for (uint8_t i = 0; i < arr_len; i++)
    {
        sprintf((char *)(str + i * 2), "%02X", hex_arr[i]);
    }
}

void str_to_hex_arr(uint8_t hex_arr[], uint8_t *str, uint8_t arr_len) 
{
    for (uint8_t i = 0; i < arr_len; i++)
    {
        if ((str[i * 2] <= '9') && (str[i * 2] >= '0'))
            hex_arr[i] = (str[i * 2] - '0') * 16;
        else if ((str[i * 2] <= 'f') && (str[i * 2] >= 'a'))
            hex_arr[i] = (str[i * 2] - 'a' + 10) * 16;
        else if ((str[i * 2] <= 'F') && (str[i * 2] >= 'A'))
            hex_arr[i] = (str[i * 2] - 'A' + 10) * 16;

        if ((str[i * 2 + 1] <= '9') && (str[i * 2 + 1] >= '0'))
            hex_arr[i] += (str[i * 2 + 1] - '0');
        else if ((str[i * 2 + 1] <= 'f') && (str[i * 2 + 1] >= 'a'))
            hex_arr[i] += (str[i * 2 + 1] - 'a' + 10);
        else if ((str[i * 2 + 1] <= 'F') && (str[i * 2 + 1] >= 'A'))
            hex_arr[i] += (str[i * 2 + 1] - 'A' + 10);
    }
}

void trans_mode_enter(void)
{
    if(ls_at_buff_env.default_info.auto_trans == true)
    {
        at_clr_uart_buff();
        ls_at_ctl_env.transparent_start = 1;
    }
}

void trans_mode_exit(void)
{
    if(ls_at_ctl_env.transparent_start)
    {
        ls_at_ctl_env.transparent_start = 0;
        at_clr_uart_buff();
    }
}

void at_recv_cmd_handler(at_recv_cmd_t *param)
{
    uint8_t *buff;
    uint8_t index;
    uint8_t msg_len;
    uint8_t temp;
    uint8_t msg_rsp[100];
    memset(msg_rsp, 0, sizeof(msg_rsp));
    buff = param->recv_data;

    for (index = 0; index < (sizeof(cmds) / 4); index++)
    {
        if (memcmp(buff, cmds[index], strlen(cmds[index])) == 0)
        {
            buff += strlen(cmds[index]);
            break;
        }
    }

    // LOG_I("cmd hdl:%d",index);
    // LOG_HEX(param->recv_data,param->recv_len);
    switch (index)
    {
    case AT_CMD_IDX_NAME:
    {
        uint8_t idx = 0;
        switch (*buff++)
        {
        case '?':
            msg_len = sprintf((char*)msg_rsp, "\r\n+NAME:%s\r\nOK\r\n", ble_device_name);
            uart_write(msg_rsp,msg_len);
            break;
        case '=':
            for (idx = 0; idx < DEV_NAME_MAX_LEN; idx++)
            {
                if (*(buff + idx) == '\r')
                    break;
            }
            if (idx >= DEV_NAME_MAX_LEN)
            {
                *(buff + idx) = 0x0;
                msg_len = sprintf((char*)msg_rsp, "\r\n+NAME:%s\r\nERR\r\n", buff);
                uart_write(msg_rsp,msg_len);
                break;
            }
            memset(ble_device_name, 0x0, DEV_NAME_MAX_LEN);
            memcpy(ble_device_name, buff, idx);
            at_update_adv_data();

            msg_len = sprintf((char*)msg_rsp, "\r\n+NAME:%s\r\nOK\r\n", ble_device_name);
            uart_write(msg_rsp,msg_len);
            break;
        default:
            break;
        }
    }
    break;
    case AT_CMD_IDX_ADVINT:
    {
        switch (*buff++)
        {
        case '?':
            msg_len = sprintf((char *)msg_rsp,"\r\n+ADVINT:%d\r\nOK\r\n",ls_at_buff_env.default_info.advint);
            uart_write(msg_rsp,msg_len);
            break;
        case '=':
            temp = ls_at_buff_env.default_info.advint;
            update_adv_intv(adv_int_arr[atoi((const char *)buff)]);
            ls_at_buff_env.default_info.advint = atoi((const char *)buff);
            if(ls_at_buff_env.default_info.advint>5)
            {
                msg_len = sprintf((char *)msg_rsp,"\r\n+ADVINT:%d\r\nERR\r\n",ls_at_buff_env.default_info.advint);
                ls_at_buff_env.default_info.advint = temp;
            }
            else{
                msg_len = sprintf((char *)msg_rsp,"\r\n+ADVINT:%d\r\nOK\r\n",ls_at_buff_env.default_info.advint);
            }
            uart_write(msg_rsp,msg_len);
            break;
        default:
            break;
        }
    }
    break;
    case AT_CMD_IDX_ADV:
    {
        switch (*buff++)
        {
        case '?':
            if(get_adv_status())
            {
                msg_len = sprintf((char *)msg_rsp, "\r\n+ADV:B\r\nOK\r\n");
                uart_write(msg_rsp,msg_len);
            }
            else
            {
               msg_len = sprintf((char *)msg_rsp, "\r\n+ADV:I\r\nOK\r\n");
               uart_write(msg_rsp,msg_len);
            }
            break;
        case '=':
            if(*buff == 'B')
            {
                at_start_adv();
                msg_len = sprintf((char *)msg_rsp, "\r\n+ADV:B\r\nOK\r\n");
                uart_write(msg_rsp,msg_len);
            }
            else if(*buff == 'I')
            {
                at_stop_adv();
                msg_len = sprintf((char *)msg_rsp, "\r\n+ADV:I\r\nOK\r\n");
                uart_write(msg_rsp,msg_len);
            }
            break;
        default:
            break;
        }
    }
    break;
    case AT_CMD_IDX_Z:
    {
        msg_len = sprintf((char *)msg_rsp,"\r\n+Z\r\nOK\r\n");
        uart_write(msg_rsp,msg_len);
        platform_reset(0);
    }
    break;
    case AT_CMD_IDX_MAC:
    {
        uint8_t dev_addr_str[2 * ADDR_LEN + 1];
        uint8_t dev_addr[ADDR_LEN];
        bool type;
        switch (*buff++)
        {
        case '=':
            str_to_hex_arr(dev_addr, buff, ADDR_LEN);
            dev_manager_set_mac_addr(dev_addr);
            hex_arr_to_str(dev_addr_str, dev_addr, ADDR_LEN);
            dev_addr_str[ADDR_LEN * 2] = 0;

            msg_len = sprintf((char *)msg_rsp, "\r\n+MAC:%s\r\nOK\r\n", dev_addr_str);
            uart_write(msg_rsp,msg_len);
            //app_send_gapm_reset_cmd();
            break;
        case '?':
            dev_manager_get_identity_bdaddr(dev_addr, &type);
            hex_arr_to_str(dev_addr_str, dev_addr, ADDR_LEN);
            dev_addr_str[ADDR_LEN * 2] = 0;

            msg_len = sprintf((char *)msg_rsp, "\r\n+MAC:%s\r\nOK\r\n", dev_addr_str);
            uart_write(msg_rsp,msg_len);
            break;
        default:
            break;
        }
    }
    break;
    case AT_CMD_IDX_CONN:
    {
        uint8_t peer_dev_addr[ADDR_LEN];
        uint8_t peer_dev_addr_str[ADDR_LEN*2+1];
        switch (*buff++)
        {
        case '=':
            str_to_hex_arr(peer_dev_addr, buff, ADDR_LEN);
            start_init(peer_dev_addr);
            break;
        case '?':
            break;
        default:
            break;
        }
        hex_arr_to_str(peer_dev_addr_str,peer_dev_addr,ADDR_LEN);
        msg_len = sprintf((char *)msg_rsp,"\r\n+CONN:%s\r\nOK\r\n",peer_dev_addr_str);
        uart_write(msg_rsp,msg_len);
    }
    break;
    case AT_CMD_IDX_DISCONN:
    {
        switch(*buff++)
        {
            case '=':
                if(*buff == 'A')
                {
                    for(uint8_t i=0; i < get_ble_con_num(); i++)
                    {
                        if (get_con_status(i)==false)
                        {
                            continue;
                        }
                        gap_manager_disconnect(i, 0x13);           
                    }
                }
                else
                {
                    uint8_t con_idx = atoi((const char *)buff);
                    if(get_con_status(con_idx))
                    {
                        gap_manager_disconnect(con_idx, 0x13);
                    }
                    else
                    {
                        msg_len = sprintf((char *)msg_rsp,"\r\n+DISCONN:%d\r\nERR\r\n",con_idx);
                        uart_write(msg_rsp,msg_len);
                    }
                }
            break;
            default:break;
        }
    }
    break;
    case AT_CMD_IDX_LINK:
    {
        struct ble_addr tmp_addr;
        uint8_t mac_str[ADDR_LEN*2+1];
        uint8_t link_mode;
        msg_len = sprintf((char *)msg_rsp,"\r\n+LINK\r\nOK\r\n");
        uart_write(msg_rsp,msg_len);
        if ( *buff == '?')
        {
            for (uint8_t i=0; i < get_ble_con_num(); i++)
            {
                if (get_con_status(i)==false)
                {
                    continue;
                }
                if (gap_manager_get_role(i) == LS_BLE_ROLE_SLAVE)
                {
                    gap_manager_get_peer_addr(i, &tmp_addr);
                    link_mode = 'S';
                }
                else
                {
                    gap_manager_get_peer_addr(i, &tmp_addr);
                    link_mode = 'M';
                }
                
                hex_arr_to_str(mac_str,(const uint8_t*)&tmp_addr.addr.addr,ADDR_LEN);
                mac_str[ADDR_LEN*2] = 0;
                msg_len = sprintf((char *)msg_rsp,"Link_ID: %d LinkMode:%c PeerAddr:%s\r\n",i,link_mode,mac_str);
                uart_write(msg_rsp,msg_len);
            }
        }
    }
    break;
    case AT_CMD_IDX_TRANSPARENT:
    {
        if (get_ble_con_num() == 1)
        {
            uint8_t i;
            for (i = 0; i < SDK_MAX_CONN_NUM; i++)
            {
                if (get_con_status(i))
                    break;
            }
            ls_at_ctl_env.transparent_start = 1;
            ls_at_ctl_env.transparent_conidx = i;
            at_clr_uart_buff();

            msg_len = sprintf((char *)msg_rsp, "\r\n+++\r\nOK\r\n");
        }
        else
        {
            msg_len = sprintf((char *)msg_rsp, "\r\n+++\r\nERR\r\n");
        }
        uart_write(msg_rsp,msg_len);
    }
    break;
    case AT_CMD_IDX_FLASH:
    {
        at_store_info_to_flash();
        msg_len = sprintf((char *)msg_rsp,"\r\n+FLASH\r\nOK\r\n");
        uart_write(msg_rsp,msg_len);
    }
    break;
    case AT_CMD_IDX_SEND:
    {
        if ((*buff++) == '=')
        {
            uint8_t *pos_int_end = NULL;
            pos_int_end = find_int_from_str(buff);
            uint8_t con_idx = atoi((const char *)buff);

            buff = pos_int_end + 1;
            pos_int_end = find_int_from_str(buff);
            uint32_t len = atoi((const char *)buff);

            if (get_con_status(con_idx))
            {
                ls_at_ctl_env.transparent_conidx = con_idx;
                ls_at_ctl_env.one_slot_send_len = len;
                ls_at_ctl_env.one_slot_send_start = true;
                at_clr_uart_buff();
                msg_len = sprintf((char *)msg_rsp, "\r\n>\r\n");
            }
            else
            {
                msg_len = sprintf((char *)msg_rsp, "\r\n+SEND\r\nERR\r\n");
            }
            uart_write(msg_rsp,msg_len);
        }
    }
    break;
    case AT_CMD_IDX_AUTO_TRANSPARENT:
    {
        switch (*buff++)
        {
        case '?':
            if(ls_at_buff_env.default_info.auto_trans == true)
                msg_len = sprintf((char *)msg_rsp,"\r\n+AUTO+++:Y\r\nOK\r\n");
            else
                msg_len = sprintf((char *)msg_rsp,"\r\n+AUTO+++:Y\r\nOK\r\n");
            uart_write(msg_rsp,msg_len);
            break;
        case '=':
            if(*buff == 'Y')
            {
                ls_at_buff_env.default_info.auto_trans = true;
                msg_len = sprintf((char *)msg_rsp,"\r\n+AUTO+++:Y\r\nOK\r\n");
            }
            else if(*buff == 'N')
            {
                ls_at_buff_env.default_info.auto_trans = false;
                msg_len = sprintf((char *)msg_rsp,"\r\n+AUTO+++:N\r\nOK\r\n");
            }
            uart_write(msg_rsp,msg_len);
            break;
        default:
            break;
        }
    }
    break;
    case AT_CMD_IDX_POWER:
    {
        switch(*buff++)
        {
            case '?':
                msg_len = sprintf((char *)msg_rsp,"\r\n+POWER:%d\r\nOK\r\n",ls_at_buff_env.default_info.rfpower);
                uart_write(msg_rsp,msg_len);
                break;
            case '=':
                ls_at_buff_env.default_info.rfpower = atoi((const char *)buff);
                if(ls_at_buff_env.default_info.rfpower > 5)
                    msg_len = sprintf((char *)msg_rsp,"\r\n+POWER:%d\r\nERR\r\n",ls_at_buff_env.default_info.rfpower);
                else
                {
                    LOG_I("power:%d",tx_power_arr[ls_at_buff_env.default_info.rfpower]);
                    rf_set_power(tx_power_arr[ls_at_buff_env.default_info.rfpower]);
                    msg_len = sprintf((char *)msg_rsp,"\r\n+POWER:%d\r\nOK\r\n",ls_at_buff_env.default_info.rfpower);
                }
                uart_write(msg_rsp,msg_len);
                break;
            default:
                break;
        }
    }
    break;
    default:
    {
    }
    break;
    }
}
