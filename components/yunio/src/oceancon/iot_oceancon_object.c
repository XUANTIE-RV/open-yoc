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
 *    Axel Lorente - Please refer to git log
 *    Achim Kraus, Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Ville Skyttä - Please refer to git log
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
 * Implements an object for testing purpose
 *
 *                  Multiple
 * Object |  ID   | Instances | Mandatory |
 *  Test  | 31024 |    Yes    |    No     |
 *
 *  Resources:
 *              Supported    Multiple
 *  Name | ID | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  test |  1 |    R/W     |    No     |    Yes    | Integer | 0-255 |       |             |
 *  exec |  2 |     E      |    No     |    Yes    |         |       |       |             |
 *  dec  |  3 |    R/W     |    No     |    Yes    |  Float  |       |       |             |
 *
 */

#include <liblwm2m.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <yoc/iot.h>

#include "iot_oceancon.h"

#define PRV_TLV_BUFFER_SIZE 64
#define TEST_OBJECT_ID 19

/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
 */
typedef struct _prv_instance_ {
    /*
     * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
     * is the instance scope user data (uint8_t test in this case)
     */
    struct _prv_instance_ *next;    // matches lwm2m_list_t::next
    uint16_t shortID;               // matches lwm2m_list_t::id
} prv_instance_t;


static uint8_t prv_read(uint16_t instanceId,
                        int *numDataP,
                        lwm2m_data_t **dataArrayP,
                        lwm2m_object_t *objectP)
{
    iot_channel_t *ch = (iot_channel_t *)objectP->userData;
    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;
    prv_instance_t *targetP;

    if (instanceId != 0) return COAP_405_METHOD_NOT_ALLOWED;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    if (*numDataP == 0) {
        *dataArrayP = lwm2m_data_new(1);

        if (*dataArrayP == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        (*dataArrayP)->id = 0;

        *numDataP = 1;
    }

    //void *buffer = NULL;
    int ret;

    switch ((*dataArrayP)[0].id) {
        case 0:
#if 0
            if (ch->ch_set == NULL) {
                return COAP_503_SERVICE_UNAVAILABLE;
            }

            ch->ch_set(ch->uData, ch->ch_cfg_arg);
#endif

            if (ctx->udata_to_buffer == NULL) {
                return COAP_503_SERVICE_UNAVAILABLE;
            }

            ret = ctx->udata_to_buffer(ch->uData, (void*)ctx->inter_buffer);
            if (ret < 0) {

                return COAP_503_SERVICE_UNAVAILABLE;
            }

            lwm2m_data_encode_opaque(ctx->inter_buffer, ret, *dataArrayP);

            break;

        default:
            return COAP_404_NOT_FOUND;
    }

    aos_event_set(&ctx->event, 1, 0);

    return COAP_205_CONTENT;
}


static uint8_t prv_write(uint16_t instanceId,
                         int numData,
                         lwm2m_data_t *dataArray,
                         lwm2m_object_t *objectP)
{
    prv_instance_t *targetP;
    int i;
    iot_channel_t *ch = (iot_channel_t *)objectP->userData;
    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;

    if (instanceId != 1) return COAP_405_METHOD_NOT_ALLOWED;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    int ret;
    for (i = 0 ; i < numData ; i++) {
        switch (dataArray[i].id) {
            case 0:
            {
                if (ctx->buffer_to_udata == NULL) {
                    return COAP_503_SERVICE_UNAVAILABLE;
                }

                ret = ctx->buffer_to_udata(ch->uData,
                                           dataArray->value.asBuffer.buffer,
                                           dataArray->value.asBuffer.length);
                if (ret < 0) {

                    return COAP_503_SERVICE_UNAVAILABLE;
                }

                if (ch->ch_set) {
                    ch->ch_set(ch->uData, ch->ch_cfg_arg);
                }

                /* clear update flag */
                yoc_udata_clear_flag_all(ch->uData);
            }
            break;

            case 1:
                return COAP_405_METHOD_NOT_ALLOWED;

            default:
                return COAP_404_NOT_FOUND;
        }
    }

    return COAP_204_CHANGED;
}

lwm2m_object_t *get_test_object(iot_channel_t *ch)
{
    lwm2m_object_t *testObj;

    testObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != testObj) {
        int i;
        prv_instance_t *targetP;

        memset(testObj, 0, sizeof(lwm2m_object_t));

        testObj->objID = TEST_OBJECT_ID;

        for (i = 0 ; i < 2 ; i++) {
            targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));

            if (NULL == targetP) {
                free_test_object(testObj);
                return NULL;
            }

            memset(targetP, 0, sizeof(prv_instance_t));
            targetP->shortID = i;
            testObj->instanceList = LWM2M_LIST_ADD(testObj->instanceList, targetP);
        }

        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying his disponibility, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        testObj->readFunc = prv_read;
        testObj->discoverFunc = NULL;
        testObj->writeFunc = prv_write;
        testObj->executeFunc = NULL;
        testObj->createFunc = NULL;
        testObj->deleteFunc = NULL;
        testObj->userData   = ch;
    }

    return testObj;
}

void free_test_object(lwm2m_object_t *object)
{
    LWM2M_LIST_FREE(object->instanceList);

    if (object->userData != NULL) {
        //lwm2m_free(object->userData);
        object->userData = NULL;
    }

    lwm2m_free(object);
}
