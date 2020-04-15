/*******************************************************************************
*
* Copyright (c) 2015 Intel Corporation and others.
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
*    Baijie & Longrong, China Mobile - please refer to the git log
*
*******************************************************************************/
#include "cis_internals.h"
#include "cis_log.h"

#define PRV_LINK_BUFFER_SIZE  512

#define PRV_CONCAT_STR(buf, len, index, str, str_len)\
    {\
        if ( (len)-(index) < (str_len) ) \
            goto TAG_FAILED; \
        cis_memmove( (buf)+(index), (str), (str_len) ); \
        (index) += (str_len); \
    }


static int prv_serializeLinkData(st_context_t *contextP,
                                 st_data_t *tlvP,
                                 st_uri_t *parentUriP,
                                 uint8_t *parentUriStr,
                                 size_t parentUriLen,
                                 uint8_t *buffer,
                                 size_t bufferLen)
{
    int head;
    int res;
    st_uri_t uri;

    head = 0;

    switch (tlvP->type) {
        case DATA_TYPE_UNDEFINE:
            break;

        case DATA_TYPE_STRING:
        case DATA_TYPE_OPAQUE:
        case DATA_TYPE_INTEGER:
        case DATA_TYPE_FLOAT:
        case DATA_TYPE_BOOL:
        case DATA_TYPE_LINK:
            //case cis_data_type_object:
            //case cis_data_type_object_instance:
        {
            if (bufferLen < LINK_ITEM_START_SIZE) {
                return -1;
            }

            cis_memmove(buffer + head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
            head = LINK_ITEM_START_SIZE;

            if (parentUriLen > 0) {
                if (bufferLen - head < parentUriLen) {
                    return -1;
                }

                cis_memmove(buffer + head, parentUriStr, parentUriLen);
                head += parentUriLen;
            }

            if (bufferLen - head < LINK_URI_SEPARATOR_SIZE) {
                return -1;
            }

            cis_memmove(buffer + head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
            head += LINK_URI_SEPARATOR_SIZE;

            res = utils_intToText(tlvP->id, buffer + head, bufferLen - head);

            if (res <= 0) {
                return -1;
            }

            head += res;

            if (tlvP->type == DATA_TYPE_OBJECT_INSTANCE) {
                if (bufferLen - head < LINK_ITEM_DIM_START_SIZE) {
                    return -1;
                }

                cis_memmove(buffer + head, LINK_ITEM_DIM_START, LINK_ITEM_DIM_START_SIZE);
                head += LINK_ITEM_DIM_START_SIZE;

                res = utils_intToText(tlvP->value.asChildren.count, buffer + head, bufferLen - head);

                if (res <= 0) {
                    return -1;
                }

                head += res;

                if (bufferLen - head < LINK_ITEM_ATTR_END_SIZE) {
                    return -1;
                }

                cis_memmove(buffer + head, LINK_ITEM_ATTR_END, LINK_ITEM_ATTR_END_SIZE);
                head += LINK_ITEM_ATTR_END_SIZE;
            } else {
                if (bufferLen - head < LINK_ITEM_END_SIZE) {
                    return -1;
                }

                cis_memmove(buffer + head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
                head += LINK_ITEM_END_SIZE;
            }
        }
        break;

        case DATA_TYPE_OBJECT_INSTANCE:
        case DATA_TYPE_OBJECT: {
            uint8_t uriStr[URI_MAX_STRING_LEN];
            size_t uriLen;
            size_t index;

            if (parentUriLen > 0) {
                if (URI_MAX_STRING_LEN < parentUriLen) {
                    return -1;
                }

                cis_memmove(uriStr, parentUriStr, parentUriLen);
                uriLen = parentUriLen;
            } else {
                uriLen = 0;
            }

            if (URI_MAX_STRING_LEN - uriLen < LINK_URI_SEPARATOR_SIZE) {
                return -1;
            }

            cis_memmove(uriStr + uriLen, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
            uriLen += LINK_URI_SEPARATOR_SIZE;

            res = utils_intToText(tlvP->id, uriStr + uriLen, URI_MAX_STRING_LEN - uriLen);

            if (res <= 0) {
                return -1;
            }

            uriLen += res;

            cis_memmove(&uri, parentUriP, sizeof(st_uri_t));
            uri.instanceId = (cis_iid_t)tlvP->id;
            uri.flag |= URI_FLAG_INSTANCE_ID;

            head = 0;
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, uriStr, uriLen);
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ITEM_END, LINK_ITEM_END_SIZE);

            for (index = 0; index < tlvP->value.asChildren.count; index++) {
                if (tlvP->value.asChildren.array[index].id != URI_INVALID && tlvP->value.asChildren.array[index].type != DATA_TYPE_UNDEFINE) {
                    res = prv_serializeLinkData(contextP, tlvP->value.asChildren.array + index, &uri, uriStr, uriLen, buffer + head, bufferLen - head);

                    if (res < 0) {
                        return -1;
                    }

                    head += res;
                }
            }
        }
        break;


        default:
            return -1;
    }

    return head;
TAG_FAILED:
    return -1;
}

int discover_serialize(st_context_t *contextP,
                       st_uri_t *uriP,
                       int size,
                       st_data_t *dataP,
                       uint8_t **bufferP)
{
    uint8_t baseUriStr[URI_MAX_STRING_LEN];
    int baseUriLen;
    int index;
    size_t head;
    int res;
    st_uri_t tempUri;

    uint8_t *bufferLink = (uint8_t *)cis_malloc(PRV_LINK_BUFFER_SIZE);

    if (bufferLink == NULL) {
        CIS_LOGE("discover serialize malloc failed.");
        return -1;
    }

    CIS_LOGD("discover_serialize size: %d", size);
    baseUriLen = uri_toString(uriP, baseUriStr, URI_MAX_STRING_LEN, NULL);

    if (baseUriLen < 0) {
        goto TAG_FAILED;
    }

    baseUriLen -= 1;

    head = 0;
    cis_memset(&tempUri, 0, sizeof(st_uri_t));

    /* get object level attributes */
    PRV_CONCAT_STR(bufferLink, PRV_LINK_BUFFER_SIZE, head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
    PRV_CONCAT_STR(bufferLink, PRV_LINK_BUFFER_SIZE, head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
    res = utils_intToText(uriP->objectId, bufferLink + head, PRV_LINK_BUFFER_SIZE - head);

    if (res <= 0) {
        goto TAG_FAILED;
    };

    head += res;

    PRV_CONCAT_STR(bufferLink, PRV_LINK_BUFFER_SIZE, head, LINK_ITEM_END, LINK_ITEM_END_SIZE);

    tempUri.objectId = uriP->objectId;

    if (CIS_URI_IS_SET_INSTANCE(uriP)) {
        size_t subHead;

        /* get object instance level attributes */
        subHead = 0;
        PRV_CONCAT_STR(bufferLink + head, PRV_LINK_BUFFER_SIZE - head, subHead, LINK_ITEM_START, LINK_ITEM_START_SIZE);
        PRV_CONCAT_STR(bufferLink + head, PRV_LINK_BUFFER_SIZE - head, subHead, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
        res = utils_intToText(uriP->objectId, bufferLink + head + subHead, PRV_LINK_BUFFER_SIZE - head - subHead);

        if (res <= 0) {
            goto TAG_FAILED;
        }

        subHead += res;
        PRV_CONCAT_STR(bufferLink + head, PRV_LINK_BUFFER_SIZE - head, subHead, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
        res = utils_intToText(uriP->instanceId, bufferLink + head + subHead, PRV_LINK_BUFFER_SIZE - head - subHead);

        if (res <= 0) {
            goto TAG_FAILED;
        }

        subHead += res;
        PRV_CONCAT_STR(bufferLink + head, PRV_LINK_BUFFER_SIZE - head, subHead, LINK_ITEM_END, LINK_ITEM_END_SIZE);
        tempUri.instanceId = uriP->instanceId;
        tempUri.flag = URI_FLAG_INSTANCE_ID;

        head += subHead;
    }

    for (index = 0; index < size && head < PRV_LINK_BUFFER_SIZE; index++) {
        res = prv_serializeLinkData(contextP, dataP + index, uriP, baseUriStr, baseUriLen, bufferLink + head, PRV_LINK_BUFFER_SIZE - head);

        if (res < 0) {
            goto TAG_FAILED;
        }

        head += res;
    }

    if (head > 0) {
        head -= 1;

        *bufferP = (uint8_t *)cis_malloc(head);

        if (*bufferP == NULL) {
            goto TAG_FAILED;
        }

        cis_memmove(*bufferP, bufferLink, head);
    }

    if (bufferLink) {
        cis_free(bufferLink);
    }

    return (int)head;

TAG_FAILED:

    if (bufferLink) {
        cis_free(bufferLink);
    }

    return -1;
}
