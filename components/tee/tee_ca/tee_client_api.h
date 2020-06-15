/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_CLIENT_API_H
#define TEE_CLIENT_API_H

#include "tee_debug.h"
#include "tee_common.h"
#include "tee_msg_cmd.h"

typedef uint32_t    teec_result;
typedef void        teec_session_handle;
typedef void        teec_sharedmem_handle;
typedef void        teec_context_handle;
typedef void        teec_operation_handle;
typedef uuid        teec_uuid;

#define TEE_CID_SRV_UUID  {0x5445455f, 0x4944, 0x325f, \
        {0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x49, 0x44 }}

#define  XOR_TA_UUID   {0x5445455f, 0x4944, 0x325f, \
        {0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x19, 0x33 }}

#define TEE_NAME_LEN                    8
#define TEE_SHAREDMEM_REGISTER          0
#define TEE_SHAREDMEM_ALLOCATE          1

#define CONTEXT_INIT_FLAG               0xFB83DA36
#define OPERATION_START                 0x38fa84fb
/*
 * configuration settings
 */
#define TEEC_CONFIG_SHAREDMEM_MAX_SIZE  0x00080000

/*
 * return code
 */
#define TEEC_SUCCESS                    0x00000000
#define TEEC_ERROR_GENERIC              0xFFFF0000
#define TEEC_ERROR_ACCESS_DENIED        0xFFFF0001
#define TEEC_ERROR_CANCEL               0xFFFF0002
#define TEEC_ERROR_ACCESS_CONFLICT      0xFFFF0003
#define TEEC_ERROR_EXCESS_DATA          0xFFFF0004
#define TEEC_ERROR_BAD_FORMAT           0xFFFF0005
#define TEEC_ERROR_BAD_PARAMETERS       0xFFFF0006
#define TEEC_ERROR_BAD_STATE            0xFFFF0007
#define TEEC_ERROR_ITEM_NOT_FOUND       0xFFFF0008
#define TEEC_ERROR_NOT_IMPLEMENTED      0xFFFF0009
#define TEEC_ERROR_NOT_SUPPORTED        0xFFFF000A
#define TEEC_ERROR_NO_DATA              0xFFFF000B
#define TEEC_ERROR_OUT_OF_MEMORY        0xFFFF000C
#define TEEC_ERROR_BUSY                 0xFFFF000D
#define TEEC_ERROR_COMMUNICATION        0xFFFF000E
#define TEEC_ERROR_SECURITY             0xFFFF000F
#define TEEC_ERROR_SHORT_BUFFER         0xFFFF0010

/*
 * return code origins
 */
#define TEEC_ORIGIN_API                 0x00000001
#define TEEC_ORIGIN_COMMS               0x00000002
#define TEEC_ORIGIN_TEE                 0x00000003
#define TEEC_ORIGIN_TRUSTED_APP         0x00000004


/*
 * shared memory control
 */
#define TEEC_MEM_INPUT                  0x00000001
#define TEEC_MEM_OUTPUT                 0x00000002


/*
 * parameter types
 */
#define TEEC_NONE                       0x00000000
#define TEEC_VALUE_INPUT                0x00000001
#define TEEC_VALUE_OUTPUT               0x00000002
#define TEEC_VALUE_INOUT                0x00000003
#define TEEC_MEMREF_TEMP_INPUT          0x00000005
#define TEEC_MEMREF_TEMP_OUTPUT         0x00000006
#define TEEC_MEMREF_TEMP_INOUT          0x00000007


typedef struct teec_sharedmemory_t {
    void                           *buffer;
    size_t                          size;
    uint32_t                        flags;
    teec_sharedmem_handle          *imp;
} teec_sharedmemory;

typedef struct teec_tempmemoryreference_t {
    void                           *buffer;
    size_t                          size;
} teec_tempmemoryreference;

typedef struct teec_registeredmemoryreference_t {
    teec_sharedmemory              *parent;
    size_t                          size;
    size_t                          offset;
} teec_registeredmemoryreference;

typedef struct teec_value_t {
    uint32_t                        a;
    uint32_t                        b;
} teec_value;

typedef union teec_parameter_t {
    teec_tempmemoryreference        tmpref;
    // teec_registeredmemoryreference  memref;
    teec_value                      value;
} teec_parameter;

typedef struct teec_operation_t {
    uint32_t                        started;
    uint32_t                        paramtypes;
    teec_parameter                  params[4];
    teec_operation_handle          *imp;
} teec_operation;

typedef struct teec_session_t {
    teec_session_handle            *imp;
} teec_session;

typedef struct teec_context_t {
    teec_context_handle            *imp;
} teec_context;

typedef struct context_internel_t {
    uint64_t        operation_id;
    uint32_t        context_status;
} context_internel;

typedef struct sharedmem_internel_t {
    void           *buffer;
    size_t          size;
    uint32_t        type;
} sharedmem_internel;


teec_result teec_initializecontext(const char   *name,
                                   teec_context *context);

void teec_finalizecontext(teec_context *context);

teec_result teec_registersharedmemory(teec_sharedmemory *sharedmem);

teec_result teec_allocatesharedmemory(teec_sharedmemory *sharedmem);

void tee_releasesharedmemory(teec_sharedmemory *sharedmem);

teec_result teec_opensession(teec_session    *session,
                             const teec_uuid *destination,
                             teec_operation  *operation);

void teec_closesession(teec_session  *session);

teec_result teec_invokecommand(teec_session   *session,
                               uint32_t        commandid,
                               teec_operation *operation);

#endif

