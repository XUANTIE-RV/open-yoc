/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <aos/kernel.h>
#include <aos/log.h>

#include <cis_api.h>
#include "cis_internals.h"
#include "iot_onenet_api.h"


extern int iot_onenet_on_discover_cb(uint32_t mid, uint16_t obj_id);
extern int iot_onenet_on_read_cb(uint32_t mid, uint16_t obj_id, uint16_t ins_id, uint16_t res_id);
extern int iot_onenet_on_write_cb(uint16_t obj_id, uint16_t ins_id, const cis_data_t *res_data, int count);
extern int iot_onenet_on_exec_cb(uint16_t obj_id, uint16_t ins_id, uint16_t res_id, const uint8_t *data, uint32_t len);
extern int iot_onenet_on_event_cb(cis_evt_t eid);

#define TAG "iotonetapi"
//183.230.40.40, no bootstrap
static uint8_t config_hex[] = {0x13, 0x00, 0x34,
                               0xf1, 0x00, 0x03,
                               0xf2, 0x00, 0x21, 0x05, 0x00/*mtu*/,
                               0x11/*Link&bind type*/,
                               0x00, 0x00/*vpn*/,
                               0x00, 0x00/*username*/,
                               0x00, 0x00/*password*/,
                               0x00, 0x0d/*host length*/,
                               0x31, 0x38, 0x33, 0x2e, 0x32, 0x33, 0x30, 0x2e, 0x34, 0x30,
                               0x2e, 0x34, 0x30, 0x00, 0x04, 0x4e, 0x55, 0x4c, 0x4c, 0xf3,
                               0x00, 0x0d, 0x00, 0xea, 0x04, 0x00,
                               0x00, 0x04, 0x4e, 0x55, 0x4c, 0x4c
                              };

static void *g_onenet_context = NULL;

static cis_time_t g_lifetime = 300;

static bool g_shutdown = true;

static aos_task_t g_onenet_task = { NULL };

static aos_sem_t client_opensem;

// /static slist_t obs_list_head = {0};

static int onet_deinit(void)
{
    cis_ret_t ret;

    if (g_onenet_context) {
        ret = cis_deinit(&g_onenet_context);

        if (ret != CIS_RET_OK) {
            LOGE(TAG, "deinit %d", ret);
            return ret;
        }

        g_onenet_context = NULL;
    }

    return 0;
}

static uint8_t get_coap_result_code(uint8_t  at_result_code)
{
    unsigned int index = 0;
    static const struct result_code_map code_map[] = {
        { 1,  CIS_COAP_205_CONTENT },
        { 2,  CIS_COAP_204_CHANGED},
        { 11, CIS_COAP_400_BAD_REQUEST },
        { 12, CIS_COAP_401_UNAUTHORIZED },
        { 13, CIS_COAP_404_NOT_FOUND },
        { 14, CIS_COAP_405_METHOD_NOT_ALLOWED },
        { 15, CIS_COAP_406_NOT_ACCEPTABLE }
    };

    for (; index < sizeof(code_map) / sizeof(struct result_code_map); index++) {
        if (code_map[index].at_result_code == at_result_code) {
            return code_map[index].coap_result_code;
        }
    }

    return CIS_COAP_503_SERVICE_UNAVAILABLE;
}

static int check_coap_result_code(int code, enum onenet_rsp_type type)
{
    int res = -1;

    switch (type) {
        case RSP_READ:
        case RSP_OBSERVE:
        case RSP_DISCOVER:
            if (code == 1 || code == 11 || code == 12 ||
                code == 13 || code == 14 || code == 15) {
                res = 0;
            }

            break;

        case RSP_WRITE:
        case RSP_EXECUTE:
        case RSP_SETPARAMS:
            if (code == 2 || code == 11 || code == 12
                || code == 13 || code == 14) {
                res = 0;
            }

            break;

        default:
            break;
    }

    return res;
}

static int onet_conf_init()
{
    int ret;
    uint8_t bs_flag = 0;

    if (bs_flag) { //need bootstrap
        config_hex[31] = 0x33;
        config_hex[32] = 0x39;
        config_hex[42] = 0x01;
    } else {
        config_hex[31] = 0x34;
        config_hex[32] = 0x30;
        config_hex[42] = 0x00;
    }

    ret = cis_init(&g_onenet_context, config_hex, sizeof(config_hex));

    if (ret != CIS_RET_OK) {
        LOGE(TAG, "cis init ret=%d", ret);
        return -1;
    }

    return 0;
}

int iot_onenet_miplcreate(void)
{
    if (g_onenet_context != NULL) {
        LOGE(TAG, "g_onenet_context");
        return -1;
    }

    if (onet_conf_init() != 0) {
        return -1;
    }

    return 0;
}

static int onet_have_context()
{
    //onenet only support single ref
    if (g_onenet_context == NULL) {
        return -1;
    }

    return 0;
}

int iot_onenet_mipldel(void)
{
    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context");
        return -1;
    }

    if (!g_shutdown) {
        LOGE(TAG, "open task running");
        return -1;
    }

    if (onet_deinit() != 0) {
        return -1;
    }

    return 0;
}

static cis_ret_t onet_mipladdobj_req(struct onenet_addobj *para)
{
    cis_ret_t ret = 0;
    cis_inst_bitmap_t bitmap = {0};
    uint8_t *instPtr;
    uint16_t instBytes;
    cis_res_count_t  rescount;
    int i;

    /*make bitmap */
    bitmap.instanceCount = para->ins_count;
    instBytes = (bitmap.instanceCount - 1) / 8 + 1;
    instPtr = (uint8_t *)aos_zalloc(instBytes);

    if (instPtr == NULL) {
        return CIS_RET_MEMORY_ERR;
    }

    for (i = 0; i < bitmap.instanceCount; i++) {
        uint8_t instBytePos = i / 8;
        uint8_t instByteOffset = 7 - (i % 8);

        if (*(para->ins_map + i) == '1') {
            instPtr[instBytePos] += 0x01 << instByteOffset;
        }
    }

    bitmap.instanceBitmap = instPtr;

    bitmap.instanceBytes = (para->ins_count - 1) / 8 + 1;
    //bitmap.attr_count = para->attr_count;

    rescount.attrCount = para->attr_count;
    rescount.actCount = para->action_count;

    ret = cis_addobject(g_onenet_context, para->obj_id, &bitmap, &rescount);

    free(instPtr);

    return ret;
}

int iot_onenet_mipladdobj(struct onenet_addobj *param)
{
    int ret;

    if (param->ins_count != strlen(param->ins_map)) {
        LOGE(TAG, "param");
        return -1;
    }

    ret = onet_have_context(param->ref);

    if (ret != CIS_RET_NO_ERROR) {
        LOGE(TAG, "onet_have_context %d", ret);
        return -1;
    }

    ret = onet_mipladdobj_req(param);

    if (ret != CIS_RET_NO_ERROR) {
        LOGE(TAG, "onet_mipladdobj_req %d", ret);
        return -1;
    }

    return 0;
}

int iot_onenet_mipldelobj(unsigned int obj_id)
{
    int ret;

    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context");
        return -1;
    }

    ret = cis_delobject(g_onenet_context, obj_id);

    if (ret != CIS_RET_NO_ERROR) {
        LOGE(TAG, "cis_delobject=%d", ret);
        return -1;
    }

    return 0;
}

static void onet_at_pump(void *param)
{
    g_shutdown = false;

    while (!g_shutdown) {
        if (g_onenet_context != NULL) {
            uint32_t pumpRet;
            int timeout = g_lifetime;

            /*pump function*/
            pumpRet = cis_pump(g_onenet_context, &timeout);

            if (pumpRet == PUMP_RET_NOSLEEP) {
                timeout = 1;
            }

            LOGD(TAG, "cis_pump %d", timeout);

            packet_read(g_onenet_context, timeout);

//            uint32_t nowtime = cissys_gettime();

#if 0

            if ((!cisnet_attached_state(g_onenet_context))
                && (nowtime - g_lifetime_last > g_lifetime - 10))) {
                if (cis_update_reg(g_onenet_context, LIFETIME_INVALID, false) == COAP_NO_ERROR) {
                    g_lifetime_last = utils_gettime_s();
                    //g_auto_update = true;
                }
            }

#endif
        }

        aos_msleep(100);
    }

    aos_sem_signal(&client_opensem);
    aos_task_exit(0);
}

static cis_coapret_t onet_on_read_cb(void *context, cis_uri_t *uri, cis_mid_t mid)
{
    uint16_t obj_id = uri->objectId;
    uint16_t ins_id = CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : 0xffff;
    uint16_t res_id = CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : 0xffff;

    iot_onenet_on_read_cb(mid, obj_id, ins_id, res_id);

    return CIS_CALLBACK_CONFORM;
}

//attention: onenet server only support cis_data_type_opaque
static cis_coapret_t onet_on_write_cb(void *context, cis_uri_t *uri, const cis_data_t *value,
                                      cis_attrcount_t attrcount, cis_mid_t mid)
{
    if (!CIS_URI_IS_SET_INSTANCE(uri)) {
        return CIS_CALLBACK_BAD_REQUEST;
    }

    LOGD(TAG, "write cb 0x%x %d/%d/? rc=%d", mid, uri->objectId, uri->instanceId, attrcount);
    iot_onenet_on_write_cb(uri->objectId, uri->instanceId, value, attrcount);

    /* write rsp */
    if (iot_onenet_miplchangedrsp(mid) != 0) {
        LOGE(TAG, "write rsp");
    }

    return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t onet_on_execute_cb(void *context, cis_uri_t *uri, const uint8_t *value,
                                        uint32_t length, cis_mid_t mid)
{

    uint16_t obj_id = uri->objectId;
    uint16_t ins_id = CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : 0xffff;
    uint16_t res_id = CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : 0xffff;

    iot_onenet_on_exec_cb(obj_id, ins_id, res_id, value, length);

    /* execute rsp */
    if (iot_onenet_miplchangedrsp(mid) != 0) {
        LOGE(TAG, "exec rsp");
    }

    return CIS_CALLBACK_CONFORM;
}

static cis_ret_t onet_on_observe_cb(void *context, cis_uri_t *uri, bool flag, cis_mid_t mid)
{
    int ret;

    /* flag:1 add obs, 0: cancel obs */
    LOGI(TAG, "obs rsp %d,%d,%d/%d/%d", mid, flag,
         uri->objectId, CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : 0xffff,
         CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : 0xffff);
#if 0
    uint16_t obj_id = uri->objectId;
    uint16_t ins_id = CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : 0xffff;
    uint16_t res_id = CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : 0xffff;

    if (flag != 0) {
        /* Add to obs list */
        struct onenet_obs_node *obs = aos_zalloc(sizeof(struct onenet_obs_node));
        obs->msg_id = mid;
        obs->obj_id = obj_id;
        obs->ins_id = ins_id;
        obs->res_id = res_id;
        slist_add(&obs->node, &obs_list_head);
    } else {
        /* Del obs from list */
        slist_t *tmp;
        struct onenet_obs_node *obs;
        slist_for_each_entry_safe(&obs_list_head, tmp, obs, struct onenet_obs_node, node) {
            if (obs->obj_id == obj_id) {
                if (ins_id == -1) {
                    /* del all by obj_id */
                    slist_del(&obs->node, &obs_list_head);
                } else if (obs->ins_id == ins_id) {
                    if (res_id == -1) {
                        /* del all by obj_id & ins_id */
                        slist_del(&obs->node, &obs_list_head);
                    } else if (obs->res_id == res_id) {
                        /* del by obj_id & ins_id & res_id */
                        slist_del(&obs->node, &obs_list_head);
                    }
                }
            }
        }
    }
#endif

    /* Auto observe respone */
    struct onenet_write_exe obsrsp;
    obsrsp.ref = 0;
    obsrsp.msg_id = mid;
    obsrsp.result = 1;

    ret = iot_onenet_miplobserveresp(obsrsp);

    if (ret != 0) {
        LOGE(TAG, "obs rsp");
    }

    return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t onet_on_discover_cb(void *context, cis_uri_t *uri, cis_mid_t mid)
{
    iot_onenet_on_discover_cb(mid, uri->objectId);

    return CIS_CALLBACK_CONFORM;
}

static cis_ret_t onet_on_parameter_cb(void *context, cis_uri_t *uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
    char *at_str = malloc(MAX_ONE_NET_AT_SIZE);

    if (at_str == NULL) {
        return CIS_CALLBACK_SERVICE_UNAVAILABLE;
    }

    char *str_params = aos_zalloc(MAX_SET_PARAM_SIZE);

    if (str_params == NULL) {
        free(at_str);
        return CIS_CALLBACK_SERVICE_UNAVAILABLE;
    }

    cis_iid_t ins_id = CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : 0xffff;
    cis_rid_t res_id = CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : 0xffff;

    if ((parameters.toSet & ATTR_FLAG_MIN_PERIOD) != 0) {
        snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), "pmin=%d",  parameters.minPeriod);
    }

    if ((parameters.toSet & ATTR_FLAG_MAX_PERIOD) != 0) {
        if (strlen(str_params) > 0) {
            snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), ";");
        }

        snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), "pmax=%d",  parameters.maxPeriod);
    }

    if ((parameters.toSet & ATTR_FLAG_LESS_THAN) != 0) {
        if (strlen(str_params) > 0) {
            snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), ";");
        }

        snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), "lt=%.1f",  parameters.lessThan);
    }

    if ((parameters.toSet & ATTR_FLAG_GREATER_THAN) != 0) {
        if (strlen(str_params) > 0) {
            snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), ";");
        }

        snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), "gt=%.1f",  parameters.greaterThan);
    }

    if ((parameters.toSet & ATTR_FLAG_STEP) != 0) {
        if (strlen(str_params) > 0) {
            snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), ";");
        }

        snprintf(str_params + strlen(str_params), MAX_SET_PARAM_SIZE - strlen(str_params), "stp=%.1f",  parameters.step);
    }

    LOGD(TAG, "[onet_on_parameter_cb]:%s", str_params);
    snprintf(at_str, MAX_SET_PARAM_AT_SIZE, "\r\n+MIPLPARAMETER: %d,%d,%d,%d,%d,%d,\"%s\"\r\n", 1, mid, uri->objectId, ins_id, res_id, strlen(str_params), str_params);

    /*TODO:*/
    LOGI(TAG, "%s", at_str);

    free(at_str);
    free(str_params);
    return CIS_CALLBACK_CONFORM;
}

static void onet_on_event_cb(void *context, cis_evt_t eid, void *param)
{
    int user_action = 0;

    LOGD(TAG, "cis_on_event id:%d\n", eid);

    switch (eid) {
        case CIS_EVENT_UPDATE_NEED:
            LOGD(TAG, "cis_on_event need to update,reserve time:%ds\n", (int32_t)param);
            cis_update_reg(g_onenet_context, g_lifetime, false);
            break;
        case CIS_EVENT_REG_SUCCESS:
            user_action = 1;
            break;
        default:
            user_action = 1;
    }

    if (user_action) {
        iot_onenet_on_event_cb(eid);
    }
}

static int onet_client_open(unsigned int liftime)
{
    if (g_onenet_task.hdl == NULL) {
        cis_callback_t callback;
        callback.onRead = (cis_read_callback_t)onet_on_read_cb;
        callback.onWrite = (cis_write_callback_t)onet_on_write_cb;
        callback.onExec = (cis_exec_callback_t)onet_on_execute_cb;
        callback.onObserve = (cis_observe_callback_t)onet_on_observe_cb;
        callback.onSetParams = (cis_set_params_callback_t)onet_on_parameter_cb;
        callback.onEvent = (cis_event_callback_t)onet_on_event_cb;
        callback.onDiscover = (cis_discover_callback_t)onet_on_discover_cb;

        if (cis_register(g_onenet_context, liftime, &callback) != CIS_RET_OK) {
            return -1;
        }

        return aos_task_new_ext(&g_onenet_task, "iot_onenet", onet_at_pump,
                                NULL, ONENET_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    } else {
        return -1;
    }
}

int iot_onenet_miplopen(unsigned int liftime)
{
    int ret;

    g_lifetime = liftime;

    if (onet_have_context() != 0) {
        LOGE(TAG, "context");
        return -1;
    }

    ret = aos_sem_new(&client_opensem, 0);

    if (ret != 0) {
        LOGE(TAG, "sem");
        return -1;
    }

    if (onet_client_open(g_lifetime) != 0) {
        aos_sem_free(&client_opensem);
        LOGE(TAG, "open");
        return -1;
    }

    return 0;
}

int iot_onenet_miplclose(void)
{
    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context");
        return -1;
    }

    if (g_onenet_task.hdl != NULL) {

        /* we shoud ensure that we do cis_unregister succeed */
        while (cis_isregister_enabled(g_onenet_context)) {
            cis_unregister(g_onenet_context);
            cisnet_break_recv(NULL, NULL);
            aos_msleep(100);
        }

        g_shutdown = true;
        aos_sem_wait(&client_opensem, -1);
        aos_sem_free(&client_opensem);
        g_onenet_task.hdl = NULL;
        return 0;
    }

    return -1;
}

static int onet_valuetype_check(int len, cis_datatype_t type)
{
    if (type == cis_data_type_integer) {
        if (len != 2 && len != 4 && len != 8) {
            return -1;
        }
    } else if (type == cis_data_type_float) {
        if (len != 4 && len != 8) {
            return -1;
        }
    } else if (type == cis_data_type_bool) {
        if (len != 1) {
            return -1;
        }
    } else if (type == cis_data_type_string) {
        if (len >= STR_VALUE_LEN) {
            return -1;
        }
    } else if (type == cis_data_type_opaque) {
        if ((len * 2) >= OPAQUE_VALUE_LEN) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

static cis_ret_t onet_read_param(struct onenet_notify_read *param, cis_data_t *dataP)
{
    dataP->type = param->value_type;
    dataP->id = param->res_id;

    if (dataP->type == cis_data_type_integer) {
        dataP->value.asInteger = *((int *)param->value);
    } else if (dataP->type == cis_data_type_float) {
        dataP->value.asFloat = *((float *)param->value);
    } else if (dataP->type == cis_data_type_bool) {
        dataP->value.asBoolean = *((bool *)param->value);
    } else if (dataP->type == cis_data_type_string) {
        dataP->asBuffer.length = param->len;
        dataP->asBuffer.buffer = (uint8_t *)(param->value);
    } else if (dataP->type == cis_data_type_opaque) {
        dataP->asBuffer.length = param->len;
        dataP->asBuffer.buffer = (uint8_t *)(param->value);
    } else {
        return CIS_CALLBACK_NOT_FOUND;
    }

    return CIS_RET_OK;
}


static cis_ret_t onet_notify_data(struct onenet_notify_read *param)
{
    cis_data_t tmpdata = {0};
    cis_uri_t uri = {0};
    cis_ret_t ret = 0;
    cis_coapret_t result = CIS_NOTIFY_CONTINUE;

    if (param->ackid != 0 && param->ackid != 1) {
        return CIS_RET_PARAMETER_ERR;
    }

    uri.objectId = param->obj_id;
    uri.instanceId = param->ins_id;
    uri.resourceId = param->res_id;
    cis_uri_update(&uri);

    if ((ret = onet_read_param(param, &tmpdata)) != CIS_RET_OK) {
        return ret;
    }

    if (param->last == 1) {
        result = CIS_NOTIFY_CONTENT;
    }

    ret = cis_notify(g_onenet_context, &uri, &tmpdata, param->msg_id, result, param->ackid);

    return ret;
}

static cis_ret_t onet_miplnotify_req(struct onenet_notify_read *param)
{
    cis_ret_t ret = CIS_RET_INVILID;

    st_object_t *targetP = (st_object_t *)CIS_LIST_FIND(((st_context_t *)g_onenet_context)->objectList, param->obj_id);

    if (targetP == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    ret = onet_notify_data(param);

    return ret;
}

int iot_onenet_miplnotify(struct onenet_notify_read *param)
{
    int ret;

    if (param == NULL) {
        return -EINVAL;
    }

    if (onet_valuetype_check(param->len, param->value_type) != 0) {
        LOGE(TAG, "type %d, %d", param->len, param->value_type);
        return -EINVAL;
    }

    if (param->value_type == cis_data_type_string && param->len != strlen(param->value)) {
        return -EINVAL;
    }

    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context");
        return -1;
    }

    ret = onet_miplnotify_req(param);

    if (ret != CIS_RET_OK && ret != COAP_205_CONTENT) {
        return -1;
    }

    return 0;
}

static cis_ret_t onet_read_data(struct onenet_notify_read *param)
{
    cis_data_t tmpdata = {0};
    cis_uri_t uri = {0};
    cis_ret_t ret = 0;

    uri.objectId = param->obj_id;
    uri.instanceId = param->ins_id;
    uri.resourceId = param->res_id;
    cis_uri_update(&uri);

    if ((ret = onet_read_param(param, &tmpdata)) != CIS_RET_OK) {
        return ret;
    }

    if (param->last == 1) {
        ret = cis_response(g_onenet_context, NULL, NULL, param->msg_id, CIS_RESPONSE_READ);
    } else {
        ret = cis_response(g_onenet_context, &uri, &tmpdata, param->msg_id, CIS_RESPONSE_CONTINUE);
    }

    return ret;
}

static cis_ret_t onet_miplread_req(struct onenet_notify_read *param)
{
    cis_ret_t ret = CIS_RET_INVILID;

    st_object_t *targetP = (st_object_t *)CIS_LIST_FIND(((st_context_t *)g_onenet_context)->objectList, param->obj_id);

    if (targetP == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    ret = onet_read_data(param);

    return ret;
}

int iot_onenet_miplreadrsp(struct onenet_notify_read *param)
{
    int ret;

    if (param == NULL) {
        return -EINVAL;
    }

    if (onet_valuetype_check(param->len, param->value_type) != 0) {
        LOGE(TAG, "type");
        return -EINVAL;
    }

    if ((param->value_type == cis_data_type_string) && (param->len != strlen(param->value))) {
        LOGE(TAG, "len");
        return -EINVAL;
    }

    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context");
        return -EINVAL;
    }

    ret = onet_miplread_req(param);

    if (ret != CIS_RET_OK) {
        LOGE(TAG, "onet_miplread_req ret=%d!", ret);
        return -1;
    }

    return 0;
}

int iot_onenet_miplchangedrsp(uint32_t mid)
{
    int ret;

    if (onet_have_context() != 0) {
        LOGE(TAG, "onet_have_context err");
        return -1;
    }

    /* must be CIS_COAP_204_CHANGED*/
    ret = cis_response(g_onenet_context, NULL, NULL, mid, CIS_COAP_204_CHANGED);

    if (ret != COAP_NO_ERROR) {
        LOGE(TAG, "write rsp err");
        return -1;
    }

    return 0;
}

int iot_onenet_miplobserveresp(struct onenet_write_exe param)
{
    int ret;

    if (onet_have_context(param.ref) != 0) {
        LOGE(TAG, "onet_have_context err");
        return -1;
    }

    if (0 != check_coap_result_code(param.result, RSP_OBSERVE)) {
        LOGE(TAG, "result");
        return -1;
    }

    ret = cis_response(g_onenet_context, NULL, NULL, param.msg_id,
                       get_coap_result_code(param.result));

    if (ret != CIS_RET_OK) {
        LOGE(TAG, "cis_notify err");
        return -1;
    }

    return 0;
}

int iot_onenet_miplparameterresp(struct onenet_parameter_rsp param)
{
    if (onet_have_context(param.ref) != 0) {
        LOGE(TAG, "ref");
        return -1;
    }

    if (0 != check_coap_result_code(param.result, RSP_SETPARAMS)) {
        LOGE(TAG, "result");
        return -1;
    }

    if (cis_response(g_onenet_context, NULL, NULL, param.msg_id,
                     get_coap_result_code(param.result)) != CIS_RET_OK) {
        LOGE(TAG, "response");
        return -1;
    }

    return 0;
}

int iot_onenet_mipldiscoverresp(int mid, uint8_t result, int *atts, int count)
{
    int i;

    if (onet_have_context() != 0) {
        LOGE(TAG, "ref");
        return -1;
    }

    if (0 != check_coap_result_code(result, RSP_DISCOVER)) {
        LOGE(TAG, "result");
        return -1;
    }

    if (result == 1) {
        for (i = 0; i < count; i++) {
            cis_uri_t uri = {0};
            uri.objectId = URI_INVALID;
            uri.instanceId = URI_INVALID;
            uri.resourceId = atts[i];
            cis_uri_update(&uri);
            cis_response(g_onenet_context, &uri, NULL, mid, CIS_RESPONSE_CONTINUE);
        }
    }

    if (cis_response(g_onenet_context, NULL, NULL, mid,
                     get_coap_result_code(result)) != CIS_RET_OK) {
        LOGE(TAG, "response");
        return -1;
    }

    return 0;
}

#if 0
int iot_onenet_miplupdate(struct onenet_update param)
{
    if (onet_have_context() != 0) {
        LOGE(TAG, "ref");
        return -1;
    }

    if (param.lifetime == 0) {
        param.lifetime = LIFETIME_INVALID;
    } else if (param.lifetime >= LIFETIME_LIMIT_MIN) {
        g_lifetime = param.lifetime;
    } else {
        LOGE(TAG, "lifetime");
        return -1;
    }

    if (!(param.with_obj_flag == 0 || param.with_obj_flag == 1)) {
        LOGE(TAG, "obj_flag");
        return -1;
    }

    if (cis_update_reg(g_onenet_context, param.lifetime, param.with_obj_flag) != COAP_NO_ERROR) {
        LOGE(TAG, "reg");
        return -1;
    }

    return 0;
}
#endif


char *iot_onenet_miplver(void)
{
    cis_version_t ver;
    static char at_str[8] = {0};

    if (cis_version(&ver) != CIS_RET_OK) {
        return NULL;
    }

    snprintf(at_str, sizeof(at_str), "%x.%x",
             ver.major, ver.minor);
    return at_str;
}

#if 0
int iot_onenet_get_notify_mid(uint16_t obj_id, uint16_t ins_id, uint32_t *ptr_mid)
{
    int ret = -1;

    slist_t *tmp;
    struct onenet_obs_node *obs;
    slist_for_each_entry_safe(&obs_list_head, tmp, obs, struct onenet_obs_node, node) {
        if (obs->obj_id == obj_id && obs->ins_id == ins_id) {
            ret = 0;
            *ptr_mid = obs->msg_id;
            break;
        }
    }

    return ret;
}
#endif


int iot_onenet_get_notify_mid(uint16_t obj_id, uint16_t ins_id, uint32_t *ptr_mid)
{
    st_observed_t *targetP;

    if (onet_have_context() != 0) {
        LOGE(TAG, "ref");
        return -1;
    }

    targetP = ((st_context_t*)g_onenet_context)->observedList;

    while (targetP != NULL) {

        if (targetP->uri.objectId == obj_id && targetP->uri.instanceId == ins_id) {
            *ptr_mid = targetP->msgid;
            return 0;
        }  

        targetP = targetP->next;
    }

    return -1;
}
