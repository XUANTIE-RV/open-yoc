/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_msg_cmd.h>
#include <tee_srv.h>

extern uint32_t __srvdata_end;
extern uint32_t __srvdata_start;

static tee_srv_intf_t *tee_srv_intf;
static const tee_srv_head_t srv_data_head
__attribute__((__used__))                       \
__attribute__((__section__(".srv.data"))) = {   \
    {'C', 'K', 'T', 'S'},    \
    0x31303030, \
    0xFFFFFFFF, \
    0x00000000, \
    0x00000000, \
    0x00000000, \
    0x00000000, \
    0x00000000, \
    0x00000000
};

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

static uint32_t ta_message_id_get(uint32_t recvmsg)
{
    com_msg_opensess *msg_opensess = (com_msg_opensess *)recvmsg;

    if (recvmsg == 0) {
        return 0xFFFFFFFF;
    }

    return msg_opensess->msg_head.msg_id;
}

static uint32_t ta_find_with_uuid(void *uuid)
{
    tee_srv_head_t *srv_head;
    uint32_t ta_num;
    int i;

    srv_head = (tee_srv_head_t *)&__srvdata_start;

    if (!IS_TEE_SRV_VALID(srv_head)) {
        return 1;
    }

    ta_num = ((uint32_t)&__srvdata_end - (uint32_t)&__srvdata_start - sizeof(tee_srv_head_t)) / sizeof(tee_srv_data_t);

    for (i = 0; i < ta_num; i++) {
        if (memcmp(uuid, (void *) & ((*srv_head->srv_data[i].prop)[0].data.uuid), sizeof(tee_uuid)) == 0) {
            tee_srv_intf = (tee_srv_intf_t *)srv_head->srv_data[i].intf;
            return 0;
        }
    }

    return 1;
}

static void ta_opensession(void *msg)
{
    com_msg_opensess *msg_opensess = (com_msg_opensess *)msg;
    uint32_t   paramtypes          = msg_opensess->opr.paramtypes;
    tee_param *params              = msg_opensess->opr.params;
    void      *session_ctx         = msg_opensess->sess_ctx;
    tee_result ret = 0;

    TEE_LOGD("ta_opensession called \n");

    if ((session_ctx == NULL) || (params == NULL) || (msg_opensess == NULL) || (msg == NULL)) {
        msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_opensess->code              = TEE_ERROR_BAD_PARAMETERS;
        return;
    }

    if ((msg_opensess->msg_head.msg_id   != TEE_MSG_OPEN_SESSION) ||
        (msg_opensess->msg_head.msg_type != TEE_COM_TYPE_QUERY)) {
        TEE_LOGE("invalid message\n");
        return;
    }

    if (ta_find_with_uuid((void *)&msg_opensess->uuid_dest)) {
        msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_opensess->code              = TEE_ERROR_SECURITY;
        TEE_LOGE("invalid uuid\n");
        return;
    }

    if (msg_opensess->opr.operation_id != 0) {
        msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_opensess->code              = TEE_ERROR_GENERIC;
        TEE_LOGE("invalid operation\n");
        return;
    }

#if CONFIG_MSG_VERIFY > 0
    uint16_t checksum = crc16((uint8_t *)msg_opensess->opr.params, sizeof(msg_opensess->opr.params));

    if (checksum != msg_opensess->opr.checksum) {
        msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_opensess->code              = TEE_ERROR_BAD_PARAMETERS;
        TEE_LOGE("invalid params\n");
        TEE_LOGE("checksum %x, opr.checksum %x\n", checksum, msg_opensess->opr.checksum);
        TEE_HEX_DUMP("checksum", (uint8_t *)msg_opensess->opr.params, sizeof(msg_opensess->opr.params));
        return;
    }

#endif

    if (tee_srv_intf != NULL) {
        ret = tee_srv_intf->ta_createentrypoint();

        if (ret != TEE_SUCCESS) {
            msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
            msg_opensess->code              = TEE_ERROR_GENERIC;
            TEE_LOGE("ta generic error\n");
            return;
        }

        ret = tee_srv_intf->ta_opensessionentrypoint(paramtypes, params, &session_ctx);
    }

    msg_opensess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
    msg_opensess->opr.operation_id  = 0;
    msg_opensess->code              = ret;

}

static void ta_closesession(void *msg)
{
    com_msg_closesess *msg_closesess = (com_msg_closesess *)msg;

    TEE_LOGD("ta_closesession called \n");

    if (msg_closesess == NULL) {
        msg_closesess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        return;
    }

    if ((msg_closesess->msg_head.msg_id   != TEE_MSG_CLOSE_SESSION) ||
        (msg_closesess->msg_head.msg_type != TEE_COM_TYPE_QUERY)) {
        TEE_LOGE("invalid message\n");
        return;
    }

    if (tee_srv_intf != NULL) {
        tee_srv_intf->ta_closesessionentrypoint((void *)msg_closesess->sess_ctx);
        tee_srv_intf->ta_destroyentrypoint();
        tee_srv_intf = NULL;
    }

    msg_closesess->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
}

static void ta_invokecommand(void *msg)
{
    com_msg_invokecmd *msg_invokecmd = (com_msg_invokecmd *)msg;
    uint32_t   paramtypes            = msg_invokecmd->opr.paramtypes;
    tee_param *params                = msg_invokecmd->opr.params;
    void      *session_ctx           = msg_invokecmd->sess_ctx;
    uint32_t   commandid             = msg_invokecmd->commandid;
    tee_result ret = 0;

    TEE_LOGD("ta_invokecommand called \n");

    if ((session_ctx == NULL) || (params == NULL) || (msg == NULL) || (msg_invokecmd == NULL)) {
        msg_invokecmd->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_invokecmd->code              = TEE_ERROR_BAD_PARAMETERS;
        return;
    }

    if ((msg_invokecmd->msg_head.msg_id   != TEE_MSG_INVOKE_COMMAND) ||
        (msg_invokecmd->msg_head.msg_type != TEE_COM_TYPE_QUERY)) {
        TEE_LOGE("invalid message\n");
        return;
    }

    if (msg_invokecmd->opr.operation_id != 0) {
        msg_invokecmd->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_invokecmd->code              = TEE_ERROR_GENERIC;
        TEE_LOGE("invalid operation\n");
        return;
    }

#if CONFIG_MSG_VERIFY > 0
    uint16_t checksum = crc16((uint8_t *)msg_invokecmd->opr.params, sizeof(msg_invokecmd->opr.params));

    if (checksum != msg_invokecmd->opr.checksum) {
        msg_invokecmd->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
        msg_invokecmd->code              = TEE_ERROR_BAD_PARAMETERS;
        TEE_LOGE("invalid params\n");
        TEE_LOGE("checksum %x, opr.checksum %x\n", checksum, msg_invokecmd->opr.checksum);
        TEE_HEX_DUMP("checksum", (uint8_t *)msg_invokecmd->opr.params, sizeof(msg_invokecmd->opr.params));
        return;
    }

#endif

    if (tee_srv_intf != NULL) {
        ret = tee_srv_intf->ta_invokecommandentrypoint(session_ctx, commandid, paramtypes, params);
    }

    msg_invokecmd->msg_head.msg_type = TEE_COM_TYPE_RESPONSE;
    msg_invokecmd->opr.operation_id  = 0;
    msg_invokecmd->code              = ret;
}

void process_ta_command(uint32_t recv_msg)
{
    uint32_t msgid;

    msgid = ta_message_id_get(recv_msg);
    TEE_LOGI("TEE get msg id: %d\n", msgid);

    switch (msgid) {
        case TEE_MSG_OPEN_SESSION:
            ta_opensession((void *)recv_msg);
            break;

        case TEE_MSG_CLOSE_SESSION:
            ta_closesession((void *)recv_msg);
            break;

        case TEE_MSG_INVOKE_COMMAND:
            ta_invokecommand((void *)recv_msg);
            break;

        default:
            break;
    }
}

