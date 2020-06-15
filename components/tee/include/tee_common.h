/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_COMMON_H
#define TEE_COMMON_H
#include <stdint.h>
#include <string.h>

#define SESSION_ID      0x1234567812345678

#define TEEC_4PARAMS(p,v0,v1,v2,v3,v4,v5,v6,v7) \
    p[0].value.a = (uint32_t)v0; \
    p[0].value.b = (uint32_t)v1; \
    p[1].value.a = (uint32_t)v2; \
    p[1].value.b = (uint32_t)v3; \
    p[2].value.a = (uint32_t)v4; \
    p[2].value.b = (uint32_t)v5; \
    p[3].value.a = (uint32_t)v6; \
    p[3].value.b = (uint32_t)v7;

#define TEEC_1PARAMS(p,v0,v1) \
    p[0].value.a = (uint32_t)v0; \
    p[0].value.b = (uint32_t)v1;\
    p[1].value.a = (uint32_t)0; \
    p[1].value.b = (uint32_t)0; \
    p[2].value.a = (uint32_t)0; \
    p[2].value.b = (uint32_t)0; \
    p[3].value.a = (uint32_t)0; \
    p[3].value.b = (uint32_t)0;


#define TEEC_2PARAMS(p,v0,v1,v2,v3) \
    p[0].value.a = (uint32_t)v0; \
    p[0].value.b = (uint32_t)v1; \
    p[1].value.a = (uint32_t)v2; \
    p[1].value.b = (uint32_t)v3; \
    p[2].value.a = (uint32_t)0; \
    p[2].value.b = (uint32_t)0; \
    p[3].value.a = (uint32_t)0; \
    p[3].value.b = (uint32_t)0;


#define TEE_PARAM_TYPES(t0, t1, t2, t3) \
    ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))
#define TEE_PARAM_TYPE_GET(t, i) (((t) >> (i*4)) & 0xF)

#define TEEC_PARAM_TYPES        TEE_PARAM_TYPES
#define TEEC_PARAM_TYPE_GET     TEE_PARAM_TYPE_GET

typedef uint32_t    tee_result;

typedef struct uuid_t {
    uint32_t    timelow;
    uint16_t    timdmid;
    uint16_t    timehiandversion;
    uint8_t     clockseqandnode[8];
} uuid;

typedef uuid        tee_uuid;

typedef struct com_msg_t {
    uint64_t        session_id;
    uint32_t        msg_id;
    uint32_t        msg_type;
} com_msg;

typedef union tee_param_t {
    struct memref_t {
        void       *buffer;
        size_t      size;
    } memref;

    struct value_t {
        uint32_t    a;
        uint32_t    b;
    } value;
} tee_param;

typedef tee_param   com_msg_param;

typedef struct com_opr_t {
    uint64_t        operation_id;
    uint32_t        paramtypes;
    com_msg_param   params[4];
#if CONFIG_MSG_VERIFY > 0
    uint16_t        checksum;
#endif
} com_opr;

typedef struct com_msg_initctx_t {
    com_msg         msg_head;
    uint64_t        operation_id;
    tee_result      code;
} com_msg_initctx;

typedef struct com_msg_finalctx_t {
    com_msg         msg_head;
    tee_result      code;
} com_msg_finalctx;

typedef struct com_msg_opensess_t {
    com_msg         msg_head;
    com_opr         opr;
    void           *sess_ctx;
    tee_uuid        uuid_dest;
    tee_result      code;
} com_msg_opensess;

typedef struct com_msg_closesess_t {
    com_msg         msg_head;
    void           *sess_ctx;
} com_msg_closesess;

typedef struct com_msg_invokecmd_t {
    com_msg         msg_head;
    com_opr         opr;
    void           *sess_ctx;
    tee_result      code;
    uint32_t        commandid;
} com_msg_invokecmd;

typedef struct session_internel_t {
    uint64_t        session_id;
} session_internel;

#endif

