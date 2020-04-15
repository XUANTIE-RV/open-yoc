#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"

extern cis_coapret_t std_security_read(st_context_t *contextP, cis_iid_t instanceId, int *numDataP, st_data_t **dataArrayP, st_object_t *objectP);
extern cis_coapret_t std_security_write(st_context_t *contextP, cis_iid_t instanceId, int numData, st_data_t *dataArray, st_object_t *objectP);


//mdy by zpr static cis_list_t* std_instSecurity = NULL;
static const struct st_std_object_callback_mapping std_object_callback_mapping[1] = {
    {CIS_SECURITY_OBJECT_ID, std_security_read, std_security_write, NULL},
};

cis_list_t *std_object_get_securitys(st_context_t *contextP)
{
    return contextP->std_instSecurity;
}

cis_list_t *std_object_put_securitys(st_context_t *contextP, cis_list_t *targetP)
{
    contextP->std_instSecurity = CIS_LIST_ADD(contextP->std_instSecurity, targetP);
    return contextP->std_instSecurity;
}

void std_object_remove_securitys(st_context_t *contextP, cis_list_t *targetP)
{
    contextP->std_instSecurity = CIS_LIST_RM(contextP->std_instSecurity, targetP->id, NULL);
}


cis_coapret_t std_object_read_handler(st_context_t *contextP, cis_iid_t instanceId, int *numDataP, st_data_t **dataArrayP, st_object_t *objectP)
{
    int i = 0;

    for (i = 0; i < sizeof(std_object_callback_mapping) / sizeof(struct st_std_object_callback_mapping); i++) {
        if (std_object_callback_mapping[i].onRead == NULL) {
            continue;
        }

        if (objectP->objID == std_object_callback_mapping[i].stdObjectId) {
            std_object_callback_mapping[i].onRead(contextP, instanceId, numDataP, dataArrayP, objectP);
            return COAP_205_CONTENT;
        }
    }

    return COAP_503_SERVICE_UNAVAILABLE;
}

cis_coapret_t std_object_exec_handler(st_context_t *contextP, cis_iid_t instanceId, uint16_t resourceId, uint8_t *buffer, int length, st_object_t *objectP)
{
    int i = 0;

    for (i = 0; i < sizeof(std_object_callback_mapping) / sizeof(struct st_std_object_callback_mapping); i++) {
        if (std_object_callback_mapping[i].onExec == NULL) {
            continue;
        }

        if (objectP->objID == std_object_callback_mapping[i].stdObjectId) {
            std_object_callback_mapping[i].onExec(contextP, instanceId, resourceId, buffer, length, objectP);
            return CIS_COAP_204_CHANGED;
        }
    }

    return COAP_503_SERVICE_UNAVAILABLE;
}

cis_coapret_t std_object_write_handler(st_context_t *contextP, cis_iid_t instanceId, int numData, st_data_t *dataArray, st_object_t *objectP)
{
    int i = 0;

    for (i = 0; i < sizeof(std_object_callback_mapping) / sizeof(struct st_std_object_callback_mapping); i++) {
        if (std_object_callback_mapping[i].onWrite == NULL) {
            continue;
        }

        if (objectP->objID == std_object_callback_mapping[i].stdObjectId) {
            std_object_callback_mapping[i].onWrite(contextP, instanceId, numData, dataArray, objectP);
            return CIS_COAP_204_CHANGED;
        }
    }

    return COAP_503_SERVICE_UNAVAILABLE;
}



cis_coapret_t std_object_writeInstance(st_context_t *contextP, st_uri_t *uriP, st_data_t *dataP)
{
    st_object_t *targetP;
    cis_coapret_t result;

    targetP = NULL;

    if (uriP->objectId == CIS_SECURITY_OBJECT_ID) {
        targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
    }

    if (NULL == targetP) {
        return CIS_COAP_404_NOT_FOUND;
    }

    result = std_object_write_handler(contextP, uriP->instanceId, 1, dataP, targetP);
    return result;
}