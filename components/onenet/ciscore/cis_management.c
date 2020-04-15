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
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
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
#include "std_object/std_object.h"
#include "cis_log.h"
#include <stdio.h>


static int  prv_readAttributes(multi_option_t *query, st_observe_attr_t *attrP);


coap_status_t management_handleRequest(st_context_t *contextP,
                                       st_uri_t *uriP,
                                       coap_packet_t *message,
                                       coap_packet_t *response)
{
    coap_status_t result;
    et_media_type_t format;
    st_server_t *serverP = contextP->server;
    CIS_LOGD("Code: %02X, server status: %s", message->code, STR_STATUS(serverP->status));

    format = utils_convertMediaType(message->content_type);

    if (uriP->objectId == CIS_SECURITY_OBJECT_ID) {
        return COAP_404_NOT_FOUND;
    }

    if (serverP->status != STATE_REGISTERED
        && serverP->status != STATE_REG_UPDATE_NEEDED
        && serverP->status != STATE_REG_UPDATE_NEEDED_WITHOBJECTS
        && serverP->status != STATE_REG_UPDATE_PENDING) {
        return COAP_IGNORE;
    }

    // TODO: check ACL

    switch (message->code) {
        case COAP_GET: {
            uint8_t *buffer = NULL;
            int32_t length = 0;

            if (IS_OPTION(message, COAP_OPTION_OBSERVE)) {
                result = observe_handleRequest(contextP, uriP, message, response);
            } else if (IS_OPTION(message, COAP_OPTION_ACCEPT)
                       && message->accept_num == 1
                       && message->accept[0] == APPLICATION_LINK_FORMAT) {
                format = LWM2M_CONTENT_LINK;
                result = object_discover(contextP, uriP, message);
            } else {
                result = object_read(contextP, uriP, message);
            }

            if (COAP_205_CONTENT == result) {
                if (buffer != NULL) {
                    coap_set_header_content_type(response, format);
                    coap_set_payload(response, buffer, length);
                    // handle_packet will free buffer
                }
            } else {
                if (buffer) {
                    cis_free(buffer);
                }
            }
        }
        break;

        case COAP_POST: {
            if (!CIS_URI_IS_SET_INSTANCE(uriP)) {
                //spec 5.4.6
                result = COAP_400_BAD_REQUEST;
            } else if (!CIS_URI_IS_SET_RESOURCE(uriP)) { //ref: 5.4.3 resource id is not required for write operator
                result = object_write(contextP, uriP, format, message->payload, message->payload_len, message);
            } else {
                result = object_execute(contextP, uriP, message->payload, message->payload_len, message);
            }
        }
        break;

        case COAP_PUT: {
            if (IS_OPTION(message, COAP_OPTION_URI_QUERY)) {
                st_observe_attr_t attr;

                if (0 == prv_readAttributes(message->uri_query, &attr)) {
                    result = observe_setParameters(contextP, uriP, &attr, message);
                } else {
                    result = COAP_400_BAD_REQUEST;
                }
            } else if (CIS_URI_IS_SET_INSTANCE(uriP)) {
                result = object_write(contextP, uriP, format, message->payload, message->payload_len, message);
            } else {
                result = COAP_400_BAD_REQUEST;
            }
        }
        break;

        case COAP_DELETE:
#if 0
            if (!LWM2M_URI_IS_SET_INSTANCE(uriP) || LWM2M_URI_IS_SET_RESOURCE(uriP)) {
                result = COAP_400_BAD_REQUEST;
            } else {
                result = object_delete(contextP, uriP);

                if (result == COAP_202_DELETED) {
                    registration_update_registration(contextP, TRUE);
                }
            }
    }

#endif
    result = COAP_400_BAD_REQUEST;
    break;

default:
    result = COAP_400_BAD_REQUEST;
    break;
}

return result;
}



void management_refreshDirtyServer(st_context_t *contextP)
{
    st_server_t *targetP;
    st_server_t   *nextP;

    // Remove all servers marked as dirty
    targetP = contextP->bootstrapServer;
    contextP->bootstrapServer = NULL;

    if (targetP != NULL) {
        nextP = targetP->next;

        if (!targetP->dirty) {
            targetP->status = STATE_UNCREATED;
            targetP->next = contextP->bootstrapServer;
            contextP->bootstrapServer = targetP;
        } else {
            management_destoryServer(contextP, targetP);
        }

        targetP = nextP;
    }

    targetP = contextP->server;
    contextP->server = NULL;

    if (targetP != NULL) {
        nextP = targetP->next;

        if (!targetP->dirty) {
            targetP->status = STATE_UNCREATED;
            targetP->next = contextP->server;
            contextP->server = targetP;
        } else {
            management_destoryServer(contextP, targetP);
        }

        targetP = nextP;
    }

}

void *management_createNetwork(st_context_t *context, st_server_t *serverP)
{
    cis_ret_t ret;
    cisnet_t newNetContext = NULL;

    if (context->server != serverP &&
        context->bootstrapServer != serverP) {
        CIS_LOGE("ERROR:invalid server %s", serverP->host);
        return NULL;
    }

    if (serverP->sessionH != NULL) {
        cisnet_destroy(context, (cisnet_t)serverP->sessionH);
        serverP->sessionH = NULL;
    }

    ret = cisnet_create(context, &newNetContext, serverP->host);

    if (ret != CIS_RET_OK || newNetContext == NULL) {
        /* c-sky add: shoud not set STATE_CREATE_FAILED status */
        //serverP->status = STATE_CREATE_FAILED;
        return NULL;
    }

    serverP->status = STATE_CREATED;
    serverP->sessionH = newNetContext;
    return serverP->sessionH;
}


bool management_connectServer(st_context_t *context, st_server_t *serverP)
{
    cis_ret_t ret;

    if (context->server != serverP &&
        context->bootstrapServer != serverP) {
        CIS_LOGE("ERROR:invalid server %s", serverP->host);
        return false;
    }

    if (serverP == NULL || serverP->sessionH == NULL) {
        return false;
    }

    serverP->status = STATE_CONNECT_PENDING;
    ret = cisnet_connect(context, (cisnet_t)serverP->sessionH);

    if (ret != CIS_RET_OK) {
        serverP->status = STATE_CONNECT_FAILED;
        return false;
    }

    CIS_LOGI("waiting for connected.");
    return true;
}

void management_disconnectServer(st_context_t *context, st_server_t *serverP)
{
    if (context->server != serverP &&
        context->bootstrapServer != serverP) {
        CIS_LOGE("ERROR:invalid server %s", serverP->host);
        return;
    }

    if (serverP != NULL && serverP->sessionH != NULL) {
        cisnet_disconnect(context, (cisnet_t)serverP->sessionH);
        cisnet_destroy(context, (cisnet_t)serverP->sessionH);
        serverP->status = STATE_CREATED;
        serverP->sessionH = NULL;
    }
}

void management_destoryServer(st_context_t *context, st_server_t *serverP)
{
    if (serverP == NULL) {
        return;
    }

    if (serverP->sessionH != NULL) {
        cisnet_disconnect(context, (cisnet_t)serverP->sessionH);
        cisnet_destroy(context, (cisnet_t)serverP->sessionH);
        serverP->sessionH = NULL;
    }

    if (NULL != serverP->host) {
        cis_free(serverP->host);
    }

    if (NULL != serverP->location) {
        cis_free(serverP->location);
    }

    if (context->server == serverP) {
        context->server = NULL;
    }

    if (context->bootstrapServer == serverP) {
        context->bootstrapServer = NULL;
    }


    cis_free(serverP);



}


//////////////////////////////////////////////////////////////////////////////////
//private

static int prv_readAttributes(multi_option_t *query,
                              st_observe_attr_t *attrP)
{
    int64_t intValue;
    double floatValue;

    cis_memset(attrP, 0, sizeof(st_observe_attr_t));

    while (query != NULL) {
        if (utils_strncmp((char *)query->data, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_MIN_PERIOD)) {
                return -1;
            }

            if (query->len == ATTR_MIN_PERIOD_LEN) {
                return -1;
            }

            if (1 != utils_plainTextToInt64(query->data + ATTR_MIN_PERIOD_LEN, query->len - ATTR_MIN_PERIOD_LEN, &intValue)) {
                return -1;
            }

            if (intValue < 0) {
                return -1;
            }

            attrP->toSet |= ATTR_FLAG_MIN_PERIOD;
            attrP->minPeriod = (cis_time_t)intValue;
        } else if (utils_strncmp((char *)query->data, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN - 1) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_MIN_PERIOD)) {
                return -1;
            }

            if (query->len != ATTR_MIN_PERIOD_LEN - 1) {
                return -1;
            }

            attrP->toClear |= ATTR_FLAG_MIN_PERIOD;
        } else if (utils_strncmp((char *)query->data, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_MAX_PERIOD)) {
                return -1;
            }

            if (query->len == ATTR_MAX_PERIOD_LEN) {
                return -1;
            }

            if (1 != utils_plainTextToInt64(query->data + ATTR_MAX_PERIOD_LEN, query->len - ATTR_MAX_PERIOD_LEN, &intValue)) {
                return -1;
            }

            if (intValue < 0) {
                return -1;
            }

            attrP->toSet |= ATTR_FLAG_MAX_PERIOD;
            attrP->maxPeriod = (cis_time_t)intValue;
        } else if (utils_strncmp((char *)query->data, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN - 1) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_MAX_PERIOD)) {
                return -1;
            }

            if (query->len != ATTR_MAX_PERIOD_LEN - 1) {
                return -1;
            }

            attrP->toClear |= ATTR_FLAG_MAX_PERIOD;
        } else if (utils_strncmp((char *)query->data, ATTR_GREATER_THAN_STR, ATTR_GREATER_THAN_LEN) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_GREATER_THAN)) {
                return -1;
            }

            if (query->len == ATTR_GREATER_THAN_LEN) {
                return -1;
            }

            if (1 != utils_plainTextToFloat64(query->data + ATTR_GREATER_THAN_LEN, query->len - ATTR_GREATER_THAN_LEN, &floatValue)) {
                return -1;
            }

            attrP->toSet |= ATTR_FLAG_GREATER_THAN;
            attrP->greaterThan = (float)floatValue;
        } else if (utils_strncmp((char *)query->data, ATTR_GREATER_THAN_STR, ATTR_GREATER_THAN_LEN - 1) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_GREATER_THAN)) {
                return -1;
            }

            if (query->len != ATTR_GREATER_THAN_LEN - 1) {
                return -1;
            }

            attrP->toClear |= ATTR_FLAG_GREATER_THAN;
        } else if (utils_strncmp((char *)query->data, ATTR_LESS_THAN_STR, ATTR_LESS_THAN_LEN) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_LESS_THAN)) {
                return -1;
            }

            if (query->len == ATTR_LESS_THAN_LEN) {
                return -1;
            }

            if (1 != utils_plainTextToFloat64(query->data + ATTR_LESS_THAN_LEN, query->len - ATTR_LESS_THAN_LEN, &floatValue)) {
                return -1;
            }

            attrP->toSet |= ATTR_FLAG_LESS_THAN;
            attrP->lessThan = (float)floatValue;
        } else if (utils_strncmp((char *)query->data, ATTR_LESS_THAN_STR, ATTR_LESS_THAN_LEN - 1) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_LESS_THAN)) {
                return -1;
            }

            if (query->len != ATTR_LESS_THAN_LEN - 1) {
                return -1;
            }

            attrP->toClear |= ATTR_FLAG_LESS_THAN;
        } else if (utils_strncmp((char *)query->data, ATTR_STEP_STR, ATTR_STEP_LEN) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_STEP)) {
                return -1;
            }

            if (query->len == ATTR_STEP_LEN) {
                return -1;
            }

            if (1 != utils_plainTextToFloat64(query->data + ATTR_STEP_LEN, query->len - ATTR_STEP_LEN, &floatValue)) {
                return -1;
            }

            if (floatValue < 0) {
                return -1;
            }

            attrP->toSet |= ATTR_FLAG_STEP;
            attrP->step = (float)floatValue;
        } else if (utils_strncmp((char *)query->data, ATTR_STEP_STR, ATTR_STEP_LEN - 1) == 0) {
            if (0 != ((attrP->toSet | attrP->toClear) & ATTR_FLAG_STEP)) {
                return -1;
            }

            if (query->len != ATTR_STEP_LEN - 1) {
                return -1;
            }

            attrP->toClear |= ATTR_FLAG_STEP;
        } else {
            return -1;
        }

        query = query->next;
    }

    return 0;
}


st_server_t *management_makeServerList(st_context_t *contextP, bool bootstrap)
{
    st_object_t *objectP;
    st_object_t *securityObjP = NULL;
    cis_list_t *securityInstP;
    st_server_t *targetServers = NULL;

    for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next) {
        if (objectP->objID == CIS_SECURITY_OBJECT_ID) {
            securityObjP = objectP;
        }
    }

    if (NULL == securityObjP) {
        return NULL;
    }

    if (bootstrap) {
        targetServers = contextP->bootstrapServer;
    } else {
        targetServers = contextP->server;
    }

    securityInstP = std_object_get_securitys(contextP);

    while (securityInstP != NULL) {
        cis_iid_t instid = (cis_iid_t)securityInstP->id;
        securityInstP = securityInstP->next;

        if (CIS_LIST_FIND(targetServers, instid) == NULL) {
            st_data_t *dataP;
            st_server_t *targetP;
            bool bootstrapServer;
            int size = 4;

            dataP = data_new(size);

            if (dataP == NULL) {
                return NULL;
            }

            dataP[0].id = CIS_SECURITY_BOOTSTRAP_ID;
            dataP[1].id = CIS_SECURITY_SHORT_SERVER_ID;
            dataP[2].id = CIS_SECURITY_HOLD_OFF_ID;

            if (std_object_read_handler(contextP, (cis_iid_t)instid, &size, &dataP, securityObjP) != COAP_205_CONTENT) {
                data_free(size, dataP);
                CIS_LOGE("ERROR:makeServer failed.security read error");
                return NULL;
            }

            if (0 == data_decode_bool(dataP + 0, &bootstrapServer)) {
                data_free(size, dataP);
                CIS_LOGE("ERROR:makeServer failed.bootstrap flag get invalid");
                return NULL;
            }


            if ((bootstrapServer ? 1 : 0) != (bootstrap ? 1 : 0)) {
                data_free(size, dataP);
                continue;
            }

            targetP = (st_server_t *)cis_malloc(sizeof(st_server_t));

            if (targetP == NULL) {
                data_free(size, dataP);
                return NULL;
            }

            cis_memset(targetP, 0, sizeof(st_server_t));
            targetP->secObjInstID = (cis_iid_t)instid;
            targetP->shortID = (uint16_t)0;
            targetP->host = std_security_get_host(contextP, (cis_iid_t)targetP->secObjInstID);
            targetP->status = STATE_UNCREATED;

            targetServers = (st_server_t *)CIS_LIST_ADD(targetServers, targetP);

            data_free(size, dataP);

        }
    }

    return targetServers;
}



