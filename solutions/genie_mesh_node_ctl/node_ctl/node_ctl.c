/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "common/log.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <api/mesh.h>
#include "genie_service.h"
#include "node_ctl.h"

static gpio_dev_t led_gpio;
static sig_model_element_state_t node_ctl_elem_stat[ELEMENT_NUM];
static sig_model_powerup_t node_ctl_powerup[ELEMENT_NUM];

static struct bt_mesh_model primary_element[] = {
    MESH_MODEL_CFG_SRV_NULL(),
    MESH_MODEL_HEALTH_SRV_NULL(),

    MESH_MODEL_GEN_ONOFF_SRV(&node_ctl_elem_stat[0]),
    MESH_MODEL_LIGHTNESS_SRV(&node_ctl_elem_stat[0]),
    MESH_MODEL_CTL_SRV(&node_ctl_elem_stat[0]),
    MESH_MODEL_SCENE_SRV(&node_ctl_elem_stat[0])
};

static struct bt_mesh_model primary_vendor_element[] = {
    MESH_MODEL_VENDOR_SRV(&node_ctl_elem_stat[0]),
};

struct bt_mesh_elem node_ctl_elements[] = { //node don't sub any group address by default
    BT_MESH_ELEM(0, primary_element, primary_vendor_element, 0),
};

#ifdef CONFIG_GENIE_OTA
bool genie_sal_ota_is_allow_reboot(void)
{
    // the device will reboot when it is off
    if (node_ctl_elem_stat[0].state.onoff[TYPE_PRESENT] == 0)
    {
        // save light para, always off
        node_ctl_powerup[0].last_onoff = 0;
        genie_storage_write_userdata(GFI_MESH_POWERUP, (uint8_t *)node_ctl_powerup, sizeof(node_ctl_powerup));
        GENIE_LOG_INFO("Allow to reboot!");

        return true;
    }

    GENIE_LOG_INFO("light is no so no reboot!");

    return false;
}
#endif

void node_ctl_elem_state_init(void)
{
    uint8_t index = 0;
    genie_storage_status_e ret;

    memset(node_ctl_elem_stat, 0, sizeof(node_ctl_elem_stat));

    // load light param
    ret = genie_storage_read_userdata(GFI_MESH_POWERUP, (uint8_t *)node_ctl_powerup, sizeof(node_ctl_powerup));
    for (index = 0; index < ELEMENT_NUM; index++)
    {
        node_ctl_elem_stat[index].element_id = index;

        if (ret == GENIE_STORAGE_SUCCESS) //Use saved data
        {
            memcpy(&node_ctl_elem_stat[index].powerup, &node_ctl_powerup[index], sizeof(sig_model_powerup_t));
        }
        else //Use default value
        {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
            node_ctl_elem_stat[index].powerup.last_onoff = GEN_ONOFF_DEFAULT;
#endif
        }

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        node_ctl_elem_stat[index].state.onoff[TYPE_PRESENT] = 0;
        node_ctl_elem_stat[index].state.onoff[TYPE_TARGET] = node_ctl_elem_stat[index].powerup.last_onoff;
#endif
    }
}

static int32_t node_ctl_set_led_status(led_status_e staus)
{
    int32_t ret = 0;

    if (staus == LED_ON)
    {
        ret = hal_gpio_output_low(&led_gpio);
    }
    else
    {
        ret = hal_gpio_output_high(&led_gpio);
    }

    if (ret != 0)
    {
        GENIE_LOG_ERR("fail ret:%d", ret);
    }

    return ret;
}

static void node_ctl_handle_order_msg(vendor_attr_data_t *attr_data)
{
    GENIE_LOG_INFO("type:%04x data:%04x\r\n", attr_data->type, attr_data->para);

    if (attr_data->type == ATTR_TYPE_GENERIC_ONOFF)
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        node_ctl_elem_stat[0].state.onoff[TYPE_TARGET] = attr_data->para;

        if (node_ctl_elem_stat[0].state.onoff[TYPE_PRESENT] != node_ctl_elem_stat[0].state.onoff[TYPE_TARGET])
        {
            node_ctl_set_led_status(node_ctl_elem_stat[0].state.onoff[TYPE_TARGET]);
            node_ctl_elem_stat[0].state.onoff[TYPE_PRESENT] = node_ctl_elem_stat[0].state.onoff[TYPE_TARGET];
        }
#endif
    }
}

static void node_ctl_param_reset(void)
{
    genie_storage_delete_userdata(GFI_MESH_POWERUP);
}

static void node_ctl_save_state(sig_model_element_state_t *p_elem)
{
    uint8_t *p_read = NULL;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    p_elem->powerup.last_onoff = p_elem->state.onoff[TYPE_PRESENT];
    node_ctl_powerup[p_elem->element_id].last_onoff = p_elem->state.onoff[TYPE_PRESENT];
#endif

    p_read = aos_malloc(sizeof(node_ctl_powerup));
    if (!p_read)
    {
        GENIE_LOG_WARN("no mem");
        return;
    }

    genie_storage_read_userdata(GFI_MESH_POWERUP, p_read, sizeof(node_ctl_powerup));

    if (memcmp(node_ctl_powerup, p_read, sizeof(node_ctl_powerup)))
    {
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
        GENIE_LOG_INFO("save %d %d %d", node_ctl_powerup[0].last_onoff);
#endif
        genie_storage_write_userdata(GFI_MESH_POWERUP, (uint8_t *)node_ctl_powerup, sizeof(node_ctl_powerup));
    }

    aos_free(p_read);
#ifdef CONFIG_GENIE_OTA
    if (node_ctl_powerup[0].last_onoff == 0 && genie_ota_is_ready() == 1)
    {
        //Means have ota, wait for reboot while light off
        GENIE_LOG_INFO("reboot by ota");
        aos_reboot();
    }
#endif
}

static void node_ctl_update(sig_model_element_state_t *p_elem)
{
#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    node_ctl_set_led_status(p_elem->state.onoff[TYPE_TARGET]);
#endif
}

static void node_ctl_report_poweron_state(sig_model_element_state_t *p_elem)
{
    uint16_t index = 0;
    uint8_t payload[20];
    genie_transport_payload_param_t transport_payload_param;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    payload[index++] = ATTR_TYPE_GENERIC_ONOFF & 0xff;
    payload[index++] = (ATTR_TYPE_GENERIC_ONOFF >> 8) & 0xff;
    payload[index++] = p_elem->state.onoff[TYPE_PRESENT];
#endif

    memset(&transport_payload_param, 0, sizeof(genie_transport_payload_param_t));
    transport_payload_param.opid = VENDOR_OP_ATTR_INDICATE;
    transport_payload_param.p_payload = payload;
    transport_payload_param.payload_len = index;
    transport_payload_param.retry_cnt = GENIE_TRANSPORT_DEFAULT_RETRY_COUNT;

    genie_transport_send_payload(&transport_payload_param);
}

static void node_ctl_event_handler(genie_event_e event, void *p_arg)
{
    switch (event)
    {
    case GENIE_EVT_SW_RESET:
    {
        node_ctl_param_reset();
    }
    break;
    case GENIE_EVT_MESH_READY:
    {
        //User can report data to cloud at here
        GENIE_LOG_INFO("User report data");
        node_ctl_report_poweron_state(&node_ctl_elem_stat[0]);
    }
    break;
    case GENIE_EVT_USER_ACTION_DONE:
    {
        sig_model_element_state_t *p_elem = (sig_model_element_state_t *)p_arg;
        node_ctl_update(p_elem);
        node_ctl_save_state(p_elem);
    }
    break;
    case GENIE_EVT_SIG_MODEL_MSG:
    {
        sig_model_msg *p_msg = (sig_model_msg *)p_arg;

        if (p_msg)
        {
            GENIE_LOG_INFO("SIG mesg ElemID(%d)", p_msg->element_id);
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
            (void)p_elem_state;
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
            GENIE_LOG_INFO("User cmd ElemID(%d)", p_msg->element_id);
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
            node_ctl_handle_order_msg(pdata);
        }
    }
    break;
#endif
    default:
        break;
    }
}

#ifdef CONFIG_PM_SLEEP
static void node_ctl_lpm_cb(genie_lpm_wakeup_reason_e reason, genie_lpm_status_e status, void* args)
{
    if (status == STATUS_WAKEUP)
    {
        GENIE_LOG_INFO("wakeup by %s", (reason == WAKEUP_BY_IO) ? "io" : "timer");
    }
    else
    {
        GENIE_LOG_INFO("sleep");
    }
}
#endif

static void led_gpio_init(void)
{
    led_gpio.port = NODE_CTL_LED_PIN;
    led_gpio.config = OUTPUT_PUSH_PULL;

    hal_gpio_init(&led_gpio);
}

int application_start(int argc, char **argv)
{
    genie_service_ctx_t context;

    GENIE_LOG_INFO("BTIME:%s\n", __DATE__ ","__TIME__);

    led_gpio_init();
    node_ctl_elem_state_init();

    memset(&context, 0, sizeof(genie_service_ctx_t));
    context.prov_timeout = MESH_PBADV_TIME;
    context.event_cb = node_ctl_event_handler;
    context.p_mesh_elem = node_ctl_elements;
    context.mesh_elem_counts = sizeof(node_ctl_elements) / sizeof(struct bt_mesh_elem);

#ifdef CONFIG_PM_SLEEP
    context.lpm_conf.is_auto_enable = 1;
    context.lpm_conf.lpm_wakeup_io = 14;

    context.lpm_conf.genie_lpm_cb = node_ctl_lpm_cb;

    //User can config sleep time and wakeup time when not config GLP
    context.lpm_conf.sleep_ms = 1000;
    context.lpm_conf.wakeup_ms = 30;
#endif

    genie_service_init(&context);

    //aos_loop_run();

    return 0;
}
