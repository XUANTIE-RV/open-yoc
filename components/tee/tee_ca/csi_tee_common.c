/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "csi_tee_common.h"

teec_result tee_send(uint32_t cmd_id, teec_parameter params[4], uint32_t params_type)
{
    teec_result    ret;
    teec_session   session;
    teec_operation op;
    const teec_uuid tee_cid_uuid = TEE_CID_SRV_UUID;

    memset(&op, 0, sizeof(teec_operation));
    memset(&session, 0, sizeof(teec_session));
    TEE_ENTRY_LOCK();
    ret = teec_opensession(&session, &tee_cid_uuid, NULL);
    TEE_ENTRY_UNLOCK();

    if (ret != TEEC_SUCCESS) {
        return ret;
    }

    op.paramtypes = params_type;
    memcpy(op.params, params, sizeof(teec_parameter) * 4);

    TEE_ENTRY_LOCK();
    ret = teec_invokecommand(&session, cmd_id, &op);
    TEE_ENTRY_UNLOCK();

    if (ret != TEEC_SUCCESS) {
        return ret;
    }

    TEE_ENTRY_LOCK();
    teec_closesession(&session);
    TEE_ENTRY_UNLOCK();

    return 0;
}

teec_result tee_send_xor(uint32_t cmd_id, teec_parameter params[4], uint32_t params_type)
{
    teec_result    ret;
    teec_session   session;
    teec_operation op;
    const teec_uuid tee_xor_uuid = XOR_TA_UUID;

    memset(&op, 0, sizeof(teec_operation));
    memset(&session, 0, sizeof(teec_session));
    TEE_ENTRY_LOCK();
    ret = teec_opensession(&session, &tee_xor_uuid, NULL);
    TEE_ENTRY_UNLOCK();

    if (ret != TEEC_SUCCESS) {
        return ret;
    }

    op.paramtypes = params_type;
    memcpy(op.params, params, sizeof(teec_parameter) * 4);

    TEE_ENTRY_LOCK();
    ret = teec_invokecommand(&session, cmd_id, &op);
    TEE_ENTRY_UNLOCK();

    if (ret != TEEC_SUCCESS) {
        return ret;
    }

    TEE_ENTRY_LOCK();
    teec_closesession(&session);
    TEE_ENTRY_UNLOCK();

    return 0;
}

