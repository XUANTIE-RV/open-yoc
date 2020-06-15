/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "tee_storage_api.h"
#include "tee_internel_api.h"
#include "string.h"

#define CONFIG_TA_ATTR_COUNT            4

static tee_attribute             ta_attr_s[CONFIG_TA_ATTR_COUNT];
static struct tee_objecthandle_t ta_objhandle_s;
static tee_objecthandle          ta_objhandle;

void tee_getobjectinfo(tee_objecthandle object, tee_objectinfo *objectinfo)
{
    if ((object == NULL) || (objectinfo == NULL)) {
        return;
    }

    memcpy(objectinfo, &object->objectinfo, sizeof(tee_objectinfo));
}

tee_result tee_allocatetransientobject(uint32_t objecttype, uint32_t maxobjectsize, tee_objecthandle *object)
{
    memset(&ta_attr_s, 0, sizeof(tee_attribute));
    memset(&ta_objhandle_s, 0, sizeof(struct tee_objecthandle_t));
    ta_objhandle = &ta_objhandle_s;

    ta_objhandle->objectinfo.objectusage    = 0xFFFFFFFF;
    ta_objhandle->objectinfo.maxobjectsize  = maxobjectsize;
    ta_objhandle->objectinfo.objectype      = objecttype;
    ta_objhandle->objectinfo.objectsize     = 0;
    ta_objhandle->objectinfo.datasize       = 0;
    ta_objhandle->objectinfo.handleflags    = 0x00000000;

    ta_objhandle->attrcount                 = CONFIG_TA_ATTR_COUNT;

    ta_objhandle->attr                      = (tee_attribute *)&ta_attr_s;

    *object                                 = ta_objhandle;
    return TEE_SUCCESS;
}

void tee_freetransientobject(tee_objecthandle *object)
{

}

void tee_resettransientobject(tee_objecthandle object)
{

}

tee_result tee_populatetransientobject(tee_objecthandle object, tee_attribute *attrs, uint32_t attrcount)
{
    uint8_t i;

    if ((object == NULL) || (attrs == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    for (i = 0; i < attrcount; i++) {
        memcpy(&object->attr[i], &attrs[i], sizeof(tee_attribute));
    }

    return TEE_SUCCESS;
}

void tee_initrefattribute(tee_attribute *attr, uint32_t attributeid, void *buffer, size_t length)
{

}

void tee_initvalueattribute(tee_attribute *attr, uint32_t attributeid, uint32_t a, uint32_t b)
{

}

void tee_copyobjectattributes(tee_objecthandle destobject, tee_objecthandle srcobject)
{

}

tee_result tee_generatekey(tee_objecthandle object, uint32_t keysize, tee_attribute *params, uint32_t paramcount)
{

    return TEE_SUCCESS;
}

tee_result tee_setkey(tee_objecthandle object, uint8_t *key, uint32_t keysize)
{
    if ((object == NULL) || (key == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (object->objectinfo.maxobjectsize < keysize) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (object->objectinfo.objectype == TEE_TYPE_DES3 || object->objectinfo.objectype == TEE_TYPE_DES) {
        object->attr[0].content.ref.buffer = key;
        object->attr[0].content.ref.length = keysize;
    } else if (object->objectinfo.objectype == TEE_TYPE_AES) {
        object->attr[0].content.ref.buffer = key;
        object->attr[0].content.ref.length = keysize;
    } else {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    object->objectinfo.handleflags += TEE_HANDLE_FLAG_INITIALIZED;

    return TEE_SUCCESS;
}

tee_result tee_openpersistentobject(uint32_t storageid, void *objectid, size_t objectidlen,
                                    uint32_t flags, tee_objecthandle *object)
{

    return TEE_SUCCESS;
}

tee_result tee_createpersistentobject(uint32_t storageid, void *objectid, size_t objectidlen,
                                      uint32_t flags, tee_objecthandle attributes,
                                      void *initialdata, size_t initialdatalen,
                                      tee_objecthandle *object)
{

    return TEE_SUCCESS;
}

void tee_closeanddeletepersistentobject(tee_objecthandle object)
{

}

