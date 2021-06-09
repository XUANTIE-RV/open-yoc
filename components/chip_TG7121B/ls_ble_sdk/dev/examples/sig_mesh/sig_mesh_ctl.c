#include <string.h>
#include <stdio.h>
#include "sig_mesh_ctl.h"
#include "cpu.h"
#include "log.h"
#include "le501x.h"
#include "lsgpio.h"
#include "sig_light_cfg.h"
extern UART_HandleTypeDef UART_SIG_MESH_Config;
extern struct mesh_model_info model_env;
//static uint8_t advertising_data[28];
static uint8_t msg_state_data[4]={0};
static uint8_t check_msg_state_data[4]={0};
void dev_mesh_status_rsp(struct model_rx_info const *ind,uint32_t opcode,uint8_t *status,uint16_t cmd_len)
{
    struct rsp_model_info rsp;
    rsp.opcode = opcode;
    rsp.ModelHandle = ind->ModelHandle;
    rsp.app_key_lid = ind->app_key_lid;
    rsp.dest_addr = ind->source_addr;
    rsp.len = ind->rx_info_len;
    memcpy(rsp.info, status, cmd_len);
    rsp_model_info_ind(&rsp);
}

void sig_mesh_mdl_state_upd_hdl(struct model_state_upd* msg)
{
    switch(msg->state_id)
    {
        case MESH_STATE_GEN_ONOFF:
        {
            if (msg->elmt_idx==model_env.info[MODEL0_GENERIC_ONOFF_SVC].element_id)
            {
               ls_mesh_light_set_onoff(msg->state, LIGHT_LED_2); 
            }
            else if (msg->elmt_idx==model_env.info[MODEL5_GENERIC_ONOFF_SVC].element_id)
            {
                ls_mesh_light_set_onoff(msg->state, LIGHT_LED_3); 
            }
        }    
        break;
        case MESH_STATE_GEN_LVL:
        {
            if (msg->elmt_idx==model_env.info[MODEL1_GENERIC_LVL_SVC].element_id)
            {
               ls_mesh_light_set_lightness((msg->state - GENERIC_LEVEL_MIN),LIGHT_LED_2); 
            }
            else if (msg->elmt_idx==model_env.info[MODEL2_GENERIC_LVL_SVC].element_id)
            {
                          
               memcpy(&msg_state_data[0], (uint8_t *)&msg->state,4);   
               if (memcmp(&msg_state_data[0],&check_msg_state_data[0],4))
               {
                 memcpy(&check_msg_state_data[0], &msg_state_data[0],4);
                 enter_critical();
                 HAL_UART_Transmit_IT(&UART_SIG_MESH_Config, &msg_state_data[0], 4, NULL);
                 exit_critical();
               }
            }
        }
        break;
        default:
        break;
    }
}
