/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "genie_service.h"
#include "switches.h"
#include "switches_queue.h"
#include "switches_input.h"
#include "switches_output.h"
#include "switches_report.h"
#include "switches_led.h"
#include "sig_models/sig_model_opcode.h"

/* element configuration start */
sig_model_element_state_t switches_state[ELEMENT_NUM];
static sig_model_powerup_t switches_powerup[ELEMENT_NUM];

static struct bt_mesh_model primary_element[] = {
    MESH_MODEL_CFG_SRV_NULL(),
    MESH_MODEL_HEALTH_SRV_NULL(),

    MESH_MODEL_GEN_ONOFF_SRV(&switches_state[0]),
};

static struct bt_mesh_model primary_vendor_element[] = {
    MESH_MODEL_VENDOR_SRV(&switches_state[0]),
};

static struct bt_mesh_model secondary_element[] = {
    MESH_MODEL_GEN_ONOFF_SRV(&switches_state[1]),
};

static struct bt_mesh_model secondary_vendor_element[] = {
    MESH_MODEL_VENDOR_SRV(&switches_state[1]),
};

static struct bt_mesh_model third_element[] = {
    MESH_MODEL_GEN_ONOFF_SRV(&switches_state[2]),
};

static struct bt_mesh_model third_vendor_element[] = {
    MESH_MODEL_VENDOR_SRV(&switches_state[2]),
};

struct bt_mesh_elem switches_elements[] = {
    BT_MESH_ELEM(0, primary_element, primary_vendor_element, GENIE_ADDR_SWITCH),
    BT_MESH_ELEM(0, secondary_element, secondary_vendor_element, GENIE_ADDR_SWITCH),
    BT_MESH_ELEM(0, third_element, third_vendor_element, GENIE_ADDR_SWITCH),
};
/* element configuration end */

#ifdef CONFIG_GENIE_OTA
bool genie_sal_ota_is_allow_reboot(void)
{
    GENIE_LOG_INFO("Not allow to reboot!");
    return false;
}
#endif

static void switches_param_reset(void)
{
    genie_storage_delete_userdata(GFI_MESH_POWERUP);
}

static void switches_save_state(sig_model_element_state_t *p_elem)
{
    uint8_t *p_read = NULL;
    genie_storage_status_e ret;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    p_elem->powerup.last_onoff = p_elem->state.onoff[TYPE_PRESENT];
    switches_powerup[p_elem->element_id].last_onoff = p_elem->state.onoff[TYPE_PRESENT];
#endif

    p_read = aos_malloc(sizeof(switches_powerup));
    if (!p_read)
    {
        GENIE_LOG_WARN("no mem");
        return;
    }
    memset(p_read, 0x00, sizeof(switches_powerup));

    ret = genie_storage_read_userdata(GFI_MESH_POWERUP, p_read, sizeof(switches_powerup));
    if (ret == GENIE_STORAGE_SUCCESS)
    {
        GENIE_LOG_DBG("genie_storage_read_userdata  %d", ret);
    }

    if (memcmp(switches_powerup, p_read, sizeof(switches_powerup)))
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        GENIE_LOG_DBG("save %d %d %d\n",
               switches_powerup[0].last_onoff, switches_powerup[1].last_onoff, switches_powerup[2].last_onoff);
#endif
        genie_storage_write_userdata(GFI_MESH_POWERUP, (uint8_t *)switches_powerup, sizeof(switches_powerup));
    }

    aos_free(p_read);
}

static void switches_update(uint8_t onoff)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    GENIE_LOG_INFO("Switch:%s", (onoff != 0) ? "on" : "off");
#endif
}

void switches_report_poweron_state(int elem_index)
{
    uint16_t index = 0;
    uint8_t payload[20];
    genie_transport_model_param_t genie_transport_model_param;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    payload[index++] = ATTR_TYPE_GENERIC_ONOFF & 0xff;
    payload[index++] = (ATTR_TYPE_GENERIC_ONOFF >> 8) & 0xff;
    payload[index++] = switches_state[elem_index].state.onoff[TYPE_PRESENT];
#endif

    memset(&genie_transport_model_param, 0, sizeof(genie_transport_model_param_t));
    genie_transport_model_param.opid = VENDOR_OP_ATTR_INDICATE;
    genie_transport_model_param.data = payload;
    genie_transport_model_param.len = index;
    genie_transport_model_param.p_elem = &switches_elements[elem_index];
    genie_transport_model_param.retry_period = EVENT_REPORT_INTERVAL;
    genie_transport_model_param.retry = EVENT_REPORT_RETRY;

    genie_transport_send_model(&genie_transport_model_param);
}

void report_event_to_cloud(uint8_t onoff, uint8_t element_id)
{
    uint8_t pdata[3];
    genie_transport_model_param_t genie_transport_model_param;

    pdata[0] = ATTR_TYPE_GENERIC_ONOFF & 0xFF;
    pdata[1] = (ATTR_TYPE_GENERIC_ONOFF >> 8) & 0xFF;
    pdata[2] = onoff;

    memset(&genie_transport_model_param, 0, sizeof(genie_transport_model_param_t));
    genie_transport_model_param.opid = VENDOR_OP_ATTR_INDICATE;
    genie_transport_model_param.tid = genie_transport_gen_tid();
    genie_transport_model_param.data = pdata;
    genie_transport_model_param.len = 3;
    genie_transport_model_param.p_elem = &switches_elements[element_id];
    genie_transport_model_param.retry_period = EVENT_REPORT_INTERVAL;
    genie_transport_model_param.retry = EVENT_REPORT_RETRY;

    genie_transport_send_model(&genie_transport_model_param);
}

#ifdef MESH_MODEL_VENDOR_TIMER
static void switches_handle_order_msg(vendor_attr_data_t *attr_data)
{
    GENIE_LOG_INFO("type:%04x data:%04x\r\n", attr_data->type, attr_data->para);

    if (attr_data->type == ATTR_TYPE_GENERIC_ONOFF)
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        switches_state[0].state.onoff[TYPE_TARGET] = attr_data->para;

        if (switches_state[0].state.onoff[TYPE_PRESENT] != switches_state[0].state.onoff[TYPE_TARGET])
        {
            switches_update(switches_state[0].state.onoff[TYPE_TARGET]);
            switches_state[0].state.onoff[TYPE_PRESENT] = switches_state[0].state.onoff[TYPE_TARGET];
        }
#endif
    }
}
#endif

static void sig_model_event_handler(sig_model_msg *p_msg)
{
    sig_model_element_state_t *p_elem = &switches_state[p_msg->element_id];
    _report_onoff_messages_t data;

    switch (p_msg->opcode)
    {
    case OP_GENERIC_ONOFF_GET:
    {
        GENIE_LOG_DBG("get elem %d onoff:%d\r\n", p_elem->state.onoff[TYPE_PRESENT]);
        data.elem_id = p_elem->element_id;
        data.onoff_status = p_elem->state.onoff[TYPE_PRESENT];
        //switches_report_start(ONOFF_MESSAGES, &data, SWITCHES_CLOUD_REPORT_DELAY);
        report_event_to_cloud(data.onoff_status, data.elem_id);
    }
    break;
    case OP_GENERIC_ONOFF_SET:
    {
        GENIE_LOG_DBG("set elem %d onoff:%d\r\n", p_msg->element_id, p_msg->data[0]);
        p_elem->state.onoff[TYPE_TARGET] = p_msg->data[0];
        GENIE_LOG_DBG("remote-switch set elem(%d):present %d target %d\r\n", p_elem->element_id,
               p_elem->state.onoff[TYPE_PRESENT], p_elem->state.onoff[TYPE_TARGET]);
        switch_led_set(p_elem->element_id, p_elem->state.onoff[TYPE_TARGET]);
        data.elem_id = p_elem->element_id;
        data.onoff_status = p_elem->state.onoff[TYPE_TARGET];
        //switches_report_start(ONOFF_MESSAGES, &data, SWITCHES_CLOUD_REPORT_DELAY);
        switch_output_gpio_set(p_msg->element_id,p_msg->data[0]);
        report_event_to_cloud(data.onoff_status, data.elem_id);
        if (p_elem->state.onoff[TYPE_PRESENT] != p_elem->state.onoff[TYPE_TARGET])
        {
            switches_save_state(p_elem);
        }
        p_elem->state.onoff[TYPE_PRESENT] = p_elem->state.onoff[TYPE_TARGET];
    }
    break;
    default:
        break;
    }
}

static void switches_event_handler(genie_event_e event, void *p_arg)
{
    int index = 0;

    switch (event)
    {
    case GENIE_EVT_SW_RESET:
    {
        switches_param_reset();
    }
    break;
    case GENIE_EVT_MESH_READY:
    {
        //User can report data to cloud at here
        GENIE_LOG_INFO("User report data");
        for (index = 0; index < ELEMENT_NUM; index++)
        {
            switches_report_poweron_state(index);
        }
    }
    break;
    case GENIE_EVT_USER_ACTION_DONE:
    {
        sig_model_element_state_t *p_elem = (sig_model_element_state_t *)p_arg;

        if (p_elem)
        {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            switches_update(p_elem->state.onoff[TYPE_TARGET]);
#endif
            switches_save_state(p_elem);
        }
    }
    break;
    case GENIE_EVT_SIG_MODEL_MSG:
    {
        sig_model_msg *p_msg = (sig_model_msg *)p_arg;

        if (p_msg)
        {
            GENIE_LOG_INFO("SIG mesg ElemID(%d)", p_msg->element_id);
    #ifdef CONFIG_GENIE_MESH_NO_AUTO_REPLY
            sig_model_event_handler(p_msg);
    #endif
        }
    }
    break;
    case GENIE_EVT_VENDOR_MODEL_MSG:
    {
        genie_transport_model_param_t *p_msg = (genie_transport_model_param_t *)p_arg;

        if (p_msg && p_msg->p_model && p_msg->p_model->user_data)
        {
            sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_msg->p_model->user_data;
            GENIE_LOG_INFO("ElemID(%d) TID(%d)", p_elem_state->element_id, p_msg->tid);
        }
    }
    break;

#ifdef CONIFG_GENIE_MESH_USER_CMD
    case GENIE_EVT_DOWN_MSG:
    {
        genie_down_msg_t *p_msg = (genie_down_msg_t *)p_arg;
        //User handle this msg,such as send to MCU
        if (p_msg)
        {
        }
    }
    break;
#endif
#ifdef MESH_MODEL_VENDOR_TIMER
    case GENIE_EVT_TIMEOUT:
    {
        vendor_attr_data_t *pdata = (vendor_attr_data_t *)p_arg;
        //User handle vendor timeout event at here
        if (pdata)
        {
            switches_handle_order_msg(pdata);
        }
    }
    break;
#endif
    default:
    {
    }
    break;
    }
}

static void handle_input_event(uint8_t press_data)
{
    static uint8_t onoff = 0;
    #ifdef CONFIG_PM_SLEEP
    static uint8_t sleep_toggle_flag = 0;
    #endif
    uint8_t port = press_data & 0x0F;
    uint8_t press_type = (press_data & 0xF0) >> 4;
    switch (press_type)
    {
    case SWITCH_PRESS_ONCE:
    {
        _report_onoff_messages_t data;
        data.elem_id = 0;
        GENIE_LOG_DBG("port %d short press", port);
        if (port == INPUT_EVENT_PIN_1)
        {
            data.elem_id = 0;
        }
        else if (port == INPUT_EVENT_PIN_2)
        {
            data.elem_id = 1;
        }
        else if (port == INPUT_EVENT_PIN_3)
        {
            data.elem_id = 2;
        }
        onoff = switches_state[data.elem_id].state.onoff[TYPE_PRESENT];
        if (onoff == 0)
        {
            onoff = 1;
        }
        else if (onoff == 1)
        {
            onoff = 0;
        }
        switches_state[data.elem_id].state.onoff[TYPE_TARGET] = onoff;
        GENIE_LOG_DBG("local -switch set elem(%d):present %d target %d", data.elem_id,
               switches_state[data.elem_id].state.onoff[TYPE_PRESENT], switches_state[data.elem_id].state.onoff[TYPE_TARGET]);
        switches_state[data.elem_id].state.onoff[TYPE_PRESENT] = switches_state[data.elem_id].state.onoff[TYPE_TARGET];
        data.onoff_status = switches_state[data.elem_id].state.onoff[TYPE_TARGET];
        switch_output_gpio_set(data.elem_id,onoff);
        switch_led_set(data.elem_id, onoff);
        //switches_report_start(ONOFF_MESSAGES, &data, SWITCHES_LOCAL_REPORT_DELAY);
        report_event_to_cloud(data.onoff_status, data.elem_id);
        switches_save_state(&switches_state[data.elem_id]);
    }
    break;
    case SWITCH_PRESS_LONG:
    {
        GENIE_LOG_DBG("port %d long press", port);
        #ifdef CONFIG_PM_SLEEP
        if (sleep_toggle_flag == 0)
        {
            genie_lpm_disable();
            sleep_toggle_flag = 1;
            GENIE_LOG_DBG("genie_lpm_disable");
        }
        else if (sleep_toggle_flag == 1)
        {
            genie_lpm_enable(FALSE);
            sleep_toggle_flag = 0;
            GENIE_LOG_DBG("genie_lpm_enable");
        }
        #endif
    }
    break;

    default:
        break;
    }
}

static uint8_t switches_statee_init(uint8_t state_count, sig_model_element_state_t *p_elem)
{
    uint8_t index = 0;

    while (index < state_count)
    {
        p_elem[index].element_id = index;
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        p_elem[index].state.onoff[TYPE_TARGET] = GEN_ONOFF_DEFAULT;
        p_elem[index].state.onoff[TYPE_PRESENT] = GEN_ONOFF_DEFAULT;
#endif
        index++;
    }

    return 0;
}

static int powerup_init(void)
{
    uint8_t index = 0;
    genie_storage_status_e ret;

    // init element state
    memset(switches_state, 0, sizeof(switches_state));
    switches_statee_init(ELEMENT_NUM, switches_state);

    ret = genie_storage_read_userdata(GFI_MESH_POWERUP, (uint8_t *)switches_powerup, sizeof(switches_powerup));
    if (ret == GENIE_STORAGE_SUCCESS)
    {
        while (index < ELEMENT_NUM)
        {
            memcpy(&switches_state[index].powerup, &switches_powerup[index], sizeof(sig_model_powerup_t));
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            switches_state[index].state.onoff[TYPE_TARGET] = switches_powerup[index].last_onoff;
            switches_state[index].state.onoff[TYPE_PRESENT] = switches_powerup[index].last_onoff;
#endif
            index++;
        }
    }

    for (index = 0; index < ELEMENT_NUM; index++)
    {
        GENIE_LOG_DBG("read ret:%d,index:%d, present:%d, target:%d", ret, index,
               switches_state[index].state.onoff[TYPE_PRESENT], switches_state[index].state.onoff[TYPE_TARGET]);
        switch_led_set(index, switches_state[index].state.onoff[TYPE_PRESENT]);
    }

    return ret;
}

static void switches_init(void)
{
    int ret = 0;

    input_event_init();
    queue_init();
    _output_io_config_t io_config[] = {
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_1_A, OUTPUT_PIN_1_B),
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_2_A, OUTPUT_PIN_2_B),
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_3_A, OUTPUT_PIN_3_B),
    };

    ret = switch_output_gpio_init(io_config, sizeof(io_config) / sizeof(io_config[0]));
    if (ret < 0)
    {
        GENIE_LOG_ERR("init output pin faild");
    }
#if 0
    ret = switches_report_init();
    if (ret < 0)
    {
        printf("init report faild\r\n");
    }
#endif
    _switch_led_config_t led_config[] = {
        SWITCH_LED_PIN(LED_PIN_1, 0),
        SWITCH_LED_PIN(LED_PIN_2, 0),
        SWITCH_LED_PIN(LED_PIN_3, 0),
    };
    ret = switch_led_init(led_config, sizeof(led_config) / sizeof(led_config[0]));
    if (ret < 0)
    {
        GENIE_LOG_DBG("init led faild");
    }

    powerup_init();
}

#ifdef CONFIG_PM_SLEEP
__attribute__((section(".__sram.code"))) static void switches_lpm_cb(genie_lpm_wakeup_reason_e reason, genie_lpm_status_e status, void *arg)
{
    if (status == STATUS_WAKEUP)
    {
        if (reason == WAKEUP_BY_IO)
        {
            _genie_lpm_io_status_list_t *list = (_genie_lpm_io_status_list_t *)arg;
            for (int i = 0; i < list->size; i++)
            {
                if (list->io_status[i].trigger_flag == true)
                {
                    input_event_check_key_state(list->io_status[i].port);
                }
            }
        }
    }
    else
    {
        GENIE_LOG_INFO("sleep");
    }
}
#endif

int application_start(int argc, char **argv)
{
    genie_service_ctx_t context;
    queue_mesg_t queue_mesg;
    uint8_t press_data = 0;

    GENIE_LOG_INFO("BTIME:%s\n", __DATE__ ","__TIME__);

    switches_init();

    memset(&context, 0, sizeof(genie_service_ctx_t));
    context.prov_timeout = MESH_PBADV_TIME;
    context.event_cb = switches_event_handler;
    context.p_mesh_elem = switches_elements;
    context.mesh_elem_counts = sizeof(switches_elements) / sizeof(struct bt_mesh_elem);

#ifdef CONFIG_PM_SLEEP
    genie_lpm_wakeup_io_config_t io[] = {GENIE_WAKEUP_PIN(INPUT_EVENT_PIN_1, INPUT_PIN_POL_PIN_1),
                                         GENIE_WAKEUP_PIN(INPUT_EVENT_PIN_2, INPUT_PIN_POL_PIN_2),
                                         GENIE_WAKEUP_PIN(INPUT_EVENT_PIN_3, INPUT_PIN_POL_PIN_3)};

    genie_lpm_wakeup_io_t io_config = {sizeof(io) / sizeof(io[0]), io};
    context.lpm_conf.is_auto_enable = 1;
    context.lpm_conf.lpm_wakeup_io = 1;
    context.lpm_conf.lpm_wakeup_io_config = io_config;
    context.lpm_conf.delay_sleep_time = 20 * 1000; //Unit:ms
    context.lpm_conf.genie_lpm_cb = switches_lpm_cb;
#endif

    if (GENIE_SERVICE_SUCCESS != genie_service_init(&context))
    {
        //Maybe there is no triple info
        GENIE_LOG_INFO("Can i goto standby?");
    }

    while (1)
    {
        if (queue_recv_data(&queue_mesg) < 0)
        {
            continue;
        }

        if (QUEUE_MESG_TYPE_INPUT == queue_mesg.type)
        {
            press_data = queue_mesg.data;
            handle_input_event(press_data);
        }
    }

    return 0;
}
