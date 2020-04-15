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
 *    Toby Jaffey - Please refer to git log
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
#include "cis_internals.h"
#include "cis_log.h"

static st_observed_t *prv_findObserved(st_context_t *contextP, st_uri_t *uriP);
static void prv_unlinkObserved(st_context_t *contextP, st_observed_t *observedP);
static st_observed_t *prv_getObserved(st_context_t *contextP, st_uri_t *uriP);



coap_status_t observe_asynAckNodata(st_context_t *context, st_request_t *request, cis_coapret_t result)
{
    st_observed_t *observed;
    st_uri_t *uriP = &request->uri;
    coap_packet_t packet[1];

    if (request->tokenLen == 0) {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    coap_init_message(packet, COAP_TYPE_ACK, result, (uint16_t)request->mid);
    coap_set_header_token(packet, request->token, request->tokenLen);

    if (request->type == CALLBACK_TYPE_OBSERVE) {
        if (!CIS_URI_IS_SET_INSTANCE(uriP) && CIS_URI_IS_SET_RESOURCE(uriP)) {
            return COAP_400_BAD_REQUEST;
        }

        if (result == COAP_205_CONTENT) {
            observed = prv_getObserved(context, uriP);

            if (observed == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            observed->actived = true;
            observed->msgid = request->mid;
            observed->tokenLen = request->tokenLen;
            cis_memcpy(observed->token, request->token, request->tokenLen);
            observed->lastTime = utils_gettime_s();
            coap_set_header_observe(packet, observed->counter++);
        }
    } else if (request->type == CALLBACK_TYPE_OBSERVE_CANCEL) {
        observed = prv_findObserved(context, uriP);

        if (observed == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        observe_cancel(context, MESSAGEID_INVALID, uriP);
    } else if (request->type == CALLBACK_TYPE_OBSERVE_PARAMS) {
        observed = prv_findObserved(context, uriP);

        if (observed == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
    } else {
        return COAP_400_BAD_REQUEST;
    }


    packet_send(context, packet);

    return COAP_NO_ERROR;
}


coap_status_t observe_asynReport(st_context_t *context, const st_observed_t *observe, cis_coapret_t result)
{
    st_observed_t *target;
    coap_packet_t packet[1];


    et_media_type_t formatP;
    uint8_t *bufferP;
    int32_t length;


    if (observe == NULL)
        return COAP_400_BAD_REQUEST;

    st_data_t *data = observe->reportData;
    uint16_t count = observe->reportDataCount;

    if (data == NULL) {
        return COAP_400_BAD_REQUEST;
    }

    for (target = context->observedList ; target != NULL ; target = target->next) {
        if (target->msgid ==  observe->msgid) {
            break;
        }
    }

    if (target == NULL) {
        return COAP_404_NOT_FOUND;
    }

    if (CIS_URI_IS_SET_INSTANCE(&target->uri) && CIS_URI_IS_SET_RESOURCE(&target->uri)) {
        if (count > 1) {
            return COAP_400_BAD_REQUEST;
        } else if (!uri_exist(&target->uri, &observe->uri)) {
            return COAP_400_BAD_REQUEST;
        }
    }


    formatP = target->format;
    length = data_serialize(&target->uri, count, data, &formatP, &bufferP);

    if (length <= -1) {
        CIS_LOGE("ERROR:observe report data serialize failed.");
        return COAP_500_INTERNAL_SERVER_ERROR;
    } else if (length == 0) {
        CIS_LOGW("WARNING:observe report serialize length == 0.");
    }


    coap_init_message(packet, COAP_TYPE_ACK, COAP_205_CONTENT, 0);
    coap_set_header_content_type(packet, formatP);
    if (length > 0)
        coap_set_payload(packet, bufferP, length);
    target->lastTime = utils_gettime_s();
    target->lastMid = context->nextMid++;
    packet->mid = target->lastMid;
    coap_set_header_token(packet, target->token, target->tokenLen);
    coap_set_header_observe(packet, target->counter++);
    packet_send(context, packet);

    if (length > 0)
        cis_free(bufferP);

    return COAP_NO_ERROR;
}


void observe_removeAll(st_context_t *contextP)
{
    while (NULL != contextP->observedList) {
        st_observed_t *targetP;

        targetP = contextP->observedList;
        contextP->observedList = contextP->observedList->next;

        if (targetP->parameters != NULL) {
            cis_free(targetP->parameters);
        }

        if (targetP->reportData != NULL) {
            data_free(targetP->reportDataCount, targetP->reportData);
            targetP->reportData = NULL;
            targetP->reportDataCount = 0;
        }

        cis_free(targetP);
    }
}



coap_status_t observe_handleRequest(st_context_t *contextP, st_uri_t *uriP,
                                    coap_packet_t *message, coap_packet_t *response)
{
    uint32_t flag;
    bool isObserveFlag;
    bool isObserveRepeat;
    coap_status_t result;
    cis_mid_t observeMid;
    st_object_t *targetObject;

    if (!CIS_URI_IS_SET_INSTANCE(uriP) && CIS_URI_IS_SET_RESOURCE(uriP)) {
        return COAP_400_BAD_REQUEST;
    }

    if (message->token_len == 0) {
        return COAP_400_BAD_REQUEST;
    }

    targetObject = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);

    if (NULL == targetObject) {
        return COAP_404_NOT_FOUND;
    }

    coap_get_header_observe(message, &flag);
    isObserveFlag = (flag == 0 ? true : false);

    if (isObserveFlag) {
        isObserveRepeat = packet_asynFindObserveRequest(contextP, message->mid, &observeMid);

        if (isObserveRepeat) {
            CIS_LOGD("repeat observe request 0x%x", observeMid);
        } else {
            observeMid = ++contextP->nextObserveNum;
            observeMid = (observeMid << 16) | message->mid;
            CIS_LOGD("new observe request 0x%x", observeMid);
        }

        result = contextP->callback.onObserve(contextP, uriP, true, observeMid);

        if (COAP_206_CONFORM != result) {
            return result;
        }

        if (result == COAP_206_CONFORM) {
            if (!isObserveRepeat) {
                packet_asynPushRequest(contextP, message, CALLBACK_TYPE_OBSERVE, observeMid);
            }

            result = COAP_IGNORE;
        }
    } else {
        st_observed_t *observedP;
        observedP = prv_findObserved(contextP, uriP);

        if (observedP == NULL) {
            return COAP_404_NOT_FOUND;
        }

        result = contextP->callback.onObserve(contextP, uriP, false, message->mid);

        if (result == COAP_206_CONFORM) {
            packet_asynPushRequest(contextP, message, CALLBACK_TYPE_OBSERVE_CANCEL, message->mid);
            result = COAP_IGNORE;
        }
    }

    return result;
}


//NOTE: if mid==LWM2M_MAX_ID then find observed_obj by uriP and remove it from list
//     else find observed_obj which lastMid=mid and remove it from list
void observe_cancel(st_context_t *contextP,
                    uint16_t mid,
                    st_uri_t *uriP)
{
    st_observed_t *observedP;

    CIS_LOGD("observe_cancel mid: %d", mid);

    if (mid == MESSAGEID_INVALID) {
        observedP = prv_findObserved(contextP, uriP);

        if (observedP != NULL) {
            prv_unlinkObserved(contextP, observedP);

            if (observedP->parameters != NULL) {
                cis_free(observedP->parameters);
            }

            if (observedP->reportData != NULL) {
                data_free(observedP->reportDataCount, observedP->reportData);
                observedP->reportData = NULL;
                observedP->reportDataCount = 0;
            }

            cis_free(observedP);
        }

        return;
    }


    for (observedP = contextP->observedList;
         observedP != NULL;
         observedP = observedP->next) {
        if (observedP->lastMid == mid) {
            prv_unlinkObserved(contextP, observedP);

            if (observedP->parameters != NULL) {
                cis_free(observedP->parameters);
            }

            if (observedP->reportData != NULL) {
                data_free(observedP->reportDataCount, observedP->reportData);
                observedP->reportData = NULL;
                observedP->reportDataCount = 0;
            }

            cis_free(observedP);
            return;
        }
    }

    return;
}

coap_status_t observe_setParameters(st_context_t *contextP,
                                    st_uri_t *uriP, st_observe_attr_t *attrP, coap_packet_t *message)
{
    cis_coapret_t result;

    CIS_LOGD("toSet: %08X, toClear: %08X, minPeriod: %d, maxPeriod: %d, greaterThan: %f, lessThan: %f, step: %f",
             attrP->toSet, attrP->toClear, attrP->minPeriod, attrP->maxPeriod, attrP->greaterThan, attrP->lessThan, attrP->step);
    CIS_LOG_URI("observe set", uriP);

    if (!CIS_URI_IS_SET_INSTANCE(uriP) && CIS_URI_IS_SET_RESOURCE(uriP)) {
        return COAP_400_BAD_REQUEST;
    }

    st_observed_t *targetP = prv_getObserved(contextP, uriP);

    if (targetP == NULL) {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    // Check rule “lt” value + 2*”stp” values < “gt” value
    if ((((attrP->toSet | (targetP->parameters ? targetP->parameters->toSet : 0)) & ~attrP->toClear) & ATTR_FLAG_NUMERIC) == ATTR_FLAG_NUMERIC) {
        float gt;
        float lt;
        float stp;

        if (0 != (attrP->toSet & ATTR_FLAG_GREATER_THAN)) {
            gt = (float)attrP->greaterThan;
        } else {
            gt = (float)targetP->parameters->greaterThan;
        }

        if (0 != (attrP->toSet & ATTR_FLAG_LESS_THAN)) {
            lt = (float)attrP->lessThan;
        } else {
            lt = (float)targetP->parameters->lessThan;
        }

        if (0 != (attrP->toSet & ATTR_FLAG_STEP)) {
            stp = (float)attrP->step;
        } else {
            stp = (float)targetP->parameters->step;
        }

        if (lt + (2 * stp) >= gt) {
            return COAP_400_BAD_REQUEST;
        }
    }

    if (targetP->parameters == NULL) {
        if (attrP->toSet != 0) {
            targetP->parameters = (st_observe_attr_t *)cis_malloc(sizeof(st_observe_attr_t));

            if (targetP->parameters == NULL) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            cis_memcpy(targetP->parameters, attrP, sizeof(st_observe_attr_t));
        }
    } else {
        targetP->parameters->toSet &= ~attrP->toClear;

        if (attrP->toSet & ATTR_FLAG_MIN_PERIOD) {
            targetP->parameters->minPeriod = attrP->minPeriod;
        }

        if (attrP->toSet & ATTR_FLAG_MAX_PERIOD) {
            targetP->parameters->maxPeriod = attrP->maxPeriod;
        }

        if (attrP->toSet & ATTR_FLAG_GREATER_THAN) {
            targetP->parameters->greaterThan = attrP->greaterThan;
        }

        if (attrP->toSet & ATTR_FLAG_LESS_THAN) {
            targetP->parameters->lessThan = attrP->lessThan;
        }

        if (attrP->toSet & ATTR_FLAG_STEP) {
            targetP->parameters->step = attrP->step;
        }
    }

    CIS_LOGD("Final toSet: %08X, minPeriod: %d, maxPeriod: %d, greaterThan: %f, lessThan: %f, step: %f",
             targetP->parameters->toSet, targetP->parameters->minPeriod, targetP->parameters->maxPeriod, targetP->parameters->greaterThan, targetP->parameters->lessThan, targetP->parameters->step);

    if (contextP->callback.onSetParams == NULL) {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }


    result = contextP->callback.onSetParams(contextP, uriP, *attrP, message->mid);

    if (result == COAP_206_CONFORM) {
        packet_asynPushRequest(contextP, message, CALLBACK_TYPE_OBSERVE_PARAMS, message->mid);
        result = COAP_IGNORE;
    }

    return result;
}

st_observed_t *observe_findByUri(st_context_t *contextP,
                                 st_uri_t *uriP)
{
    return prv_findObserved(contextP, uriP);
}

st_observed_t *observe_findByMsgid(st_context_t *contextP, cis_mid_t mid)
{
    st_observed_t *targetP;
    targetP = contextP->observedList;

    while (targetP != NULL) {
        if (targetP->msgid == mid) {
            return targetP;
        }

        targetP = targetP->next;
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//private
static st_observed_t *prv_findObserved(st_context_t *contextP, st_uri_t *uriP)
{
    st_observed_t *targetP;

    //
    targetP = contextP->observedList;

    while (targetP != NULL) {
        if (targetP->uri.objectId == uriP->objectId) {
            if ((!CIS_URI_IS_SET_INSTANCE(uriP) && !CIS_URI_IS_SET_INSTANCE(&(targetP->uri)))
                || (CIS_URI_IS_SET_INSTANCE(uriP) && CIS_URI_IS_SET_INSTANCE(&(targetP->uri)) && (uriP->instanceId == targetP->uri.instanceId))) {
                if (((CIS_URI_IS_SET_RESOURCE(uriP) && CIS_URI_IS_SET_RESOURCE(&(targetP->uri))) && (uriP->resourceId == targetP->uri.resourceId))
                    || (!CIS_URI_IS_SET_RESOURCE(uriP) && !CIS_URI_IS_SET_RESOURCE(&(targetP->uri)))) {
                    return targetP;
                }
            }
        }

        targetP = targetP->next;
    }

    //CIS_LOGD("Found nothing");
    return NULL;
}

static void prv_unlinkObserved(st_context_t *contextP, st_observed_t *observedP)
{
    CIS_LOGD("observe unlink mid:0x%x", observedP->msgid);
    CIS_LOG_URI("observe unlink", &observedP->uri);

    if (contextP->observedList == observedP) {
        contextP->observedList = contextP->observedList->next;
    } else {
        st_observed_t *parentP;

        parentP = contextP->observedList;

        while (parentP->next != NULL
               && parentP->next != observedP) {
            parentP = parentP->next;
        }

        if (parentP->next != NULL) {
            parentP->next = parentP->next->next;
        }
    }
}


static st_observed_t *prv_getObserved(st_context_t *contextP, st_uri_t *uriP)
{
    st_observed_t *observedP;

    observedP = prv_findObserved(contextP, uriP);

    if (observedP == NULL) {
        observedP = (st_observed_t *)cis_malloc(sizeof(st_observed_t));

        if (observedP == NULL) {
            return NULL;
        }

        cis_memset(observedP, 0, sizeof(st_observed_t));
        cis_memcpy(&(observedP->uri), uriP, sizeof(st_uri_t));
        observedP->actived = false;
        observedP->format = LWM2M_CONTENT_TEXT;
        contextP->observedList = (st_observed_t *)CIS_LIST_ADD(contextP->observedList, observedP);

        CIS_LOG_URI("observe new", uriP);
    }

    return observedP;
}
