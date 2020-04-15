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

static bool prv_getInstId(uint8_t *inst_bitmap, cis_instcount_t index, cis_iid_t *iid);
static int  prv_getObjectTemplate(uint8_t *buffer, size_t length, uint32_t id);


coap_status_t object_asynAckReadData(st_context_t *context, st_request_t *request, cis_coapret_t result)
{
    et_media_type_t formatP;
    uint8_t *bufferP = NULL;
    int32_t length = 0;
    coap_packet_t packet[1];

    st_data_t *value = request->respData;
    uint16_t size = request->respDataCount;

    if (result == COAP_205_CONTENT && value != NULL) {
        if (request->type == CALLBACK_TYPE_READ) {
            formatP = request->format;
            length = data_serialize(&request->uri, size, value, &formatP, &bufferP);

            if (length <= -1) {
                CIS_LOGD("object_asyn_ack_readdata data serialize failed.");
                return COAP_500_INTERNAL_SERVER_ERROR;
            }
        } else if (request->type == CALLBACK_TYPE_DISCOVER) {
            formatP = LWM2M_CONTENT_LINK;
            length = discover_serialize(context, &request->uri, size, value, &bufferP);

            if (length <= 0) {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }
        }
    }

    coap_init_message(packet, COAP_TYPE_ACK, result, (uint16_t)request->mid);
    coap_set_header_token(packet, request->token, request->tokenLen);

    if (length > 0) {
        coap_set_header_content_type(packet, formatP);
        coap_set_payload(packet, bufferP, length);
    }

    packet_send(context, packet);
    if (length > 0)
        cis_free(bufferP);

    return COAP_NO_ERROR;
}

coap_status_t object_asynAckNodata(st_context_t *context, st_request_t *request, cis_coapret_t result)
{
    coap_packet_t response[1];
    coap_init_message(response, COAP_TYPE_ACK, result, (uint16_t)request->mid);
    coap_set_header_token(response, request->token, request->tokenLen);
    packet_send(context, response);

    return COAP_NO_ERROR;
}


void object_removeAll(st_context_t *context)
{
    while (NULL != context->objectList) {
        st_object_t *targetP;
        targetP = context->objectList;
        context->objectList = context->objectList->next;

        if (targetP->instBitmapPtr) {
            cis_free(targetP->instBitmapPtr);
        }

        cis_free(targetP);
    }

    context->objectList = NULL;
}




coap_status_t object_read(st_context_t *contextP, st_uri_t *uriP, coap_packet_t *message)
{
    coap_status_t   result;
    st_object_t    *targetP;
    uint8_t        *bitmapPtr;

    CIS_LOG_URI("object read", uriP);

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    if (CIS_URI_IS_SET_INSTANCE(uriP)) {
        bitmapPtr = targetP->instBitmapPtr;

        if (!object_checkInstExisted(bitmapPtr, uriP->instanceId)) {
            return COAP_404_NOT_FOUND;
        }
    }


    result = contextP->callback.onRead(contextP, uriP, message->mid);

    if (result == COAP_206_CONFORM) {
        packet_asynPushRequest(contextP, message, CALLBACK_TYPE_READ, message->mid);
        result = COAP_IGNORE;
    }

    return result;
}



coap_status_t object_write(st_context_t *contextP,
                           st_uri_t *uriP,
                           et_media_type_t format,
                           uint8_t *buffer,
                           size_t length,
                           coap_packet_t *message)
{
    coap_status_t result = COAP_NO_ERROR;
    st_object_t *targetP;
    cis_rescount_t resCount = 0;
    cis_rescount_t resIndex;
    st_data_t *dataP;

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);

    if (NULL == targetP) {
        result = COAP_404_NOT_FOUND;
    }

    if (result == COAP_NO_ERROR && CIS_URI_IS_SET_INSTANCE(uriP)) {
        if (!object_checkInstExisted(targetP->instBitmapPtr, uriP->instanceId)) {
            result = COAP_404_NOT_FOUND;
        }
    }

    if (result == COAP_NO_ERROR) {
        //NOTE: before object_write called, caller guarantees uriP shall carry instanceID,
        //  So we don't do check the intance ID here.
        // And uriP might NOT carry resource ID while dataP is TLV type and have multiple dataset
        resCount = (cis_rescount_t)data_parse(uriP, buffer, length, format, &dataP);

        if (resCount <= 0) {
            return COAP_406_NOT_ACCEPTABLE;
        }


        if (dataP->type == DATA_TYPE_STRING ||
            dataP->type == DATA_TYPE_OPAQUE ||
            dataP->type == DATA_TYPE_INTEGER ||
            dataP->type == DATA_TYPE_FLOAT ||
            dataP->type == DATA_TYPE_BOOL) {
            cis_data_t *data;

            data = (cis_data_t *)cis_malloc(sizeof(cis_data_t) * resCount);

            if (data == NULL) {
                /* c sky add: fix mem leak */
                data_free(resCount, dataP);
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            result = COAP_206_CONFORM;

            for (resIndex = 0; resIndex < resCount; resIndex++) {
                data[resIndex].id = dataP[resIndex].id;
                data[resIndex].type = (cis_datatype_t)dataP[resIndex].type;
                data[resIndex].asBuffer.length = dataP[resIndex].asBuffer.length;
                data[resIndex].asBuffer.buffer = utils_bufferdup(dataP[resIndex].asBuffer.buffer, dataP[resIndex].asBuffer.length);
                cis_memcpy(&data[resIndex].value.asFloat, &dataP[resIndex].value.asFloat, sizeof(data[resIndex].value));
            }

            uri_make(uriP->objectId, uriP->instanceId, URI_INVALID, uriP);
            result = contextP->callback.onWrite(contextP, uriP, data, resCount, message->mid);

            for (resIndex = 0; resIndex < resCount; resIndex++) {
                if (data[resIndex].type == cis_data_type_opaque || data[resIndex].type == cis_data_type_string) {
                    cis_free(data[resIndex].asBuffer.buffer);
                }
            }

            cis_free(data);

        }
        /* c sky add: data_free should be here */
        data_free(resCount, dataP);

    }

    if (result == COAP_206_CONFORM) {
        packet_asynPushRequest(contextP, message, CALLBACK_TYPE_WRITE, message->mid);
        result = COAP_IGNORE;
    }

    return result;
}

coap_status_t object_execute(st_context_t *contextP,
                             st_uri_t *uriP,
                             uint8_t *buffer,
                             size_t length,
                             coap_packet_t *message)
{
    st_object_t *targetP;
    coap_status_t result = COAP_NO_ERROR;
    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    if (CIS_URI_IS_SET_INSTANCE(uriP)) {
        if (!object_checkInstExisted(targetP->instBitmapPtr, uriP->instanceId)) {
            result = COAP_404_NOT_FOUND;
        }
    }

    if (result == COAP_NO_ERROR) {
        //NOTE: refer 5.4.5, caller makes sure uriP has carried instanceID, resourceID
        result = contextP->callback.onExec(contextP, uriP, buffer, length, message->mid);
    }


    if (result == COAP_206_CONFORM) {
        packet_asynPushRequest(contextP, message, CALLBACK_TYPE_EXECUTE, message->mid);
        result = COAP_IGNORE;
    }

    return result;
}


coap_status_t object_discover(st_context_t *contextP, st_uri_t *uriP, coap_packet_t *message)
{
    coap_status_t result;
    st_object_t *targetP;
    uint8_t        *bitmapPtr;

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }


    if (CIS_URI_IS_SET_INSTANCE(uriP)) { //single instance
        bitmapPtr = targetP->instBitmapPtr;

        if (!object_checkInstExisted(bitmapPtr, uriP->instanceId)) {
            return COAP_404_NOT_FOUND;
        }
    }

    result = contextP->callback.onDiscover(contextP, uriP, message->mid);

    if (result == COAP_206_CONFORM) {
        packet_asynPushRequest(contextP, message, CALLBACK_TYPE_DISCOVER, message->mid);
        result = COAP_IGNORE;
    }

    return result;
}

bool object_isInstanceNew(st_context_t *contextP,
                          cis_oid_t  objectId,
                          cis_iid_t  instanceId)
{
    st_object_t *targetP;

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, objectId);

    if (targetP != NULL) {

        uint8_t *bitmapPtr = targetP->instBitmapPtr;
        cis_instcount_t bitmapBytes = targetP->instBitmapBytes;

        if (bitmapBytes == 0) {
            return false;
        }

        if (!object_checkInstExisted(bitmapPtr, instanceId)) {
            return false;
        }
    }

    return true;
}


int object_getRegisterPayload(st_context_t *contextP,
                              uint8_t *buffer,
                              size_t bufferLen)
{
    size_t index;
    int result;
    st_object_t *objectP;
    cis_iid_t bitmapIndex;


    // index can not be greater than bufferLen
    index = 0;

    result = utils_stringCopy((char *)buffer, bufferLen, REG_START);

    if (result < 0) {
        return 0;
    }

    index += result;
    //printf("0 buffer:%s,index:%d\n",buffer,index);

    if ((contextP->altPath != NULL)
        && (contextP->altPath[0] != 0)) {
        result = utils_stringCopy((char *)buffer + index, bufferLen - index, contextP->altPath);
    } else {
        result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_DEFAULT_PATH);
    }

    if (result < 0) {
        return 0;
    }

    index += result;
    //printf("1 buffer:%s,index:%d\n",buffer,index);
    result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_LWM2M_RESOURCE_TYPE);

    if (result < 0) {
        return 0;
    }

    index += result;

    //printf("2 buffer:%s,index:%d\n",buffer,index);
    for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next) {
        size_t start;
        size_t length;

        start = index;
        result = prv_getObjectTemplate(buffer + index, bufferLen - index, objectP->objID);

        if (result < 0) {
            return 0;
        }

        length = result;
        index += length;

        uint8_t *bitmapPtr = objectP->instBitmapPtr;

        cis_instcount_t bitmapBytes = objectP->instBitmapBytes;
        cis_instcount_t bitmapCount = objectP->instBitmapCount;

        //printf("3 buffer:%s,index:%d,0x%x,bitmapCount:%d,objectP->objID:%d,bitmapBytes:%d\n",buffer,index,*bitmapPtr,bitmapCount,objectP->objID,bitmapBytes);
        if (bitmapBytes == 0) {
            index--;  //NOTE: remove the prepended '/'
            result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);

            if (result < 0) {
                return 0;
            }

            index += result;
            //printf("4 buffer:%s,index:%d\n",buffer,index);
        } else {
            for (bitmapIndex = 0; bitmapIndex < bitmapCount; bitmapIndex++) {
                cis_iid_t instanceId;;

                if (!prv_getInstId(bitmapPtr, bitmapIndex, &instanceId)) {
                    //printf("x buffer:%s,index:%d,bitmapIndex:%d\n",buffer,index,bitmapIndex);
                    continue;
                }

                if (bufferLen - index <= length) {
                    return 0;
                }

                if (index != start + length) {
                    cis_memcpy(buffer + index, buffer + start, length);
                    index += length;
                    // printf("5 buffer:%s,index:%d\n",buffer,index);
                }

                result = utils_intCopy((char *)buffer + index, bufferLen - index, instanceId);

                if (result < 0) {
                    return 0;
                }

                index += result;
                //printf("6 buffer:%s,index:%d,bitmapIndex:%d,instanceId:%d\n",buffer,index,bitmapIndex,instanceId);
                result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);

                if (result < 0) {
                    return 0;
                }

                index += result;
                //printf("7 buffer:%s,index:%d\n",buffer,index);
            }
        }
    }

    if (index > 0) {
        index = index - 1;  // remove trailing ','
    }

    buffer[index] = 0;
    //	printf("8 buffer:%s,index:%d\n",buffer,index);
    return index;
}


bool object_checkInstExisted(uint8_t *inst_bitmap, cis_iid_t iid)
{
    cis_instcount_t bytes = iid / 8;
    cis_instcount_t bits = iid % 8;

    if (inst_bitmap == NULL) {
        return true;
    }

    return (((*(inst_bitmap + bytes) >> (7 - bits)) & 0x01) > 0);
}

//////////////////////////////////////////////////////////////////////////

static bool prv_getInstId(uint8_t *inst_bitmap, cis_instcount_t index, cis_iid_t *iid)
{
    if (inst_bitmap == NULL) {
        return 0;
    }

    cis_instcount_t bytes = index / 8;
    cis_instcount_t bits = index % 8;

    if ((*(inst_bitmap + bytes) >> (7 - bits)) & 0x01) {
        if (iid != NULL) {
            *iid = index;
        }

        return true;
    }

    return false;
}

static int prv_getObjectTemplate(uint8_t *buffer, size_t length, uint32_t id)
{
    int index;
    int result;

    if (length < REG_OBJECT_MIN_LEN) {
        return -1;
    }

    buffer[0] = '<';
    buffer[1] = '/';
    index = 2;

    result = utils_intCopy((char *)buffer + index, length - index, id);

    if (result < 0) {
        return -1;
    }

    index += result;

    if (length - index < REG_OBJECT_MIN_LEN - 3) {
        return -1;
    }

    buffer[index] = '/';
    index++;

    return index;
}


