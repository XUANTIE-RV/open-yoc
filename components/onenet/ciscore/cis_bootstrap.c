/*******************************************************************************
 *
 * Copyright (c) 2015 Sierra Wireless and others.
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
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *    Baijie & LongRong, China Mobile - Please refer to git log
 *
 *******************************************************************************/
#include "cis_api.h"
#include "cis_internals.h"
#include "cis_log.h"
#include "std_object/std_object.h"




#define PRV_QUERY_BUFFER_LENGTH 200


static void prv_handleBootstrapReply(st_transaction_t *transaction, void *message);
static void prv_requestBootstrap(st_context_t *context, st_server_t *bootstrapServer);
static coap_status_t prv_checkServerStatus(st_server_t *serverP);
static void prv_tagServer(st_context_t *contextP, uint16_t id);
static void prv_tagAllServer(st_context_t *contextP, st_server_t *serverP);



void bootstrap_step(st_context_t *context,
                    cis_time_t currentTime)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *targetP;
    targetP = context->bootstrapServer;

    if (targetP != NULL) {
        CIS_LOGD("bootstrap step status: %s", STR_STATUS(targetP->status));

        switch (targetP->status) {
            case STATE_UNCREATED:
                break;

            case STATE_CONNECTED: {
                targetP->registration = currentTime + context->lifetime;
                targetP->status = STATE_BS_HOLD_OFF;
                prv_requestBootstrap(context, targetP);
            }
            break;

            case STATE_BS_HOLD_OFF:
                break;

            default:
                break;
        }

        CIS_LOGD("bootstrap step status finish(%d): %s", targetP->status, STR_STATUS(targetP->status));
    }

#endif//CIS_ENABLE_BOOTSTRAP
}

coap_status_t bootstrap_handleFinish(st_context_t *context,
                                     void *fromSessionH)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *bootstrapServer;
    bootstrapServer = context->bootstrapServer;

    if (bootstrapServer != NULL
        && bootstrapServer->status == STATE_BS_PENDING) {
        CIS_LOGD("Bootstrap server status changed to STATE_BS_FINISHED");
        bootstrapServer->status = STATE_BS_FINISHED;
        return COAP_204_CHANGED;
    }

#endif//CIS_ENABLE_BOOTSTRAP
    return COAP_IGNORE;
}

void bootstrap_init(st_context_t *context)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *bootstrapServer;
    bootstrapServer = management_makeServerList(context, true);

    if (bootstrapServer != NULL) {
        bootstrapServer->status = STATE_UNCREATED;
        context->bootstrapServer = bootstrapServer;
    } else {
        CIS_LOGE("ERROR:bootstrap_init failed.");
    }

#endif//CIS_ENABLE_BOOTSTRAP
}

/*
 * Reset the bootstrap servers statuses
 *
 * handle LWM2M Servers the client is registered to
 *
 */
void bootstrap_create(st_context_t *context)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *bootstrapServer;
    bootstrapServer = context->bootstrapServer;

    if (bootstrapServer == NULL) {
        CIS_LOGE("ERROR:bootstrapServer create failed,server is NULL");
        return;
    }

    management_createNetwork(context, bootstrapServer);

    if (bootstrapServer->sessionH == NULL) {
        bootstrapServer->status = STATE_CREATE_FAILED;
    }

#endif//CIS_ENABLE_BOOTSTRAP
}

void  bootstrap_connect(st_context_t *context)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *bootstrapServer;
    bootstrapServer = context->bootstrapServer;

    if (bootstrapServer == NULL) {
        CIS_LOGE("ERROR:bootstrapServer create failed,server is NULL");
        return;
    }

    if (!management_connectServer(context, bootstrapServer)) {
        bootstrapServer->status = STATE_CONNECT_FAILED;
    }

#endif//CIS_ENABLE_BOOTSTRAP
}


void  bootstrap_destory(st_context_t *context)
{
#if CIS_ENABLE_BOOTSTRAP
    st_server_t *targetP;
    targetP = context->bootstrapServer;
    management_destoryServer(context, targetP);
    context->bootstrapServer = NULL;
#endif//CIS_ENABLE_BOOTSTRAP
}

/*
 * Returns STATE_BS_PENDING if at least one bootstrap is still pending
 * Returns STATE_BS_FINISHED if at least one bootstrap succeeded and no bootstrap is pending
 * Returns STATE_BS_FAILED if all bootstrap failed.
 */
et_status_t bootstrap_getStatus(st_context_t *context)
{
    et_status_t bs_status;
    bs_status = STATE_BS_FAILED;

#if CIS_ENABLE_BOOTSTRAP
    st_server_t *targetP;

    targetP = context->bootstrapServer;

    if (targetP == NULL) {
        return bs_status;
    }

    bs_status = targetP->status;

    switch (targetP->status) {
        case STATE_BS_FINISHED:
            break;

        case STATE_BS_INITIATED:
            break;

        case STATE_BS_HOLD_OFF:
            break;

        case STATE_BS_PENDING:
            bs_status = STATE_BS_PENDING;
            break;

        case STATE_BS_FAILED:
        case STATE_CREATE_FAILED:
        case STATE_CONNECT_FAILED:
            bs_status = STATE_BS_FAILED;
            break;

        default:
            break;
    }

#endif//CIS_ENABLE_BOOTSTRAP
    return bs_status;
}





coap_status_t bootstrap_handleCommand(st_context_t *contextP,
                                      st_uri_t *uriP,
                                      st_server_t *serverP,
                                      coap_packet_t *message,
                                      coap_packet_t *response)
{
    coap_status_t result = COAP_400_BAD_REQUEST;
#if CIS_ENABLE_BOOTSTRAP
    et_media_type_t format;
    CIS_LOGD("Code: %02X", message->code);

    format = utils_convertMediaType(message->content_type);

    result = prv_checkServerStatus(serverP);

    if (result != COAP_NO_ERROR) {
        return result;
    }


    switch (message->code) {
        case COAP_PUT: {
            if (!CIS_URI_IS_SET_INSTANCE(uriP)) {
                result = COAP_501_NOT_IMPLEMENTED;
            } else {
                st_data_t *dataP = NULL;
                int size = 0;
                int i;

                if (message->payload_len == 0 || message->payload == 0) {
                    result = COAP_400_BAD_REQUEST;
                } else {
                    size = data_parse(uriP, message->payload, message->payload_len, format, &dataP);

                    if (size == 0) {
                        result = COAP_500_INTERNAL_SERVER_ERROR;
                        break;
                    }

                    for (i = 0 ; i < size ; i++) {
                        //if(dataP[i].type == cis_data_type_multiple_object)
                        //if(dataP[i].type == cis_data_type_object)
                        {
                            result = std_object_writeInstance(contextP, uriP, &dataP[i]);

                            if (uriP->objectId == CIS_SECURITY_OBJECT_ID
                                && result == COAP_204_CHANGED) {
                                prv_tagServer(contextP, dataP[i].id);
                            }

                            if (result != COAP_204_CHANGED) { // Stop object create or write when result is error
                                break;
                            }
                        }
                        //else
                        //{
                        //    result = COAP_400_BAD_REQUEST;
                        //}
                    }

                    data_free(size, dataP);
                }
            }
        }
        break;

        case COAP_DELETE: {
            if (CIS_URI_IS_SET_RESOURCE(uriP)) {
                result = COAP_400_BAD_REQUEST;
            } else {
                result = COAP_501_NOT_IMPLEMENTED;
            }
        }
        break;

        case COAP_GET:
        case COAP_POST:
        default:
            result = COAP_400_BAD_REQUEST;
            break;
    }


    if (result == COAP_202_DELETED
        || result == COAP_204_CHANGED) {
        if (serverP->status != STATE_BS_PENDING) {
            serverP->status = STATE_BS_PENDING;
        }
    }

    CIS_LOGD("bootstrap result server status: %s", STR_STATUS(serverP->status));

#endif//CIS_ENABLE_BOOTSTRAP
    return result;
}

coap_status_t bootstrap_handleDeleteAll(st_context_t *contextP,
                                        void *fromSessionH)
{
    coap_status_t result = COAP_405_METHOD_NOT_ALLOWED;
#if CIS_ENABLE_BOOTSTRAP

    st_object_t *objectP;
    st_server_t *serverP;

    serverP = utils_findBootstrapServer(contextP, fromSessionH);

    if (serverP == NULL) {
        return COAP_IGNORE;
    }

    result = prv_checkServerStatus(serverP);

    if (result != COAP_NO_ERROR) {
        return result;
    }

    result = COAP_202_DELETED;

    for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next) {
        st_uri_t uri;

        cis_memset(&uri, 0, sizeof(st_uri_t));
        uri.flag = URI_FLAG_OBJECT_ID;
        uri.objectId = (cis_oid_t)objectP->objID;

        if (objectP->objID == CIS_SECURITY_OBJECT_ID) {
            cis_list_t *instanceP;

            instanceP = std_object_get_securitys(contextP);

            while (NULL != instanceP
                   && result == COAP_202_DELETED) {
                if (instanceP->id == serverP->secObjInstID) {
                    instanceP = instanceP->next;
                } else {
                    uri.flag = URI_FLAG_OBJECT_ID | URI_FLAG_INSTANCE_ID;
                    uri.instanceId = (cis_iid_t)instanceP->id;
                    //TODO
                    result = COAP_405_METHOD_NOT_ALLOWED;
                    //result = object_delete(contextP, &uri);
                    //instanceP = objectP->instanceList;
                }
            }

            if (result == COAP_202_DELETED) {
                prv_tagAllServer(contextP, serverP);
            }
        } else {
            //TODO
            //result = object_delete(contextP, &uri);
            //if (result == COAP_405_METHOD_NOT_ALLOWED)
            //{
            //    // Fake a successful deletion for static objects like the Device object.
            //    result = COAP_202_DELETED;
            //}
            result = COAP_405_METHOD_NOT_ALLOWED;
        }
    }

#endif//CIS_ENABLE_BOOTSTRAP
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////
#if CIS_ENABLE_BOOTSTRAP

static void prv_handleBootstrapReply(st_transaction_t *transaction, void *message)
{
    st_server_t *bootstrapServer = (st_server_t *)transaction->server;
    coap_packet_t *coapMessage = (coap_packet_t *)message;

    CIS_LOGD("bootstrap reply status:%d", bootstrapServer->status);

    if (bootstrapServer->status == STATE_BS_INITIATED) {
        if (NULL != coapMessage && COAP_204_CHANGED == coapMessage->code) {
            CIS_LOGD("Received ACK/2.04, Bootstrap pending...");
            bootstrapServer->status = STATE_BS_PENDING;
        } else {
            CIS_LOGE("bootstrap failed..");
            bootstrapServer->status = STATE_BS_FAILED;
        }
    }
}

// start a device initiated bootstrap
static void prv_requestBootstrap(st_context_t *context,
                                 st_server_t *bootstrapServer)
{
    char query[PRV_QUERY_BUFFER_LENGTH];
    int query_length = 0;
    int res;


    query_length = utils_stringCopy(query, PRV_QUERY_BUFFER_LENGTH, "?ep=");

    if (query_length < 0) {
        bootstrapServer->status = STATE_BS_FAILED;
        return;
    }

    res = utils_stringCopy(query + query_length, PRV_QUERY_BUFFER_LENGTH - query_length, context->endpointName);

    if (res < 0) {
        bootstrapServer->status = STATE_BS_FAILED;
        return;
    }

    query_length += res;

    if (bootstrapServer->sessionH == NULL) {
        //bootstrapServer->sessionH =  management_createConnect(context,bootstrapServer);
        CIS_LOGE("Bootstrap request failed. session is null");
    }

    if (bootstrapServer->sessionH != NULL) {
        st_transaction_t *transaction = NULL;

        CIS_LOGD("Bootstrap server connection opened");

        transaction = transaction_new(COAP_TYPE_CON, COAP_POST, NULL, NULL, context->nextMid++, 4, NULL);

        if (transaction == NULL) {
            bootstrapServer->status = STATE_BS_FAILED;
            return;
        }

        coap_set_header_uri_path(transaction->message, "/"URI_BOOTSTRAP_SEGMENT);
        coap_set_header_uri_query(transaction->message, query);
        transaction->callback = prv_handleBootstrapReply;
        transaction->userData = (void *)context;
        transaction->server = bootstrapServer;
        context->transactionList = (st_transaction_t *)CIS_LIST_ADD(context->transactionList, transaction);

        if (transaction_send(context, transaction) == true) {
            CIS_LOGD("CI bootstrap requested to BS server");
            bootstrapServer->status = STATE_BS_INITIATED;
        }
    } else {
        CIS_LOGD("Connecting bootstrap server failed");
        bootstrapServer->status = STATE_BS_FAILED;
    }
}




static coap_status_t prv_checkServerStatus(st_server_t *serverP)
{
    CIS_LOGD("check server status: %s", STR_STATUS(serverP->status));

    switch (serverP->status) {
        case STATE_BS_HOLD_OFF:
            serverP->status = STATE_BS_PENDING;
            CIS_LOGD("Status changed to: %s", STR_STATUS(serverP->status));
            break;

        case STATE_BS_INITIATED:
            // The ACK was probably lost
            serverP->status = STATE_BS_PENDING;
            CIS_LOGD("Status changed to: %s", STR_STATUS(serverP->status));
            break;

        case STATE_UNCREATED:

        // server initiated bootstrap
        case STATE_BS_PENDING:
            // do nothing
            break;

        case STATE_BS_FINISHED:
        case STATE_BS_FAILED:
        default:
            CIS_LOGW("check server status returning COAP_IGNORE");
            return COAP_IGNORE;
    }

    return COAP_NO_ERROR;
}


static void prv_tagServer(st_context_t *contextP,
                          uint16_t id)
{
    st_server_t *targetP;
    //TODO:
    targetP = (st_server_t *)CIS_LIST_FIND(contextP->bootstrapServer, id);

    if (targetP == NULL) {
        targetP = (st_server_t *)CIS_LIST_FIND(contextP->server, id);
    }

    if (targetP != NULL) {
        targetP->dirty = true;
    }
}


static void prv_tagAllServer(st_context_t *contextP,
                             st_server_t *serverP)
{
    st_server_t *targetP;

    targetP = contextP->bootstrapServer;

    if (targetP != NULL) {
        if (targetP != serverP) {
            targetP->dirty = true;
        }

        targetP = targetP->next;
    }

    targetP = contextP->server;

    while (targetP != NULL) {
        targetP->dirty = true;
        targetP = targetP->next;
    }
}

#endif//CIS_ENABLE_BOOTSTRAP