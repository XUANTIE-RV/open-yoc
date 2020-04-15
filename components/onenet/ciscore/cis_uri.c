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
#include "cis_log.h"


#include <ctype.h>

static int prv_parseNumber(uint8_t *uriString,
                           size_t uriLength,
                           size_t *headP)
{
    int result = 0;

    if (uriString[*headP] == '/') {
        // empty Object Instance ID with resource ID is not allowed
        return -1;
    }

    while (*headP < uriLength && uriString[*headP] != '/') {
        if ('0' <= uriString[*headP] && uriString[*headP] <= '9') {
            result += uriString[*headP] - '0';
            result *= 10;
        } else {
            return -1;
        }

        *headP += 1;
    }

    result /= 10;
    return result;
}


int uri_getNumber(uint8_t *uriString,
                  size_t uriLength)
{
    size_t index = 0;

    return prv_parseNumber(uriString, uriLength, &index);
}


st_uri_t *uri_decode(char *altPath,
                     multi_option_t *uriPath)
{
    st_uri_t *uriP;
    int readNum;

    uriP = (st_uri_t *)cis_malloc(sizeof(st_uri_t));

    if (NULL == uriP) {
        return NULL;
    }

    cis_memset(uriP, 0, sizeof(st_uri_t));

    // Read object ID
    if (NULL != uriPath
        && URI_REGISTRATION_SEGMENT_LEN == uriPath->len
        && 0 == utils_strncmp(URI_REGISTRATION_SEGMENT, (char *)uriPath->data, uriPath->len)) {
        uriP->flag |= URI_FLAG_REGISTRATION;
        uriPath = uriPath->next;

        if (uriPath == NULL) {
            return uriP;
        }
    } else if (NULL != uriPath
               && URI_BOOTSTRAP_SEGMENT_LEN == uriPath->len
               && 0 == utils_strncmp(URI_BOOTSTRAP_SEGMENT, (char *)uriPath->data, uriPath->len)) {
        uriP->flag |= URI_FLAG_BOOTSTRAP;
        uriPath = uriPath->next;

        if (uriPath != NULL) {
            goto error;
        }

        return uriP;
    }

    if ((uriP->flag & URI_MASK_TYPE) != URI_FLAG_REGISTRATION) {
        // Read altPath if any
        if (altPath != NULL) {
            int i;

            if (NULL == uriPath) {
                cis_free(uriP);
                return NULL;
            }

            for (i = 0 ; i < uriPath->len ; i++) {
                if (uriPath->data[i] != altPath[i + 1]) {
                    cis_free(uriP);
                    return NULL;
                }
            }

            uriPath = uriPath->next;
        }

        if (NULL == uriPath || uriPath->len == 0) {
            uriP->flag |= URI_FLAG_DELETE_ALL;
            return uriP;
        }
    }

    readNum = uri_getNumber(uriPath->data, uriPath->len);

    if (readNum < 0 || readNum > URI_MAX_ID) {
        goto error;
    }

    uriP->objectId = (uint16_t)readNum;
    uriP->flag |= URI_FLAG_OBJECT_ID;
    uriPath = uriPath->next;

    if ((uriP->flag & URI_MASK_TYPE) == URI_FLAG_REGISTRATION) {
        if (uriPath != NULL) {
            goto error;
        }

        return uriP;
    }

    uriP->flag |= URI_FLAG_DM;

    if (uriPath == NULL) {
        return uriP;
    }

    // Read object instance
    if (uriPath->len != 0) {
        readNum = uri_getNumber(uriPath->data, uriPath->len);

        if (readNum < 0 || readNum > URI_MAX_ID) {
            goto error;
        }

        uriP->instanceId = (cis_iid_t)readNum;
        uriP->flag |= URI_FLAG_INSTANCE_ID;
    }

    uriPath = uriPath->next;

    if (uriPath == NULL) {
        return uriP;
    }

    // Read resource ID
    if (uriPath->len != 0) {
        // resource ID without an instance ID is not allowed
        if ((uriP->flag & URI_FLAG_INSTANCE_ID) == 0) {
            goto error;
        }

        readNum = uri_getNumber(uriPath->data, uriPath->len);

        if (readNum < 0 || readNum > URI_MAX_ID) {
            goto error;
        }

        uriP->resourceId = (uint16_t)readNum;
        uriP->flag |= URI_FLAG_RESOURCE_ID;
    }

    // must be the last segment
    if (NULL == uriPath->next) {

        return uriP;
    }

error:
    CIS_LOGD("Exiting on error");
    cis_free(uriP);
    return NULL;
}

int uri_stringToUri(const char *buffer,
                    size_t buffer_len,
                    st_uri_t *uriP)
{
    size_t head;
    int readNum;

    CIS_LOGD("buffer_len: %u, buffer: \"%.*s\"", buffer_len, buffer_len, buffer);

    if (buffer == NULL || buffer_len == 0 || uriP == NULL) {
        return 0;
    }

    cis_memset(uriP, 0, sizeof(st_uri_t));

    // Skip any white space
    head = 0;

    while (head < buffer_len && isspace(buffer[head] & 0xFF)) {
        head++;
    }

    if (head == buffer_len) {
        return 0;
    }

    // Check the URI start with a '/'
    if (buffer[head] != '/') {
        return 0;
    }

    head++;

    if (head == buffer_len) {
        return 0;
    }

    // Read object ID
    readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);

    if (readNum < 0 || readNum > URI_MAX_ID) {
        return 0;
    }

    uriP->objectId = (uint16_t)readNum;
    uriP->flag |= URI_FLAG_OBJECT_ID;

    if (buffer[head] == '/') {
        head += 1;
    }

    if (head >= buffer_len) {
        CIS_LOGD("Parsed characters: %u", head);

        return head;
    }

    readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);

    if (readNum < 0 || readNum > URI_MAX_ID) {
        return 0;
    }

    uriP->instanceId = (cis_iid_t)readNum;
    uriP->flag |= URI_FLAG_INSTANCE_ID;

    if (buffer[head] == '/') {
        head += 1;
    }

    if (head >= buffer_len) {
        CIS_LOGD("Parsed characters: %u", head);

        return head;
    }

    readNum = prv_parseNumber((uint8_t *)buffer, buffer_len, &head);

    if (readNum < 0 || readNum > URI_MAX_ID) {
        return 0;
    }

    uriP->resourceId = (uint16_t)readNum;
    uriP->flag |= URI_FLAG_RESOURCE_ID;

    if (head != buffer_len) {
        return 0;
    }

    CIS_LOGD("Parsed characters: %u", head);


    return head;
}

int uri_toString(st_uri_t *uriP,
                 uint8_t *buffer,
                 size_t bufferLen,
                 uri_depth_t *depthP)
{
    size_t head;
    int res;

    CIS_LOGD("bufferLen: %u", bufferLen);


    buffer[0] = '/';

    if (uriP == NULL) {
        if (depthP) {
            *depthP = URI_DEPTH_OBJECT;
        }

        return 1;
    }

    head = 1;

    res = utils_intToText(uriP->objectId, buffer + head, bufferLen - head);

    if (res <= 0) {
        return -1;
    }

    head += res;

    if (head >= bufferLen - 1) {
        return -1;
    }

    if (depthP) {
        *depthP = URI_DEPTH_OBJECT_INSTANCE;
    }

    if (CIS_URI_IS_SET_INSTANCE(uriP)) {
        buffer[head] = '/';
        head++;
        res = utils_intToText(uriP->instanceId, buffer + head, bufferLen - head);

        if (res <= 0) {
            return -1;
        }

        head += res;

        if (head >= bufferLen - 1) {
            return -1;
        }

        if (depthP) {
            *depthP = URI_DEPTH_RESOURCE;
        }

        if (CIS_URI_IS_SET_RESOURCE(uriP)) {
            buffer[head] = '/';
            head++;
            res = utils_intToText(uriP->resourceId, buffer + head, bufferLen - head);

            if (res <= 0) {
                return -1;
            }

            head += res;

            if (head >= bufferLen - 1) {
                return -1;
            }

            if (depthP) {
                *depthP = URI_DEPTH_RESOURCE_INSTANCE;
            }
        }
    }

    buffer[head] = '/';
    head++;

    return head;
}

int uri_make(cis_oid_t objectId, cis_iid_t instanceId, cis_rid_t resourceId, st_uri_t *uriP)
{
    if (uriP == NULL) {
        return -1;
    }

    cis_memset(uriP, 0, sizeof(st_uri_t));


    if (objectId < 0 || objectId > URI_MAX_ID) {
        return -1;
    }

    uriP->objectId = (cis_oid_t)objectId;
    uriP->flag |= URI_FLAG_OBJECT_ID;

    if (instanceId < 0 || instanceId > URI_MAX_ID) {
        return -1;
    }

    if (instanceId == URI_INVALID) {
        return 0;
    }

    uriP->instanceId = (cis_iid_t)instanceId;
    uriP->flag |= URI_FLAG_INSTANCE_ID;

    if (resourceId < 0 || resourceId > URI_MAX_ID) {
        return -1;
    }

    uriP->resourceId = (cis_rid_t)resourceId;
    uriP->flag |= URI_FLAG_RESOURCE_ID;

    return 0;
}


bool uri_exist(const st_uri_t *uri1, const st_uri_t *uri2)
{
    if (uri1 == NULL || uri2 == NULL) {
        return false;
    }

    if (uri1->objectId != uri2->objectId) {
        return false;
    }

    if (CIS_URI_IS_SET_INSTANCE(uri1)) {
        if (!CIS_URI_IS_SET_INSTANCE(uri2)) {
            return false;
        }

        if (uri1->instanceId != uri2->instanceId) {
            return false;
        }
    }

    if (CIS_URI_IS_SET_RESOURCE(uri1)) {
        if (!CIS_URI_IS_SET_RESOURCE(uri2)) {
            return false;
        }

        if (uri1->resourceId != uri2->resourceId) {
            return false;
        }
    }

    return true;
}