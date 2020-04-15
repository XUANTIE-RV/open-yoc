/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Baijie & Longrong, China Mobile - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/
#include "cis_internals.h"
#include "cis_api.h"
#include "cis_if_sys.h"
#include "cis_if_net.h"
#include "std_object/std_object.h"
#include "cis_log.h"
//#include "led.h"

#if CIS_ENABLE_MEMORYTRACE
static cis_time_t g_tracetime = 0;
#endif//CIS_ENABLE_MEMORY_TRACE


//private function.
static void     prv_localDeinit(st_context_t **context);
static cis_data_t *prv_dataDup(const cis_data_t *src);
static st_object_t *prv_findObject(st_context_t *context, cis_oid_t objectid);
static void     prv_deleteTransactionList(st_context_t *context);
static void     prv_deleteObservedList(st_context_t *context);
static void     prv_deleteRequestList(st_context_t *context);
static void     prv_deleteNotifyList(st_context_t *context);
static void     prv_deleteObjectList(st_context_t *context);
static void     prv_deleteServer(st_context_t *context);

static cis_ret_t prv_onNetEventHandler(cisnet_t netctx, cisnet_event_t id, void *param, void *userData);
int       prv_makeDeviceName(char **name);

cis_ret_t    cis_uri_make(cis_oid_t oid, cis_iid_t iid, cis_rid_t rid, cis_uri_t *uri)
{
    if (uri == NULL) {
        return CIS_RET_ERROR;
    }

    cis_memset(uri, 0, sizeof(st_uri_t));
    uri->objectId = 0;
    uri->instanceId = 0;
    uri->resourceId = 0;

    if (oid > 0 && oid <= URI_MAX_ID) {
        uri->objectId = (cis_oid_t)oid;
        uri->flag |= URI_FLAG_OBJECT_ID;
    }

    if (iid >= 0 && iid <= URI_MAX_ID) {
        uri->instanceId = (cis_iid_t)iid;
        uri->flag |= URI_FLAG_INSTANCE_ID;
    }

    if (rid > 0 && rid <= URI_MAX_ID) {
        uri->resourceId = (cis_rid_t)rid;
        uri->flag |= URI_FLAG_RESOURCE_ID;
    }

    return CIS_RET_OK;
}

cis_ret_t    cis_uri_update(cis_uri_t *uri)
{
    return cis_uri_make(uri->objectId, uri->instanceId, uri->resourceId, uri);
}

cis_ret_t    cis_version(cis_version_t *version)
{
    if (version == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    version->major = CIS_VERSION_MAJOR;
    version->minor = CIS_VERSION_MINOR;
    version->micro = CIS_VERSION_MICRO;
    return CIS_RET_OK;
}

cis_ret_t  cis_init(void **context, void *config, uint16_t configLen)
{
    cis_cfgret_t configRet;
    cisnet_callback_t netCallback;
    st_context_t *contextP;
    char *targetServerHost;
    char *pDeviceName;
    cisnet_config_t netConfig;
    st_serialize_t serialize;

    if (*context != NULL) {
        return CIS_RET_EXIST;
    }

    CIS_LOGD("api cis_init");

    contextP = (st_context_t *)cis_malloc(sizeof(st_context_t));

    if (NULL == contextP) {
        return CIS_RET_ERROR;
    }

    cis_memset(contextP, 0, sizeof(st_context_t));
    contextP->nextMid = (uint16_t)cissys_rand();

    //deal with configuration data
    if (cis_config_init(contextP, config, configLen) < 0) {
        cis_free(contextP);
        return CIS_RET_ERROR;
    }

    CIS_LOGD("----------------\n");
    CIS_LOGD("DEBUG CONFIG INIT INFORMATION.");

    cissys_lockcreate(&contextP->lockRequest);
    cissys_lockcreate(&contextP->lockNotify);

    contextP->nextObserveNum = 0;

    /* Get sys parameter and init */
    cis_config_get(contextP, cis_cfgid_sys, &configRet);
    if (cissys_init(configRet.data.cfg_sys) < 0) {
        cis_free(contextP);
        return CIS_RET_ERROR;
    }

    /*Get net parameter and create net*/
    netCallback.onEvent = prv_onNetEventHandler;
    netCallback.userData = contextP;

    cis_config_get(contextP, cis_cfgid_net, &configRet);

    netConfig.mtu = configRet.data.cfg_net->mtu;
    netConfig.data = configRet.data.cfg_net->user_data.data;
    netConfig.datalen = configRet.data.cfg_net->user_data.len;
    cisnet_init(contextP, &netConfig, netCallback);
    targetServerHost = (char *)configRet.data.cfg_net->host.data;

    if (targetServerHost == NULL || strlen(targetServerHost) <= 0) {
        cis_free(contextP);
        return CIS_RET_ERROR;
    }

    memcpy(contextP->platform_server, targetServerHost, strlen(targetServerHost)); //Fix for Dm

    cis_addobject(contextP, std_object_security, NULL, NULL);

    st_object_t *securityObj = prv_findObject(contextP, std_object_security);

    if (securityObj == NULL) {
        CIS_LOGE("ERROR:Failed to init security object");
        cis_free(contextP);
        return CIS_RET_ERROR;
    }

    if (prv_makeDeviceName(&pDeviceName) <= 0) {
        CIS_LOGE("ERROR:Get device name error from IMEI/IMSI.");
        cis_free(contextP);

        if (pDeviceName != NULL) {
            cis_free(pDeviceName);
        }

        return CIS_RET_ERROR;
    }

    contextP->endpointName = (char *)pDeviceName;
    CIS_LOGD("INIT:endpoint name: %s", contextP->endpointName);

    //load serialize from firmware memory
    cissys_load((uint8_t *)&serialize, sizeof(serialize));


    if (contextP->prv_cfg_sys.bootstrap_enabled) {
        std_security_create(contextP, 0, "", false, securityObj);
        std_security_create(contextP, 1, targetServerHost, true, securityObj);
    } else {
        std_security_create(contextP, 0, targetServerHost, false, securityObj);
    }

    contextP->stateStep = PUMP_STATE_INITIAL;
    (*context) = contextP;
    return CIS_RET_OK;
}

cis_ret_t    cis_deinit(void **context)
{
    st_context_t *ctx = *(st_context_t **)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    if (ctx->registerEnabled) {
        return CIS_RET_PENDING;
    }

    CIS_LOGD("api cis_deinit");

    cisnet_deinit(ctx);
    prv_localDeinit((st_context_t **)context);

    return CIS_RET_OK;
}

cis_ret_t    cis_isregister_enabled(void *context)
{
    st_context_t *ctx = (st_context_t *)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    if (ctx->registerEnabled) {
        return true;
    }

    return false;
}

cis_ret_t    cis_register(void *context, cis_time_t lifetime, const cis_callback_t *cb)
{
    st_context_t *ctx = (st_context_t *)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    CIS_LOGD("api cis_register");

    if (cb->onEvent == NULL || cb->onExec == NULL  || cb->onObserve == NULL ||
        cb->onRead == NULL || cb->onSetParams == NULL || cb->onWrite == NULL ||
        cb->onDiscover == NULL) {
        CIS_LOGE("ERROR:cis_register request failed.invalid parameters");
        return CIS_RET_ERROR;
    }

    if (lifetime < LIFETIME_LIMIT_MIN ||
        lifetime > LIFETIME_LIMIT_MAX) {
        CIS_LOGE("ERROR:invalid lifetime parameter");
        return CIS_RET_ERROR;
    }

    ctx->lifetime = lifetime;
    ctx->registerEnabled = true;
    cis_memcpy(&ctx->callback, cb, sizeof(cis_callback_t));
    return CIS_RET_OK;
}

cis_ret_t    cis_unregister(void *context)
{
    st_context_t *ctx = (st_context_t *)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    CIS_LOGD("api cis_unregister");
    core_updatePumpState(ctx, PUMP_STATE_UNREGISTER);

    cisnet_break_recv(ctx, ctx->server->sessionH);
    return CIS_RET_OK;
}



cis_ret_t    cis_addobject(void *context, cis_oid_t objectid, const cis_inst_bitmap_t *bitmap, const cis_res_count_t *resource)
{
    st_object_t *targetP;
    uint16_t index;
    cis_instcount_t instValidCount;
    cis_instcount_t instCount;
    cis_instcount_t instBytes;
    uint8_t *instPtr;
    bool noneBitmap;

    st_context_t *ctx = (st_context_t *)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    CIS_LOGD("api cis_addobject");

    if (bitmap == NULL || bitmap->instanceBitmap == NULL) {
        instCount = 1;
        instBytes = 1;
        noneBitmap = true;
    } else {
        noneBitmap = false;
        instCount = bitmap->instanceCount;
        instBytes = bitmap->instanceBytes;

        if (instBytes <= 0 || instCount <= 0) {
            return CIS_RET_ERROR;
        }

        if (instBytes != ((instCount - 1) / 8) + 1) {
            return CIS_RET_ERROR;
        }

    }


    targetP = (st_object_t *)CIS_LIST_FIND(ctx->objectList, objectid);

    if (targetP != NULL) {
        return CIS_RET_EXIST;
    }

    targetP = (st_object_t *)cis_malloc(sizeof(st_object_t));

    if (targetP == NULL) {
        return CIS_RET_MEMORY_ERR;
    }

    cis_memset(targetP, 0, sizeof(st_object_t));
    targetP->objID = objectid;

    instPtr = (uint8_t *)cis_malloc(instBytes);

    if (instPtr == NULL) {
        cis_free(targetP);
        return CIS_RET_MEMORY_ERR;
    }

    cis_memset(instPtr, 0, instBytes);

    if (noneBitmap) {
        instPtr[0] = 0x80;
    } else {
        cis_memcpy(instPtr, bitmap->instanceBitmap, instBytes);
    }

    instValidCount = 0;

    for (index = 0; index < instCount; index++) {
        if (object_checkInstExisted(instPtr, index)) {
            instValidCount++;
        }
    }

    if (instValidCount == 0) {
        CIS_LOGD("Error:instance bitmap invalid.");
        cis_free(targetP);
        cis_free(instPtr);
        return CIS_RET_ERROR;
    }

    targetP->instBitmapBytes = instBytes;
    targetP->instBitmapPtr = instPtr;
    targetP->instBitmapCount = instCount;

    if (resource != NULL) {
        targetP->attributeCount = resource->attrCount;
        targetP->actionCount = resource->actCount;
    }

    targetP->instValidCount = instValidCount;
    ctx->objectList = (st_object_t *)CIS_LIST_ADD(ctx->objectList, targetP);

    return CIS_RET_NO_ERROR;

}


cis_ret_t    cis_delobject(void *context, cis_oid_t objectid)
{
    st_object_t *targetP;
    st_context_t *ctx = (st_context_t *)context;
    cissys_assert(context != NULL);

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    CIS_LOGD("api cis_delobject");
    ctx->objectList = (st_object_t *)CIS_LIST_RM(ctx->objectList, objectid, &targetP);

    if (targetP == NULL) {
        return COAP_404_NOT_FOUND;
    }

    if (targetP->instBitmapPtr != NULL) {
        cis_free(targetP->instBitmapPtr);
    }

    cis_free(targetP);

    return CIS_RET_NO_ERROR;

}

uint32_t    cis_pump(void *context, int *timeoutP)
{
    uint32_t notifyCount;
    uint32_t requestCount;
    //bool readHasData;
    cis_time_t tv_sec;
    st_context_t *ctx = (st_context_t *)context;

    cissys_assert(context != NULL);
    tv_sec = utils_gettime_s();

    if (tv_sec <= 0) {
        return PUMP_RET_CUSTOM;
    }

#if CIS_ENABLE_MEMORYTRACE

    if (tv_sec - g_tracetime > CIS_CONFIG_MEMORYTRACE_TIMEOUT) {
        g_tracetime = tv_sec;
        int block, i;
        size_t size;
        trace_status(&block, &size);

        for (i = 0; i < block; i++) {
            trace_print(i, 1);
        }
    }

#endif//CIS_ENABLE_MEMORY_TRACE

    if (!ctx->registerEnabled) {
        return PUMP_RET_CUSTOM;
    }

    if (!cisnet_attached_state(ctx)) {
        return PUMP_RET_CUSTOM;
    }

    switch (ctx->stateStep) {
        case PUMP_STATE_HALT: {
            core_callbackEvent(ctx, CIS_EVENT_STATUS_HALT, NULL);
            prv_localDeinit(&ctx);
            cissys_fault(0);
            return PUMP_RET_CUSTOM;
        }
        break;

        case PUMP_STATE_INITIAL: {
            core_updatePumpState(ctx, PUMP_STATE_BOOTSTRAPPING);
            return PUMP_RET_NOSLEEP;
        }
        break;

        case PUMP_STATE_BOOTSTRAPPING: {

            if (! ctx->prv_cfg_sys.bootstrap_enabled) {
                CIS_LOGD("CIS_ENABLE_BOOTSTRAP is disabled");
                core_updatePumpState(ctx, PUMP_STATE_CONNECTING);
                return PUMP_RET_NOSLEEP;
            } else {
                if (ctx->bootstrapServer == NULL) {
                    core_callbackEvent(ctx, CIS_EVENT_BOOTSTRAP_START, NULL);
                    bootstrap_init(ctx);

                    if (ctx->bootstrapServer == NULL) {
                        return PUMP_RET_CUSTOM;
                    }
                }

                switch (bootstrap_getStatus(ctx)) {
                    case STATE_UNCREATED: {
                        ctx->lasttime = tv_sec;
                        bootstrap_create(ctx);
                        return PUMP_RET_NOSLEEP;
                    }
                    break;

                    case STATE_CREATED: {
                        ctx->lasttime = tv_sec;
                        bootstrap_connect(ctx);
                        return PUMP_RET_NOSLEEP;
                    }
                    break;

                    case STATE_CONNECT_PENDING: {
                        if (ctx->lasttime == 0 || tv_sec - ctx->lasttime > CIS_CONFIG_CONNECT_RETRY_TIME) {
                            ctx->lasttime = tv_sec;
                            bootstrap_connect(ctx);
                        }

                        return PUMP_RET_NOSLEEP;
                    }
                    break;

                    case STATE_BS_INITIATED: {
                        //wait;
                    }
                    break;

                    case STATE_BS_PENDING: {
                        if (ctx->lasttime == 0 || tv_sec - ctx->lasttime > CIS_CONFIG_BOOTSTRAP_TIMEOUT) {
                            ctx->lasttime = tv_sec;
                            CIS_LOGD("bootstrap pending timeout.");

                            if (ctx->bootstrapServer != NULL) {
                                ctx->bootstrapServer->status = STATE_BS_FAILED;
                            }
                        }
                    }
                    break;

                    case STATE_BS_FINISHED: {
                        CIS_LOGI("Bootstrap finish.");
                        bootstrap_destory(ctx);
                        core_updatePumpState(ctx, PUMP_STATE_CONNECTING);
                        core_callbackEvent(ctx, CIS_EVENT_BOOTSTRAP_SUCCESS, NULL);
                        return PUMP_RET_NOSLEEP;
                    }
                    break;

                    case STATE_BS_FAILED: {
                        CIS_LOGE("Bootstrap failed.");
                        bootstrap_destory(ctx);
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        core_callbackEvent(ctx, CIS_EVENT_BOOTSTRAP_FAILED, NULL);
                        return PUMP_RET_CUSTOM;
                    }
                    break;

                    default: {
                        bootstrap_step(ctx, tv_sec);
                    }
                    break;
                }
            }//#endif//CIS_ENABLE_BOOTSTRAP
        }
        break;

        case PUMP_STATE_CONNECTING: {
            if (ctx->server == NULL) {
                ctx->server = management_makeServerList(ctx, false);

                if (ctx->server == NULL) {
                    CIS_LOGE("ERROR:makeServer failed.");
                    core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                    return PUMP_RET_CUSTOM;
                }
            }

            switch (ctx->server->status) {
                case STATE_UNCREATED: {
                    if (ctx->server == NULL) {
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        return PUMP_RET_CUSTOM;
                    }

                    ctx->lasttime = tv_sec;

                    if (!management_createNetwork(ctx, ctx->server)) {
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        return PUMP_RET_CUSTOM;
                    }

                    return PUMP_RET_NOSLEEP;
                }
                break;

                case STATE_CREATED: {
                    if (ctx->server == NULL || ctx->server->sessionH == NULL) {
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        return PUMP_RET_CUSTOM;
                    }

                    ctx->lasttime = tv_sec;

                    if (!management_connectServer(ctx, ctx->server)) {
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        return PUMP_RET_CUSTOM;
                    }

                    return PUMP_RET_NOSLEEP;
                }
                break;

                case STATE_CONNECT_PENDING: {
                    if (ctx->server == NULL || ctx->server->sessionH == NULL) {
                        core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                        return PUMP_RET_CUSTOM;
                    }

                    if (ctx->lasttime == 0 || tv_sec - ctx->lasttime > CIS_CONFIG_CONNECT_RETRY_TIME) {
                        ctx->lasttime = tv_sec;

                        if (!management_connectServer(ctx, ctx->server)) {
                            core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                            return PUMP_RET_CUSTOM;
                        }
                    }

                    return PUMP_RET_NOSLEEP;
                }
                break;

                case STATE_CONNECTED: {
                    ctx->lasttime = 0;
                    core_updatePumpState(ctx, PUMP_STATE_REGISTER_REQUIRED);
                    core_callbackEvent(ctx, CIS_EVENT_CONNECT_SUCCESS, NULL);
                    return PUMP_RET_NOSLEEP;
                }
                break;

                case STATE_CONNECT_FAILED: {
                    CIS_LOGE("server connect failed.");
                    prv_deleteServer(ctx);
                    core_updatePumpState(ctx, PUMP_STATE_INITIAL);
                    core_callbackEvent(ctx, CIS_EVENT_CONNECT_FAILED, NULL);
                    return PUMP_RET_CUSTOM;
                }
                break;

                case STATE_CREATE_FAILED: {
                    CIS_LOGE("server create failed.");
                }
                break;

                default :
                    break;
            }
        }
        break;

        case PUMP_STATE_DISCONNECTED: {
            prv_deleteServer(ctx);
            prv_deleteObservedList(ctx);
            prv_deleteTransactionList(ctx);
            prv_deleteRequestList(ctx);
            prv_deleteNotifyList(ctx);

            core_updatePumpState(ctx, PUMP_STATE_INITIAL);
            return PUMP_RET_NOSLEEP;
        }
        break;

        case PUMP_STATE_UNREGISTER: {
            registration_deregister(ctx);

            prv_deleteServer(ctx);
            prv_deleteObservedList(ctx);
            prv_deleteTransactionList(ctx);
            prv_deleteRequestList(ctx);
            prv_deleteNotifyList(ctx);

            ctx->registerEnabled = false;
            core_updatePumpState(ctx, PUMP_STATE_INITIAL);
            core_callbackEvent(ctx, CIS_EVENT_UNREG_DONE, NULL);
            return PUMP_RET_NOSLEEP;
        }
        break;

        case PUMP_STATE_REGISTER_REQUIRED: { //from waiting connection to here
            //Led4_Set(LED_OFF);
            if (ctx->lasttime == 0 || tv_sec - ctx->lasttime > CIS_CONFIG_REG_INTERVAL_TIME) {
                uint32_t result;
                result = registration_start(ctx);

                if (COAP_NO_ERROR != result) {
                    return PUMP_RET_CUSTOM;
                }

                core_updatePumpState(ctx, PUMP_STATE_REGISTERING);
                ctx->lasttime = tv_sec;
            }
        }
        break;

        case PUMP_STATE_REGISTERING: {
            switch (registration_getStatus(ctx)) {
                case STATE_REGISTERED: {
                    st_serialize_t serialize = {0};
                    ctx->lasttime = 0;
                    core_updatePumpState(ctx, PUMP_STATE_READY);

                    serialize.size = sizeof(serialize);
                    utils_stringCopy((char *)&serialize.host, sizeof(serialize.host), ctx->server->host);
                    cissys_save((uint8_t *)&serialize, sizeof(serialize));

                    return PUMP_RET_NOSLEEP;
                }
                break;

                case STATE_REG_FAILED: {
                    core_updatePumpState(ctx, PUMP_STATE_DISCONNECTED);
                    return PUMP_RET_CUSTOM;
                }
                break;

                case STATE_REG_PENDING:
                default:
                    // keep on waiting
                    break;
            }
        }
        break;

        case PUMP_STATE_READY: {
            if (registration_getStatus(ctx) == STATE_REG_FAILED) {
                CIS_LOGE("ERROR:pump got STATE_REG_FAILED");
                core_updatePumpState(ctx, PUMP_STATE_DISCONNECTED);
                return PUMP_RET_NOSLEEP;
            }

            //Led4_Set(LED_ON);
        }
        break;

        default: {
            //
        }
        break;
    }

    registration_step(ctx, tv_sec, timeoutP);
    transaction_step(ctx, tv_sec, timeoutP);
    packet_step(ctx, tv_sec);

    //readHasData = packet_read(ctx);

    //if (readHasData || ctx->stateStep != PUMP_STATE_READY || transaction_count(ctx) != 0) {
    //if (ctx->stateStep != PUMP_STATE_READY || transaction_count(ctx) != 0) {
       // return PUMP_RET_NOSLEEP;
    //}

    //if (tv_sec - ctx->lasttime < 1) {
    //    return PUMP_RET_NOSLEEP;
    //}

    cissys_lock(ctx->lockNotify, CIS_CONFIG_LOCK_INFINITY);
    notifyCount = CIS_LIST_COUNT((cis_list_t *)ctx->notifyList);
    cissys_unlock(ctx->lockNotify);

    cissys_lock(ctx->lockRequest, CIS_CONFIG_LOCK_INFINITY);
    requestCount = CIS_LIST_COUNT((cis_list_t *)ctx->requestList);
    cissys_unlock(ctx->lockRequest);

    if (requestCount != 0 || notifyCount != 0) {
        return PUMP_RET_NOSLEEP;
    }

    ctx->lasttime = tv_sec;
    return PUMP_RET_CUSTOM;
}



cis_ret_t    cis_update_reg(void *context, cis_time_t lifetime, bool withObjects)
{
    cissys_assert(context != NULL);
    st_context_t *ctx = (st_context_t *)context;

    if (ctx->stateStep != PUMP_STATE_READY) {
        return CIS_RET_INVILID;
    }

    if (lifetime != LIFETIME_INVALID &&
        lifetime > LIFETIME_LIMIT_MIN && lifetime < LIFETIME_LIMIT_MAX) {
        ctx->lifetime = lifetime;
    }

    return registration_update_registration(ctx, withObjects);
}


CIS_API cis_ret_t    cis_response(void *context, const cis_uri_t *uri, const cis_data_t *value, cis_mid_t mid, cis_coapret_t result)
{
    st_context_t *ctx = (st_context_t *)context;
    st_notify_t *notify = NULL;

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }

    if (ctx->stateStep != PUMP_STATE_READY) {
        return CIS_RET_INVILID;
    }

    if (result < CIS_COAP_204_CHANGED || result > CIS_COAP_503_SERVICE_UNAVAILABLE) {
        return CIS_RET_ERROR;
    }


    notify = (st_notify_t *)cis_malloc(sizeof(st_notify_t));
    cissys_assert(notify != NULL);

    if (notify == NULL) {
        return CIS_RET_MEMORY_ERR;
    }

    notify->isResponse = true;
    notify->next = NULL;
    notify->id = ++ctx->nextNotifyId;
    notify->mid = mid;
    notify->result = result;
    notify->value = NULL;

    if (value != NULL) {
        notify->value = prv_dataDup(value);

        if (notify->value == NULL) {
            cis_free(notify);
            return CIS_RET_MEMORY_ERR;
        }
    }

    if (uri != NULL) {
        notify->uri = *uri;
        CIS_LOGD("cis_response add index:%d mid:0x%x [%d/%d/%d],result:(%s)", ctx->nextNotifyId, mid, uri->objectId, uri->instanceId, uri->resourceId, STR_COAP_CODE(result));
    } else {
        cis_uri_make(URI_INVALID, URI_INVALID, URI_INVALID, &notify->uri);
        CIS_LOGD("cis_response add index:%d mid:0x%x ,result:[%s]", ctx->nextNotifyId, mid, STR_COAP_CODE(result));
    }

    cissys_lock(ctx->lockNotify, CIS_CONFIG_LOCK_INFINITY);
    ctx->notifyList = (st_notify_t *)CIS_LIST_ADD(ctx->notifyList, notify);
    cissys_unlock(ctx->lockNotify);

    //cisnet_break_recv(ctx, ctx->server->sessionH);
    return CIS_RET_OK;
}

CIS_API cis_ret_t    cis_notify(void *context, const cis_uri_t *uri, const cis_data_t *value, cis_mid_t mid, cis_coapret_t result, bool needAck)
{
    st_context_t *ctx = (st_context_t *)context;
    st_notify_t *notify = NULL;

    if (context == NULL) {
        return CIS_RET_PARAMETER_ERR;
    }


    if (ctx->stateStep != PUMP_STATE_READY) {
        return CIS_RET_INVILID;
    }

    if (result != CIS_NOTIFY_CONTENT && result != CIS_NOTIFY_CONTINUE) {
        return CIS_RET_PARAMETER_ERR;
    }


    notify = (st_notify_t *)cis_malloc(sizeof(st_notify_t));
    cissys_assert(notify != NULL);

    if (notify == NULL) {
        return CIS_RET_MEMORY_ERR;
    }

    notify->isResponse = false;
    notify->next = NULL;
    notify->id = ++ctx->nextNotifyId;
    notify->mid = mid;
    notify->result = result;
    notify->value = NULL;

    if (value != NULL) {
        notify->value = prv_dataDup(value);

        if (notify->value == NULL) {
            cis_free(notify);
            return CIS_RET_MEMORY_ERR;
        }
    }

    if (uri != NULL) {
        notify->uri = *uri;
        CIS_LOGD("cis_notify add index:%d mid:0x%x [%d/%d/%d]", ctx->nextNotifyId, mid, uri->objectId, uri->instanceId, uri->resourceId);
    } else {
        cis_uri_make(URI_INVALID, URI_INVALID, URI_INVALID, &notify->uri);
        CIS_LOGD("cis_notify add index:%d mid:0x%x", ctx->nextNotifyId, mid);
    }

    cissys_lock(ctx->lockNotify, CIS_CONFIG_LOCK_INFINITY);
    ctx->notifyList = (st_notify_t *)CIS_LIST_ADD(ctx->notifyList, notify);
    cissys_unlock(ctx->lockNotify);

    cisnet_break_recv(ctx, ctx->server->sessionH);
    return CIS_RET_OK;
}




//////////////////////////////////////////////////////////////////////////
void  core_callbackEvent(st_context_t *context, cis_evt_t id, void *param)
{
    if (context->callback.onEvent != NULL) {
        CIS_LOGD("callback event(%d):%s", id, STR_EVENT_CODE(id));
        context->callback.onEvent(context, id, param);
    }
}

void core_updatePumpState(st_context_t *context, et_client_state_t state)
{
    CIS_LOGD("Update State To %s(%d)", STR_STATE(state), state);
    context->stateStep = state;
}


//////////////////////////////////////////////////////////////////////////
//static private function.
//


void prv_localDeinit(st_context_t **context)
{
    st_context_t *ctx = *context;

    if (ctx == NULL)
        return;

    prv_deleteObservedList(ctx);
    prv_deleteTransactionList(ctx);
    prv_deleteRequestList(ctx);
    prv_deleteNotifyList(ctx);
    prv_deleteObjectList(ctx);
    prv_deleteServer(ctx);

    cissys_lockdestory(ctx->lockRequest);
    cissys_lockdestory(ctx->lockNotify);

    if (ctx->endpointName != NULL) {
        cis_free(ctx->endpointName);
        ctx->endpointName = NULL;
    }

    std_security_clean(ctx);

    cis_free(ctx);
    (*context) = NULL;
}



static cis_data_t *prv_dataDup(const cis_data_t *src)
{
    if (src == NULL) {
        return NULL;
    }

    cis_data_t *newData = (cis_data_t *)cis_malloc(sizeof(cis_data_t));

    if (newData == NULL) {
        return NULL;
    }

    if (src->type == cis_data_type_opaque || src->type == cis_data_type_string) {
        newData->asBuffer.buffer = (uint8_t *)cis_malloc(src->asBuffer.length);

        if (newData->asBuffer.buffer == NULL) {
            cis_free(newData);
            return NULL;
        }

        newData->asBuffer.length = src->asBuffer.length;
        cissys_memcpy(newData->asBuffer.buffer, src->asBuffer.buffer, src->asBuffer.length);
    }

    cissys_memcpy(&newData->value.asInteger, &src->value.asInteger, sizeof(src->value.asInteger));
    newData->type = src->type;
    return newData;
}


static st_object_t *prv_findObject(st_context_t *context, cis_oid_t objectid)
{
    st_object_t *targetP;
    targetP = (st_object_t *)CIS_LIST_FIND(context->objectList, objectid);
    return targetP;
}


static void prv_deleteTransactionList(st_context_t *context)
{
    CIS_LOGD("fall in transaction_removeAll\n");
    transaction_removeAll(context);
}

static void prv_deleteObservedList(st_context_t *context)
{
    CIS_LOGD("fall in observe_removeAll\n");
    observe_removeAll(context);

}

static void     prv_deleteRequestList(st_context_t *context)
{
    CIS_LOGD("fall in packet_request_removeAll\n");
    packet_asynRemoveRequestAll(context);
}

static void     prv_deleteNotifyList(st_context_t *context)
{
    CIS_LOGD("fall in packet_notify_removeAll\n");
    packet_asynRemoveNotifyAll(context);
}

static void     prv_deleteObjectList(st_context_t *context)
{
    CIS_LOGD("fall in object_removeAll\n");
    object_removeAll(context);

}


static void     prv_deleteServer(st_context_t *context)
{
    if (context->server) {
        management_destoryServer(context, context->server);
        context->server = NULL;
    }

    if (context->bootstrapServer) {
        management_destoryServer(context, context->bootstrapServer);
        context->bootstrapServer = NULL;
    }
}


static cis_ret_t prv_onNetEventHandler(cisnet_t netctx, cisnet_event_t id, void *param, void *userData)
{
    st_context_t *context = (st_context_t *)userData;
    CIS_LOGD("onNetEventHandler:%d", id);

    switch (id) {
        case cisnet_event_unknow: {
            core_updatePumpState(context, PUMP_STATE_HALT);
        }
        break;

        case cisnet_event_connected: {
            if (context->stateStep == PUMP_STATE_BOOTSTRAPPING) {
                context->bootstrapServer->status = STATE_CONNECTED;
            } else if (context->stateStep == PUMP_STATE_CONNECTING) {
                context->server->status = STATE_CONNECTED;
            }
        }
        break;

        case cisnet_event_disconnect: {
            if (context->stateStep == PUMP_STATE_READY) {
                core_updatePumpState(context, PUMP_STATE_DISCONNECTED);
            } else if (context->stateStep == PUMP_STATE_BOOTSTRAPPING) {
                context->bootstrapServer->status = STATE_BS_FAILED;
            } else if (context->stateStep == PUMP_STATE_CONNECTING) {
                context->server->status = STATE_CONNECT_FAILED;
            }
        }
        break;

        default:
            break;
    }

    return CIS_RET_ERROR;
}

int prv_makeDeviceName(char **name)
{
    (*name) = (char *)cis_malloc(NBSYS_IMEI_MAXLENGTH + NBSYS_IMSI_MAXLENGTH + 2);

    if ((*name) == NULL) {
        return -1;
    }

    cis_memset((*name), 0, NBSYS_IMEI_MAXLENGTH + NBSYS_IMSI_MAXLENGTH + 1);
    uint8_t imei =  cissys_getIMEI((uint8_t *)(*name), NBSYS_IMEI_MAXLENGTH);
    *((char *)((*name) + imei)) = ';';
    uint8_t imsi =  cissys_getIMSI((uint8_t *)(*name) + imei + 1, NBSYS_IMSI_MAXLENGTH);

    if (imei <= 0 || imsi <= 0 || utils_strlen((char *)(*name)) <= 0) {
        CIS_LOGE("ERROR:Get IMEI/IMSI ERROR.\n");
        return 0;
    }

    return imei + imsi;
}
