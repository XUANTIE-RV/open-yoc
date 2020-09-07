/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tee_client_api.h"
#include "tee_msg_cmd.h"
#include <tee_debug.h>

extern __attribute__((naked)) void tee_wsl(uint32_t send_msg);

static context_internel ctx_internal = {0x0000000000000000, CONTEXT_INIT_FLAG};
static session_internel sess_internal = {0x0000000000000000};

#undef tee_malloc
#define tee_malloc  malloc

#undef tee_free
#define tee_free    free


#if CONFIG_MSG_VERIFY > 0
static uint16_t crc16(const uint8_t *src, size_t len)
{
    uint16_t crc = 0xffff;
    size_t padding = sizeof(crc);
    size_t i, b;

    /* src length + padding (if required) */
    for (i = 0; i < len + padding; i++) {

        for (b = 0; b < 8; b++) {
            uint16_t divide = crc & 0x8000;

            crc = (crc << 1);

            /* choose input bytes or implicit trailing zeros */
            if (i < len) {
                crc |= !!(src[i] & (0x80 >> b));
            }

            if (divide) {
                crc = crc ^ 0x1012;
            }
        }
    }

    return crc;
}
#endif

teec_result teec_initializecontext(const char  *name,
                                   teec_context *context)
{
    com_msg_initctx  msg_initctx;
    teec_result      ret;

    if ((context == NULL) || (((context_internel *)(context->imp))->context_status != CONTEXT_INIT_FLAG)) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    memset(&ctx_internal, 0, sizeof(context_internel));

    memset(&msg_initctx,  0, sizeof(com_msg_initctx));
    msg_initctx.msg_head.msg_id     = TEE_MSG_INIT_CONTEXT;
    msg_initctx.msg_head.msg_type   = TEE_COM_TYPE_QUERY;
    msg_initctx.msg_head.session_id = 0;

    tee_wsl((uint32_t)&msg_initctx);

    if ((msg_initctx.msg_head.msg_id   != TEE_MSG_INIT_CONTEXT) ||
        (msg_initctx.msg_head.msg_type != TEE_COM_TYPE_RESPONSE)) {
        return TEEC_ERROR_COMMUNICATION;
    }

    ret = msg_initctx.code;

    if (ret == TEEC_SUCCESS) {
        ctx_internal.operation_id   = msg_initctx.operation_id;
        ctx_internal.context_status = CONTEXT_INIT_FLAG;
        context->imp = (void *)&ctx_internal;
    }

    return ret;
}

void teec_finalizecontext(teec_context *context)
{
    com_msg_finalctx msg_finalctx;
    teec_result      ret;

    if ((context == NULL) || (((context_internel *)(context->imp))->context_status != CONTEXT_INIT_FLAG)) {
        return;
    }

    memset(&msg_finalctx, 0, sizeof(com_msg_finalctx));
    msg_finalctx.msg_head.msg_id     = TEE_MSG_FINAL_CONTEXT;
    msg_finalctx.msg_head.msg_type   = TEE_COM_TYPE_RESPONSE;
    msg_finalctx.msg_head.session_id = 0;

    tee_wsl((uint32_t)&msg_finalctx);

    if ((msg_finalctx.msg_head.msg_id   != TEE_MSG_INIT_CONTEXT) ||
        (msg_finalctx.msg_head.msg_type != TEE_COM_TYPE_RESPONSE)) {
        return;
    }

    ret = msg_finalctx.code;

    if (ret == TEEC_SUCCESS) {
        memset(&ctx_internal, 0, sizeof(context_internel));
    }
}

static teec_result create_sharedmem(teec_sharedmemory *sharedmem, uint32_t type)
{
    sharedmem_internel *shm_internel;

    if (sharedmem == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if ((type == TEE_SHAREDMEM_REGISTER) && (sharedmem->buffer != NULL) && (sharedmem->size == 0)) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if ((type == TEE_SHAREDMEM_REGISTER) && (sharedmem->buffer == NULL) && (sharedmem->size != 0)) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if ((type == TEE_SHAREDMEM_ALLOCATE) && (sharedmem->size == 0)) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    shm_internel = (sharedmem_internel *)tee_malloc(sizeof(sharedmem->size));

    if (shm_internel == NULL) {
        return TEEC_ERROR_OUT_OF_MEMORY;
    }

    shm_internel->size = sharedmem->size;
    shm_internel->type = type;

    if (type == TEE_SHAREDMEM_ALLOCATE) {
        shm_internel->buffer = (void *)tee_malloc(shm_internel->size);

        if (shm_internel->buffer == NULL) {
            tee_free(shm_internel);
            sharedmem->imp = NULL;
            return TEEC_ERROR_OUT_OF_MEMORY;
        }
    }

    sharedmem->buffer = shm_internel->buffer;
    sharedmem->imp    = shm_internel;

    return TEEC_SUCCESS;
}

teec_result teec_registersharedmemory(teec_sharedmemory *sharedmem)
{
    return create_sharedmem(sharedmem, TEE_SHAREDMEM_REGISTER);
}

teec_result teec_allocatesharedmemory(teec_sharedmemory *sharedmem)
{
    return create_sharedmem(sharedmem, TEE_SHAREDMEM_ALLOCATE);
}

void tee_releasesharedmemory(teec_sharedmemory *sharedmem)
{
    if (sharedmem != NULL) {
        if (sharedmem->buffer) {
            tee_free(sharedmem->buffer);
        }

        if (sharedmem->imp) {
            tee_free(sharedmem->imp);
        }
    }
}

teec_result teec_opensession(teec_session    *session,
                             const teec_uuid *destination,
                             teec_operation  *operation)
{
    com_msg_opensess msg_opensess;
    teec_result ret;

    if (ctx_internal.context_status != CONTEXT_INIT_FLAG) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if (session == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if (((operation != NULL) && (operation->started != 0))) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    memset(&msg_opensess, 0, sizeof(com_msg_opensess));

    if (operation != NULL) {
        operation->imp = (teec_operation_handle *)operation;
    }

    msg_opensess.msg_head.msg_id     = TEE_MSG_OPEN_SESSION;
    msg_opensess.msg_head.msg_type   = TEE_COM_TYPE_QUERY;
    msg_opensess.msg_head.session_id = 0;
    memcpy(&msg_opensess.uuid_dest, destination, sizeof(teec_uuid));

    if (operation != NULL) {
        msg_opensess.opr.operation_id = ctx_internal.operation_id;
        msg_opensess.opr.paramtypes   = operation->paramtypes;
        memcpy(&msg_opensess.opr.params[0], &operation->params[0], sizeof(teec_parameter) * 4);
    }

#if CONFIG_MSG_VERIFY > 0
    msg_opensess.opr.checksum = crc16((uint8_t *)msg_opensess.opr.params, sizeof(teec_parameter) * 4);
#endif

    msg_opensess.sess_ctx = (void *)&sess_internal;

    if (operation != NULL) {
        operation->started = OPERATION_START;
    }

    tee_wsl((uint32_t)&msg_opensess);

    if (operation != NULL) {
        operation->started = 0;
    }

    if ((msg_opensess.msg_head.msg_id != TEE_MSG_OPEN_SESSION) || (msg_opensess.msg_head.msg_type != TEE_COM_TYPE_RESPONSE)) {
        return TEEC_ERROR_COMMUNICATION;
    }

    ret                      = msg_opensess.code;
    session->imp             = (void *)&sess_internal;

    return ret;
}

void teec_closesession(teec_session  *session)
{
    com_msg_closesess msg_closesess;

    if (session == NULL) {
        return;
    }

    msg_closesess.msg_head.msg_id     = TEE_MSG_CLOSE_SESSION;
    msg_closesess.msg_head.msg_type   = TEE_COM_TYPE_QUERY;
    msg_closesess.msg_head.session_id = sess_internal.session_id;

    msg_closesess.sess_ctx            = (void *)&sess_internal;

    tee_wsl((uint32_t)&msg_closesess);
}

teec_result teec_invokecommand(teec_session   *session,
                               uint32_t        commandid,
                               teec_operation *operation)
{
    com_msg_invokecmd msg_invokecmd;
    teec_result ret;

    if (session == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if ((operation == NULL) || (operation && (operation->started != 0))) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if (operation != NULL) {
        operation->imp = (teec_operation_handle *)operation;
    }

    msg_invokecmd.msg_head.msg_id     = TEE_MSG_INVOKE_COMMAND;
    msg_invokecmd.msg_head.msg_type   = TEE_COM_TYPE_QUERY;
    msg_invokecmd.msg_head.session_id = ((session_internel *)session)->session_id;
    msg_invokecmd.commandid           = commandid;
    msg_invokecmd.opr.paramtypes      = operation->paramtypes;
    memcpy(&msg_invokecmd.opr.params[0], &operation->params[0], sizeof(teec_parameter) * 4);
#if CONFIG_MSG_VERIFY > 0
    msg_invokecmd.opr.checksum = crc16((uint8_t *)msg_invokecmd.opr.params, sizeof(teec_parameter) * 4);
#endif
    msg_invokecmd.opr.operation_id    = ctx_internal.operation_id;
    msg_invokecmd.sess_ctx            =  session->imp;

    operation->started = OPERATION_START;
    tee_wsl((uint32_t)&msg_invokecmd);
    operation->started = 0;

    if ((msg_invokecmd.msg_head.msg_id != TEE_MSG_INVOKE_COMMAND) || (msg_invokecmd.msg_head.msg_type != TEE_COM_TYPE_RESPONSE)) {
        return TEEC_ERROR_COMMUNICATION;
    }

    ret = msg_invokecmd.code;

    return ret;
}

