#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"

#define CIS_SECURITY_URI_ID                 0
#define CIS_SECURITY_BOOTSTRAP_ID           1
#define CIS_SECURITY_MODE_ID            2
#define CIS_SECURITY_PUBLIC_KEY_ID          3
#define CIS_SECURITY_SERVER_PUBLIC_KEY_ID   4
#define CIS_SECURITY_SECRET_KEY_ID          5
#define CIS_SECURITY_SMS_SECURITY_ID        6
#define CIS_SECURITY_SMS_KEY_PARAM_ID       7
#define CIS_SECURITY_SMS_SECRET_KEY_ID      8
#define CIS_SECURITY_SMS_SERVER_NUMBER_ID   9
#define CIS_SECURITY_SHORT_SERVER_ID        10
#define CIS_SECURITY_HOLD_OFF_ID            11




typedef struct _security_instance_ {
    struct _security_instance_ *next;         // matches lwm2m_list_t::next
    cis_listid_t                 instanceId;  // matches lwm2m_list_t::id
    char                        *host;        // ip address;
    bool                         isBootstrap;
    uint8_t                      securityMode;
    char                        *publicIdentity;
    uint16_t                     publicIdLen;
    char                        *serverPublicKey;
    uint16_t                     serverPublicKeyLen;
    char                        *secretKey;
    uint16_t                     secretKeyLen;
    uint8_t                      smsSecurityMode;
    char                        *smsParams; // SMS binding key parameters
    uint16_t                     smsParamsLen;
    char                        *smsSecret; // SMS binding secret key
    uint16_t                     smsSecretLen;
    uint16_t                     shortID;
    uint32_t                     clientHoldOffTime;
} security_instance_t;


static uint8_t prv_get_value(st_data_t *dataP,
                             security_instance_t *targetP)
{
    switch (dataP->id) {
        case CIS_SECURITY_URI_ID:
            data_encode_string(targetP->host, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_BOOTSTRAP_ID:
            data_encode_bool(targetP->isBootstrap, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SECURITY_ID:
            data_encode_int(targetP->securityMode, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_PUBLIC_KEY_ID:
            data_encode_opaque((uint8_t *)targetP->publicIdentity, targetP->publicIdLen, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SERVER_PUBLIC_KEY_ID:
            data_encode_opaque((uint8_t *)targetP->serverPublicKey, targetP->serverPublicKeyLen, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SECRET_KEY_ID:
            data_encode_opaque((uint8_t *)targetP->secretKey, targetP->secretKeyLen, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SMS_SECURITY_ID:
            data_encode_int(targetP->smsSecurityMode, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SMS_KEY_PARAM_ID:
            data_encode_opaque((uint8_t *)targetP->smsParams, targetP->smsParamsLen, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SMS_SECRET_KEY_ID:
            data_encode_opaque((uint8_t *)targetP->smsSecret, targetP->smsSecretLen, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SMS_SERVER_NUMBER_ID:
            data_encode_int(0, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_SHORT_SERVER_ID:
            data_encode_int(targetP->shortID, dataP);
            return COAP_205_CONTENT;

        case CIS_SECURITY_HOLD_OFF_ID:
            data_encode_int(targetP->clientHoldOffTime, dataP);
            return COAP_205_CONTENT;

        default:
            return COAP_404_NOT_FOUND;
    }
}


static security_instance_t *prv_security_find(st_context_t *contextP, cis_iid_t instanceId)
{
    security_instance_t *targetP;
    targetP = (security_instance_t *)cis_list_find(std_object_get_securitys(contextP), instanceId);

    if (NULL != targetP) {
        return targetP;
    }

    return NULL;
}


#ifdef CIS_ENABLE_BOOTSTRAP


cis_coapret_t std_security_read(st_context_t *contextP, cis_iid_t instanceId,
                                int *numDataP,
                                st_data_t **dataArrayP,
                                st_object_t *objectP)
{
    security_instance_t *targetP;
    uint8_t result;
    int i;

    targetP = prv_security_find(contextP, instanceId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full instance ?
    if (*numDataP == 0) {
        uint16_t resList[] = {CIS_SECURITY_URI_ID,
                              CIS_SECURITY_BOOTSTRAP_ID,
                              CIS_SECURITY_SECURITY_ID,
                              CIS_SECURITY_PUBLIC_KEY_ID,
                              CIS_SECURITY_SERVER_PUBLIC_KEY_ID,
                              CIS_SECURITY_SECRET_KEY_ID,
                              CIS_SECURITY_SMS_SECURITY_ID,
                              CIS_SECURITY_SMS_KEY_PARAM_ID,
                              CIS_SECURITY_SMS_SECRET_KEY_ID,
                              CIS_SECURITY_SMS_SERVER_NUMBER_ID,
                              CIS_SECURITY_SHORT_SERVER_ID,
                              CIS_SECURITY_HOLD_OFF_ID,
                             };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = data_new(nbRes);

        if (*dataArrayP == NULL) {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }

        *numDataP = nbRes;

        for (i = 0 ; i < nbRes ; i++) {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;

    do {
        result = prv_get_value((*dataArrayP) + i, targetP);
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

cis_coapret_t std_security_write(st_context_t *contextP, cis_iid_t instanceId,
                                 int numData,
                                 st_data_t *dataArray,
                                 st_object_t *objectP)
{
    security_instance_t *targetP;
    int i;
    uint8_t result = COAP_204_CHANGED;

    targetP = prv_security_find(contextP, instanceId);

    if (NULL == targetP) {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

    do {
        switch (dataArray[i].id) {
            case CIS_SECURITY_URI_ID:
                if (targetP->host != NULL) {
                    cis_free(targetP->host);
                }

                targetP->host = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);

                if (targetP->host != NULL) {
                    cis_memset(targetP->host, 0, dataArray[i].asBuffer.length + 1);
                    utils_stringCopy(targetP->host, dataArray[i].asBuffer.length - URI_HEADER_HOST_LEN - URI_TAILER_HOST_LEN, (char *)dataArray[i].asBuffer.buffer + URI_HEADER_HOST_LEN);
                    result = COAP_204_CHANGED;
                } else {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                break;

            case CIS_SECURITY_BOOTSTRAP_ID:
                if (1 == data_decode_bool(dataArray + i, &(targetP->isBootstrap))) {
                    result = COAP_204_CHANGED;
                } else {
                    result = COAP_400_BAD_REQUEST;
                }

                break;

            case CIS_SECURITY_SECURITY_ID: {
                int64_t value;

                if (1 == data_decode_int(dataArray + i, &value)) {
                    if (value >= 0 && value <= 3) {
                        targetP->securityMode = (uint8_t)value;
                        result = COAP_204_CHANGED;
                    } else {
                        result = COAP_406_NOT_ACCEPTABLE;
                    }
                } else {
                    result = COAP_400_BAD_REQUEST;
                }
            }
            break;

            case CIS_SECURITY_PUBLIC_KEY_ID:
                if (targetP->publicIdentity != NULL) {
                    cis_free(targetP->publicIdentity);
                }

                targetP->publicIdentity = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);

                if (targetP->publicIdentity != NULL) {
                    cis_memset(targetP->publicIdentity, 0, dataArray[i].asBuffer.length + 1);
                    cis_memcpy(targetP->publicIdentity, (char *)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
                    targetP->publicIdLen = (uint16_t)dataArray[i].asBuffer.length;
                    result = COAP_204_CHANGED;
                } else {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                break;

            case CIS_SECURITY_SERVER_PUBLIC_KEY_ID:
                if (targetP->serverPublicKey != NULL) {
                    cis_free(targetP->serverPublicKey);
                }

                targetP->serverPublicKey = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);
                cis_memset(targetP->serverPublicKey, 0, dataArray[i].asBuffer.length + 1);

                if (targetP->serverPublicKey != NULL) {
                    cis_memcpy(targetP->serverPublicKey, (char *)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
                    targetP->serverPublicKeyLen = (uint16_t)dataArray[i].asBuffer.length;
                    result = COAP_204_CHANGED;
                } else {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                break;

            case CIS_SECURITY_SECRET_KEY_ID:
                if (targetP->secretKey != NULL) {
                    cis_free(targetP->secretKey);
                }

                targetP->secretKey = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);

                if (targetP->secretKey != NULL) {
                    cis_memset(targetP->secretKey, 0, dataArray[i].asBuffer.length + 1);
                    cis_memcpy(targetP->secretKey, (char *)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
                    targetP->secretKeyLen = (uint16_t)dataArray[i].asBuffer.length;
                    result = COAP_204_CHANGED;
                } else {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                }

                break;

            case CIS_SECURITY_SMS_SECURITY_ID:
                // Let just ignore this
                result = COAP_204_CHANGED;
                break;

            case CIS_SECURITY_SMS_KEY_PARAM_ID:
                // Let just ignore this
                result = COAP_204_CHANGED;
                break;

            case CIS_SECURITY_SMS_SECRET_KEY_ID:
                // Let just ignore this
                result = COAP_204_CHANGED;
                break;

            case CIS_SECURITY_SMS_SERVER_NUMBER_ID:
                // Let just ignore this
                result = COAP_204_CHANGED;
                break;

            case CIS_SECURITY_SHORT_SERVER_ID: {
                int64_t value;

                if (1 == data_decode_int(dataArray + i, &value)) {
                    if (value >= 0 && value <= 0xFFFF) {
                        targetP->shortID = (uint16_t)value;
                        result = COAP_204_CHANGED;
                    } else {
                        result = COAP_406_NOT_ACCEPTABLE;
                    }
                } else {
                    result = COAP_400_BAD_REQUEST;
                }
            }
            break;

            case CIS_SECURITY_HOLD_OFF_ID: {
                int64_t value;

                if (1 == data_decode_int(dataArray + i, &value)) {
                    if (value >= 0 && value <= 0xFFFF) {
                        targetP->clientHoldOffTime = (uint32_t)value;
                        result = COAP_204_CHANGED;
                    } else {
                        result = COAP_406_NOT_ACCEPTABLE;
                    }
                } else {
                    result = COAP_400_BAD_REQUEST;
                }

                break;
            }

            default:
                return COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

#endif//CIS_ENABLE_BOOTSTRAP




bool std_security_create(st_context_t *contextP, int instanceId,
                         const char *serverHost,
                         bool isBootstrap,
                         st_object_t *securityObj)
{
    security_instance_t *instSecurity = NULL;
    security_instance_t *targetP = NULL;
    cis_instcount_t instBytes = 0;
    cis_instcount_t instCount = 0;
    cis_iid_t instIndex;

    if (NULL == securityObj) {
        return false;
    }

    // Manually create an hardcoded instance
    targetP = (security_instance_t *)cis_malloc(sizeof(security_instance_t));

    if (NULL == targetP) {
        return false;
    }

    cis_memset(targetP, 0, sizeof(security_instance_t));
    targetP->instanceId = (uint16_t)instanceId;
    targetP->host = (char *)cis_malloc(utils_strlen(serverHost) + 1);

    if (targetP->host == NULL) {
        cis_free(targetP);
        return false;
    }

    cis_memset(targetP->host, 0, utils_strlen(serverHost) + 1);
    utils_stringCopy(targetP->host, utils_strlen(serverHost), serverHost);

    targetP->securityMode = CIS_SECURITY_MODE_NONE;
    targetP->publicIdentity = NULL;
    targetP->publicIdLen = 0;
    targetP->secretKey = NULL;
    targetP->secretKeyLen = 0;

    targetP->isBootstrap = isBootstrap;
    targetP->shortID = 0;
    targetP->clientHoldOffTime = 10;

    instSecurity = (security_instance_t *)std_object_put_securitys(contextP, (cis_list_t *)targetP);

    instCount = (cis_instcount_t)CIS_LIST_COUNT(instSecurity);

    if (instCount == 0) {
        cissys_free(targetP->host);
        cissys_free(targetP);
        return false;
    }

    /*first security object instance
     *don't malloc instance bitmap ptr*/
    if (instCount == 2) {
        return true;
    }

    securityObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;

    if (securityObj->instBitmapBytes < instBytes) {
        if (securityObj->instBitmapBytes != 0 && securityObj->instBitmapPtr != NULL) {
            cissys_free(securityObj->instBitmapPtr);
        }

        securityObj->instBitmapPtr = (uint8_t *)cis_malloc(instBytes);
        securityObj->instBitmapBytes = instBytes;
    }

    cissys_memset(securityObj->instBitmapPtr, 0, instBytes);
    targetP = instSecurity;

    for (instIndex = 0; instIndex < instCount; instIndex++) {
        uint16_t instBytePos = (uint16_t)targetP->instanceId / 8;
        uint16_t instByteOffset = 7 - (targetP->instanceId % 8);
        securityObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }


    return true;
}



void std_security_clean(st_context_t *contextP)
{
    security_instance_t *deleteInst;
    security_instance_t *securityInstance = (security_instance_t *)std_object_get_securitys(contextP);

    while (securityInstance != NULL) {
        deleteInst = securityInstance;
        securityInstance = securityInstance->next;

        std_object_remove_securitys(contextP, (cis_list_t *)deleteInst);

        if (NULL != deleteInst->host) {
            cis_free(deleteInst->host);
        }

        if (deleteInst->securityMode == CIS_SECURITY_MODE_PRE_SHARED_KEY) {
            cis_free(deleteInst->publicIdentity);
            cis_free(deleteInst->secretKey);
        }

        cis_free(deleteInst);
    }
}



char *std_security_get_host(st_context_t *contextP, cis_iid_t InstanceId)
{
    security_instance_t *targetP = prv_security_find(contextP, InstanceId);

    if (NULL != targetP) {
        return utils_strdup(targetP->host);
    }

    return NULL;
}