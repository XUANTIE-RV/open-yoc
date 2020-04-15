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
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
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

/*
Contains code snippets which are:

 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.

*/
#include "cis_internals.h"
#include "cis_log.h"

static void prv_handleReset(st_context_t *contextP,
                            coap_packet_t *message)
{
    CIS_LOGI("observe reset,msgid:%d", message->mid);
    observe_cancel(contextP, message->mid, NULL);
}

static coap_status_t prv_handleRequest(st_context_t *contextP,
                                       void *fromSessionH,
                                       coap_packet_t *message,
                                       coap_packet_t *response)
{
    st_uri_t *uriP;
    st_server_t *serverP;
    coap_status_t result = (coap_status_t)COAP_IGNORE;
    uriP = uri_decode(contextP->altPath, message->uri_path);

    if (uriP == NULL) {
        return COAP_400_BAD_REQUEST;
    }

    switch (uriP->flag & URI_MASK_TYPE) {
        case URI_FLAG_DM: {
            serverP = contextP->server;

            if (serverP != NULL && serverP->sessionH == fromSessionH) {
                result = management_handleRequest(contextP, uriP, message, response);
            }

#if CIS_ENABLE_BOOTSTRAP
            else {
                serverP = utils_findBootstrapServer(contextP, fromSessionH);

                if (serverP != NULL) {
                    result = bootstrap_handleCommand(contextP, uriP,  serverP, message, response);
                }
            }

#endif
        }
        break;

#if CIS_ENABLE_BOOTSTRAP

        case URI_FLAG_DELETE_ALL: {
            result = COAP_400_BAD_REQUEST;
        }
        break;

        case URI_FLAG_BOOTSTRAP: {
            if (message->code == COAP_POST) {
                serverP = utils_findBootstrapServer(contextP, fromSessionH);

                if (serverP != NULL) {
                    result = bootstrap_handleFinish(contextP, serverP);
                }
            }
        }
        break;
#endif

        default:
            result = (coap_status_t)COAP_IGNORE;
            break;
    }

    coap_set_status_code(response, result);

    if (COAP_IGNORE < result && result < COAP_400_BAD_REQUEST) {
        result = COAP_NO_ERROR;
    }

    cis_free(uriP);
    return result;
}

coap_status_t prv_notifyObserveReport(st_context_t *context, st_observed_t *observe, const st_notify_t *notify)
{
    st_object_t *object;
    uint32_t index;
    cis_instcount_t instIndex = 0;
    cis_instcount_t instCount = 0;
    cis_attrcount_t attrIndex = 0;
    cis_attrcount_t attrCount = 0;

    //send message for notify data before
    if ((!URI_IS_SET_OBJECT(&notify->uri) || notify->value == NULL) &&
        notify->result != COAP_231_CONTINUE) {
        goto TAG_DONE;
    }


    if (!CIS_URI_IS_SET_INSTANCE(&notify->uri) ||
        !CIS_URI_IS_SET_RESOURCE(&notify->uri) ||
        !URI_IS_SET_OBJECT(&notify->uri)) {
        CIS_LOGE("ERROR:notify observe report failed.invalid uri");
        return COAP_400_BAD_REQUEST;
    }

    if (notify->value == NULL ||
        notify->value->type == cis_data_type_undefine) {
        CIS_LOGE("ERROR:notify observe report failed.invalid value");
        return COAP_400_BAD_REQUEST;
    }

    object = (st_object_t *)CIS_LIST_FIND(context->objectList, notify->uri.objectId);

    if (object == NULL) {
        CIS_LOGE("ERROR:notify observe response failed.not found object");
        return COAP_404_NOT_FOUND;
    }


    if (!object_checkInstExisted(object->instBitmapPtr, notify->uri.instanceId)) {
        CIS_LOGE("ERROR:notify observe response failed.not exist instance");
        return COAP_404_NOT_FOUND;
    }


    //malloc data memory in first call of observe
    if (observe->reportData == NULL) {
        if ((!CIS_URI_IS_SET_INSTANCE(&observe->uri) && !CIS_URI_IS_SET_RESOURCE(&observe->uri)) ||
            ((CIS_URI_IS_SET_INSTANCE(&observe->uri) && !CIS_URI_IS_SET_RESOURCE(&observe->uri)))) {
            //object/instance
            //for single instance.
            instCount = 1;
            observe->reportData = data_new(instCount);
            cissys_assert(observe->reportData != NULL);

            if (observe->reportData == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            observe->reportDataCount = instCount;
            observe->reportData->id = notify->uri.instanceId;

        } else if (CIS_URI_IS_SET_INSTANCE(&observe->uri) && CIS_URI_IS_SET_RESOURCE(&observe->uri)) {
            //object/instance/resource
            //for single instance.single resource;
            instCount = 1;
            observe->reportData = data_new(instCount);
            cissys_assert(observe->reportData != NULL);

            if (observe->reportData == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            observe->reportDataCount = instCount;
            observe->reportData->id = notify->uri.instanceId;
        }
    }

    if (observe->reportData && observe->reportData->id != notify->uri.instanceId) {
        CIS_LOGE("ERROR:notify observe report failed.last request not finish");
        return COAP_400_BAD_REQUEST;
    }

    if ((!CIS_URI_IS_SET_INSTANCE(&observe->uri) && !CIS_URI_IS_SET_RESOURCE(&observe->uri)) ||
        ((CIS_URI_IS_SET_INSTANCE(&observe->uri) && !CIS_URI_IS_SET_RESOURCE(&observe->uri)))) {
        //object/instance
        //for single instance.
        instIndex = 0;
        instCount = 1;

        //set all resources count;
        attrCount =  object->attributeCount;
    } else if (CIS_URI_IS_SET_INSTANCE(&observe->uri) && CIS_URI_IS_SET_RESOURCE(&observe->uri)) {
        //object/instance/resource
        //for single instance.
        instIndex = 0;
        instCount = 1;
        //for single resource;
        attrCount = 1;
    }


    //CIS_URI_IS_SET_INSTANCE(request->uri) && CIS_URI_IS_SET_RESOURCE(&request->uri)
    //for single resource,base data type
    if (instCount == 1 && attrCount == 1) {
        observe->reportData->type = (et_data_type_t)notify->value->type;
        observe->reportData->id = notify->uri.resourceId;

        if (notify->value->type == cis_data_type_opaque || notify->value->type == cis_data_type_string) {
            observe->reportData->asBuffer.length = notify->value->asBuffer.length;
            observe->reportData->asBuffer.buffer = (uint8_t *)cis_malloc(notify->value->asBuffer.length);

            if (observe->reportData->asBuffer.buffer == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            cis_memcpy(observe->reportData->asBuffer.buffer, notify->value->asBuffer.buffer, notify->value->asBuffer.length);
        } else {
            observe->reportData->value.asInteger = notify->value->value.asInteger;
        }

    } else {

        if (observe->reportData[instIndex].value.asChildren.array == NULL) {
            observe->reportData[instIndex].type = DATA_TYPE_OBJECT_INSTANCE;
            observe->reportData[instIndex].id = notify->uri.instanceId;
            cis_attrcount_t attrCount =  object->attributeCount;
            observe->reportData[instIndex].value.asChildren.array = data_new(attrCount);
            observe->reportData[instIndex].value.asChildren.count = attrCount;
        }

        //found out index of resource for unused array memory
        for (index = 0; index < object->attributeCount; index++) {
            if (observe->reportData[instIndex].value.asChildren.array[index].type == DATA_TYPE_UNDEFINE) {
                break;
            }

            attrIndex++;
        }

        //it's all occupied of resource array.
        if (attrIndex >= object->attributeCount) {
            CIS_LOGE("ERROR:notify observe report failed.attr index out of bounds");
            return COAP_400_BAD_REQUEST;
        }

        observe->reportData[instIndex].value.asChildren.array[attrIndex].id = notify->uri.resourceId;
        observe->reportData[instIndex].value.asChildren.array[attrIndex].type = (et_data_type_t)notify->value->type;

        if (notify->value->type == cis_data_type_opaque || notify->value->type == cis_data_type_string) {
            observe->reportData[instIndex].value.asChildren.array[attrIndex].asBuffer.length = notify->value->asBuffer.length;
            observe->reportData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer = (uint8_t *)cis_malloc(notify->value->asBuffer.length);

            if (observe->reportData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            cis_memcpy(observe->reportData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer, notify->value->asBuffer.buffer, notify->value->asBuffer.length);
        } else {
            observe->reportData[instIndex].value.asChildren.array[attrIndex].value.asInteger = notify->value->value.asInteger;
        }
    }

    if (notify->result == COAP_231_CONTINUE) {
        CIS_LOGD("ObserveReport 231_Continue id:0x%x", notify->mid);
        return COAP_231_CONTINUE;
    }

TAG_DONE:
    CIS_LOGD("ObserveReport done id:0x%x", notify->mid);

    //send observe report message with data packet
    observe_asynReport(context, observe, notify->result);
    data_free(observe->reportDataCount, observe->reportData);
    observe->reportData = NULL;
    observe->reportDataCount = 0;
    return COAP_NO_ERROR;
}


coap_status_t prv_notifyReadReport(st_context_t *context, st_request_t *request, const st_notify_t *notify)
{
    st_object_t *object;
    cis_instcount_t loopInstance;
    cis_rescount_t loopResource;

    cis_instcount_t instIndex = 0;
    cis_instcount_t instCount = 0;

    cis_attrcount_t attrIndex = 0;
    cis_attrcount_t attrCount = 0;

    if (notify->result != COAP_231_CONTINUE &&
        notify->result != COAP_205_CONTENT) {
        goto TAG_DONE;
    }

    //send message for notify data before
    if ((!URI_IS_SET_OBJECT(&notify->uri) || notify->value == NULL) &&
        notify->result != COAP_231_CONTINUE) {
        goto TAG_DONE;
    }

    if (!CIS_URI_IS_SET_INSTANCE(&notify->uri) ||
        !CIS_URI_IS_SET_RESOURCE(&notify->uri) ||
        !URI_IS_SET_OBJECT(&notify->uri)) {
        CIS_LOGE("ERROR:notify read response failed.invalid uri");
        return COAP_400_BAD_REQUEST;
    }

    if (notify->value == NULL ||
        notify->value->type == cis_data_type_undefine) {
        CIS_LOGE("ERROR:notify read response failed.invalid value");
        return COAP_400_BAD_REQUEST;
    }

    object = (st_object_t *)CIS_LIST_FIND(context->objectList, notify->uri.objectId);

    if (object == NULL) {
        CIS_LOGE("ERROR:notify read response failed.not found object");
        return COAP_404_NOT_FOUND;
    }

    //malloc data memory in first call of request
    if (request->respData == NULL) {
        if (!CIS_URI_IS_SET_INSTANCE(&request->uri) &&
            !CIS_URI_IS_SET_RESOURCE(&request->uri)) {
            //object
            //get instance valid count for malloc instance memory
            instCount = object->instValidCount;
            request->respData = data_new(instCount);
            cissys_assert(request->respData != NULL);
            request->respDataCount = instCount;
        } else if ((CIS_URI_IS_SET_INSTANCE(&request->uri) && !CIS_URI_IS_SET_RESOURCE(&request->uri))) {
            //object/instance
            //for single instance.
            instCount = 1;
            request->respData = data_new(instCount);
            cissys_assert(request->respData != NULL);
            request->respDataCount = instCount;

        } else if (CIS_URI_IS_SET_INSTANCE(&request->uri) && CIS_URI_IS_SET_RESOURCE(&request->uri)) {
            //object/instance/resource
            //for single instance.
            instCount = 1;
            request->respData = data_new(instCount);
            cissys_assert(request->respData != NULL);
            request->respDataCount = instCount;
            //for single resource;
            attrCount = 1;
        }
    }

    if (request->respData == NULL) {
        CIS_LOGE("ERROR:notify read response failed.malloc data error");
        return COAP_400_BAD_REQUEST;
    }


    if (!object_checkInstExisted(object->instBitmapPtr, notify->uri.instanceId)) {
        CIS_LOGE("ERROR:notify read response failed.invalid instance id");
        return COAP_400_BAD_REQUEST;
    }

    if (!CIS_URI_IS_SET_INSTANCE(&request->uri) &&
        !CIS_URI_IS_SET_RESOURCE(&request->uri)) {
        instIndex = 0;
        instCount = object->instValidCount;

        /*Get instance array index of reportData by bitmap index*/
        for (loopInstance = 0; loopInstance < object->instBitmapCount; loopInstance++) {
            if (notify->uri.instanceId == loopInstance) {
                break;
            }

            if (object_checkInstExisted(object->instBitmapPtr, loopInstance)) {
                instIndex++;
            }
        }

        //set all resources count;
        attrCount =  object->attributeCount;

    } else if ((CIS_URI_IS_SET_INSTANCE(&request->uri) && !CIS_URI_IS_SET_RESOURCE(&request->uri))) {
        //object/instance
        //for single instance.
        instIndex = 0;
        instCount = 1;

        //set all resources count;
        attrCount =  object->attributeCount;

    } else if (CIS_URI_IS_SET_INSTANCE(&request->uri) && CIS_URI_IS_SET_RESOURCE(&request->uri)) {
        //object/instance/resource
        //for single instance.
        instIndex = 0;
        instCount = 1;
        //for single resource;
        attrCount = 1;
    }


    //CIS_URI_IS_SET_INSTANCE(request->uri) && CIS_URI_IS_SET_RESOURCE(&request->uri)
    //for single resource,base data type
    if (instCount == 1 && attrCount == 1) {
        request->respData->type = (et_data_type_t)notify->value->type;
        request->respData->id = notify->uri.resourceId;

        if (notify->value->type == cis_data_type_opaque || notify->value->type == cis_data_type_string) {
            request->respData->asBuffer.length = notify->value->asBuffer.length;
            request->respData->asBuffer.buffer = (uint8_t *)cis_malloc(notify->value->asBuffer.length);

            if (request->respData->asBuffer.buffer == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            cis_memcpy(request->respData->asBuffer.buffer, notify->value->asBuffer.buffer, notify->value->asBuffer.length);
        } else {
            request->respData->value.asInteger = notify->value->value.asInteger;
        }

    } else {
        //used asChildren
        if (request->respData[instIndex].value.asChildren.array == NULL) {
            request->respData[instIndex].type = DATA_TYPE_OBJECT_INSTANCE;
            request->respData[instIndex].id = notify->uri.instanceId;
            request->respData[instIndex].value.asChildren.array = data_new(attrCount);
            request->respData[instIndex].value.asChildren.count = attrCount;
        }

        //found out index of resource for unused array memory
        for (loopResource = 0; loopResource < object->attributeCount; loopResource++) {
            if (request->respData[instIndex].value.asChildren.array[loopResource].type == DATA_TYPE_UNDEFINE) {
                break;
            }

            attrIndex++;
        }

        //it's all occupied of resource array.
        if (attrIndex >= object->attributeCount) {
            CIS_LOGE("ERROR:notify read response failed.attr index out of bounds");
            return COAP_400_BAD_REQUEST;
        }


        request->respData[instIndex].value.asChildren.array[attrIndex].id = notify->uri.resourceId;
        request->respData[instIndex].value.asChildren.array[attrIndex].type = (et_data_type_t)notify->value->type;

        if (notify->value->type == cis_data_type_opaque || notify->value->type == cis_data_type_string) {
            request->respData[instIndex].value.asChildren.array[attrIndex].asBuffer.length = notify->value->asBuffer.length;
            request->respData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer = (uint8_t *)cis_malloc(notify->value->asBuffer.length);

            if (request->respData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            cis_memcpy(request->respData[instIndex].value.asChildren.array[attrIndex].asBuffer.buffer, notify->value->asBuffer.buffer, notify->value->asBuffer.length);
        } else { //other type ,exclude buffer
            request->respData[instIndex].value.asChildren.array[attrIndex].value.asInteger = notify->value->value.asInteger;
        }

    }

    if (notify->result == COAP_231_CONTINUE) {
        CIS_LOGD("ReadResponse 231_Continue id:0x%x", notify->mid);
        return COAP_231_CONTINUE;
    }


TAG_DONE:
    CIS_LOGD("ReadResponse Done id:0x%x,Result:(%s)", notify->mid, STR_COAP_CODE(notify->result));

    //send response message with data packet
    object_asynAckReadData(context, request, notify->result);

    data_free(request->respDataCount, request->respData);
    request->respData = NULL;
    request->respDataCount = 0;

    return COAP_NO_ERROR;
}



coap_status_t prv_notifyDiscoverReport(st_context_t *context, st_request_t *request, const st_notify_t *notify)
{
    st_object_t *object;
    cis_instcount_t loopInstance;
    cis_rescount_t  loopResource;

    cis_instcount_t instIndex = 0;
    cis_instcount_t instCount = 0;

    cis_rescount_t  resIndex = 0;
    cis_rescount_t  resCount = 0;


    if (notify->result != COAP_231_CONTINUE &&
        notify->result != COAP_205_CONTENT) {
        goto TAG_DONE;
    }


    //send message for notify data before
    if ((!CIS_URI_IS_SET_RESOURCE(&notify->uri) && notify->value == NULL) &&
        notify->result != COAP_231_CONTINUE) {
        goto TAG_DONE;
    }


    object = (st_object_t *)CIS_LIST_FIND(context->objectList, request->uri.objectId);

    if (object == NULL) {
        CIS_LOGE("ERROR:notify discover response failed.not found object");
        core_callbackEvent(context, CIS_EVENT_RESPONSE_FAILED, (void *)notify->mid);
        return COAP_404_NOT_FOUND;
    }


    //malloc data memory in first call of request
    if (request->respData == NULL) {
        //object
        //get instance valid count for malloc instance memory
        instCount = object->instValidCount;
        request->respData = data_new(instCount);
        cissys_assert(request->respData != NULL);
        request->respDataCount = instCount;
    }

    if (request->respData == NULL) {
        CIS_LOGE("ERROR:notify discover  response failed.malloc data error");
        core_callbackEvent(context, CIS_EVENT_RESPONSE_FAILED, (void *)notify->mid);
        return COAP_400_BAD_REQUEST;
    }


    /*Get instance array index of reportData by bitmap index*/
    for (loopInstance = 0, instIndex = 0; loopInstance < object->instBitmapCount; loopInstance++) {
        if (!object_checkInstExisted(object->instBitmapPtr, loopInstance)) {
            continue;
        }

        resIndex = 0;
        //set all resources count;
        resCount =  object->attributeCount + object->actionCount;

        if (request->respData[instIndex].value.asChildren.array == NULL) {
            request->respData[instIndex].type = DATA_TYPE_OBJECT_INSTANCE;
            request->respData[instIndex].id = loopInstance;
            request->respData[instIndex].value.asChildren.array = data_new(resCount);
            request->respData[instIndex].value.asChildren.count = resCount;
        }

        //found out index of resource for unused array memory
        for (loopResource = 0; loopResource < resCount ; loopResource++) {
            if (request->respData[instIndex].value.asChildren.array[loopResource].type == DATA_TYPE_UNDEFINE) {
                break;
            }

            resIndex++;
        }

        //it's all occupied of resource array.
        if (resIndex >= resCount) {
            CIS_LOGE("ERROR:notify discover response failed.attr index out of bounds");
            core_callbackEvent(context, CIS_EVENT_RESPONSE_FAILED, (void *)notify->mid);
            goto TAG_FAILED;
        }

        request->respData[instIndex].value.asChildren.array[resIndex].id = notify->uri.resourceId;
        request->respData[instIndex].value.asChildren.array[resIndex].type = DATA_TYPE_LINK;
        request->respData[instIndex].value.asChildren.array[resIndex].value.asObjLink.instId = loopInstance;
        request->respData[instIndex].value.asChildren.array[resIndex].value.asObjLink.objectId = request->uri.objectId;

        instIndex++;
    }


    if (notify->result == COAP_231_CONTINUE) {
        CIS_LOGD("DiscoverResponse 231_Continue id:0x%x", notify->mid);
        return COAP_231_CONTINUE;
    }


TAG_DONE:
    CIS_LOGD("DiscoverResponse Done id:0x%x,Result:(%s)", notify->mid, STR_COAP_CODE(notify->result));
    //send response message with data packet
    object_asynAckReadData(context, request, notify->result);

TAG_FAILED:
    data_free(request->respDataCount, request->respData);
    request->respData = NULL;
    request->respDataCount = 0;

    return COAP_NO_ERROR;
}



//////////////////////////////////////////////////////////////////////////


bool packet_asynPushRequest(st_context_t *context, const coap_packet_t *packet, et_callback_type_t type, cis_mid_t mid)
{
    st_request_t *nodeP;
    st_uri_t *uriP;

    CIS_LOGD("asyn push message id:%d,type:%d", mid, type);

    nodeP = (st_request_t *)CIS_LIST_FIND(context->requestList, mid);

    if (nodeP != NULL) {
        return false;
    }

    nodeP = (st_request_t *)cis_malloc(sizeof(st_request_t));

    if (nodeP == NULL) {
        return false;
    }


    uriP = uri_decode(context->altPath, packet->uri_path);

    if (uriP == NULL) {
        free(nodeP);
        return false;
    }

    cis_memset(nodeP, 0, sizeof(st_request_t));
    nodeP->format = utils_convertMediaType(packet->content_type);
    cis_memcpy(nodeP->token, packet->token, packet->token_len);
    nodeP->tokenLen = packet->token_len;
    nodeP->type = type;
    nodeP->uri = *uriP;
    nodeP->mid = mid;
    nodeP->time = utils_gettime_s();
    nodeP->respData = NULL;
    nodeP->respDataCount = 0;

    cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
    context->requestList = (st_request_t *)CIS_LIST_ADD(context->requestList, nodeP);
    cissys_unlock(context->lockRequest);


    cis_free(uriP);
    return true;
}

void packet_asynRemoveRequest(st_context_t *context, cis_mid_t mid, st_request_t **node)
{
    CIS_LOGD("asyn remove message id:0x%x,type:%d", mid, (*node)->type);

    cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
    context->requestList = (st_request_t *)CIS_LIST_RM((cis_list_t *)context->requestList, mid, node);

    if ((*node)->respData != NULL) {
        data_free((*node)->respDataCount, (*node)->respData);
    }

    cis_free(*node);
    *node = NULL;
    cissys_unlock(context->lockRequest);
}



bool packet_asynFindObserveRequest(st_context_t *context, cis_coapid_t coapmid, cis_mid_t *observemid)
{
    st_request_t *nodeP;

    cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
    nodeP = (st_request_t *)CIS_LIST_FIND_U16(context->requestList, coapmid);
    cissys_unlock(context->lockRequest);

    if (nodeP != NULL) {
        (*observemid) = nodeP->mid;
        return true;
    }

    return false;
}



void packet_asynRemoveRequestAll(st_context_t *context)
{
    cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);

    while (NULL != context->requestList) {
        st_request_t *targetP;

        targetP = context->requestList;
        context->requestList = context->requestList->next;

        cis_free(targetP);
    }

    context->requestList = NULL;
    cissys_unlock(context->lockRequest);
}

void packet_asynRemoveNotifyAll(st_context_t *context)
{
    cissys_lock(context->lockNotify, CIS_CONFIG_LOCK_INFINITY);

    while (NULL != context->notifyList) {
        st_notify_t *targetP;

        targetP = context->notifyList;
        context->notifyList = context->notifyList->next;

        cis_free(targetP);
    }

    context->notifyList = NULL;
    cissys_unlock(context->lockNotify);
}

/* This function is an adaptation of function coap_receive() from Erbium's er-coap-13-engine.c.
 * Erbium is Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 */
void packet_handle_packet(st_context_t *contextP,
                          void *fromSessionH,
                          uint8_t *buffer,
                          int length)
{
    coap_status_t coap_error_code = COAP_NO_ERROR;
    static coap_packet_t message[1];
    static coap_packet_t response[1];

    if (buffer == NULL || length <= 0) {
        return;
    }

    coap_error_code = coap_parse_message(message, buffer, (uint16_t)length);

    if (coap_error_code == COAP_NO_ERROR) {
        CIS_LOGD("Parsed: ver %u, type %u, tkl %u, code %u.%.2u, mid %u, Content type: %d",
                 message->version, message->type, message->token_len, message->code >> 5, message->code & 0x1F, message->mid, message->content_type);

        if (message->code >= COAP_GET && message->code <= COAP_DELETE) {

#if CIS_ENABLE_BLOCK
            uint32_t block_num = 0;
            uint16_t block_size = REST_MAX_CHUNK_SIZE;
            uint32_t block_offset = 0;
            int64_t new_offset = 0;
#endif//CIS_ENABLE_BLOCK

            /* prepare response */
            if (message->type == COAP_TYPE_CON) {
                /* Reliable CON requests are answered with an ACK. */
                coap_init_message(response, COAP_TYPE_ACK, COAP_205_CONTENT, message->mid);
            } else {
                /* Unreliable NON requests are answered with a NON as well. */
                coap_init_message(response, COAP_TYPE_NON, COAP_205_CONTENT, contextP->nextMid++);
            }

            /* mirror token */
            if (message->token_len) {
                coap_set_header_token(response, message->token, message->token_len);
            }

#if CIS_ENABLE_BLOCK

            /* get offset for blockwise transfers */
            if (coap_get_header_block2(message, &block_num, NULL, &block_size, &block_offset)) {
                CIS_LOGD("Blockwise: block request %u (%u/%u) @ %u bytes", block_num, block_size, REST_MAX_CHUNK_SIZE, block_offset);
                block_size = MIN(block_size, REST_MAX_CHUNK_SIZE);
                new_offset = block_offset;
            }

#endif//CIS_ENABLE_BLOCK


#if CIS_ENABLE_BOOTSTRAP || CIS_ENABLE_BLOCK
            // get server
            st_server_t *serverP = contextP->server;
#endif//CIS_ENABLE_BOOTSTRAP || CIS_ENABLE_BLOCK

#if CIS_ENABLE_BOOTSTRAP

            if (serverP == NULL) {
                serverP = utils_findBootstrapServer(contextP, fromSessionH);
            }

#endif//LWM2M_BOOTSTRAP

            /* handle block1 option */
            if (IS_OPTION(message, COAP_OPTION_BLOCK1)) {
                //NOTE:we removed block implementation in current
#if CIS_ENABLE_BLOCK

                if (serverP == NULL) {
                    coap_error_code = COAP_500_INTERNAL_SERVER_ERROR;
                } else {
                    // parse block1 header
                    uint32_t block1_num;
                    uint8_t  block1_more;
                    uint16_t block1_size;
                    coap_get_header_block1(message, &block1_num, &block1_more, &block1_size, NULL);
                    CIS_LOGD("Blockwise: block1 request NUM %u (SZX %u/ SZX Max%u) MORE %u", block1_num, block1_size, REST_MAX_CHUNK_SIZE, block1_more);

                    // handle block 1
                    uint8_t *complete_buffer = NULL;
                    uint32_t complete_buffer_size = 0;
                    coap_error_code = coap_block1_handler(&contextP->server->block1Data, message->mid, message->payload, message->payload_len, block1_size, block1_num, block1_more ? true : false, &complete_buffer, &complete_buffer_size);

                    // if payload is complete, replace it in the coap message.
                    if (coap_error_code == COAP_NO_ERROR) {
                        message->payload = complete_buffer;
                        message->payload_len = (uint16_t)complete_buffer_size;
                    } else if (coap_error_code == COAP_231_CONTINUE) {
                        block1_size = MIN(block1_size, REST_MAX_CHUNK_SIZE);
                        coap_set_header_block1(response, block1_num, block1_more, block1_size);
                    }
                }

#else
                coap_error_code = COAP_501_NOT_IMPLEMENTED;
#endif//CIS_ENABLE_BLOCK
            }

            if (coap_error_code == COAP_NO_ERROR) {
                coap_error_code = prv_handleRequest(contextP, fromSessionH, message, response);
            }

            if (coap_error_code == COAP_NO_ERROR) {
#if CIS_ENABLE_BLOCK

                if (IS_OPTION(message, COAP_OPTION_BLOCK2)) {
                    /* unchanged new_offset indicates that resource is unaware of blockwise transfer */
                    if (new_offset == block_offset) {
                        CIS_LOGD("Blockwise: unaware resource with payload length %u/%u", response->payload_len, block_size);

                        if (block_offset >= response->payload_len) {
                            CIS_LOGD("handle_incoming_data(): block_offset >= response->payload_len");

                            response->code = COAP_402_BAD_OPTION;
                            coap_set_payload(response, "BlockOutOfScope", 15); /* a const char str[] and sizeof(str) produces larger code size */
                        } else {
                            coap_set_header_block2(response, block_num, response->payload_len - block_offset > block_size, block_size);
                            coap_set_payload(response, response->payload + block_offset, MIN(response->payload_len - block_offset, block_size));
                        } /* if (valid offset) */
                    } else {
                        /* resource provides chunk-wise data */
                        CIS_LOGD("Blockwise: blockwise resource, new offset %d", (int) new_offset);
                        coap_set_header_block2(response, block_num, new_offset != -1 || response->payload_len > block_size, block_size);

                        if (response->payload_len > block_size) {
                            coap_set_payload(response, response->payload, block_size);
                        }
                    } /* if (resource aware of blockwise) */
                } else if (new_offset != 0) {
                    CIS_LOGD("Blockwise: no block option for blockwise resource, using block size %u", REST_MAX_CHUNK_SIZE);

                    coap_set_header_block2(response, 0, new_offset != -1, REST_MAX_CHUNK_SIZE);
                    coap_set_payload(response, response->payload, MIN(response->payload_len, REST_MAX_CHUNK_SIZE));
                } /* if (blockwise request) */

#endif//CIS_ENABLE_BLOCK
                coap_error_code = packet_send(contextP, response);

                cis_free(response->payload);
                response->payload = NULL;
                response->payload_len = 0;
            } else if (coap_error_code != COAP_IGNORE) {
                if (1 == coap_set_status_code(response, coap_error_code)) {
                    coap_error_code = packet_send(contextP, response);
                }
            }

        } else {
            /* Responses */
            switch (message->type) {
                case COAP_TYPE_NON:
                case COAP_TYPE_CON: {
                    bool done = transaction_handleResponse(contextP, message, response);

                    if (!done && message->type == COAP_TYPE_CON) {
                        coap_init_message(response, COAP_TYPE_ACK, 0, message->mid);
                        coap_error_code = packet_send(contextP, response);
                    }
                }
                break;

                case COAP_TYPE_RST:
                    /* Cancel possible subscriptions. */
                {
                    prv_handleReset(contextP, message);
                    transaction_handleResponse(contextP, message, NULL);
                    break;
                }

                case COAP_TYPE_ACK: {
                    transaction_handleResponse(contextP, message, NULL);
                    break;
                }

                default:
                    break;
            }
        } /* Request or Response */

        coap_free_header(message);
    } /* if (parsed correctly) */
    else {
        CIS_LOGD("Message parsing failed %u.%2u", coap_error_code >> 5, coap_error_code & 0x1F);
    }

    if (coap_error_code != COAP_NO_ERROR && coap_error_code != COAP_IGNORE) {
        CIS_LOGD("ERROR %u: %s", coap_error_code, coap_error_message);

        /* Set to sendable error code. */
        if (coap_error_code >= COAP_UNKOWN_ERROR) {
            coap_error_code = COAP_500_INTERNAL_SERVER_ERROR;
        }

        /* Reuse input buffer for error message. */
        coap_init_message(message, COAP_TYPE_ACK, coap_error_code, message->mid);
        coap_set_payload(message, coap_error_message, utils_strlen(coap_error_message));
        packet_send(contextP, message);
    }
}


void packet_step(st_context_t *context, cis_time_t currentTime)
{
    st_request_t *targetRequest, *delRequest;
    cis_time_t interval;

    st_notify_t *notifyNode, *delNotify;
    coap_status_t  result;

    if (context->server == NULL && context->bootstrapServer == NULL) {
        return;
    }

    /*traversal notify list*/
    cissys_lock(context->lockNotify, CIS_CONFIG_LOCK_INFINITY);
    notifyNode = context->notifyList;
    cissys_unlock(context->lockNotify);

    while (notifyNode != NULL) {
        st_request_t *request;

        if (notifyNode->isResponse) {
            CIS_LOGD("notify response index:%d,id:0x%x,result:(%s)", notifyNode->id, notifyNode->mid, STR_COAP_CODE(notifyNode->result));
            CIS_LOG_URI("notify uri", &notifyNode->uri);

            cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
            request = (st_request_t *)CIS_LIST_FIND(context->requestList, notifyNode->mid);
            cissys_unlock(context->lockRequest);

            if (request != NULL) {
                result = COAP_500_INTERNAL_SERVER_ERROR;

                if (CIS_URI_IS_SET_INSTANCE(&notifyNode->uri) && !uri_exist(&request->uri, &notifyNode->uri)) {
                    CIS_LOGE("ERROR:response not exist uri,request by removed,id:0x%x", notifyNode->mid);
                    packet_asynRemoveRequest(context, notifyNode->mid, &request);
                    core_callbackEvent(context, CIS_EVENT_RESPONSE_FAILED, (void *)notifyNode->mid);
                } else {
                    if (request->type == CALLBACK_TYPE_READ) {
                        result = prv_notifyReadReport(context, request, notifyNode);
                    } else if (request->type == CALLBACK_TYPE_DISCOVER) {
                        result = prv_notifyDiscoverReport(context, request, notifyNode);
                    } else if (request->type == CALLBACK_TYPE_EXECUTE || request->type == CALLBACK_TYPE_WRITE) {
                        result = object_asynAckNodata(context, request, notifyNode->result);
                    } else if (request->type == CALLBACK_TYPE_OBSERVE_CANCEL || request->type == CALLBACK_TYPE_OBSERVE_PARAMS) {
                        result = observe_asynAckNodata(context, request, notifyNode->result);
                    } else if (request->type == CALLBACK_TYPE_OBSERVE) {
                        result = observe_asynAckNodata(context, request, notifyNode->result);
                    }

                    CIS_LOG_URI("request uri", &request->uri);
                    CIS_LOGD("request finish id:0x%x,result:%s", notifyNode->mid, STR_COAP_CODE(result));

                    if (notifyNode->result != COAP_231_CONTINUE) {
                        packet_asynRemoveRequest(context, notifyNode->mid, &request);
                    }
                }//!uri_exist
            } else {
                core_callbackEvent(context, CIS_EVENT_RESPONSE_FAILED, (void *)notifyNode->mid);
                CIS_LOGE("ERROR:notify response invalid id:0x%x", notifyNode->mid);
            }
        } else {
            st_observed_t *observe;
            result = COAP_404_NOT_FOUND;

            if (notifyNode->value == NULL) {
                CIS_LOGW("WARNING:observe data is null.");
            }

            CIS_LOGD("notify observe index:%d,id:0x%x,result:(%s)", notifyNode->id, notifyNode->mid, STR_COAP_CODE(notifyNode->result));
            CIS_LOG_URI("notify uri", &notifyNode->uri);

            observe = observe_findByMsgid(context, notifyNode->mid);

            if (observe != NULL) {
                CIS_LOG_URI("observe uri", &observe->uri);

                if (!uri_exist(&observe->uri, &notifyNode->uri)) {
                    CIS_LOGE("ERROR:notify not exist uri,notify by removed,id:0x%x", notifyNode->mid);
                    core_callbackEvent(context, CIS_EVENT_NOTIFY_FAILED, (void *)notifyNode->mid);
                } else {
                    result = prv_notifyObserveReport(context, observe, notifyNode);
                    CIS_LOGD("observe finish id:0x%x", notifyNode->mid);
                }
            } else {
                core_callbackEvent(context, CIS_EVENT_NOTIFY_FAILED, (void *)notifyNode->mid);
                CIS_LOGE("ERROR:observe not found,id:0x%x", notifyNode->mid);
            }
        }


        delNotify = notifyNode;

        if (delNotify->value != NULL) {
            if ((delNotify->value->type == cis_data_type_opaque ||
                 delNotify->value->type == cis_data_type_string) &&
                delNotify->value->asBuffer.buffer != NULL) {
                cis_free(delNotify->value->asBuffer.buffer);
            }

            cis_free(delNotify->value);
        }

        cissys_lock(context->lockNotify, CIS_CONFIG_LOCK_INFINITY);
        context->notifyList = (st_notify_t *)CIS_LIST_RM(context->notifyList, delNotify->id, NULL);
        notifyNode = notifyNode->next;
        cissys_unlock(context->lockNotify);

        cis_free(delNotify);
    }

    /*check request node list*/
    cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
    targetRequest = context->requestList;
    cissys_unlock(context->lockRequest);

    while (targetRequest != NULL) {
        interval = currentTime - targetRequest->time;

        if (interval < CIS_CONFIG_CALLBACK_TIMEOUT) {
            cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
            targetRequest = targetRequest->next;
            cissys_unlock(context->lockRequest);
            continue;
        } else {
            delRequest = targetRequest;

            cissys_lock(context->lockRequest, CIS_CONFIG_LOCK_INFINITY);
            targetRequest = targetRequest->next;
            cissys_unlock(context->lockRequest);

            packet_asynRemoveRequest(context, delRequest->mid, &delRequest);
        }
    }
}

bool packet_read(st_context_t *context, cis_time_t timeout)
{
    uint8_t *buffer = NULL;
    uint32_t length = 0;
    cisnet_t netContext = NULL;

    if (context->server == NULL && context->bootstrapServer == NULL) {
        //CIS_LOGE("ERROR:server=null.");
        return false;
    }

    if (context->server != NULL) {
        netContext = (cisnet_t)context->server->sessionH;
    } else if (context->bootstrapServer != NULL) {
        netContext = (cisnet_t)context->bootstrapServer->sessionH;
    }

    if (netContext == NULL) {
        CIS_LOGE("ERROR:net context invalid in packet_step.");
        return false;
    }

    if (cisnet_read(context, netContext, &buffer, &length, timeout) == CIS_RET_OK) {
        CIS_LOGI("-Recv Buffer %d bytes---", length);
        CIS_LOG_BUF("Recv", buffer, length);
        packet_handle_packet(context, (void *)netContext, buffer, length);
        cisnet_free(context, netContext, buffer, length);
        return true;
    }

    return false;
}
/*
bool packet_read(st_context_t *context)
{
    uint8_t* buffer;
    uint32_t length;
    cisnet_t netContext = NULL;

    if(context->server == NULL && context->bootstrapServer == NULL)
    {
        return false;
    }

    if(context->server != NULL)
    {
        netContext = (cisnet_t)context->server->sessionH;
    }else
    if(context->bootstrapServer !=NULL)
    {
        netContext = (cisnet_t)context->bootstrapServer->sessionH;
    }

    if(netContext == NULL)
    {
        CIS_LOGE("ERROR:net context invalid in packet_step.");
        return false;
    }

    if(cisnet_read(netContext,&buffer,&length) == CIS_RET_OK){
        CIS_LOGI("-Recv Buffer %d bytes---",length);
        CIS_LOG_BUF("Recv",buffer,length);
        packet_handle_packet(context,(void*)netContext,buffer,length);
        cisnet_free(netContext,buffer,length);
        return true;
    }
    return false;
}*/

coap_status_t packet_send(st_context_t *contextP,
                          coap_packet_t *message)
{
    coap_status_t result = COAP_500_INTERNAL_SERVER_ERROR;
    uint8_t *pktBuffer;
    size_t pktBufferLen = 0;
    size_t allocLen;
    void *sessionH;

    allocLen = coap_serialize_get_size(message);

    if (allocLen == 0) {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    pktBuffer = (uint8_t *)cis_malloc(allocLen);

    if (pktBuffer != NULL) {
        pktBufferLen = coap_serialize_message(message, pktBuffer);

        if (0 != pktBufferLen) {
            if (contextP->stateStep == PUMP_STATE_BOOTSTRAPPING) {
                sessionH = contextP->bootstrapServer->sessionH;
            } else {
                sessionH = contextP->server->sessionH;
            }

            if (sessionH != NULL) {
                if (cisnet_write(contextP, (cisnet_t)sessionH, pktBuffer, pktBufferLen) == CIS_RET_OK) {
                    CIS_LOGD("-[Pack]Send Buffer %d bytes---", pktBufferLen);
                    CIS_LOG_BUF("Send", pktBuffer, pktBufferLen);
                    result = COAP_NO_ERROR;
                } else {
                    CIS_LOGE("ERROR:send buffer failed.");
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }
            } else {
                CIS_LOGE("ERROR:net context invalid.send buffer failed.");
                result = COAP_500_INTERNAL_SERVER_ERROR;
            }
        }

        cis_free(pktBuffer);
    }

    return result;
}


