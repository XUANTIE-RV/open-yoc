/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_SRV_H
#define TEE_SRV_H
#include <stdbool.h>
#include <tee_common.h>

#define IS_TEE_SRV_VALID(_srv)  (('C' == (_srv)->magic[0]) && \
                                 ('K' == (_srv)->magic[1]) && \
                                 ('T' == (_srv)->magic[2]) && \
                                 ('S' == (_srv)->magic[3]))

typedef struct _tee_srv_intf_t
{
    tee_result (*ta_createentrypoint)(void);
    void (*ta_destroyentrypoint)(void);
    tee_result (*ta_opensessionentrypoint)(uint32_t  paramtypes,
                                           tee_param params[4],
                                           void    **sessioncontext);
    void (*ta_closesessionentrypoint)(void *sessioncontext);
    tee_result (*ta_invokecommandentrypoint)(void     *sessioncontext,
                                             uint32_t  commandid,
                                             uint32_t  paramtypes,
                                             tee_param params[4]);
} tee_srv_intf_t;

typedef enum _tee_srv_prop_type_t
{
    TEE_SRV_PROP_TYPE_INVALID = 0,
    TEE_SRV_PROP_TYPE_STR,
    TEE_SRV_PROP_TYPE_BOOL,
    TEE_SRV_PROP_TYPE_INT,
    TEE_SRV_PROP_TYPE_BINARY,
    TEE_SRV_PROP_TYPE_UUID,
    TEE_SRV_PROP_TYPE_IDENTITY,
    TEE_SRV_PROP_TYPE_MAX,
} tee_srv_prop_type_t;

typedef struct _tee_srv_prop_t
{
    char *name;
    tee_srv_prop_type_t type;
    union
    {
        int8_t *str;
        bool boolean;
        uint32_t u32;
        struct
        {
            void *buf;
            uint32_t size;
        } bin;
        tee_uuid uuid;
    } data;
} tee_srv_prop_t;

typedef struct _tee_srv_data_t
{
    const tee_srv_intf_t    *intf;
    const tee_srv_prop_t    (*prop)[];
} tee_srv_data_t;

typedef struct _tee_srv_head_t
{
    uint8_t         magic[4];
    uint32_t        ver;
    uint32_t        rsvr;
    uint32_t        data_start;
    uint32_t        data_end;
    uint32_t        data_offset;
    uint32_t        bss_start;
    uint32_t        bss_end;
    uint32_t        bss_offset;
    tee_srv_data_t  srv_data[0];
} tee_srv_head_t;

#define TEE_SRV_DATA_START(create, destroy, open_ss, close_ss, invoke_cmd)  \
    static const tee_srv_intf_t _g_$$intf$$ __attribute__((__used__)) = {   \
            create, destroy, open_ss, close_ss, invoke_cmd                  \
    };                                                                      \
    static const tee_srv_prop_t _g_$$prop$$[] __attribute__((__used__)) = {

#define TEE_SRV_UUID_PROPERTY(n, val)       \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_UUID, \
            .data.uuid = val,               \
        },

#define TEE_SRV_STR_PROPERTY(n, val)        \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_STR,  \
            .data.str = val,                \
        },

#define TEE_SRV_INT_PROPERTY(n, val)        \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_INT,  \
            .data.u32 = val,                \
        },

#define TEE_SRV_BOOL_PROPERTY(n, val)       \
        {                                   \
            .name = n,                      \
            .type = TEE_SRV_PROP_TYPE_BOOL, \
            .data.boolean = val,            \
        },

#define TEE_SRV_DATA_END                                \
        {                                               \
            .name = NULL,                               \
            .type = TEE_SRV_PROP_TYPE_INVALID,          \
            .data.u32 = 0,                              \
        },                                              \
    };                                                  \
                                                        \
volatile static const tee_srv_data_t _g_$$srv$$_$$data$$      \
        __attribute__((__used__))                       \
        __attribute__((__section__(".srv.data"))) =     \
        { &_g_$$intf$$, &_g_$$prop$$ };

#endif
